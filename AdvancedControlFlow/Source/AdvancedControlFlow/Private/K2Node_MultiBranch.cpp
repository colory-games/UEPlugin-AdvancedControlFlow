/*!
 * AdvancedControlFlow
 *
 * Copyright (c) 2022-2023 Colory Games
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "K2Node_MultiBranch.h"

#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "EdGraphUtilities.h"
#include "EditorCategoryUtils.h"
#include "GraphEditorSettings.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetCompiledFunctionContext.h"
#include "KismetCompiler.h"
#include "KismetCompilerMisc.h"

#define LOCTEXT_NAMESPACE "AdvancedControlFlow"

class FKCHandler_MultiBranch : public FNodeHandlingFunctor
{
	TMap<UEdGraphNode*, FBPTerminal*> BoolTermMap;

public:
	FKCHandler_MultiBranch(FKismetCompilerContext& InCompilerContext) : FNodeHandlingFunctor(InCompilerContext)
	{
	}

	virtual void RegisterNets(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UK2Node_MultiBranch* MultiBranchNode = CastChecked<UK2Node_MultiBranch>(Node);

		FNodeHandlingFunctor::RegisterNets(Context, Node);

		FBPTerminal* BoolTerm = Context.CreateLocalTerminal();
		BoolTerm->Type.PinCategory = UEdGraphSchema_K2::PC_Boolean;
		BoolTerm->Source = Node;
		BoolTerm->Name = Context.NetNameMap->MakeValidName(Node, TEXT("Inverted"));
		BoolTermMap.Add(Node, BoolTerm);
	}

	virtual void Compile(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UK2Node_MultiBranch* MultiBranchNode = CastChecked<UK2Node_MultiBranch>(Node);

		FEdGraphPinType ExpectedExecPinType;
		ExpectedExecPinType.PinCategory = UEdGraphSchema_K2::PC_Exec;

		{
			UEdGraphPin* ExecTriggeringPin =
				Context.FindRequiredPinByName(MultiBranchNode, UEdGraphSchema_K2::PN_Execute, EGPD_Input);
			if ((ExecTriggeringPin == nullptr) || !Context.ValidatePinType(ExecTriggeringPin, ExpectedExecPinType))
			{
				CompilerContext.MessageLog.Error(
					*LOCTEXT("NoValidExecutionPinForMultiBranch_Error", "@@ must have a valid execution pin @@").ToString(),
					MultiBranchNode, ExecTriggeringPin);
				return;
			}
			else if (ExecTriggeringPin->LinkedTo.Num() == 0)
			{
				CompilerContext.MessageLog.Warning(
					*LOCTEXT("NodeNeverExecuted_Warning", "@@ will never be executed").ToString(), MultiBranchNode);
				return;
			}
		}

		UEdGraphPin* DefaultExecPin = MultiBranchNode->GetDefaultExecPin();

		UEdGraphPin* FunctionPin = MultiBranchNode->GetFunctionPin();
		FBPTerminal* FunctionContext = Context.NetMap.FindRef(FunctionPin);
		UClass* FunctionClass = Cast<UClass>(FunctionPin->PinType.PinSubCategoryObject.Get());
		UFunction* FunctionPtr = FindUField<UFunction>(FunctionClass, FunctionPin->PinName);
		check(FunctionPtr);

		FBPTerminal* BoolTerm = BoolTermMap.FindRef(MultiBranchNode);

		for (auto PinIt = MultiBranchNode->Pins.CreateIterator(); PinIt; ++PinIt)
		{
			UEdGraphPin* ExecPin = *PinIt;

			if (ExecPin->Direction != EGPD_Output)
			{
				continue;
			}
			if (ExecPin->GetFName() == DefaultExecPinName)
			{
				continue;
			}

			UEdGraphPin* CondPin = MultiBranchNode->GetCaseKeyPinFromCaseValuePin(ExecPin);
			UEdGraphPin* CondNet = FEdGraphUtilities::GetNetFromPin(CondPin);
			FBPTerminal* CondValueTerm = Context.NetMap.FindRef(CondNet);

			// Goto if Not_PreBool(Cond)
			{
				FBlueprintCompiledStatement& CallFuncStatement = Context.AppendStatementForNode(MultiBranchNode);
				CallFuncStatement.Type = KCST_CallFunction;
				CallFuncStatement.FunctionToCall = FunctionPtr;
				CallFuncStatement.FunctionContext = FunctionContext;
				CallFuncStatement.bIsParentContext = false;
				CallFuncStatement.LHS = BoolTerm;
				CallFuncStatement.RHS.Add(CondValueTerm);

				FBlueprintCompiledStatement& GotoStatement = Context.AppendStatementForNode(MultiBranchNode);
				GotoStatement.Type = KCST_GotoIfNot;
				GotoStatement.LHS = BoolTerm;

				Context.GotoFixupRequestMap.Add(&GotoStatement, ExecPin);
			}
		}

		// Goto default
		GenerateSimpleThenGoto(Context, *MultiBranchNode, DefaultExecPin);
	}
};

UK2Node_MultiBranch::UK2Node_MultiBranch(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ConditionPreProcessFuncClass = UKismetMathLibrary::StaticClass();
	ConditionPreProcessFuncName = TEXT("Not_PreBool");
	NodeContextMenuSectionName = "K2NodeMultiBranch";
	NodeContextMenuSectionLabel = LOCTEXT("MultiBranch", "MultiBranch");
	CaseKeyPinNamePrefix = TEXT("CaseCond");
	CaseValuePinNamePrefix = TEXT("CaseExec");
	CaseKeyPinFriendlyNamePrefix = TEXT("Condition ");
	CaseValuePinFriendlyNamePrefix = TEXT(" ");
}

void UK2Node_MultiBranch::AllocateDefaultPins()
{
	// Pin structure
	//   N: Number of case pin pair
	// -----
	// 0: Internal function (Hidden, Object)
	// 1: Execution Triggering (In, Exec)
	// 2: Default Execution (Out, Exec)
	// 3 - 2+N: Case Conditional (In, Boolean)
	// 2+N+1 - 2*(N+1): Case Execution (Out, Exec)

	CreateFunctionPin();
	CreateExecTriggeringPin();
	CreateDefaultExecPin();

	Super::AllocateDefaultPins();
}

FText UK2Node_MultiBranch::GetTooltipText() const
{
	return LOCTEXT("MultiBranchStatement_Tooltip", "Multi-Branch Statement\nExecution goes where condition is true");
}

FLinearColor UK2Node_MultiBranch::GetNodeTitleColor() const
{
	return GetDefault<UGraphEditorSettings>()->ExecBranchNodeTitleColor;
}

FText UK2Node_MultiBranch::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("MultiBranch", "Multi-Branch");
}

FSlateIcon UK2Node_MultiBranch::GetIconAndTint(FLinearColor& OutColor) const
{
	static FSlateIcon Icon("EditorStyle", "GraphEditor.Switch_16x");
	return Icon;
}

void UK2Node_MultiBranch::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	CreateFunctionPin();
	CreateExecTriggeringPin();
	CreateDefaultExecPin();

	Super::ReallocatePinsDuringReconstruction(OldPins);
}

class FNodeHandlingFunctor* UK2Node_MultiBranch::CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const
{
	return new FKCHandler_MultiBranch(CompilerContext);
}

void UK2Node_MultiBranch::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_MultiBranch::GetMenuCategory() const
{
	return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::FlowControl);
}

CasePinPair UK2Node_MultiBranch::AddCasePinPair(int32 CaseIndex)
{
	CasePinPair Pair;
	int N = GetCasePinCount();

	{
		FCreatePinParams Params;
		Params.Index = 3 + CaseIndex;
		Pair.Key = CreatePin(
			EGPD_Input, UEdGraphSchema_K2::PC_Boolean, *GetCasePinName(CaseKeyPinNamePrefix.ToString(), CaseIndex), Params);
		Pair.Key->PinFriendlyName =
			FText::AsCultureInvariant(GetCasePinFriendlyName(CaseKeyPinFriendlyNamePrefix.ToString(), CaseIndex));
	}
	{
		FCreatePinParams Params;
		Params.Index = 3 + N + 1 + CaseIndex;
		Pair.Value = CreatePin(
			EGPD_Output, UEdGraphSchema_K2::PC_Exec, *GetCasePinName(CaseValuePinNamePrefix.ToString(), CaseIndex), Params);
		Pair.Value->PinFriendlyName =
			FText::AsCultureInvariant(GetCasePinFriendlyName(CaseValuePinFriendlyNamePrefix.ToString(), CaseIndex));
	}

	return Pair;
}

void UK2Node_MultiBranch::CreateFunctionPin()
{
	FCreatePinParams Params;
	Params.Index = 0;
	UEdGraphPin* FunctionPin =
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, ConditionPreProcessFuncClass, ConditionPreProcessFuncName, Params);
	FunctionPin->bDefaultValueIsReadOnly = true;
	FunctionPin->bNotConnectable = true;
	FunctionPin->bHidden = true;

	UFunction* Function = FindUField<UFunction>(ConditionPreProcessFuncClass, ConditionPreProcessFuncName);
	if (Function != nullptr && Function->HasAllFunctionFlags(FUNC_Static))
	{
		UBlueprint* Blueprint = GetBlueprint();
		if (Blueprint != nullptr)
		{
			UClass* FunctionOwnerClass = Function->GetOuterUClass();
			if (!Blueprint->SkeletonGeneratedClass->IsChildOf(FunctionOwnerClass))
			{
				FunctionPin->DefaultObject = FunctionOwnerClass->GetDefaultObject();
			}
		}
	}
}

void UK2Node_MultiBranch::CreateExecTriggeringPin()
{
	FCreatePinParams Params;
	Params.Index = 1;
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute, Params);
}

void UK2Node_MultiBranch::CreateDefaultExecPin()
{
	FCreatePinParams Params;
	Params.Index = 2;
	UEdGraphPin* DefaultExecPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, DefaultExecPinName, Params);
	DefaultExecPin->PinFriendlyName = FText::AsCultureInvariant(DefaultExecPinFriendlyName.ToString());
}

UEdGraphPin* UK2Node_MultiBranch::GetDefaultExecPin() const
{
	return FindPin(DefaultExecPinName);
}

UEdGraphPin* UK2Node_MultiBranch::GetFunctionPin() const
{
	return FindPin(ConditionPreProcessFuncName);
}

#undef LOCTEXT_NAMESPACE
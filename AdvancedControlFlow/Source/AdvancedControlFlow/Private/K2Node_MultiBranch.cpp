/*!
 * AdvancedControlFlow
 *
 * Copyright (c) 2022 Colory Games
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

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

#define LOCTEXT_NAMESPACE "K2Node"

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
			if (ExecPin->LinkedTo.Num() == 0)
			{
				continue;
			}

			UEdGraphPin* CondPin = MultiBranchNode->GetCondPinFromExecPin(ExecPin);
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
}

void UK2Node_MultiBranch::AllocateDefaultPins()
{
	// Pin structure
	//   N: Number of case pin pair
	// -----
	// 0: Execution Triggering (In, Exec)
	// 1: Default Execution (Out, Exec)
	// 2: Internal function (Hidden, Object)
	// 3 - 2+N: Case Conditional (In, Boolean)
	// 2+N+1 - 2*(N+1): Case Execution (Out, Exec)

	CreateExecTriggeringPin();

	CreateDefaultExecPin();

	CreateFunctionPin();
}

FText UK2Node_MultiBranch::GetTooltipText() const
{
	return LOCTEXT("MultiBranchStatement_Tooltip", "Multi Branch Statement\nExecution goes where condition is true");
}

FLinearColor UK2Node_MultiBranch::GetNodeTitleColor() const
{
	return GetDefault<UGraphEditorSettings>()->ExecBranchNodeTitleColor;
}

FText UK2Node_MultiBranch::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("MultiBranch", "MultiBranch");
}

FSlateIcon UK2Node_MultiBranch::GetIconAndTint(FLinearColor& OutColor) const
{
	static FSlateIcon Icon("EditorStyle", "GraphEditor.Switch_16x");
	return Icon;
}

void UK2Node_MultiBranch::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	CreateExecTriggeringPin();

	CreateDefaultExecPin();

	CreateFunctionPin();

	int32 CasePinCount = 0;
	for (auto& Pin : OldPins)
	{
		if (UEdGraphSchema_K2::IsExecPin(*Pin) && (Pin->Direction == EGPD_Output) && IsCasePin(Pin))
		{
			++CasePinCount;
		}
	}

	for (int32 Index = 0; Index < CasePinCount; ++Index)
	{
		AddCasePinPair(Index);
	}
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

UEdGraphPin* UK2Node_MultiBranch::GetFunctionPin() const
{
	return FindPin(ConditionPreProcessFuncName);
}

void UK2Node_MultiBranch::CreateFunctionPin()
{
	FCreatePinParams Params;
	Params.Index = 2;
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

#undef LOCTEXT_NAMESPACE
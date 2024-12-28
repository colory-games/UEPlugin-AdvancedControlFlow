/*!
 * AdvancedControlFlow
 *
 * Copyright (c) 2022-2023 Colory Games
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "K2Node_ConditionalSequence.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "K2Node_ExecutionSequence.h"
#include "K2Node_IfThenElse.h"
#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "AdvancedControlFlow"

UK2Node_ConditionalSequence::UK2Node_ConditionalSequence(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeContextMenuSectionName = "K2NodeConditionalSequence";
	NodeContextMenuSectionLabel = LOCTEXT("ConditionalSequence", "Conditional Sequence");
	CaseKeyPinNamePrefix = TEXT("CaseCond");
	CaseValuePinNamePrefix = TEXT("CaseExec");
	CaseKeyPinFriendlyNamePrefix = TEXT("Condition ");
	CaseValuePinFriendlyNamePrefix = TEXT(" ");
}

void UK2Node_ConditionalSequence::AllocateDefaultPins()
{
	// Pin structure
	//   N: Number of case pin pair
	// -----
	// 0: Execution Triggering (In, Exec)
	// 1: Default Execution (Out, Exec)
	// 2 - 1+N: Case Conditional (In, Boolean)
	// 1+N+1 - 2*(N+1)-1: Case Execution (Out, Exec)

	CreateExecTriggeringPin();
	CreateDefaultExecPin();

	Super::AllocateDefaultPins();
}

FText UK2Node_ConditionalSequence::GetTooltipText() const
{
	return LOCTEXT(
		"ConditionalSequence_Tooltip", "Conditional Sequence\nExecutes a series of pins in order which meets the condition");
}

FLinearColor UK2Node_ConditionalSequence::GetNodeTitleColor() const
{
	return FLinearColor::White;
}

FText UK2Node_ConditionalSequence::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("ConditionalSequence", "Conditional Sequence");
}

FSlateIcon UK2Node_ConditionalSequence::GetIconAndTint(FLinearColor& OutColor) const
{
	static FSlateIcon Icon("EditorStyle", "GraphEditor.Sequence_16x");
	return Icon;
}

void UK2Node_ConditionalSequence::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	CreateExecTriggeringPin();
	CreateDefaultExecPin();

	Super::ReallocatePinsDuringReconstruction(OldPins);
}

void UK2Node_ConditionalSequence::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	TArray<CasePinPair> CasePairs = GetCasePinPairs();

	UEdGraphPin* ExecTriggeringPin = GetExecPin();
	UEdGraphPin* DefaultExecPin = FindPin(DefaultExecPinName);

	{
		UK2Node_ExecutionSequence* Sequence = CompilerContext.SpawnIntermediateNode<UK2Node_ExecutionSequence>(this, SourceGraph);
		Sequence->AllocateDefaultPins();

		CompilerContext.MovePinLinksToIntermediate(*ExecTriggeringPin, *Sequence->GetExecPin());

		for (int32 Index = 0; Index < CasePairs.Num(); ++Index)
		{
			UK2Node_IfThenElse* IfThenElse = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(this, SourceGraph);
			IfThenElse->AllocateDefaultPins();

			Sequence->AddInputPin();

			UEdGraphPin* CaseCondPin = CasePairs[Index].Key;
			UEdGraphPin* CaseExecPin = CasePairs[Index].Value;
			UEdGraphPin* SequenceExecPin = Sequence->GetThenPinGivenIndex(Index);
			UEdGraphPin* IfThenElseExecPin = IfThenElse->GetExecPin();
			UEdGraphPin* IfThenElseThenPin = IfThenElse->GetThenPin();
			UEdGraphPin* IfThenElseCondPin = IfThenElse->GetConditionPin();

			SequenceExecPin->MakeLinkTo(IfThenElseExecPin);
			CompilerContext.MovePinLinksToIntermediate(*CaseExecPin, *IfThenElseThenPin);
			CompilerContext.MovePinLinksToIntermediate(*CaseCondPin, *IfThenElseCondPin);
		}

		CompilerContext.MovePinLinksToIntermediate(*DefaultExecPin, *Sequence->GetThenPinGivenIndex(CasePairs.Num()));
	}

	BreakAllNodeLinks();
}

void UK2Node_ConditionalSequence::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_ConditionalSequence::GetMenuCategory() const
{
	return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::FlowControl);
}

void UK2Node_ConditionalSequence::CreateExecTriggeringPin()
{
	FCreatePinParams Params;
	Params.Index = 0;
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute, Params);
}

void UK2Node_ConditionalSequence::CreateDefaultExecPin()
{
	FCreatePinParams Params;
	Params.Index = 1;
	UEdGraphPin* DefaultExecPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, DefaultExecPinName, Params);
	DefaultExecPin->PinFriendlyName = FText::AsCultureInvariant(DefaultExecPinFriendlyName.ToString());
}

CasePinPair UK2Node_ConditionalSequence::AddCasePinPair(int32 CaseIndex)
{
	CasePinPair Pair;
	int N = GetCasePinCount();

	{
		FCreatePinParams Params;
		Params.Index = 2 + CaseIndex;
		Pair.Key = CreatePin(
			EGPD_Input, UEdGraphSchema_K2::PC_Boolean, *GetCasePinName(CaseKeyPinNamePrefix.ToString(), CaseIndex), Params);
		Pair.Key->PinFriendlyName =
			FText::AsCultureInvariant(GetCasePinFriendlyName(CaseKeyPinFriendlyNamePrefix.ToString(), CaseIndex));
	}
	{
		FCreatePinParams Params;
		Params.Index = 2 + N + 1 + CaseIndex;
		Pair.Value = CreatePin(
			EGPD_Output, UEdGraphSchema_K2::PC_Exec, *GetCasePinName(CaseValuePinNamePrefix.ToString(), CaseIndex), Params);
		Pair.Value->PinFriendlyName =
			FText::AsCultureInvariant(GetCasePinFriendlyName(CaseValuePinFriendlyNamePrefix.ToString(), CaseIndex));
	}

	return Pair;
}

UEdGraphPin* UK2Node_ConditionalSequence::GetDefaultExecPin() const
{
	return FindPin(DefaultExecPinName);
}

#undef LOCTEXT_NAMESPACE

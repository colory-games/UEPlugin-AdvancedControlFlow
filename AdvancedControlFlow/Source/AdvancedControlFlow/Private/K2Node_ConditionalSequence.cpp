/*!
 * AdvancedControlFlow
 *
 * Copyright (c) 2022 Colory Games
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "K2Node_ConditionalSequence.h"

#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "K2Node_ExecutionSequence.h"
#include "K2Node_IfThenElse.h"
#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "K2Node"

UK2Node_ConditionalSequence::UK2Node_ConditionalSequence(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
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
		CompilerContext.MovePinLinksToIntermediate(*DefaultExecPin, *Sequence->GetThenPinGivenIndex(0));

		for (int32 Index = 0; Index < CasePairs.Num(); ++Index)
		{
			UK2Node_IfThenElse* IfThenElse = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(this, SourceGraph);
			IfThenElse->AllocateDefaultPins();

			Sequence->AddInputPin();

			UEdGraphPin* CaseCondPin = CasePairs[Index].Key;
			UEdGraphPin* CaseExecPin = CasePairs[Index].Value;
			UEdGraphPin* SequenceExecPin = Sequence->GetThenPinGivenIndex(Index + 1);
			UEdGraphPin* IfThenElseExecPin = IfThenElse->GetExecPin();
			UEdGraphPin* IfThenElseThenPin = IfThenElse->GetThenPin();
			UEdGraphPin* IfThenElseCondPin = IfThenElse->GetConditionPin();

			SequenceExecPin->MakeLinkTo(IfThenElseExecPin);
			CompilerContext.MovePinLinksToIntermediate(*CaseExecPin, *IfThenElseThenPin);
			CompilerContext.MovePinLinksToIntermediate(*CaseCondPin, *IfThenElseCondPin);
		}
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

#undef LOCTEXT_NAMESPACE

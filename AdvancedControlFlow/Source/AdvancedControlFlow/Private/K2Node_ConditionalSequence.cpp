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
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetCompiler.h"
#include "ToolMenu.h"

#define LOCTEXT_NAMESPACE "K2Node"

namespace ConditionalSequence
{
static const FName DefaultExecPinName(TEXT("DefaultExec"));
static const FName CaseExecPinNamePrefix(TEXT("CaseExec"));
static const FName CaseCondPinNamePrefix(TEXT("CaseCond"));
static const FName DefaultExecPinFriendlyName(TEXT("Default"));
static const FName CaseExecPinFriendlyNamePrefix(TEXT("Case"));
static const FName CaseCondPinFriendlyNamePrefix(TEXT("Case"));
}	 // namespace ConditionalSequence

using namespace ConditionalSequence;

UK2Node_ConditionalSequence::UK2Node_ConditionalSequence(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
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

void UK2Node_ConditionalSequence::GetNodeContextMenuActions(
	class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const
{
	Super::GetNodeContextMenuActions(Menu, Context);

	if (!Context->bIsDebugging)
	{
		FToolMenuSection& Section = Menu->AddSection(
			"K2NodeConditionalSequence", NSLOCTEXT("K2Nodes", "ConditionalSequenceHeader", "Conditional Sequence"));

		if (Context->Node->Pins.Num() >= 1)
		{
			Section.AddMenuEntry("AddCasePinBefore", LOCTEXT("AddCasePinBefore", "Add case pin before"),
				LOCTEXT("AddCasePinBeforeTooltip", "Add case pin before this pin on this node"), FSlateIcon(),
				FUIAction(FExecuteAction::CreateUObject(const_cast<UK2Node_ConditionalSequence*>(this),
					&UK2Node_ConditionalSequence::AddCasePinBefore, const_cast<UEdGraphPin*>(Context->Pin))));
			Section.AddMenuEntry("AddCasePinAfter", LOCTEXT("AddCasePinAfter", "Add case pin after"),
				LOCTEXT("AddCasePinAfterTooltip", "Add case pin after this pin on this node"), FSlateIcon(),
				FUIAction(FExecuteAction::CreateUObject(const_cast<UK2Node_ConditionalSequence*>(this),
					&UK2Node_ConditionalSequence::AddCasePinAfter, const_cast<UEdGraphPin*>(Context->Pin))));
			Section.AddMenuEntry("RemoveFirstCasePin", LOCTEXT("RemoveFirstCasePin", "Remove first case pin"),
				LOCTEXT("RemoveFirstCasePinTooltip", "Remove first case pin on this node"), FSlateIcon(),
				FUIAction(FExecuteAction::CreateUObject(
					const_cast<UK2Node_ConditionalSequence*>(this), &UK2Node_ConditionalSequence::RemoveFirstCasePin)));
			Section.AddMenuEntry("RemoveLastCasePin", LOCTEXT("RemoveLastCasePin", "Remove last case pin"),
				LOCTEXT("RemoveLastCasePinTooltip", "Remove last case pin on this node"), FSlateIcon(),
				FUIAction(FExecuteAction::CreateUObject(
					const_cast<UK2Node_ConditionalSequence*>(this), &UK2Node_ConditionalSequence::RemoveLastCasePin)));
		}

		if (Context->Pin != nullptr)
		{
			Section.AddMenuEntry("RemoveThisCasePin", LOCTEXT("RemoveThisCasePin", "Remove this case pin"),
				LOCTEXT("RemoveThisCasePinTooltip", "Remove this case pin on this node"), FSlateIcon(),
				FUIAction(FExecuteAction::CreateUObject(const_cast<UK2Node_ConditionalSequence*>(this),
					&UK2Node_ConditionalSequence::RemoveInputPin, const_cast<UEdGraphPin*>(Context->Pin))));
		}
	}
}

void UK2Node_ConditionalSequence::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	Super::AllocateDefaultPins();

	CreateExecTriggeringPin();

	CreateDefaultExecPin();

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

void UK2Node_ConditionalSequence::AddInputPin()
{
	Modify();

	int32 N = GetCasePinCount();

	AddCasePinPair(N);
}

void UK2Node_ConditionalSequence::RemoveInputPin(UEdGraphPin* Pin)
{
	if (Pin == nullptr)
	{
		return;
	}

	UK2Node_ConditionalSequence* OwnerNode = Cast<UK2Node_ConditionalSequence>(Pin->GetOwningNode());

	if (OwnerNode)
	{
		Modify();

		int32 CaseIndex = GetCaseIndexFromCasePin(Pin);
		RemoveCasePinAt(CaseIndex);
	}
}

void UK2Node_ConditionalSequence::AddCasePinAfter(UEdGraphPin* Pin)
{
	if (Pin == nullptr)
	{
		return;
	}

	UK2Node_ConditionalSequence* OwnerNode = Cast<UK2Node_ConditionalSequence>(Pin->GetOwningNode());

	if (OwnerNode)
	{
		Modify();

		CasePinPair Pair = GetCasePinPair(Pin);

		UEdGraphPin* CaseCondAfterPin = Pair.Key;
		UEdGraphPin* CaseExecAfterPin = Pair.Value;
		int32 CaseIndexAfter = GetCaseIndexFromCaseCondPin(CaseCondAfterPin);
		check(CaseIndexAfter == GetCaseIndexFromCaseExecPin(CaseExecAfterPin));

		// Get current cond-exec pin pair.
		TArray<CasePinPair> CasePairs = GetCasePinPairs();

		// Add new pin pair.
		AddCasePinPair(CaseIndexAfter + 1);

		// Restore cond-exec pin pair name.
		for (int32 Index = CaseIndexAfter + 1; Index < CasePairs.Num(); ++Index)
		{
			UEdGraphPin* CaseCondPin = CasePairs[Index].Key;
			UEdGraphPin* CaseExecPin = CasePairs[Index].Value;

			CaseExecPin->PinName = *GetCasePinName(CaseExecPinNamePrefix.ToString(), Index + 1);
			CaseExecPin->PinFriendlyName =
				FText::AsCultureInvariant(GetCasePinFriendlyName(CaseExecPinFriendlyNamePrefix.ToString(), Index + 1));
			CaseCondPin->PinName = *GetCasePinName(CaseCondPinNamePrefix.ToString(), Index + 1);
			CaseCondPin->PinFriendlyName =
				FText::AsCultureInvariant(GetCasePinFriendlyName(CaseCondPinFriendlyNamePrefix.ToString(), Index + 1));
		}

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
	}
}

void UK2Node_ConditionalSequence::AddCasePinBefore(UEdGraphPin* Pin)
{
	if (Pin == nullptr)
	{
		return;
	}

	UK2Node_ConditionalSequence* OwnerNode = Cast<UK2Node_ConditionalSequence>(Pin->GetOwningNode());

	if (OwnerNode)
	{
		Modify();

		CasePinPair Pair = GetCasePinPair(Pin);

		UEdGraphPin* CaseCondBeforePin = Pair.Key;
		UEdGraphPin* CaseExecBeforePin = Pair.Value;
		int32 CaseIndexBefore = GetCaseIndexFromCaseCondPin(CaseCondBeforePin);
		check(CaseIndexBefore == GetCaseIndexFromCaseExecPin(CaseExecBeforePin));

		// Get current cond-exec pin pair.
		TArray<CasePinPair> CasePairs = GetCasePinPairs();

		// Add new pin pair.
		AddCasePinPair(CaseIndexBefore);

		// Restore cond-exec pin pair name.
		for (int32 Index = CaseIndexBefore; Index < CasePairs.Num(); ++Index)
		{
			UEdGraphPin* CaseCondPin = CasePairs[Index].Key;
			UEdGraphPin* CaseExecPin = CasePairs[Index].Value;

			CaseExecPin->PinName = *GetCasePinName(CaseExecPinNamePrefix.ToString(), Index + 1);
			CaseExecPin->PinFriendlyName =
				FText::AsCultureInvariant(GetCasePinFriendlyName(CaseExecPinFriendlyNamePrefix.ToString(), Index + 1));
			CaseCondPin->PinName = *GetCasePinName(CaseCondPinNamePrefix.ToString(), Index + 1);
			CaseCondPin->PinFriendlyName =
				FText::AsCultureInvariant(GetCasePinFriendlyName(CaseCondPinFriendlyNamePrefix.ToString(), Index + 1));
		}

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
	}
}

void UK2Node_ConditionalSequence::RemoveFirstCasePin()
{
	Modify();

	RemoveCasePinAt(0);
}

void UK2Node_ConditionalSequence::RemoveLastCasePin()
{
	Modify();

	RemoveCasePinAt(GetCasePinCount() - 1);
}

UEdGraphPin* UK2Node_ConditionalSequence::GetCaseCondPinFromCaseIndex(int32 CaseIndex) const
{
	for (auto& P : Pins)
	{
		if (!UEdGraphSchema_K2::IsExecPin(*P) && (P->Direction == EGPD_Input) && IsCasePin(P))
		{
			int32 ActualIndex = GetCaseIndexFromCaseCondPin(P);

			if ((ActualIndex != -1) && (ActualIndex == CaseIndex))
			{
				return P;
			}
		}
	}

	return nullptr;
}

UEdGraphPin* UK2Node_ConditionalSequence::GetCaseExecPinFromCaseIndex(int32 CaseIndex) const
{
	for (auto& P : Pins)
	{
		if (UEdGraphSchema_K2::IsExecPin(*P) && (P->Direction == EGPD_Output) && IsCasePin(P))
		{
			int32 ActualIndex = GetCaseIndexFromCaseExecPin(P);

			if ((ActualIndex != -1) && (ActualIndex == CaseIndex))
			{
				return P;
			}
		}
	}

	return nullptr;
}

CasePinPair UK2Node_ConditionalSequence::GetCasePinPair(UEdGraphPin* Pin) const
{
	CasePinPair Pair;

	if (UEdGraphSchema_K2::IsExecPin(*Pin))
	{
		Pair.Key = GetCondPinFromExecPin(Pin);
		Pair.Value = Pin;
		check(Pair.Key);
	}
	else
	{
		Pair.Key = Pin;
		Pair.Value = GetExecPinFromCondPin(Pin);
		check(Pair.Value);
	}

	return Pair;
}

int32 UK2Node_ConditionalSequence::GetCaseIndexFromCasePin(UEdGraphPin* Pin) const
{
	check(IsCasePin(Pin));

	if (UEdGraphSchema_K2::IsExecPin(*Pin))
	{
		return GetCaseIndexFromCaseExecPin(Pin);
	}

	return GetCaseIndexFromCaseCondPin(Pin);
}

int32 UK2Node_ConditionalSequence::GetCaseIndexFromCasePin(const FString& Prefix, UEdGraphPin* Pin) const
{
	check(IsCasePin(Pin));

	FString PinName = Pin->GetFName().ToString();
	FString Dummy;
	FString IndexStr;
	if (!PinName.Split(Prefix + "_", &Dummy, &IndexStr, ESearchCase::CaseSensitive))
	{
		return -1;
	}

	return FCString::Atoi(*IndexStr);
}

int32 UK2Node_ConditionalSequence::GetCaseIndexFromCaseExecPin(UEdGraphPin* Pin) const
{
	check(Pin->Direction == EGPD_Output);
	check(IsCasePin(Pin));
	check(UEdGraphSchema_K2::IsExecPin(*Pin));

	return GetCaseIndexFromCasePin(CaseExecPinNamePrefix.ToString(), Pin);
}

int32 UK2Node_ConditionalSequence::GetCaseIndexFromCaseCondPin(UEdGraphPin* Pin) const
{
	check(Pin->Direction == EGPD_Input);
	check(IsCasePin(Pin));
	check(!UEdGraphSchema_K2::IsExecPin(*Pin));

	return GetCaseIndexFromCasePin(CaseCondPinNamePrefix.ToString(), Pin);
}

void UK2Node_ConditionalSequence::RemoveCasePinAt(int32 CaseIndex)
{
	UEdGraphPin* CaseExecPinToRemove = GetCaseExecPinFromCaseIndex(CaseIndex);
	UEdGraphPin* CaseCondPinToRemove = GetCaseCondPinFromCaseIndex(CaseIndex);
	check(CaseExecPinToRemove);
	check(CaseCondPinToRemove);

	Pins.Remove(CaseExecPinToRemove);
	Pins.Remove(CaseCondPinToRemove);
	CaseExecPinToRemove->MarkAsGarbage();
	CaseCondPinToRemove->MarkAsGarbage();

	int32 Index = 0;
	for (auto& P : Pins)
	{
		if (UEdGraphSchema_K2::IsExecPin(*P) && (P->Direction == EGPD_Output) && IsCasePin(P))
		{
			UEdGraphPin* CaseExecPin = P;
			UEdGraphPin* CaseCondPin = GetCondPinFromExecPin(CaseExecPin);

			CaseExecPin->PinName = *GetCasePinName(CaseExecPinNamePrefix.ToString(), Index);
			CaseExecPin->PinFriendlyName =
				FText::AsCultureInvariant(GetCasePinFriendlyName(CaseExecPinFriendlyNamePrefix.ToString(), Index));
			CaseCondPin->PinName = *GetCasePinName(CaseCondPinNamePrefix.ToString(), Index);
			CaseCondPin->PinFriendlyName =
				FText::AsCultureInvariant(GetCasePinFriendlyName(CaseCondPinFriendlyNamePrefix.ToString(), Index));

			++Index;
		}
	}

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
}

int32 UK2Node_ConditionalSequence::GetCasePinCount() const
{
	// TODO: need to optimize.
	for (int32 Index = Pins.Num() / 2; Index >= 0; --Index)
	{
		UEdGraphPin* CaseCondPinIndex = GetCaseCondPinFromCaseIndex(Index);
		UEdGraphPin* CaseExecPinIndex = GetCaseExecPinFromCaseIndex(Index);

		if ((CaseCondPinIndex != nullptr) && (CaseExecPinIndex != nullptr))
		{
			return Index + 1;
		}
	}

	return 0;
}

TArray<CasePinPair> UK2Node_ConditionalSequence::GetCasePinPairs() const
{
	TArray<CasePinPair> CasePairs;
	CasePairs.SetNum(GetCasePinCount());

	for (auto& P : Pins)
	{
		if (UEdGraphSchema_K2::IsExecPin(*P) && (P->Direction == EGPD_Output) && IsCasePin(P))
		{
			UEdGraphPin* CaseExecPin = P;
			int32 Index = GetCaseIndexFromCaseExecPin(CaseExecPin);
			CasePairs[Index] = GetCasePinPair(CaseExecPin);
		}
	}

	return CasePairs;
}

void UK2Node_ConditionalSequence::AddCasePinAt(int32 Index)
{
}

bool UK2Node_ConditionalSequence::IsCasePin(UEdGraphPin* Pin) const
{
	TArray<FName> NonCasePinName = {
		UEdGraphSchema_K2::PN_Execute,
		DefaultExecPinName,
	};

	for (auto& Name : NonCasePinName)
	{
		if (Pin->GetFName() == Name)
		{
			return false;
		}
	}

	return true;
}

CasePinPair UK2Node_ConditionalSequence::AddCasePinPair(int32 CaseIndex)
{
	CasePinPair Pair;
	int N = GetCasePinCount();

	{
		FCreatePinParams Params;
		Params.Index = 3 + CaseIndex;
		Pair.Key = CreatePin(
			EGPD_Input, UEdGraphSchema_K2::PC_Boolean, *GetCasePinName(CaseCondPinNamePrefix.ToString(), CaseIndex), Params);
		Pair.Key->PinFriendlyName =
			FText::AsCultureInvariant(GetCasePinFriendlyName(CaseCondPinFriendlyNamePrefix.ToString(), CaseIndex));
	}
	{
		FCreatePinParams Params;
		Params.Index = 3 + N + CaseIndex + 1;
		Pair.Value = CreatePin(
			EGPD_Output, UEdGraphSchema_K2::PC_Exec, *GetCasePinName(CaseExecPinNamePrefix.ToString(), CaseIndex), Params);
		Pair.Value->PinFriendlyName =
			FText::AsCultureInvariant(GetCasePinFriendlyName(CaseExecPinFriendlyNamePrefix.ToString(), CaseIndex));
	}

	return Pair;
}

FString UK2Node_ConditionalSequence::GetCasePinName(const FString& Prefix, int32 CaseIndex) const
{
	return FString::Printf(TEXT("%s_%d"), *Prefix, CaseIndex);
}

FString UK2Node_ConditionalSequence::GetCasePinFriendlyName(const FString& Prefix, int32 CaseIndex) const
{
	return FString::Printf(TEXT("%s %d"), *Prefix, CaseIndex);
}

UEdGraphPin* UK2Node_ConditionalSequence::GetDefaultExecPin() const
{
	return FindPin(DefaultExecPinName);
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

UEdGraphPin* UK2Node_ConditionalSequence::GetCondPinFromExecPin(const UEdGraphPin* ExecPin) const
{
	FString ExecPinName = ExecPin->GetFName().ToString();

	FString Dummy;
	FString Suffix;
	if (!ExecPinName.Split(CaseExecPinNamePrefix.ToString(), &Dummy, &Suffix, ESearchCase::CaseSensitive))
	{
		return nullptr;
	}

	return FindPin(CaseCondPinNamePrefix.ToString() + Suffix);
}

UEdGraphPin* UK2Node_ConditionalSequence::GetExecPinFromCondPin(const UEdGraphPin* CondPin) const
{
	FString CondPinName = CondPin->GetFName().ToString();

	FString Dummy;
	FString Suffix;
	if (!CondPinName.Split(CaseCondPinNamePrefix.ToString(), &Dummy, &Suffix, ESearchCase::CaseSensitive))
	{
		return nullptr;
	}

	return FindPin(CaseExecPinNamePrefix.ToString() + Suffix);
}

#undef LOCTEXT_NAMESPACE

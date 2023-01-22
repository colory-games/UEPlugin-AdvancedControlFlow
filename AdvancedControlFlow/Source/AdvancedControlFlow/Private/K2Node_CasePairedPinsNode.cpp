/*!
 * AdvancedControlFlow
 *
 * Copyright (c) 2022-2023 Colory Games
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "K2Node_CasePairedPinsNode.h"

#include "Internationalization/Regex.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "ToolMenu.h"

#define LOCTEXT_NAMESPACE "AdvancedControlFlow"

const FName DefaultExecPinName(TEXT("DefaultExec"));
const FName DefaultExecPinFriendlyName(TEXT("Default"));

UK2Node_CasePairedPinsNode::UK2Node_CasePairedPinsNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UK2Node_CasePairedPinsNode::GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const
{
	Super::GetNodeContextMenuActions(Menu, Context);

	if (!Context->bIsDebugging)
	{
		FToolMenuSection& Section = Menu->AddSection(NodeContextMenuSectionName, NodeContextMenuSectionLabel);

		if (Context->Pin != nullptr && IsCasePin(Context->Pin))
		{
#ifdef ACF_FREE_VERSION
			const UK2Node_CasePairedPinsNode* CasePairedPinsNode = CastChecked<UK2Node_CasePairedPinsNode>(Context->Node);
			if (CasePairedPinsNode->GetCasePinCount() < 3)
			{
#endif
				Section.AddMenuEntry("AddCasePinBefore", LOCTEXT("AddCasePinBefore", "Add case pin before"),
					LOCTEXT("AddCasePinBeforeTooltip", "Add case pin before this pin on this node"), FSlateIcon(),
					FUIAction(FExecuteAction::CreateUObject(const_cast<UK2Node_CasePairedPinsNode*>(this),
						&UK2Node_CasePairedPinsNode::AddCasePinBefore, const_cast<UEdGraphPin*>(Context->Pin))));
				Section.AddMenuEntry("AddCasePinAfter", LOCTEXT("AddCasePinAfter", "Add case pin after"),
					LOCTEXT("AddCasePinAfterTooltip", "Add case pin after this pin on this node"), FSlateIcon(),
					FUIAction(FExecuteAction::CreateUObject(const_cast<UK2Node_CasePairedPinsNode*>(this),
						&UK2Node_CasePairedPinsNode::AddCasePinAfter, const_cast<UEdGraphPin*>(Context->Pin))));
#ifdef ACF_FREE_VERSION
			}
#endif
			Section.AddMenuEntry("RemoveThisCasePin", LOCTEXT("RemoveThisCasePin", "Remove this case pin"),
				LOCTEXT("RemoveThisCasePinTooltip", "Remove this case pin on this node"), FSlateIcon(),
				FUIAction(FExecuteAction::CreateUObject(const_cast<UK2Node_CasePairedPinsNode*>(this),
					&UK2Node_CasePairedPinsNode::RemoveCasePinAt, const_cast<UEdGraphPin*>(Context->Pin))));
		}

		if (Context->Node->Pins.Num() >= 1)
		{
			Section.AddMenuEntry("RemoveFirstCasePin", LOCTEXT("RemoveFirstCasePin", "Remove first case pin"),
				LOCTEXT("RemoveFirstCasePinTooltip", "Remove first case pin on this node"), FSlateIcon(),
				FUIAction(FExecuteAction::CreateUObject(
					const_cast<UK2Node_CasePairedPinsNode*>(this), &UK2Node_CasePairedPinsNode::RemoveFirstCasePin)));
			Section.AddMenuEntry("RemoveLastCasePin", LOCTEXT("RemoveLastCasePin", "Remove last case pin"),
				LOCTEXT("RemoveLastCasePinTooltip", "Remove last case pin on this node"), FSlateIcon(),
				FUIAction(FExecuteAction::CreateUObject(
					const_cast<UK2Node_CasePairedPinsNode*>(this), &UK2Node_CasePairedPinsNode::RemoveLastCasePin)));
		}
	}
}

void UK2Node_CasePairedPinsNode::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	Super::AllocateDefaultPins();

	int32 CasePinCount = 0;
	for (auto& Pin : OldPins)
	{
		if (IsCaseKeyPin(Pin))
		{
			++CasePinCount;
		}
	}

	for (int32 Index = 0; Index < CasePinCount; ++Index)
	{
		AddCasePinPair(Index);
	}
}

void UK2Node_CasePairedPinsNode::AddCasePinAfter(UEdGraphPin* Pin)
{
	if (Pin == nullptr)
	{
		return;
	}

	UK2Node_CasePairedPinsNode* OwnerNode = Cast<UK2Node_CasePairedPinsNode>(Pin->GetOwningNode());

	if (OwnerNode)
	{
		Modify();

		CasePinPair Pair = GetCasePinPair(Pin);

		UEdGraphPin* CaseKeyAfterPin = Pair.Key;
		UEdGraphPin* CaseValueAfterPin = Pair.Value;
		int32 CaseIndexAfter = GetCaseIndexFromCaseKeyPin(CaseKeyAfterPin);
		check(CaseIndexAfter == GetCaseIndexFromCaseValuePin(CaseValueAfterPin));

		// Get current key-value pin pair.
		TArray<CasePinPair> CasePairs = GetCasePinPairs();

		// Add new pin pair.
		AddCasePinPair(CaseIndexAfter + 1);

		// Restore key-value pin pair name.
		for (int32 Index = CaseIndexAfter + 1; Index < CasePairs.Num(); ++Index)
		{
			UEdGraphPin* CaseKeyPin = CasePairs[Index].Key;
			UEdGraphPin* CaseValuePin = CasePairs[Index].Value;

			CaseValuePin->PinName = *GetCasePinName(CaseValuePinNamePrefix.ToString(), Index + 1);
			CaseValuePin->PinFriendlyName =
				FText::AsCultureInvariant(GetCasePinFriendlyName(CaseValuePinFriendlyNamePrefix.ToString(), Index + 1));
			CaseKeyPin->PinName = *GetCasePinName(CaseKeyPinNamePrefix.ToString(), Index + 1);
			CaseKeyPin->PinFriendlyName =
				FText::AsCultureInvariant(GetCasePinFriendlyName(CaseKeyPinFriendlyNamePrefix.ToString(), Index + 1));
		}

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
	}
}

void UK2Node_CasePairedPinsNode::AddCasePinBefore(UEdGraphPin* Pin)
{
	if (Pin == nullptr)
	{
		return;
	}

	UK2Node_CasePairedPinsNode* OwnerNode = Cast<UK2Node_CasePairedPinsNode>(Pin->GetOwningNode());

	if (OwnerNode)
	{
		Modify();

		CasePinPair Pair = GetCasePinPair(Pin);

		UEdGraphPin* CaseKeyBeforePin = Pair.Key;
		UEdGraphPin* CaseValueBeforePin = Pair.Value;
		int32 CaseIndexBefore = GetCaseIndexFromCaseKeyPin(CaseKeyBeforePin);
		check(CaseIndexBefore == GetCaseIndexFromCaseValuePin(CaseValueBeforePin));

		// Get current key-value pin pair.
		TArray<CasePinPair> CasePairs = GetCasePinPairs();

		// Add new pin pair.
		AddCasePinPair(CaseIndexBefore);

		// Restore key-value pin pair name.
		for (int32 Index = CaseIndexBefore; Index < CasePairs.Num(); ++Index)
		{
			UEdGraphPin* CaseKeyPin = CasePairs[Index].Key;
			UEdGraphPin* CaseValuePin = CasePairs[Index].Value;

			CaseValuePin->PinName = *GetCasePinName(CaseValuePinNamePrefix.ToString(), Index + 1);
			CaseValuePin->PinFriendlyName =
				FText::AsCultureInvariant(GetCasePinFriendlyName(CaseValuePinFriendlyNamePrefix.ToString(), Index + 1));
			CaseKeyPin->PinName = *GetCasePinName(CaseKeyPinNamePrefix.ToString(), Index + 1);
			CaseKeyPin->PinFriendlyName =
				FText::AsCultureInvariant(GetCasePinFriendlyName(CaseKeyPinFriendlyNamePrefix.ToString(), Index + 1));
		}

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
	}
}

void UK2Node_CasePairedPinsNode::RemoveCasePinAt(UEdGraphPin* Pin)
{
	if (Pin == nullptr)
	{
		return;
	}

	UK2Node_CasePairedPinsNode* OwnerNode = Cast<UK2Node_CasePairedPinsNode>(Pin->GetOwningNode());

	if (OwnerNode)
	{
		Modify();

		int32 CaseIndex = GetCaseIndexFromCasePin(Pin);
		RemoveCasePinAt(CaseIndex);
	}
}

void UK2Node_CasePairedPinsNode::RemoveFirstCasePin()
{
	Modify();

	RemoveCasePinAt(0);
}

void UK2Node_CasePairedPinsNode::RemoveLastCasePin()
{
	Modify();

	RemoveCasePinAt(GetCasePinCount() - 1);
}

UEdGraphPin* UK2Node_CasePairedPinsNode::GetCaseKeyPinFromCaseIndex(int32 CaseIndex) const
{
	for (auto& P : Pins)
	{
		if (IsCaseKeyPin(P))
		{
			int32 ActualIndex = GetCaseIndexFromCaseKeyPin(P);

			if ((ActualIndex != -1) && (ActualIndex == CaseIndex))
			{
				return P;
			}
		}
	}

	return nullptr;
}

UEdGraphPin* UK2Node_CasePairedPinsNode::GetCaseValuePinFromCaseIndex(int32 CaseIndex) const
{
	for (auto& P : Pins)
	{
		if (IsCaseValuePin(P))
		{
			int32 ActualIndex = GetCaseIndexFromCaseValuePin(P);

			if ((ActualIndex != -1) && (ActualIndex == CaseIndex))
			{
				return P;
			}
		}
	}

	return nullptr;
}

CasePinPair UK2Node_CasePairedPinsNode::GetCasePinPair(UEdGraphPin* Pin) const
{
	CasePinPair Pair;

	if (IsCaseValuePin(Pin))
	{
		Pair.Key = GetCaseKeyPinFromCaseValuePin(Pin);
		Pair.Value = Pin;
		check(Pair.Key);
	}
	else
	{
		check(IsCaseKeyPin(Pin));

		Pair.Key = Pin;
		Pair.Value = GetCaseValuePinFromCaseKeyPin(Pin);
		check(Pair.Value);
	}

	return Pair;
}

int32 UK2Node_CasePairedPinsNode::GetCaseIndexFromCasePin(UEdGraphPin* Pin) const
{
	check(IsCasePin(Pin));

	if (IsCaseValuePin(Pin))
	{
		return GetCaseIndexFromCaseValuePin(Pin);
	}
	check(IsCaseKeyPin(Pin));

	return GetCaseIndexFromCaseKeyPin(Pin);
}

int32 UK2Node_CasePairedPinsNode::GetCaseIndexFromCasePin(const FString& Prefix, UEdGraphPin* Pin) const
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

int32 UK2Node_CasePairedPinsNode::GetCaseIndexFromCaseValuePin(UEdGraphPin* Pin) const
{
	check(IsCaseValuePin(Pin));

	return GetCaseIndexFromCasePin(CaseValuePinNamePrefix.ToString(), Pin);
}

int32 UK2Node_CasePairedPinsNode::GetCaseIndexFromCaseKeyPin(UEdGraphPin* Pin) const
{
	check(IsCaseKeyPin(Pin));

	return GetCaseIndexFromCasePin(CaseKeyPinNamePrefix.ToString(), Pin);
}

void UK2Node_CasePairedPinsNode::RemoveCasePinAt(int32 CaseIndex)
{
	UEdGraphPin* CaseValuePinToRemove = GetCaseValuePinFromCaseIndex(CaseIndex);
	UEdGraphPin* CaseKeyPinToRemove = GetCaseKeyPinFromCaseIndex(CaseIndex);
	check(CaseValuePinToRemove);
	check(CaseKeyPinToRemove);

	Pins.Remove(CaseValuePinToRemove);
	Pins.Remove(CaseKeyPinToRemove);
#if UE_VERSION_OLDER_THAN(5, 0, 0)
	CaseValuePinToRemove->MarkPendingKill();
	CaseKeyPinToRemove->MarkPendingKill();
#else
	CaseValuePinToRemove->MarkAsGarbage();
	CaseKeyPinToRemove->MarkAsGarbage();
#endif

	int32 Index = 0;
	for (auto& P : Pins)
	{
		if (IsCaseValuePin(P))
		{
			UEdGraphPin* CaseValuePin = P;
			UEdGraphPin* CaseKeyPin = GetCaseKeyPinFromCaseValuePin(CaseValuePin);

			CaseValuePin->PinName = *GetCasePinName(CaseValuePinNamePrefix.ToString(), Index);
			CaseValuePin->PinFriendlyName =
				FText::AsCultureInvariant(GetCasePinFriendlyName(CaseValuePinFriendlyNamePrefix.ToString(), Index));
			CaseKeyPin->PinName = *GetCasePinName(CaseKeyPinNamePrefix.ToString(), Index);
			CaseKeyPin->PinFriendlyName =
				FText::AsCultureInvariant(GetCasePinFriendlyName(CaseKeyPinFriendlyNamePrefix.ToString(), Index));

			++Index;
		}
	}

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
}

int32 UK2Node_CasePairedPinsNode::GetCasePinCount() const
{
	// TODO: need to optimize.
	for (int32 Index = Pins.Num(); Index >= 0; --Index)
	{
		UEdGraphPin* CaseKeyPinIndex = GetCaseKeyPinFromCaseIndex(Index);
		UEdGraphPin* CaseValuePinIndex = GetCaseValuePinFromCaseIndex(Index);

		if ((CaseKeyPinIndex != nullptr) && (CaseValuePinIndex != nullptr))
		{
			return Index + 1;
		}
	}

	return 0;
}

TArray<CasePinPair> UK2Node_CasePairedPinsNode::GetCasePinPairs() const
{
	TArray<CasePinPair> CasePairs;
	CasePairs.SetNum(GetCasePinCount());

	for (auto& P : Pins)
	{
		if (IsCaseValuePin(P))
		{
			UEdGraphPin* CaseValuePin = P;
			int32 Index = GetCaseIndexFromCaseValuePin(CaseValuePin);
			CasePairs[Index] = GetCasePinPair(CaseValuePin);
		}
	}

	return CasePairs;
}

bool UK2Node_CasePairedPinsNode::IsCasePin(const UEdGraphPin* Pin) const
{
	return IsCaseKeyPin(Pin) || IsCaseValuePin(Pin);
}

bool UK2Node_CasePairedPinsNode::IsCaseKeyPin(const UEdGraphPin* Pin) const
{
	FString PinName = Pin->GetFName().ToString();

	FRegexPattern Pattern = FRegexPattern(FString::Format(TEXT("{0}_[0-9]+"), {CaseKeyPinNamePrefix.ToString()}));
	FRegexMatcher Matcher(Pattern, PinName);

	return Matcher.FindNext();
}

bool UK2Node_CasePairedPinsNode::IsCaseValuePin(const UEdGraphPin* Pin) const
{
	FString PinName = Pin->GetFName().ToString();

	FRegexPattern Pattern = FRegexPattern(FString::Format(TEXT("{0}_[0-9]+"), {CaseValuePinNamePrefix.ToString()}));
	FRegexMatcher Matcher(Pattern, PinName);

	return Matcher.FindNext();
}

FString UK2Node_CasePairedPinsNode::GetCasePinName(const FString& Prefix, int32 CaseIndex) const
{
	return FString::Printf(TEXT("%s_%d"), *Prefix, CaseIndex);
}

FString UK2Node_CasePairedPinsNode::GetCasePinFriendlyName(const FString& Prefix, int32 CaseIndex) const
{
	return FString::Printf(TEXT("%s%d"), *Prefix, CaseIndex);
}

UEdGraphPin* UK2Node_CasePairedPinsNode::GetCaseKeyPinFromCaseValuePin(const UEdGraphPin* ValuePin) const
{
	FString ValuePinName = ValuePin->GetFName().ToString();

	FString Dummy;
	FString Suffix;
	if (!ValuePinName.Split(CaseValuePinNamePrefix.ToString(), &Dummy, &Suffix, ESearchCase::CaseSensitive))
	{
		return nullptr;
	}

	return FindPin(CaseKeyPinNamePrefix.ToString() + Suffix);
}

UEdGraphPin* UK2Node_CasePairedPinsNode::GetCaseValuePinFromCaseKeyPin(const UEdGraphPin* KeyPin) const
{
	FString KeyPinName = KeyPin->GetFName().ToString();

	FString Dummy;
	FString Suffix;
	if (!KeyPinName.Split(CaseKeyPinNamePrefix.ToString(), &Dummy, &Suffix, ESearchCase::CaseSensitive))
	{
		return nullptr;
	}

	return FindPin(CaseValuePinNamePrefix.ToString() + Suffix);
}

void UK2Node_CasePairedPinsNode::AddCasePinLast()
{
	Modify();

	int32 N = GetCasePinCount();

	AddCasePinPair(N);
}

#undef LOCTEXT_NAMESPACE

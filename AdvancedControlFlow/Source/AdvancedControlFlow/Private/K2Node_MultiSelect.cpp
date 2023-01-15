/*!
 * AdvancedControlFlow
 *
 * Copyright (c) 2022-2023 Colory Games
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "K2Node_MultiSelect.h"

#include "EditorCategoryUtils.h"
#include "GraphEditorSettings.h"
#include "K2Node_Select.h"
#include "K2Node_MakeArray.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"
#include "Internationalization/Regex.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#define LOCTEXT_NAMESPACE "AdvancedControlFlow"

const FName OptionPinNamePrefix(TEXT("Option"));
const FName ConditionPinNamePrefix(TEXT("Condition"));
const FName DefaultOptionPinName(TEXT("Default"));
const FName ReturnValueOptionPinName(TEXT("Return Value"));
const FName OptionPinFriendlyNamePrefix(TEXT("Option "));
const FName ConditionPinFriendlyNamePrefix(TEXT("Condition "));

UK2Node_MultiSelect::UK2Node_MultiSelect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UK2Node_MultiSelect::AllocateDefaultPins()
{
	// Pin structure
	//   N: Number of option/condition pin pair
	// -----
	// 0-(N-1): Option (In, Wildcard)
	// N: Default (In, Wildcard)
	// (N+1)-2N: Condition (In, Boolean)
	// 2N+1: Return Value (Out, Boolean)

	CreateDefaultOptionPin();
	CreateReturnValuePin();

	for (int32 Index = 0; Index < 2; ++Index)
	{
		AddCasePinPair(Index);
	}

	Super::AllocateDefaultPins();
}

FText UK2Node_MultiSelect::GetTooltipText() const
{
	return LOCTEXT("MultiSelect_Tooltip", "Multi Select\nReturn the option where the condition is true");
}

FLinearColor UK2Node_MultiSelect::GetNodeTitleColor() const
{
	return GetDefault<UGraphEditorSettings>()->PureFunctionCallNodeTitleColor;
}

FText UK2Node_MultiSelect::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("MultiSelect", "MultiSelect");
}

FSlateIcon UK2Node_MultiSelect::GetIconAndTint(FLinearColor& OutColor) const
{
	static FSlateIcon Icon("EditorStyle", "GraphEditor.Select_16x");
	return Icon;
}

void UK2Node_MultiSelect::PinConnectionListChanged(UEdGraphPin* Pin)
{
	if (Pin == nullptr)
	{
		return;
	}

	if (Pin->LinkedTo.Num() == 0)
	{
		return;
	}

	UEdGraphPin* LinkedPin = Pin->LinkedTo[0];

	GetDefaultOptionPin()->PinType = LinkedPin->PinType;
	GetReturnValuePin()->PinType = LinkedPin->PinType;

	TArray<CasePinPair> CasePinPairs = GetCasePinPairs();
	for (auto& Pair : CasePinPairs)
	{
		Pair.Key->PinType = LinkedPin->PinType;
	}
}

void UK2Node_MultiSelect::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	Super::AllocateDefaultPins();

	UEdGraphPin* OldDefaultPin = nullptr;
	int32 CasePinCount = 0;
	for (auto& Pin : OldPins)
	{
		if (Pin->GetFName() == DefaultOptionPinName)
		{
			OldDefaultPin = Pin;
		}
		if (IsOptionPin(Pin))
		{
			++CasePinCount;
		}
	}

	CreateDefaultOptionPin();
	CreateReturnValuePin();
	for (int32 Index = 0; Index < CasePinCount; ++Index)
	{
		AddCasePinPair(Index);
	}

	if (OldDefaultPin != nullptr)
	{
		GetDefaultOptionPin()->PinType = OldDefaultPin->PinType;
		for (auto& Pair : GetCasePinPairs())
		{
			Pair.Key->PinType = OldDefaultPin->PinType;
		}
	}
}

void UK2Node_MultiSelect::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_MultiSelect::GetMenuCategory() const
{
	return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::Utilities);
}

void UK2Node_MultiSelect::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	UEdGraphPin* ReferenceOptionPin = GetCasePinPairs()[0].Key;

	FEdGraphPinType Select1stPinType;
	Select1stPinType.PinCategory = UEdGraphSchema_K2::PC_Int;
	Select1stPinType.PinSubCategory = UEdGraphSchema_K2::PSC_Index;
	Select1stPinType.PinSubCategoryObject = nullptr;

	FEdGraphPinType Select2ndPinType;
	Select2ndPinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
	Select2ndPinType.PinSubCategory = UEdGraphSchema_K2::PSC_Index;
	Select1stPinType.PinSubCategoryObject = nullptr;

	UK2Node_Select* Select1st = CompilerContext.SpawnIntermediateNode<UK2Node_Select>(this, SourceGraph);
	Select1st->AllocateDefaultPins();
	Select1st->ChangePinType(ReferenceOptionPin);

	UK2Node_Select* Select2nd = CompilerContext.SpawnIntermediateNode<UK2Node_Select>(this, SourceGraph);
	Select2nd->AllocateDefaultPins();
	Select2nd->ChangePinType(ReferenceOptionPin);

	UK2Node_MakeArray* MakeArray = CompilerContext.SpawnIntermediateNode<UK2Node_MakeArray>(this, SourceGraph);
	MakeArray->AllocateDefaultPins();
	MakeArray->AddInputPin();

	UK2Node_CallFunction* ArrayFind = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	UClass* KismetArrayLibrary = UKismetArrayLibrary::StaticClass();
	UFunction* ArrayFindFunction = KismetArrayLibrary->FindFunctionByName("Array_Find");
	ArrayFind->SetFromFunction(ArrayFindFunction);
	ArrayFind->AllocateDefaultPins();

	UK2Node_CallFunction* IntEqual = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	UClass* KismetMathLibrary = UKismetMathLibrary::StaticClass();
	UFunction* EqualEqualIntIntFunction = KismetMathLibrary->FindFunctionByName("EqualEqual_IntInt");
	IntEqual->SetFromFunction(EqualEqualIntIntFunction);
	IntEqual->AllocateDefaultPins();

	TArray<CasePinPair> CasePinPairs = GetCasePinPairs();

	// Link between outer and 1st Select
	TArray<UEdGraphPin*> Select1stOptionPins;
	Select1st->GetOptionPins(Select1stOptionPins);
	for (int Index = 0; Index < CasePinPairs.Num(); ++Index)
	{
		CompilerContext.MovePinLinksToIntermediate(*CasePinPairs[Index].Key, *Select1stOptionPins[Index]);
	}

	// Link between outer and Make Array
	TArray<UEdGraphPin*> KeyPins;
	TArray<UEdGraphPin*> ValuePins;
	MakeArray->GetKeyAndValuePins(KeyPins, ValuePins);
	for (int Index = 0; Index < CasePinPairs.Num(); ++Index)
	{
		KeyPins[Index]->PinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
		CompilerContext.MovePinLinksToIntermediate(*CasePinPairs[Index].Value, *KeyPins[Index]);
	}

	// Link between Make Array and Array Find
	UEdGraphPin* ArrayPin = MakeArray->GetOutputPin();
	UEdGraphPin* TargetArrayPin = ArrayFind->FindPinChecked(TEXT("TargetArray"));
	UEdGraphPin* ItemToFindPin = ArrayFind->FindPinChecked(TEXT("ItemToFind"));
	ArrayPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
	ArrayPin->MakeLinkTo(TargetArrayPin);
	TargetArrayPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
	ItemToFindPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
	MakeArray->GetSchema()->TrySetDefaultValue(*ItemToFindPin, TEXT("true"));

	// Link between Array Find and 1st Select
	UEdGraphPin* ArrayFindOutputPin = ArrayFind->GetReturnValuePin();
	UEdGraphPin* Select1stIndexPin = Select1st->GetIndexPin();
	ArrayFindOutputPin->MakeLinkTo(Select1stIndexPin);
	Select1st->NotifyPinConnectionListChanged(Select1stIndexPin);

	// Link between Array Find and Int Equal
	UEdGraphPin* IntEqualAPin = IntEqual->FindPinChecked(TEXT("A"));
	UEdGraphPin* IntEqualBPin = IntEqual->FindPinChecked(TEXT("B"));
	ArrayFindOutputPin->MakeLinkTo(IntEqualAPin);
	ArrayFindOutputPin->GetSchema()->TrySetDefaultValue(*IntEqualBPin, TEXT("-1"));

	// Link among 1st Select, 2nd Select and Int Equal
	UEdGraphPin* Select1stReturnValuePin = Select1st->GetReturnValuePin();
	UEdGraphPin* IntEqualReturnValuePin = IntEqual->GetReturnValuePin();
	UEdGraphPin* Select2ndIndexPin = Select2nd->GetIndexPin();
	TArray<UEdGraphPin*> Select2ndOptionPins;
	Select2nd->GetOptionPins(Select2ndOptionPins);
	IntEqualReturnValuePin->MakeLinkTo(Select2ndIndexPin);
	Select2nd->NotifyPinConnectionListChanged(Select2ndIndexPin);
	Select1stReturnValuePin->MakeLinkTo(Select2ndOptionPins[0]);
	CompilerContext.MovePinLinksToIntermediate(*GetDefaultOptionPin(), *Select2ndOptionPins[1]);

	// Link 2nd Select and outer
	UEdGraphPin* Select2ndReturnValuePin = Select2nd->GetReturnValuePin();
	CompilerContext.MovePinLinksToIntermediate(*GetReturnValuePin(), *Select2ndReturnValuePin);

	BreakAllNodeLinks();
}

UEdGraphPin* UK2Node_MultiSelect::GetOptionPinFromCaseIndex(int32 CaseIndex) const
{
	for (auto& P : Pins)
	{
		if ((P->Direction == EGPD_Input) && IsOptionPin(P))
		{
			int32 ActualIndex = GetCaseIndexFromCasePin(OptionPinNamePrefix.ToString(), P);
			if ((ActualIndex != -1) && (ActualIndex == CaseIndex))
			{
				return P;
			}
		}
	}

	return nullptr;
}

UEdGraphPin* UK2Node_MultiSelect::GetConditionPinFromCaseIndex(int32 CaseIndex) const
{
	for (auto& P : Pins)
	{
		if ((P->Direction == EGPD_Input) && IsConditionPin(P))
		{
			int32 ActualIndex = GetCaseIndexFromCasePin(ConditionPinNamePrefix.ToString(), P);
			if ((ActualIndex != -1) && (ActualIndex == CaseIndex))
			{
				return P;
			}
		}
	}

	return nullptr;
}

CasePinPair UK2Node_MultiSelect::AddCasePinPair(int32 CaseIndex)
{
	CasePinPair Pair;
	int N = GetCasePinCount();

	{
		FCreatePinParams Params;
		Params.Index = CaseIndex;
		Pair.Key = CreatePin(
			EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, *GetCasePinName(OptionPinNamePrefix.ToString(), CaseIndex), Params);
		Pair.Key->PinFriendlyName =
			FText::AsCultureInvariant(GetCasePinFriendlyName(OptionPinFriendlyNamePrefix.ToString(), CaseIndex));
	}
	{
		FCreatePinParams Params;
		Params.Index = N + 1 + CaseIndex;
		Pair.Value = CreatePin(
			EGPD_Input, UEdGraphSchema_K2::PC_Boolean, *GetCasePinName(ConditionPinNamePrefix.ToString(), CaseIndex), Params);
		Pair.Value->PinFriendlyName =
			FText::AsCultureInvariant(GetCasePinFriendlyName(ConditionPinFriendlyNamePrefix.ToString(), CaseIndex));
	}

	return Pair;
}

FString UK2Node_MultiSelect::GetCasePinName(const FString& Prefix, int32 CaseIndex) const
{
	return FString::Printf(TEXT("%s_%d"), *Prefix, CaseIndex);
}

FString UK2Node_MultiSelect::GetCasePinFriendlyName(const FString& Prefix, int32 CaseIndex) const
{
	return FString::Printf(TEXT("%s%d"), *Prefix, CaseIndex);
}

bool UK2Node_MultiSelect::IsOptionPin(const UEdGraphPin* Pin) const
{
	FString PinName = Pin->GetFName().ToString();

	FRegexPattern Pattern = FRegexPattern(FString::Format(TEXT("{0}_"), { OptionPinNamePrefix.ToString() }));
	FRegexMatcher Matcher(Pattern, PinName);

	return Matcher.FindNext();
}

bool UK2Node_MultiSelect::IsConditionPin(const UEdGraphPin* Pin) const
{
	FString PinName = Pin->GetFName().ToString();

	FRegexPattern Pattern = FRegexPattern(FString::Format(TEXT("{0}_"), { ConditionPinNamePrefix.ToString() }));
	FRegexMatcher Matcher(Pattern, PinName);

	return Matcher.FindNext();
}

int32 UK2Node_MultiSelect::GetCaseIndexFromCasePin(const FString& Prefix, UEdGraphPin* Pin) const
{
	FString PinName = Pin->GetFName().ToString();
	FString Dummy;
	FString IndexStr;
	if (!PinName.Split(Prefix + "_", &Dummy, &IndexStr, ESearchCase::CaseSensitive))
	{
		return -1;
	}

	return FCString::Atoi(*IndexStr);
}

TArray<CasePinPair> UK2Node_MultiSelect::GetCasePinPairs() const
{
	TArray<CasePinPair> CasePairs;
	CasePairs.SetNum(GetCasePinCount());

	int32 FoundPairsNum = 0;
	for (auto& P : Pins)
	{
		if (IsOptionPin(P))
		{
			UEdGraphPin* CaseOptionPin = P;
			int32 Index = GetCaseIndexFromCasePin(OptionPinNamePrefix.ToString(), CaseOptionPin);
			UEdGraphPin* CaseConditionPin = GetConditionPinFromCaseIndex(Index);
			check(CaseConditionPin);

			CasePairs[Index] = {CaseOptionPin, CaseConditionPin};
			++FoundPairsNum;
		}
	}
	check(CasePairs.Num() == FoundPairsNum);

	return CasePairs;
}

int32 UK2Node_MultiSelect::GetCasePinCount() const
{
	// TODO: need to optimize.
	for (int32 Index = Pins.Num(); Index >= 0; --Index)
	{
		UEdGraphPin* OptionPinIndex = GetOptionPinFromCaseIndex(Index);
		UEdGraphPin* ConditionPinIndex = GetConditionPinFromCaseIndex(Index);

		if ((OptionPinIndex != nullptr) && (ConditionPinIndex != nullptr))
		{
			return Index + 1;
		}
	}

	return 0;
}

void UK2Node_MultiSelect::CreateDefaultOptionPin()
{
	int N = GetCasePinCount();

	FCreatePinParams Params;
	Params.Index = N;
	UEdGraphPin* DefaultOptionPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, DefaultOptionPinName, Params);
}

void UK2Node_MultiSelect::CreateReturnValuePin()
{
	int N = GetCasePinCount();

	FCreatePinParams Params;
	Params.Index = 2 * N + 1;
	UEdGraphPin* DefaultOptionPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, ReturnValueOptionPinName, Params);
}

UEdGraphPin* UK2Node_MultiSelect::GetDefaultOptionPin() const
{
	return FindPin(DefaultOptionPinName);
}

UEdGraphPin* UK2Node_MultiSelect::GetReturnValuePin() const
{
	return FindPin(ReturnValueOptionPinName);
}

#undef LOCTEXT_NAMESPACE
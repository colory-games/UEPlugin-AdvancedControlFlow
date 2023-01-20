/*!
 * AdvancedControlFlow
 *
 * Copyright (c) 2022-2023 Colory Games
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "K2Node.h"
#include "Misc/EngineVersionComparison.h"

#include "K2Node_CasePairedPinsNode.generated.h"

typedef TPair<UEdGraphPin*, UEdGraphPin*> CasePinPair;

extern const FName DefaultExecPinName;
extern const FName DefaultExecPinFriendlyName;

UCLASS(MinimalAPI)
class UK2Node_CasePairedPinsNode : public UK2Node
{
	GENERATED_BODY()

protected:
	// Override from UK2Node
	virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual bool CanEverInsertExecutionPin() const override
	{
		return true;
	}
	virtual bool CanEverRemoveExecutionPin() const override
	{
		return true;
	}
	virtual bool IncludeParentNodeContextMenu() const override
	{
		return true;
	}

	// Internal functions.
	UEdGraphPin* GetCaseKeyPinFromCaseIndex(int32 CaseIndex) const;
	UEdGraphPin* GetCaseValuePinFromCaseIndex(int32 CaseIndex) const;

	int32 GetCaseIndexFromCasePin(UEdGraphPin* Pin) const;
	int32 GetCaseIndexFromCasePin(const FString& Prefix, UEdGraphPin* Pin) const;
	int32 GetCaseIndexFromCaseKeyPin(UEdGraphPin* Pin) const;
	int32 GetCaseIndexFromCaseValuePin(UEdGraphPin* Pin) const;

	CasePinPair GetCasePinPair(UEdGraphPin* Pin) const;
	TArray<CasePinPair> GetCasePinPairs() const;

	FString GetCasePinName(const FString& Prefix, int32 CaseIndex) const;
	FString GetCasePinFriendlyName(const FString& Prefix, int32 CaseIndex) const;

	virtual CasePinPair AddCasePinPair(int32 CaseIndex) { return CasePinPair(); }
	void AddCasePinAfter(UEdGraphPin* Pin);
	void AddCasePinBefore(UEdGraphPin* Pin);
	void RemoveCasePinAt(UEdGraphPin* Pin);
	void RemoveCasePinAt(int32 CaseIndex);
	void RemoveFirstCasePin();
	void RemoveLastCasePin();

	bool IsCasePin(const UEdGraphPin* Pin) const;
	bool IsCaseKeyPin(const UEdGraphPin* Pin) const;
	bool IsCaseValuePin(const UEdGraphPin* Pin) const;

	FName NodeContextMenuSectionName;
	FText NodeContextMenuSectionLabel;
	FName CaseKeyPinNamePrefix;
	FName CaseValuePinNamePrefix;
	FName CaseKeyPinFriendlyNamePrefix;
	FName CaseValuePinFriendlyNamePrefix;

public:
	UK2Node_CasePairedPinsNode(const FObjectInitializer& ObjectInitializer);

	UEdGraphPin* GetCaseValuePinFromCaseKeyPin(const UEdGraphPin* CondPin) const;
	UEdGraphPin* GetCaseKeyPinFromCaseValuePin(const UEdGraphPin* ExecPin) const;

	int32 GetCasePinCount() const;
	void AddCasePinLast();
};

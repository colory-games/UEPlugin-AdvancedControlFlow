/*!
 * AdvancedControlFlow
 *
 * Copyright (c) 2022 Colory Games
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "K2Node.h"
#include "K2Node_AddPinInterface.h"

#include "K2Node_CondExecPairedNode.generated.h"

// { Case Conditional Pin : Case Execution Pin }
typedef TPair<UEdGraphPin*, UEdGraphPin*> CasePinPair;

extern const FName DefaultExecPinName;
extern const FName CaseExecPinNamePrefix;
extern const FName CaseCondPinNamePrefix;
extern const FName DefaultExecPinFriendlyName;
extern const FName CaseExecPinFriendlyNamePrefix;
extern const FName CaseCondPinFriendlyNamePrefix;

UCLASS(MinimalAPI)
class UK2Node_CondExecPairedNode : public UK2Node, public IK2Node_AddPinInterface
{
	GENERATED_BODY()

public:
	// Override from IK2Node_AddPinInterface
	virtual void AddInputPin() override;
#if UE_VERSION_NEWER_THAN(5, 0, 0)
	virtual void RemoveInputPin(UEdGraphPin* Pin) override;
#else
	virtual void RemoveInputPin(UEdGraphPin* Pin);
#endif

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

	// Override from UEdGraphNode
	virtual void AllocateDefaultPins() override;

	// Internal functions.
	UEdGraphPin* GetCaseCondPinFromCaseIndex(int32 CaseIndex) const;
	UEdGraphPin* GetCaseExecPinFromCaseIndex(int32 CaseIndex) const;
	CasePinPair GetCasePinPair(UEdGraphPin* Pin) const;
	TArray<CasePinPair> GetCasePinPairs() const;
	int32 GetCaseIndexFromCasePin(UEdGraphPin* Pin) const;
	int32 GetCaseIndexFromCasePin(const FString& Prefix, UEdGraphPin* Pin) const;
	int32 GetCaseIndexFromCaseExecPin(UEdGraphPin* Pin) const;
	int32 GetCaseIndexFromCaseCondPin(UEdGraphPin* Pin) const;
	FString GetCasePinName(const FString& Prefix, int32 CaseIndex) const;
	FString GetCasePinFriendlyName(const FString& Prefix, int32 CaseIndex) const;
	CasePinPair AddCasePinPair(int32 CaseIndex);
	void AddCasePinAfter(UEdGraphPin* Pin);
	void AddCasePinBefore(UEdGraphPin* Pin);
	void RemoveCasePinAt(int32 CaseIndex);
	void RemoveFirstCasePin();
	void RemoveLastCasePin();
	bool IsCasePin(UEdGraphPin* Pin) const;

	virtual void CreateExecTriggeringPin();
	virtual void CreateDefaultExecPin();

	FString NodeContextMenuSectionName;
	FString NodeContextMenuSectionLabel;

public:
	UK2Node_CondExecPairedNode(const FObjectInitializer& ObjectInitializer);

	UEdGraphPin* GetDefaultExecPin() const;
	UEdGraphPin* GetCondPinFromExecPin(const UEdGraphPin* ExecPin) const;
	UEdGraphPin* GetExecPinFromCondPin(const UEdGraphPin* CondPin) const;

	int32 GetCasePinCount() const;
};

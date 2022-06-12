#pragma once

#include "BlueprintActionDatabaseRegistrar.h"
#include "K2Node.h"
#include "K2Node_AddPinInterface.h"

#include "K2Node_MultipleCaseBranch.generated.h"

// { Case Conditional Pin : Case Execution Pin }
typedef TPair<UEdGraphPin*, UEdGraphPin*> CasePinPair;

UCLASS(MinimalAPI, meta = (Keywords = "elseif"))
class UK2Node_MultipleCaseBranch : public UK2Node, public IK2Node_AddPinInterface
{
	GENERATED_BODY()

#if WITH_EDITOR
	// Override from UEdGraphNode
	virtual void AllocateDefaultPins() override;
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
#endif

	// Override from UK2Node
	virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual class FNodeHandlingFunctor* CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
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

	// Override from IK2Node_AddPinInterface
	virtual void AddInputPin() override;
	virtual void RemoveInputPin(UEdGraphPin* Pin) override;

	void AddCasePinAfter(UEdGraphPin* Pin);
	void AddCasePinBefore(UEdGraphPin* Pin);
	void RemoveFirstCasePin();
	void RemoveLastCasePin();

	// Internal functions.
	UEdGraphPin* GetCaseCondPinFromCaseIndex(int32 CaseIndex) const;
	UEdGraphPin* GetCaseExecPinFromCaseIndex(int32 CaseIndex) const;
	CasePinPair GetCasePinPair(UEdGraphPin* Pin) const;
	int32 GetCaseIndexFromCasePin(UEdGraphPin* Pin) const;
	int32 GetCaseIndexFromCasePin(const FString& Prefix, UEdGraphPin* Pin) const;
	int32 GetCaseIndexFromCaseExecPin(UEdGraphPin* Pin) const;
	int32 GetCaseIndexFromCaseCondPin(UEdGraphPin* Pin) const;
	void RemoveCasePinAt(int32 CaseIndex);
	int32 GetCasePinCount() const;
	FString GetCasePinName(const FString& Prefix, int32 CaseIndex) const;
	FString GetCasePinFriendlyName(const FString& Prefix, int32 CaseIndex) const;
	TArray<CasePinPair> GetCasePinPairs() const;
	void AddCasePinAt(int32 CaseIndex);
	bool IsCasePin(UEdGraphPin* Pin) const;
	CasePinPair AddCasePinPair(int32 CaseIndex);

protected:
	TSubclassOf<class UObject> ConditionPreProcessFuncClass;
	FName ConditionPreProcessFuncName;

	virtual void CreateExecTriggeringPin();
	virtual void CreateDefaultExecPin();
	virtual void CreateFunctionPin();

public:
	UK2Node_MultipleCaseBranch(const FObjectInitializer& ObjectInitializer);

	UEdGraphPin* GetDefaultExecPin() const;
	UEdGraphPin* GetFunctionPin() const;
	UEdGraphPin* GetCondPinFromExecPin(const UEdGraphPin* ExecPin) const;
	UEdGraphPin* GetExecPinFromCondPin(const UEdGraphPin* CondPin) const;
};

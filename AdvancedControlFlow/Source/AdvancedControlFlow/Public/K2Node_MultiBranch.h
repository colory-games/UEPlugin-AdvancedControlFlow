/*!
 * AdvancedControlFlow
 *
 * Copyright (c) 2022 Colory Games
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "BlueprintActionDatabaseRegistrar.h"
#include "K2Node_CasePairedPinsNode.h"

#include "K2Node_MultiBranch.generated.h"

UCLASS(MinimalAPI, meta = (Keywords = "If ElseIf Else Branch"))
class UK2Node_MultiBranch : public UK2Node_CasePairedPinsNode
{
	GENERATED_BODY()

	// Override from UEdGraphNode
	virtual void AllocateDefaultPins() override;
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;

	// Override from UK2Node
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual class FNodeHandlingFunctor* CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;

	void CreateFunctionPin();
	void CreateExecTriggeringPin();
	void CreateDefaultExecPin();
	virtual CasePinPair AddCasePinPair(int32 CaseIndex) override;

	TSubclassOf<class UObject> ConditionPreProcessFuncClass;
	FName ConditionPreProcessFuncName;


public:
	UK2Node_MultiBranch(const FObjectInitializer& ObjectInitializer);

	UEdGraphPin* GetDefaultExecPin() const;
	UEdGraphPin* GetFunctionPin() const;
};

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
#include "K2Node_CondExecPairedNode.h"

#include "K2Node_MultiBranch.generated.h"

UCLASS(MinimalAPI, meta = (Keywords = "If ElseIf Else Branch"))
class UK2Node_MultiBranch : public UK2Node_CondExecPairedNode
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

protected:
	TSubclassOf<class UObject> ConditionPreProcessFuncClass;
	FName ConditionPreProcessFuncName;

	virtual void CreateFunctionPin();

public:
	UK2Node_MultiBranch(const FObjectInitializer& ObjectInitializer);

	UEdGraphPin* GetFunctionPin() const;
};

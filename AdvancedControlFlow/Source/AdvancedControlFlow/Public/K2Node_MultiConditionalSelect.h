/*!
 * AdvancedControlFlow
 *
 * Copyright (c) 2022-2023 Colory Games
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "BlueprintActionDatabaseRegistrar.h"
#include "K2Node_CasePairedPinsNode.h"

#include "K2Node_MultiConditionalSelect.generated.h"


UCLASS(MinimalAPI, meta = (Keywords = "Select"))
class UK2Node_MultiConditionalSelect : public UK2Node_CasePairedPinsNode
{
	GENERATED_BODY()

	// Override from UEdGraphNode
	virtual void AllocateDefaultPins() override;
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;

	// Override from UK2Node
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual bool IsNodePure() const override
	{
		return true;
	}

	// Internal functions.
	void CreateDefaultOptionPin();
	void CreateReturnValuePin();
	UEdGraphPin* GetDefaultOptionPin() const;
	UEdGraphPin* GetReturnValuePin() const;
	virtual CasePinPair AddCasePinPair(int32 CaseIndex) override;

public:
	UK2Node_MultiConditionalSelect(const FObjectInitializer& ObjectInitializer);
};

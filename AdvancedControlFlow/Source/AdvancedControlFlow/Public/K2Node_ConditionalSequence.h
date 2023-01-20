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

#include "K2Node_ConditionalSequence.generated.h"

UCLASS(MinimalAPI, meta = (Keywords = "Sequence Conditional ConditionalSequence"))
class UK2Node_ConditionalSequence : public UK2Node_CasePairedPinsNode
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
	virtual FText GetMenuCategory() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

	void CreateExecTriggeringPin();
	void CreateDefaultExecPin();
	virtual CasePinPair AddCasePinPair(int32 CaseIndex) override;

public:
	UK2Node_ConditionalSequence(const FObjectInitializer& ObjectInitializer);

	UEdGraphPin* GetDefaultExecPin() const;
};

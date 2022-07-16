/*!
 * AdvancedControlFlow
 *
 * Copyright (c) 2022 Colory Games
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "K2Node_CondExecPairedNode.h"

#include "K2Node_ConditionalSequence.generated.h"

UCLASS(MinimalAPI, meta = (Keywords = "Sequence Conditional"))
class UK2Node_ConditionalSequence : public UK2Node_CondExecPairedNode
{
	GENERATED_BODY()

	// Override from UEdGraphNode
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;

	// Override from UK2Node
	virtual FText GetMenuCategory() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

public:
	UK2Node_ConditionalSequence(const FObjectInitializer& ObjectInitializer);
};

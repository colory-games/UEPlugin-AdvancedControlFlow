/*!
 * AdvancedControlFlow
 *
 * Copyright (c) 2022 Colory Games
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "SGraphNodeMultiConditionalSelect.h"

void SGraphNodeMultiConditionalSelect::Construct(const FArguments& InArgs, UK2Node_MultiConditionalSelect* InNode)
{
	this->GraphNode = InNode;
	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
}

void SGraphNodeMultiConditionalSelect::CreatePinWidgets()
{
	UK2Node_MultiConditionalSelect* MultiConditionalSelect = CastChecked<UK2Node_MultiConditionalSelect>(GraphNode);

	for (auto It = GraphNode->Pins.CreateConstIterator(); It; ++It)
	{
		UEdGraphPin* Pin = *It;
		if (!Pin->bHidden)
		{
			TSharedPtr<SGraphPin> NewPin = FNodeFactory::CreatePinWidget(Pin);
			check(NewPin.IsValid());

			this->AddPin(NewPin.ToSharedRef());
		}
	}
}

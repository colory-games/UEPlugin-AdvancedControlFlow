/*!
 * AdvancedControlFlow
 *
 * Copyright (c) 2022-2023 Colory Games
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "SGraphNodeCasePairedPinsNode.h"

class UK2Node_ConditionalSequence;

class SGraphNodeConditionalSequence : public SGraphNodeCasePairedPinsNode
{
	SLATE_BEGIN_ARGS(SGraphNodeConditionalSequence)
	{
	}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UK2Node_ConditionalSequence* InNode);

	virtual void CreatePinWidgets() override;
};
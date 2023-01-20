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

class UK2Node_MultiConditionalSelect;

class SGraphNodeMultiConditionalSelect : public SGraphNodeCasePairedPinsNode
{
	SLATE_BEGIN_ARGS(SGraphNodeMultiConditionalSelect)
	{
	}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UK2Node_MultiConditionalSelect* InNode);

	virtual void CreatePinWidgets() override;

};
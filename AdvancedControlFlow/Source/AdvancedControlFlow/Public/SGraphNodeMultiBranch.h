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

class UK2Node_MultiBranch;

class SGraphNodeMultiBranch : public SGraphNodeCasePairedPinsNode
{
	SLATE_BEGIN_ARGS(SGraphNodeMultiBranch)
	{
	}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UK2Node_MultiBranch* InNode);

	virtual void CreatePinWidgets() override;

};
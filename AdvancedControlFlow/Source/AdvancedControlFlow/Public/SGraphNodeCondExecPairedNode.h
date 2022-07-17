/*!
 * AdvancedControlFlow
 *
 * Copyright (c) 2022 Colory Games
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "KismetNodes/SGraphNodeK2Base.h"

class UK2Node_CondExecPairedNode;

class SGraphNodeCondExecPairedNode : public SGraphNodeK2Base
{
	SLATE_BEGIN_ARGS(SGraphNodeCondExecPairedNode)
	{
	}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UK2Node_CondExecPairedNode* InNode);

	virtual void CreatePinWidgets() override;

protected:
	virtual void CreateOutputSideAddButton(TSharedPtr<SVerticalBox> OutputBox) override;
	virtual EVisibility IsAddPinButtonVisible() const override;
	virtual FReply OnAddPin() override;
};
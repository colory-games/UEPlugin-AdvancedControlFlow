#pragma once

#include "KismetNodes/SGraphNodeK2Base.h"

class UK2Node_MultipleCaseBranch;

class SGraphNodeMultipleCaseBranch : public SGraphNodeK2Base
{
	SLATE_BEGIN_ARGS(SGraphNodeMultipleCaseBranch){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UK2Node_MultipleCaseBranch* InNode);

	virtual void CreatePinWidgets() override;

protected:
	virtual void CreateOutputSideAddButton(TSharedPtr<SVerticalBox> OutputBox) override;
	virtual EVisibility IsAddPinButtonVisible() const override;
	virtual FReply OnAddPin() override;
};
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

#include "DetailLayoutBuilder.h"
#include "EditorStyleSet.h"
#include "GraphEditorSettings.h"
#include "K2Node_MultiConditionalSelect.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetPins/SGraphPinExec.h"
#include "NodeFactory.h"

class SGraphPinExecMultiConditionalSelect : public SGraphPinExec
{
public:
	SLATE_BEGIN_ARGS(SGraphPinExecMultiConditionalSelect)
	{
	}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
	{
		SGraphPin::Construct(SGraphPin::FArguments().PinLabelStyle(FName("Graph.Node.DefaultPinName")), InPin);

		CachePinIcons();
	}
};

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

void SGraphNodeMultiConditionalSelect::CreateOutputSideAddButton(TSharedPtr<SVerticalBox> OutputBox)
{
	UK2Node_MultiConditionalSelect* MultiConditionalSelect = CastChecked<UK2Node_MultiConditionalSelect>(GraphNode);

#ifdef ACF_FREE_VERSION
	if (MultiConditionalSelect->GetCasePinCount() >= 3)
	{
		FMargin Padding = Settings->GetOutputPinPadding();
		Padding.Top += 6.0f;
		Padding.Right += 3.0f;
		FText ErrorMessage = FText::FromString("Free Ver: Up to 3");
		OutputBox->AddSlot()
			.AutoHeight()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			.Padding(Padding)[SNew(STextBlock).Font(IDetailLayoutBuilder::GetDetailFont()).Text(ErrorMessage)];
		return;
	}
#endif

	// TODO: Use NSLOCTEXT macro
	TSharedRef<SWidget> AddPinButton =
		AddPinButtonContent(FText::AsCultureInvariant("Add pin"), FText::AsCultureInvariant("Add new pin"));

	FMargin AddPinPadding = Settings->GetOutputPinPadding();
	AddPinPadding.Top += 6.0f;
	AddPinPadding.Right += 3.0f;

	OutputBox->AddSlot().AutoHeight().VAlign(VAlign_Center).HAlign(HAlign_Right).Padding(AddPinPadding)[AddPinButton];
}

EVisibility SGraphNodeMultiConditionalSelect::IsAddPinButtonVisible() const
{
	return SGraphNode::IsAddPinButtonVisible();
}

FReply SGraphNodeMultiConditionalSelect::OnAddPin()
{
	UK2Node_MultiConditionalSelect* MultiConditionalSelect = CastChecked<UK2Node_MultiConditionalSelect>(GraphNode);

	// TODO: Use NSLOCTEXT macro
	const FScopedTransaction Transaction(FText::AsCultureInvariant("Add Execution Pin"));
	MultiConditionalSelect->Modify();

	MultiConditionalSelect->AddCasePinLast();
	FBlueprintEditorUtils::MarkBlueprintAsModified(MultiConditionalSelect->GetBlueprint());

	UpdateGraphNode();
	GraphNode->GetGraph()->NotifyGraphChanged();

	return FReply::Handled();
}
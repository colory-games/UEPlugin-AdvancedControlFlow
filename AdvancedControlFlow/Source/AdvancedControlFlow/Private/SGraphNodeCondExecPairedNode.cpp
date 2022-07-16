/*!
 * AdvancedControlFlow
 *
 * Copyright (c) 2022 Colory Games
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "SGraphNodeCondExecPairedNode.h"

#include "GraphEditorSettings.h"
#include "K2Node_CondExecPairedNode.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetPins/SGraphPinExec.h"
#include "NodeFactory.h"

class SGraphPinCondExecPairedNodeDefaultCaseExec : public SGraphPinExec
{
public:
	SLATE_BEGIN_ARGS(SGraphPinCondExecPairedNodeDefaultCaseExec)
	{
	}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
	{
		SGraphPin::Construct(SGraphPin::FArguments().PinLabelStyle(FName("Graph.Node.DefaultPinName")), InPin);

		CachePinIcons();
	}
};

void SGraphNodeCondExecPairedNode::Construct(const FArguments& InArgs, UK2Node_CondExecPairedNode* InNode)
{
	this->GraphNode = InNode;
	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
}

void SGraphNodeCondExecPairedNode::CreatePinWidgets()
{
	UK2Node_CondExecPairedNode* CondExecPairedNode = CastChecked<UK2Node_CondExecPairedNode>(GraphNode);
	UEdGraphPin* DefaultPin = CondExecPairedNode->GetDefaultExecPin();

	RightNodeBox->AddSlot().AutoHeight()[SNew(STextBlock).LineHeightPercentage(2.0f)];

	for (auto It = GraphNode->Pins.CreateConstIterator(); It; ++It)
	{
		UEdGraphPin* Pin = *It;
		if ((!Pin->bHidden) && (Pin != DefaultPin))
		{
			TSharedPtr<SGraphPin> NewPin = FNodeFactory::CreatePinWidget(Pin);
			check(NewPin.IsValid());

			this->AddPin(NewPin.ToSharedRef());
		}
	}

	if (DefaultPin != nullptr)
	{
		RightNodeBox->AddSlot()
			.AutoHeight()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			.Padding(1.0f)[SNew(SImage).Image(FEditorStyle::GetBrush("Graph.Pin.DefaultPinSeparator"))];

		TSharedPtr<SGraphPin> NewPin = SNew(SGraphPinCondExecPairedNodeDefaultCaseExec, DefaultPin);
		this->AddPin(NewPin.ToSharedRef());
	}
}

void SGraphNodeCondExecPairedNode::CreateOutputSideAddButton(TSharedPtr<SVerticalBox> OutputBox)
{
	// TODO: Use NSLOCTEXT macro
	TSharedRef<SWidget> AddPinButton = AddPinButtonContent(FText::AsCultureInvariant("Add pin"), FText::AsCultureInvariant("Add new pin"));

	FMargin AddPinPadding = Settings->GetOutputPinPadding();
	AddPinPadding.Top += 6.0f;

	OutputBox->AddSlot().AutoHeight().VAlign(VAlign_Center).Padding(AddPinPadding)[AddPinButton];
}

EVisibility SGraphNodeCondExecPairedNode::IsAddPinButtonVisible() const
{
	return SGraphNode::IsAddPinButtonVisible();
}

FReply SGraphNodeCondExecPairedNode::OnAddPin()
{
	UK2Node_CondExecPairedNode* CondExecPairedNode = CastChecked<UK2Node_CondExecPairedNode>(GraphNode);

	// TODO: Use NSLOCTEXT macro
	const FScopedTransaction Transaction(FText::AsCultureInvariant("Add Execution Pin"));
	CondExecPairedNode->Modify();

	CondExecPairedNode->AddInputPin();
	FBlueprintEditorUtils::MarkBlueprintAsModified(CondExecPairedNode->GetBlueprint());

	UpdateGraphNode();
	GraphNode->GetGraph()->NotifyGraphChanged();

	return FReply::Handled();
}
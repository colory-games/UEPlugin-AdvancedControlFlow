#pragma once

#include "SGraphNodeMultiBranch.h"

#include "GraphEditorSettings.h"
#include "K2Node_MultiBranch.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetPins/SGraphPinExec.h"
#include "NodeFactory.h"

class SGraphPinMultiBranchDefaultCaseExec : public SGraphPinExec
{
public:
	SLATE_BEGIN_ARGS(SGraphPinMultiBranchDefaultCaseExec)
	{
	}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
	{
		SGraphPin::Construct(SGraphPin::FArguments().PinLabelStyle(FName("Graph.Node.DefaultPinName")), InPin);

		CachePinIcons();
	}
};

void SGraphNodeMultiBranch::Construct(const FArguments& InArgs, UK2Node_MultiBranch* InNode)
{
	this->GraphNode = InNode;
	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
}

void SGraphNodeMultiBranch::CreatePinWidgets()
{
	UK2Node_MultiBranch* MultiBranch = CastChecked<UK2Node_MultiBranch>(GraphNode);
	UEdGraphPin* DefaultPin = MultiBranch->GetDefaultExecPin();

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

		TSharedPtr<SGraphPin> NewPin = SNew(SGraphPinMultiBranchDefaultCaseExec, DefaultPin);
		this->AddPin(NewPin.ToSharedRef());
	}
}

void SGraphNodeMultiBranch::CreateOutputSideAddButton(TSharedPtr<SVerticalBox> OutputBox)
{
	TSharedRef<SWidget> AddPinButton = AddPinButtonContent(NSLOCTEXT("MultiBranchNode", "MultiBranchNodeAddPinButton", "Add pin"),
			NSLOCTEXT("MultiBranchNode", "MultiBranchNodeAddPinButton_Tooltip", "Add new pin"));

	FMargin AddPinPadding = Settings->GetOutputPinPadding();
	AddPinPadding.Top += 6.0f;

	OutputBox->AddSlot().AutoHeight().VAlign(VAlign_Center).Padding(AddPinPadding)[AddPinButton];
}

EVisibility SGraphNodeMultiBranch::IsAddPinButtonVisible() const
{
	return SGraphNode::IsAddPinButtonVisible();
}

FReply SGraphNodeMultiBranch::OnAddPin()
{
	UK2Node_MultiBranch* MultiBranch = CastChecked<UK2Node_MultiBranch>(GraphNode);

	const FScopedTransaction Transaction(NSLOCTEXT("Kismet", "AddExecutionPin", "Add Execution Pin"));
	MultiBranch->Modify();

	MultiBranch->AddInputPin();
	FBlueprintEditorUtils::MarkBlueprintAsModified(MultiBranch->GetBlueprint());

	UpdateGraphNode();
	GraphNode->GetGraph()->NotifyGraphChanged();

	return FReply::Handled();
}
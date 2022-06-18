#pragma once

#include "SGraphNodeMultipleCaseBranch.h"

#include "GraphEditorSettings.h"
#include "K2Node_MultipleCaseBranch.h"
#include "NodeFactory.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetPins/SGraphPinExec.h"

class SGraphPinMultipleCaseBranchDefaultCaseExec : public SGraphPinExec
{
public:
	SLATE_BEGIN_ARGS(SGraphPinMultipleCaseBranchDefaultCaseExec) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
	{
		SGraphPin::Construct(SGraphPin::FArguments().PinLabelStyle(FName("Graph.Node.DefaultPinName")), InPin);

		CachePinIcons();
	}
};

void SGraphNodeMultipleCaseBranch::Construct(const FArguments& InArgs, UK2Node_MultipleCaseBranch* InNode)
{
	this->GraphNode = InNode;
	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
}

void SGraphNodeMultipleCaseBranch::CreatePinWidgets()
{
	UK2Node_MultipleCaseBranch* MultipleCaseBranch = CastChecked<UK2Node_MultipleCaseBranch>(GraphNode);
	UEdGraphPin* DefaultPin = MultipleCaseBranch->GetDefaultExecPin();

	RightNodeBox->AddSlot()
		.AutoHeight()
		[
			SNew(STextBlock).LineHeightPercentage(2.0f)
		];

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
			.Padding(1.0f)
			[
				SNew(SImage)
				.Image(FEditorStyle::GetBrush("Graph.Pin.DefaultPinSeparator"))
			];

		TSharedPtr<SGraphPin> NewPin = SNew(SGraphPinMultipleCaseBranchDefaultCaseExec, DefaultPin);
		this->AddPin(NewPin.ToSharedRef());
	}
}

void SGraphNodeMultipleCaseBranch::CreateOutputSideAddButton(TSharedPtr<SVerticalBox> OutputBox)
{
	TSharedRef<SWidget> AddPinButton = AddPinButtonContent(
		NSLOCTEXT("MultipleCaseBranchNode", "MultipleCaseBranchNodeAddPinButton", "Add pin"),
		NSLOCTEXT("MultipleCaseBranchNode", "MultipleCaseBranchNodeAddPinButton_Tooltip", "Add new pin")
	);

	FMargin AddPinPadding = Settings->GetOutputPinPadding();
	AddPinPadding.Top += 6.0f;

	OutputBox->AddSlot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		.Padding(AddPinPadding)
		[
			AddPinButton
		];
}

EVisibility SGraphNodeMultipleCaseBranch::IsAddPinButtonVisible() const
{
	return SGraphNode::IsAddPinButtonVisible();
}

FReply SGraphNodeMultipleCaseBranch::OnAddPin()
{
	UK2Node_MultipleCaseBranch* MultipleCaseBranch = CastChecked<UK2Node_MultipleCaseBranch>(GraphNode);

	const FScopedTransaction Transaction(NSLOCTEXT("Kismet", "AddExecutionPin", "Add Execution Pin"));
	MultipleCaseBranch->Modify();

	MultipleCaseBranch->AddInputPin();
	FBlueprintEditorUtils::MarkBlueprintAsModified(MultipleCaseBranch->GetBlueprint());

	UpdateGraphNode();
	GraphNode->GetGraph()->NotifyGraphChanged();

	return FReply::Handled();
}
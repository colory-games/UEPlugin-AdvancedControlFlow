/*!
 * AdvancedControlFlow
 *
 * Copyright (c) 2022-2023 Colory Games
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "SGraphNodeMultiBranch.h"

#include "K2Node_MultiBranch.h"
#include "KismetPins/SGraphPinExec.h"
#include "NodeFactory.h"

class SGraphPinExecMultiBranch : public SGraphPinExec
{
public:
	SLATE_BEGIN_ARGS(SGraphPinExecMultiBranch)
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
			.Padding(1.0f)[
#if UE_VERSION_NEWER_THAN(5, 1, 0)
				SNew(SImage).Image(FAppStyle::GetBrush("Graph.Pin.DefaultPinSeparator"))
#else
				SNew(SImage).Image(FEditorStyle::GetBrush("Graph.Pin.DefaultPinSeparator"))
#endif
		];

		TSharedPtr<SGraphPin> NewPin = SNew(SGraphPinExecMultiBranch, DefaultPin);
		this->AddPin(NewPin.ToSharedRef());
	}
}
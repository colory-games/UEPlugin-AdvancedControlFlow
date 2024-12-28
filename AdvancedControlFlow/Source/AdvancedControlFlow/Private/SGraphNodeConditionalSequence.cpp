/*!
 * AdvancedControlFlow
 *
 * Copyright (c) 2022-2023 Colory Games
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "SGraphNodeConditionalSequence.h"

#include "K2Node_ConditionalSequence.h"
#include "KismetPins/SGraphPinExec.h"
#include "NodeFactory.h"

class SGraphPinExecConditionalSequence : public SGraphPinExec
{
public:
	SLATE_BEGIN_ARGS(SGraphPinExecConditionalSequence)
	{
	}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
	{
		SGraphPin::Construct(SGraphPin::FArguments().PinLabelStyle(FName("Graph.Node.DefaultPinName")), InPin);

		CachePinIcons();
	}
};

void SGraphNodeConditionalSequence::Construct(const FArguments& InArgs, UK2Node_ConditionalSequence* InNode)
{
	this->GraphNode = InNode;
	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
}

void SGraphNodeConditionalSequence::CreatePinWidgets()
{
	UK2Node_ConditionalSequence* ConditionalSequence = CastChecked<UK2Node_ConditionalSequence>(GraphNode);
	UEdGraphPin* DefaultPin = ConditionalSequence->GetDefaultExecPin();

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

		TSharedPtr<SGraphPin> NewPin = SNew(SGraphPinExecConditionalSequence, DefaultPin);
		this->AddPin(NewPin.ToSharedRef());
	}
}
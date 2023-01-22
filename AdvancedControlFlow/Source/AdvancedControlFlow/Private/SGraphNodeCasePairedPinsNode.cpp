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

#include "DetailLayoutBuilder.h"
#include "EditorStyleSet.h"
#include "GraphEditorSettings.h"
#include "K2Node_CasePairedPinsNode.h"
#include "Kismet2/BlueprintEditorUtils.h"

void SGraphNodeCasePairedPinsNode::Construct(const FArguments& InArgs, UK2Node_CasePairedPinsNode* InNode)
{
	this->GraphNode = InNode;
	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
}

void SGraphNodeCasePairedPinsNode::CreateOutputSideAddButton(TSharedPtr<SVerticalBox> OutputBox)
{
	UK2Node_CasePairedPinsNode* CasePairedPinsNode = CastChecked<UK2Node_CasePairedPinsNode>(GraphNode);

#ifdef ACF_FREE_VERSION
	if (CasePairedPinsNode->GetCasePinCount() >= 3)
	{
		FMargin Padding = Settings->GetOutputPinPadding();
		Padding.Top += 6.0f;
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

EVisibility SGraphNodeCasePairedPinsNode::IsAddPinButtonVisible() const
{
	return SGraphNode::IsAddPinButtonVisible();
}

FReply SGraphNodeCasePairedPinsNode::OnAddPin()
{
	UK2Node_CasePairedPinsNode* CasePairedPinsNode = CastChecked<UK2Node_CasePairedPinsNode>(GraphNode);

	// TODO: Use NSLOCTEXT macro
	const FScopedTransaction Transaction(FText::AsCultureInvariant("Add Execution Pin"));
	CasePairedPinsNode->Modify();

	CasePairedPinsNode->AddCasePinLast();
	FBlueprintEditorUtils::MarkBlueprintAsModified(CasePairedPinsNode->GetBlueprint());

	UpdateGraphNode();
	GraphNode->GetGraph()->NotifyGraphChanged();

	return FReply::Handled();
}
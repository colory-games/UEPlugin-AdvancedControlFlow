#include "MultipleCaseBranchModule.h"

#include "EdGraphUtilities.h"
#include "K2Node_MultipleCaseBranch.h"
#include "SGraphNodeMultipleCaseBranch.h"

#define LOCTEXT_NAMESPACE "FMultipleCaseBranchModule"

class FGraphPanelNodeFactory_MultipleCaseBranch : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<SGraphNode> CreateNode(UEdGraphNode* Node) const override
	{
		if (UK2Node_MultipleCaseBranch* MultipleCaseBranch = Cast<UK2Node_MultipleCaseBranch>(Node))
		{
			return SNew(SGraphNodeMultipleCaseBranch, MultipleCaseBranch);
		}

		return nullptr;
	}
};

void FMultipleCaseBranchModule::StartupModule()
{
	GraphPanelNodeFactory_MultipleCaseBranch = MakeShareable(new FGraphPanelNodeFactory_MultipleCaseBranch());
	FEdGraphUtilities::RegisterVisualNodeFactory(GraphPanelNodeFactory_MultipleCaseBranch);
}

void FMultipleCaseBranchModule::ShutdownModule()
{
	if (GraphPanelNodeFactory_MultipleCaseBranch.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualNodeFactory(GraphPanelNodeFactory_MultipleCaseBranch);
		GraphPanelNodeFactory_MultipleCaseBranch.Reset();
	}
}

bool FMultipleCaseBranchModule::SupportsDynamicReloading()
{
	return true;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMultipleCaseBranchModule, MultipleCaseBranch);

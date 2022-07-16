#include "AdvancedControlFlowModule.h"

#include "EdGraphUtilities.h"
#include "K2Node_MultiBranch.h"
#include "SGraphNodeMultiBranch.h"

#define LOCTEXT_NAMESPACE "FAdvancedControlFlowModule"

class FGraphPanelNodeFactory_AdvancedControlFlow : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<SGraphNode> CreateNode(UEdGraphNode* Node) const override
	{
		if (UK2Node_MultiBranch* MultiBranch = Cast<UK2Node_MultiBranch>(Node))
		{
			return SNew(SGraphNodeMultiBranch, MultiBranch);
		}

		return nullptr;
	}
};

void FAdvancedControlFlowModule::StartupModule()
{
	GraphPanelNodeFactory_AdvancedControlFlow = MakeShareable(new FGraphPanelNodeFactory_AdvancedControlFlow());
	FEdGraphUtilities::RegisterVisualNodeFactory(GraphPanelNodeFactory_AdvancedControlFlow);
}

void FAdvancedControlFlowModule::ShutdownModule()
{
	if (GraphPanelNodeFactory_AdvancedControlFlow.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualNodeFactory(GraphPanelNodeFactory_AdvancedControlFlow);
		GraphPanelNodeFactory_AdvancedControlFlow.Reset();
	}
}

bool FAdvancedControlFlowModule::SupportsDynamicReloading()
{
	return true;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAdvancedControlFlowModule, AdvancedControlFlow);

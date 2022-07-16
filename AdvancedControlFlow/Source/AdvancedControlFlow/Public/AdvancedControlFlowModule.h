#pragma once

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FAdvancedControlFlowModule"

class FGraphPanelNodeFactory_AdvancedControlFlow;

class FAdvancedControlFlowModule : public IModuleInterface
{
	TSharedPtr<FGraphPanelNodeFactory_AdvancedControlFlow> GraphPanelNodeFactory_AdvancedControlFlow;

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool SupportsDynamicReloading() override;
};

#undef LOCTEXT_NAMESPACE

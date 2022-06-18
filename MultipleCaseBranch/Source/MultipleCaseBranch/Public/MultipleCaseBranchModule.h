#pragma once

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FMultipleCaseBranchModule"

class FGraphPanelNodeFactory_MultipleCaseBranch;

class FMultipleCaseBranchModule : public IModuleInterface
{
	TSharedPtr<FGraphPanelNodeFactory_MultipleCaseBranch> GraphPanelNodeFactory_MultipleCaseBranch;

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool SupportsDynamicReloading() override;
};

#undef LOCTEXT_NAMESPACE

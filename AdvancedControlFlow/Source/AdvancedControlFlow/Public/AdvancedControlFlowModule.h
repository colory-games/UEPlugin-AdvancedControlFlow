/*!
 * AdvancedControlFlow
 *
 * Copyright (c) 2022 Colory Games
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "AdvancedControlFlow"

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

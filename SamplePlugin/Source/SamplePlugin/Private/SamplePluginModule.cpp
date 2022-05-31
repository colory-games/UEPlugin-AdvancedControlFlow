#include "SamplePluginModule.h"

#define LOCTEXT_NAMESPACE "FSamplePluginModule"

void FSamplePluginModule::StartupModule()
{
}

void FSamplePluginModule::ShutdownModule()
{
}

bool FSamplePluginModule::SupportsDynamicReloading()
{
	return true;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSamplePluginModule, SamplePlugin);

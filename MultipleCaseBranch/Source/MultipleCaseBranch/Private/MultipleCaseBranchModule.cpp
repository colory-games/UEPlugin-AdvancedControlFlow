#include "MultipleCaseBranchModule.h"

#define LOCTEXT_NAMESPACE "FMultipleCaseBranchModule"

void FMultipleCaseBranchModule::StartupModule()
{
}

void FMultipleCaseBranchModule::ShutdownModule()
{
}

bool FMultipleCaseBranchModule::SupportsDynamicReloading()
{
	return true;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMultipleCaseBranchModule, MultipleCaseBranch);

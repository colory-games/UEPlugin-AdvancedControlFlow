#include "MultipleCaseBranchEditorModule.h"

#define LOCTEXT_NAMESPACE "FMultipleCaseBranchEditorModule"

void FMultipleCaseBranchEditorModule::StartupModule()
{
}

void FMultipleCaseBranchEditorModule::ShutdownModule()
{
}

bool FMultipleCaseBranchEditorModule::SupportsDynamicReloading()
{
	return true;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMultipleCaseBranchEditorModule, MultipleCaseBranchEditor);

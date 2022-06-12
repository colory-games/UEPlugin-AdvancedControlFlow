// Some copyright should be here...

using UnrealBuildTool;

public class MultipleCaseBranchEditor : ModuleRules
{
	public MultipleCaseBranchEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]{
			"MultipleCaseBranch",
		});

		PrivateDependencyModuleNames.AddRange(new string[]{
			"UnrealEd",
		});
	}
}

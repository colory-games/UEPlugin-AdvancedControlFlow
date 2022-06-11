// Some copyright should be here...

using UnrealBuildTool;

public class MultipleCaseBranch : ModuleRules
{
	public MultipleCaseBranch(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]{
			"Core",
		});

		PrivateDependencyModuleNames.AddRange(new string[]{});
	}
}

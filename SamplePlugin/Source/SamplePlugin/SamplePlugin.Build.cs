// Some copyright should be here...

using UnrealBuildTool;

public class SamplePlugin : ModuleRules
{
	public SamplePlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]{
			"Core",
		});

		PrivateDependencyModuleNames.AddRange(new string[]{});
	}
}

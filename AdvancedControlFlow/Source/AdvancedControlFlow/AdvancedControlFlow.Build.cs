// Some copyright should be here...

using UnrealBuildTool;

public class AdvancedControlFlow : ModuleRules
{
	public AdvancedControlFlow(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]{
			"Core",
			"CoreUObject",
			"Engine",
		});

		PrivateDependencyModuleNames.AddRange(new string[]{
			"BlueprintGraph",
			"EditorStyle",
			"GraphEditor",
			"KismetCompiler",
			"Slate",
			"SlateCore",
			"ToolMenus",
			"UnrealEd",
		});
	}
}

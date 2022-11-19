/*!
 * AdvancedControlFlow
 *
 * Copyright (c) 2022 Colory Games
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

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

		// @remove-start FULL_VERSION=true
		PublicDefinitions.Add("ACF_FREE_VERSION");
		// @remove-end
	}
}

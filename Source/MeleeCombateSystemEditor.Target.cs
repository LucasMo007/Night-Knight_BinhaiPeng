using UnrealBuildTool;

public class MeleeCombateSystemEditorTarget : TargetRules
{
	public MeleeCombateSystemEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("MeleeCombateSystem");
	}
}

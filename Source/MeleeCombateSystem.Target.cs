using UnrealBuildTool;

public class MeleeCombateSystemTarget : TargetRules
{
	public MeleeCombateSystemTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("MeleeCombateSystem");
	}
}

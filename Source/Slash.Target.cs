using UnrealBuildTool;
using System.Collections.Generic;

public class SlashTarget : TargetRules
{
    public SlashTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        CppStandard = CppStandardVersion.Cpp20;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        // 빌드 환경 강제 오버라이드
        bOverrideBuildEnvironment = true;

        ExtraModuleNames.Add("Slash");
    }
}

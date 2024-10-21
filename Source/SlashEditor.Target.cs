using UnrealBuildTool;
using System.Collections.Generic;

public class SlashEditorTarget : TargetRules
{
    public SlashEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		CppStandard = CppStandardVersion.Cpp20;

        // 빌드 환경 강제 오버라이드
        bOverrideBuildEnvironment = true;

        ExtraModuleNames.Add("Slash");
    }
}
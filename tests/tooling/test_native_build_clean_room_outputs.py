from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"


def test_contract_source_output_reports_only_selected_generated_packets() -> None:
    build_script = BUILD_SCRIPT.read_text(encoding="utf-8")

    assert "$selectedFrontendPacketDefinitions = @(" in build_script
    assert "Get-Objc3cNativeSelectedFrontendPacketDefinitions `" in build_script
    assert "foreach ($packetDefinition in $selectedFrontendPacketDefinitions)" in build_script
    assert "foreach ($packetDefinition in $frontendPacketDefinitions)" not in build_script


def test_binary_output_lines_are_gated_to_native_build_modes() -> None:
    build_script = BUILD_SCRIPT.read_text(encoding="utf-8")

    output_section = build_script.split(
        "Write-Objc3cNativeRepoSupercleanSourceOfTruthArtifact", maxsplit=1
    )[1]
    built_line = (
        'Write-Output ("built=" + '
        "(Get-Objc3cNativeBuildRepoRelativePath -RootPath $repoRoot -TargetPath $outExe))"
    )

    assert "if (Test-ExecutionModeRunsNativeBuild -Mode $ExecutionMode)" in output_section
    assert built_line in output_section
    assert output_section.index(
        "if (Test-ExecutionModeRunsNativeBuild -Mode $ExecutionMode)"
    ) < output_section.index(built_line)

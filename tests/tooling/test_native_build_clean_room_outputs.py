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


def test_contract_source_mode_skips_native_toolchain_resolution() -> None:
    build_script = BUILD_SCRIPT.read_text(encoding="utf-8")

    assert "$modeRunsNativeBuild = Test-ExecutionModeRunsNativeBuild -Mode $ExecutionMode" in build_script
    assert "if ($modeRunsNativeBuild)" in build_script
    assert "$nativeToolchain = Resolve-Objc3cNativeToolchain -RepoRoot $repoRoot" in build_script
    assert 'Write-BuildStep "toolchain_resolution=skipped-source-contracts"' in build_script


def test_repo_relative_output_uses_host_neutral_path_api() -> None:
    build_script = BUILD_SCRIPT.read_text(encoding="utf-8")

    assert "[System.IO.Path]::GetRelativePath($resolvedRoot, $resolvedTarget)" in build_script
    assert "[System.Uri]::new(($resolvedRoot + '\\'))" not in build_script


def test_clean_room_roots_route_native_and_frontend_outputs() -> None:
    build_script = BUILD_SCRIPT.read_text(encoding="utf-8")

    assert "[string]$CleanRoomRoot = \"\"" in build_script
    assert "[string]$BuildDir = \"\"" in build_script
    assert "[string]$RuntimeOutputDir = \"\"" in build_script
    assert "[string]$LibraryOutputDir = \"\"" in build_script
    assert "[string]$FrontendArtifactRoot = \"\"" in build_script
    assert "[int]$Parallelism = 0" in build_script
    assert "if ($Parallelism -eq 0) {" in build_script
    assert "$Parallelism = 4" in build_script
    assert "-CleanRoomRoot $resolvedCleanRoomRoot `" in build_script
    assert "-BuildDir $BuildDir `" in build_script
    assert "-RuntimeOutputDir $RuntimeOutputDir `" in build_script
    assert "-LibraryOutputDir $LibraryOutputDir" in build_script
    assert 'Join-Path $resolvedCleanRoomRoot "artifacts/frontend-contracts"' in build_script
    assert (
        "$frontendArtifactPaths = Get-Objc3cNativeFrontendArtifactPaths "
        "-RepoRoot $repoRoot -ArtifactRoot $resolvedFrontendArtifactRoot"
    ) in build_script


def test_native_build_writes_reproducible_summary_manifest() -> None:
    build_script = BUILD_SCRIPT.read_text(encoding="utf-8")

    assert "[string]$SummaryPath = \"\"" in build_script
    assert '$sourceDateEpoch = "1704067200"' in build_script
    assert "$env:SOURCE_DATE_EPOCH = $sourceDateEpoch" in build_script
    assert 'contract_id = "objc3c-native-bootstrap-reproducible-build-v1"' in build_script
    assert "parallelism = $Parallelism" in build_script
    assert "source_date_epoch = $SourceDateEpoch" in build_script
    assert "function Get-Objc3cNativeBuildFileDigest" in build_script
    assert "Get-FileHash -LiteralPath $TargetPath -Algorithm SHA256" in build_script
    assert "function Write-Objc3cNativeBuildSummary" in build_script
    assert "runtime_archive_timestamps_normalized = $RuntimeArchiveNormalized" in build_script
    assert "normalize_coff_archive_timestamps.py" in build_script
    assert "native_build_summary=" in build_script


def test_cmake_reproducible_build_policy_is_fingerprinted() -> None:
    cmake_lists = (ROOT / "native" / "objc3c" / "CMakeLists.txt").read_text(
        encoding="utf-8"
    )
    toolchain_module = (
        ROOT / "scripts" / "objc3c_native_cmake" / "toolchain.psm1"
    ).read_text(encoding="utf-8")
    configure_module = (
        ROOT / "scripts" / "objc3c_native_cmake" / "configure.psm1"
    ).read_text(encoding="utf-8")
    fingerprint_module = (
        ROOT / "scripts" / "objc3c_native_cmake" / "fingerprint.psm1"
    ).read_text(encoding="utf-8")

    assert "OBJC3C_ENABLE_REPRODUCIBLE_BUILD" in cmake_lists
    assert "/Brepro" in cmake_lists
    assert "-ffile-prefix-map=${CMAKE_SOURCE_DIR}=." in cmake_lists
    assert "-fdebug-prefix-map=${CMAKE_SOURCE_DIR}=." in cmake_lists
    assert '"bin\\llvm-ar.exe"' in toolchain_module
    assert '"bin\\llvm-ranlib.exe"' in toolchain_module
    assert "LlvmArTool = $llvmArTool" in toolchain_module
    assert "LlvmRanlibTool = $llvmRanlibTool" in toolchain_module
    assert "-DCMAKE_AR=$LlvmArTool" in configure_module
    assert "-DCMAKE_RANLIB=$LlvmRanlibTool" in configure_module
    assert "-DOBJC3C_ENABLE_REPRODUCIBLE_BUILD=ON" in configure_module
    assert "llvm_ar = $LlvmArTool" in fingerprint_module
    assert "llvm_ranlib = $LlvmRanlibTool" in fingerprint_module
    assert "llvm_lib = $LlvmLibTool" in fingerprint_module
    assert "reproducible_build = $true" in fingerprint_module
    assert "source_date_epoch = $SourceDateEpoch" in fingerprint_module
    assert "cmake_build_parallelism=" in (
        ROOT / "scripts" / "objc3c_native_cmake" / "build.psm1"
    ).read_text(encoding="utf-8")
    assert "--parallel --target" not in (
        ROOT / "scripts" / "objc3c_native_cmake" / "build.psm1"
    ).read_text(encoding="utf-8")


def test_binary_output_lines_are_gated_to_native_build_modes() -> None:
    build_script = BUILD_SCRIPT.read_text(encoding="utf-8")

    output_section = build_script.split(
        "Write-Objc3cNativeRepoSupercleanSourceOfTruthArtifact", maxsplit=1
    )[1]
    built_line = (
        'Write-Output ("built=" + '
        "(Get-Objc3cNativeBuildRepoRelativePath -RootPath $repoRoot -TargetPath $outExe))"
    )

    assert "if ($modeRunsNativeBuild)" in output_section
    assert built_line in output_section
    assert output_section.index("if ($modeRunsNativeBuild)") < output_section.index(
        built_line
    )

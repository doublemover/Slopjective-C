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


def test_native_binary_builds_are_serialized_per_build_directory() -> None:
    build_script = BUILD_SCRIPT.read_text(encoding="utf-8")

    assert "function Enter-Objc3cNativeBuildDirectoryLock" in build_script
    assert "function Exit-Objc3cNativeBuildDirectoryLock" in build_script
    assert '"OBJC3C_NATIVE_BUILD_LOCK_TIMEOUT_SECONDS must be a positive integer when set"' in build_script
    assert 'Join-Path $BuildDirPath ".objc3c-native-build.lock"' in build_script
    assert "[System.IO.FileShare]::None" in build_script
    assert "native_build_lock_acquired=" in build_script
    assert "native_build_lock_released=" in build_script
    assert "native_build_lock_wait_seconds=" in build_script
    assert "native_build_lock = " in build_script
    assert "$nativeBuildLockState = Enter-Objc3cNativeBuildDirectoryLock `" in build_script
    assert "finally {\n    Exit-Objc3cNativeBuildDirectoryLock -LockState $nativeBuildLockState\n  }" in build_script


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
    assert "function Get-Objc3cNativeToolExecutableName" in toolchain_module
    assert 'return $CommandName + ".exe"' in toolchain_module
    assert 'Join-Path (Join-Path $LlvmRoot "bin")' in toolchain_module
    assert (
        'Join-Objc3cNativeLlvmToolPath -LlvmRoot $llvmRoot -CommandName "llvm-ar"'
        in toolchain_module
    )
    assert (
        'Join-Objc3cNativeLlvmToolPath -LlvmRoot $llvmRoot -CommandName "llvm-ranlib"'
        in toolchain_module
    )
    assert "LlvmArTool = $llvmArTool" in toolchain_module
    assert "LlvmRanlibTool = $llvmRanlibTool" in toolchain_module
    assert "-DCMAKE_AR=$LlvmArTool" in configure_module
    assert "-DCMAKE_RANLIB=$LlvmRanlibTool" in configure_module
    assert "-DCMAKE_BUILD_TYPE=RelWithDebInfo" in configure_module
    assert "set(CMAKE_BUILD_TYPE RelWithDebInfo" in cmake_lists
    assert "-DOBJC3C_ENABLE_REPRODUCIBLE_BUILD=ON" in configure_module
    assert "llvm_ar = $LlvmArTool" in fingerprint_module
    assert "llvm_ranlib = $LlvmRanlibTool" in fingerprint_module
    assert "llvm_lib = $LlvmLibTool" in fingerprint_module
    assert 'build_type = "RelWithDebInfo"' in fingerprint_module
    assert "reproducible_build = $true" in fingerprint_module
    assert "source_date_epoch = $SourceDateEpoch" in fingerprint_module
    assert "cmake_build_parallelism=" in (
        ROOT / "scripts" / "objc3c_native_cmake" / "build.psm1"
    ).read_text(encoding="utf-8")
    assert "--parallel --target" not in (
        ROOT / "scripts" / "objc3c_native_cmake" / "build.psm1"
    ).read_text(encoding="utf-8")


def test_runnable_package_includes_native_cmake_support_modules() -> None:
    inventory = (
        ROOT
        / "scripts"
        / "objc3c_runnable_toolchain_package_helpers"
        / "file_inventory.psm1"
    ).read_text(encoding="utf-8")

    assert '"scripts/objc3c_native_cmake.psm1"' in inventory
    assert '"scripts/objc3c_native_cmake"' in inventory
    assert '"scripts/normalize_coff_archive_timestamps.py"' in inventory
    assert '"scripts/objc3c_native_artifact_io.psm1"' in inventory
    assert '"scripts/objc3c_native_frontend_contracts.psm1"' in inventory
    assert '"scripts/objc3c_native_frontend_contracts"' in inventory
    assert '"scripts/objc3c_native_frontend_artifacts.psm1"' in inventory
    assert '"scripts/objc3c_native_frontend_artifacts"' in inventory
    assert '"scripts/objc3c_native_frontend_closeout_artifacts.psm1"' in inventory
    assert '"scripts/objc3c_native_frontend_closeout_edge_artifacts.psm1"' in inventory
    assert '"scripts/objc3c_native_frontend_closeout_edge_artifacts"' in inventory
    assert (
        '"scripts/objc3c_native_frontend_closeout_conformance_artifacts.psm1"'
        in inventory
    )
    assert '"scripts/objc3c_native_frontend_closeout_conformance_artifacts"' in inventory
    assert '"scripts/objc3c_native_superclean_surface.psm1"' in inventory
    assert '"scripts/objc3c_native_superclean_surface_catalog.psm1"' in inventory
    assert '"scripts/objc3c_native_superclean_surface_catalog"' in inventory


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

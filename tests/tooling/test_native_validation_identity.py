"""Snapshot boundaries use small synthetic toolchains, never compiler builds."""

from __future__ import annotations

import copy
import hashlib
import json
import os
from pathlib import Path

import pytest

from scripts.objc3c_native_validation import identity


def write(path: Path, content: str | bytes) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(content.encode("utf-8") if isinstance(content, str) else content)


@pytest.fixture
def toolchain(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> dict:
    repo = tmp_path / "repo"
    sdk = repo / "out/toolchains/objc3-development/build"
    stage = repo / "out/toolchains/libobjc2-v2.3/build/stage"
    source = repo / "out/toolchains/libobjc2-v2.3/src"
    robin = repo / "out/toolchains/robin-map-v1.4.0/src"
    robin.mkdir(parents=True)
    patch_digest = "a" * 64
    upstream = "b" * 40
    marker = "ObjC3-patches=" + patch_digest
    seal = {"build_id": "synthetic-sdk-build-id", "patch_digest": patch_digest,
            "upstream_commit": upstream, "source_kind": "development", "tree_sha256": "c" * 40,
            "patches": [{"file": "native.patch", "sha256": "d" * 64}]}
    manifest = {"patch_series": "toolchains/llvm/patches/series"}
    profile = {"target_triple": "x86_64-pc-windows-msvc", "v143_toolset_version": "14.44.35207",
               "windows_sdk_version": "10.0.26100.0", "runtime_commit": "e" * 40,
               "robin_map_commit": "f" * 40}
    write(repo / "toolchains/llvm/manifest.json", json.dumps(manifest))
    write(repo / "toolchains/llvm/patches/series", "native.patch\n")
    write(repo / "toolchains/llvm/patches/native.patch", "tiny LLVM patch")
    write(repo / "scripts/objc3c_toolchain_runtime/runtime-profile.json",
          json.dumps({"profiles": {"windows-x86_64": profile}}))
    write(repo / "out/toolchains/objc3-development/src/.objc3c-prepared-source.json", json.dumps(seal))
    write(sdk / "tools/clang/include/clang/Basic/ObjC3Foundation.h", patch_digest + upstream)
    write(sdk / "lib/clang/23/include/stddef.h", "typedef long ptrdiff_t;\n")
    write(sdk / "lib/clang/23/lib/windows/builtins.lib", "resource builtin archive")
    for filename in ("clang.exe", "clang++.exe", "clang-cl.exe", "clangd.exe", "clang-format.exe"):
        write(sdk / "bin" / filename, "synthetic " + filename + marker + upstream)
    write(sdk / "bin/lld-link.exe", "synthetic LLD " + upstream)
    write(source / "objc/objc.h", "runtime public API")
    write(source / "Block.h", "embedded Blocks API")
    write(stage / "objc/objc-config.h", "generated runtime config")
    write(stage / "objc.dll", "native runtime DLL")
    write(stage / "objc.lib", "native runtime import library")
    include = tmp_path / "admitted/include"
    libraries = tmp_path / "admitted/lib/x64"
    write(include / "vector", "C++ standard header without a filename suffix")
    write(libraries / "msvcrt.lib", "admitted dynamic CRT import library")
    system = tmp_path / "Windows"
    for name in ("ucrtbase.dll", "vcruntime140.dll", "vcruntime140_1.dll", "msvcp140.dll"):
        write(system / "System32" / name, "selected CRT " + name)
    environment = {"VCToolsVersion": profile["v143_toolset_version"],
                   "WindowsSDKVersion": profile["windows_sdk_version"],
                   "VCToolsInstallDir": str(tmp_path / "admitted"), "WindowsSdkDir": str(system),
                   "INCLUDE": str(include), "LIB": str(libraries), "PATH": str(sdk / "bin"),
                   "SystemRoot": str(system)}
    monkeypatch.setattr(identity, "_host_platform", lambda: "windows-x86_64")
    monkeypatch.setattr(identity, "_capture_environment", lambda _: dict(environment))
    seal_calls = []

    def verify(repo_path: Path) -> dict:
        seal_calls.append(repo_path)
        return copy.deepcopy(seal)

    def run(command: list[str]) -> str:
        if command[-1] == "--version":
            return Path(command[0]).read_text(encoding="utf-8")
        assert command[:2] == ["git", "-C"] and command[-2:] == ["rev-parse", "HEAD"]
        return profile["runtime_commit"] if command[2] == str(source) else profile["robin_map_commit"]

    monkeypatch.setattr(identity, "verify_development", verify)
    monkeypatch.setattr(identity, "_run", run)
    monkeypatch.setattr(identity, "facade_identity", lambda _: {"sha256": "9" * 64})
    inputs = identity._Inputs()
    tree = inputs.add_tree(sdk / "lib/clang/23")
    resource_digest = identity._resource_digest(tree, inputs.files)
    driver_digest = hashlib.sha256((sdk / "bin/clang.exe").read_bytes()).hexdigest()
    facade = repo / "out/toolchains/objc3-facade/build/src/clang_foundation/objc3c-clang.exe"
    write(facade, "synthetic facade " + marker + upstream + driver_digest + resource_digest + "9" * 64)
    return {"repo": repo, "sdk": sdk, "stage": stage, "source": source, "facade": facade,
            "seal": seal, "profile": profile, "environment": environment, "seal_calls": seal_calls}


def test_snapshot_is_actual_stable_inputs_and_family_independent(toolchain: dict) -> None:
    before_environment = dict(os.environ)
    snapshot = identity.resolve_toolchain(toolchain["repo"])
    editor_snapshot = identity.resolve_toolchain(toolchain["repo"], require_editor=True)
    assert snapshot == editor_snapshot
    assert snapshot["sdk_build_id"] == toolchain["seal"]["build_id"]
    assert snapshot["build_identity"] != snapshot["sdk_build_id"]
    assert len(snapshot["build_identity"]) == 64
    assert set(snapshot["tools"]) == {"clang", "clangxx", "clang_cl", "facade", "linker", "clangd", "clang_format"}
    assert all(Path(path).is_absolute() for path in snapshot["tools"].values())
    assert snapshot["environment"] == toolchain["environment"]
    assert snapshot["runtime"]["include_dirs"] == [str(toolchain["source"]), str(toolchain["stage"] / "objc")]
    assert snapshot["runtime"]["import_library"] == str(toolchain["stage"] / "objc.lib")
    paths = [entry["path"] for entry in snapshot["input_files"]]
    assert paths == sorted(set(paths))
    assert str(Path(toolchain["environment"]["INCLUDE"]) / "vector") in paths
    assert str(toolchain["stage"] / "objc.dll") in paths
    assert str(toolchain["stage"] / "objc.lib") in paths
    identity.verify_toolchain_snapshot(snapshot)
    assert len(toolchain["seal_calls"]) == 3  # two resolutions and one parent verification
    assert dict(os.environ) == before_environment


@pytest.mark.parametrize("relative", ["bin/lld-link.exe", "lib/clang/23/include/stddef.h"])
def test_actual_binary_or_resource_mutation_invalidates_snapshot(toolchain: dict, relative: str) -> None:
    snapshot = identity.resolve_toolchain(toolchain["repo"])
    path = toolchain["sdk"] / relative
    path.write_bytes(path.read_bytes() + b" changed actual bytes")
    with pytest.raises(identity.NativeToolchainError, match="input changed"):
        identity.verify_toolchain_snapshot(snapshot)


def test_runtime_dll_change_changes_batch_identity(toolchain: dict) -> None:
    before = identity.resolve_toolchain(toolchain["repo"])
    write(toolchain["stage"] / "objc.dll", "different native runtime DLL")
    after = identity.resolve_toolchain(toolchain["repo"])
    assert before["sdk_build_id"] == after["sdk_build_id"]
    assert before["build_identity"] != after["build_identity"]
    with pytest.raises(identity.NativeToolchainError, match="input changed"):
        identity.verify_toolchain_snapshot(before)


@pytest.mark.parametrize("operation", ["add", "delete", "rename"])
def test_tree_inventory_detects_new_missing_and_renamed_resources(toolchain: dict, operation: str) -> None:
    snapshot = identity.resolve_toolchain(toolchain["repo"])
    header = toolchain["sdk"] / "lib/clang/23/include/stddef.h"
    if operation == "add":
        write(header.with_name("new-resource.h"), "new compiler resource")
    elif operation == "delete":
        header.unlink()
    else:
        header.rename(header.with_name("renamed-resource.h"))
    with pytest.raises(identity.NativeToolchainError, match="tree membership changed"):
        identity.verify_toolchain_snapshot(snapshot)


def test_stale_facade_rejected_even_when_patch_marker_matches(toolchain: dict) -> None:
    driver = toolchain["sdk"] / "bin/clang.exe"
    driver.write_bytes(driver.read_bytes() + b" same-patch recompiled driver")
    with pytest.raises(identity.NativeToolchainError, match="stale or unsealed native facade"):
        identity.resolve_toolchain(toolchain["repo"])


def test_observable_version_marker_may_use_separate_binary_literals(
    toolchain: dict, monkeypatch: pytest.MonkeyPatch,
) -> None:
    driver = toolchain["sdk"] / "bin/clang.exe"
    old_digest = hashlib.sha256(driver.read_bytes()).hexdigest()
    seal = toolchain["seal"]
    write(driver, seal["upstream_commit"] + "\0ObjC3-patches=\0" + seal["patch_digest"])
    new_digest = hashlib.sha256(driver.read_bytes()).hexdigest()
    write(toolchain["facade"], toolchain["facade"].read_bytes().replace(old_digest.encode(), new_digest.encode()))
    original_run = identity._run

    def run(command: list[str]) -> str:
        if command == [str(driver), "--version"]:
            return "clang version " + seal["upstream_commit"] + " ObjC3-patches=" + seal["patch_digest"]
        return original_run(command)

    monkeypatch.setattr(identity, "_run", run)
    snapshot = identity.resolve_toolchain(toolchain["repo"])
    assert next(entry["sha256"] for entry in snapshot["input_files"] if entry["path"] == str(driver)) == new_digest


def test_missing_facade_and_unsealed_driver_have_clear_errors(toolchain: dict) -> None:
    toolchain["facade"].unlink()
    with pytest.raises(identity.NativeToolchainError, match="missing native facade"):
        identity.resolve_toolchain(toolchain["repo"])
    write(toolchain["sdk"] / "bin/clang.exe", "upstream-only compiler")
    with pytest.raises(identity.NativeToolchainError, match="stale or unsealed clang"):
        identity.resolve_toolchain(toolchain["repo"])


@pytest.mark.parametrize("filename", ["clangd.exe", "clang-format.exe"])
@pytest.mark.parametrize("state", ["missing", "stale"])
def test_optional_editor_admission_only_blocks_when_required(toolchain: dict, filename: str, state: str) -> None:
    path = toolchain["sdk"] / "bin" / filename
    if state == "missing":
        path.unlink()
    else:
        write(path, "older editor patch identity")
    snapshot = identity.resolve_toolchain(toolchain["repo"])
    key = "clangd" if filename == "clangd.exe" else "clang_format"
    assert key not in snapshot["tools"]
    assert snapshot["editor_tools"][key]["status"] == "unavailable"
    with pytest.raises(identity.NativeToolchainError, match="required editor tool rejected"):
        identity.resolve_toolchain(toolchain["repo"], require_editor=True)


def test_metadata_and_seal_mutation_are_rejected(toolchain: dict) -> None:
    snapshot = identity.resolve_toolchain(toolchain["repo"])
    changed = copy.deepcopy(snapshot)
    changed["tools"]["clang"] = "another compiler"
    with pytest.raises(identity.NativeToolchainError, match="snapshot metadata"):
        identity.verify_toolchain_snapshot(changed)
    toolchain["seal"]["tree_sha256"] = "0" * 40
    with pytest.raises(identity.NativeToolchainError, match="source seal changed"):
        identity.verify_toolchain_snapshot(snapshot)


def test_private_devshell_captures_environment_without_global_changes(
    tmp_path: Path, monkeypatch: pytest.MonkeyPatch,
) -> None:
    programs = tmp_path / "programs"
    installation = tmp_path / "VS2022"
    write(programs / "Microsoft Visual Studio/Installer/vswhere.exe", "discovery executable")
    write(installation / "Common7/Tools/Microsoft.VisualStudio.DevShell.dll", "private admission module")
    monkeypatch.setenv("ProgramFiles(x86)", str(programs))
    monkeypatch.delenv("VCToolsVersion", raising=False)
    commands = []
    expected = {"PATH": "private toolset path", "INCLUDE": "private headers", "LIB": "private libraries"}

    def run(command: list[str]) -> str:
        commands.append(command)
        return str(installation) if command[0].endswith("vswhere.exe") else (
            "Developer PowerShell banner\nOBJC3_ENV_JSON:" + json.dumps(expected))

    monkeypatch.setattr(identity, "_run", run)
    monkeypatch.setattr(identity.shutil, "which", lambda _: "powershell-child")
    before = dict(os.environ)
    captured = identity._capture_environment({"v143_toolset_version": "14.44.35207",
                                              "windows_sdk_version": "10.0.26100.0"})
    assert captured == expected
    assert commands[0][commands[0].index("-version") + 1] == "[17.0,18.0)"
    assert "-NoProfile" in commands[1]
    assert "Import-Module" in commands[1][-1] and "Enter-VsDevShell" in commands[1][-1]
    assert "-arch=x64 -host_arch=x64 -winsdk=10.0.26100.0 -vcvars_ver=14.44.35207" in commands[1][-1]
    assert dict(os.environ) == before


def test_stale_frontend_source_receipt_is_rejected(toolchain, monkeypatch):
    monkeypatch.setattr(identity, 'facade_identity', lambda _: {'sha256': '8' * 64})
    with pytest.raises(identity.NativeToolchainError, match='missing sealed marker'):
        identity.resolve_toolchain(toolchain['repo'])

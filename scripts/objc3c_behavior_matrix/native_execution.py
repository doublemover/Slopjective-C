"""Native compile, link, and run operations for behavior fixtures."""

from __future__ import annotations

import json
import shutil
from pathlib import Path

from objc3c_tooling.behavior_fixtures import BehaviorFixture
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.probe_compile import find_clangxx, normal_user_manifest_link_args
from objc3c_tooling.subprocesses import CommandExecution, bounded_text, run_timed

from .config import BUILD_SCRIPT, NATIVE_EXE, RUNTIME_LIB
from .errors import BehaviorMatrixFailure


def ensure_native_binaries() -> None:
    if NATIVE_EXE.is_file() and RUNTIME_LIB.is_file():
        return
    pwsh = shutil.which("pwsh") or "pwsh"
    result = run_timed(
        [
            pwsh,
            "-NoProfile",
            "-NonInteractive",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            BUILD_SCRIPT,
            "-ExecutionMode",
            "binaries-only",
        ],
        metadata={"stage": "build-native-binaries"},
    )
    if result.returncode != 0:
        raise BehaviorMatrixFailure(
            "native binary build failed before behavior matrix:\n"
            f"{bounded_text(result.stdout + result.stderr)}"
        )
    if not NATIVE_EXE.is_file():
        raise BehaviorMatrixFailure(f"native compiler missing after build: {repo_rel(NATIVE_EXE)}")
    if not RUNTIME_LIB.is_file():
        raise BehaviorMatrixFailure(f"runtime library missing after build: {repo_rel(RUNTIME_LIB)}")


def compile_fixture(fixture: BehaviorFixture, case_dir: Path) -> CommandExecution:
    out_dir = case_dir / "compile"
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_timed(
        [
            NATIVE_EXE,
            fixture.source_path,
            "--out-dir",
            out_dir,
            "--emit-prefix",
            "module",
            *fixture.native_compile_args,
        ],
        metadata={
            "stage": "compile",
            "fixture": fixture.relative_source,
        },
    )


def compile_output_text(result: CommandExecution, compile_dir: Path) -> str:
    diagnostics_path = compile_dir / "module.diagnostics.txt"
    diagnostics = diagnostics_path.read_text(encoding="utf-8") if diagnostics_path.is_file() else ""
    return "\n".join(part for part in (diagnostics, result.stdout, result.stderr) if part)


def object_path(compile_dir: Path) -> Path:
    obj_path = compile_dir / "module.obj"
    if not obj_path.is_file():
        raise BehaviorMatrixFailure(f"compile did not publish object artifact: {repo_rel(obj_path)}")
    return obj_path


def runtime_registration_manifest_path(compile_dir: Path) -> Path:
    manifest_path = compile_dir / "module.runtime-registration-manifest.json"
    if not manifest_path.is_file():
        raise BehaviorMatrixFailure(
            "compile did not publish runtime registration manifest: "
            f"{repo_rel(manifest_path)}"
        )
    return manifest_path


def runtime_driver_linker_flags(compile_dir: Path) -> list[str]:
    manifest_path = runtime_registration_manifest_path(compile_dir)
    try:
        payload = json.loads(manifest_path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        raise BehaviorMatrixFailure(
            f"runtime registration manifest is invalid JSON: {repo_rel(manifest_path)}"
        ) from exc
    raw_flags = payload.get("driver_linker_flags")
    if not isinstance(raw_flags, list) or not raw_flags:
        raise BehaviorMatrixFailure(
            "runtime registration manifest missing non-empty driver_linker_flags: "
            f"{repo_rel(manifest_path)}"
        )
    flags: list[str] = []
    for index, flag in enumerate(raw_flags):
        if not isinstance(flag, str) or not flag.strip():
            raise BehaviorMatrixFailure(
                "runtime registration manifest has invalid driver_linker_flags "
                f"entry {index}: {repo_rel(manifest_path)}"
            )
        flags.append(flag)
    return flags


def clang_command() -> str:
    return find_clangxx()


def link_fixture(
    fixture: BehaviorFixture,
    case_dir: Path,
    compile_dir: Path,
    *,
    include_runtime: bool,
) -> CommandExecution:
    exe_path = case_dir / "module.exe"
    command: list[object] = [
        clang_command(),
        "-std=c++20",
        "-fms-runtime-lib=dll",
        *normal_user_manifest_link_args(),
        object_path(compile_dir),
    ]
    if include_runtime:
        command.append(RUNTIME_LIB)
        command.extend(runtime_driver_linker_flags(compile_dir))
    command.extend(["-o", exe_path, "-fno-color-diagnostics"])
    return run_timed(
        command,
        metadata={
            "stage": "link",
            "fixture": fixture.relative_source,
            "include_runtime": include_runtime,
        },
    )


def canonical_link_text(fixture: BehaviorFixture, link_result: CommandExecution, compile_dir: Path) -> str:
    text = "\n".join(part for part in (link_result.stdout, link_result.stderr) if part)
    symbol = fixture.runtime_dispatch_symbol
    canonical_lines: list[str] = []
    if symbol and symbol in text:
        canonical_lines.append(f"link.unresolved_symbol:{symbol}")
    if object_path(compile_dir).name.startswith("module."):
        canonical_lines.append("link.input_object_basename:module.")
    return "\n".join([text, *canonical_lines])


def run_executable(fixture: BehaviorFixture, case_dir: Path) -> CommandExecution:
    return run_timed(
        [case_dir / "module.exe"],
        metadata={
            "stage": "run",
            "fixture": fixture.relative_source,
        },
    )

"""Packaged fixture compilation helpers for runnable interop validation."""

from __future__ import annotations

from pathlib import Path
from typing import Sequence

from objc3c_tooling.subprocesses import run_capture

from .assertions import expect
from .paths import PWSH


def compile_fixture(
    compile_script: Path,
    fixture_path: Path,
    compile_out_dir: Path,
    *,
    cwd: Path,
    extra_args: Sequence[str] = (),
) -> dict[str, Path]:
    compile_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(compile_script),
            str(fixture_path),
            "--out-dir",
            str(compile_out_dir),
            "--emit-prefix",
            "module",
            *extra_args,
        ],
        cwd=cwd,
    )
    if compile_result.returncode != 0:
        raise RuntimeError(f"packaged compile wrapper failed for {fixture_path}")

    artifacts = {
        "manifest": compile_out_dir / "module.manifest.json",
        "registration_manifest": compile_out_dir / "module.runtime-registration-manifest.json",
        "runtime_import_surface": compile_out_dir / "module.runtime-import-surface.json",
        "bridge_header": compile_out_dir / "module.interop-bridge.h",
        "bridge_modulemap": compile_out_dir / "module.interop-bridge.modulemap",
        "bridge_json": compile_out_dir / "module.interop-bridge.json",
        "object": compile_out_dir / "module.obj",
    }
    for artifact_path in artifacts.values():
        expect(artifact_path.is_file(), f"packaged compile wrapper did not publish {artifact_path}")
    return artifacts


def compile_consumer_fixture(
    compile_script: Path,
    fixture_path: Path,
    compile_out_dir: Path,
    *,
    cwd: Path,
    runtime_surface: Path,
    registration_order: str,
    failure_message: str,
) -> object:
    compile_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(compile_script),
            str(fixture_path),
            "--out-dir",
            str(compile_out_dir),
            "--emit-prefix",
            "module",
            "--objc3-bootstrap-registration-order-ordinal",
            registration_order,
            "--objc3-import-runtime-surface",
            str(runtime_surface),
        ],
        cwd=cwd,
    )
    if compile_result.returncode != 0:
        raise RuntimeError(failure_message)
    return compile_result


def consumer_artifact_paths(compile_dir: Path) -> dict[str, Path]:
    return {
        "manifest": compile_dir / "module.manifest.json",
        "registration_manifest": compile_dir / "module.runtime-registration-manifest.json",
        "cross_module_link_plan": compile_dir / "module.cross-module-runtime-link-plan.json",
        "cross_module_linker_rsp": compile_dir / "module.cross-module-runtime-linker-options.rsp",
        "runtime_metadata_linker_rsp": compile_dir / "module.runtime-metadata-linker-options.rsp",
        "object": compile_dir / "module.obj",
    }


def assert_artifacts_exist(artifacts: dict[str, Path], *, message_prefix: str) -> None:
    for artifact_path in artifacts.values():
        expect(artifact_path.is_file(), f"{message_prefix} {artifact_path}")


__all__ = [
    "assert_artifacts_exist",
    "compile_consumer_fixture",
    "compile_fixture",
    "consumer_artifact_paths",
]

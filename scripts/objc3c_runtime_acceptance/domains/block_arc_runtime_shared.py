"""Shared helpers for Block/ARC linked-runtime acceptance cases."""

from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.compile_backends import link_fixture_executable
from objc3c_runtime_acceptance.fixture_compilation import (
    compile_fixture_outputs,
    compile_fixture_outputs_with_args,
)
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.process_execution import run
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

_NATIVE_FIXTURE_DIR = ROOT / "tests" / "tooling" / "fixtures" / "native"
_RUNTIME_PROBE_DIR = ROOT / "tests" / "tooling" / "runtime"


@dataclass(frozen=True)
class LinkedRuntimeFixture:
    manifest: dict[str, Any]
    returncode: int


def native_fixture(file_name: str) -> Path:
    return _NATIVE_FIXTURE_DIR / file_name


def runtime_probe(file_name: str) -> Path:
    return _RUNTIME_PROBE_DIR / file_name


def read_json_file(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def block_copy_dispose_surface(manifest: dict[str, Any]) -> dict[str, Any]:
    return (
        manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )


def sema_pass_manager(manifest: dict[str, Any]) -> dict[str, Any]:
    return manifest.get("frontend", {}).get("pipeline", {}).get(
        "sema_pass_manager",
        {},
    )


def compile_link_run_fixture(
    clangxx: str,
    fixture: Path,
    out_dir: Path,
    exe_name: str,
    *,
    extra_args: list[str] | None = None,
) -> LinkedRuntimeFixture:
    if extra_args is None:
        obj_path, _, manifest_path = compile_fixture_outputs(fixture, out_dir)
    else:
        obj_path, _, manifest_path = compile_fixture_outputs_with_args(
            fixture,
            out_dir,
            extra_args=extra_args,
        )
    manifest = read_json_file(manifest_path)
    exe_path = out_dir / exe_name
    link_fixture_executable(clangxx, obj_path, exe_path)
    completed = run([str(exe_path)])
    return LinkedRuntimeFixture(
        manifest=manifest,
        returncode=completed.returncode,
    )


def compile_run_json_probe(
    clangxx: str,
    probe: Path,
    exe_path: Path,
    output_label: str,
    link_inputs: list[Path] | None = None,
) -> dict[str, Any]:
    compile_probe(clangxx, probe, exe_path, list(link_inputs or []))
    return parse_json_output(run_probe(exe_path), output_label)


__all__ = [
    "LinkedRuntimeFixture",
    "block_copy_dispose_surface",
    "compile_link_run_fixture",
    "compile_run_json_probe",
    "native_fixture",
    "read_json_file",
    "runtime_probe",
    "sema_pass_manager",
]

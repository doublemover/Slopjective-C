"""Compile and probe helpers for storage ownership reflection runtime cases."""

from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_outputs
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe


@dataclass(frozen=True)
class StorageOwnershipReflectionSources:
    case_id: str
    case_dir: Path
    fixture: Path
    probe: Path
    exe_path: Path
    probe_label: str

    def fixture_summary_path(self) -> str:
        return "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3"

    def probe_summary_path(self) -> str:
        return "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp"


@dataclass(frozen=True)
class StorageOwnershipReflectionArtifacts:
    obj_path: Path
    ll_path: Path
    manifest_path: Path
    registration_manifest_path: Path
    ll_text: str
    manifest: dict[str, Any]
    registration_manifest: dict[str, Any]


def build_storage_ownership_reflection_sources(
    run_dir: Path,
) -> StorageOwnershipReflectionSources:
    case_dir = run_dir / "storage-ownership-reflection"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "runtime_backed_storage_ownership_reflection_positive.objc3"
    )
    probe = (
        ROOT
        / "tests"
        / "tooling"
        / "runtime"
        / "runtime_backed_storage_ownership_reflection_probe.cpp"
    )
    return StorageOwnershipReflectionSources(
        case_id="storage-ownership-reflection",
        case_dir=case_dir,
        fixture=fixture,
        probe=probe,
        exe_path=case_dir / "runtime_backed_storage_ownership_reflection_probe.exe",
        probe_label="storage ownership reflection probe",
    )


def compile_storage_ownership_reflection_fixture(
    sources: StorageOwnershipReflectionSources,
) -> StorageOwnershipReflectionArtifacts:
    obj_path, ll_path, manifest_path = compile_fixture_outputs(
        sources.fixture,
        sources.case_dir / "compile",
    )
    registration_manifest_path = (
        sources.case_dir / "compile" / "module.runtime-registration-manifest.json"
    )
    if not registration_manifest_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {registration_manifest_path}")

    return StorageOwnershipReflectionArtifacts(
        obj_path=obj_path,
        ll_path=ll_path,
        manifest_path=manifest_path,
        registration_manifest_path=registration_manifest_path,
        ll_text=ll_path.read_text(encoding="utf-8"),
        manifest=json.loads(manifest_path.read_text(encoding="utf-8")),
        registration_manifest=json.loads(
            registration_manifest_path.read_text(encoding="utf-8")
        ),
    )


def run_storage_ownership_reflection_probe(
    clangxx: str,
    sources: StorageOwnershipReflectionSources,
    artifacts: StorageOwnershipReflectionArtifacts,
) -> dict[str, Any]:
    compile_probe(clangxx, sources.probe, sources.exe_path, [artifacts.obj_path])
    return parse_json_output(run_probe(sources.exe_path), sources.probe_label)


__all__ = [
    "StorageOwnershipReflectionArtifacts",
    "StorageOwnershipReflectionSources",
    "build_storage_ownership_reflection_sources",
    "compile_storage_ownership_reflection_fixture",
    "run_storage_ownership_reflection_probe",
]

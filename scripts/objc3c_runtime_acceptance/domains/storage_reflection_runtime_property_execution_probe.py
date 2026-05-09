"""Fixture and probe helpers for the storage/reflection property execution case."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.fixture_compilation import compile_fixture
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

PROPERTY_EXECUTION_CASE_ID = "property-execution"
PROPERTY_EXECUTION_PROBE = "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp"
PROPERTY_EXECUTION_FIXTURE = (
    "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3"
)
PROPERTY_EXECUTION_PROBE_LABEL = "property execution probe"


@dataclass(frozen=True)
class PropertyExecutionProbeSources:
    case_dir: Path
    fixture: Path
    probe: Path
    exe_path: Path


def build_property_execution_probe_sources(run_dir: Path) -> PropertyExecutionProbeSources:
    case_dir = run_dir / PROPERTY_EXECUTION_CASE_ID
    return PropertyExecutionProbeSources(
        case_dir=case_dir,
        fixture=ROOT / PROPERTY_EXECUTION_FIXTURE,
        probe=ROOT / PROPERTY_EXECUTION_PROBE,
        exe_path=case_dir / "property_ivar_execution_matrix_probe.exe",
    )


def run_property_execution_probe(clangxx: str, run_dir: Path) -> dict[str, Any]:
    sources = build_property_execution_probe_sources(run_dir)
    obj_path = compile_fixture(sources.fixture, sources.case_dir / "compile")
    compile_probe(clangxx, sources.probe, sources.exe_path, [obj_path])
    return parse_json_output(run_probe(sources.exe_path), PROPERTY_EXECUTION_PROBE_LABEL)


__all__ = [
    "PROPERTY_EXECUTION_CASE_ID",
    "PROPERTY_EXECUTION_FIXTURE",
    "PROPERTY_EXECUTION_PROBE",
    "PROPERTY_EXECUTION_PROBE_LABEL",
    "PropertyExecutionProbeSources",
    "build_property_execution_probe_sources",
    "run_property_execution_probe",
]

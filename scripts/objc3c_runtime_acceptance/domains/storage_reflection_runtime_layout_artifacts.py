"""Runtime artifact capture for storage/reflection layout cases."""

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


PROPERTY_LAYOUT_CASE_ID = "property-layout"
INSTANCE_ALLOCATION_LAYOUT_CASE_ID = "instance-allocation-layout-runtime"
SYNTHESIZED_ACCESSOR_PROPERTY_FIXTURE = (
    "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3"
)
INHERITED_INSTANCE_ALLOCATION_LAYOUT_FIXTURE = (
    "tests/tooling/fixtures/native/inherited_instance_allocation_layout_runtime_positive.objc3"
)
PROPERTY_LAYOUT_PROBE = "tests/tooling/runtime/property_layout_runtime_probe.cpp"
INSTANCE_ALLOCATION_LAYOUT_PROBE = (
    "tests/tooling/runtime/instance_allocation_runtime_probe.cpp"
)


@dataclass(frozen=True)
class PropertyLayoutArtifacts:
    payload: dict[str, Any]
    ll_text: str


@dataclass(frozen=True)
class InstanceAllocationLayoutArtifacts:
    payload: dict[str, Any]
    ll_text: str
    manifest: dict[str, Any]


def run_property_layout_probe(
    clangxx: str,
    run_dir: Path,
) -> PropertyLayoutArtifacts:
    case_dir = run_dir / PROPERTY_LAYOUT_CASE_ID
    fixture = ROOT / SYNTHESIZED_ACCESSOR_PROPERTY_FIXTURE
    obj_path, ll_path, _ = compile_fixture_outputs(fixture, case_dir / "compile")
    probe = ROOT / PROPERTY_LAYOUT_PROBE
    exe_path = case_dir / "property_layout_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "property layout runtime probe")

    return PropertyLayoutArtifacts(
        payload=payload,
        ll_text=ll_path.read_text(encoding="utf-8"),
    )


def run_instance_allocation_layout_probe(
    clangxx: str,
    run_dir: Path,
) -> InstanceAllocationLayoutArtifacts:
    case_dir = run_dir / INSTANCE_ALLOCATION_LAYOUT_CASE_ID
    fixture = ROOT / INHERITED_INSTANCE_ALLOCATION_LAYOUT_FIXTURE
    obj_path, ll_path, manifest_path = compile_fixture_outputs(
        fixture,
        case_dir / "compile",
    )
    probe = ROOT / INSTANCE_ALLOCATION_LAYOUT_PROBE
    exe_path = case_dir / "instance_allocation_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "instance allocation runtime probe")

    return InstanceAllocationLayoutArtifacts(
        payload=payload,
        ll_text=ll_path.read_text(encoding="utf-8"),
        manifest=json.loads(manifest_path.read_text(encoding="utf-8")),
    )


__all__ = [
    "INSTANCE_ALLOCATION_LAYOUT_CASE_ID",
    "INSTANCE_ALLOCATION_LAYOUT_PROBE",
    "INHERITED_INSTANCE_ALLOCATION_LAYOUT_FIXTURE",
    "InstanceAllocationLayoutArtifacts",
    "PROPERTY_LAYOUT_CASE_ID",
    "PROPERTY_LAYOUT_PROBE",
    "PropertyLayoutArtifacts",
    "SYNTHESIZED_ACCESSOR_PROPERTY_FIXTURE",
    "run_instance_allocation_layout_probe",
    "run_property_layout_probe",
]

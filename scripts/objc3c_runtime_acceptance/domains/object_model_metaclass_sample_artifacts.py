"""Compile artifacts and compile-surface assertions for metaclass samples."""

from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.fixture_compilation import (
    NegativeDiagnosticExpectation,
    compile_fixture_outputs,
    compile_negative_diagnostic_batch,
)
from objc3c_runtime_acceptance.paths import ROOT

from ..runtime_contract_object_model import (
    RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
    RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
)
from objc3c_runtime_acceptance.domains.object_model_metaclass_sample_sources import (
    CanonicalSampleSetSources,
    MetaclassGraphRootClassSources,
)


@dataclass(frozen=True)
class MetaclassGraphCompileArtifacts:
    obj_path: Path
    ll_path: Path
    manifest_path: Path
    ll_text: str
    manifest: dict[str, Any]


@dataclass(frozen=True)
class CanonicalSampleCompileArtifacts:
    obj_path: Path
    ll_path: Path
    manifest_path: Path
    registration_manifest_path: Path
    ll_text: str
    manifest: dict[str, Any]
    registration_manifest: dict[str, Any]


def compile_metaclass_graph_root_class_fixture(
    sources: MetaclassGraphRootClassSources,
) -> MetaclassGraphCompileArtifacts:
    obj_path, ll_path, manifest_path = compile_fixture_outputs(
        sources.fixture,
        sources.case_dir / "compile",
    )
    return MetaclassGraphCompileArtifacts(
        obj_path=obj_path,
        ll_path=ll_path,
        manifest_path=manifest_path,
        ll_text=ll_path.read_text(encoding="utf-8"),
        manifest=json.loads(manifest_path.read_text(encoding="utf-8")),
    )


def compile_canonical_sample_set_fixture(
    sources: CanonicalSampleSetSources,
) -> CanonicalSampleCompileArtifacts:
    obj_path, ll_path, manifest_path = compile_fixture_outputs(
        sources.fixture,
        sources.case_dir / "compile",
    )
    registration_manifest_path = (
        sources.case_dir / "compile" / "module.runtime-registration-manifest.json"
    )
    if not registration_manifest_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {registration_manifest_path}")

    return CanonicalSampleCompileArtifacts(
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


def assert_metaclass_graph_compile_artifacts(
    artifacts: MetaclassGraphCompileArtifacts,
) -> None:
    surface = artifacts.manifest.get(
        "runtime_class_metaclass_protocol_realization_surface"
    )
    expect(
        isinstance(surface, dict)
        and surface.get("contract_id")
        == RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID
        and surface.get("realized_class_graph_snapshot_symbol")
        == "objc3_runtime_copy_realized_class_graph_state_for_testing"
        and surface.get("realized_class_entry_snapshot_symbol")
        == "objc3_runtime_copy_realized_class_entry_for_testing",
        "expected compile manifest to publish the class/metaclass protocol realization surface and private snapshot symbols",
    )
    expect(
        "runtime_metadata_class_metaclass_emission" in artifacts.ll_text
        and "runtime_metaclass_graph_root_class_baseline" in artifacts.ll_text,
        "expected LLVM IR to carry class/metaclass emission and root-class baseline proof comments",
    )


def assert_metaclass_graph_negative_diagnostics(
    sources: MetaclassGraphRootClassSources,
) -> Any:
    return compile_negative_diagnostic_batch(
        case_id=sources.case_id,
        out_dir=sources.case_dir / "negative-diagnostics-batch",
        expectations=[
            NegativeDiagnosticExpectation(
                key="missing-superclass",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "inheritance_override_missing_superclass.objc3",
                expected_snippets=[
                    "runtime realization failed: interface 'Widget' inherits from missing superclass 'MissingRoot'",
                ],
                expected_codes=["O3S220"],
            ),
            NegativeDiagnosticExpectation(
                key="unrealized-superclass",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "inheritance_override_unrealized_superclass.objc3",
                expected_snippets=[
                    "runtime realization failed: implementation 'Widget' requires realized superclass implementation 'Root'",
                ],
                expected_codes=["O3S220"],
            ),
            NegativeDiagnosticExpectation(
                key="superclass-cycle",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "inheritance_override_cycle.objc3",
                expected_snippets=[
                    "cyclic Objective-C interface inheritance cannot produce a stable ivar layout",
                    "runtime metadata export blocked",
                ],
                expected_codes=["O3P150", "O3S260"],
            ),
        ],
    )


def assert_canonical_sample_registration_manifest(
    artifacts: CanonicalSampleCompileArtifacts,
) -> None:
    registration_manifest = artifacts.registration_manifest
    expect(
        registration_manifest.get("class_descriptor_count") == 4,
        "expected canonical sample set to publish four class descriptors",
    )
    expect(
        registration_manifest.get("protocol_descriptor_count") == 2,
        "expected canonical sample set to publish two protocol descriptors",
    )
    expect(
        registration_manifest.get("category_descriptor_count") == 2,
        "expected canonical sample set to publish two category descriptors",
    )
    expect(
        registration_manifest.get("property_descriptor_count") == 8,
        "expected canonical sample set to publish eight property descriptors",
    )
    expect(
        registration_manifest.get("ivar_descriptor_count") == 4,
        "expected canonical sample set to publish four ivar descriptors",
    )
    expect(
        registration_manifest.get("compile_output_truthfulness_property_descriptor_count")
        == 8,
        "expected compile-output truthfulness to certify eight property descriptors for the canonical sample set",
    )
    expect(
        registration_manifest.get("compile_output_truthfulness_ivar_descriptor_count")
        == 4,
        "expected compile-output truthfulness to certify four ivar descriptors for the canonical sample set",
    )


def assert_canonical_sample_lowering_artifacts(
    artifacts: CanonicalSampleCompileArtifacts,
) -> None:
    dispatch_table_reflection_record_lowering_surface = artifacts.manifest.get(
        "runtime_dispatch_table_reflection_record_lowering_surface", {}
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("contract_id")
        == RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        "expected canonical sample set compile manifest to publish dispatch-table/reflection-record lowering surface",
    )
    expect(
        "; executable_realization_records = contract=objc3c.executable.realization.records.v1"
        in artifacts.ll_text,
        "expected canonical sample set LLVM IR to publish executable realization records",
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("class_aggregate_symbol")
        == "__objc3_sec_class_descriptors",
        "expected canonical sample set lowering surface to preserve the class aggregate root symbol",
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("property_aggregate_symbol")
        == "__objc3_sec_property_descriptors",
        "expected canonical sample set lowering surface to preserve the property aggregate root symbol",
    )


__all__ = [
    "CanonicalSampleCompileArtifacts",
    "MetaclassGraphCompileArtifacts",
    "assert_canonical_sample_lowering_artifacts",
    "assert_canonical_sample_registration_manifest",
    "assert_metaclass_graph_compile_artifacts",
    "assert_metaclass_graph_negative_diagnostics",
    "compile_canonical_sample_set_fixture",
    "compile_metaclass_graph_root_class_fixture",
]

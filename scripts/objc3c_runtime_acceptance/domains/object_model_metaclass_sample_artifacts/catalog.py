"""Expected compile outputs for Object Model metaclass sample artifacts."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from objc3c_runtime_acceptance.fixture_compilation import NegativeDiagnosticExpectation
from objc3c_runtime_acceptance.paths import ROOT

from objc3c_runtime_acceptance.runtime_contract_object_model import (
    RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
)
from objc3c_runtime_acceptance.runtime_contract_object_model import (
    RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
)


@dataclass(frozen=True)
class FieldExpectation:
    key: str
    expected_value: Any
    message: str


@dataclass(frozen=True)
class MappingExpectation:
    surface_key: str
    expected_fields: dict[str, Any]
    message: str


METACLASS_GRAPH_SURFACE_EXPECTATION = MappingExpectation(
    surface_key="runtime_class_metaclass_protocol_realization_surface",
    expected_fields={
        "contract_id": RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
        "realized_class_graph_snapshot_symbol": "objc3_runtime_copy_realized_class_graph_state_for_testing",
        "realized_class_entry_snapshot_symbol": "objc3_runtime_copy_realized_class_entry_for_testing",
    },
    message="expected compile manifest to publish the class/metaclass protocol realization surface and private snapshot symbols",
)

METACLASS_GRAPH_LL_MARKERS = (
    "runtime_metadata_class_metaclass_emission",
    "runtime_metaclass_graph_root_class_baseline",
)

METACLASS_GRAPH_LL_MARKERS_MESSAGE = (
    "expected LLVM IR to carry class/metaclass emission and root-class baseline proof comments"
)

METACLASS_GRAPH_NEGATIVE_DIAGNOSTIC_EXPECTATIONS = [
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
]

CANONICAL_SAMPLE_REGISTRATION_MANIFEST_EXPECTATIONS = (
    FieldExpectation(
        key="class_descriptor_count",
        expected_value=4,
        message="expected canonical sample set to publish four class descriptors",
    ),
    FieldExpectation(
        key="protocol_descriptor_count",
        expected_value=2,
        message="expected canonical sample set to publish two protocol descriptors",
    ),
    FieldExpectation(
        key="category_descriptor_count",
        expected_value=2,
        message="expected canonical sample set to publish two category descriptors",
    ),
    FieldExpectation(
        key="property_descriptor_count",
        expected_value=8,
        message="expected canonical sample set to publish eight property descriptors",
    ),
    FieldExpectation(
        key="ivar_descriptor_count",
        expected_value=4,
        message="expected canonical sample set to publish four ivar descriptors",
    ),
    FieldExpectation(
        key="compile_output_truthfulness_property_descriptor_count",
        expected_value=8,
        message="expected compile-output truthfulness to certify eight property descriptors for the canonical sample set",
    ),
    FieldExpectation(
        key="compile_output_truthfulness_ivar_descriptor_count",
        expected_value=4,
        message="expected compile-output truthfulness to certify four ivar descriptors for the canonical sample set",
    ),
)

CANONICAL_SAMPLE_LOWERING_SURFACE_EXPECTATION = MappingExpectation(
    surface_key="runtime_dispatch_table_reflection_record_lowering_surface",
    expected_fields={
        "contract_id": RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        "class_aggregate_symbol": "__objc3_sec_class_descriptors",
        "property_aggregate_symbol": "__objc3_sec_property_descriptors",
    },
    message="expected canonical sample set compile manifest to publish dispatch-table/reflection-record lowering surface",
)

CANONICAL_SAMPLE_LOWERING_LL_MARKER = (
    "; executable_realization_records = contract=objc3c.executable.realization.records.v1"
)

CANONICAL_SAMPLE_LOWERING_LL_MARKER_MESSAGE = (
    "expected canonical sample set LLVM IR to publish executable realization records"
)

CANONICAL_SAMPLE_CLASS_AGGREGATE_MESSAGE = (
    "expected canonical sample set lowering surface to preserve the class aggregate root symbol"
)

CANONICAL_SAMPLE_PROPERTY_AGGREGATE_MESSAGE = (
    "expected canonical sample set lowering surface to preserve the property aggregate root symbol"
)


__all__ = [
    "CANONICAL_SAMPLE_CLASS_AGGREGATE_MESSAGE",
    "CANONICAL_SAMPLE_LOWERING_LL_MARKER",
    "CANONICAL_SAMPLE_LOWERING_LL_MARKER_MESSAGE",
    "CANONICAL_SAMPLE_LOWERING_SURFACE_EXPECTATION",
    "CANONICAL_SAMPLE_PROPERTY_AGGREGATE_MESSAGE",
    "CANONICAL_SAMPLE_REGISTRATION_MANIFEST_EXPECTATIONS",
    "FieldExpectation",
    "METACLASS_GRAPH_LL_MARKERS",
    "METACLASS_GRAPH_LL_MARKERS_MESSAGE",
    "METACLASS_GRAPH_NEGATIVE_DIAGNOSTIC_EXPECTATIONS",
    "METACLASS_GRAPH_SURFACE_EXPECTATION",
    "MappingExpectation",
]

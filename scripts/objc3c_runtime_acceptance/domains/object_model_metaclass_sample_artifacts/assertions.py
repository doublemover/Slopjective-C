"""Compile-surface assertions for Object Model metaclass samples."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.domains.object_model_metaclass_sample_sources import (
    MetaclassGraphRootClassSources,
)
from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.fixture_compilation import compile_negative_diagnostic_batch

from .catalog import CANONICAL_SAMPLE_CLASS_AGGREGATE_MESSAGE
from .catalog import CANONICAL_SAMPLE_LOWERING_LL_MARKER
from .catalog import CANONICAL_SAMPLE_LOWERING_LL_MARKER_MESSAGE
from .catalog import CANONICAL_SAMPLE_LOWERING_SURFACE_EXPECTATION
from .catalog import CANONICAL_SAMPLE_PROPERTY_AGGREGATE_MESSAGE
from .catalog import CANONICAL_SAMPLE_REGISTRATION_MANIFEST_EXPECTATIONS
from .catalog import METACLASS_GRAPH_LL_MARKERS
from .catalog import METACLASS_GRAPH_LL_MARKERS_MESSAGE
from .catalog import METACLASS_GRAPH_NEGATIVE_DIAGNOSTIC_EXPECTATIONS
from .catalog import METACLASS_GRAPH_SURFACE_EXPECTATION
from .data import CanonicalSampleCompileArtifacts
from .data import MetaclassGraphCompileArtifacts
from .predicates import mapping_has_expected_fields
from .predicates import text_contains_all


def assert_metaclass_graph_compile_artifacts(
    artifacts: MetaclassGraphCompileArtifacts,
) -> None:
    surface = artifacts.manifest.get(METACLASS_GRAPH_SURFACE_EXPECTATION.surface_key)
    expect(
        isinstance(surface, dict)
        and mapping_has_expected_fields(
            surface,
            METACLASS_GRAPH_SURFACE_EXPECTATION.expected_fields,
        ),
        METACLASS_GRAPH_SURFACE_EXPECTATION.message,
    )
    expect(
        text_contains_all(artifacts.ll_text, METACLASS_GRAPH_LL_MARKERS),
        METACLASS_GRAPH_LL_MARKERS_MESSAGE,
    )


def assert_metaclass_graph_negative_diagnostics(
    sources: MetaclassGraphRootClassSources,
) -> Any:
    return compile_negative_diagnostic_batch(
        case_id=sources.case_id,
        out_dir=sources.case_dir / "negative-diagnostics-batch",
        expectations=METACLASS_GRAPH_NEGATIVE_DIAGNOSTIC_EXPECTATIONS,
    )


def assert_canonical_sample_registration_manifest(
    artifacts: CanonicalSampleCompileArtifacts,
) -> None:
    registration_manifest = artifacts.registration_manifest
    for expectation in CANONICAL_SAMPLE_REGISTRATION_MANIFEST_EXPECTATIONS:
        expect(
            registration_manifest.get(expectation.key) == expectation.expected_value,
            expectation.message,
        )


def assert_canonical_sample_lowering_artifacts(
    artifacts: CanonicalSampleCompileArtifacts,
) -> None:
    dispatch_table_reflection_record_lowering_surface = artifacts.manifest.get(
        CANONICAL_SAMPLE_LOWERING_SURFACE_EXPECTATION.surface_key,
        {},
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("contract_id")
        == CANONICAL_SAMPLE_LOWERING_SURFACE_EXPECTATION.expected_fields["contract_id"],
        CANONICAL_SAMPLE_LOWERING_SURFACE_EXPECTATION.message,
    )
    expect(
        CANONICAL_SAMPLE_LOWERING_LL_MARKER in artifacts.ll_text,
        CANONICAL_SAMPLE_LOWERING_LL_MARKER_MESSAGE,
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("class_aggregate_symbol")
        == CANONICAL_SAMPLE_LOWERING_SURFACE_EXPECTATION.expected_fields[
            "class_aggregate_symbol"
        ],
        CANONICAL_SAMPLE_CLASS_AGGREGATE_MESSAGE,
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("property_aggregate_symbol")
        == CANONICAL_SAMPLE_LOWERING_SURFACE_EXPECTATION.expected_fields[
            "property_aggregate_symbol"
        ],
        CANONICAL_SAMPLE_PROPERTY_AGGREGATE_MESSAGE,
    )


__all__ = [
    "assert_canonical_sample_lowering_artifacts",
    "assert_canonical_sample_registration_manifest",
    "assert_metaclass_graph_compile_artifacts",
    "assert_metaclass_graph_negative_diagnostics",
]

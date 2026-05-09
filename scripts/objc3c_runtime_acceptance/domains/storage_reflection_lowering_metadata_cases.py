"""Storage/reflection runtime acceptance case helpers."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.storage_reflection_lowering_metadata_artifacts import (
    load_storage_lowering_compile_artifacts,
)
from objc3c_runtime_acceptance.domains.storage_reflection_lowering_metadata_summary import (
    build_accessor_storage_lowering_metadata_summary,
)
from objc3c_runtime_acceptance.domains.storage_reflection_lowering_metadata_surface_assertions import (
    assert_arc_accessor_lowering_metadata_surface,
    assert_synthesized_accessor_lowering_metadata_surface,
)


def check_accessor_storage_lowering_metadata_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "accessor-storage-lowering-metadata"
    synthesized_manifest, synthesized_ll = load_storage_lowering_compile_artifacts(
        case_dir,
        "synthesized_accessor_property_lowering_positive.objc3",
        "synthesized-accessors",
    )
    synthesized_lowering_surface = (
        assert_synthesized_accessor_lowering_metadata_surface(
            synthesized_manifest,
            synthesized_ll,
        )
    )

    arc_manifest, arc_ll = load_storage_lowering_compile_artifacts(
        case_dir,
        "arc_property_interaction_positive.objc3",
        "arc-accessors",
    )
    arc_lowering_surface = assert_arc_accessor_lowering_metadata_surface(
        arc_manifest,
        arc_ll,
    )

    return CaseResult(
        case_id="accessor-storage-lowering-metadata-surface",
        probe="compile-manifest-and-executable-metadata-surface",
        fixture="tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=build_accessor_storage_lowering_metadata_summary(
            synthesized_lowering_surface,
            arc_lowering_surface,
        ),
    )


__all__ = ["check_accessor_storage_lowering_metadata_surface_case"]

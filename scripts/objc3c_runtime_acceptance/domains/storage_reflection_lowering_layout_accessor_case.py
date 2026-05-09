"""Property accessor layout lowering acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.storage_reflection_lowering_layout_artifacts import (
    PROPERTY_ACCESSOR_LAYOUT_FIXTURE_LABEL,
    compile_property_accessor_layout_fixture,
)
from objc3c_runtime_acceptance.domains.storage_reflection_lowering_layout_ir_assertions import (
    assert_property_accessor_layout_ir,
)
from objc3c_runtime_acceptance.domains.storage_reflection_lowering_layout_surface_assertions import (
    assert_accessor_layout_surface,
    assert_executable_synthesized_accessor_surface,
    assert_ivar_layout_surface,
    assert_property_source_surface_links,
    assert_registration_manifest_layout_counts,
)


def check_property_accessor_layout_lowering_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-accessor-layout-lowering"
    artifacts = compile_property_accessor_layout_fixture(case_dir)
    manifest = json.loads(artifacts.manifest_path.read_text(encoding="utf-8"))
    registration_manifest = json.loads(
        artifacts.registration_manifest_path.read_text(encoding="utf-8")
    )
    ll_text = artifacts.ll_path.read_text(encoding="utf-8")

    assert_property_source_surface_links(manifest)
    accessor_layout_surface = assert_accessor_layout_surface(manifest)
    ivar_layout_surface = assert_ivar_layout_surface(manifest)
    synthesized_accessor_surface = assert_executable_synthesized_accessor_surface(
        manifest
    )
    assert_registration_manifest_layout_counts(registration_manifest)
    assert_property_accessor_layout_ir(ll_text)

    return CaseResult(
        case_id="property-accessor-layout-lowering",
        probe="compile-manifest-registration-manifest-and-llvm-ir",
        fixture=PROPERTY_ACCESSOR_LAYOUT_FIXTURE_LABEL,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "property_descriptor_entries": accessor_layout_surface.get(
                "property_descriptor_entries"
            ),
            "ivar_descriptor_entries": accessor_layout_surface.get(
                "ivar_descriptor_entries"
            ),
            "synthesized_accessor_entries": synthesized_accessor_surface.get(
                "synthesized_accessor_entries"
            ),
            "layout_table_entries": ivar_layout_surface.get("layout_table_entries"),
        },
    )


__all__ = ["check_property_accessor_layout_lowering_case"]

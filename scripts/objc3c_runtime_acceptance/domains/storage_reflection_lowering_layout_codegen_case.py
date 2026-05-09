"""Synthesized accessor codegen acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.storage_reflection_lowering_layout_artifacts import (
    PROPERTY_ACCESSOR_LAYOUT_FIXTURE_LABEL,
    compile_property_accessor_layout_fixture,
)
from objc3c_runtime_acceptance.domains.storage_reflection_lowering_layout_ir_assertions import (
    assert_synthesized_accessor_codegen_ir_snippets,
    assert_synthesized_accessor_codegen_surface_banners,
)
from objc3c_runtime_acceptance.domains.storage_reflection_lowering_layout_surface_assertions import (
    assert_synthesized_accessor_codegen_manifest,
)

from ..core import ROOT


def check_synthesized_accessor_codegen_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-codegen"
    artifacts = compile_property_accessor_layout_fixture(case_dir)

    ll_text = artifacts.ll_path.read_text(encoding="utf-8")
    manifest = json.loads(artifacts.manifest_path.read_text(encoding="utf-8"))
    registration_manifest = json.loads(
        artifacts.registration_manifest_path.read_text(encoding="utf-8")
    )

    assert_synthesized_accessor_codegen_ir_snippets(ll_text)
    assert_synthesized_accessor_codegen_manifest(manifest, registration_manifest)
    assert_synthesized_accessor_codegen_surface_banners(ll_text)

    return CaseResult(
        case_id="property-codegen",
        probe="real-compile-llvm-inspection",
        fixture=PROPERTY_ACCESSOR_LAYOUT_FIXTURE_LABEL,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "llvm_ir": str(artifacts.ll_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(artifacts.manifest_path.relative_to(ROOT)).replace(
                "\\", "/"
            ),
            "registration_manifest": str(
                artifacts.registration_manifest_path.relative_to(ROOT)
            ).replace("\\", "/"),
            "property_descriptor_count": registration_manifest.get(
                "property_descriptor_count"
            ),
        },
    )


__all__ = ["check_synthesized_accessor_codegen_case"]

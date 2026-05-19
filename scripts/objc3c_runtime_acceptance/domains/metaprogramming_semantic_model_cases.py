"""Metaprogramming semantic model runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_outputs
from objc3c_runtime_acceptance.paths import ROOT



def check_metaprogramming_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "metaprogramming-semantics"
    fixtures: dict[str, tuple[Path, int, int, int]] = {
        "semantic_model": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "expansion_behavior_semantic_model_positive.objc3",
            0,
            1,
            2,
        ),
        "lowering_ready": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "expansion_lowering_positive.objc3",
            1,
            1,
            2,
        ),
    }
    summary: dict[str, Any] = {}

    for fixture_key, (
        fixture_path,
        expected_derive_marker_sites,
        expected_macro_marker_sites,
        expected_property_behavior_sites,
    ) in fixtures.items():
        _, _, manifest_path = compile_fixture_outputs(fixture_path, case_dir / fixture_key / "compile")
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        semantic_surface = (
            manifest.get("frontend", {})
            .get("pipeline", {})
            .get("semantic_surface", {})
            .get("objc_metaprogramming_expansion_and_behavior_semantic_model", {})
        )
        expect(
            isinstance(semantic_surface, dict),
            f"expected {fixture_key} fixture to publish objc_metaprogramming_expansion_and_behavior_semantic_model",
        )
        expect(
            semantic_surface.get("contract_id")
            == "objc3c.metaprogramming.expansion.behavior.semantic.model.v1",
            f"expected {fixture_key} fixture to preserve the metaprogramming expansion behavior semantic contract",
        )
        expect(
            semantic_surface.get("frontend_dependency_contract_id")
            == "objc3c.metaprogramming.property.behavior.source.completion.v1",
            f"expected {fixture_key} fixture to preserve the property behavior source dependency contract",
        )
        expect(
            semantic_surface.get("derive_marker_sites") == expected_derive_marker_sites
            and semantic_surface.get("macro_marker_sites") == expected_macro_marker_sites
            and semantic_surface.get("property_behavior_sites")
            == expected_property_behavior_sites,
            f"expected {fixture_key} fixture to preserve semantic site counts",
        )
        expect(
            semantic_surface.get("macro_package_provenance_surface_reused") is True
            and semantic_surface.get("property_behavior_source_supported") is True
            and semantic_surface.get("synthesized_visibility_surface_reused")
            is True,
            f"expected {fixture_key} fixture to preserve semantic surface reuse flags",
        )
        expect(
            semantic_surface.get("derive_synthesis_deferred") is True
            and semantic_surface.get("macro_execution_deferred") is True
            and semantic_surface.get("property_behavior_runtime_deferred") is True,
            f"expected {fixture_key} fixture to preserve deferred runtime semantics",
        )
        expect(
            semantic_surface.get("deterministic") is True
            and semantic_surface.get("ready_for_core_implementation") is True,
            f"expected {fixture_key} fixture to preserve deterministic semantic readiness",
        )
        summary[fixture_key] = {
            "fixture": str(fixture_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "contract_id": semantic_surface.get("contract_id"),
            "derive_marker_sites": semantic_surface.get("derive_marker_sites"),
            "macro_marker_sites": semantic_surface.get("macro_marker_sites"),
            "property_behavior_sites": semantic_surface.get("property_behavior_sites"),
        }

    return CaseResult(
        case_id="metaprogramming-semantics",
        probe="compile-manifest-semantic-surface",
        fixture="tests/tooling/fixtures/native/expansion_behavior_semantic_model_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=summary,
    )

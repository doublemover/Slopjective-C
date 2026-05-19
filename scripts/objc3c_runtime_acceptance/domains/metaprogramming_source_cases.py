"""Metaprogramming source-surface runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_outputs
from objc3c_runtime_acceptance.paths import ROOT

def check_metaprogramming_source_surface_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "metaprogramming-source-surface"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "expansion_lowering_positive.objc3"
    )
    _, _, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    semantic_surface = (
        manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_metaprogramming_derive_macro_property_behavior_source_closure", {})
    )
    expect(
        isinstance(semantic_surface, dict),
        "expected metaprogramming source fixture to publish objc_metaprogramming_derive_macro_property_behavior_source_closure",
    )
    expected_fields = {
        "contract_id": "objc3c.metaprogramming.metaprogramming.source.closure.v1",
        "frontend_surface_path": (
            "frontend.pipeline.semantic_surface."
            "objc_metaprogramming_derive_macro_property_behavior_source_closure"
        ),
        "source_model": (
            "derive-markers-macro-markers-and-property-behavior-markers-are-live-parser-owned-source-surfaces-while-expansion-synthesis-and-runtime-behavior-remain-deferred"
        ),
        "failure_model": (
            "metaprogramming-stays-source-closure-only-with-no-macro-expansion-derived-conformance-synthesis-or-property-runtime-claims-yet"
        ),
    }
    for field, expected_value in expected_fields.items():
        expect(
            semantic_surface.get(field) == expected_value,
            f"expected metaprogramming source surface to preserve {field}",
        )
    expect(
        semantic_surface.get("source_only_claim_ids")
        == [
            "source-only:derive-markers",
            "source-only:macro-markers",
            "source-only:property-behavior-markers",
        ],
        "expected metaprogramming source surface to preserve source_only_claim_ids",
    )
    expect(
        semantic_surface.get("derive_marker_sites") == 1,
        "expected metaprogramming source surface to publish one derive marker site",
    )
    expect(
        semantic_surface.get("macro_marker_sites") == 1,
        "expected metaprogramming source surface to publish one macro marker site",
    )
    expect(
        semantic_surface.get("property_behavior_sites") == 2,
        "expected metaprogramming source surface to publish two property behavior sites",
    )
    expect(
        semantic_surface.get("derive_marker_source_supported") is True
        and semantic_surface.get("macro_marker_source_supported") is True
        and semantic_surface.get("property_behavior_source_supported") is True,
        "expected metaprogramming source surface to preserve source support flags",
    )
    expect(
        semantic_surface.get("deterministic_handoff") is True
        and semantic_surface.get("ready_for_semantic_expansion") is True,
        "expected metaprogramming source surface to preserve deterministic source handoff",
    )

    return CaseResult(
        case_id="metaprogramming-source-surface",
        probe="compile-manifest-source-surface",
        fixture="tests/tooling/fixtures/native/expansion_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "surface": "objc_metaprogramming_derive_macro_property_behavior_source_closure",
            "contract_id": semantic_surface.get("contract_id"),
            "derive_marker_sites": semantic_surface.get("derive_marker_sites"),
            "macro_marker_sites": semantic_surface.get("macro_marker_sites"),
            "property_behavior_sites": semantic_surface.get("property_behavior_sites"),
        },
    )

def check_metaprogramming_package_provenance_source_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "metaprogramming-package-provenance-source-surface"
    fixtures: dict[str, tuple[Path, str, str]] = {
        "macro_package_provenance": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "expansion_lowering_positive.objc3",
            "objc_metaprogramming_macro_package_and_provenance_source_completion",
            "objc3c.metaprogramming.macro.package.provenance.source.completion.v1",
        ),
        "property_behavior_source_completion": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_behavior_source_completion_positive.objc3",
            "objc_metaprogramming_property_behavior_and_synthesized_declaration_source_completion",
            "objc3c.metaprogramming.property.behavior.source.completion.v1",
        ),
    }
    summary: dict[str, Any] = {}

    for fixture_key, (
        fixture_path,
        semantic_surface_name,
        expected_contract_id,
    ) in fixtures.items():
        _, _, manifest_path = compile_fixture_outputs(fixture_path, case_dir / fixture_key / "compile")
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        semantic_surface = (
            manifest.get("frontend", {})
            .get("pipeline", {})
            .get("semantic_surface", {})
            .get(semantic_surface_name, {})
        )
        expect(
            isinstance(semantic_surface, dict),
            f"expected {fixture_key} fixture to publish {semantic_surface_name}",
        )
        expect(
            semantic_surface.get("contract_id") == expected_contract_id,
            f"expected {fixture_key} fixture to preserve {expected_contract_id}",
        )
        expect(
            semantic_surface.get("deterministic_handoff") is True
            and semantic_surface.get("ready_for_semantic_expansion") is True,
            f"expected {fixture_key} fixture to preserve deterministic source completion handoff",
        )
        summary[fixture_key] = {
            "fixture": str(fixture_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "surface": semantic_surface_name,
            "contract_id": semantic_surface.get("contract_id"),
        }
        if fixture_key == "macro_package_provenance":
            expect(
                semantic_surface.get("macro_marker_sites") == 1
                and semantic_surface.get("macro_package_sites") == 1
                and semantic_surface.get("macro_provenance_sites") == 1
                and semantic_surface.get("expansion_visible_macro_sites") == 1,
                "expected macro package/provenance source completion to preserve marker and visibility counts",
            )
            expect(
                semantic_surface.get("macro_package_source_supported") is True
                and semantic_surface.get("macro_provenance_source_supported") is True
                and semantic_surface.get("expansion_visible_source_supported") is True,
                "expected macro package/provenance source completion to preserve support flags",
            )
        else:
            expect(
                semantic_surface.get("property_behavior_sites") == 5
                and semantic_surface.get("interface_property_behavior_sites") == 2
                and semantic_surface.get("implementation_property_behavior_sites") == 2
                and semantic_surface.get("protocol_property_behavior_sites") == 1,
                "expected property-behavior source completion to preserve property behavior counts",
            )
            expect(
                semantic_surface.get("synthesized_binding_visible_sites") == 4
                and semantic_surface.get("synthesized_getter_visible_sites") == 5
                and semantic_surface.get("synthesized_setter_visible_sites") == 2,
                "expected property-behavior source completion to preserve synthesized declaration visibility counts",
            )
            expect(
                semantic_surface.get("property_behavior_source_supported") is True
                and semantic_surface.get("synthesized_declaration_visibility_supported")
                is True,
                "expected property-behavior source completion to preserve support flags",
            )

    return CaseResult(
        case_id="metaprogramming-package-provenance-source-surface",
        probe="compile-manifest-source-completion-surface",
        fixture="tests/tooling/fixtures/native/expansion_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=summary,
    )

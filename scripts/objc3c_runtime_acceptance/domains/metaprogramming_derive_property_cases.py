"""Metaprogramming derive/property behavior semantic acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import (
    compile_fixture_outputs,
    run_fixture_compile,
)

from ..native_build import ROOT


def check_metaprogramming_derive_property_behavior_semantics_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "metaprogramming-derive-property-behavior-semantics"
    derive_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "derive_expansion_inventory_positive.objc3"
    )
    property_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_behavior_legality_positive.objc3"
    )
    _, _, derive_manifest_path = compile_fixture_outputs(
        derive_fixture, case_dir / "derive-positive" / "compile"
    )
    _, _, property_manifest_path = compile_fixture_outputs(
        property_fixture, case_dir / "property-positive" / "compile"
    )
    derive_manifest = json.loads(derive_manifest_path.read_text(encoding="utf-8"))
    property_manifest = json.loads(property_manifest_path.read_text(encoding="utf-8"))
    derive_surface = (
        derive_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_metaprogramming_derive_expansion_inventory", {})
    )
    property_surface = (
        property_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get(
            "objc_metaprogramming_property_behavior_legality_and_interaction_completion",
            {},
        )
    )
    expect(
        derive_surface.get("contract_id")
        == "objc3c.metaprogramming.derive.expansion.inventory.v1",
        "expected derive expansion inventory fixture to preserve the derive inventory contract",
    )
    expect(
        derive_surface.get("derive_request_sites") == 4
        and derive_surface.get("supported_derive_request_sites") == 4
        and derive_surface.get("generated_method_entry_count") == 4,
        "expected derive expansion inventory fixture to preserve supported derive counts",
    )
    expect(
        derive_surface.get("equatable_alias_sites") == 1
        and derive_surface.get("equality_derive_sites") == 2
        and derive_surface.get("hash_derive_sites") == 1
        and derive_surface.get("debug_description_derive_sites") == 1,
        "expected derive expansion inventory fixture to preserve derive family counts",
    )
    expect(
        derive_surface.get("unsupported_derive_fail_closed") is True
        and derive_surface.get("selector_conflicts_fail_closed") is True
        and derive_surface.get("deterministic") is True
        and derive_surface.get("ready_for_lowering_and_runtime") is True,
        "expected derive expansion inventory fixture to preserve fail-closed readiness",
    )
    expect(
        property_surface.get("contract_id")
        == "objc3c.metaprogramming.property.behavior.legality.interaction.completion.v1",
        "expected property behavior legality fixture to preserve the legality contract",
    )
    expect(
        property_surface.get("property_behavior_sites") == 5
        and property_surface.get("supported_behavior_sites") == 5
        and property_surface.get("unsupported_behavior_sites") == 0,
        "expected property behavior legality fixture to preserve behavior counts",
    )
    expect(
        property_surface.get("observed_behavior_sites") == 2
        and property_surface.get("projected_behavior_sites") == 3,
        "expected property behavior legality fixture to preserve observed/projected counts",
    )
    expect(
        property_surface.get("unsupported_behavior_fail_closed") is True
        and property_surface.get("owner_topology_fail_closed") is True
        and property_surface.get("interaction_legality_fail_closed") is True
        and property_surface.get("storage_legality_fail_closed") is True
        and property_surface.get("deterministic") is True
        and property_surface.get("ready_for_lowering_and_runtime") is True,
        "expected property behavior legality fixture to preserve fail-closed readiness",
    )

    negative_fixtures = {
        "unsupported_derive": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "derive_expansion_inventory_negative_unsupported.objc3",
            "O3S317",
            "unsupported derive 'Networked'",
        ),
        "unsupported_behavior": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_behavior_legality_negative_unsupported.objc3",
            "O3S326",
            "unsupported property behavior 'Cached'",
        ),
        "nonobject_behavior": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_behavior_legality_negative_nonobject.objc3",
            "O3S327",
            "requires an Objective-C object property",
        ),
        "protocol_observed": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_behavior_legality_negative_protocol_observed.objc3",
            "O3S328",
            "requires a concrete interface or implementation property",
        ),
        "projected_writable": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_behavior_legality_negative_projected_writable.objc3",
            "O3S330",
            "requires a readonly getter-only property",
        ),
    }
    negative_summary: dict[str, Any] = {}
    for negative_key, (fixture_path, expected_code, expected_message) in negative_fixtures.items():
        compile_dir = case_dir / negative_key / "compile"
        compile_result, _ = run_fixture_compile(
            fixture_path,
            compile_dir,
            write_provenance=False,
        )
        expect(
            compile_result.returncode != 0,
            f"expected negative metaprogramming fixture {negative_key} to fail compilation",
        )
        diagnostics_path = compile_dir / "module.diagnostics.txt"
        expect(diagnostics_path.is_file(), f"expected diagnostics for negative fixture {negative_key}")
        diagnostics_text = diagnostics_path.read_text(encoding="utf-8")
        expect(
            expected_code in diagnostics_text and expected_message in diagnostics_text,
            f"expected negative metaprogramming fixture {negative_key} to preserve {expected_code}",
        )
        negative_summary[negative_key] = {
            "fixture": str(fixture_path.relative_to(ROOT)).replace("\\", "/"),
            "diagnostics": str(diagnostics_path.relative_to(ROOT)).replace("\\", "/"),
            "expected_code": expected_code,
        }

    return CaseResult(
        case_id="metaprogramming-derive-property-behavior-semantics",
        probe="compile-manifest-derive-property-behavior-semantics",
        fixture="tests/tooling/fixtures/native/derive_expansion_inventory_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "derive_positive": {
                "fixture": str(derive_fixture.relative_to(ROOT)).replace("\\", "/"),
                "manifest": str(derive_manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "generated_method_entry_count": derive_surface.get(
                    "generated_method_entry_count"
                ),
                "expansion_inventory_rows_lexicographic": derive_surface.get(
                    "expansion_inventory_rows_lexicographic"
                ),
            },
            "property_positive": {
                "fixture": str(property_fixture.relative_to(ROOT)).replace("\\", "/"),
                "manifest": str(property_manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "supported_behavior_sites": property_surface.get(
                    "supported_behavior_sites"
                ),
                "observed_behavior_sites": property_surface.get(
                    "observed_behavior_sites"
                ),
                "projected_behavior_sites": property_surface.get(
                    "projected_behavior_sites"
                ),
            },
            "negative_cases": negative_summary,
        },
    )

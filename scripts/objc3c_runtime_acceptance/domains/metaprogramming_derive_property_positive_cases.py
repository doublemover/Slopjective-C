"""Positive derive/property surfaces for metaprogramming acceptance."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_outputs
from objc3c_runtime_acceptance.paths import ROOT


DERIVE_PROPERTY_DERIVE_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "derive_expansion_inventory_positive.objc3"
)
DERIVE_PROPERTY_BEHAVIOR_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "property_behavior_legality_positive.objc3"
)


def check_positive_derive_property_surfaces(case_dir: Path) -> dict[str, Any]:
    _, _, derive_manifest_path = compile_fixture_outputs(
        DERIVE_PROPERTY_DERIVE_FIXTURE, case_dir / "derive-positive" / "compile"
    )
    _, _, property_manifest_path = compile_fixture_outputs(
        DERIVE_PROPERTY_BEHAVIOR_FIXTURE,
        case_dir / "property-positive" / "compile",
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

    return {
        "derive_positive": {
            "fixture": str(DERIVE_PROPERTY_DERIVE_FIXTURE.relative_to(ROOT)).replace(
                "\\", "/"
            ),
            "manifest": str(derive_manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "generated_method_entry_count": derive_surface.get(
                "generated_method_entry_count"
            ),
            "expansion_inventory_rows_lexicographic": derive_surface.get(
                "expansion_inventory_rows_lexicographic"
            ),
        },
        "property_positive": {
            "fixture": str(DERIVE_PROPERTY_BEHAVIOR_FIXTURE.relative_to(ROOT)).replace(
                "\\", "/"
            ),
            "manifest": str(property_manifest_path.relative_to(ROOT)).replace(
                "\\", "/"
            ),
            "supported_behavior_sites": property_surface.get(
                "supported_behavior_sites"
            ),
            "observed_behavior_sites": property_surface.get("observed_behavior_sites"),
            "projected_behavior_sites": property_surface.get(
                "projected_behavior_sites"
            ),
        },
    }


__all__ = [
    "DERIVE_PROPERTY_BEHAVIOR_FIXTURE",
    "DERIVE_PROPERTY_DERIVE_FIXTURE",
    "check_positive_derive_property_surfaces",
]

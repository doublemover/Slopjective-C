"""Storage/reflection runtime acceptance case helpers."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.fixture_compilation import NegativeDiagnosticExpectation
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_expect_failure
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_outputs
from objc3c_runtime_acceptance.fixture_compilation import compile_negative_diagnostic_batch
from objc3c_runtime_acceptance.paths import ROOT

from ..runtime_contract_storage_reflection import (
    RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
)
from .storage_reflection_owner_contracts import storage_reflection_case_summary


PROPERTY_IVAR_ORDERING_SEMANTICS_CASE_ID = "property-ivar-ordering-semantics"

def check_property_ivar_ordering_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / PROPERTY_IVAR_ORDERING_SEMANTICS_CASE_ID
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_ivar_source_model_completion_positive.objc3"
    )
    _, _, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))

    surface = manifest.get("runtime_property_ivar_storage_accessor_source_surface", {})
    expect(
        surface.get("layout_init_order_field")
        == "Objc3PropertyDecl.executable_ivar_init_order_index",
        "expected property/ivar storage source surface to publish the init-order field",
    )
    expect(
        surface.get("layout_destroy_order_field")
        == "Objc3PropertyDecl.executable_ivar_destroy_order_index",
        "expected property/ivar storage source surface to publish the destruction-order field",
    )
    expect(
        surface.get("storage_semantics_model")
        == "interface-owned-property-layout-slots-sizes-alignment-init-order-and-reverse-destruction-order-remain-deterministic-before-runtime-allocation",
        "expected property/ivar storage source surface to publish the init/destroy ordering model",
    )

    property_records = manifest.get("runtime_metadata_source_records", {}).get(
        "properties", []
    )
    ivar_records = manifest.get("runtime_metadata_source_records", {}).get(
        "ivars", []
    )
    expect(
        isinstance(property_records, list) and property_records,
        "expected property ordering fixture to publish property source records",
    )
    expect(
        isinstance(ivar_records, list) and ivar_records,
        "expected property ordering fixture to publish ivar source records",
    )

    property_index = {
        (record.get("owner_kind"), record.get("owner_name"), record.get("property_name")): record
        for record in property_records
        if isinstance(record, dict)
    }
    ivar_index = {
        (record.get("owner_kind"), record.get("owner_name"), record.get("property_name")): record
        for record in ivar_records
        if isinstance(record, dict)
    }
    expected_property_records = (
        ("class-interface", "Widget", "token", 0, 2),
        ("class-interface", "Widget", "value", 1, 1),
        ("class-interface", "Widget", "count", 2, 0),
        ("class-implementation", "Widget", "token", 0, 2),
        ("class-implementation", "Widget", "value", 1, 1),
        ("class-implementation", "Widget", "count", 2, 0),
    )
    for owner_kind, owner_name, property_name, init_index, destroy_index in expected_property_records:
        record = property_index.get((owner_kind, owner_name, property_name), {})
        expect(
            record.get("executable_ivar_layout_slot_index") == init_index,
            f"expected {owner_kind} {owner_name}.{property_name} to preserve slot index {init_index}",
        )
        expect(
            record.get("executable_ivar_init_order_index") == init_index,
            f"expected {owner_kind} {owner_name}.{property_name} to preserve init order {init_index}",
        )
        expect(
            record.get("executable_ivar_destroy_order_index") == destroy_index,
            f"expected {owner_kind} {owner_name}.{property_name} to preserve destruction order {destroy_index}",
        )

    expected_ivar_records = (
        ("class-interface", "Widget", "token", 0, 2),
        ("class-interface", "Widget", "value", 1, 1),
        ("class-interface", "Widget", "count", 2, 0),
    )
    for owner_kind, owner_name, property_name, init_index, destroy_index in expected_ivar_records:
        record = ivar_index.get((owner_kind, owner_name, property_name), {})
        expect(
            record.get("executable_ivar_init_order_index") == init_index,
            f"expected ivar record for {owner_name}.{property_name} to preserve init order {init_index}",
        )
        expect(
            record.get("executable_ivar_destroy_order_index") == destroy_index,
            f"expected ivar record for {owner_name}.{property_name} to preserve destruction order {destroy_index}",
        )

    interface_property_records = [
        property_index[( "class-interface", "Widget", property_name)]
        for property_name in ("token", "value", "count")
    ]
    implementation_property_records = [
        property_index[( "class-implementation", "Widget", property_name)]
        for property_name in ("token", "value", "count")
    ]
    interface_init_order = [
        record.get("executable_ivar_init_order_index")
        for record in interface_property_records
    ]
    interface_destroy_order = [
        record.get("executable_ivar_destroy_order_index")
        for record in interface_property_records
    ]
    implementation_init_order = [
        record.get("executable_ivar_init_order_index")
        for record in implementation_property_records
    ]
    implementation_destroy_order = [
        record.get("executable_ivar_destroy_order_index")
        for record in implementation_property_records
    ]
    expect(
        interface_init_order == [0, 1, 2],
        "expected interface property init order to remain monotonic",
    )
    expect(
        interface_destroy_order == [2, 1, 0],
        "expected interface property destruction order to remain reverse-monotonic",
    )
    expect(
        implementation_init_order == [0, 1, 2],
        "expected implementation property init order to match interface ordering",
    )
    expect(
        implementation_destroy_order == [2, 1, 0],
        "expected implementation property destruction order to match interface reverse ordering",
    )

    return CaseResult(
        case_id=PROPERTY_IVAR_ORDERING_SEMANTICS_CASE_ID,
        probe="compile-manifest-source-records",
        fixture="tests/tooling/fixtures/native/property_ivar_source_model_completion_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=storage_reflection_case_summary(
            PROPERTY_IVAR_ORDERING_SEMANTICS_CASE_ID,
            {
                "property_record_count": len(property_records),
                "ivar_record_count": len(ivar_records),
                "token_destroy_order_index": property_index.get(
                    ("class-interface", "Widget", "token"), {}
                ).get("executable_ivar_destroy_order_index"),
                "count_init_order_index": property_index.get(
                    ("class-interface", "Widget", "count"), {}
                ).get("executable_ivar_init_order_index"),
                "interface_init_order": interface_init_order,
                "interface_destroy_order": interface_destroy_order,
                "implementation_init_order": implementation_init_order,
                "implementation_destroy_order": implementation_destroy_order,
            },
        ),
    )

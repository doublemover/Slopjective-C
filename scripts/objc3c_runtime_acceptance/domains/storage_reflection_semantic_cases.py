"""Storage/reflection runtime acceptance case helpers."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import NegativeDiagnosticExpectation
from objc3c_runtime_acceptance.native_build import compile_fixture_expect_failure
from objc3c_runtime_acceptance.native_build import compile_fixture_outputs
from objc3c_runtime_acceptance.native_build import compile_negative_diagnostic_batch

from ..core import (
    ROOT,
    RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
)

def check_storage_legality_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "storage-legality-semantics"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "runtime_backed_storage_ownership_legality_positive.objc3"
    )
    _, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "positive")
    registration_manifest_path = (
        case_dir / "positive" / "module.runtime-registration-manifest.json"
    )
    if not registration_manifest_path.is_file():
        raise RuntimeError(
            f"compiled fixture did not publish {registration_manifest_path}"
        )
    registration_manifest = json.loads(
        registration_manifest_path.read_text(encoding="utf-8")
    )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    sema_pass_manager_manifest = (
        manifest.get("frontend", {})
        .get("pipeline", {})
        .get("sema_pass_manager", {})
    )
    ll_text = ll_path.read_text(encoding="utf-8")

    expect(
        registration_manifest.get("property_descriptor_count") == 10,
        "expected storage legality positive fixture to publish ten property descriptors",
    )
    expect(
        registration_manifest.get("ivar_descriptor_count") == 5,
        "expected storage legality positive fixture to publish five ivar descriptors",
    )
    expect(
        manifest.get("runtime_property_ivar_storage_accessor_source_surface", {}).get(
            "contract_id"
        )
        == RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        "expected storage legality positive fixture to publish the property/ivar/storage/accessor source surface",
    )
    expect(
        manifest.get(
            "runtime_property_atomicity_synthesis_reflection_source_surface", {}
        ).get("contract_id")
        == RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID,
        "expected storage legality positive fixture to publish the property atomicity/synthesis/reflection source surface",
    )
    expect(
        sema_pass_manager_manifest.get("runtime_export_boundary_ready") is True,
        "expected storage legality positive fixture to publish a ready runtime export legality boundary",
    )
    expect(
        sema_pass_manager_manifest.get("runtime_export_property_attribute_invalid_entries")
        == 0,
        "expected storage legality positive fixture to publish zero invalid property-attribute entries",
    )
    expect(
        sema_pass_manager_manifest.get(
            "runtime_export_property_attribute_contract_violations"
        )
        == 0,
        "expected storage legality positive fixture to publish zero property contract violations",
    )
    expect(
        sema_pass_manager_manifest.get("runtime_export_property_ivar_binding_missing")
        == 0,
        "expected storage legality positive fixture to publish zero missing property ivar bindings",
    )
    expect(
        sema_pass_manager_manifest.get(
            "runtime_export_property_ivar_binding_conflicts"
        )
        == 0,
        "expected storage legality positive fixture to publish zero conflicting property ivar bindings",
    )
    for needle, label in (
        ("runtime_backed_storage_ownership_legality", "runtime-backed storage ownership legality"),
        ("property_attribute_profiles=10", "ten property-attribute profiles"),
        ("accessor_ownership_profiles=10", "ten accessor ownership profiles"),
    ):
        expect(
            needle in ll_text,
            f"expected storage legality positive fixture to publish {label} in LLVM IR",
        )

    negative_batch = compile_negative_diagnostic_batch(
        case_id="storage-legality-semantics",
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=[
            NegativeDiagnosticExpectation(
                key="negative-atomic-ownership",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "property_atomic_ownership_negative.objc3",
                expected_snippets=[
                    "atomic ownership-aware property 'value' in interface 'Widget' is unsupported until executable accessor storage semantics land"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="negative-weak-mismatch",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "runtime_backed_storage_ownership_weak_mismatch_negative.objc3",
                expected_snippets=[
                    "property ownership qualifier '__weak' conflicts with @property ownership modifier 'assign'"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="negative-unowned-mismatch",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "runtime_backed_storage_ownership_unowned_mismatch_negative.objc3",
                expected_snippets=[
                    "property ownership qualifier '__unsafe_unretained' conflicts with @property ownership modifier 'unowned'"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="negative-scalar-ownership",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "property_scalar_ownership_negative.objc3",
                expected_snippets=[
                    "@property ownership modifier 'strong' requires an Objective-C object property"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="negative-duplicate-getter",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "accessor_duplicate_getter_negative.objc3",
                expected_snippets=[
                    "duplicate effective getter selector 'value' for properties 'token' and 'alias'"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="negative-duplicate-setter",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "accessor_duplicate_setter_negative.objc3",
                expected_snippets=[
                    "duplicate effective setter selector 'setValue:' for properties 'token' and 'alias'"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="negative-readonly-setter",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "property_readonly_setter_negative.objc3",
                expected_snippets=[
                    "readonly property 'value' in interface 'Widget' must not declare a setter modifier"
                ],
                expected_codes=["O3S206"],
            ),
        ],
    )
    storage_negative_results = {
        str(entry["key"]): entry for entry in negative_batch["results"]
    }

    return CaseResult(
        case_id="storage-legality-semantics",
        probe="compile-manifest-and-diagnostics",
        fixture="tests/tooling/fixtures/native/runtime_backed_storage_ownership_legality_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "property_descriptor_count": registration_manifest.get("property_descriptor_count"),
            "ivar_descriptor_count": registration_manifest.get("ivar_descriptor_count"),
            "runtime_export_property_attribute_invalid_entries": sema_pass_manager_manifest.get(
                "runtime_export_property_attribute_invalid_entries"
            ),
            "runtime_export_property_attribute_contract_violations": sema_pass_manager_manifest.get(
                "runtime_export_property_attribute_contract_violations"
            ),
            "atomic_negative_diagnostic_count": storage_negative_results[
                "negative-atomic-ownership"
            ]["diagnostic_count"],
            "weak_mismatch_diagnostic_count": storage_negative_results[
                "negative-weak-mismatch"
            ]["diagnostic_count"],
            "unowned_mismatch_diagnostic_count": storage_negative_results[
                "negative-unowned-mismatch"
            ]["diagnostic_count"],
            "scalar_ownership_negative_diagnostic_count": storage_negative_results[
                "negative-scalar-ownership"
            ]["diagnostic_count"],
            "duplicate_getter_negative_diagnostic_count": storage_negative_results[
                "negative-duplicate-getter"
            ][
                "diagnostic_count"
            ],
            "duplicate_setter_negative_diagnostic_count": storage_negative_results[
                "negative-duplicate-setter"
            ][
                "diagnostic_count"
            ],
            "readonly_setter_negative_diagnostic_count": storage_negative_results[
                "negative-readonly-setter"
            ][
                "diagnostic_count"
            ],
            "negative_diagnostics_batch": negative_batch,
        },
    )


def check_property_synthesis_storage_binding_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-synthesis-storage-binding-semantics"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_synthesis_default_ivar_binding_no_redeclaration.objc3"
    )
    _, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "positive")
    registration_manifest_path = (
        case_dir / "positive" / "module.runtime-registration-manifest.json"
    )
    if not registration_manifest_path.is_file():
        raise RuntimeError(
            f"compiled fixture did not publish {registration_manifest_path}"
        )
    registration_manifest = json.loads(
        registration_manifest_path.read_text(encoding="utf-8")
    )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    ll_text = ll_path.read_text(encoding="utf-8")
    lowering_surface = manifest.get("dispatch_and_synthesized_accessor_lowering_surface", {})
    expect(
        isinstance(lowering_surface, dict),
        "expected property synthesis/storage-binding positive fixture to publish the lowering surface",
    )
    expect(
        lowering_surface.get("property_synthesis_sites") == 2,
        "expected no-redeclaration property synthesis fixture to publish two synthesis sites",
    )
    expect(
        lowering_surface.get("property_synthesis_default_ivar_bindings") == 2,
        "expected no-redeclaration property synthesis fixture to publish two default ivar bindings",
    )
    expect(
        lowering_surface.get("interface_owned_property_synthesis_sites") == 2,
        "expected no-redeclaration property synthesis fixture to publish two interface-owned synthesis sites",
    )
    expect(
        lowering_surface.get("implementation_property_redeclaration_sites") == 0,
        "expected no-redeclaration property synthesis fixture to publish zero implementation redeclaration sites",
    )
    expect(
        lowering_surface.get("ivar_binding_resolved") == 2,
        "expected no-redeclaration property synthesis fixture to publish two resolved ivar bindings",
    )
    expect(
        lowering_surface.get("synthesized_accessor_owner_entries") == 2,
        "expected no-redeclaration property synthesis fixture to publish two synthesized accessor owner entries",
    )
    expect(
        lowering_surface.get("synthesized_getter_entries") == 2,
        "expected no-redeclaration property synthesis fixture to publish two synthesized getter entries",
    )
    expect(
        lowering_surface.get("synthesized_setter_entries") == 1,
        "expected no-redeclaration property synthesis fixture to publish one synthesized setter entry",
    )
    expect(
        lowering_surface.get("current_property_read_entries") == 2,
        "expected no-redeclaration property synthesis fixture to route both getters through current-property reads",
    )
    expect(
        lowering_surface.get("current_property_exchange_entries") == 1,
        "expected no-redeclaration property synthesis fixture to route the strong setter through current-property exchange",
    )
    expect(
        lowering_surface.get("current_property_write_entries") == 0,
        "expected no-redeclaration property synthesis fixture to avoid plain current-property writes for the strong setter path",
    )
    expect(
        lowering_surface.get("weak_current_property_load_entries") == 0
        and lowering_surface.get("weak_current_property_store_entries") == 0,
        "expected no-redeclaration property synthesis fixture to avoid weak helper selection",
    )
    expect(
        registration_manifest.get("property_descriptor_count") == 2,
        "expected no-redeclaration property synthesis fixture to publish two property descriptors",
    )
    expect(
        registration_manifest.get("ivar_descriptor_count") == 2,
        "expected no-redeclaration property synthesis fixture to publish two ivar descriptors",
    )
    replay_key = manifest.get("lowering_property_synthesis_ivar_binding", {}).get(
        "replay_key", ""
    )
    for snippet, label in (
        (
            "interface_owned_property_synthesis_sites=2",
            "two interface-owned synthesis sites in the replay key",
        ),
        (
            "implementation_property_redeclaration_sites=0",
            "zero implementation redeclaration sites in the replay key",
        ),
        (
            "define void @objc3_method_Widget_instance_setCurrentValue_(i32 %arg0)",
            "the synthesized setter definition",
        ),
        (
            "call i32 @objc3_runtime_exchange_current_property_i32(i32 %objc3_property_retained)",
            "the runtime-backed setter exchange path",
        ),
    ):
        expect(
            snippet in (replay_key if "sites=" in snippet else ll_text),
            f"expected no-redeclaration property synthesis fixture to publish {label}",
        )

    incompatible_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_synthesis_default_ivar_binding_incompatible_redeclaration.objc3",
        case_dir / "negative-incompatible-redeclaration",
        expected_snippets=[
            "type mismatch: property synthesis for 'token' in implementation 'Widget' drifted from the interface default ivar binding",
            "type mismatch: incompatible property signature for 'token' in implementation 'Widget'",
        ],
        expected_codes=["O3S206"],
    )

    return CaseResult(
        case_id="property-synthesis-storage-binding-semantics",
        probe="compile-manifest-and-diagnostics",
        fixture="tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "property_synthesis_sites": lowering_surface.get("property_synthesis_sites"),
            "interface_owned_property_synthesis_sites": lowering_surface.get(
                "interface_owned_property_synthesis_sites"
            ),
            "implementation_property_redeclaration_sites": lowering_surface.get(
                "implementation_property_redeclaration_sites"
            ),
            "synthesized_getter_entries": lowering_surface.get(
                "synthesized_getter_entries"
            ),
            "synthesized_setter_entries": lowering_surface.get(
                "synthesized_setter_entries"
            ),
            "current_property_exchange_entries": lowering_surface.get(
                "current_property_exchange_entries"
            ),
            "negative_incompatible_redeclaration_diagnostic_count": incompatible_negative[
                "diagnostic_count"
            ],
        },
    )


def check_property_reflection_accessor_compatibility_diagnostics_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "property-reflection-accessor-compatibility-diagnostics"
    negative_batch = compile_negative_diagnostic_batch(
        case_id="property-reflection-accessor-compatibility-diagnostics",
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=[
            NegativeDiagnosticExpectation(
                key="accessor-selector-mismatch",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "property_accessor_selector_compatibility_negative.objc3",
                expected_snippets=[
                    "type mismatch: effective getter selector profile for property 'value' in implementation 'Widget' drifted from the interface declaration",
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="setter-selector-mismatch",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "property_setter_selector_compatibility_negative.objc3",
                expected_snippets=[
                    "type mismatch: effective setter selector profile for property 'value' in implementation 'Widget' drifted from the interface declaration",
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="reflection-attribute-mismatch",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "property_reflection_attribute_compatibility_negative.objc3",
                expected_snippets=[
                    "type mismatch: reflected property attribute and ownership profile for property 'value' in implementation 'Widget' drifted from the interface declaration",
                ],
                expected_codes=["O3S206"],
            ),
        ],
    )
    negative_results = {str(entry["key"]): entry for entry in negative_batch["results"]}

    return CaseResult(
        case_id="property-reflection-accessor-compatibility-diagnostics",
        probe="compile-diagnostics",
        fixture="tests/tooling/fixtures/native/property_accessor_selector_compatibility_negative.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "getter_selector_negative_diagnostic_count": negative_results[
                "accessor-selector-mismatch"
            ][
                "diagnostic_count"
            ],
            "setter_selector_negative_diagnostic_count": negative_results[
                "setter-selector-mismatch"
            ][
                "diagnostic_count"
            ],
            "reflection_attribute_negative_diagnostic_count": negative_results[
                "reflection-attribute-mismatch"
            ][
                "diagnostic_count"
            ],
            "negative_diagnostics_batch": negative_batch,
        },
    )


def check_property_ivar_ordering_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-ivar-ordering-semantics"
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
        case_id="property-ivar-ordering-semantics",
        probe="compile-manifest-source-records",
        fixture="tests/tooling/fixtures/native/property_ivar_source_model_completion_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
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
    )

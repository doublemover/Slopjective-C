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


STORAGE_LEGALITY_SEMANTICS_CASE_ID = "storage-legality-semantics"

def check_storage_legality_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / STORAGE_LEGALITY_SEMANTICS_CASE_ID
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
        case_id=STORAGE_LEGALITY_SEMANTICS_CASE_ID,
        probe="compile-manifest-and-diagnostics",
        fixture="tests/tooling/fixtures/native/runtime_backed_storage_ownership_legality_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=storage_reflection_case_summary(
            STORAGE_LEGALITY_SEMANTICS_CASE_ID,
            {
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
        ),
    )



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


PROPERTY_SYNTHESIS_STORAGE_BINDING_SEMANTICS_CASE_ID = (
    "property-synthesis-storage-binding-semantics"
)

def check_property_synthesis_storage_binding_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / PROPERTY_SYNTHESIS_STORAGE_BINDING_SEMANTICS_CASE_ID
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
        case_id=PROPERTY_SYNTHESIS_STORAGE_BINDING_SEMANTICS_CASE_ID,
        probe="compile-manifest-and-diagnostics",
        fixture="tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=storage_reflection_case_summary(
            PROPERTY_SYNTHESIS_STORAGE_BINDING_SEMANTICS_CASE_ID,
            {
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
        ),
    )

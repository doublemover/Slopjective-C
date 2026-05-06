"""Storage/reflection runtime acceptance case helpers."""

from __future__ import annotations

import json
from pathlib import Path
from time import perf_counter
from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import compile_fixture_with_args

from ..core import (
    ROOT,
    RUNTIME_CROSS_MODULE_STORAGE_REFLECTION_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
    STORAGE_REFLECTION_PRESERVATION_CONSUMER_FIXTURE,
    STORAGE_REFLECTION_PRESERVATION_PROVIDER_FIXTURE,
)

def check_cross_module_storage_reflection_artifact_preservation_case(
    run_dir: Path,
) -> CaseResult:
    case_started = perf_counter()
    case_dir = run_dir / "cross-module-storage-reflection-artifact-preservation"
    provider_fixture = ROOT / Path(STORAGE_REFLECTION_PRESERVATION_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(STORAGE_REFLECTION_PRESERVATION_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    provider_compile_started = perf_counter()
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_compile_ms = int((perf_counter() - provider_compile_started) * 1000)
    provider_import_surface = provider_compile_dir / "module.runtime-import-surface.json"
    provider_import_payload = json.loads(
        provider_import_surface.read_text(encoding="utf-8")
    )
    provider_registration_manifest = json.loads(
        (provider_compile_dir / "module.runtime-registration-manifest.json").read_text(
            encoding="utf-8"
        )
    )
    provider_storage_surface = provider_import_payload.get(
        "objc_runtime_storage_reflection_artifact_preservation", {}
    )
    expect(
        isinstance(provider_storage_surface, dict),
        "expected storage-reflection provider import surface to publish the preservation packet",
    )
    expected_provider_storage_fields = {
        "contract_id": RUNTIME_CROSS_MODULE_STORAGE_REFLECTION_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        "source_contract_id": RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        "dispatch_and_synthesized_accessor_lowering_surface_contract_id": "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
        "executable_property_accessor_layout_lowering_contract_id": "objc3c.executable.property.accessor.layout.lowering.v1",
        "executable_ivar_layout_emission_contract_id": "objc3c.executable.ivar.layout.emission.v1",
        "executable_synthesized_accessor_property_lowering_contract_id": "objc3c.executable.synthesized.accessor.property.lowering.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_runtime_storage_reflection_artifact_preservation",
        "import_artifact_member_name": "objc_runtime_storage_reflection_artifact_preservation",
        "source_model": "runtime-metadata-source-records-preserve-property-ivar-accessor-layout-and-runtime-helper-facts-for-separate-compilation",
        "preservation_model": "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-property-ivar-accessor-layout-and-runtime-helper-facts-beyond-local-ir-object-emission",
        "fail_closed_model": "missing-or-drifted-storage-reflection-preservation-packets-disable-cross-module-storage-reflection-claims",
    }
    for field_name, expected_value in expected_provider_storage_fields.items():
        expect(
            provider_storage_surface.get(field_name) == expected_value,
            f"expected storage-reflection provider import surface to preserve {field_name}",
        )
    expect(
        provider_storage_surface.get("local_property_descriptor_count")
        == provider_registration_manifest.get("property_descriptor_count")
        == 6,
        "expected storage-reflection provider import surface to preserve six property descriptors",
    )
    expect(
        provider_storage_surface.get("local_ivar_descriptor_count")
        == provider_registration_manifest.get("ivar_descriptor_count")
        == 3,
        "expected storage-reflection provider import surface to preserve three ivar descriptors",
    )
    for field_name, expected_value in (
        ("implementation_owned_property_entries", 3),
        ("synthesized_accessor_owner_entries", 3),
        ("synthesized_getter_entries", 3),
        ("synthesized_setter_entries", 3),
        ("synthesized_accessor_entries", 6),
        ("current_property_read_entries", 3),
        ("current_property_write_entries", 2),
        ("current_property_exchange_entries", 1),
        ("weak_current_property_load_entries", 0),
        ("weak_current_property_store_entries", 0),
        ("ivar_layout_entries", 3),
        ("ivar_layout_owner_entries", 1),
    ):
        expect(
            provider_storage_surface.get(field_name) == expected_value,
            f"expected storage-reflection provider import surface to preserve {field_name}",
        )
    expect(
        provider_storage_surface.get("runtime_import_artifact_ready") is True
        and provider_storage_surface.get("separate_compilation_preservation_ready")
        is True
        and provider_storage_surface.get("deterministic") is True,
        "expected storage-reflection provider import surface to be import-ready deterministic and separate-compilation ready",
    )
    expect(
        isinstance(provider_storage_surface.get("replay_key"), str)
        and provider_storage_surface.get("replay_key") != "",
        "expected storage-reflection provider import surface to publish a replay key",
    )
    consumer_compile_dir = case_dir / "consumer"
    consumer_compile_started = perf_counter()
    compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_import_surface),
        ],
    )
    consumer_compile_ms = int((perf_counter() - consumer_compile_started) * 1000)
    consumer_registration_manifest = json.loads(
        (consumer_compile_dir / "module.runtime-registration-manifest.json").read_text(
            encoding="utf-8"
        )
    )
    link_plan = json.loads(
        (consumer_compile_dir / "module.cross-module-runtime-link-plan.json").read_text(
            encoding="utf-8"
        )
    )

    for field_name, expected_value in (
        (
            "runtime_cross_module_storage_reflection_artifact_preservation_surface_contract_id",
            RUNTIME_CROSS_MODULE_STORAGE_REFLECTION_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        ),
        (
            "runtime_property_ivar_storage_accessor_source_surface_contract_id",
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        ),
        (
            "dispatch_and_synthesized_accessor_lowering_surface_contract_id",
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
        ),
        (
            "executable_property_accessor_layout_lowering_contract_id",
            "objc3c.executable.property.accessor.layout.lowering.v1",
        ),
        (
            "executable_ivar_layout_emission_contract_id",
            "objc3c.executable.ivar.layout.emission.v1",
        ),
        (
            "executable_synthesized_accessor_property_lowering_contract_id",
            "objc3c.executable.synthesized.accessor.property.lowering.v1",
        ),
        (
            "storage_reflection_artifact_preservation_model",
            "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-property-ivar-accessor-layout-and-runtime-helper-facts-beyond-local-ir-object-emission",
        ),
    ):
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    expect(
        link_plan.get("storage_reflection_cross_module_preservation_ready") is True,
        "expected cross-module link plan to mark storage/reflection preservation ready",
    )

    imported_modules = link_plan.get("imported_modules")
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected storage-reflection link plan to publish exactly one imported module",
    )
    imported_module = imported_modules[0]
    expect(
        imported_module.get("module_name")
        == provider_import_payload.get("module_name")
        == "synthesizedAccessorPropertyLowering",
        "expected storage-reflection link plan to preserve the provider module name",
    )
    for field_name, expected_value in (
        ("storage_reflection_artifact_preservation_present", True),
        ("storage_reflection_runtime_import_artifact_ready", True),
        ("storage_reflection_separate_compilation_preservation_ready", True),
        ("storage_reflection_deterministic", True),
        (
            "storage_reflection_contract_id",
            RUNTIME_CROSS_MODULE_STORAGE_REFLECTION_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        ),
        (
            "storage_reflection_source_contract_id",
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        ),
        (
            "storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id",
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
        ),
        (
            "storage_reflection_executable_property_accessor_layout_lowering_contract_id",
            "objc3c.executable.property.accessor.layout.lowering.v1",
        ),
        (
            "storage_reflection_executable_ivar_layout_emission_contract_id",
            "objc3c.executable.ivar.layout.emission.v1",
        ),
        (
            "storage_reflection_executable_synthesized_accessor_property_lowering_contract_id",
            "objc3c.executable.synthesized.accessor.property.lowering.v1",
        ),
        ("storage_reflection_local_property_descriptor_count", 6),
        ("storage_reflection_local_ivar_descriptor_count", 3),
        ("storage_reflection_implementation_owned_property_entries", 3),
        ("storage_reflection_synthesized_accessor_owner_entries", 3),
        ("storage_reflection_synthesized_getter_entries", 3),
        ("storage_reflection_synthesized_setter_entries", 3),
        ("storage_reflection_synthesized_accessor_entries", 6),
        ("storage_reflection_current_property_read_entries", 3),
        ("storage_reflection_current_property_write_entries", 2),
        ("storage_reflection_current_property_exchange_entries", 1),
        ("storage_reflection_weak_current_property_load_entries", 0),
        ("storage_reflection_weak_current_property_store_entries", 0),
        ("storage_reflection_ivar_layout_entries", 3),
        ("storage_reflection_ivar_layout_owner_entries", 1),
    ):
        expect(
            imported_module.get(field_name) == expected_value,
            f"expected imported storage-reflection module to preserve {field_name}",
        )
    expect(
        isinstance(imported_module.get("storage_reflection_replay_key"), str)
        and imported_module.get("storage_reflection_replay_key") != "",
        "expected imported storage-reflection module to preserve a replay key",
    )

    local_expected = {
        "local_storage_reflection_implementation_owned_property_entries": 0,
        "local_storage_reflection_synthesized_accessor_owner_entries": 0,
        "local_storage_reflection_synthesized_getter_entries": 0,
        "local_storage_reflection_synthesized_setter_entries": 0,
        "local_storage_reflection_synthesized_accessor_entries": 0,
        "local_storage_reflection_current_property_read_entries": 0,
        "local_storage_reflection_current_property_write_entries": 0,
        "local_storage_reflection_current_property_exchange_entries": 0,
        "local_storage_reflection_weak_current_property_load_entries": 0,
        "local_storage_reflection_weak_current_property_store_entries": 0,
        "local_storage_reflection_ivar_layout_entries": 0,
        "local_storage_reflection_ivar_layout_owner_entries": 0,
    }
    imported_expected = {
        "imported_storage_reflection_implementation_owned_property_entries": 3,
        "imported_storage_reflection_synthesized_accessor_owner_entries": 3,
        "imported_storage_reflection_synthesized_getter_entries": 3,
        "imported_storage_reflection_synthesized_setter_entries": 3,
        "imported_storage_reflection_synthesized_accessor_entries": 6,
        "imported_storage_reflection_current_property_read_entries": 3,
        "imported_storage_reflection_current_property_write_entries": 2,
        "imported_storage_reflection_current_property_exchange_entries": 1,
        "imported_storage_reflection_weak_current_property_load_entries": 0,
        "imported_storage_reflection_weak_current_property_store_entries": 0,
        "imported_storage_reflection_ivar_layout_entries": 3,
        "imported_storage_reflection_ivar_layout_owner_entries": 1,
    }
    transitive_expected = {
        "transitive_storage_reflection_implementation_owned_property_entries": 3,
        "transitive_storage_reflection_synthesized_accessor_owner_entries": 3,
        "transitive_storage_reflection_synthesized_getter_entries": 3,
        "transitive_storage_reflection_synthesized_setter_entries": 3,
        "transitive_storage_reflection_synthesized_accessor_entries": 6,
        "transitive_storage_reflection_current_property_read_entries": 3,
        "transitive_storage_reflection_current_property_write_entries": 2,
        "transitive_storage_reflection_current_property_exchange_entries": 1,
        "transitive_storage_reflection_weak_current_property_load_entries": 0,
        "transitive_storage_reflection_weak_current_property_store_entries": 0,
        "transitive_storage_reflection_ivar_layout_entries": 3,
        "transitive_storage_reflection_ivar_layout_owner_entries": 1,
    }
    for field_name, expected_value in local_expected.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    for field_name, expected_value in imported_expected.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    for field_name, expected_value in transitive_expected.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )

    case_total_ms = int((perf_counter() - case_started) * 1000)
    return CaseResult(
        case_id="cross-module-storage-reflection-artifact-preservation",
        probe=None,
        fixture=STORAGE_REFLECTION_PRESERVATION_PROVIDER_FIXTURE,
        claim_class="compile-manifest-and-link-plan",
        passed=True,
        summary={
            "provider_fixture": STORAGE_REFLECTION_PRESERVATION_PROVIDER_FIXTURE,
            "consumer_fixture": STORAGE_REFLECTION_PRESERVATION_CONSUMER_FIXTURE,
            "provider_compile_ms": provider_compile_ms,
            "consumer_compile_ms": consumer_compile_ms,
            "case_total_ms": case_total_ms,
            "provider_module_name": provider_import_payload.get("module_name"),
            "consumer_module_name": link_plan.get("local_module", {}).get("module_name"),
            "imported_property_descriptor_count": link_plan.get("imported_property_descriptor_count"),
            "imported_ivar_descriptor_count": link_plan.get("imported_ivar_descriptor_count"),
            "imported_synthesized_accessor_entries": link_plan.get(
                "imported_storage_reflection_synthesized_accessor_entries"
            ),
            "imported_ivar_layout_entries": link_plan.get(
                "imported_storage_reflection_ivar_layout_entries"
            ),
        },
    )

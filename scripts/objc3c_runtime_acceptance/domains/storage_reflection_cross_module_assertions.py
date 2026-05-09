"""Assertions for cross-module storage/reflection preservation cases."""

from __future__ import annotations

from objc3c_runtime_acceptance.assertions import expect

from ..core import (
    RUNTIME_CROSS_MODULE_STORAGE_REFLECTION_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
)
from .storage_reflection_cross_module_artifacts import (
    CrossModuleStorageReflectionArtifacts,
)


_DISPATCH_AND_SYNTHESIZED_ACCESSOR_SURFACE_CONTRACT_ID = (
    "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1"
)
_EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_CONTRACT_ID = (
    "objc3c.executable.property.accessor.layout.lowering.v1"
)
_EXECUTABLE_IVAR_LAYOUT_EMISSION_CONTRACT_ID = (
    "objc3c.executable.ivar.layout.emission.v1"
)
_EXECUTABLE_SYNTHESIZED_ACCESSOR_PROPERTY_CONTRACT_ID = (
    "objc3c.executable.synthesized.accessor.property.lowering.v1"
)
_STORAGE_REFLECTION_PRESERVATION_MODEL = (
    "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-"
    "preserve-property-ivar-accessor-layout-and-runtime-helper-facts-beyond-local-"
    "ir-object-emission"
)


def assert_cross_module_storage_reflection_artifacts(
    artifacts: CrossModuleStorageReflectionArtifacts,
) -> None:
    _assert_provider_storage_surface(artifacts)
    _assert_cross_module_link_plan_contracts(artifacts)
    _assert_imported_module_storage_reflection_surface(artifacts)
    _assert_link_plan_storage_reflection_totals(artifacts)


def _assert_provider_storage_surface(
    artifacts: CrossModuleStorageReflectionArtifacts,
) -> None:
    provider_storage_surface = artifacts.provider_storage_surface
    expect(
        isinstance(provider_storage_surface, dict),
        "expected storage-reflection provider import surface to publish the preservation packet",
    )
    expected_provider_storage_fields = {
        "contract_id": RUNTIME_CROSS_MODULE_STORAGE_REFLECTION_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        "source_contract_id": RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        "dispatch_and_synthesized_accessor_lowering_surface_contract_id": _DISPATCH_AND_SYNTHESIZED_ACCESSOR_SURFACE_CONTRACT_ID,
        "executable_property_accessor_layout_lowering_contract_id": _EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_CONTRACT_ID,
        "executable_ivar_layout_emission_contract_id": _EXECUTABLE_IVAR_LAYOUT_EMISSION_CONTRACT_ID,
        "executable_synthesized_accessor_property_lowering_contract_id": _EXECUTABLE_SYNTHESIZED_ACCESSOR_PROPERTY_CONTRACT_ID,
        "surface_path": "frontend.pipeline.semantic_surface.objc_runtime_storage_reflection_artifact_preservation",
        "import_artifact_member_name": "objc_runtime_storage_reflection_artifact_preservation",
        "source_model": "runtime-metadata-source-records-preserve-property-ivar-accessor-layout-and-runtime-helper-facts-for-separate-compilation",
        "preservation_model": _STORAGE_REFLECTION_PRESERVATION_MODEL,
        "fail_closed_model": "missing-or-drifted-storage-reflection-preservation-packets-disable-cross-module-storage-reflection-claims",
    }
    for field_name, expected_value in expected_provider_storage_fields.items():
        expect(
            provider_storage_surface.get(field_name) == expected_value,
            f"expected storage-reflection provider import surface to preserve {field_name}",
        )

    expect(
        provider_storage_surface.get("local_property_descriptor_count")
        == artifacts.provider_registration_manifest.get("property_descriptor_count")
        == 6,
        "expected storage-reflection provider import surface to preserve six property descriptors",
    )
    expect(
        provider_storage_surface.get("local_ivar_descriptor_count")
        == artifacts.provider_registration_manifest.get("ivar_descriptor_count")
        == 3,
        "expected storage-reflection provider import surface to preserve three ivar descriptors",
    )
    for field_name, expected_value in _STORAGE_REFLECTION_ENTRY_COUNTS:
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


def _assert_cross_module_link_plan_contracts(
    artifacts: CrossModuleStorageReflectionArtifacts,
) -> None:
    link_plan = artifacts.link_plan
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
            _DISPATCH_AND_SYNTHESIZED_ACCESSOR_SURFACE_CONTRACT_ID,
        ),
        (
            "executable_property_accessor_layout_lowering_contract_id",
            _EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_CONTRACT_ID,
        ),
        (
            "executable_ivar_layout_emission_contract_id",
            _EXECUTABLE_IVAR_LAYOUT_EMISSION_CONTRACT_ID,
        ),
        (
            "executable_synthesized_accessor_property_lowering_contract_id",
            _EXECUTABLE_SYNTHESIZED_ACCESSOR_PROPERTY_CONTRACT_ID,
        ),
        (
            "storage_reflection_artifact_preservation_model",
            _STORAGE_REFLECTION_PRESERVATION_MODEL,
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


def _assert_imported_module_storage_reflection_surface(
    artifacts: CrossModuleStorageReflectionArtifacts,
) -> None:
    imported_modules = artifacts.link_plan.get("imported_modules")
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected storage-reflection link plan to publish exactly one imported module",
    )
    imported_module = imported_modules[0]
    expect(
        imported_module.get("module_name")
        == artifacts.provider_import_payload.get("module_name")
        == "synthesizedAccessorPropertyLowering",
        "expected storage-reflection link plan to preserve the provider module name",
    )
    for field_name, expected_value in _IMPORTED_MODULE_FIELDS:
        expect(
            imported_module.get(field_name) == expected_value,
            f"expected imported storage-reflection module to preserve {field_name}",
        )
    expect(
        isinstance(imported_module.get("storage_reflection_replay_key"), str)
        and imported_module.get("storage_reflection_replay_key") != "",
        "expected imported storage-reflection module to preserve a replay key",
    )


def _assert_link_plan_storage_reflection_totals(
    artifacts: CrossModuleStorageReflectionArtifacts,
) -> None:
    for field_name, expected_value in _LOCAL_STORAGE_REFLECTION_COUNTS.items():
        expect(
            artifacts.link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    for field_name, expected_value in _IMPORTED_STORAGE_REFLECTION_COUNTS.items():
        expect(
            artifacts.link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    for field_name, expected_value in _TRANSITIVE_STORAGE_REFLECTION_COUNTS.items():
        expect(
            artifacts.link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )


_STORAGE_REFLECTION_ENTRY_COUNTS = (
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
)
_IMPORTED_MODULE_FIELDS = (
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
        _DISPATCH_AND_SYNTHESIZED_ACCESSOR_SURFACE_CONTRACT_ID,
    ),
    (
        "storage_reflection_executable_property_accessor_layout_lowering_contract_id",
        _EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_CONTRACT_ID,
    ),
    (
        "storage_reflection_executable_ivar_layout_emission_contract_id",
        _EXECUTABLE_IVAR_LAYOUT_EMISSION_CONTRACT_ID,
    ),
    (
        "storage_reflection_executable_synthesized_accessor_property_lowering_contract_id",
        _EXECUTABLE_SYNTHESIZED_ACCESSOR_PROPERTY_CONTRACT_ID,
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
)
_LOCAL_STORAGE_REFLECTION_COUNTS = {
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
_IMPORTED_STORAGE_REFLECTION_COUNTS = {
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
_TRANSITIVE_STORAGE_REFLECTION_COUNTS = {
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


__all__ = ["assert_cross_module_storage_reflection_artifacts"]

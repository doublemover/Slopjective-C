"""Imported-runtime packaging replay link-plan assertions."""

from __future__ import annotations

from ..assertions import expect
from ..core import (
    RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
    RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
)
from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_artifacts import (
    ImportedRuntimePackagingReplayArtifacts,
)


def assert_imported_runtime_link_plan_contract(
    artifacts: ImportedRuntimePackagingReplayArtifacts,
) -> None:
    link_plan = artifacts.link_plan
    provider_import_payload = artifacts.provider_import_payload
    provider_registration_manifest = artifacts.provider_registration_manifest
    consumer_registration_manifest = artifacts.consumer_registration_manifest

    expect(
        link_plan.get("bootstrap_live_registration_contract_id")
        == "objc3c.runtime.live.registration.discovery.replay.v1",
        "expected cross-module link plan to preserve the live registration replay contract",
    )
    expect(
        link_plan.get("bootstrap_live_restart_hardening_contract_id")
        == "objc3c.runtime.live.restart.hardening.v1",
        "expected cross-module link plan to preserve the live restart hardening contract",
    )
    expect(
        link_plan.get("bootstrap_replay_registered_images_symbol")
        == "objc3_runtime_replay_registered_images_for_testing",
        "expected cross-module link plan to preserve the replay_registered_images symbol",
    )
    expect(
        link_plan.get("bootstrap_reset_replay_state_snapshot_symbol")
        == "objc3_runtime_copy_reset_replay_state_for_testing",
        "expected cross-module link plan to preserve the reset/replay snapshot symbol",
    )
    expect(
        link_plan.get("bootstrap_reset_for_testing_symbol")
        == "objc3_runtime_reset_for_testing",
        "expected cross-module link plan to preserve the reset_for_testing symbol",
    )
    expect(
        link_plan.get(
            "runtime_cross_module_realized_metadata_replay_preservation_surface_contract_id"
        )
        == RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
        "expected cross-module link plan to publish the realized-metadata replay preservation surface contract",
    )
    expect(
        link_plan.get("runtime_object_model_realization_source_surface_contract_id")
        == RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
        "expected cross-module link plan to preserve the object-model realization source contract",
    )
    expect(
        link_plan.get(
            "runtime_realization_lowering_reflection_artifact_surface_contract_id"
        )
        == RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
        "expected cross-module link plan to preserve the realization/reflection artifact surface contract",
    )
    expect(
        link_plan.get(
            "runtime_dispatch_table_reflection_record_lowering_surface_contract_id"
        )
        == RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        "expected cross-module link plan to preserve the dispatch/reflection-record lowering surface contract",
    )
    expect(
        link_plan.get("realized_metadata_replay_preservation_model")
        == "cross-module-link-plan-preserves-local-and-imported-realized-metadata-descriptor-counts-identities-and-reset-replay-readiness-from-runtime-registration-manifests",
        "expected cross-module link plan to publish the realized-metadata replay preservation model",
    )
    expect(
        link_plan.get("imported_live_registration_replay_ready") is True,
        "expected cross-module link plan to mark imported live registration replay ready",
    )
    expect(
        link_plan.get("imported_live_restart_hardening_ready") is True,
        "expected cross-module link plan to mark imported live restart hardening ready",
    )
    imported_modules = link_plan.get("imported_modules")
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected cross-module link plan to publish exactly one imported module",
    )
    imported_module = imported_modules[0]
    expect(
        provider_import_payload.get("module_name") == imported_module.get("module_name"),
        "expected imported runtime surface module name to match the cross-module link plan",
    )
    expect(
        imported_module.get("module_name") == "runtimePackagingProvider",
        "expected cross-module link plan to preserve the imported provider module name",
    )
    expect(
        imported_module.get("translation_unit_registration_order_ordinal") == 1,
        "expected imported module registration ordinal to be preserved in the link plan",
    )
    expect(
        imported_module.get("ready_for_live_registration_discovery_replay") is True,
        "expected imported module to preserve live registration replay readiness",
    )
    expect(
        imported_module.get("ready_for_live_restart_hardening") is True,
        "expected imported module to preserve live restart hardening readiness",
    )
    for field_name, expected_value in (
        (
            "bootstrap_live_registration_contract_id",
            "objc3c.runtime.live.registration.discovery.replay.v1",
        ),
        (
            "bootstrap_live_restart_hardening_contract_id",
            "objc3c.runtime.live.restart.hardening.v1",
        ),
        (
            "bootstrap_live_replay_registered_images_symbol",
            "objc3_runtime_replay_registered_images_for_testing",
        ),
        (
            "bootstrap_live_reset_replay_state_snapshot_symbol",
            "objc3_runtime_copy_reset_replay_state_for_testing",
        ),
        (
            "bootstrap_live_restart_reset_for_testing_symbol",
            "objc3_runtime_reset_for_testing",
        ),
        (
            "bootstrap_live_restart_replay_registered_images_symbol",
            "objc3_runtime_replay_registered_images_for_testing",
        ),
        (
            "bootstrap_live_restart_reset_replay_state_snapshot_symbol",
            "objc3_runtime_copy_reset_replay_state_for_testing",
        ),
    ):
        expect(
            imported_module.get(field_name) == expected_value,
            f"expected imported module to preserve {field_name}",
        )
    for field_name in (
        "class_descriptor_count",
        "protocol_descriptor_count",
        "category_descriptor_count",
        "property_descriptor_count",
        "ivar_descriptor_count",
        "total_descriptor_count",
    ):
        expect(
            imported_module.get(field_name)
            == provider_registration_manifest.get(field_name),
            f"expected imported module to preserve {field_name}",
        )
    local_module = link_plan.get("local_module")
    expect(
        isinstance(local_module, dict)
        and local_module.get("translation_unit_registration_order_ordinal") == 2,
        "expected cross-module link plan to preserve the local registration ordinal",
    )
    for field_name in (
        "class_descriptor_count",
        "protocol_descriptor_count",
        "category_descriptor_count",
        "property_descriptor_count",
        "ivar_descriptor_count",
        "total_descriptor_count",
    ):
        expect(
            local_module.get(field_name) == consumer_registration_manifest.get(field_name),
            f"expected local module to preserve {field_name}",
        )
    expected_imported_counts = {
        "imported_class_descriptor_count": provider_registration_manifest[
            "class_descriptor_count"
        ],
        "imported_protocol_descriptor_count": provider_registration_manifest[
            "protocol_descriptor_count"
        ],
        "imported_category_descriptor_count": provider_registration_manifest[
            "category_descriptor_count"
        ],
        "imported_property_descriptor_count": provider_registration_manifest[
            "property_descriptor_count"
        ],
        "imported_ivar_descriptor_count": provider_registration_manifest[
            "ivar_descriptor_count"
        ],
        "imported_total_descriptor_count": provider_registration_manifest[
            "total_descriptor_count"
        ],
    }
    expected_local_counts = {
        "local_class_descriptor_count": consumer_registration_manifest[
            "class_descriptor_count"
        ],
        "local_protocol_descriptor_count": consumer_registration_manifest[
            "protocol_descriptor_count"
        ],
        "local_category_descriptor_count": consumer_registration_manifest[
            "category_descriptor_count"
        ],
        "local_property_descriptor_count": consumer_registration_manifest[
            "property_descriptor_count"
        ],
        "local_ivar_descriptor_count": consumer_registration_manifest[
            "ivar_descriptor_count"
        ],
        "local_total_descriptor_count": consumer_registration_manifest[
            "total_descriptor_count"
        ],
    }
    for field_name, expected_value in expected_imported_counts.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    for field_name, expected_value in expected_local_counts.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    expect(
        link_plan.get("module_image_count") == 2,
        "expected cross-module link plan to preserve a two-image runtime topology",
    )
    expect(
        link_plan.get("module_names_lexicographic")
        == ["runtimePackagingConsumer", "runtimePackagingProvider"],
        "expected cross-module link plan to preserve the stable module-name ordering",
    )
    expect(
        link_plan.get("direct_import_input_count") == 1,
        "expected cross-module link plan to preserve one direct imported runtime surface",
    )
    direct_import_surface_artifact_paths = link_plan.get(
        "direct_import_surface_artifact_paths"
    )
    expect(
        isinstance(direct_import_surface_artifact_paths, list)
        and len(direct_import_surface_artifact_paths) == 1
        and direct_import_surface_artifact_paths[0].endswith(
            "provider/module.runtime-import-surface.json"
        ),
        "expected cross-module link plan to preserve the imported runtime surface artifact path",
    )
    for field_name, expected_value in (
        (
            "transitive_class_descriptor_count",
            provider_registration_manifest["class_descriptor_count"]
            + consumer_registration_manifest["class_descriptor_count"],
        ),
        (
            "transitive_protocol_descriptor_count",
            provider_registration_manifest["protocol_descriptor_count"]
            + consumer_registration_manifest["protocol_descriptor_count"],
        ),
        (
            "transitive_category_descriptor_count",
            provider_registration_manifest["category_descriptor_count"]
            + consumer_registration_manifest["category_descriptor_count"],
        ),
        (
            "transitive_property_descriptor_count",
            provider_registration_manifest["property_descriptor_count"]
            + consumer_registration_manifest["property_descriptor_count"],
        ),
        (
            "transitive_ivar_descriptor_count",
            provider_registration_manifest["ivar_descriptor_count"]
            + consumer_registration_manifest["ivar_descriptor_count"],
        ),
        (
            "transitive_total_descriptor_count",
            provider_registration_manifest["total_descriptor_count"]
            + consumer_registration_manifest["total_descriptor_count"],
        ),
    ):
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    link_object_artifacts = link_plan.get("link_object_artifacts")
    expect(
        isinstance(link_object_artifacts, list) and len(link_object_artifacts) == 2,
        "expected cross-module link plan to publish two ordered link objects",
    )


__all__ = ["assert_imported_runtime_link_plan_contract"]

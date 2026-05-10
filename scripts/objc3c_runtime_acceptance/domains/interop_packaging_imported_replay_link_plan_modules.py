"""Imported-runtime packaging replay link-plan module assertions."""

from __future__ import annotations

from typing import Any

from ..expectation_matching import expect

_DESCRIPTOR_COUNT_FIELDS = (
    "class_descriptor_count",
    "protocol_descriptor_count",
    "category_descriptor_count",
    "property_descriptor_count",
    "ivar_descriptor_count",
    "total_descriptor_count",
)


def assert_imported_runtime_link_plan_modules(
    link_plan: dict[str, Any],
    provider_import_payload: dict[str, Any],
    provider_registration_manifest: dict[str, Any],
    consumer_registration_manifest: dict[str, Any],
) -> None:
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
    for field_name in _DESCRIPTOR_COUNT_FIELDS:
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
    for field_name in _DESCRIPTOR_COUNT_FIELDS:
        expect(
            local_module.get(field_name) == consumer_registration_manifest.get(field_name),
            f"expected local module to preserve {field_name}",
        )


__all__ = ["assert_imported_runtime_link_plan_modules"]

"""Concurrency actor cross-module import and link-plan assertions."""

from __future__ import annotations

from collections.abc import Mapping

from objc3c_runtime_acceptance.expectation_matching import expect

EXPECTED_PROVIDER_ACTOR_FIELDS = {
    "contract_id": "objc3c.concurrency.actor.mailbox.isolation.import.surface.v1",
    "source_contract_id": "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
    "surface_path": "frontend.pipeline.semantic_surface.objc_concurrency_actor_mailbox_and_isolation_runtime_import_surface",
    "source_model": "runtime-import-surface-preserves-actor-lowering-and-isolation-replay-facts-for-cross-module-runtime-link-planning",
    "fail_closed_model": "missing-or-drifted-actor-mailbox-runtime-import-packets-disable-cross-module-actor-runtime-preservation-claims",
}

DESCRIPTOR_COUNT_FIELDS = (
    "class_descriptor_count",
    "protocol_descriptor_count",
    "category_descriptor_count",
    "property_descriptor_count",
    "ivar_descriptor_count",
    "total_descriptor_count",
)


def expect_provider_actor_import_surface(
    provider_actor_surface: Mapping[str, object],
) -> None:
    expect(
        isinstance(provider_actor_surface, dict),
        "expected concurrency actor provider import surface to publish the actor mailbox preservation packet",
    )
    for field_name, expected_value in EXPECTED_PROVIDER_ACTOR_FIELDS.items():
        expect(
            provider_actor_surface.get(field_name) == expected_value,
            f"expected concurrency actor provider import surface to preserve {field_name}",
        )
    expect(
        provider_actor_surface.get("actor_mailbox_runtime_ready") is True
        and provider_actor_surface.get("deterministic") is True,
        "expected concurrency actor provider import surface to be runtime-ready and deterministic",
    )
    for field_name in (
        "replay_key",
        "actor_lowering_replay_key",
        "actor_isolation_lowering_replay_key",
    ):
        expect(
            isinstance(provider_actor_surface.get(field_name), str)
            and provider_actor_surface.get(field_name) != "",
            f"expected concurrency actor provider import surface to publish {field_name}",
        )


def expect_actor_cross_module_link_plan(
    link_plan: Mapping[str, object],
    provider_import_payload: Mapping[str, object],
    provider_registration_manifest: Mapping[str, object],
    consumer_registration_manifest: Mapping[str, object],
) -> tuple[Mapping[str, object], Mapping[str, object]]:
    expect(
        link_plan.get("concurrency_actor_imported_module_count") == 1,
        "expected cross-module link plan to publish one imported actor module",
    )
    expect(
        link_plan.get("concurrency_actor_imported_module_names_lexicographic")
        == [provider_import_payload.get("module_name")],
        "expected cross-module link plan to preserve the imported actor module names",
    )
    expect(
        link_plan.get("concurrency_actor_cross_module_isolation_ready") is True,
        "expected cross-module link plan to mark actor cross-module isolation ready",
    )
    expect(
        link_plan.get("expected_concurrency_actor_contract_id")
        == "objc3c.concurrency.actor.mailbox.isolation.import.surface.v1",
        "expected cross-module link plan to preserve the actor import contract id",
    )
    expect(
        link_plan.get("expected_concurrency_actor_source_contract_id")
        == "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
        "expected cross-module link plan to preserve the actor source contract id",
    )

    imported_modules = link_plan.get("imported_modules")
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected cross-module actor link plan to publish exactly one imported module",
    )
    imported_module = imported_modules[0]
    expect(
        imported_module.get("module_name") == provider_import_payload.get("module_name"),
        "expected imported actor module name to match the provider import surface",
    )
    expect(
        imported_module.get("translation_unit_registration_order_ordinal") == 1,
        "expected imported actor module registration ordinal to remain one",
    )
    expect(
        imported_module.get("concurrency_actor_mailbox_runtime_import_present") is True
        and imported_module.get("concurrency_actor_mailbox_runtime_ready") is True
        and imported_module.get("concurrency_actor_mailbox_runtime_deterministic")
        is True,
        "expected imported actor module to preserve actor runtime import readiness",
    )
    for field_name, expected_value in (
        (
            "concurrency_actor_contract_id",
            "objc3c.concurrency.actor.mailbox.isolation.import.surface.v1",
        ),
        (
            "concurrency_actor_source_contract_id",
            "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
        ),
    ):
        expect(
            imported_module.get(field_name) == expected_value,
            f"expected imported actor module to preserve {field_name}",
        )
    for field_name in (
        "concurrency_actor_mailbox_runtime_replay_key",
        "concurrency_actor_lowering_replay_key",
        "concurrency_actor_isolation_lowering_replay_key",
    ):
        expect(
            isinstance(imported_module.get(field_name), str)
            and imported_module.get(field_name) != "",
            f"expected imported actor module to preserve {field_name}",
        )
    for field_name in DESCRIPTOR_COUNT_FIELDS:
        expect(
            imported_module.get(field_name) == provider_registration_manifest.get(field_name),
            f"expected imported actor module to preserve {field_name}",
        )

    local_module = link_plan.get("local_module", {})
    expect(
        local_module.get("module_name") == "M270D003Consumer"
        and local_module.get("translation_unit_registration_order_ordinal") == 2,
        "expected cross-module actor consumer to preserve the local module identity and registration ordinal",
    )
    for field_name in DESCRIPTOR_COUNT_FIELDS:
        expect(
            local_module.get(field_name) == consumer_registration_manifest.get(field_name),
            f"expected local actor module to preserve {field_name}",
        )

    return imported_module, local_module


__all__ = [
    "expect_actor_cross_module_link_plan",
    "expect_provider_actor_import_surface",
]

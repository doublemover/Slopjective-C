"""Concurrency actor cross-module import and link-plan assertions."""

from __future__ import annotations

from collections.abc import Mapping
from copy import deepcopy

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

ACTOR_METADATA_COUNT_FIELDS = (
    "actor_interface_sites",
    "actor_method_sites",
    "actor_metadata_record_sites",
    "nonisolated_entry_sites",
    "executor_affinity_sites",
    "actor_hop_artifact_sites",
    "actor_isolation_thunk_sites",
    "replay_proof_dependency_sites",
    "race_guard_dependency_sites",
    "task_handoff_sites",
    "guard_blocked_sites",
    "contract_violation_sites",
)

ACTOR_LINK_PLAN_COUNT_FIELDS = {
    "actor_interface_sites": "concurrency_actor_interface_sites",
    "actor_method_sites": "concurrency_actor_method_sites",
    "actor_metadata_record_sites": "concurrency_actor_metadata_record_sites",
    "nonisolated_entry_sites": "concurrency_actor_nonisolated_entry_sites",
    "executor_affinity_sites": "concurrency_actor_executor_affinity_sites",
    "actor_hop_artifact_sites": "concurrency_actor_hop_artifact_sites",
    "actor_isolation_thunk_sites": "concurrency_actor_isolation_thunk_sites",
    "replay_proof_dependency_sites": (
        "concurrency_actor_replay_proof_dependency_sites"
    ),
    "race_guard_dependency_sites": (
        "concurrency_actor_race_guard_dependency_sites"
    ),
    "task_handoff_sites": "concurrency_actor_task_handoff_sites",
    "guard_blocked_sites": "concurrency_actor_guard_blocked_sites",
    "contract_violation_sites": "concurrency_actor_contract_violation_sites",
}


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
    for field_name in ACTOR_METADATA_COUNT_FIELDS:
        expect(
            isinstance(provider_actor_surface.get(field_name), int),
            f"expected concurrency actor provider import surface to publish {field_name}",
        )
    expect(
        provider_actor_surface.get("actor_interface_sites") == 1
        and provider_actor_surface.get("actor_method_sites") == 2
        and provider_actor_surface.get("actor_metadata_record_sites", 0)
        >= provider_actor_surface.get("actor_interface_sites", 0)
        and provider_actor_surface.get("nonisolated_entry_sites", 0) >= 1
        and provider_actor_surface.get("executor_affinity_sites", 0) >= 1
        and provider_actor_surface.get("actor_hop_artifact_sites", 0) >= 1
        and provider_actor_surface.get("actor_isolation_thunk_sites", 0) >= 1
        and provider_actor_surface.get("replay_proof_dependency_sites", 0) >= 1
        and provider_actor_surface.get("race_guard_dependency_sites", 0) >= 1
        and provider_actor_surface.get("task_handoff_sites", 0) >= 1
        and provider_actor_surface.get("contract_violation_sites") == 0,
        "expected concurrency actor provider import surface to preserve actor identity/mailbox/executor metadata counts",
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
    provider_actor_surface = provider_import_payload.get(
        "objc_concurrency_actor_mailbox_and_isolation_runtime_import_surface", {}
    )
    for provider_field, link_plan_field in ACTOR_LINK_PLAN_COUNT_FIELDS.items():
        expect(
            imported_module.get(link_plan_field)
            == provider_actor_surface.get(provider_field),
            f"expected imported actor module to preserve {link_plan_field}",
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


def expect_actor_cross_module_link_plan_rejects_metadata_count_drift(
    link_plan: Mapping[str, object],
    provider_import_payload: Mapping[str, object],
    provider_registration_manifest: Mapping[str, object],
    consumer_registration_manifest: Mapping[str, object],
) -> str:
    drifted_provider_import_payload = deepcopy(provider_import_payload)
    provider_actor_surface = drifted_provider_import_payload.get(
        "objc_concurrency_actor_mailbox_and_isolation_runtime_import_surface", {}
    )
    expect(
        isinstance(provider_actor_surface, dict),
        "expected drift probe to find actor mailbox preservation packet",
    )
    drifted_provider_field = "actor_method_sites"
    original_count = provider_actor_surface.get(drifted_provider_field)
    expect(
        isinstance(original_count, int),
        "expected drift probe to find integer actor method metadata count",
    )
    provider_actor_surface[drifted_provider_field] = original_count + 1

    try:
        expect_actor_cross_module_link_plan(
            link_plan,
            drifted_provider_import_payload,
            provider_registration_manifest,
            consumer_registration_manifest,
        )
    except RuntimeError as exc:
        rejected_link_plan_field = ACTOR_LINK_PLAN_COUNT_FIELDS[drifted_provider_field]
        expect(
            f"expected imported actor module to preserve {rejected_link_plan_field}"
            in str(exc),
            "expected actor metadata count drift probe to fail closed on the imported module count assertion",
        )
        return rejected_link_plan_field

    raise RuntimeError(
        "expected actor metadata count drift between provider import surface and consumer link plan to fail closed"
    )


__all__ = [
    "expect_actor_cross_module_link_plan",
    "expect_actor_cross_module_link_plan_rejects_metadata_count_drift",
    "expect_provider_actor_import_surface",
]

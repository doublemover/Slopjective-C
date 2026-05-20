"""Registration replay link-plan and probe assertions."""

from __future__ import annotations

from ...expectation_matching import expect
from .catalog import (
    EXPECTED_MODULE_NAMES_LEXICOGRAPHIC,
    LIVE_REGISTRATION_REPLAY_CONTRACT_ID,
    REPLAY_REGISTERED_IMAGES_SYMBOL,
    RESET_FOR_TESTING_SYMBOL,
    RESET_REPLAY_STATE_SNAPSHOT_SYMBOL,
    SUCCESS_STATUS_KEYS,
)
from .models import (
    JsonObject,
    RegistrationReplayArtifacts,
    RegistrationReplayIdentities,
)
from .predicates import manifest_is_replay_ready, status_succeeded


def assert_registration_replay_link_plan(
    artifacts: RegistrationReplayArtifacts,
) -> None:
    link_plan = artifacts.link_plan
    expect(
        link_plan.get("module_image_count") == 2,
        "expected multi-image reset/replay link plan to publish two images",
    )
    expect(
        link_plan.get("module_names_lexicographic")
        == list(EXPECTED_MODULE_NAMES_LEXICOGRAPHIC),
        (
            "expected multi-image reset/replay link plan to keep deterministic "
            "module ordering"
        ),
    )
    expect(
        link_plan.get("bootstrap_live_registration_contract_id")
        == LIVE_REGISTRATION_REPLAY_CONTRACT_ID,
        (
            "expected multi-image reset/replay link plan to preserve the live "
            "registration replay contract"
        ),
    )
    expect(
        link_plan.get("bootstrap_reset_for_testing_symbol")
        == RESET_FOR_TESTING_SYMBOL,
        "expected multi-image reset/replay link plan to preserve the reset symbol",
    )
    expect(
        link_plan.get("bootstrap_replay_registered_images_symbol")
        == REPLAY_REGISTERED_IMAGES_SYMBOL,
        "expected multi-image reset/replay link plan to preserve the replay symbol",
    )
    expect(
        link_plan.get("bootstrap_reset_replay_state_snapshot_symbol")
        == RESET_REPLAY_STATE_SNAPSHOT_SYMBOL,
        (
            "expected multi-image reset/replay link plan to preserve the "
            "reset/replay snapshot symbol"
        ),
    )
    expect(
        link_plan.get("imported_live_registration_replay_ready") is True
        and manifest_is_replay_ready(artifacts.provider_registration_manifest)
        and manifest_is_replay_ready(artifacts.consumer_registration_manifest),
        "expected imported and local manifests to be live-registration replay ready",
    )


def assert_registration_replay_payload(
    payload: JsonObject,
    identities: RegistrationReplayIdentities,
) -> None:
    for status_key in SUCCESS_STATUS_KEYS:
        expect(
            status_succeeded(payload, status_key),
            f"expected {status_key} to succeed",
        )

    _assert_startup_payload(payload, identities)
    _assert_first_reset_payload(payload)
    _assert_first_replay_payload(payload, identities)
    _assert_blocked_replay_payload(payload)
    _assert_second_reset_payload(payload)
    _assert_second_replay_payload(payload, identities)


def _assert_startup_payload(
    payload: JsonObject,
    identities: RegistrationReplayIdentities,
) -> None:
    expect(
        payload.get("startup_registered_image_count") == 2,
        "expected startup to install two images",
    )
    expect(
        payload.get("startup_next_expected_registration_order_ordinal") == 3,
        "expected startup next registration ordinal to be three",
    )
    expect(
        payload.get("startup_walked_image_count") == 2,
        "expected startup to walk two images",
    )
    expect(
        payload.get("startup_realized_class_count") == 2,
        "expected startup to realize two classes",
    )
    for generation_key in (
        "startup_class_graph_generation",
        "startup_method_surface_generation",
    ):
        expect(
            payload.get(generation_key, 0) > 0,
            f"expected {generation_key} to be initialized by startup",
        )
    expect(
        payload.get("startup_retained_bootstrap_image_count") == 2,
        "expected startup to retain two bootstrap images",
    )
    expect(
        payload.get("startup_last_walked_module_name")
        == "runtimePackagingConsumer",
        "expected startup to walk the local consumer image last",
    )
    expect(
        payload.get("startup_provider_registration_order_ordinal") == 1,
        "expected provider ordinal one at startup",
    )
    expect(
        payload.get("startup_consumer_registration_order_ordinal") == 2,
        "expected consumer ordinal two at startup",
    )
    expect(
        payload.get("startup_provider_identity") == identities.provider,
        "expected startup provider identity to match the provider manifest",
    )
    expect(
        payload.get("startup_consumer_identity") == identities.consumer,
        "expected startup consumer identity to match the consumer manifest",
    )


def _assert_first_reset_payload(payload: JsonObject) -> None:
    expect(
        payload.get("first_reset_registered_image_count") == 0,
        "expected first reset to clear installed images",
    )
    expect(
        payload.get("first_reset_next_expected_registration_order_ordinal") == 1,
        "expected first reset to restore ordinal one",
    )
    expect(
        payload.get("first_reset_retained_bootstrap_image_count") == 2,
        "expected first reset to retain both bootstrap images",
    )
    expect(
        payload.get("first_reset_cleared_image_local_init_state_count") == 2,
        "expected first reset to clear two image-local init states",
    )
    expect(
        payload.get("first_reset_generation") == 1,
        "expected first reset generation to advance to one",
    )


def _assert_first_replay_payload(
    payload: JsonObject,
    identities: RegistrationReplayIdentities,
) -> None:
    expect(
        payload.get("first_replay_status") == 0,
        "expected first replay to succeed",
    )
    expect(
        payload.get("first_replay_registered_image_count") == 2,
        "expected first replay to restore two images",
    )
    expect(
        payload.get("first_replay_next_expected_registration_order_ordinal") == 3,
        "expected first replay to restore ordinal three",
    )
    expect(
        payload.get("first_replay_walked_image_count") == 2,
        "expected first replay to walk two images",
    )
    expect(
        payload.get("first_replay_realized_class_count") == 2,
        "expected first replay to restore two realized classes",
    )
    _assert_replayed_object_model_generations_match_startup(payload, "first")
    expect(
        payload.get("first_replay_last_replayed_image_count") == 2,
        "expected first replay to publish two replayed images",
    )
    expect(
        payload.get("first_replay_generation") == 1,
        "expected first replay generation to advance to one",
    )
    expect(
        payload.get("first_replay_last_walked_module_name")
        == "runtimePackagingConsumer",
        "expected first replay to walk the consumer image last",
    )
    expect(
        payload.get("first_replay_last_replayed_module_name")
        == "runtimePackagingConsumer",
        "expected first replay to replay the consumer image last",
    )
    expect(
        payload.get("first_replay_provider_registration_order_ordinal") == 1,
        "expected first replay provider ordinal one",
    )
    expect(
        payload.get("first_replay_consumer_registration_order_ordinal") == 2,
        "expected first replay consumer ordinal two",
    )
    expect(
        payload.get("first_replay_provider_identity") == identities.provider,
        "expected first replay provider identity to survive reset",
    )
    expect(
        payload.get("first_replay_consumer_identity") == identities.consumer,
        "expected first replay consumer identity to survive reset",
    )


def _assert_blocked_replay_payload(payload: JsonObject) -> None:
    expect(
        payload.get("replay_without_reset_status") == -1,
        "expected replay without reset to fail closed",
    )
    expect(
        payload.get("blocked_replay_last_replay_status") == -1,
        "expected blocked replay status to be captured",
    )
    expect(
        payload.get("blocked_replay_last_replayed_image_count") == 0,
        "expected blocked replay to avoid replaying images",
    )


def _assert_second_reset_payload(payload: JsonObject) -> None:
    expect(
        payload.get("second_reset_registered_image_count") == 0,
        "expected second reset to clear installed images",
    )
    expect(
        payload.get("second_reset_next_expected_registration_order_ordinal") == 1,
        "expected second reset to restore ordinal one",
    )
    expect(
        payload.get("second_reset_retained_bootstrap_image_count") == 2,
        "expected second reset to retain both bootstrap images",
    )
    expect(
        payload.get("second_reset_cleared_image_local_init_state_count") == 2,
        "expected second reset to clear two image-local init states",
    )
    expect(
        payload.get("second_reset_generation") == 2,
        "expected second reset generation to advance to two",
    )


def _assert_second_replay_payload(
    payload: JsonObject,
    identities: RegistrationReplayIdentities,
) -> None:
    expect(
        payload.get("second_replay_status") == 0,
        "expected second replay to succeed",
    )
    expect(
        payload.get("second_replay_registered_image_count") == 2,
        "expected second replay to restore two images",
    )
    expect(
        payload.get("second_replay_next_expected_registration_order_ordinal") == 3,
        "expected second replay to restore ordinal three",
    )
    expect(
        payload.get("second_replay_walked_image_count") == 2,
        "expected second replay to walk two images",
    )
    expect(
        payload.get("second_replay_realized_class_count") == 2,
        "expected second replay to restore two realized classes",
    )
    _assert_replayed_object_model_generations_match_startup(payload, "second")
    expect(
        payload.get("second_replay_last_replayed_image_count") == 2,
        "expected second replay to publish two replayed images",
    )
    expect(
        payload.get("second_replay_generation") == 2,
        "expected second replay generation to advance to two",
    )
    expect(
        payload.get("second_replay_last_walked_module_name")
        == "runtimePackagingConsumer",
        "expected second replay to walk the consumer image last",
    )
    expect(
        payload.get("second_replay_last_replayed_module_name")
        == "runtimePackagingConsumer",
        "expected second replay to replay the consumer image last",
    )
    expect(
        payload.get("second_replay_provider_registration_order_ordinal") == 1,
        "expected second replay provider ordinal one",
    )
    expect(
        payload.get("second_replay_consumer_registration_order_ordinal") == 2,
        "expected second replay consumer ordinal two",
    )
    expect(
        payload.get("second_replay_provider_identity") == identities.provider,
        "expected second replay provider identity to survive reset",
    )
    expect(
        payload.get("second_replay_consumer_identity") == identities.consumer,
        "expected second replay consumer identity to survive reset",
    )


def _assert_replayed_object_model_generations_match_startup(
    payload: JsonObject,
    prefix: str,
) -> None:
    generation_fields = (
        "class_graph_generation",
        "category_attachment_generation",
        "protocol_declaration_generation",
        "storage_surface_generation",
        "method_surface_generation",
    )
    for field in generation_fields:
        replay_key = f"{prefix}_replay_{field}"
        startup_key = f"startup_{field}"
        expect(
            payload.get(replay_key) == payload.get(startup_key),
            (
                f"expected {replay_key} to match {startup_key} after "
                "deterministic reset/replay"
            ),
        )


__all__ = [
    "assert_registration_replay_link_plan",
    "assert_registration_replay_payload",
]

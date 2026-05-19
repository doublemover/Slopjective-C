"""Runtime lifecycle assertions for registration acceptance probes."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect


@dataclass(frozen=True)
class RegistrationLifecycleStartupIdentity:
    module_name: str
    translation_unit_identity_key: str


def assert_registration_lifecycle_payload(
    payload: dict[str, Any],
) -> RegistrationLifecycleStartupIdentity:
    startup_identity = _assert_startup_state(payload)
    _assert_duplicate_rejection(payload, startup_identity)
    _assert_out_of_order_rejection(payload, startup_identity)
    _assert_post_reset_state(payload)
    _assert_invalid_anchor_rejection(payload)
    _assert_invalid_discovery_root_rejection(payload)
    _assert_replay_state(payload, startup_identity)
    return startup_identity


def _assert_startup_state(
    payload: dict[str, Any],
) -> RegistrationLifecycleStartupIdentity:
    expect(payload.get("startup_registration_copy_status") == 0, "expected startup registration snapshot copy to succeed")
    expect(payload.get("startup_image_walk_copy_status") == 0, "expected startup image walk snapshot copy to succeed")
    expect(payload.get("startup_reset_replay_copy_status") == 0, "expected startup reset/replay snapshot copy to succeed")
    expect(payload.get("startup_registered_image_count") == 1, "expected startup runtime installation state to contain one registered image")
    expect(payload.get("startup_next_expected_registration_order_ordinal") == 2, "expected startup installation state to advance the next registration ordinal")
    expect(payload.get("startup_walked_image_count") == 1, "expected startup image walk state to publish one walked image")
    expect(payload.get("startup_last_discovery_root_entry_count", 0) > 0, "expected startup image walk state to publish a non-empty discovery root")
    expect(payload.get("startup_last_registration_used_staged_table") == 1, "expected startup installation state to consume the staged registration table")
    expect(payload.get("startup_retained_bootstrap_image_count") == 1, "expected startup reset/replay state to retain one bootstrap image")

    startup_module_name = payload.get("startup_last_registered_module_name")
    startup_identity_key = payload.get("startup_last_registered_translation_unit_identity_key")
    expect(isinstance(startup_module_name, str) and startup_module_name != "", "expected startup installation state to publish a registered module name")
    expect(isinstance(startup_identity_key, str) and startup_identity_key != "", "expected startup installation state to publish a registered translation unit identity key")
    return RegistrationLifecycleStartupIdentity(
        module_name=startup_module_name,
        translation_unit_identity_key=startup_identity_key,
    )


def _assert_duplicate_rejection(
    payload: dict[str, Any],
    startup_identity: RegistrationLifecycleStartupIdentity,
) -> None:
    expect(payload.get("duplicate_status") == -2, "expected duplicate registration to fail with duplicate translation-unit identity status")
    expect(payload.get("after_duplicate_registration_copy_status") == 0, "expected duplicate rejection registration snapshot copy to succeed")
    expect(payload.get("after_duplicate_image_walk_copy_status") == 0, "expected duplicate rejection image walk snapshot copy to succeed")
    expect(payload.get("after_duplicate_registered_image_count") == 1, "expected duplicate rejection to leave installed image count unchanged")
    expect(payload.get("after_duplicate_next_expected_registration_order_ordinal") == 2, "expected duplicate rejection to preserve the next expected registration ordinal")
    expect(payload.get("after_duplicate_last_successful_registration_order_ordinal") == 1, "expected duplicate rejection to preserve the last successful registration ordinal")
    expect(payload.get("after_duplicate_last_registration_status") == -2, "expected duplicate rejection snapshot to publish duplicate registration status")
    expect(payload.get("after_duplicate_last_rejected_module_name") == startup_identity.module_name, "expected duplicate rejection snapshot to publish the rejected module name")
    expect(payload.get("after_duplicate_last_rejected_translation_unit_identity_key") == startup_identity.translation_unit_identity_key, "expected duplicate rejection snapshot to publish the rejected translation unit identity key")
    expect(payload.get("after_duplicate_last_rejected_registration_order_ordinal") == 1, "expected duplicate rejection snapshot to publish the rejected registration ordinal")
    expect(payload.get("after_duplicate_walked_image_count") == 1, "expected duplicate rejection to leave image walk state unchanged")


def _assert_out_of_order_rejection(
    payload: dict[str, Any],
    startup_identity: RegistrationLifecycleStartupIdentity,
) -> None:
    expect(payload.get("out_of_order_status") == -3, "expected out-of-order registration to fail with out-of-order status")
    expect(payload.get("after_out_of_order_registration_copy_status") == 0, "expected out-of-order rejection registration snapshot copy to succeed")
    expect(payload.get("after_out_of_order_image_walk_copy_status") == 0, "expected out-of-order rejection image walk snapshot copy to succeed")
    expect(payload.get("after_out_of_order_registered_image_count") == 1, "expected out-of-order rejection to leave installed image count unchanged")
    expect(payload.get("after_out_of_order_next_expected_registration_order_ordinal") == 2, "expected out-of-order rejection to preserve the next expected registration ordinal")
    expect(payload.get("after_out_of_order_last_successful_registration_order_ordinal") == 1, "expected out-of-order rejection to preserve the last successful registration ordinal")
    expect(payload.get("after_out_of_order_last_registration_status") == -3, "expected out-of-order rejection snapshot to publish out-of-order status")
    expect(payload.get("after_out_of_order_last_rejected_module_name") == "out-of-order-module", "expected out-of-order rejection snapshot to publish the rejected module name")
    expect(
        payload.get("after_out_of_order_last_rejected_translation_unit_identity_key")
        == startup_identity.translation_unit_identity_key + "-out-of-order",
        "expected out-of-order rejection snapshot to publish the rejected translation unit identity key",
    )
    expect(payload.get("after_out_of_order_last_rejected_registration_order_ordinal") == 3, "expected out-of-order rejection snapshot to publish the rejected registration ordinal")
    expect(payload.get("after_out_of_order_walked_image_count") == 1, "expected out-of-order rejection to leave image walk state unchanged")


def _assert_post_reset_state(payload: dict[str, Any]) -> None:
    expect(payload.get("post_reset_registration_copy_status") == 0, "expected post-reset registration snapshot copy to succeed")
    expect(payload.get("post_reset_reset_replay_copy_status") == 0, "expected post-reset reset/replay snapshot copy to succeed")
    expect(payload.get("post_reset_registered_image_count") == 0, "expected reset to clear installed runtime images")
    expect(payload.get("post_reset_next_expected_registration_order_ordinal") == 1, "expected reset to restore the initial registration ordinal")
    expect(payload.get("post_reset_retained_bootstrap_image_count") == 1, "expected reset to retain one bootstrap image for replay")
    expect(payload.get("post_reset_last_reset_cleared_image_local_init_state_count") == 1, "expected reset to clear one image-local initialization state record")


def _assert_invalid_anchor_rejection(payload: dict[str, Any]) -> None:
    expect(payload.get("post_reset_invalid_anchor_status") == -4, "expected mismatched linker-anchor registration to fail with invalid registration roots status")
    expect(payload.get("after_invalid_anchor_registration_copy_status") == 0, "expected invalid-anchor rejection registration snapshot copy to succeed")
    expect(payload.get("after_invalid_anchor_image_walk_copy_status") == 0, "expected invalid-anchor rejection image walk snapshot copy to succeed")
    expect(payload.get("after_invalid_anchor_registered_image_count") == 0, "expected invalid-anchor rejection to preserve the cleared installed image count")
    expect(payload.get("after_invalid_anchor_next_expected_registration_order_ordinal") == 1, "expected invalid-anchor rejection to preserve the reset registration ordinal")
    expect(payload.get("after_invalid_anchor_last_registration_status") == -4, "expected invalid-anchor rejection snapshot to publish invalid registration roots status")
    expect(payload.get("after_invalid_anchor_walked_image_count") == 0, "expected invalid-anchor rejection to leave image walk state empty")
    expect(payload.get("after_invalid_anchor_last_linker_anchor_matches_discovery_root") == 0, "expected invalid-anchor rejection to leave linker-anchor/discovery-root proof unset")


def _assert_invalid_discovery_root_rejection(payload: dict[str, Any]) -> None:
    expect(payload.get("post_reset_invalid_discovery_root_status") == -4, "expected malformed discovery-root registration to fail with invalid registration roots status")
    expect(payload.get("after_invalid_discovery_root_registration_copy_status") == 0, "expected invalid-discovery-root rejection registration snapshot copy to succeed")
    expect(payload.get("after_invalid_discovery_root_image_walk_copy_status") == 0, "expected invalid-discovery-root rejection image walk snapshot copy to succeed")
    expect(payload.get("after_invalid_discovery_root_registered_image_count") == 0, "expected invalid-discovery-root rejection to preserve the cleared installed image count")
    expect(payload.get("after_invalid_discovery_root_next_expected_registration_order_ordinal") == 1, "expected invalid-discovery-root rejection to preserve the reset registration ordinal")
    expect(payload.get("after_invalid_discovery_root_last_registration_status") == -4, "expected invalid-discovery-root rejection snapshot to publish invalid registration roots status")
    expect(payload.get("after_invalid_discovery_root_walked_image_count") == 0, "expected invalid-discovery-root rejection to leave image walk state empty")
    expect(payload.get("after_invalid_discovery_root_last_linker_anchor_matches_discovery_root") == 0, "expected invalid-discovery-root rejection to leave linker-anchor/discovery-root proof unset")


def _assert_replay_state(
    payload: dict[str, Any],
    startup_identity: RegistrationLifecycleStartupIdentity,
) -> None:
    expect(payload.get("replay_status") == 0, "expected replay_registered_images_for_testing to succeed")
    expect(payload.get("post_replay_registration_copy_status") == 0, "expected post-replay registration snapshot copy to succeed")
    expect(payload.get("post_replay_image_walk_copy_status") == 0, "expected post-replay image walk snapshot copy to succeed")
    expect(payload.get("post_replay_reset_replay_copy_status") == 0, "expected post-replay reset/replay snapshot copy to succeed")
    expect(payload.get("post_replay_registered_image_count") == 1, "expected replay to restore one registered runtime image")
    expect(payload.get("post_replay_next_expected_registration_order_ordinal") == 2, "expected replay to restore the next registration ordinal")
    expect(payload.get("post_replay_walked_image_count") == 1, "expected replay to restore one walked image")
    expect(payload.get("post_replay_last_discovery_root_entry_count") == payload.get("startup_last_discovery_root_entry_count"), "expected replay to preserve discovery root entry count")
    expect(payload.get("post_replay_last_registration_used_staged_table") == 1, "expected replay registration to consume the staged registration table")
    expect(payload.get("post_replay_retained_bootstrap_image_count") == 1, "expected replay to preserve the retained bootstrap catalog")
    expect(payload.get("post_replay_last_replayed_image_count") == 1, "expected replay state to publish one replayed image")
    expect(payload.get("post_replay_replay_generation", 0) >= 1, "expected replay state to advance the replay generation")
    expect(payload.get("post_replay_last_replay_status") == 0, "expected replay state to publish a successful replay status")
    expect(payload.get("post_replay_last_registered_module_name") == startup_identity.module_name, "expected replay to restore the registered module name")
    expect(payload.get("post_replay_last_walked_module_name") == startup_identity.module_name, "expected replay image walk state to publish the registered module name")
    expect(payload.get("post_replay_last_replayed_module_name") == startup_identity.module_name, "expected replay state to publish the replayed module name")
    expect(payload.get("post_replay_last_registered_translation_unit_identity_key") == startup_identity.translation_unit_identity_key, "expected replay to restore the registered translation unit identity key")
    expect(payload.get("post_replay_last_walked_translation_unit_identity_key") == startup_identity.translation_unit_identity_key, "expected replay image walk state to publish the translation unit identity key")
    expect(payload.get("post_replay_last_replayed_translation_unit_identity_key") == startup_identity.translation_unit_identity_key, "expected replay state to publish the replayed translation unit identity key")


__all__ = [
    "RegistrationLifecycleStartupIdentity",
    "assert_registration_lifecycle_payload",
]

"""Registration replay acceptance case catalog."""

from __future__ import annotations

from ...runtime_contract_interop import (
    IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
    IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
)
from ...runtime_contract_registration import (
    MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE,
)


EXPORTED_CASE_NAMES = ("check_multi_image_registration_reset_replay_case",)

CASE_ID = "multi-image-registration-reset-replay"
CLAIM_CLASS = "linked-runtime-probe"
FIXTURE = IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE
PROBE = MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE

PROVIDER_FIXTURE = IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE
CONSUMER_FIXTURE = IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE
PROVIDER_OUTPUT_DIR = "provider"
CONSUMER_OUTPUT_DIR = "consumer"
PROBE_EXECUTABLE_NAME = "multi_image_registration_reset_replay_probe.exe"

IMPORT_SURFACE_FILE = "module.runtime-import-surface.json"
REGISTRATION_MANIFEST_FILE = "module.runtime-registration-manifest.json"
LINK_PLAN_FILE = "module.cross-module-runtime-link-plan.json"

EXPECTED_MODULE_NAMES_LEXICOGRAPHIC = (
    "runtimePackagingConsumer",
    "runtimePackagingProvider",
)
LIVE_REGISTRATION_REPLAY_CONTRACT_ID = (
    "objc3c.runtime.live.registration.discovery.replay.v1"
)
RESET_FOR_TESTING_SYMBOL = "objc3_runtime_reset_for_testing"
REPLAY_REGISTERED_IMAGES_SYMBOL = (
    "objc3_runtime_replay_registered_images_for_testing"
)
RESET_REPLAY_STATE_SNAPSHOT_SYMBOL = (
    "objc3_runtime_copy_reset_replay_state_for_testing"
)

SUCCESS_STATUS_KEYS = (
    "startup_registration_status",
    "startup_walk_status",
    "startup_graph_status",
    "startup_provider_status",
    "startup_consumer_status",
    "startup_replay_status",
    "first_reset_registration_status",
    "first_reset_replay_status",
    "first_replay_registration_status",
    "first_replay_walk_status",
    "first_replay_graph_status",
    "first_replay_provider_status",
    "first_replay_consumer_status",
    "first_replay_state_status",
    "blocked_replay_state_status",
    "second_reset_registration_status",
    "second_reset_replay_status",
    "second_replay_registration_status",
    "second_replay_walk_status",
    "second_replay_graph_status",
    "second_replay_provider_status",
    "second_replay_consumer_status",
    "second_replay_state_status",
)


__all__ = [
    "CASE_ID",
    "CLAIM_CLASS",
    "CONSUMER_FIXTURE",
    "CONSUMER_OUTPUT_DIR",
    "EXPECTED_MODULE_NAMES_LEXICOGRAPHIC",
    "EXPORTED_CASE_NAMES",
    "FIXTURE",
    "IMPORT_SURFACE_FILE",
    "LINK_PLAN_FILE",
    "LIVE_REGISTRATION_REPLAY_CONTRACT_ID",
    "PROBE",
    "PROBE_EXECUTABLE_NAME",
    "PROVIDER_FIXTURE",
    "PROVIDER_OUTPUT_DIR",
    "REGISTRATION_MANIFEST_FILE",
    "REPLAY_REGISTERED_IMAGES_SYMBOL",
    "RESET_FOR_TESTING_SYMBOL",
    "RESET_REPLAY_STATE_SNAPSHOT_SYMBOL",
    "SUCCESS_STATUS_KEYS",
]

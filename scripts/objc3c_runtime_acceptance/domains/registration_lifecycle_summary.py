"""Summary construction for registration lifecycle acceptance."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True)
class RegistrationLifecycleTimings:
    fixture_compile_ms: int
    probe_link_ms: int
    probe_run_ms: int
    case_total_ms: int


def build_registration_lifecycle_summary(
    payload: dict[str, Any],
    timings: RegistrationLifecycleTimings,
) -> dict[str, Any]:
    return {
        "fixture_compile_ms": timings.fixture_compile_ms,
        "probe_link_ms": timings.probe_link_ms,
        "probe_run_ms": timings.probe_run_ms,
        "case_total_ms": timings.case_total_ms,
        "duplicate_status": payload["duplicate_status"],
        "duplicate_rejected_module_name": payload[
            "after_duplicate_last_rejected_module_name"
        ],
        "duplicate_rejected_translation_unit_identity_key": payload[
            "after_duplicate_last_rejected_translation_unit_identity_key"
        ],
        "duplicate_rejected_registration_order_ordinal": payload[
            "after_duplicate_last_rejected_registration_order_ordinal"
        ],
        "out_of_order_status": payload["out_of_order_status"],
        "out_of_order_rejected_module_name": payload[
            "after_out_of_order_last_rejected_module_name"
        ],
        "out_of_order_rejected_translation_unit_identity_key": payload[
            "after_out_of_order_last_rejected_translation_unit_identity_key"
        ],
        "out_of_order_rejected_registration_order_ordinal": payload[
            "after_out_of_order_last_rejected_registration_order_ordinal"
        ],
        "invalid_anchor_status": payload["post_reset_invalid_anchor_status"],
        "invalid_discovery_root_status": payload[
            "post_reset_invalid_discovery_root_status"
        ],
        "startup_registered_image_count": payload["startup_registered_image_count"],
        "post_reset_registered_image_count": payload[
            "post_reset_registered_image_count"
        ],
        "post_replay_registered_image_count": payload[
            "post_replay_registered_image_count"
        ],
        "retained_bootstrap_image_count": payload[
            "post_replay_retained_bootstrap_image_count"
        ],
        "replay_generation": payload["post_replay_replay_generation"],
    }


__all__ = [
    "RegistrationLifecycleTimings",
    "build_registration_lifecycle_summary",
]

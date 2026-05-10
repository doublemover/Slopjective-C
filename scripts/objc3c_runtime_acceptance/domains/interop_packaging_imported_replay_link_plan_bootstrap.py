"""Imported-runtime packaging replay link-plan bootstrap assertions."""

from __future__ import annotations

from typing import Any

from ..expectation_matching import expect
from ..runtime_contract_object_model import (
    RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
    RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
)


def assert_imported_runtime_link_plan_bootstrap_contract(
    link_plan: dict[str, Any],
) -> None:
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


__all__ = ["assert_imported_runtime_link_plan_bootstrap_contract"]

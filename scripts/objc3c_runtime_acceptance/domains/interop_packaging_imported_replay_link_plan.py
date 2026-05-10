"""Imported-runtime packaging replay link-plan assertions."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_artifacts import (
    ImportedRuntimePackagingReplayArtifacts,
)
from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_link_plan_bootstrap import (
    assert_imported_runtime_link_plan_bootstrap_contract,
)
from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_link_plan_counts import (
    assert_imported_runtime_link_plan_counts,
)
from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_link_plan_modules import (
    assert_imported_runtime_link_plan_modules,
)


def assert_imported_runtime_link_plan_contract(
    artifacts: ImportedRuntimePackagingReplayArtifacts,
) -> None:
    link_plan = artifacts.link_plan
    provider_import_payload = artifacts.provider_import_payload
    provider_registration_manifest = artifacts.provider_registration_manifest
    consumer_registration_manifest = artifacts.consumer_registration_manifest

    assert_imported_runtime_link_plan_bootstrap_contract(link_plan)
    assert_imported_runtime_link_plan_modules(
        link_plan,
        provider_import_payload,
        provider_registration_manifest,
        consumer_registration_manifest,
    )
    assert_imported_runtime_link_plan_counts(
        link_plan,
        provider_registration_manifest,
        consumer_registration_manifest,
    )


__all__ = ["assert_imported_runtime_link_plan_contract"]

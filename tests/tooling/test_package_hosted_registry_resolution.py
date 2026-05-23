from __future__ import annotations

from copy import deepcopy
from pathlib import Path
import sys

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_package_manager.hosted_registry import (  # noqa: E402
    HOSTED_REGISTRY_CONTRACT_ID,
    HostedRegistryResolutionError,
    HostedRegistryResolutionRequest,
    collect_hosted_registry_model_failures,
    resolve_hosted_registry_package,
)
from objc3c_package_manager.model import PACKAGE_MANAGER_TAMPER_CODE  # noqa: E402
from objc3c_tooling.json_io import load_json_object as load_json  # noqa: E402
from scripts.objc3c_workflow.action_catalog_package_registry import (  # noqa: E402
    PACKAGE_REGISTRY_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_package_registry_publication import (  # noqa: E402
    PACKAGE_HOSTED_REGISTRY_INDEX_SCHEMA,
    PACKAGE_REGISTRY_PUBLIC_ACTIONS,
)
from scripts.objc3c_workflow.actions.ecosystem_publication_owner_contracts import (  # noqa: E402
    ecosystem_publication_owner_contract,
)

FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "package_ecosystem" / "hosted_registry"
REGISTRY_FIXTURE = FIXTURE_ROOT / "hosted-registry-index.json"
MIRROR_FIXTURE = FIXTURE_ROOT / "offline-mirror-index.json"
NEGATIVE_CASES_FIXTURE = FIXTURE_ROOT / "negative-registry-cases.json"


def hosted_registry_fixture() -> tuple[dict[str, object], dict[str, object]]:
    return load_json(REGISTRY_FIXTURE), load_json(MIRROR_FIXTURE)


def resolve_default(
    index: dict[str, object],
    mirror: dict[str, object],
    **overrides: object,
):
    request = HostedRegistryResolutionRequest(
        package_id=str(overrides.pop("package_id", "fixture:network.core")),
        package_version=overrides.pop("package_version", "1.0.0"),  # type: ignore[arg-type]
        **overrides,
    )
    return resolve_hosted_registry_package(index, mirror, request)


def assert_resolution_failure(
    index: dict[str, object],
    mirror: dict[str, object],
    expected: str,
    **request_overrides: object,
) -> list[str]:
    with pytest.raises(HostedRegistryResolutionError) as exc_info:
        resolve_default(index, mirror, **request_overrides)
    failures = exc_info.value.failures
    assert any(expected in failure for failure in failures)
    return failures


def test_hosted_registry_fixture_resolves_from_offline_metadata() -> None:
    index, mirror = hosted_registry_fixture()

    failures = collect_hosted_registry_model_failures(index, mirror, root=ROOT)
    resolved = resolve_default(index, mirror)

    assert failures == []
    assert index["contract_id"] == HOSTED_REGISTRY_CONTRACT_ID
    assert index["endpoint_identity"]["endpoint_id"] == "objc3c-hosted-registry-fixture-endpoint-v1"  # type: ignore[index]
    assert index["endpoint_identity"]["channel_id"] == "stable-fixture"  # type: ignore[index]
    assert index["endpoint_identity"]["fallback_registry_success"] is False  # type: ignore[index]
    assert index["provider_model"]["network_fetch"]["separated_from_resolution"] is True  # type: ignore[index]
    assert index["provider_model"]["trust_validator"]["allows_local_install_fallback"] is False  # type: ignore[index]
    assert index["snapshot"]["rollback_policy"] == "monotonic-sequence-required"  # type: ignore[index]
    assert index["service_availability"]["state"] == "offline-fixture-available"  # type: ignore[index]
    assert index["lock_materialization"]["offline_replay_sufficient"] is True  # type: ignore[index]
    assert index["lock_trust_material"]["cache_policy"] == "offline-cache-required-digest-pinned"  # type: ignore[index]
    assert resolved.package_id == "fixture:network.core"
    assert resolved.package_version == "1.0.0"
    assert resolved.snapshot_id == "objc3c-hosted-registry-fixture-v1@1"
    assert resolved.cache_key == "fixture:network.core@1.0.0"
    assert resolved.offline_mirror_path == (
        "tests/tooling/fixtures/package_ecosystem/hosted_registry/offline-mirror-index.json"
    )
    assert resolved.cache_path == (
        "tests/tooling/fixtures/package_ecosystem/hosted_registry/cache/network-core.json"
    )
    assert resolved.registry_record_digest.startswith("sha256:")
    assert resolved.registry_signature_id.startswith("sha256:")
    assert resolved.trust_result_id == "fixture-network-core-1.0.0-trust-result"


def test_hosted_registry_live_network_fetch_fails_closed() -> None:
    index, mirror = hosted_registry_fixture()

    assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: network fetch request rejected",
        allow_network=True,
        registry_url="https://registry.example.invalid/index.json",
    )


def test_hosted_registry_missing_metadata_fails_closed() -> None:
    index, mirror = hosted_registry_fixture()

    assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: missing hosted registry metadata for fixture:missing@1.0.0",
        package_id="fixture:missing",
    )


def test_hosted_registry_unpinned_dependency_fails_closed() -> None:
    index, mirror = hosted_registry_fixture()

    assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: unpinned hosted dependency for fixture:network.core",
        package_version=None,
    )


def test_hosted_registry_invalid_semver_fails_closed() -> None:
    index, mirror = hosted_registry_fixture()

    assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: invalid semver for fixture:network.core@01.0.0",
        package_version="01.0.0",
    )


def test_hosted_registry_unsupported_platform_fails_closed() -> None:
    index, mirror = hosted_registry_fixture()

    assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: unsupported platform linux-x64 for fixture:network.core",
        host_platform="linux-x64",
    )


def test_hosted_registry_rollback_snapshot_fails_closed() -> None:
    index, mirror = hosted_registry_fixture()

    assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: rollback snapshot for fixture:network.core",
        minimum_snapshot_sequence=2,
    )


def test_hosted_registry_endpoint_channel_drift_fails_closed() -> None:
    index, mirror = hosted_registry_fixture()
    index["endpoint_identity"]["channel_id"] = "nightly-fixture"  # type: ignore[index]

    assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: hosted registry endpoint channel_id drifted",
    )


def test_hosted_registry_metadata_digest_mismatch_fails_closed() -> None:
    index, mirror = hosted_registry_fixture()
    record = index["packages"][0]  # type: ignore[index]
    record["metadata_digest"] = "sha256:" + ("0" * 64)  # type: ignore[index]

    assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: registry metadata digest mismatch for fixture:network.core",
    )


def test_hosted_registry_signature_mismatch_fails_closed() -> None:
    index, mirror = hosted_registry_fixture()
    record = index["packages"][0]  # type: ignore[index]
    record["registry_signature"]["signature"] = "sha256:" + ("0" * 64)  # type: ignore[index]

    assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: bad signature for fixture:network.core",
    )


def test_hosted_registry_unknown_trust_root_fails_closed() -> None:
    index, mirror = hosted_registry_fixture()
    record = index["packages"][0]  # type: ignore[index]
    record["registry_signature"]["trust_root_id"] = "unknown-registry-root"  # type: ignore[index]

    assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: unknown trust root unknown-registry-root",
    )


def test_hosted_registry_registry_trust_mismatch_fails_closed() -> None:
    index, mirror = hosted_registry_fixture()
    record = index["packages"][0]  # type: ignore[index]
    record["registry_signature"]["trust_root_id"] = "objc3c-other-registry-root-v1"  # type: ignore[index]

    assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: hosted registry trust root mismatch for fixture:network.core",
    )


def test_hosted_registry_unsigned_artifact_does_not_fall_back_to_local_install() -> None:
    index, mirror = hosted_registry_fixture()
    index["trust_results"][0]["status"] = "unverified"  # type: ignore[index]
    index["trust_results"][0]["allows_local_install_fallback"] = True  # type: ignore[index]

    failures = assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: unsigned hosted artifact fixture:network.core@1.0.0",
    )
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: unsigned hosted artifact local install fallback is forbidden" in failures


def test_hosted_registry_unavailable_service_fails_closed() -> None:
    index, mirror = hosted_registry_fixture()
    index["service_availability"]["state"] = "unavailable"  # type: ignore[index]

    assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: hosted registry unavailable",
    )


def test_hosted_registry_missing_package_provenance_fails_closed() -> None:
    index, mirror = hosted_registry_fixture()
    record = index["packages"][0]  # type: ignore[index]
    record["package_manifest"] = {}  # type: ignore[index]

    assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: missing package provenance for fixture:network.core",
    )


def test_hosted_registry_revoked_registry_package_and_signature_fail_closed() -> None:
    index, mirror = hosted_registry_fixture()
    record = index["packages"][0]  # type: ignore[index]
    signature_id = record["registry_signature"]["signature_id"]  # type: ignore[index]
    index["registry_state"] = "revoked"
    index["revocations"]["revoked_package_ids"] = ["fixture:network.core"]  # type: ignore[index]
    index["revocations"]["revoked_signature_ids"] = [signature_id]  # type: ignore[index]

    failures = assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: revoked registry objc3c-hosted-registry-fixture-v1",
    )
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: revoked package fixture:network.core" in failures
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: revoked signature {signature_id}" in failures


def test_hosted_registry_nondeterministic_candidates_fail_closed() -> None:
    index, mirror = hosted_registry_fixture()
    index["packages"].append(deepcopy(index["packages"][0]))  # type: ignore[index]

    assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: nondeterministic candidates for fixture:network.core@1.0.0",
    )


def test_hosted_registry_duplicate_version_entries_fail_closed() -> None:
    index, mirror = hosted_registry_fixture()
    index["package_versions"].append(deepcopy(index["package_versions"][0]))  # type: ignore[index]

    assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: duplicate package version entry for fixture:network.core@1.0.0",
    )


def test_hosted_registry_ambiguous_version_selection_fails_closed() -> None:
    index, mirror = hosted_registry_fixture()
    index["package_versions"].append(deepcopy(index["package_versions"][0]))  # type: ignore[index]

    assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: ambiguous version selection for fixture:network.core",
    )


def test_hosted_registry_yanked_version_fails_closed() -> None:
    index, mirror = hosted_registry_fixture()
    index["package_versions"][0]["yank_state"] = "yanked"  # type: ignore[index]
    index["yank_state"]["yanked_versions"] = [  # type: ignore[index]
        {"package_id": "fixture:network.core", "package_version": "1.0.0"}
    ]

    assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: yanked hosted registry version fixture:network.core@1.0.0",
    )


def test_hosted_registry_dependency_cycle_fails_closed() -> None:
    index, mirror = hosted_registry_fixture()
    index["dependency_records"] = [  # type: ignore[index]
        {
            "package_id": "fixture:network.core",
            "depends_on_package_id": "fixture:network.core",
            "version_requirement": "1.0.0",
            "resolved_version": "1.0.0",
            "source": "hosted-registry-offline-mirror",
            "resolution_policy": "exact-pinned-version-only",
        }
    ]

    assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: dependency cycle detected at fixture:network.core",
    )


def test_hosted_registry_language_and_abi_mismatch_fail_closed() -> None:
    index, mirror = hosted_registry_fixture()

    failures = assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: language mismatch for fixture:network.core",
        language_version="4.0",
        abi_identity="objc3-abi-drift",
    )
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: ABI mismatch for fixture:network.core" in failures


def test_hosted_registry_index_and_mirror_policy_drift_fail_closed() -> None:
    index, mirror = hosted_registry_fixture()
    index["language_version"] = "4.0"
    index["abi_identity"] = "objc3-abi-drift"
    mirror["network_policy"] = "network-allowed"

    failures = assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: hosted registry language version drifted",
    )
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: hosted registry ABI identity drifted" in failures
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: offline mirror network policy drifted" in failures


def test_hosted_registry_cache_offline_mirror_pin_mismatch_fails_closed() -> None:
    index, mirror = hosted_registry_fixture()
    mirror["packages"][0]["cache_digest"] = "sha256:" + ("4" * 64)  # type: ignore[index]

    assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: cache/offline mirror pin mismatch for fixture:network.core",
    )


def test_hosted_registry_cache_identity_drift_fails_closed() -> None:
    index, mirror = hosted_registry_fixture()
    index["cache_identities"][0]["cache_digest"] = "sha256:" + ("5" * 64)  # type: ignore[index]

    assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: cache identity drift for fixture:network.core@1.0.0",
    )


def test_hosted_registry_offline_mirror_handoff_drift_fails_closed() -> None:
    index, mirror = hosted_registry_fixture()
    index["offline_mirror_handoffs"][0]["mirror_path"] = "tests/tooling/fixtures/package_ecosystem/other-mirror.json"  # type: ignore[index]

    assert_resolution_failure(
        index,
        mirror,
        f"{PACKAGE_MANAGER_TAMPER_CODE}: offline mirror handoff drift for fixture:network.core@1.0.0",
    )


def test_hosted_registry_negative_cases_are_source_owned() -> None:
    payload = load_json(NEGATIVE_CASES_FIXTURE)
    case_ids = {str(case["case_id"]) for case in payload["cases"]}  # type: ignore[index]

    assert payload["diagnostic_code"] == PACKAGE_MANAGER_TAMPER_CODE
    assert {
        "endpoint-channel-drift",
        "invalid-semver",
        "missing-package-provenance",
        "offline-mirror-handoff-drift",
        "registry-trust-mismatch",
        "rollback-snapshot",
        "unsupported-platform",
        "unknown-trust-root",
        "unpinned-hosted-dependency",
        "unsigned-hosted-artifact",
        "yanked-version",
    } <= case_ids
    assert str(payload["fixture_index"]) == (
        "tests/tooling/fixtures/package_ecosystem/hosted_registry/hosted-registry-index.json"
    )


def test_hosted_registry_public_actions_are_registered() -> None:
    assert "validate-package-registry-model" in PACKAGE_REGISTRY_ACTION_SPECS
    assert "package-registry-resolve" in PACKAGE_REGISTRY_ACTION_SPECS
    assert PACKAGE_REGISTRY_ACTION_SPECS["package-registry-resolve"].pass_through_args
    assert any(
        action.action == "validate-package-registry-model"
        and PACKAGE_HOSTED_REGISTRY_INDEX_SCHEMA in action.schema_contracts
        for action in PACKAGE_REGISTRY_PUBLIC_ACTIONS
    )
    assert (
        ecosystem_publication_owner_contract("validate-package-registry-model").owner_role
        == "package-ecosystem-registry-owner"
    )
    assert (
        ecosystem_publication_owner_contract("package-registry-resolve").owner_role
        == "package-ecosystem-registry-owner"
    )

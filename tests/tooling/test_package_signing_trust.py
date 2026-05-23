from __future__ import annotations

from copy import deepcopy
from pathlib import Path
import sys

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_package_manager.model import (  # noqa: E402
    LOCAL_PACKAGE_ABI_IDENTITY,
    LOCAL_PACKAGE_LANGUAGE_VERSION,
    PACKAGE_MANAGER_TAMPER_CODE,
    module_graph_payload,
    package_manifest_digest,
    package_manifest_payload,
)
from objc3c_package_manager.trust import (  # noqa: E402
    LOCAL_PACKAGE_SIGNING_BACKEND,
    PRODUCTION_SIGNING_BACKEND,
    PackageTrustError,
    collect_manifest_trust_failures,
    collect_trust_policy_failures,
    default_trust_policy_payload,
    production_signing_reserved_diagnostic,
    sign_manifest_trust_envelope,
)


SOURCE_DIGEST = "sha256:" + ("a" * 64)


def signed_manifest() -> dict[str, object]:
    source = "tests/tooling/fixtures/package_ecosystem/package_manager_model_contract.json"
    return package_manifest_payload(
        package_id="fixture:trust.root",
        source=source,
        source_kind="showcase-workspace-manifest",
        package_version="1.2.3",
        source_digest=SOURCE_DIGEST,
        module_graph=module_graph_payload(
            package_id="fixture:trust.root",
            source=source,
            source_kind="showcase-workspace-manifest",
            source_digest=SOURCE_DIGEST,
            module_id="fixture.trust.root",
            implementation_module="trust.root",
            source_authority="showcase/portfolio.json",
            source_authority_digest=SOURCE_DIGEST,
            dependencies=[],
        ),
        dependencies=[],
        runtime_symbols=[],
        replay_actions=["build-package-lock", "package-verify"],
    )


def test_deterministic_local_signature_verifies() -> None:
    manifest = signed_manifest()

    failures = collect_manifest_trust_failures(
        manifest,
        manifest_digest=str(manifest["manifest_digest"]),
    )

    assert failures == []
    trust = manifest["trust"]
    assert isinstance(trust, dict)
    assert trust["signing_backend"] == LOCAL_PACKAGE_SIGNING_BACKEND
    assert trust["subject_package_id"] == "fixture:trust.root"
    assert trust["artifact_digest"] == SOURCE_DIGEST
    assert trust["manifest_digest"] == manifest["manifest_digest"]
    assert trust["language_version"] == LOCAL_PACKAGE_LANGUAGE_VERSION
    assert trust["abi_identity"] == LOCAL_PACKAGE_ABI_IDENTITY


def test_digest_mismatch_fails_closed() -> None:
    manifest = deepcopy(signed_manifest())
    manifest["source_digest"] = "sha256:" + ("b" * 64)

    failures = collect_manifest_trust_failures(
        manifest,
        manifest_digest=str(manifest["manifest_digest"]),
    )

    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: artifact digest mismatch for fixture:trust.root" in failures


def test_unknown_trust_root_fails_closed() -> None:
    manifest = deepcopy(signed_manifest())
    assert isinstance(manifest["trust"], dict)
    manifest["trust"]["trust_root_id"] = "unknown-package-trust-root"

    failures = collect_manifest_trust_failures(
        manifest,
        manifest_digest=str(manifest["manifest_digest"]),
    )

    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: unknown trust root unknown-package-trust-root" in failures


def test_revoked_key_and_package_fail_closed() -> None:
    manifest = signed_manifest()
    trust = manifest["trust"]
    assert isinstance(trust, dict)
    trust_policy = default_trust_policy_payload()
    trust_policy["revocations"]["revoked_signing_key_ids"] = [trust["signing_key_id"]]
    trust_policy["revocations"]["revoked_package_ids"] = [manifest["package_id"]]

    failures = collect_manifest_trust_failures(
        manifest,
        manifest_digest=str(manifest["manifest_digest"]),
        trust_policy=trust_policy,
    )

    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: revoked signing key {trust['signing_key_id']}" in failures
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: revoked package fixture:trust.root" in failures


def test_malformed_trust_policy_fails_closed() -> None:
    trust_policy = default_trust_policy_payload()
    trust_policy["verification_policy"]["check_revocation"] = False
    trust_policy["trust_roots"].append(deepcopy(trust_policy["trust_roots"][0]))
    trust_policy["revocations"]["revoked_package_ids"] = ["fixture:one", "fixture:one"]

    failures = collect_trust_policy_failures(trust_policy)

    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: trust policy disabled check_revocation" in failures
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: duplicate trust root objc3c-local-deterministic-trust-root-v1" in failures
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: duplicate signing key objc3c-local-package-key-v1" in failures
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: duplicate revocation subject fixture:one" in failures


def test_trust_root_binding_scope_and_signature_window_fail_closed() -> None:
    manifest = deepcopy(signed_manifest())
    assert isinstance(manifest["trust"], dict)
    manifest["trust"]["signer_id"] = "objc3c-wrong-signer"
    manifest["trust"]["signed_at_utc"] = "2030-01-01T00:00:00Z"
    trust_policy = default_trust_policy_payload()
    trust_policy["trust_roots"][0]["allowed_package_namespaces"] = ["stdlib"]

    failures = collect_manifest_trust_failures(
        manifest,
        manifest_digest=str(manifest["manifest_digest"]),
        trust_policy=trust_policy,
    )

    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: signer_id is not bound to trust root objc3c-local-deterministic-trust-root-v1" in failures
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: package namespace fixture is outside trust root objc3c-local-deterministic-trust-root-v1 scope" in failures
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: expired signature for fixture:trust.root" in failures


def test_malformed_envelope_fails_closed() -> None:
    manifest = deepcopy(signed_manifest())
    assert isinstance(manifest["trust"], dict)
    del manifest["trust"]["signature"]

    failures = collect_manifest_trust_failures(
        manifest,
        manifest_digest=str(manifest["manifest_digest"]),
    )

    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: malformed signature envelope missing signature" in failures
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: bad signature encoding" in failures


def test_production_signing_backend_is_reserved_fail_closed() -> None:
    manifest = signed_manifest()
    manifest_digest = package_manifest_digest(manifest)

    with pytest.raises(PackageTrustError, match="production package signing backend"):
        sign_manifest_trust_envelope(
            manifest,
            manifest_digest=manifest_digest,
            backend=PRODUCTION_SIGNING_BACKEND,
            fixture_replay=False,
        )

    assert "reserved-fail-closed" in production_signing_reserved_diagnostic()

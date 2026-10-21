from __future__ import annotations

from copy import deepcopy
import json
from pathlib import Path
import sys

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from scripts.objc3c_workflow.action_catalog_package_lock import PACKAGE_LOCK_ACTION_SPECS
from scripts.objc3c_workflow.action_catalog_package_lock_contracts import PACKAGE_LOCK_PUBLIC_ACTIONS
from scripts.objc3c_workflow.actions.ecosystem_publication_owner_contracts import (
    ecosystem_publication_owner_contract,
)
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
    collect_extraction_plan_failures,
    collect_manifest_trust_failures,
    collect_trust_policy_failures,
    default_trust_policy_payload,
    package_extraction_plan_payload,
    production_signing_reserved_diagnostic,
    resolve_package_trust_cli_path,
    sign_manifest_trust_envelope,
)


SOURCE_DIGEST = "sha256:" + ("a" * 64)
PACKAGE_SECURITY_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "package_ecosystem"
    / "package_security_hardening_contract.json"
)


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


def test_trust_policy_requires_extraction_and_installer_update_contracts() -> None:
    trust_policy = default_trust_policy_payload()
    trust_policy["extraction_path_policy"]["reject_symlink_entries"] = False
    trust_policy["installer_update_key_policy"]["installer_key_state"] = "active"
    trust_policy["release_registry_trust_root_policy"]["fallback_trust_root_allowed"] = True
    trust_policy["trust_roots"][1]["key_state"] = "active"
    trust_policy["trust_roots"][2]["key_state"] = "active"

    failures = collect_trust_policy_failures(trust_policy)

    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: extraction path policy disabled reject_symlink_entries" in failures
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: installer/update key policy installer_key_state is not reserved-fail-closed" in failures
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: release/registry fallback trust root allowed" in failures
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: release trust root objc3c-release-signing-root-v1 is not reserved-fail-closed" in failures
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: registry trust root objc3c-registry-signing-root-v1 is not reserved-fail-closed" in failures


def test_extraction_plan_accepts_only_safe_repo_relative_paths() -> None:
    plan = package_extraction_plan_payload(
        plan_id="fixture-safe-extraction-plan",
        entries=[
            {
                "path": "tmp/artifacts/package-ecosystem/install-validation/clean-root/objc3c/packages/fixture/trust_root/package-manifest.json",
                "entry_type": "file",
                "mutation": "copy",
                "package_id": "fixture:trust.root",
                "order": 0,
            },
            {
                "path": "tmp/artifacts/package-ecosystem/install-validation/local-package-artifacts/fixture/trust_root.json",
                "entry_type": "file",
                "mutation": "write",
                "package_id": "fixture:trust.root",
                "order": 1,
            },
        ],
    )

    assert collect_extraction_plan_failures(plan) == []


def test_extraction_plan_rejects_unsafe_paths_before_mutation() -> None:
    plan = package_extraction_plan_payload(
        plan_id="fixture-unsafe-extraction-plan",
        entries=[
            {"path": "/absolute/package.json", "entry_type": "file", "mutation": "write"},
            {"path": "../escape/package.json", "entry_type": "file", "mutation": "write"},
            {"path": "objc3c/packages/Foo/package.json", "entry_type": "file", "mutation": "write"},
            {"path": "objc3c/packages/foo/package.json", "entry_type": "file", "mutation": "write"},
            {"path": "objc3c/packages/foo/package.json", "entry_type": "file", "mutation": "write"},
            {"path": "objc3c/packages/link", "entry_type": "symlink", "mutation": "write"},
            {"path": "objc3c/packages/existing/package.json", "entry_type": "file", "mutation": "write"},
        ],
    )

    failures = collect_extraction_plan_failures(
        plan,
        existing_paths=["objc3c/packages/existing/package.json"],
    )

    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: absolute package extraction path rejected: /absolute/package.json" in failures
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: parent traversal package extraction path rejected: ../escape/package.json" in failures
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: case-conflicting package extraction path rejected: objc3c/packages/Foo/package.json vs objc3c/packages/foo/package.json" in failures
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: duplicate package extraction path rejected: objc3c/packages/foo/package.json" in failures
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: symlink package extraction path rejected: objc3c/packages/link" in failures
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: overwrite package extraction path rejected: objc3c/packages/existing/package.json" in failures


def test_package_trust_cli_paths_are_repo_relative_and_non_overwriting() -> None:
    existing = "tests/tooling/fixtures/package_ecosystem/package_manager_model_contract.json"

    with pytest.raises(PackageTrustError, match="parent traversal package manifest input path rejected"):
        resolve_package_trust_cli_path(
            "../outside.json",
            root=ROOT,
            purpose="package manifest input",
        )
    with pytest.raises(PackageTrustError, match="absolute package manifest input path rejected"):
        resolve_package_trust_cli_path(
            str((ROOT / existing).resolve()),
            root=ROOT,
            purpose="package manifest input",
        )
    with pytest.raises(PackageTrustError, match="overwrite package signature envelope output path rejected"):
        resolve_package_trust_cli_path(
            existing,
            root=ROOT,
            purpose="package signature envelope output",
            must_exist=False,
            reject_existing=True,
        )


def test_package_security_hardening_fixture_records_negative_cases() -> None:
    fixture = json.loads(PACKAGE_SECURITY_FIXTURE.read_text(encoding="utf-8"))

    case_ids = {
        str(case.get("case_id"))
        for case in fixture.get("negative_cases", [])
        if isinstance(case, dict)
    }

    assert fixture["contract_id"] == "objc3c.package_ecosystem.package_security_hardening.v1"
    assert fixture["issue_refs"] == ["#8223", "#8204"]
    assert {
        "absolute-extraction-path",
        "parent-traversal-extraction-path",
        "symlink-extraction-entry",
        "overwrite-existing-install-path",
        "duplicate-extraction-path",
        "case-conflicting-extraction-path",
        "installer-key-active-overclaim",
        "update-key-missing",
        "release-trust-root-active",
        "registry-trust-root-active",
    }.issubset(case_ids)


def test_package_security_public_action_is_registered_and_owned() -> None:
    assert "validate-package-security-hardening" in PACKAGE_LOCK_ACTION_SPECS
    action = PACKAGE_LOCK_ACTION_SPECS["validate-package-security-hardening"]
    assert action.backend == "python:scripts/check_objc3c_package_security_hardening.py"
    assert any(
        public.action == "validate-package-security-hardening"
        for public in PACKAGE_LOCK_PUBLIC_ACTIONS
    )

    contract = ecosystem_publication_owner_contract("validate-package-security-hardening")
    assert contract.owner_role == "package-ecosystem-security-owner"
    assert "package_security_hardening_contract.json" in " ".join(contract.source_contracts)
    assert not contract.wrapper_only_allowed

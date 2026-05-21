"""Package signing, trust roots, and fail-closed verification helpers.

Production package signing is intentionally reserved until the repository pins
and reviews a real cryptographic backend.  The deterministic backend in this
module is for checked-in fixtures and local replay only.
"""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from .digests import stable_digest

PACKAGE_MANAGER_TAMPER_CODE = "O3PKG8055"
PACKAGE_TRUST_CONTRACT_ID = "objc3c.package_ecosystem.signing_trust.v1"
SIGNATURE_ENVELOPE_CONTRACT_ID = "objc3c.package_ecosystem.signature_envelope.v1"
LOCAL_PACKAGE_TRUST_ROOT_ID = "objc3c-local-deterministic-trust-root-v1"
LOCAL_PACKAGE_TRUST_KEY_ID = "objc3c-local-package-key-v1"
LOCAL_PACKAGE_SIGNATURE_FORMAT = "objc3c-deterministic-test-sha256-v1"
LOCAL_PACKAGE_SIGNING_BACKEND = "deterministic-test-replay"
LOCAL_PACKAGE_SIGNER_ID = "objc3c-local-package-signer-v1"
LOCAL_PACKAGE_SIGNING_MATERIAL = "objc3c-local-deterministic-test-material-v1"
PRODUCTION_SIGNATURE_FORMAT = "ed25519-reserved-fail-closed"
PRODUCTION_SIGNING_BACKEND = "production-ed25519-reserved"
SIGNATURE_VERIFICATION_POLICY = "fail-closed-local-digest-trust-root-revocation-v1"
DETERMINISTIC_SIGNED_AT_UTC = "omitted-for-deterministic-replay"

_SIGNED_SUBJECT_FIELDS = (
    "subject_kind",
    "subject_package_id",
    "subject_version",
    "package_namespace",
    "artifact_digest",
    "manifest_digest",
    "abi_identity",
    "language_version",
)
_REQUIRED_ENVELOPE_FIELDS = (
    "contract_id",
    "signature_id",
    "signature_format",
    "signing_backend",
    "signer_id",
    "signing_key_id",
    "trust_root_id",
    *_SIGNED_SUBJECT_FIELDS,
    "signature",
    "revocation_state",
    "verification_policy",
    "signed_at_utc",
    "provenance",
)


class PackageTrustError(RuntimeError):
    """Raised when a signing request must fail closed."""


def trust_diagnostic(message: str) -> str:
    return f"{PACKAGE_MANAGER_TAMPER_CODE}: {message}"


def production_signing_reserved_diagnostic() -> str:
    return trust_diagnostic(
        "production package signing backend is reserved-fail-closed; "
        "use deterministic-test-replay only for fixtures and local replay"
    )


def package_namespace_from_id(package_id: str) -> str:
    namespace, separator, _ = package_id.partition(":")
    if not separator or not namespace:
        return ""
    return namespace


def default_trust_policy_payload() -> dict[str, Any]:
    return {
        "contract_id": PACKAGE_TRUST_CONTRACT_ID,
        "policy_version": 1,
        "diagnostic_code": PACKAGE_MANAGER_TAMPER_CODE,
        "trust_roots": [
            {
                "trust_root_id": LOCAL_PACKAGE_TRUST_ROOT_ID,
                "root_kind": "local-development",
                "signer_id": LOCAL_PACKAGE_SIGNER_ID,
                "signing_key_id": LOCAL_PACKAGE_TRUST_KEY_ID,
                "signature_format": LOCAL_PACKAGE_SIGNATURE_FORMAT,
                "signing_backend": LOCAL_PACKAGE_SIGNING_BACKEND,
                "deterministic_public_material": LOCAL_PACKAGE_SIGNING_MATERIAL,
                "key_state": "active",
                "trust_scope": "checked-in-local-package-source",
            },
            {
                "trust_root_id": "objc3c-release-signing-root-v1",
                "root_kind": "release-signing",
                "signer_id": "objc3c-release-package-signer-v1",
                "signing_key_id": "objc3c-release-package-key-v1",
                "signature_format": PRODUCTION_SIGNATURE_FORMAT,
                "signing_backend": PRODUCTION_SIGNING_BACKEND,
                "key_state": "reserved",
                "trust_scope": "reserved-production-release-signing",
            },
            {
                "trust_root_id": "objc3c-registry-signing-root-v1",
                "root_kind": "registry-signing",
                "signer_id": "objc3c-registry-package-signer-v1",
                "signing_key_id": "objc3c-registry-package-key-v1",
                "signature_format": PRODUCTION_SIGNATURE_FORMAT,
                "signing_backend": PRODUCTION_SIGNING_BACKEND,
                "key_state": "reserved",
                "trust_scope": "reserved-production-registry-signing",
            },
        ],
        "revocations": {
            "revoked_trust_root_ids": [],
            "revoked_signing_key_ids": [],
            "revoked_package_ids": [],
            "revoked_signature_ids": [],
        },
        "key_rotation_policy": {
            "rotation_model": "explicit-new-trust-root-before-old-root-revocation",
            "overlap_required": True,
            "implicit_key_rollover_allowed": False,
        },
        "production_signing_backend": {
            "state": "reserved-fail-closed",
            "required_backend": "ed25519-pinned-reviewed-provider",
            "diagnostic_code": PACKAGE_MANAGER_TAMPER_CODE,
        },
        "verification_policy": {
            "require_signature": True,
            "require_trust_root": True,
            "check_revocation": True,
            "check_artifact_digest": True,
            "check_manifest_digest": True,
            "check_subject_identity": True,
            "production_backend_reserved_fail_closed": True,
        },
    }


def load_trust_policy(path: Path | str | None = None) -> dict[str, Any]:
    if path is None:
        return default_trust_policy_payload()
    payload = json.loads(Path(path).read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise PackageTrustError(trust_diagnostic("trust root config is not an object"))
    return payload


def trust_roots_by_id(trust_policy: dict[str, Any] | None = None) -> dict[str, dict[str, Any]]:
    policy = trust_policy if isinstance(trust_policy, dict) else default_trust_policy_payload()
    roots = policy.get("trust_roots", [])
    if not isinstance(roots, list):
        return {}
    return {
        str(root.get("trust_root_id")): root
        for root in roots
        if isinstance(root, dict)
    }


def revoked_values(trust_policy: dict[str, Any] | None, field_name: str) -> set[str]:
    policy = trust_policy if isinstance(trust_policy, dict) else default_trust_policy_payload()
    revocations = policy.get("revocations", {})
    if not isinstance(revocations, dict):
        return set()
    values = revocations.get(field_name, [])
    if not isinstance(values, list):
        return set()
    return {str(value) for value in values if isinstance(value, str)}


def signature_subject_payload(
    *,
    subject_kind: str,
    package_id: str,
    package_version: str,
    package_namespace: str,
    artifact_digest: str,
    manifest_digest: str,
    abi_identity: str,
    language_version: str,
    lock_digest: str | None = None,
) -> dict[str, str]:
    payload = {
        "subject_kind": subject_kind,
        "subject_package_id": package_id,
        "subject_version": package_version,
        "package_namespace": package_namespace,
        "artifact_digest": artifact_digest,
        "manifest_digest": manifest_digest,
        "abi_identity": abi_identity,
        "language_version": language_version,
    }
    if lock_digest is not None:
        payload["lock_digest"] = lock_digest
    return payload


def manifest_signature_subject(
    manifest: dict[str, Any],
    *,
    manifest_digest: str,
) -> dict[str, str]:
    language = manifest.get("language", {})
    if not isinstance(language, dict):
        language = {}
    abi = manifest.get("abi", {})
    if not isinstance(abi, dict):
        abi = {}
    package_id = str(manifest.get("package_id", ""))
    return signature_subject_payload(
        subject_kind="package-manifest",
        package_id=package_id,
        package_version=str(manifest.get("package_version", "")),
        package_namespace=str(
            manifest.get("package_namespace")
            or package_namespace_from_id(package_id)
        ),
        artifact_digest=str(manifest.get("source_digest", "")),
        manifest_digest=manifest_digest,
        abi_identity=str(abi.get("identity", "")),
        language_version=str(language.get("version", "")),
    )


def lock_package_signature_subject(package: dict[str, Any]) -> dict[str, str]:
    manifest_ref = package.get("package_manifest", {})
    if not isinstance(manifest_ref, dict):
        manifest_ref = {}
    package_id = str(package.get("package_id", ""))
    return signature_subject_payload(
        subject_kind="package-manifest",
        package_id=package_id,
        package_version=str(package.get("package_version", "")),
        package_namespace=package_namespace_from_id(package_id),
        artifact_digest=str(package.get("source_digest", "")),
        manifest_digest=str(manifest_ref.get("digest", "")),
        abi_identity=str(package.get("abi_identity", "")),
        language_version=str(package.get("language_version", "")),
    )


def signature_subject_from_envelope(envelope: dict[str, Any]) -> dict[str, str]:
    subject = {
        field_name: str(envelope.get(field_name, ""))
        for field_name in _SIGNED_SUBJECT_FIELDS
    }
    if "lock_digest" in envelope:
        subject["lock_digest"] = str(envelope.get("lock_digest", ""))
    return subject


def deterministic_test_signature(
    envelope: dict[str, Any],
    *,
    trust_root: dict[str, Any],
) -> str:
    return stable_digest(
        {
            "signature_format": str(envelope.get("signature_format", "")),
            "signing_backend": str(envelope.get("signing_backend", "")),
            "signing_key_id": str(envelope.get("signing_key_id", "")),
            "trust_root_id": str(envelope.get("trust_root_id", "")),
            "deterministic_public_material": str(
                trust_root.get("deterministic_public_material", "")
            ),
            "subject": signature_subject_from_envelope(envelope),
        }
    )


def sign_subject_with_deterministic_test_key(
    subject: dict[str, str],
    *,
    trust_policy: dict[str, Any] | None = None,
    backend: str = LOCAL_PACKAGE_SIGNING_BACKEND,
    fixture_replay: bool = True,
) -> dict[str, Any]:
    if backend != LOCAL_PACKAGE_SIGNING_BACKEND or not fixture_replay:
        raise PackageTrustError(production_signing_reserved_diagnostic())
    policy = trust_policy if isinstance(trust_policy, dict) else default_trust_policy_payload()
    trust_root = trust_roots_by_id(policy).get(LOCAL_PACKAGE_TRUST_ROOT_ID)
    if trust_root is None:
        raise PackageTrustError(trust_diagnostic("missing local deterministic trust root"))
    envelope = {
        "contract_id": SIGNATURE_ENVELOPE_CONTRACT_ID,
        "signature_format": LOCAL_PACKAGE_SIGNATURE_FORMAT,
        "signing_backend": LOCAL_PACKAGE_SIGNING_BACKEND,
        "signer_id": str(trust_root.get("signer_id", LOCAL_PACKAGE_SIGNER_ID)),
        "signing_key_id": str(trust_root.get("signing_key_id", LOCAL_PACKAGE_TRUST_KEY_ID)),
        "trust_root_id": LOCAL_PACKAGE_TRUST_ROOT_ID,
        **subject,
        "revocation_state": "not-revoked",
        "verification_policy": SIGNATURE_VERIFICATION_POLICY,
        "signed_at_utc": DETERMINISTIC_SIGNED_AT_UTC,
        "provenance": "deterministic-local-fixture-replay-only",
    }
    envelope["signature_id"] = stable_digest(
        {
            "contract_id": SIGNATURE_ENVELOPE_CONTRACT_ID,
            "subject": signature_subject_from_envelope(envelope),
            "signing_key_id": envelope["signing_key_id"],
            "trust_root_id": envelope["trust_root_id"],
        }
    )
    envelope["signature"] = deterministic_test_signature(envelope, trust_root=trust_root)
    return envelope


def sign_manifest_trust_envelope(
    manifest: dict[str, Any],
    *,
    manifest_digest: str,
    trust_policy: dict[str, Any] | None = None,
    backend: str = LOCAL_PACKAGE_SIGNING_BACKEND,
    fixture_replay: bool = True,
) -> dict[str, Any]:
    return sign_subject_with_deterministic_test_key(
        manifest_signature_subject(manifest, manifest_digest=manifest_digest),
        trust_policy=trust_policy,
        backend=backend,
        fixture_replay=fixture_replay,
    )


def _validate_envelope_shape(envelope: Any) -> list[str]:
    if not isinstance(envelope, dict):
        return [trust_diagnostic("missing signature envelope")]
    failures: list[str] = []
    for field_name in _REQUIRED_ENVELOPE_FIELDS:
        if field_name not in envelope:
            failures.append(
                trust_diagnostic(f"malformed signature envelope missing {field_name}")
            )
    if envelope.get("contract_id") != SIGNATURE_ENVELOPE_CONTRACT_ID:
        failures.append(trust_diagnostic("malformed signature envelope contract id"))
    signature = envelope.get("signature")
    if not isinstance(signature, str) or not signature.startswith("sha256:"):
        failures.append(trust_diagnostic("bad signature encoding"))
    return failures


def collect_signature_envelope_failures(
    envelope: Any,
    *,
    expected_subject: dict[str, str],
    trust_policy: dict[str, Any] | None = None,
) -> list[str]:
    shape_failures = _validate_envelope_shape(envelope)
    if not isinstance(envelope, dict):
        return shape_failures

    failures = list(shape_failures)
    policy = trust_policy if isinstance(trust_policy, dict) else default_trust_policy_payload()
    trust_root_id = str(envelope.get("trust_root_id", ""))
    signing_key_id = str(envelope.get("signing_key_id", ""))
    package_id = str(envelope.get("subject_package_id", ""))
    signature_id = str(envelope.get("signature_id", ""))
    roots = trust_roots_by_id(policy)
    trust_root = roots.get(trust_root_id)
    if trust_root is None:
        failures.append(trust_diagnostic(f"unknown trust root {trust_root_id}"))
    else:
        if trust_root.get("key_state") == "revoked":
            failures.append(trust_diagnostic(f"revoked trust root {trust_root_id}"))
        if trust_root.get("key_state") == "reserved":
            failures.append(production_signing_reserved_diagnostic())
        if str(trust_root.get("signing_key_id")) != signing_key_id:
            failures.append(
                trust_diagnostic(f"signing key {signing_key_id} is not bound to trust root {trust_root_id}")
            )

    if trust_root_id in revoked_values(policy, "revoked_trust_root_ids"):
        failures.append(trust_diagnostic(f"revoked trust root {trust_root_id}"))
    if signing_key_id in revoked_values(policy, "revoked_signing_key_ids"):
        failures.append(trust_diagnostic(f"revoked signing key {signing_key_id}"))
    if package_id in revoked_values(policy, "revoked_package_ids"):
        failures.append(trust_diagnostic(f"revoked package {package_id}"))
    if signature_id in revoked_values(policy, "revoked_signature_ids"):
        failures.append(trust_diagnostic(f"revoked signature {signature_id}"))
    if envelope.get("revocation_state") != "not-revoked":
        failures.append(trust_diagnostic(f"revoked signature envelope for {package_id}"))

    signature_format = str(envelope.get("signature_format", ""))
    signing_backend = str(envelope.get("signing_backend", ""))
    if signature_format != LOCAL_PACKAGE_SIGNATURE_FORMAT:
        if signature_format == PRODUCTION_SIGNATURE_FORMAT:
            failures.append(production_signing_reserved_diagnostic())
        else:
            failures.append(trust_diagnostic(f"unsupported signature format {signature_format}"))
    if signing_backend != LOCAL_PACKAGE_SIGNING_BACKEND:
        failures.append(production_signing_reserved_diagnostic())

    for field_name, expected_value in expected_subject.items():
        actual_value = str(envelope.get(field_name, ""))
        if actual_value == str(expected_value):
            continue
        if field_name == "artifact_digest":
            failures.append(trust_diagnostic(f"artifact digest mismatch for {package_id}"))
        elif field_name == "manifest_digest":
            failures.append(trust_diagnostic(f"manifest digest mismatch for {package_id}"))
        elif field_name == "abi_identity":
            failures.append(trust_diagnostic(f"ABI mismatch for {package_id}"))
        elif field_name == "language_version":
            failures.append(trust_diagnostic(f"language mismatch for {package_id}"))
        elif field_name == "package_namespace":
            failures.append(trust_diagnostic(f"namespace mismatch for {package_id}"))
        else:
            failures.append(
                trust_diagnostic(
                    f"wrong subject {field_name} for {package_id}: {actual_value} != {expected_value}"
                )
            )

    if trust_root is not None and not any(
        "reserved-fail-closed" in failure or "unknown trust root" in failure
        for failure in failures
    ):
        expected_signature = deterministic_test_signature(envelope, trust_root=trust_root)
        if envelope.get("signature") != expected_signature:
            failures.append(trust_diagnostic(f"bad signature for {package_id}"))
    return failures


def collect_manifest_trust_failures(
    manifest: dict[str, Any],
    *,
    manifest_digest: str,
    trust_policy: dict[str, Any] | None = None,
) -> list[str]:
    return collect_signature_envelope_failures(
        manifest.get("trust"),
        expected_subject=manifest_signature_subject(
            manifest,
            manifest_digest=manifest_digest,
        ),
        trust_policy=trust_policy,
    )


def collect_lock_package_trust_failures(
    package: dict[str, Any],
    *,
    trust_policy: dict[str, Any] | None = None,
) -> list[str]:
    return collect_signature_envelope_failures(
        package.get("trust"),
        expected_subject=lock_package_signature_subject(package),
        trust_policy=trust_policy,
    )


__all__ = [
    "DETERMINISTIC_SIGNED_AT_UTC",
    "LOCAL_PACKAGE_SIGNATURE_FORMAT",
    "LOCAL_PACKAGE_SIGNER_ID",
    "LOCAL_PACKAGE_SIGNING_BACKEND",
    "LOCAL_PACKAGE_TRUST_KEY_ID",
    "LOCAL_PACKAGE_TRUST_ROOT_ID",
    "PACKAGE_MANAGER_TAMPER_CODE",
    "PACKAGE_TRUST_CONTRACT_ID",
    "PRODUCTION_SIGNATURE_FORMAT",
    "PRODUCTION_SIGNING_BACKEND",
    "PackageTrustError",
    "SIGNATURE_ENVELOPE_CONTRACT_ID",
    "SIGNATURE_VERIFICATION_POLICY",
    "collect_lock_package_trust_failures",
    "collect_manifest_trust_failures",
    "collect_signature_envelope_failures",
    "default_trust_policy_payload",
    "load_trust_policy",
    "lock_package_signature_subject",
    "manifest_signature_subject",
    "production_signing_reserved_diagnostic",
    "sign_manifest_trust_envelope",
    "sign_subject_with_deterministic_test_key",
    "signature_subject_payload",
    "trust_diagnostic",
    "trust_roots_by_id",
]

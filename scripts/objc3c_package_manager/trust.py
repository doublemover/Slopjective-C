"""Package signing, trust roots, and fail-closed verification helpers.

Production package signing is intentionally reserved until the repository pins
and reviews a real cryptographic backend.  The deterministic backend in this
module is for checked-in fixtures and local replay only.
"""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any, Iterable

from .digests import stable_digest

PACKAGE_MANAGER_TAMPER_CODE = "O3PKG8055"
PACKAGE_TRUST_CONTRACT_ID = "objc3c.package_ecosystem.signing_trust.v1"
SIGNATURE_ENVELOPE_CONTRACT_ID = "objc3c.package_ecosystem.signature_envelope.v1"
PACKAGE_EXTRACTION_PLAN_CONTRACT_ID = "objc3c.package_ecosystem.extraction_plan.v1"
PACKAGE_EXTRACTION_PATH_POLICY_CONTRACT_ID = (
    "objc3c.package_ecosystem.extraction_path_policy.v1"
)
INSTALLER_UPDATE_KEY_POLICY_CONTRACT_ID = (
    "objc3c.package_ecosystem.installer_update_key_policy.v1"
)
RELEASE_REGISTRY_TRUST_ROOT_POLICY_CONTRACT_ID = (
    "objc3c.package_ecosystem.release_registry_trust_root_policy.v1"
)
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
LOCAL_PACKAGE_TRUST_ISSUER_ID = "objc3c-local-package-trust-issuer-v1"
LOCAL_PACKAGE_SIGNATURE_ALGORITHM = "sha256-fixture-digest"
PRODUCTION_SIGNATURE_ALGORITHM = "ed25519-reserved"
LOCAL_PACKAGE_TRUST_VALID_FROM_UTC = "2025-01-01T00:00:00Z"
LOCAL_PACKAGE_TRUST_VALID_UNTIL_UTC = "2027-01-01T00:00:00Z"
PACKAGE_TRUST_COMPATIBILITY_SCOPE = "objc3-abi-2025Q4-language-3.0"
EXTRACTION_PATH_POLICY = (
    "fail-closed-no-absolute-traversal-symlink-overwrite-duplicates-v1"
)
INSTALLER_UPDATE_KEY_POLICY = "reserved-fail-closed-installer-update-keys-v1"
RELEASE_REGISTRY_TRUST_ROOT_POLICY = (
    "reserved-fail-closed-release-registry-trust-roots-v1"
)
_WILDCARD_NAMESPACE = "*"

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
_REQUIRED_TRUST_POLICY_FIELDS = (
    "contract_id",
    "policy_version",
    "diagnostic_code",
    "trust_roots",
    "revocations",
    "key_rotation_policy",
    "extraction_path_policy",
    "installer_update_key_policy",
    "release_registry_trust_root_policy",
    "production_signing_backend",
    "verification_policy",
)
_REQUIRED_TRUST_ROOT_FIELDS = (
    "trust_root_id",
    "issuer_id",
    "root_kind",
    "signer_id",
    "signing_key_id",
    "signature_format",
    "signature_algorithm",
    "signing_backend",
    "key_state",
    "valid_from_utc",
    "valid_until_utc",
    "trust_scope",
    "compatibility_scope",
    "allowed_package_namespaces",
)
_EXTRACTION_PATH_POLICY_TRUE_FIELDS = (
    "before_filesystem_mutation_required",
    "reject_absolute_paths",
    "reject_parent_traversal",
    "reject_symlink_entries",
    "reject_overwrite_existing_paths",
    "reject_duplicate_paths",
    "reject_case_conflicting_paths",
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


def default_extraction_path_policy_payload() -> dict[str, Any]:
    return {
        "contract_id": PACKAGE_EXTRACTION_PATH_POLICY_CONTRACT_ID,
        "policy": EXTRACTION_PATH_POLICY,
        "diagnostic_code": PACKAGE_MANAGER_TAMPER_CODE,
        "plan_contract_id": PACKAGE_EXTRACTION_PLAN_CONTRACT_ID,
        "path_root": "repo-relative-owned-package-root",
        "before_filesystem_mutation_required": True,
        "reject_absolute_paths": True,
        "reject_parent_traversal": True,
        "reject_symlink_entries": True,
        "reject_overwrite_existing_paths": True,
        "reject_duplicate_paths": True,
        "reject_case_conflicting_paths": True,
        "allow_empty_path": False,
    }


def default_trust_policy_payload() -> dict[str, Any]:
    return {
        "contract_id": PACKAGE_TRUST_CONTRACT_ID,
        "policy_version": 1,
        "diagnostic_code": PACKAGE_MANAGER_TAMPER_CODE,
        "trust_roots": [
            {
                "trust_root_id": LOCAL_PACKAGE_TRUST_ROOT_ID,
                "issuer_id": LOCAL_PACKAGE_TRUST_ISSUER_ID,
                "root_kind": "local-development",
                "signer_id": LOCAL_PACKAGE_SIGNER_ID,
                "signing_key_id": LOCAL_PACKAGE_TRUST_KEY_ID,
                "signature_format": LOCAL_PACKAGE_SIGNATURE_FORMAT,
                "signature_algorithm": LOCAL_PACKAGE_SIGNATURE_ALGORITHM,
                "signing_backend": LOCAL_PACKAGE_SIGNING_BACKEND,
                "deterministic_public_material": LOCAL_PACKAGE_SIGNING_MATERIAL,
                "key_state": "active",
                "valid_from_utc": LOCAL_PACKAGE_TRUST_VALID_FROM_UTC,
                "valid_until_utc": LOCAL_PACKAGE_TRUST_VALID_UNTIL_UTC,
                "trust_scope": "checked-in-local-package-source",
                "compatibility_scope": PACKAGE_TRUST_COMPATIBILITY_SCOPE,
                "allowed_package_namespaces": ["fixture", "stdlib", "showcase"],
            },
            {
                "trust_root_id": "objc3c-release-signing-root-v1",
                "issuer_id": "objc3c-release-package-trust-issuer-v1",
                "root_kind": "release-signing",
                "signer_id": "objc3c-release-package-signer-v1",
                "signing_key_id": "objc3c-release-package-key-v1",
                "signature_format": PRODUCTION_SIGNATURE_FORMAT,
                "signature_algorithm": PRODUCTION_SIGNATURE_ALGORITHM,
                "signing_backend": PRODUCTION_SIGNING_BACKEND,
                "key_state": "reserved",
                "valid_from_utc": LOCAL_PACKAGE_TRUST_VALID_FROM_UTC,
                "valid_until_utc": LOCAL_PACKAGE_TRUST_VALID_UNTIL_UTC,
                "trust_scope": "reserved-production-release-signing",
                "compatibility_scope": PACKAGE_TRUST_COMPATIBILITY_SCOPE,
                "allowed_package_namespaces": [_WILDCARD_NAMESPACE],
            },
            {
                "trust_root_id": "objc3c-registry-signing-root-v1",
                "issuer_id": "objc3c-registry-package-trust-issuer-v1",
                "root_kind": "registry-signing",
                "signer_id": "objc3c-registry-package-signer-v1",
                "signing_key_id": "objc3c-registry-package-key-v1",
                "signature_format": PRODUCTION_SIGNATURE_FORMAT,
                "signature_algorithm": PRODUCTION_SIGNATURE_ALGORITHM,
                "signing_backend": PRODUCTION_SIGNING_BACKEND,
                "key_state": "reserved",
                "valid_from_utc": LOCAL_PACKAGE_TRUST_VALID_FROM_UTC,
                "valid_until_utc": LOCAL_PACKAGE_TRUST_VALID_UNTIL_UTC,
                "trust_scope": "reserved-production-registry-signing",
                "compatibility_scope": PACKAGE_TRUST_COMPATIBILITY_SCOPE,
                "allowed_package_namespaces": [_WILDCARD_NAMESPACE],
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
        "extraction_path_policy": default_extraction_path_policy_payload(),
        "installer_update_key_policy": {
            "contract_id": INSTALLER_UPDATE_KEY_POLICY_CONTRACT_ID,
            "policy": INSTALLER_UPDATE_KEY_POLICY,
            "diagnostic_code": PACKAGE_MANAGER_TAMPER_CODE,
            "installer_key_required": True,
            "update_key_required": True,
            "installer_key_state": "reserved-fail-closed",
            "update_key_state": "reserved-fail-closed",
            "reserved_key_use_fails_closed": True,
            "implicit_installer_key_rollover_allowed": False,
            "implicit_update_key_rollover_allowed": False,
        },
        "release_registry_trust_root_policy": {
            "contract_id": RELEASE_REGISTRY_TRUST_ROOT_POLICY_CONTRACT_ID,
            "policy": RELEASE_REGISTRY_TRUST_ROOT_POLICY,
            "diagnostic_code": PACKAGE_MANAGER_TAMPER_CODE,
            "local_fixture_root_only_active": True,
            "release_root_required_state": "reserved",
            "registry_root_required_state": "reserved",
            "reserved_root_use_fails_closed": True,
            "fallback_trust_root_allowed": False,
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


def _duplicate_values(values: list[str]) -> set[str]:
    seen: set[str] = set()
    duplicates: set[str] = set()
    for value in values:
        if value in seen:
            duplicates.add(value)
        seen.add(value)
    return duplicates


def _path_is_absolute_or_drive_qualified(raw_path: str) -> bool:
    normalized = raw_path.replace("\\", "/")
    return (
        Path(raw_path).is_absolute()
        or normalized.startswith("/")
        or normalized.startswith("~/")
        or (len(normalized) >= 2 and normalized[1] == ":")
    )


def _normalized_package_path_parts(raw_path: str) -> list[str]:
    return [part for part in raw_path.replace("\\", "/").split("/") if part]


def _normalized_package_path(raw_path: str) -> str:
    return "/".join(_normalized_package_path_parts(raw_path))


def _string_values(values: Iterable[str] | None) -> set[str]:
    if values is None:
        return set()
    return {str(value) for value in values}


def _required_string(root: dict[str, Any], field_name: str) -> str:
    value = root.get(field_name)
    return value if isinstance(value, str) and value else ""


def _namespace_allowed(root: dict[str, Any], package_namespace: str) -> bool:
    namespaces = root.get("allowed_package_namespaces", [])
    if not isinstance(namespaces, list):
        return False
    allowed = {value for value in namespaces if isinstance(value, str) and value}
    return _WILDCARD_NAMESPACE in allowed or package_namespace in allowed


def _collect_revocation_shape_failures(revocations: Any) -> list[str]:
    if not isinstance(revocations, dict):
        return [trust_diagnostic("trust policy revocations is not an object")]
    failures: list[str] = []
    for field_name in (
        "revoked_trust_root_ids",
        "revoked_signing_key_ids",
        "revoked_package_ids",
        "revoked_signature_ids",
    ):
        values = revocations.get(field_name)
        if not isinstance(values, list):
            failures.append(trust_diagnostic(f"trust policy revocations missing {field_name}"))
            continue
        string_values = [str(value) for value in values if isinstance(value, str)]
        if len(string_values) != len(values):
            failures.append(trust_diagnostic(f"trust policy revocations {field_name} has non-string subject"))
        for duplicate in sorted(_duplicate_values(string_values)):
            failures.append(trust_diagnostic(f"duplicate revocation subject {duplicate}"))
    return failures


def _collect_extraction_path_policy_failures(policy: Any) -> list[str]:
    if not isinstance(policy, dict):
        return [trust_diagnostic("trust policy extraction path policy is not an object")]
    failures: list[str] = []
    if policy.get("contract_id") != PACKAGE_EXTRACTION_PATH_POLICY_CONTRACT_ID:
        failures.append(trust_diagnostic("extraction path policy contract id drifted"))
    if policy.get("policy") != EXTRACTION_PATH_POLICY:
        failures.append(trust_diagnostic("extraction path policy drifted"))
    if policy.get("diagnostic_code") != PACKAGE_MANAGER_TAMPER_CODE:
        failures.append(trust_diagnostic("extraction path policy diagnostic code drifted"))
    if policy.get("plan_contract_id") != PACKAGE_EXTRACTION_PLAN_CONTRACT_ID:
        failures.append(trust_diagnostic("extraction plan contract id drifted"))
    if policy.get("path_root") != "repo-relative-owned-package-root":
        failures.append(trust_diagnostic("extraction path root drifted"))
    if policy.get("allow_empty_path") is not False:
        failures.append(trust_diagnostic("extraction path policy allows empty paths"))
    for field_name in _EXTRACTION_PATH_POLICY_TRUE_FIELDS:
        if policy.get(field_name) is not True:
            failures.append(trust_diagnostic(f"extraction path policy disabled {field_name}"))
    return failures


def _collect_installer_update_key_policy_failures(policy: Any) -> list[str]:
    if not isinstance(policy, dict):
        return [trust_diagnostic("trust policy installer/update key policy is not an object")]
    failures: list[str] = []
    if policy.get("contract_id") != INSTALLER_UPDATE_KEY_POLICY_CONTRACT_ID:
        failures.append(trust_diagnostic("installer/update key policy contract id drifted"))
    if policy.get("policy") != INSTALLER_UPDATE_KEY_POLICY:
        failures.append(trust_diagnostic("installer/update key policy drifted"))
    if policy.get("diagnostic_code") != PACKAGE_MANAGER_TAMPER_CODE:
        failures.append(trust_diagnostic("installer/update key policy diagnostic code drifted"))
    for field_name in (
        "installer_key_required",
        "update_key_required",
        "reserved_key_use_fails_closed",
    ):
        if policy.get(field_name) is not True:
            failures.append(trust_diagnostic(f"installer/update key policy disabled {field_name}"))
    for field_name in (
        "implicit_installer_key_rollover_allowed",
        "implicit_update_key_rollover_allowed",
    ):
        if policy.get(field_name) is not False:
            failures.append(trust_diagnostic(f"installer/update key policy allows {field_name}"))
    for field_name in ("installer_key_state", "update_key_state"):
        if policy.get(field_name) != "reserved-fail-closed":
            failures.append(trust_diagnostic(f"installer/update key policy {field_name} is not reserved-fail-closed"))
    return failures


def _collect_release_registry_trust_root_policy_failures(policy: Any) -> list[str]:
    if not isinstance(policy, dict):
        return [trust_diagnostic("trust policy release/registry trust-root policy is not an object")]
    failures: list[str] = []
    if policy.get("contract_id") != RELEASE_REGISTRY_TRUST_ROOT_POLICY_CONTRACT_ID:
        failures.append(trust_diagnostic("release/registry trust-root policy contract id drifted"))
    if policy.get("policy") != RELEASE_REGISTRY_TRUST_ROOT_POLICY:
        failures.append(trust_diagnostic("release/registry trust-root policy drifted"))
    if policy.get("diagnostic_code") != PACKAGE_MANAGER_TAMPER_CODE:
        failures.append(trust_diagnostic("release/registry trust-root policy diagnostic code drifted"))
    if policy.get("local_fixture_root_only_active") is not True:
        failures.append(trust_diagnostic("release/registry trust-root policy allows non-local active roots"))
    if policy.get("release_root_required_state") != "reserved":
        failures.append(trust_diagnostic("release trust root required state drifted"))
    if policy.get("registry_root_required_state") != "reserved":
        failures.append(trust_diagnostic("registry trust root required state drifted"))
    if policy.get("reserved_root_use_fails_closed") is not True:
        failures.append(trust_diagnostic("release/registry reserved root use does not fail closed"))
    if policy.get("fallback_trust_root_allowed") is not False:
        failures.append(trust_diagnostic("release/registry fallback trust root allowed"))
    return failures


def collect_trust_policy_failures(trust_policy: Any) -> list[str]:
    if not isinstance(trust_policy, dict):
        return [trust_diagnostic("trust policy is not an object")]

    failures: list[str] = []
    for field_name in _REQUIRED_TRUST_POLICY_FIELDS:
        if field_name not in trust_policy:
            failures.append(trust_diagnostic(f"trust policy missing {field_name}"))
    if trust_policy.get("contract_id") != PACKAGE_TRUST_CONTRACT_ID:
        failures.append(trust_diagnostic("trust policy contract id drifted"))
    if trust_policy.get("diagnostic_code") != PACKAGE_MANAGER_TAMPER_CODE:
        failures.append(trust_diagnostic("trust policy diagnostic code drifted"))

    verification_policy = trust_policy.get("verification_policy", {})
    if not isinstance(verification_policy, dict):
        failures.append(trust_diagnostic("trust policy verification policy is not an object"))
    else:
        for field_name in (
            "require_signature",
            "require_trust_root",
            "check_revocation",
            "check_artifact_digest",
            "check_manifest_digest",
            "check_subject_identity",
            "production_backend_reserved_fail_closed",
        ):
            if verification_policy.get(field_name) is not True:
                failures.append(trust_diagnostic(f"trust policy disabled {field_name}"))

    key_rotation_policy = trust_policy.get("key_rotation_policy", {})
    if not isinstance(key_rotation_policy, dict):
        failures.append(trust_diagnostic("trust policy key rotation policy is not an object"))
    else:
        if key_rotation_policy.get("implicit_key_rollover_allowed") is not False:
            failures.append(trust_diagnostic("trust policy allows implicit key rollover"))
        if key_rotation_policy.get("overlap_required") is not True:
            failures.append(trust_diagnostic("trust policy disabled key overlap requirement"))

    failures.extend(
        _collect_extraction_path_policy_failures(
            trust_policy.get("extraction_path_policy")
        )
    )
    failures.extend(
        _collect_installer_update_key_policy_failures(
            trust_policy.get("installer_update_key_policy")
        )
    )
    failures.extend(
        _collect_release_registry_trust_root_policy_failures(
            trust_policy.get("release_registry_trust_root_policy")
        )
    )
    failures.extend(_collect_revocation_shape_failures(trust_policy.get("revocations")))

    roots = trust_policy.get("trust_roots", [])
    if not isinstance(roots, list):
        failures.append(trust_diagnostic("trust policy trust_roots is not a list"))
        return failures

    root_ids: list[str] = []
    key_ids: list[str] = []
    for index, root in enumerate(roots):
        if not isinstance(root, dict):
            failures.append(trust_diagnostic(f"trust root {index} is not an object"))
            continue
        for field_name in _REQUIRED_TRUST_ROOT_FIELDS:
            if field_name not in root:
                failures.append(trust_diagnostic(f"trust root missing {field_name}"))
        root_id = _required_string(root, "trust_root_id")
        key_id = _required_string(root, "signing_key_id")
        if root_id:
            root_ids.append(root_id)
        if key_id:
            key_ids.append(key_id)
        namespaces = root.get("allowed_package_namespaces", [])
        if not isinstance(namespaces, list) or not namespaces:
            failures.append(trust_diagnostic(f"trust root {root_id or index} has no package namespace scope"))
        elif len(namespaces) != len({str(value) for value in namespaces if isinstance(value, str)}):
            failures.append(trust_diagnostic(f"trust root {root_id or index} has duplicate package namespace scope"))
        valid_from = root.get("valid_from_utc")
        valid_until = root.get("valid_until_utc")
        if isinstance(valid_from, str) and isinstance(valid_until, str):
            if valid_from >= valid_until:
                failures.append(trust_diagnostic(f"trust root {root_id or index} validity window is invalid"))
        else:
            failures.append(trust_diagnostic(f"trust root {root_id or index} validity window is invalid"))
        if root.get("signature_format") == LOCAL_PACKAGE_SIGNATURE_FORMAT:
            if root.get("signature_algorithm") != LOCAL_PACKAGE_SIGNATURE_ALGORITHM:
                failures.append(trust_diagnostic(f"trust root {root_id or index} signature algorithm drifted"))
            if not root.get("deterministic_public_material"):
                failures.append(trust_diagnostic(f"trust root {root_id or index} missing deterministic public material"))
        if root.get("signature_format") == PRODUCTION_SIGNATURE_FORMAT:
            if root.get("signature_algorithm") != PRODUCTION_SIGNATURE_ALGORITHM:
                failures.append(trust_diagnostic(f"trust root {root_id or index} production signature algorithm drifted"))
        if root.get("root_kind") == "release-signing" and root.get("key_state") != "reserved":
            failures.append(trust_diagnostic(f"release trust root {root_id or index} is not reserved-fail-closed"))
        if root.get("root_kind") == "registry-signing" and root.get("key_state") != "reserved":
            failures.append(trust_diagnostic(f"registry trust root {root_id or index} is not reserved-fail-closed"))
        if root.get("key_state") == "active" and root_id != LOCAL_PACKAGE_TRUST_ROOT_ID:
            failures.append(trust_diagnostic(f"non-local active trust root {root_id or index}"))
        if (
            root.get("root_kind") not in {"local-development", "release-signing", "registry-signing"}
            and root.get("key_state") == "active"
        ):
            failures.append(trust_diagnostic(f"unrecognized active trust root {root_id or index}"))

    for duplicate in sorted(_duplicate_values(root_ids)):
        failures.append(trust_diagnostic(f"duplicate trust root {duplicate}"))
    for duplicate in sorted(_duplicate_values(key_ids)):
        failures.append(trust_diagnostic(f"duplicate signing key {duplicate}"))
    return failures


def collect_package_path_safety_failures(
    raw_paths: Iterable[Any],
    *,
    existing_paths: Iterable[str] | None = None,
    symlink_paths: Iterable[str] | None = None,
    root_label: str = "package extraction",
) -> list[str]:
    failures: list[str] = []
    exact_paths: set[str] = set()
    casefold_paths: dict[str, str] = {}
    existing = {_normalized_package_path(value) for value in _string_values(existing_paths)}
    symlinks = {_normalized_package_path(value) for value in _string_values(symlink_paths)}
    for value in raw_paths:
        if not isinstance(value, str) or not value:
            failures.append(trust_diagnostic(f"{root_label} path is invalid"))
            continue
        raw_path = value
        if raw_path != raw_path.strip():
            failures.append(trust_diagnostic(f"{root_label} path has surrounding whitespace: {raw_path!r}"))
        if _path_is_absolute_or_drive_qualified(raw_path):
            failures.append(trust_diagnostic(f"absolute {root_label} path rejected: {raw_path}"))
            continue
        raw_parts = raw_path.replace("\\", "/").split("/")
        if "" in raw_parts:
            failures.append(trust_diagnostic(f"{root_label} path contains empty segment: {raw_path}"))
        if "." in raw_parts:
            failures.append(trust_diagnostic(f"{root_label} path contains current-directory segment: {raw_path}"))
        if ".." in raw_parts:
            failures.append(trust_diagnostic(f"parent traversal {root_label} path rejected: {raw_path}"))
            continue
        normalized = _normalized_package_path(raw_path)
        if not normalized:
            failures.append(trust_diagnostic(f"{root_label} path is empty after normalization"))
            continue
        if normalized in exact_paths:
            failures.append(trust_diagnostic(f"duplicate {root_label} path rejected: {normalized}"))
        exact_paths.add(normalized)
        case_key = normalized.casefold()
        previous = casefold_paths.get(case_key)
        if previous is not None and previous != normalized:
            failures.append(
                trust_diagnostic(
                    f"case-conflicting {root_label} path rejected: {previous} vs {normalized}"
                )
            )
        casefold_paths.setdefault(case_key, normalized)
        if normalized in symlinks:
            failures.append(trust_diagnostic(f"symlink {root_label} path rejected: {normalized}"))
        if normalized in existing:
            failures.append(trust_diagnostic(f"overwrite {root_label} path rejected: {normalized}"))
    return failures


def package_extraction_plan_payload(
    *,
    plan_id: str,
    entries: Iterable[dict[str, Any]],
    path_root: str = "repo-relative-owned-package-root",
    path_policy: dict[str, Any] | None = None,
    provenance: str = "source-owned-package-manager",
) -> dict[str, Any]:
    normalized_entries = [dict(entry) for entry in entries]
    payload: dict[str, Any] = {
        "contract_id": PACKAGE_EXTRACTION_PLAN_CONTRACT_ID,
        "plan_id": plan_id,
        "path_root": path_root,
        "path_policy": path_policy or default_extraction_path_policy_payload(),
        "entries": normalized_entries,
        "entry_count": len(normalized_entries),
        "before_filesystem_mutation": True,
        "provenance": provenance,
    }
    payload["plan_digest"] = stable_digest(payload)
    return payload


def collect_extraction_plan_failures(
    extraction_plan: Any,
    *,
    existing_paths: Iterable[str] | None = None,
    symlink_paths: Iterable[str] | None = None,
) -> list[str]:
    if not isinstance(extraction_plan, dict):
        return [trust_diagnostic("package extraction plan is not an object")]
    failures: list[str] = []
    for field_name in (
        "contract_id",
        "plan_id",
        "path_root",
        "path_policy",
        "entries",
        "entry_count",
        "before_filesystem_mutation",
        "provenance",
    ):
        if field_name not in extraction_plan:
            failures.append(trust_diagnostic(f"package extraction plan missing {field_name}"))
    if extraction_plan.get("contract_id") != PACKAGE_EXTRACTION_PLAN_CONTRACT_ID:
        failures.append(trust_diagnostic("package extraction plan contract id drifted"))
    if extraction_plan.get("path_root") != "repo-relative-owned-package-root":
        failures.append(trust_diagnostic("package extraction plan root drifted"))
    if extraction_plan.get("before_filesystem_mutation") is not True:
        failures.append(trust_diagnostic("package extraction plan was not checked before filesystem mutation"))
    failures.extend(
        _collect_extraction_path_policy_failures(extraction_plan.get("path_policy"))
    )
    entries = extraction_plan.get("entries", [])
    if not isinstance(entries, list) or not entries:
        failures.append(trust_diagnostic("package extraction plan entries missing"))
        return failures
    if extraction_plan.get("entry_count") != len(entries):
        failures.append(trust_diagnostic("package extraction plan entry count drifted"))
    raw_paths: list[str] = []
    entry_symlinks = set(_string_values(symlink_paths))
    for index, entry in enumerate(entries):
        if not isinstance(entry, dict):
            failures.append(trust_diagnostic(f"package extraction entry {index} is not an object"))
            continue
        raw_path = entry.get("path")
        if not isinstance(raw_path, str) or not raw_path:
            failures.append(trust_diagnostic(f"package extraction entry {index} has invalid path"))
            continue
        raw_paths.append(raw_path)
        if entry.get("entry_type") == "symlink":
            entry_symlinks.add(raw_path)
        if entry.get("mutation") not in {"write", "copy", "record"}:
            failures.append(trust_diagnostic(f"package extraction entry {raw_path} has invalid mutation"))
    failures.extend(
        collect_package_path_safety_failures(
            raw_paths,
            existing_paths=existing_paths,
            symlink_paths=entry_symlinks,
            root_label="package extraction",
        )
    )
    plan_digest = extraction_plan.get("plan_digest")
    if plan_digest is not None:
        normalized = dict(extraction_plan)
        normalized.pop("plan_digest", None)
        if plan_digest != stable_digest(normalized):
            failures.append(trust_diagnostic("package extraction plan digest drifted"))
    return failures


def _symlink_paths_under_root(root: Path, raw_path: str) -> list[str]:
    paths: list[str] = []
    current = root
    parts: list[str] = []
    for part in _normalized_package_path_parts(raw_path):
        parts.append(part)
        current = current / part
        if current.is_symlink():
            paths.append("/".join(parts))
    return paths


def collect_filesystem_extraction_plan_failures(
    *,
    root: Path,
    extraction_plan: Any,
) -> list[str]:
    base_failures = collect_extraction_plan_failures(extraction_plan)
    if base_failures:
        return base_failures
    assert isinstance(extraction_plan, dict)
    entries = extraction_plan.get("entries", [])
    existing_paths: list[str] = []
    symlink_paths: list[str] = []
    for entry in entries if isinstance(entries, list) else []:
        if not isinstance(entry, dict) or not isinstance(entry.get("path"), str):
            continue
        raw_path = str(entry["path"])
        normalized = _normalized_package_path(raw_path)
        target = root / normalized
        if target.exists() or target.is_symlink():
            existing_paths.append(normalized)
        symlink_paths.extend(_symlink_paths_under_root(root, raw_path))
    return collect_extraction_plan_failures(
        extraction_plan,
        existing_paths=existing_paths,
        symlink_paths=symlink_paths,
    )


def resolve_package_trust_cli_path(
    raw_path: str,
    *,
    root: Path,
    purpose: str,
    must_exist: bool = True,
    reject_existing: bool = False,
) -> Path:
    normalized = _normalized_package_path(raw_path)
    symlink_paths = _symlink_paths_under_root(root, raw_path) if normalized else []
    failures = collect_package_path_safety_failures(
        [raw_path],
        symlink_paths=symlink_paths,
        root_label=purpose,
    )
    if failures:
        raise PackageTrustError("\n".join(failures))
    target = root / normalized
    if must_exist and not target.is_file():
        raise PackageTrustError(trust_diagnostic(f"missing {purpose} path: {normalized}"))
    if reject_existing and (target.exists() or target.is_symlink()):
        raise PackageTrustError(trust_diagnostic(f"overwrite {purpose} path rejected: {normalized}"))
    return target


def _collect_trust_root_binding_failures(
    *,
    envelope: dict[str, Any],
    trust_root: dict[str, Any],
    package_id: str,
) -> list[str]:
    failures: list[str] = []
    trust_root_id = str(envelope.get("trust_root_id", ""))
    package_namespace = str(envelope.get("package_namespace", ""))
    for field_name in ("signer_id", "signing_key_id", "signature_format", "signing_backend"):
        if str(trust_root.get(field_name, "")) != str(envelope.get(field_name, "")):
            failures.append(
                trust_diagnostic(
                    f"{field_name} is not bound to trust root {trust_root_id}"
                )
            )
    if trust_root.get("key_state") == "expired":
        failures.append(trust_diagnostic(f"expired trust root {trust_root_id}"))
    if not _namespace_allowed(trust_root, package_namespace):
        failures.append(
            trust_diagnostic(
                f"package namespace {package_namespace} is outside trust root {trust_root_id} scope"
            )
        )
    signed_at_utc = str(envelope.get("signed_at_utc", ""))
    if signed_at_utc and signed_at_utc != DETERMINISTIC_SIGNED_AT_UTC:
        valid_from = str(trust_root.get("valid_from_utc", ""))
        valid_until = str(trust_root.get("valid_until_utc", ""))
        if valid_from and signed_at_utc < valid_from:
            failures.append(trust_diagnostic(f"signature predates trust root for {package_id}"))
        if valid_until and signed_at_utc > valid_until:
            failures.append(trust_diagnostic(f"expired signature for {package_id}"))
    return failures


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
    policy_failures = collect_trust_policy_failures(policy)
    if policy_failures:
        raise PackageTrustError("\n".join(policy_failures))
    trust_root = trust_roots_by_id(policy).get(LOCAL_PACKAGE_TRUST_ROOT_ID)
    if trust_root is None:
        raise PackageTrustError(trust_diagnostic("missing local deterministic trust root"))
    if trust_root.get("key_state") != "active":
        raise PackageTrustError(
            trust_diagnostic(f"local deterministic trust root {LOCAL_PACKAGE_TRUST_ROOT_ID} is not active")
        )
    if not _namespace_allowed(trust_root, str(subject.get("package_namespace", ""))):
        raise PackageTrustError(
            trust_diagnostic(
                f"package namespace {subject.get('package_namespace', '')} is outside trust root {LOCAL_PACKAGE_TRUST_ROOT_ID} scope"
            )
        )
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
    failures.extend(collect_trust_policy_failures(policy))
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
        failures.extend(
            _collect_trust_root_binding_failures(
                envelope=envelope,
                trust_root=trust_root,
                package_id=package_id,
            )
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
    "INSTALLER_UPDATE_KEY_POLICY_CONTRACT_ID",
    "PACKAGE_MANAGER_TAMPER_CODE",
    "PACKAGE_EXTRACTION_PATH_POLICY_CONTRACT_ID",
    "PACKAGE_EXTRACTION_PLAN_CONTRACT_ID",
    "PACKAGE_TRUST_CONTRACT_ID",
    "PRODUCTION_SIGNATURE_FORMAT",
    "PRODUCTION_SIGNING_BACKEND",
    "PackageTrustError",
    "RELEASE_REGISTRY_TRUST_ROOT_POLICY_CONTRACT_ID",
    "SIGNATURE_ENVELOPE_CONTRACT_ID",
    "SIGNATURE_VERIFICATION_POLICY",
    "collect_extraction_plan_failures",
    "collect_filesystem_extraction_plan_failures",
    "collect_lock_package_trust_failures",
    "collect_manifest_trust_failures",
    "collect_package_path_safety_failures",
    "collect_signature_envelope_failures",
    "collect_trust_policy_failures",
    "default_extraction_path_policy_payload",
    "default_trust_policy_payload",
    "load_trust_policy",
    "lock_package_signature_subject",
    "manifest_signature_subject",
    "package_extraction_plan_payload",
    "production_signing_reserved_diagnostic",
    "resolve_package_trust_cli_path",
    "sign_manifest_trust_envelope",
    "sign_subject_with_deterministic_test_key",
    "signature_subject_payload",
    "trust_diagnostic",
    "trust_roots_by_id",
]

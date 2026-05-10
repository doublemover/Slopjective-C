#!/usr/bin/env python3
"""Package-ecosystem owner contracts shared by summary and validation scripts."""

from __future__ import annotations

from pathlib import Path
from typing import Any, Iterable

PACKAGE_ECOSYSTEM_OWNER_POLICY = {
    "channel_owner": "packaging-channels-source",
    "registry_owner": "package-ecosystem-registry-source",
    "package_authoring_owner": "package-ecosystem-authoring-source",
    "package_validation_owner": "package-ecosystem-validation-gate",
    "blocker_owner": "package-ecosystem-blockers",
    "source_authority": "checked-in-package-ecosystem-contracts",
    "evidence_log_allowed": False,
}

PACKAGE_ECOSYSTEM_OWNER_FIELDS = tuple(PACKAGE_ECOSYSTEM_OWNER_POLICY)


def package_ecosystem_owner_payload() -> dict[str, object]:
    return dict(PACKAGE_ECOSYSTEM_OWNER_POLICY)


def require_package_ecosystem_owner_policy(payload: dict[str, Any], *, surface_name: str) -> dict[str, Any]:
    owner_policy = payload.get("owner_policy")
    if not isinstance(owner_policy, dict):
        raise RuntimeError(f"{surface_name} missing owner_policy")
    missing_fields = [field for field in PACKAGE_ECOSYSTEM_OWNER_FIELDS if field not in owner_policy]
    if missing_fields:
        raise RuntimeError(f"{surface_name} owner_policy missing fields: {', '.join(missing_fields)}")
    if owner_policy.get("evidence_log_allowed") is not False:
        raise RuntimeError(f"{surface_name} owner_policy must forbid evidence-log publication")
    expected = package_ecosystem_owner_payload()
    for field, expected_value in expected.items():
        if owner_policy.get(field) != expected_value:
            raise RuntimeError(f"{surface_name} owner_policy drifted for {field}")
    return owner_policy


def require_package_ecosystem_blocker_metadata(
    payload: dict[str, Any],
    *,
    surface_name: str,
    required_blockers: Iterable[str] = (),
) -> dict[str, Any]:
    metadata = payload.get("blocker_metadata")
    if not isinstance(metadata, dict):
        raise RuntimeError(f"{surface_name} missing blocker_metadata")
    if metadata.get("blocker_owner") != PACKAGE_ECOSYSTEM_OWNER_POLICY["blocker_owner"]:
        raise RuntimeError(f"{surface_name} blocker owner drifted")
    blocking_conditions = metadata.get("blocking_conditions")
    if not isinstance(blocking_conditions, list) or not blocking_conditions:
        raise RuntimeError(f"{surface_name} blocker_metadata missing blocking_conditions")
    missing_blockers = [
        blocker
        for blocker in required_blockers
        if blocker not in blocking_conditions
    ]
    if missing_blockers:
        raise RuntimeError(f"{surface_name} blocker_metadata missing blockers: {', '.join(missing_blockers)}")
    return metadata


def require_paths_under_package_ecosystem_tmp(paths: Iterable[str], *, surface_name: str) -> None:
    for raw_path in paths:
        path = Path(raw_path)
        if not path.as_posix().startswith("tmp/artifacts/package-ecosystem/"):
            raise RuntimeError(f"{surface_name} generated path is outside package ecosystem artifact root: {raw_path}")


__all__ = [
    "PACKAGE_ECOSYSTEM_OWNER_FIELDS",
    "PACKAGE_ECOSYSTEM_OWNER_POLICY",
    "package_ecosystem_owner_payload",
    "require_package_ecosystem_blocker_metadata",
    "require_package_ecosystem_owner_policy",
    "require_paths_under_package_ecosystem_tmp",
]

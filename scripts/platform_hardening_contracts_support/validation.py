"""Validation helpers for platform-hardening contracts."""

from __future__ import annotations

from pathlib import Path
from typing import Any, Iterable

from objc3c_tooling.paths import repo_rel

from .constants import PLATFORM_HARDENING_OWNER_FIELDS, PLATFORM_HARDENING_OWNER_POLICY


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def summary_passes(payload: dict[str, Any]) -> bool:
    return payload.get("status") in {"PASS", "OK"} or payload.get("ok") is True


def require_required_fields(payload: dict[str, Any], field_names: Iterable[str], surface_name: str) -> None:
    for field_name in field_names:
        expect(field_name in payload, f"{surface_name} missing required field {field_name}")


def require_platform_hardening_owner_policy(payload: dict[str, Any], *, surface_name: str) -> dict[str, Any]:
    owner_policy = payload.get("owner_policy")
    expect(isinstance(owner_policy, dict), f"{surface_name} missing owner_policy")
    missing_fields = [field for field in PLATFORM_HARDENING_OWNER_FIELDS if field not in owner_policy]
    expect(not missing_fields, f"{surface_name} owner_policy missing fields: {', '.join(missing_fields)}")
    expect(owner_policy.get("evidence_log_allowed") is False, f"{surface_name} owner_policy must forbid evidence-log publication")
    for field_name, expected_value in PLATFORM_HARDENING_OWNER_POLICY.items():
        expect(owner_policy.get(field_name) == expected_value, f"{surface_name} owner_policy drifted for {field_name}")
    return owner_policy


def require_platform_hardening_blocker_metadata(
    payload: dict[str, Any],
    *,
    surface_name: str,
    required_blockers: Iterable[str] = (),
) -> dict[str, Any]:
    blocker_metadata = payload.get("blocker_metadata")
    expect(isinstance(blocker_metadata, dict), f"{surface_name} missing blocker_metadata")
    expect(
        blocker_metadata.get("blocker_owner") == PLATFORM_HARDENING_OWNER_POLICY["blocker_owner"],
        f"{surface_name} blocker owner drifted",
    )
    blocking_conditions = blocker_metadata.get("blocking_conditions")
    expect(isinstance(blocking_conditions, list) and len(blocking_conditions) > 0, f"{surface_name} missing blocking_conditions")
    missing_blockers = [blocker for blocker in required_blockers if blocker not in blocking_conditions]
    expect(not missing_blockers, f"{surface_name} blocker_metadata missing blockers: {', '.join(missing_blockers)}")
    return blocker_metadata


def require_paths_exist(paths: Iterable[Path], *, description: str) -> None:
    for path in paths:
        expect(path.is_file(), f"missing expected {description}: {repo_rel(path)}")


__all__ = [
    "expect",
    "require_paths_exist",
    "require_platform_hardening_blocker_metadata",
    "require_platform_hardening_owner_policy",
    "require_required_fields",
    "summary_passes",
]

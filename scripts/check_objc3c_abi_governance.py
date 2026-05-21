#!/usr/bin/env python3
"""Validate the checked-in Objective-C 3 ABI governance source of truth."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_shared.json_io import load_json_object, validate_json_schema, write_json_file

DIAGNOSTIC_CODE = "O3ABI8173"
MANIFEST_PATH = ROOT / "tests" / "tooling" / "fixtures" / "abi_governance" / "source_of_truth_manifest.json"
SCHEMA_PATH = ROOT / "schemas" / "objc3c-abi-governance-manifest-v1.schema.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "abi-governance" / "abi-governance-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.abi_governance.summary.v1"
REQUIRED_BLOCKED_TRANSITIONS = {
    "public-symbol-removal-without-deprecation-window",
    "runtime-symbol-removal-without-deprecation-window",
    "signature-change-without-major-line",
    "private-helper-graduation-without-policy",
    "unsupported-upgrade-route",
    "unsupported-downgrade-route",
    "package-abi-identity-drift",
    "schema-breaking-change-without-major-line",
}
REQUIRED_UNSUPPORTED_CLAIMS = {
    "cross-major forward compatibility",
    "drop-in legacy ABI compatibility",
    "indefinite support window",
    "compatibility shim support",
    "fallback downgrade route",
}


def _repo_rel(path: Path) -> str:
    return path.resolve().relative_to(ROOT.resolve()).as_posix()


def _display_path(path: Path) -> str:
    try:
        return _repo_rel(path)
    except ValueError:
        return path.as_posix()


def _repo_path(raw_path: str) -> Path:
    return ROOT / Path(raw_path)


def _failure(message: str) -> str:
    return f"{DIAGNOSTIC_CODE}: {message}"


def _nested_value(payload: dict[str, Any], path: Sequence[str]) -> Any:
    current: Any = payload
    for segment in path:
        if not isinstance(current, dict):
            return None
        current = current.get(segment)
    return current


def _load_payload(path: Path, label: str, failures: list[str]) -> dict[str, Any]:
    if not path.is_file():
        failures.append(_failure(f"{label} missing at {_display_path(path)}"))
        return {}
    try:
        return load_json_object(path)
    except Exception as exc:
        failures.append(_failure(f"{label} failed to load at {_display_path(path)}: {exc}"))
        return {}


def _validate_manifest_schema(manifest: dict[str, Any], failures: list[str]) -> None:
    schema = _load_payload(SCHEMA_PATH, "ABI governance schema", failures)
    if not schema:
        return
    try:
        validate_json_schema(manifest, schema, label="ABI governance manifest")
    except Exception as exc:
        failures.append(_failure(f"ABI governance manifest failed schema validation: {exc}"))


def _reject_tmp_or_absolute_path(raw_path: str, label: str, failures: list[str]) -> None:
    path = Path(raw_path)
    parts = {part.lower() for part in path.parts}
    if path.is_absolute():
        failures.append(_failure(f"{label} must be a repo-relative path: {raw_path}"))
    if "tmp" in parts or "temp" in parts:
        failures.append(_failure(f"{label} must not point at generated temp output: {raw_path}"))
    if "\\" in raw_path:
        failures.append(_failure(f"{label} must use repo-relative slash paths: {raw_path}"))


def _validate_source_of_truth(manifest: dict[str, Any], failures: list[str]) -> None:
    source = manifest.get("source_of_truth")
    if not isinstance(source, dict):
        failures.append(_failure("source_of_truth must be an object"))
        return
    if source.get("manifest_path") != _repo_rel(MANIFEST_PATH):
        failures.append(_failure("source_of_truth.manifest_path drifted from canonical manifest"))
    if source.get("schema_path") != _repo_rel(SCHEMA_PATH):
        failures.append(_failure("source_of_truth.schema_path drifted from canonical schema"))
    if source.get("checker_path") != "scripts/check_objc3c_abi_governance.py":
        failures.append(_failure("source_of_truth.checker_path drifted from canonical checker"))
    for field_name in (
        "manifest_path",
        "schema_path",
        "checker_path",
        "release_governance_manifest",
        "release_governance_schema",
    ):
        raw_path = source.get(field_name)
        if not isinstance(raw_path, str) or not raw_path:
            failures.append(_failure(f"source_of_truth.{field_name} must be a path"))
            continue
        _reject_tmp_or_absolute_path(raw_path, f"source_of_truth.{field_name}", failures)
        if not _repo_path(raw_path).is_file():
            failures.append(_failure(f"source_of_truth.{field_name} references missing file {raw_path}"))


def _validate_posture(manifest: dict[str, Any], failures: list[str]) -> None:
    posture = manifest.get("governance_posture")
    if not isinstance(posture, dict):
        failures.append(_failure("governance_posture must be an object"))
        return
    if posture.get("failure_mode") != "fail-closed":
        failures.append(_failure("ABI governance failure mode must stay fail-closed"))
    if posture.get("compatibility_shims_supported") is not False:
        failures.append(_failure("ABI governance must not claim compatibility shim support"))
    if posture.get("fallback_routes_supported") is not False:
        failures.append(_failure("ABI governance must not claim fallback route support"))
    unsupported = {
        value
        for value in posture.get("unsupported_stability_claims", [])
        if isinstance(value, str)
    }
    missing = sorted(REQUIRED_UNSUPPORTED_CLAIMS - unsupported)
    if missing:
        failures.append(_failure("ABI governance lost unsupported-claim boundaries: " + ", ".join(missing)))


def _validate_governed_surfaces(manifest: dict[str, Any], failures: list[str]) -> int:
    surfaces = manifest.get("governed_surfaces")
    if not isinstance(surfaces, list) or not surfaces:
        failures.append(_failure("governed_surfaces must be a non-empty list"))
        return 0
    seen: set[str] = set()
    for surface in surfaces:
        if not isinstance(surface, dict):
            failures.append(_failure("governed surface entry is malformed"))
            continue
        surface_id = surface.get("surface_id")
        if not isinstance(surface_id, str) or not surface_id:
            failures.append(_failure("governed surface missing surface_id"))
            continue
        if surface_id in seen:
            failures.append(_failure(f"governed surface duplicated {surface_id}"))
            continue
        seen.add(surface_id)
        if surface.get("release_blocker") is not True:
            failures.append(_failure(f"governed surface {surface_id} must be release-blocking"))
        if surface.get("stability_claim") not in {
            "exact-current-abi-only",
            "same-major-checked-manifest-only",
        }:
            failures.append(_failure(f"governed surface {surface_id} has unsupported stability claim"))
        for field_name in ("source_path", "schema_path"):
            raw_path = surface.get(field_name)
            if not isinstance(raw_path, str) or not raw_path:
                failures.append(_failure(f"governed surface {surface_id} {field_name} must be a path"))
                continue
            _reject_tmp_or_absolute_path(raw_path, f"governed surface {surface_id} {field_name}", failures)
            if not _repo_path(raw_path).is_file():
                failures.append(_failure(f"governed surface {surface_id} {field_name} references missing file {raw_path}"))
        identity_path = surface.get("identity_path")
        expected_identity = surface.get("expected_identity")
        source_path = surface.get("source_path")
        if (
            isinstance(identity_path, list)
            and all(isinstance(segment, str) for segment in identity_path)
            and isinstance(expected_identity, str)
            and isinstance(source_path, str)
            and _repo_path(source_path).is_file()
        ):
            payload = _load_payload(_repo_path(source_path), f"governed surface {surface_id}", failures)
            if payload:
                observed = _nested_value(payload, identity_path)
                if observed != expected_identity:
                    failures.append(
                        _failure(
                            f"governed surface {surface_id} identity drifted: "
                            f"expected {expected_identity!r}, observed {observed!r}"
                        )
                    )
    return len(seen)


def _validate_release_governance(
    manifest: dict[str, Any],
    failures: list[str],
    release_governance_override: Path | None = None,
) -> None:
    source = manifest.get("source_of_truth")
    if not isinstance(source, dict):
        return
    raw_release_manifest = source.get("release_governance_manifest")
    if release_governance_override is None and not isinstance(raw_release_manifest, str):
        failures.append(_failure("release governance manifest path is missing"))
        return
    release_manifest_path = (
        release_governance_override
        if release_governance_override is not None
        else _repo_path(str(raw_release_manifest))
    )
    release_manifest = _load_payload(
        release_manifest_path,
        "release ABI/API governance manifest",
        failures,
    )
    if not release_manifest:
        return
    issue_refs = release_manifest.get("release_blocker_issue_refs")
    if not isinstance(issue_refs, list) or "#8173" not in issue_refs:
        failures.append(_failure("release ABI/API governance manifest is not release-blocked by #8173"))
    if release_manifest.get("gate_action") != "check-release-abi-api-drift":
        failures.append(_failure("release ABI/API governance gate action drifted"))
    release_policy = release_manifest.get("allowed_transition_policy")
    blocked = (
        release_policy.get("release_blocked_transitions")
        if isinstance(release_policy, dict)
        else []
    )
    blocked_set = {value for value in blocked if isinstance(value, str)}
    missing_blocked = sorted(REQUIRED_BLOCKED_TRANSITIONS - blocked_set)
    if missing_blocked:
        failures.append(_failure("release ABI/API governance lost release blockers: " + ", ".join(missing_blocked)))
    compatibility_window = release_manifest.get("compatibility_window_policy")
    forbidden = (
        compatibility_window.get("expected_forbidden_claims")
        if isinstance(compatibility_window, dict)
        else []
    )
    forbidden_set = {value for value in forbidden if isinstance(value, str)}
    required_forbidden = {
        "cross-major forward compatibility",
        "indefinite support window",
    }
    missing_forbidden = sorted(required_forbidden - forbidden_set)
    if missing_forbidden:
        failures.append(_failure("release ABI/API governance lost forbidden claims: " + ", ".join(missing_forbidden)))


def _validate_compatibility_policy(manifest: dict[str, Any], failures: list[str]) -> None:
    release_blocked = {
        value
        for value in manifest.get("release_blocked_transitions", [])
        if isinstance(value, str)
    }
    missing = sorted(REQUIRED_BLOCKED_TRANSITIONS - release_blocked)
    if missing:
        failures.append(_failure("ABI governance release_blocked_transitions lost blockers: " + ", ".join(missing)))
    policy = manifest.get("compatibility_policy")
    if not isinstance(policy, dict):
        failures.append(_failure("compatibility_policy must be an object"))
        return
    if policy.get("shim_policy") != "no-compatibility-shims":
        failures.append(_failure("compatibility_policy must reject compatibility shims"))
    if policy.get("fallback_route_policy") != "no-fallback-routes":
        failures.append(_failure("compatibility_policy must reject fallback routes"))
    if policy.get("private_helper_graduation_policy") != "requires-approved-governance-record":
        failures.append(_failure("private helper graduation must require an approved governance record"))


def run_check(
    *,
    manifest_path: Path = MANIFEST_PATH,
    summary_path: Path = SUMMARY_PATH,
    release_governance_override: Path | None = None,
) -> tuple[int, dict[str, Any]]:
    failures: list[str] = []
    manifest = _load_payload(manifest_path, "ABI governance manifest", failures)
    governed_surface_count = 0
    if manifest:
        _validate_manifest_schema(manifest, failures)
        _validate_source_of_truth(manifest, failures)
        _validate_posture(manifest, failures)
        governed_surface_count = _validate_governed_surfaces(manifest, failures)
        _validate_compatibility_policy(manifest, failures)
        _validate_release_governance(manifest, failures, release_governance_override)

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS" if not failures else "FAIL",
        "diagnostic_code": DIAGNOSTIC_CODE,
        "manifest_path": _display_path(manifest_path),
        "schema_path": _repo_rel(SCHEMA_PATH),
        "release_blocker_issue_refs": manifest.get("issue_refs", []) if manifest else [],
        "governed_surface_count": governed_surface_count,
        "failure_count": len(failures),
        "failures": failures,
    }
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(summary_path, summary, sort_keys=True)
    return (0 if not failures else 1), summary


def _parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--manifest", type=Path, default=MANIFEST_PATH)
    parser.add_argument("--summary", type=Path, default=SUMMARY_PATH)
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = _parser().parse_args(argv)
    rc, summary = run_check(manifest_path=args.manifest, summary_path=args.summary)
    try:
        display_summary = _repo_rel(args.summary)
    except ValueError:
        display_summary = args.summary.as_posix()
    print(f"summary_path: {display_summary}")
    print(
        "objc3c-abi-governance: PASS"
        if rc == 0
        else f"objc3c-abi-governance: FAIL ({summary['failure_count']} failures)"
    )
    return rc


if __name__ == "__main__":
    raise SystemExit(main())

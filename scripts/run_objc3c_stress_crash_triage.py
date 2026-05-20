#!/usr/bin/env python3
"""Build crash-signature triage and replay indexes from stress minimization output."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json


ROOT = Path(__file__).resolve().parents[1]
ARTIFACT_SURFACE_PATH = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "artifact_surface.json"
FIXTURE_MANIFEST_PATH = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "crash_triage_fixture_manifest.json"
MINIMIZATION_SUMMARY_PATH = ROOT / "tmp" / "reports" / "stress" / "minimization-summary.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "stress" / "crash-triage-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stress.crash.triage.summary.v1"
SIGNATURE_SHA256_RE = re.compile(r"^[0-9a-f]{64}$")


@dataclass(frozen=True)
class ValidatedCaseArtifacts:
    case_id: str
    failure_dir: Path
    minimized_dir: Path
    signature_sha256: str
    failure_summary: dict[str, Any]
    stable_signature: dict[str, Any]
    reduced_summary: dict[str, Any]


def require_case_string(case: dict[str, Any], field_name: str) -> str:
    value = case.get(field_name)
    if not isinstance(value, str) or not value:
        case_id = case.get("case_id", "<unknown>")
        raise RuntimeError(f"stress minimization case {case_id} missing {field_name}")
    return value


def require_repo_file(path_value: object, *, case_id: str, field_name: str) -> None:
    if not isinstance(path_value, str) or not path_value:
        raise RuntimeError(f"stress crash triage fixture {case_id} missing {field_name}")
    relative_path = Path(path_value)
    if relative_path.is_absolute() or ".." in relative_path.parts:
        raise RuntimeError(f"stress crash triage fixture {case_id} {field_name} must be repo-relative")
    resolved = (ROOT / relative_path).resolve()
    try:
        resolved.relative_to(ROOT.resolve())
    except ValueError as exc:
        raise RuntimeError(f"stress crash triage fixture {case_id} {field_name} escapes the repo") from exc
    if not resolved.is_file():
        raise RuntimeError(
            f"stress crash triage fixture {case_id} missing {field_name}: {relative_path.as_posix()}"
        )


def validate_fixture_manifest(payload: dict[str, Any]) -> dict[str, Any]:
    if payload.get("contract_id") != "objc3c.stress.crash.triage.fixture.manifest.v1":
        raise RuntimeError("stress crash triage fixture manifest contract_id drifted")
    if payload.get("schema_version") != 1:
        raise RuntimeError("stress crash triage fixture manifest schema_version drifted")
    positive_cases = payload.get("positive_cases")
    negative_cases = payload.get("negative_cases")
    if not isinstance(positive_cases, list) or not positive_cases:
        raise RuntimeError("stress crash triage fixture manifest missing positive_cases")
    if not isinstance(negative_cases, list) or not negative_cases:
        raise RuntimeError("stress crash triage fixture manifest missing negative_cases")

    for case in positive_cases:
        if not isinstance(case, dict):
            raise RuntimeError("stress crash triage fixture manifest has non-object positive case")
        case_id = str(case.get("case_id", ""))
        if not case_id:
            raise RuntimeError("stress crash triage fixture positive case missing case_id")
        require_repo_file(case.get("source_path"), case_id=case_id, field_name="source_path")
        artifacts = case.get("expected_triage_artifacts")
        if not isinstance(artifacts, list) or not artifacts:
            raise RuntimeError(f"stress crash triage fixture {case_id} missing expected_triage_artifacts")
        replay_fields = case.get("expected_replay_request_fields")
        if not isinstance(replay_fields, list) or not replay_fields:
            raise RuntimeError(f"stress crash triage fixture {case_id} missing expected_replay_request_fields")

    for case in negative_cases:
        if not isinstance(case, dict):
            raise RuntimeError("stress crash triage fixture manifest has non-object negative case")
        case_id = str(case.get("case_id", ""))
        if not case_id:
            raise RuntimeError("stress crash triage fixture negative case missing case_id")
        if not isinstance(case.get("expected_error"), str) or not case.get("expected_error"):
            raise RuntimeError(f"stress crash triage fixture {case_id} missing expected_error")
        diagnostic = case.get("stable_diagnostic")
        if not isinstance(diagnostic, dict):
            raise RuntimeError(f"stress crash triage fixture {case_id} missing stable_diagnostic")
        if not isinstance(diagnostic.get("code"), str) or not diagnostic.get("code"):
            raise RuntimeError(f"stress crash triage fixture {case_id} missing stable diagnostic code")
        if not isinstance(diagnostic.get("source_range"), dict):
            raise RuntimeError(f"stress crash triage fixture {case_id} missing source_range")

    return {
        "contract_id": payload["contract_id"],
        "positive_case_count": len(positive_cases),
        "negative_case_count": len(negative_cases),
    }


def require_signature(case: dict[str, Any]) -> str:
    signature_sha256 = require_case_string(case, "signature_sha256")
    if SIGNATURE_SHA256_RE.fullmatch(signature_sha256) is None:
        case_id = case.get("case_id", "<unknown>")
        raise RuntimeError(
            f"stress minimization case {case_id} has invalid signature_sha256"
        )
    return signature_sha256


def resolve_repo_relative_dir(case: dict[str, Any], field_name: str) -> Path:
    relative_path = Path(require_case_string(case, field_name))
    case_id = case.get("case_id", "<unknown>")
    if relative_path.is_absolute() or ".." in relative_path.parts:
        raise RuntimeError(
            f"stress minimization case {case_id} {field_name} must be repo-relative"
        )
    resolved = (ROOT / relative_path).resolve()
    try:
        resolved.relative_to(ROOT.resolve())
    except ValueError as exc:
        raise RuntimeError(
            f"stress minimization case {case_id} {field_name} escapes the repo"
        ) from exc
    if not resolved.is_dir():
        raise RuntimeError(
            f"stress minimization case {case_id} {field_name} is missing: {relative_path.as_posix()}"
        )
    return resolved


def require_machine_owned_artifact_dir(
    *,
    resolved: Path,
    artifact_surface: dict[str, Any],
    case_id: str,
    field_name: str,
) -> None:
    roots = artifact_surface.get("machine_owned_artifact_roots")
    if not isinstance(roots, list) or not roots:
        raise RuntimeError("stress artifact surface missing machine_owned_artifact_roots")
    for root in roots:
        if not isinstance(root, str) or not root:
            raise RuntimeError(
                "stress artifact surface has invalid machine_owned_artifact_roots entry"
            )
        try:
            resolved.relative_to((ROOT / root).resolve())
            return
        except ValueError:
            continue
    raise RuntimeError(
        f"stress minimization case {case_id} {field_name} is outside machine-owned artifact roots"
    )


def require_required_artifacts(
    *,
    root: Path,
    artifact_names: object,
    case_id: str,
    artifact_group: str,
) -> None:
    if not isinstance(artifact_names, list) or not artifact_names:
        raise RuntimeError(f"stress artifact surface missing {artifact_group}")
    for artifact_name in artifact_names:
        if not isinstance(artifact_name, str) or not artifact_name:
            raise RuntimeError(
                f"stress artifact surface has invalid {artifact_group} entry"
            )
        artifact_path = root / artifact_name
        if not artifact_path.is_file():
            raise RuntimeError(
                f"stress case {case_id} missing {artifact_group} artifact: {artifact_name}"
            )


def require_artifact_signature(
    *,
    payload: dict[str, Any],
    field_name: str,
    expected_signature: str,
    case_id: str,
    artifact_name: str,
) -> None:
    if payload.get(field_name) != expected_signature:
        raise RuntimeError(
            f"stress case {case_id} {artifact_name} {field_name} drifted from minimization summary"
        )


def require_non_empty_diagnostic_lines(
    *,
    payload: dict[str, Any],
    case_id: str,
    artifact_name: str,
) -> None:
    diagnostic_lines = payload.get("diagnostic_lines")
    if not isinstance(diagnostic_lines, list) or not all(
        isinstance(line, str) and line for line in diagnostic_lines
    ):
        raise RuntimeError(
            f"stress case {case_id} {artifact_name} missing stable diagnostic_lines"
        )


def require_positive_int(
    *,
    payload: dict[str, Any],
    field_name: str,
    case_id: str,
    artifact_name: str,
) -> int:
    value = payload.get(field_name)
    if not isinstance(value, int) or value <= 0:
        raise RuntimeError(f"stress case {case_id} {artifact_name} missing {field_name}")
    return value


def load_validated_case_artifacts(
    *,
    case: dict[str, Any],
    artifact_surface: dict[str, Any],
) -> ValidatedCaseArtifacts:
    case_id = require_case_string(case, "case_id")
    signature_sha256 = require_signature(case)
    failure_dir = resolve_repo_relative_dir(case, "failure_dir")
    minimized_dir = resolve_repo_relative_dir(case, "minimized_dir")
    require_machine_owned_artifact_dir(
        resolved=failure_dir,
        artifact_surface=artifact_surface,
        case_id=case_id,
        field_name="failure_dir",
    )
    require_machine_owned_artifact_dir(
        resolved=minimized_dir,
        artifact_surface=artifact_surface,
        case_id=case_id,
        field_name="minimized_dir",
    )

    require_required_artifacts(
        root=failure_dir,
        artifact_names=artifact_surface.get("failure_capsule_required_artifacts"),
        case_id=case_id,
        artifact_group="failure_capsule_required_artifacts",
    )
    require_required_artifacts(
        root=minimized_dir,
        artifact_names=artifact_surface.get("reducer_session_required_artifacts"),
        case_id=case_id,
        artifact_group="reducer_session_required_artifacts",
    )

    failure_summary = load_json(failure_dir / "failure-summary.json")
    stable_signature = load_json(failure_dir / "stable-signature.json")
    reduced_summary = load_json(minimized_dir / "reduced-summary.json")

    require_artifact_signature(
        payload=failure_summary,
        field_name="signature_sha256",
        expected_signature=signature_sha256,
        case_id=case_id,
        artifact_name="failure-summary.json",
    )
    require_artifact_signature(
        payload=reduced_summary,
        field_name="signature_sha256",
        expected_signature=signature_sha256,
        case_id=case_id,
        artifact_name="reduced-summary.json",
    )
    if stable_signature.get("returncode") != failure_summary.get("returncode"):
        raise RuntimeError(
            f"stress case {case_id} stable-signature.json returncode drifted from failure-summary.json"
        )
    require_non_empty_diagnostic_lines(
        payload=failure_summary,
        case_id=case_id,
        artifact_name="failure-summary.json",
    )
    require_non_empty_diagnostic_lines(
        payload=stable_signature,
        case_id=case_id,
        artifact_name="stable-signature.json",
    )
    original_bytes = require_positive_int(
        payload=reduced_summary,
        field_name="original_bytes",
        case_id=case_id,
        artifact_name="reduced-summary.json",
    )
    reduced_bytes = require_positive_int(
        payload=reduced_summary,
        field_name="reduced_bytes",
        case_id=case_id,
        artifact_name="reduced-summary.json",
    )
    if reduced_bytes > original_bytes:
        raise RuntimeError(
            f"stress case {case_id} reduced-summary.json grew beyond original bytes"
        )

    return ValidatedCaseArtifacts(
        case_id=case_id,
        failure_dir=failure_dir,
        minimized_dir=minimized_dir,
        signature_sha256=signature_sha256,
        failure_summary=failure_summary,
        stable_signature=stable_signature,
        reduced_summary=reduced_summary,
    )


def require_written_triage_artifacts(
    *,
    triage_root: Path,
    artifact_surface: dict[str, Any],
) -> None:
    require_required_artifacts(
        root=triage_root,
        artifact_names=artifact_surface.get("triage_required_artifacts"),
        case_id="<triage>",
        artifact_group="triage_required_artifacts",
    )


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--artifact-surface", type=Path, default=ARTIFACT_SURFACE_PATH)
    parser.add_argument("--fixture-manifest", type=Path, default=FIXTURE_MANIFEST_PATH)
    parser.add_argument("--minimization-summary", type=Path, default=MINIMIZATION_SUMMARY_PATH)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    parser.add_argument("--contract-mode", action="store_true")
    return parser.parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    artifact_surface = load_json(args.artifact_surface.resolve())
    fixture_manifest = load_json(args.fixture_manifest.resolve())
    fixture_manifest_summary = validate_fixture_manifest(fixture_manifest)
    if artifact_surface.get("contract_id") != "objc3c.stress.artifact.surface.v1":
        raise RuntimeError("stress artifact surface contract_id drifted")
    if artifact_surface.get("schema_version") != 1:
        raise RuntimeError("stress artifact surface schema_version drifted")
    minimization_summary = load_json(args.minimization_summary.resolve())
    if minimization_summary.get("contract_id") != "objc3c.stress.minimization.summary.v1":
        raise RuntimeError("stress minimization summary contract_id drifted")
    if minimization_summary.get("status") != "PASS":
        raise RuntimeError("stress minimization summary did not pass")

    case_summaries = minimization_summary.get("case_summaries")
    if not isinstance(case_summaries, list) or not case_summaries:
        raise RuntimeError("stress minimization summary missing case_summaries")

    validated_cases = []
    for case in case_summaries:
        if not isinstance(case, dict):
            raise RuntimeError("stress minimization summary contains a non-object case summary")
        validated_cases.append(
            load_validated_case_artifacts(case=case, artifact_surface=artifact_surface)
        )

    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    triage_root = ROOT / "tmp" / "artifacts" / "stress" / "triage" / run_id
    replay_root = ROOT / "tmp" / "artifacts" / "stress" / "replays" / run_id
    triage_root.mkdir(parents=True, exist_ok=True)
    replay_root.mkdir(parents=True, exist_ok=True)

    signature_groups: dict[str, dict[str, Any]] = {}
    case_index: list[dict[str, Any]] = []
    for case in validated_cases:
        replay_dir = replay_root / case.case_id
        replay_dir.mkdir(parents=True, exist_ok=True)
        replay_request_path = replay_dir / "replay-request.json"
        replay_request = {
            "case_id": case.case_id,
            "signature_sha256": case.signature_sha256,
            "source_path": f"{repo_rel(case.failure_dir / 'source.objc3')}",
            "reduced_candidate_path": (
                f"{repo_rel(case.minimized_dir / 'candidate.objc3')}"
            ),
            "recommended_command": [
                "artifacts/bin/objc3c-native.exe",
                repo_rel(case.failure_dir / "source.objc3"),
                "--out-dir",
                repo_rel(replay_dir / "out"),
                "--emit-prefix",
                "module",
            ],
        }
        replay_request_path.write_text(
            json.dumps(replay_request, indent=2) + "\n",
            encoding="utf-8",
        )

        signature_group = signature_groups.setdefault(
            case.signature_sha256,
            {
                "signature_sha256": case.signature_sha256,
                "case_ids": [],
                "returncodes": set(),
                "diagnostic_lines": case.stable_signature.get("diagnostic_lines", []),
            },
        )
        signature_group["case_ids"].append(case.case_id)
        signature_group["returncodes"].add(case.failure_summary.get("returncode"))
        case_index.append(
            {
                "case_id": case.case_id,
                "signature_sha256": case.signature_sha256,
                "failure_dir": repo_rel(case.failure_dir),
                "minimized_dir": repo_rel(case.minimized_dir),
                "replay_request_path": repo_rel(replay_request_path),
                "original_bytes": case.reduced_summary.get("original_bytes"),
                "reduced_bytes": case.reduced_summary.get("reduced_bytes"),
            }
        )

    signature_index = [
        {
            "signature_sha256": key,
            "case_ids": sorted(value["case_ids"]),
            "returncodes": sorted(value["returncodes"]),
            "diagnostic_lines": value["diagnostic_lines"],
        }
        for key, value in sorted(signature_groups.items())
    ]
    case_index = sorted(case_index, key=lambda item: item["case_id"])

    signature_index_path = triage_root / "signature-index.json"
    case_index_path = triage_root / "case-index.json"
    triage_summary_path = triage_root / "triage-summary.json"
    signature_index_path.write_text(json.dumps(signature_index, indent=2) + "\n", encoding="utf-8")
    case_index_path.write_text(json.dumps(case_index, indent=2) + "\n", encoding="utf-8")
    triage_summary_path.write_text(
        json.dumps(
            {
                "signature_count": len(signature_index),
                "case_count": len(case_index),
                "replay_root": repo_rel(replay_root),
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    require_written_triage_artifacts(
        triage_root=triage_root,
        artifact_surface=artifact_surface,
    )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "artifact_surface_path": repo_rel(args.artifact_surface.resolve()),
        "fixture_manifest_path": repo_rel(args.fixture_manifest.resolve()),
        "fixture_manifest_contract_id": fixture_manifest_summary["contract_id"],
        "fixture_manifest_summary": fixture_manifest_summary,
        "minimization_summary_path": repo_rel(args.minimization_summary.resolve()),
        "triage_root": repo_rel(triage_root),
        "replay_root": repo_rel(replay_root),
        "signature_index_path": repo_rel(signature_index_path),
        "case_index_path": repo_rel(case_index_path),
        "triage_summary_path": repo_rel(triage_summary_path),
        "signature_count": len(signature_index),
        "case_count": len(case_index),
        "artifact_surface_summary_reports": artifact_surface.get("summary_reports"),
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    if args.contract_mode:
        sys.stdout.write(json.dumps(payload, indent=2) + "\n")
    else:
        print(f"summary_path: {repo_rel(args.summary_out)}")
        print("objc3c-stress-crash-triage: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

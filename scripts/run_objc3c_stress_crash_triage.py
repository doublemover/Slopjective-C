#!/usr/bin/env python3
"""Build crash-signature triage and replay indexes from stress minimization output."""

from __future__ import annotations

import argparse
import json
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
ARTIFACT_SURFACE_PATH = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "artifact_surface.json"
MINIMIZATION_SUMMARY_PATH = ROOT / "tmp" / "reports" / "stress" / "minimization-summary.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "stress" / "crash-triage-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stress.crash.triage.summary.v1"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"expected JSON object at {repo_rel(path)}")
    return payload


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--artifact-surface", type=Path, default=ARTIFACT_SURFACE_PATH)
    parser.add_argument("--minimization-summary", type=Path, default=MINIMIZATION_SUMMARY_PATH)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    parser.add_argument("--contract-mode", action="store_true")
    return parser.parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    artifact_surface = load_json(args.artifact_surface.resolve())
    if artifact_surface.get("contract_id") != "objc3c.stress.artifact.surface.v1":
        raise RuntimeError("stress artifact surface contract_id drifted")
    minimization_summary = load_json(args.minimization_summary.resolve())
    if minimization_summary.get("contract_id") != "objc3c.stress.minimization.summary.v1":
        raise RuntimeError("stress minimization summary contract_id drifted")

    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    triage_root = ROOT / "tmp" / "artifacts" / "stress" / "triage" / run_id
    replay_root = ROOT / "tmp" / "artifacts" / "stress" / "replays" / run_id
    triage_root.mkdir(parents=True, exist_ok=True)
    replay_root.mkdir(parents=True, exist_ok=True)

    signature_groups: dict[str, dict[str, Any]] = {}
    case_index: list[dict[str, Any]] = []
    case_summaries = minimization_summary.get("case_summaries")
    if not isinstance(case_summaries, list) or not case_summaries:
        raise RuntimeError("stress minimization summary missing case_summaries")

    for case in case_summaries:
        if not isinstance(case, dict):
            raise RuntimeError("stress minimization summary contains a non-object case summary")
        case_id = str(case.get("case_id"))
        failure_dir = ROOT / str(case.get("failure_dir"))
        minimized_dir = ROOT / str(case.get("minimized_dir"))
        signature_sha256 = str(case.get("signature_sha256"))
        failure_summary = load_json(failure_dir / "failure-summary.json")
        stable_signature = load_json(failure_dir / "stable-signature.json")
        reduced_summary = load_json(minimized_dir / "reduced-summary.json")

        replay_dir = replay_root / case_id
        replay_dir.mkdir(parents=True, exist_ok=True)
        replay_request_path = replay_dir / "replay-request.json"
        replay_request = {
            "case_id": case_id,
            "signature_sha256": signature_sha256,
            "source_path": f"{repo_rel(failure_dir / 'source.objc3')}",
            "reduced_candidate_path": f"{repo_rel(minimized_dir / 'candidate.objc3')}",
            "recommended_command": [
                "artifacts/bin/objc3c-native.exe",
                repo_rel(failure_dir / "source.objc3"),
                "--out-dir",
                repo_rel(replay_dir / "out"),
                "--emit-prefix",
                "module",
            ],
        }
        replay_request_path.write_text(json.dumps(replay_request, indent=2) + "\n", encoding="utf-8")

        signature_group = signature_groups.setdefault(
            signature_sha256,
            {
                "signature_sha256": signature_sha256,
                "case_ids": [],
                "returncodes": set(),
                "diagnostic_lines": stable_signature.get("diagnostic_lines", []),
            },
        )
        signature_group["case_ids"].append(case_id)
        signature_group["returncodes"].add(failure_summary.get("returncode"))
        case_index.append(
            {
                "case_id": case_id,
                "signature_sha256": signature_sha256,
                "failure_dir": repo_rel(failure_dir),
                "minimized_dir": repo_rel(minimized_dir),
                "replay_request_path": repo_rel(replay_request_path),
                "original_bytes": reduced_summary.get("original_bytes"),
                "reduced_bytes": reduced_summary.get("reduced_bytes"),
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

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "artifact_surface_path": repo_rel(args.artifact_surface.resolve()),
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

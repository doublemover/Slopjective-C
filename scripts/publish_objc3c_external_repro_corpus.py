#!/usr/bin/env python3
"""Publish a machine-owned external repro corpus summary from checked-in manifests."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "external_validation" / "source_surface.json"
ARTIFACT_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "external_validation" / "artifact_surface.json"
INTAKE_REPLAY_SCRIPT = ROOT / "scripts" / "run_objc3c_external_validation_replay.py"
SUMMARY_CONTRACT_ID = "objc3c.external_validation.publication.summary.v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "external-validation" / "publication-summary.json"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON object expected at {repo_rel(path)}")
    return payload


def run_capture(command: list[str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        command,
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def ensure_replay_summary(path: Path) -> dict[str, Any]:
    if path.is_file():
        payload = load_json(path)
        if payload.get("status") == "PASS":
            return payload
    result = run_capture([sys.executable, str(INTAKE_REPLAY_SCRIPT)])
    expect(result.returncode == 0, "external intake replay summary generation failed during publication")
    payload = load_json(path)
    expect(payload.get("status") == "PASS", "external intake replay summary did not pass")
    return payload


def main() -> int:
    source_surface = load_json(SOURCE_SURFACE)
    artifact_surface = load_json(ARTIFACT_SURFACE)
    replay_summary = ensure_replay_summary(ROOT / str(artifact_surface["intake_replay_summary"]))
    intake_manifest = load_json(ROOT / str(source_surface["intake_manifest"]))
    quarantine_manifest = load_json(ROOT / str(source_surface["quarantine_manifest"]))

    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    publication_root = ROOT / str(artifact_surface["publication_root"]) / run_id
    publication_root.mkdir(parents=True, exist_ok=True)
    corpus_path = publication_root / "external-repro-corpus.json"

    accepted_entries = [entry for entry in intake_manifest["entries"] if entry.get("trust_state") == "accepted"]
    redacted_entries = [
        {
            "fixture_id": entry["fixture_id"],
            "trust_state": entry["trust_state"],
            "diagnostic_id": entry["diagnostic_id"],
            "escalation_target": entry["escalation_target"],
            "disclosure_compatibility": entry["disclosure_compatibility"],
        }
        for entry in quarantine_manifest["entries"]
        if entry.get("disclosure_compatibility") == "redacted-summary"
    ]
    blocked_entries = [
        entry["fixture_id"]
        for entry in quarantine_manifest["entries"]
        if entry.get("disclosure_compatibility") in {"internal-only", "blocked"}
    ]

    corpus_payload = {
        "contract_id": "objc3c.external_validation.publication.corpus.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "accepted_entries": accepted_entries,
        "redacted_entries": redacted_entries,
        "blocked_fixture_ids": blocked_entries,
        "replay_summary_path": repo_rel(ROOT / str(artifact_surface["intake_replay_summary"])),
    }
    corpus_path.write_text(json.dumps(corpus_payload, indent=2) + "\n", encoding="utf-8")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/publish_objc3c_external_repro_corpus.py",
        "publication_artifact_path": repo_rel(corpus_path),
        "replay_summary_path": repo_rel(ROOT / str(artifact_surface["intake_replay_summary"])),
        "accepted_fixture_count": len(accepted_entries),
        "redacted_fixture_count": len(redacted_entries),
        "blocked_fixture_count": len(blocked_entries),
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-external-repro-publication: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

#!/usr/bin/env python3
"""Validate objc3c release-operations metadata end to end."""

from __future__ import annotations

import json
import os
import subprocess
import sys
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
UPDATE_MANIFEST = ROOT / "tmp" / "artifacts" / "release-operations" / "update-manifest" / "objc3c-update-manifest.json"
COMPATIBILITY_REPORT = ROOT / "tmp" / "artifacts" / "release-operations" / "publication" / "objc3c-compatibility-report.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-operations" / "end-to-end-summary.json"


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{repo_rel(path)} did not contain a JSON object")
    return payload


def run_capture(command: Sequence[str], *, capture_output: bool = True) -> subprocess.CompletedProcess[str]:
    env = os.environ.copy()
    env["PYTHONDONTWRITEBYTECODE"] = "1"
    result = subprocess.run(
        list(command),
        cwd=ROOT,
        text=True,
        capture_output=capture_output,
        check=False,
        env=env,
    )
    if capture_output and result.stdout:
        sys.stdout.write(result.stdout)
    if capture_output and result.stderr:
        sys.stderr.write(result.stderr)
    return result


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def main() -> int:
    result = run_capture([sys.executable, str(RUNNER), "validate-release-operations"], capture_output=False)
    if result.returncode != 0:
        raise RuntimeError("validate-release-operations failed")

    update_manifest = load_json(UPDATE_MANIFEST)
    compatibility_report = load_json(COMPATIBILITY_REPORT)

    channel_ids = [entry.get("channel_id") for entry in update_manifest.get("channels", [])]
    expect(channel_ids == ["stable", "candidate", "preview"], f"channel ids drifted: {channel_ids}")
    expect(update_manifest.get("default_channel") == "stable", "default channel drifted")
    expect(len(compatibility_report.get("rollback_guidance", [])) >= 3, "rollback guidance drifted")
    expect(len(compatibility_report.get("warnings", [])) >= 3, "compatibility warnings drifted")

    stable = next(entry for entry in update_manifest["channels"] if entry["channel_id"] == "stable")
    for artifact_key in ("portable_archive", "installer_archive", "offline_archive"):
        artifact_rel = stable["artifacts"][artifact_key]
        artifact_path = ROOT / artifact_rel.replace("/", os.sep)
        expect(artifact_path.is_file(), f"missing stable artifact {artifact_rel}")

    summary = {
        "contract_id": "objc3c.release.operations.end-to-end.summary.v1",
        "status": "PASS",
        "update_manifest": repo_rel(UPDATE_MANIFEST),
        "compatibility_report": repo_rel(COMPATIBILITY_REPORT),
        "channels": channel_ids,
        "stable_artifacts": stable["artifacts"],
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-release-operations-end-to-end: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

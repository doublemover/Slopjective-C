#!/usr/bin/env python3
"""Run accepted external-validation entries through the live replay proof surfaces."""

from __future__ import annotations

import json
import re
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.subprocesses import run_capture


ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "external_validation" / "source_surface.json"
ARTIFACT_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "external_validation" / "artifact_surface.json"
SUMMARY_CONTRACT_ID = "objc3c.external_validation.intake.replay.summary.v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "external-validation" / "intake-replay-summary.json"
SUMMARY_PATH_PATTERN = re.compile(r"summary_path:\s*(?P<path>\S+)")
REPLAY_SCRIPT_ARGS = {
    "scripts/check_objc3c_execution_replay_proof.ps1": ["-Limit", "1"],
}



def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def extract_summary_paths(stdout: str) -> list[str]:
    return [match.group("path").replace("\\", "/") for match in SUMMARY_PATH_PATTERN.finditer(stdout)]


def main() -> int:
    source_surface = load_json(SOURCE_SURFACE)
    artifact_surface = load_json(ARTIFACT_SURFACE)
    intake_manifest = load_json(ROOT / str(source_surface["intake_manifest"]))
    quarantine_manifest = load_json(ROOT / str(source_surface["quarantine_manifest"]))

    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    intake_runs_root = ROOT / str(artifact_surface["intake_runs_root"])
    run_root = intake_runs_root / run_id
    run_root.mkdir(parents=True, exist_ok=True)
    accepted_index_path = run_root / "accepted-intake-index.json"
    quarantine_index_path = run_root / "quarantine-index.json"

    accepted_entries = [entry for entry in intake_manifest["entries"] if entry.get("trust_state") == "accepted"]
    quarantine_entries = list(quarantine_manifest["entries"])
    expect(accepted_entries, "external validation intake manifest did not contain accepted entries")

    accepted_index_path.write_text(json.dumps({"entries": accepted_entries}, indent=2) + "\n", encoding="utf-8")
    quarantine_index_path.write_text(json.dumps({"entries": quarantine_entries}, indent=2) + "\n", encoding="utf-8")

    replay_steps: list[dict[str, Any]] = []
    seen_scripts: set[str] = set()
    child_report_paths: list[str] = []
    for entry in accepted_entries:
        replay_script = str(entry["replay_script"])
        if replay_script in seen_scripts:
            continue
        seen_scripts.add(replay_script)
        replay_args = REPLAY_SCRIPT_ARGS.get(replay_script, [])
        result = run_capture(
            [
                PWSH,
                "-NoProfile",
                "-ExecutionPolicy",
                "Bypass",
                "-File",
                str(ROOT / replay_script),
                *replay_args,
            ]
        )
        step_report_paths = extract_summary_paths(result.stdout)
        child_report_paths.extend(step_report_paths)
        replay_steps.append(
            {
                "replay_script": replay_script,
                "replay_args": replay_args,
                "exit_code": result.returncode,
                "report_paths": step_report_paths,
            }
        )
        expect(result.returncode == 0, f"external validation replay failed for {replay_script}")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/run_objc3c_external_validation_replay.py",
        "accepted_index_path": repo_rel(accepted_index_path),
        "quarantine_index_path": repo_rel(quarantine_index_path),
        "accepted_fixture_count": len(accepted_entries),
        "quarantine_fixture_count": len(quarantine_entries),
        "replay_steps": replay_steps,
        "child_report_paths": child_report_paths,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-external-validation-replay: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

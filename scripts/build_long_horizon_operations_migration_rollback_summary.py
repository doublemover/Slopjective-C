#!/usr/bin/env python3
"""Build the conversion replay, revert, and support-window semantics summary."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.subprocesses import run_timed


ROOT = Path(__file__).resolve().parents[1]
SEMANTICS_PATH = ROOT / "tests" / "tooling" / "fixtures" / "long_horizon_operations" / "migration_rollback_support_window_semantics.json"
VERSIONING_MODEL = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "versioning_model.json"
UPDATE_MANIFEST = ROOT / "tmp" / "artifacts" / "release-operations" / "update-manifest" / "objc3c-update-manifest.json"
UPGRADE_SUPPORT_REPORT = ROOT / "tmp" / "artifacts" / "release-operations" / "publication" / "objc3c-upgrade-support-report.json"
CHANNEL_CATALOG = ROOT / "tmp" / "artifacts" / "release-operations" / "publication" / "objc3c-release-channel-catalog.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "long-horizon-operations" / "conversion-replay-revert-support-window-summary.json"




def run_step(command: list[str]) -> dict[str, object]:
    result = run_timed(command, cwd=ROOT, echo=True)
    return {
        "command": command,
        "exit_code": result.returncode,
        "duration_ms": result.duration_ms,
        "stdout": result.stdout,
        "stderr": result.stderr,
    }


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    semantics = load_json(SEMANTICS_PATH)
    versioning = load_json(VERSIONING_MODEL)
    failures: list[str] = []
    steps = []

    for raw_path in semantics.get("depends_on", []):
        expect((ROOT / str(raw_path)).is_file(), f"missing dependency {raw_path}", failures)

    for raw_command in semantics.get("generation_commands", []):
        command = str(raw_command).split()
        step = run_step(command)
        steps.append(step)
        expect(step["exit_code"] == 0, f"generation command failed: {raw_command}", failures)
        if step["exit_code"] != 0:
            break

    for raw_path in semantics.get("generated_inputs", []):
        expect((ROOT / str(raw_path)).is_file(), f"missing generated input {raw_path}", failures)

    update_manifest = load_json(UPDATE_MANIFEST) if UPDATE_MANIFEST.is_file() else {}
    upgrade_support_report = load_json(UPGRADE_SUPPORT_REPORT) if UPGRADE_SUPPORT_REPORT.is_file() else {}
    channel_catalog = load_json(CHANNEL_CATALOG) if CHANNEL_CATALOG.is_file() else {}
    version_support_windows = versioning.get("support_windows", {})

    expect(update_manifest.get("current_version") == versioning.get("current_stable_version"), "update manifest current_version drifted", failures)
    expect(upgrade_support_report.get("current_version") == update_manifest.get("current_version"), "upgrade support report current_version drifted", failures)
    expect(channel_catalog.get("default_channel") == update_manifest.get("default_channel"), "channel catalog default_channel drifted", failures)
    expect(
        upgrade_support_report.get("support_windows") == version_support_windows,
        "upgrade support report support windows drifted from versioning model",
        failures,
    )
    expect(
        upgrade_support_report.get("supported_platform_ids") == update_manifest.get("supported_platform_ids"),
        "upgrade support report supported platforms drifted from update manifest",
        failures,
    )

    revert_rules = semantics.get("revert_rules", [])
    revert_guidance = upgrade_support_report.get("revert_guidance", [])
    revert_channels = {str(entry.get("channel_id")) for entry in revert_guidance if isinstance(entry, dict)}
    for rule in revert_rules if isinstance(revert_rules, list) else []:
        if not isinstance(rule, dict):
            failures.append("revert rule must be an object")
            continue
        channel_id = str(rule.get("channel_id"))
        expect(channel_id in revert_channels, f"missing revert guidance for {channel_id}", failures)
        matching = [entry for entry in revert_guidance if isinstance(entry, dict) and entry.get("channel_id") == channel_id]
        if matching:
            expect(
                isinstance(matching[0].get("instruction"), str) and bool(matching[0].get("instruction")),
                f"revert instruction missing for {channel_id}",
                failures,
            )

    replay_requirements = semantics.get("conversion_replay_requirements", [])
    expect(isinstance(replay_requirements, list) and len(replay_requirements) >= 8, "conversion replay requirements are too narrow", failures)

    payload = {
        "contract_id": "objc3c.long_horizon_operations.conversion_replay_revert_support_window.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "semantics": repo_rel(SEMANTICS_PATH),
        "versioning_model": repo_rel(VERSIONING_MODEL),
        "update_manifest": repo_rel(UPDATE_MANIFEST),
        "upgrade_support_report": repo_rel(UPGRADE_SUPPORT_REPORT),
        "channel_catalog": repo_rel(CHANNEL_CATALOG),
        "step_count": len(steps),
        "steps": steps,
        "support_window_count": len(version_support_windows) if isinstance(version_support_windows, dict) else 0,
        "upgrade_path_count": len(upgrade_support_report.get("upgrade_paths", [])) if isinstance(upgrade_support_report.get("upgrade_paths"), list) else 0,
        "revert_rule_count": len(revert_rules) if isinstance(revert_rules, list) else 0,
        "revert_channel_count": len(revert_channels),
        "conversion_replay_requirements": replay_requirements,
        "fail_closed_conditions": semantics.get("fail_closed_conditions", []),
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("long-horizon-conversion-revert: PASS" if not failures else "long-horizon-conversion-revert: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())

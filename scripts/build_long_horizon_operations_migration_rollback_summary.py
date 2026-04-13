#!/usr/bin/env python3
"""Build the migration, rollback, and support-window semantics summary."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
SEMANTICS_PATH = ROOT / "tests" / "tooling" / "fixtures" / "long_horizon_operations" / "migration_rollback_support_window_semantics.json"
VERSIONING_MODEL = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "versioning_model.json"
UPDATE_MANIFEST = ROOT / "tmp" / "artifacts" / "release-operations" / "update-manifest" / "objc3c-update-manifest.json"
COMPATIBILITY_REPORT = ROOT / "tmp" / "artifacts" / "release-operations" / "publication" / "objc3c-compatibility-report.json"
CHANNEL_CATALOG = ROOT / "tmp" / "artifacts" / "release-operations" / "publication" / "objc3c-release-channel-catalog.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "long-horizon-operations" / "migration-rollback-support-window-summary.json"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def run_step(command: list[str]) -> dict[str, object]:
    result = subprocess.run(command, cwd=ROOT, text=True, capture_output=True, check=False)
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return {
        "command": command,
        "exit_code": result.returncode,
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
    compatibility_report = load_json(COMPATIBILITY_REPORT) if COMPATIBILITY_REPORT.is_file() else {}
    channel_catalog = load_json(CHANNEL_CATALOG) if CHANNEL_CATALOG.is_file() else {}
    version_support_windows = versioning.get("support_windows", {})

    expect(update_manifest.get("current_version") == versioning.get("current_stable_version"), "update manifest current_version drifted", failures)
    expect(compatibility_report.get("current_version") == update_manifest.get("current_version"), "compatibility report current_version drifted", failures)
    expect(channel_catalog.get("default_channel") == update_manifest.get("default_channel"), "channel catalog default_channel drifted", failures)
    expect(
        compatibility_report.get("support_windows") == version_support_windows,
        "compatibility report support windows drifted from versioning model",
        failures,
    )
    expect(
        compatibility_report.get("supported_platform_ids") == update_manifest.get("supported_platform_ids"),
        "compatibility report supported platforms drifted from update manifest",
        failures,
    )

    rollback_rules = semantics.get("rollback_rules", [])
    rollback_guidance = compatibility_report.get("rollback_guidance", [])
    rollback_channels = {str(entry.get("channel_id")) for entry in rollback_guidance if isinstance(entry, dict)}
    for rule in rollback_rules if isinstance(rollback_rules, list) else []:
        if not isinstance(rule, dict):
            failures.append("rollback rule must be an object")
            continue
        channel_id = str(rule.get("channel_id"))
        expect(channel_id in rollback_channels, f"missing rollback guidance for {channel_id}", failures)
        matching = [entry for entry in rollback_guidance if isinstance(entry, dict) and entry.get("channel_id") == channel_id]
        if matching:
            expect(
                matching[0].get("recommended_transport") == rule.get("rollback_transport"),
                f"rollback transport drifted for {channel_id}",
                failures,
            )

    replay_requirements = semantics.get("migration_replay_requirements", [])
    expect(isinstance(replay_requirements, list) and len(replay_requirements) >= 8, "migration replay requirements are too narrow", failures)

    payload = {
        "contract_id": "objc3c.long_horizon_operations.migration_rollback_support_window.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "semantics": repo_rel(SEMANTICS_PATH),
        "versioning_model": repo_rel(VERSIONING_MODEL),
        "update_manifest": repo_rel(UPDATE_MANIFEST),
        "compatibility_report": repo_rel(COMPATIBILITY_REPORT),
        "channel_catalog": repo_rel(CHANNEL_CATALOG),
        "step_count": len(steps),
        "steps": steps,
        "support_window_count": len(version_support_windows) if isinstance(version_support_windows, dict) else 0,
        "upgrade_path_count": len(compatibility_report.get("upgrade_paths", [])) if isinstance(compatibility_report.get("upgrade_paths"), list) else 0,
        "rollback_rule_count": len(rollback_rules) if isinstance(rollback_rules, list) else 0,
        "rollback_channel_count": len(rollback_channels),
        "migration_replay_requirements": replay_requirements,
        "fail_closed_conditions": semantics.get("fail_closed_conditions", []),
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("long-horizon-migration-rollback: PASS" if not failures else "long-horizon-migration-rollback: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())

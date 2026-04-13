#!/usr/bin/env python3
"""Build the adoption migration playbook semantics summary."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
SEMANTICS_PATH = ROOT / "tests" / "tooling" / "fixtures" / "adoption_legibility" / "migration_playbook_semantics.json"
PACKAGE_JSON = ROOT / "package.json"
LONG_HORIZON_MIGRATION = ROOT / "tests" / "tooling" / "fixtures" / "long_horizon_operations" / "migration_rollback_support_window_semantics.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "adoption-legibility" / "migration-playbook-summary.json"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    semantics = load_json(SEMANTICS_PATH)
    package = load_json(PACKAGE_JSON)
    long_horizon = load_json(LONG_HORIZON_MIGRATION)
    scripts = package.get("scripts", {})
    if not isinstance(scripts, dict):
        raise RuntimeError("package.json scripts field drifted from an object")

    failures: list[str] = []
    dependencies = [str(path) for path in semantics.get("depends_on", [])]
    for raw_path in dependencies:
        expect((ROOT / raw_path).is_file(), f"missing dependency {raw_path}", failures)

    phases = semantics.get("playbook_phases", [])
    expect(isinstance(phases, list) and len(phases) >= 4, "migration playbook must have at least four phases", failures)
    phase_sequences = [int(phase.get("sequence", -1)) for phase in phases if isinstance(phase, dict)]
    expect(phase_sequences == sorted(phase_sequences) == list(range(1, len(phase_sequences) + 1)), "playbook phase sequence drifted", failures)
    missing_paths: list[str] = []
    missing_public_scripts: list[str] = []
    phase_ids: list[str] = []
    for phase in phases if isinstance(phases, list) else []:
        if not isinstance(phase, dict):
            failures.append("playbook phase entry must be an object")
            continue
        phase_id = str(phase.get("phase_id"))
        phase_ids.append(phase_id)
        required_paths = [str(path) for path in phase.get("required_paths", [])]
        required_scripts = [str(name) for name in phase.get("required_public_scripts", [])]
        expect(len(required_paths) >= 3, f"{phase_id} must name concrete paths", failures)
        expect(len(required_scripts) >= 2, f"{phase_id} must name public scripts", failures)
        missing_paths.extend(path for path in required_paths if not (ROOT / path).is_file())
        missing_public_scripts.extend(name for name in required_scripts if name not in scripts)

    axes = semantics.get("interop_guidance_axes", [])
    expect(isinstance(axes, list) and len(axes) >= 3, "interop guidance must cover ObjC2, Swift-facing, and C++-facing axes", failures)
    for axis in axes if isinstance(axes, list) else []:
        if not isinstance(axis, dict):
            failures.append("interop guidance axis entry must be an object")
            continue
        axis_id = str(axis.get("axis_id"))
        showcase_anchor = str(axis.get("showcase_anchor"))
        runbook_anchor = str(axis.get("runbook_anchor"))
        expect(showcase_anchor.endswith(".objc3") and (ROOT / showcase_anchor).is_file(), f"{axis_id} missing runnable showcase anchor", failures)
        expect((ROOT / runbook_anchor).is_file(), f"{axis_id} missing runbook anchor", failures)
        expect(len(axis.get("deferred_behavior", [])) >= 2, f"{axis_id} must name deferred behavior", failures)

    required_replay_fields = [str(field) for field in semantics.get("required_migration_replay_fields", [])]
    long_horizon_fields = [str(field) for field in long_horizon.get("migration_replay_requirements", [])]
    expect(required_replay_fields == long_horizon_fields, "migration replay fields drift from long-horizon support semantics", failures)
    if missing_paths:
        failures.append("playbook phases reference missing paths")
    if missing_public_scripts:
        failures.append("playbook phases reference missing public scripts")

    payload = {
        "contract_id": "objc3c.adoption_legibility.migration_playbook.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "semantics": repo_rel(SEMANTICS_PATH),
        "long_horizon_migration_semantics": repo_rel(LONG_HORIZON_MIGRATION),
        "phase_count": len(phases) if isinstance(phases, list) else 0,
        "phase_ids": phase_ids,
        "interop_axis_count": len(axes) if isinstance(axes, list) else 0,
        "required_migration_replay_fields": required_replay_fields,
        "missing_paths": sorted(set(missing_paths)),
        "missing_public_scripts": sorted(set(missing_public_scripts)),
        "fail_closed_conditions": semantics.get("fail_closed_conditions", []),
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("adoption-legibility-migration-playbook: PASS" if not failures else "adoption-legibility-migration-playbook: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())

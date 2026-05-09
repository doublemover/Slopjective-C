#!/usr/bin/env python3
"""Build the adoption replay semantics summary."""

from __future__ import annotations

from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.public_runner import public_workflow_action_names


ROOT = Path(__file__).resolve().parents[1]
SEMANTICS_PATH = ROOT / "tests" / "tooling" / "fixtures" / "adoption_legibility" / "adoption_replay_semantics.json"
PACKAGE_JSON = ROOT / "package.json"
LONG_HORIZON_REPLAY = ROOT / "tests" / "tooling" / "fixtures" / "long_horizon_operations" / "migration_rollback_support_window_semantics.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "adoption-legibility" / "adoption-replay-summary.json"




def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    semantics = load_json(SEMANTICS_PATH)
    package = load_json(PACKAGE_JSON)
    long_horizon = load_json(LONG_HORIZON_REPLAY)
    package_scripts = package.get("scripts", {})
    if not isinstance(package_scripts, dict):
        raise RuntimeError("package.json scripts field drifted from an object")
    package_bridge = str(semantics["package_bridge"])
    package_bridge_exists = package_bridge in package_scripts
    registered_actions = set(public_workflow_action_names())

    failures: list[str] = []
    dependencies = [str(path) for path in semantics.get("depends_on", [])]
    for raw_path in dependencies:
        expect((ROOT / raw_path).is_file(), f"missing dependency {raw_path}", failures)

    phases = semantics.get("adoption_replay_phases", [])
    expect(isinstance(phases, list) and len(phases) >= 4, "adoption replay must have at least four phases", failures)
    phase_sequences = [int(phase.get("sequence", -1)) for phase in phases if isinstance(phase, dict)]
    expect(phase_sequences == sorted(phase_sequences) == list(range(1, len(phase_sequences) + 1)), "adoption replay phase sequence drifted", failures)
    missing_paths: list[str] = []
    missing_actions: list[str] = []
    phase_ids: list[str] = []
    for phase in phases if isinstance(phases, list) else []:
        if not isinstance(phase, dict):
            failures.append("adoption replay phase entry must be an object")
            continue
        phase_id = str(phase.get("phase_id"))
        phase_ids.append(phase_id)
        required_paths = [str(path) for path in phase.get("required_paths", [])]
        required_actions = [str(name) for name in phase.get("required_actions", [])]
        expect(len(required_paths) >= 3, f"{phase_id} must name concrete paths", failures)
        expect(len(required_actions) >= 2, f"{phase_id} must name workflow actions", failures)
        missing_paths.extend(path for path in required_paths if not (ROOT / path).is_file())
        missing_actions.extend(name for name in required_actions if name not in registered_actions)

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

    required_replay_fields = [str(field) for field in semantics.get("required_adoption_replay_fields", [])]
    long_horizon_fields = [str(field) for field in long_horizon.get("migration_replay_requirements", [])]
    expect(required_replay_fields == long_horizon_fields, "adoption replay fields drift from long-horizon support semantics", failures)
    if missing_paths:
        failures.append("adoption replay phases reference missing paths")
    if not package_bridge_exists:
        failures.append("adoption replay package bridge is missing")
    if missing_actions:
        failures.append("adoption replay phases reference missing workflow actions")

    payload = {
        "contract_id": "objc3c.adoption_legibility.adoption_replay.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "semantics": repo_rel(SEMANTICS_PATH),
        "long_horizon_replay_semantics": repo_rel(LONG_HORIZON_REPLAY),
        "phase_count": len(phases) if isinstance(phases, list) else 0,
        "phase_ids": phase_ids,
        "interop_axis_count": len(axes) if isinstance(axes, list) else 0,
        "required_adoption_replay_fields": required_replay_fields,
        "missing_paths": sorted(set(missing_paths)),
        "package_bridge": package_bridge,
        "package_bridge_count": 1 if package_bridge_exists else 0,
        "missing_package_bridge": [] if package_bridge_exists else [package_bridge],
        "missing_actions": sorted(set(missing_actions)),
        "fail_closed_conditions": semantics.get("fail_closed_conditions", []),
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("adoption-legibility-adoption-replay: PASS" if not failures else "adoption-legibility-adoption-replay: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())

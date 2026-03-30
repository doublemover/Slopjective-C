#!/usr/bin/env python3
"""Validate the checked-in stress source surface and emit a summary."""

from __future__ import annotations

import json
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
SURFACE_PATH = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "source_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "stress" / "source-surface-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stress.source.surface.summary.v1"
EXPECTED_FAMILIES = [
    "parser-sema-fuzz",
    "lowering-runtime-stress",
    "mixed-module-differential",
    "replay-backed-contracts",
]


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def fail(message: str) -> int:
    print(f"stress-source-surface: FAIL\n- {message}", file=sys.stderr)
    return 1


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"expected JSON object at {repo_rel(path)}")
    return payload


def require_path(relative_path: str, *, kind: str) -> Path:
    path = ROOT / relative_path
    if not path.exists():
        raise RuntimeError(f"missing {kind}: {relative_path}")
    return path


def main() -> int:
    if not SURFACE_PATH.is_file():
        return fail(f"missing stress source surface contract: {repo_rel(SURFACE_PATH)}")

    surface = load_json(SURFACE_PATH)
    if surface.get("contract_id") != "objc3c.stress.source.surface.v1":
        return fail("stress source surface contract_id drifted")
    if surface.get("schema_version") != 1:
        return fail("stress source surface schema_version drifted")
    if surface.get("runbook") != "docs/runbooks/objc3c_stress_validation.md":
        return fail("stress source surface runbook drifted")
    if surface.get("source_root") != "tests/tooling/fixtures/stress":
        return fail("stress source surface source_root drifted")
    if surface.get("source_check_script") != "scripts/check_stress_source_surface.py":
        return fail("stress source surface source_check_script drifted")
    if surface.get("safety_policy") != "tests/tooling/fixtures/stress/safety_policy.json":
        return fail("stress source surface safety_policy drifted")

    require_path("docs/runbooks/objc3c_stress_validation.md", kind="runbook")
    require_path("tests/tooling/fixtures/stress/README.md", kind="stress README")
    safety_policy_path = require_path("tests/tooling/fixtures/stress/safety_policy.json", kind="stress safety policy")
    safety_policy = load_json(safety_policy_path)
    if safety_policy.get("policy_id") != "objc3c.stress.validation.safety-policy.v1":
        return fail("stress safety policy_id drifted")
    if safety_policy.get("schema_version") != 1:
        return fail("stress safety policy schema_version drifted")
    for key in (
        "determinism_rule",
        "allowed_input_classes",
        "allowed_execution_modes",
        "required_guards",
        "differential_rules",
        "forbidden_patterns",
        "claim_boundary",
    ):
        if key not in safety_policy:
            return fail(f"stress safety policy missing {key}")

    checked_in_roots = surface.get("checked_in_roots")
    if not isinstance(checked_in_roots, list) or not checked_in_roots:
        return fail("checked_in_roots missing")
    for relative_path in checked_in_roots:
        if not isinstance(relative_path, str) or not relative_path:
            return fail("checked_in_roots contains a non-string entry")
        require_path(relative_path, kind="checked-in stress root")

    source_families = surface.get("source_families")
    if not isinstance(source_families, list) or not source_families:
        return fail("source_families missing")

    family_ids: list[str] = []
    family_summaries: list[dict[str, Any]] = []
    for family in source_families:
        if not isinstance(family, dict):
            return fail("source_families contains a non-object entry")
        family_id = family.get("family_id")
        coverage_goal = family.get("coverage_goal")
        source_paths = family.get("source_paths")
        if not isinstance(family_id, str) or not family_id:
            return fail("source_families entry missing family_id")
        if not isinstance(coverage_goal, str) or not coverage_goal:
            return fail(f"{family_id} missing coverage_goal")
        if not isinstance(source_paths, list) or not source_paths:
            return fail(f"{family_id} missing source_paths")
        for source_path in source_paths:
            if not isinstance(source_path, str) or not source_path:
                return fail(f"{family_id} contains a non-string source path")
            require_path(source_path, kind=f"{family_id} source path")
        family_ids.append(family_id)
        family_summaries.append(
            {
                "family_id": family_id,
                "source_path_count": len(source_paths),
                "coverage_goal": coverage_goal,
            }
        )

    if family_ids != EXPECTED_FAMILIES:
        return fail("source family inventory drifted")

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "source_surface_contract": repo_rel(SURFACE_PATH),
        "runbook": surface["runbook"],
        "source_check_script": surface["source_check_script"],
        "safety_policy": repo_rel(safety_policy_path),
        "checked_in_root_count": len(checked_in_roots),
        "required_guard_count": len(safety_policy["required_guards"]),
        "family_summaries": family_summaries,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("stress-source-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

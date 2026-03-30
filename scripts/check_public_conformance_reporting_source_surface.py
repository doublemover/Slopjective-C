#!/usr/bin/env python3
"""Validate the checked-in public conformance reporting source surface."""

from __future__ import annotations

import json
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
SOURCE_SURFACE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "public_conformance_reporting"
    / "source_surface.json"
)
SUMMARY_PATH = ROOT / "tmp" / "reports" / "public-conformance" / "source-surface-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.public_conformance_reporting.source.surface.summary.v1"
EXPECTED_ROOTS = [
    "tests/tooling/fixtures/public_conformance_reporting",
    "tests/tooling/fixtures/external_validation",
    "tests/conformance",
    "docs/runbooks",
    "schemas",
    "scripts",
]
EXPECTED_FAMILY_IDS = [
    "upstream-conformance-evidence",
    "external-credibility-evidence",
    "public-reporting-schema-anchors",
]


def fail(message: str) -> int:
    print(f"public-conformance-reporting-source-surface: FAIL\n- {message}", file=sys.stderr)
    return 1


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def require_path(relative_path: str, *, kind: str) -> Path:
    path = ROOT / relative_path
    if not path.exists():
        raise RuntimeError(f"missing {kind}: {relative_path}")
    return path


def main() -> int:
    if not SOURCE_SURFACE.is_file():
        return fail(f"missing source surface contract: {repo_rel(SOURCE_SURFACE)}")

    surface = load_json(SOURCE_SURFACE)
    if surface.get("contract_id") != "objc3c.public_conformance_reporting.source.surface.v1":
        return fail("contract_id drifted")
    if surface.get("schema_version") != 1:
        return fail("schema_version drifted")
    if surface.get("runbook") != "docs/runbooks/objc3c_public_conformance_reporting.md":
        return fail("runbook drifted")
    if surface.get("source_root") != "tests/tooling/fixtures/public_conformance_reporting":
        return fail("source_root drifted")
    if surface.get("source_readme") != "tests/tooling/fixtures/public_conformance_reporting/README.md":
        return fail("source_readme drifted")
    if surface.get("source_check_script") != "scripts/check_public_conformance_reporting_source_surface.py":
        return fail("source_check_script drifted")
    if surface.get("stability_policy") != "tests/tooling/fixtures/public_conformance_reporting/stability_policy.json":
        return fail("stability_policy drifted")
    if surface.get("schema_surface") != "tests/tooling/fixtures/public_conformance_reporting/schema_surface.json":
        return fail("schema_surface drifted")
    if surface.get("checked_in_roots") != EXPECTED_ROOTS:
        return fail("checked_in_roots drifted")

    require_path("docs/runbooks/objc3c_public_conformance_reporting.md", kind="runbook")
    require_path("tests/tooling/fixtures/public_conformance_reporting/README.md", kind="source readme")
    policy_path = require_path(
        "tests/tooling/fixtures/public_conformance_reporting/stability_policy.json",
        kind="stability policy",
    )
    require_path(
        "tests/tooling/fixtures/public_conformance_reporting/schema_surface.json",
        kind="schema surface",
    )
    for root in EXPECTED_ROOTS:
        require_path(root, kind="checked-in root")

    policy = load_json(policy_path)
    if policy.get("contract_id") != "objc3c.public_conformance_reporting.stability.policy.v1":
        return fail("stability policy contract_id drifted")
    if policy.get("schema_version") != 1:
        return fail("stability policy schema_version drifted")
    if policy.get("allowed_public_statuses") != ["pass", "caution", "blocked"]:
        return fail("stability policy allowed_public_statuses drifted")
    if policy.get("allowed_badges") != ["claim-ready", "provisional", "blocked"]:
        return fail("stability policy allowed_badges drifted")
    score_bands = policy.get("score_bands")
    if not isinstance(score_bands, list) or len(score_bands) != 3:
        return fail("stability policy score_bands drifted")
    fail_closed_conditions = policy.get("fail_closed_conditions")
    if not isinstance(fail_closed_conditions, list) or len(fail_closed_conditions) < 4:
        return fail("stability policy fail_closed_conditions drifted")

    families = surface.get("source_families")
    if not isinstance(families, list) or len(families) != len(EXPECTED_FAMILY_IDS):
        return fail("source_families drifted")

    family_summaries: list[dict[str, Any]] = []
    observed_family_ids: list[str] = []
    for family in families:
        if not isinstance(family, dict):
            return fail("source_families contains a non-object entry")
        family_id = family.get("family_id")
        coverage_goal = family.get("coverage_goal")
        source_paths = family.get("source_paths")
        if not isinstance(family_id, str) or not family_id:
            return fail("source_families entry missing family_id")
        if not isinstance(coverage_goal, str) or not coverage_goal:
            return fail(f"{family_id} is missing coverage_goal")
        if not isinstance(source_paths, list) or not source_paths:
            return fail(f"{family_id} is missing source_paths")
        observed_family_ids.append(family_id)
        checked_paths: list[str] = []
        for source_path in source_paths:
            if not isinstance(source_path, str) or not source_path:
                return fail(f"{family_id} contains a non-string source path")
            require_path(source_path, kind=f"{family_id} source path")
            checked_paths.append(source_path)
        family_summaries.append(
            {
                "family_id": family_id,
                "source_path_count": len(checked_paths),
                "source_paths": checked_paths,
            }
        )

    if observed_family_ids != EXPECTED_FAMILY_IDS:
        return fail("source_families inventory drifted")

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "source_surface_contract": repo_rel(SOURCE_SURFACE),
        "runbook": surface["runbook"],
        "source_check_script": surface["source_check_script"],
        "stability_policy": surface["stability_policy"],
        "schema_surface": surface["schema_surface"],
        "checked_in_roots": EXPECTED_ROOTS,
        "family_summaries": family_summaries,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("public-conformance-reporting-source-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

#!/usr/bin/env python3
"""Validate the checked-in external validation source surface."""

from __future__ import annotations

import json
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "external_validation" / "source_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "external-validation" / "source-surface-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.external_validation.source.surface.summary.v1"
EXPECTED_ROOTS = [
    "tests/tooling/fixtures/external_validation",
    "tests/tooling/fixtures/objc3c",
    "tests/conformance",
    "docs/runbooks",
]
EXPECTED_FAMILY_IDS = [
    "intake-normalization-boundary",
    "independent-replay-proofs",
    "packaged-reproducibility-surface",
]


def fail(message: str) -> int:
    print(f"external-validation-source-surface: FAIL\n- {message}", file=sys.stderr)
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
    if surface.get("contract_id") != "objc3c.external_validation.source.surface.v1":
        return fail("contract_id drifted")
    if surface.get("schema_version") != 1:
        return fail("schema_version drifted")
    if surface.get("runbook") != "docs/runbooks/objc3c_external_validation.md":
        return fail("runbook drifted")
    if surface.get("source_root") != "tests/tooling/fixtures/external_validation":
        return fail("source_root drifted")
    if surface.get("source_readme") != "tests/tooling/fixtures/external_validation/README.md":
        return fail("source_readme drifted")
    if surface.get("source_check_script") != "scripts/check_external_validation_source_surface.py":
        return fail("source_check_script drifted")
    if surface.get("trust_policy") != "tests/tooling/fixtures/external_validation/trust_policy.json":
        return fail("trust_policy drifted")
    if surface.get("checked_in_roots") != EXPECTED_ROOTS:
        return fail("checked_in_roots drifted")

    require_path("docs/runbooks/objc3c_external_validation.md", kind="runbook")
    require_path("tests/tooling/fixtures/external_validation/README.md", kind="source readme")
    trust_policy_path = require_path(
        "tests/tooling/fixtures/external_validation/trust_policy.json",
        kind="trust policy",
    )
    for root in EXPECTED_ROOTS:
        require_path(root, kind="checked-in root")

    trust_policy = load_json(trust_policy_path)
    if trust_policy.get("contract_id") != "objc3c.external_validation.trust.policy.v1":
        return fail("trust policy contract_id drifted")
    if trust_policy.get("schema_version") != 1:
        return fail("trust policy schema_version drifted")
    if trust_policy.get("allowed_trust_states") != ["candidate", "accepted", "quarantined", "rejected"]:
        return fail("trust policy allowed_trust_states drifted")
    if trust_policy.get("publishable_trust_states") != ["accepted"]:
        return fail("trust policy publishable_trust_states drifted")

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
        "trust_policy": surface["trust_policy"],
        "checked_in_roots": EXPECTED_ROOTS,
        "family_summaries": family_summaries,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("external-validation-source-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

#!/usr/bin/env python3
"""Build the governance extension review policy summary."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel


ROOT = Path(__file__).resolve().parents[1]
FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "governance_sustainability"
POLICY_PATH = FIXTURE_ROOT / "extension_review_policy.json"
TEMPLATE_PATH = FIXTURE_ROOT / "new_work_proposal_template.json"
RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_governance_sustainability.md"
AUTHOR_GUIDE_PATH = ROOT / "docs" / "governance" / "extension_author_guide_v1.md"
OUT_DIR = ROOT / "tmp" / "reports" / "governance-sustainability" / "extension-review-policy"
SUMMARY_PATH = OUT_DIR / "governance_extension_review_policy_summary.json"



def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    policy = read_json(POLICY_PATH)
    template = read_json(TEMPLATE_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    author_guide_text = AUTHOR_GUIDE_PATH.read_text(encoding="utf-8")
    failures: list[str] = []

    for raw_path in (policy.get("runbook"), policy.get("author_guide"), policy.get("proposal_template")):
        expect(isinstance(raw_path, str) and (ROOT / raw_path).is_file(), f"missing policy path {raw_path}", failures)

    review_classes = policy.get("review_classes", [])
    expect(isinstance(review_classes, list) and len(review_classes) >= 4, "review class set is too narrow", failures)
    required_review_classes = {
        "language-extension",
        "support-boundary-impacting-change",
        "package-ecosystem-change",
        "security-or-release-governance-change",
    }
    actual_review_classes = {str(entry.get("review_class")) for entry in review_classes if isinstance(entry, dict)}
    expect(required_review_classes.issubset(actual_review_classes), "required review classes missing", failures)

    missing_evidence_surfaces: list[str] = []
    for entry in review_classes if isinstance(review_classes, list) else []:
        if not isinstance(entry, dict):
            failures.append("review class entry must be an object")
            continue
        review_class = str(entry.get("review_class"))
        requires = entry.get("requires", [])
        surfaces = [str(path) for path in entry.get("required_evidence_surfaces", [])]
        expect(len(requires) >= 5, f"{review_class} required evidence is too narrow", failures)
        expect(len(surfaces) >= 3, f"{review_class} must name at least three evidence surfaces", failures)
        missing_evidence_surfaces.extend(path for path in surfaces if not (ROOT / path).is_file())

    template_required = set(template.get("required_issue_fields", []))
    policy_required = set(policy.get("proposal_required_fields", []))
    expect(policy_required.issubset(template_required), "policy requires fields not present in proposal template", failures)
    expect(any("revert" in item for item in policy.get("fail_closed_conditions", [])), "revert fail-closed condition missing", failures)
    expect(any("GitHub" in item for item in policy.get("publication_rules", [])), "GitHub publication rule missing", failures)
    expect(not missing_evidence_surfaces, f"missing evidence surfaces: {sorted(set(missing_evidence_surfaces))}", failures)

    required_mentions = {
        "runbook_mentions_policy": "tests/tooling/fixtures/governance_sustainability/extension_review_policy.json" in runbook_text,
        "runbook_mentions_summary": "python scripts/build_governance_extension_review_policy_summary.py" in runbook_text,
        "author_guide_mentions_policy": "tests/tooling/fixtures/governance_sustainability/extension_review_policy.json" in author_guide_text,
        "author_guide_mentions_summary": "python scripts/build_governance_extension_review_policy_summary.py" in author_guide_text,
    }
    for key, value in required_mentions.items():
        expect(value, f"{key} is false", failures)

    summary = {
        "contract_id": "objc3c.governance.extension_review_policy.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "policy": repo_rel(POLICY_PATH),
        "proposal_template": repo_rel(TEMPLATE_PATH),
        "review_class_count": len(review_classes) if isinstance(review_classes, list) else 0,
        "review_classes": sorted(actual_review_classes),
        "proposal_required_field_count": len(policy_required),
        "fail_closed_condition_count": len(policy.get("fail_closed_conditions", [])),
        "publication_rule_count": len(policy.get("publication_rules", [])),
        "missing_evidence_surfaces": sorted(set(missing_evidence_surfaces)),
        **required_mentions,
        "failures": failures,
    }
    summary["ok"] = not failures

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, summary)
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())

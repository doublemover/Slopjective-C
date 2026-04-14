#!/usr/bin/env python3
"""Build the runnable governance extension review workflow summary."""

from __future__ import annotations

import json
import subprocess
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel


ROOT = Path(__file__).resolve().parents[1]
FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "governance_sustainability"
TEMPLATE_PATH = FIXTURE_ROOT / "new_work_proposal_template.json"
SAMPLE_PATH = FIXTURE_ROOT / "new_work_proposal_sample.json"
POLICY_PATH = FIXTURE_ROOT / "extension_review_policy.json"
ARTIFACT_CONTRACT_PATH = FIXTURE_ROOT / "artifact_contract.json"
RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_governance_sustainability.md"
OUT_DIR = ROOT / "tmp" / "reports" / "governance-sustainability" / "extension-review-workflow"
SUMMARY_PATH = OUT_DIR / "governance_extension_review_workflow_summary.json"
RENDER_DIR = OUT_DIR / "sample-render"



def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def run_command(command: list[str]) -> dict[str, Any]:
    completed = subprocess.run(
        command,
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    return {
        "command": " ".join(command),
        "exit_code": completed.returncode,
        "ok": completed.returncode == 0,
        "stdout": completed.stdout.strip(),
        "stderr": completed.stderr.strip(),
    }


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    template = read_json(TEMPLATE_PATH)
    sample = read_json(SAMPLE_PATH)
    policy = read_json(POLICY_PATH)
    artifact_contract = read_json(ARTIFACT_CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    failures: list[str] = []

    required_fields = set(template.get("required_issue_fields", []))
    sample_fields = set(sample)
    missing_sample_fields = sorted(required_fields - sample_fields)
    expect(not missing_sample_fields, f"sample proposal missing fields: {missing_sample_fields}", failures)

    review_classes = {
        str(entry.get("review_class")): entry
        for entry in policy.get("review_classes", [])
        if isinstance(entry, dict)
    }
    selected = review_classes.get(str(sample.get("review_class")))
    expect(selected is not None, "sample review_class is not in extension review policy", failures)
    missing_evidence_surfaces: list[str] = []
    if selected is not None:
        required_surfaces = {str(path) for path in selected.get("required_evidence_surfaces", [])}
        sample_surfaces = {str(path) for path in sample.get("evidence_surfaces", [])}
        missing_evidence_surfaces = sorted(required_surfaces - sample_surfaces)
        expect(not missing_evidence_surfaces, f"sample evidence surfaces missing: {missing_evidence_surfaces}", failures)

    render_result = run_command([
        "python",
        "scripts/publish_new_work_proposal.py",
        "--proposal",
        repo_rel(SAMPLE_PATH),
        "--output-dir",
        repo_rel(RENDER_DIR),
        "--preflight-mode",
        "proposal-only",
    ])
    expect(render_result["ok"], "sample proposal render failed", failures)

    render_summary_path = RENDER_DIR / "publication_summary.json"
    render_summary = read_json(render_summary_path) if render_summary_path.is_file() else {}
    expect(bool(render_summary.get("ok")), "sample proposal render summary is not ok", failures)
    expect(render_summary.get("preflight_mode") == "proposal-only", "sample render did not use proposal-only preflight", failures)

    generated_reports = set(artifact_contract.get("generated_reports", []))
    required_scripts = set(artifact_contract.get("required_generation_scripts", []))
    expect(repo_rel(SUMMARY_PATH) in generated_reports, "artifact contract omits extension review workflow summary", failures)
    expect("scripts/build_governance_extension_review_workflow_summary.py" in required_scripts, "artifact contract omits workflow summary script", failures)

    required_mentions = {
        "runbook_mentions_workflow": "python scripts/build_governance_extension_review_workflow_summary.py" in runbook_text,
        "runbook_mentions_proposal_tool": "python scripts/publish_new_work_proposal.py" in runbook_text,
        "runbook_mentions_template": "tests/tooling/fixtures/governance_sustainability/new_work_proposal_template.json" in runbook_text,
    }
    for key, value in required_mentions.items():
        expect(value, f"{key} is false", failures)

    summary = {
        "contract_id": "objc3c.governance.extension_review.workflow.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "template": repo_rel(TEMPLATE_PATH),
        "sample_proposal": repo_rel(SAMPLE_PATH),
        "extension_review_policy": repo_rel(POLICY_PATH),
        "proposal_tool": "scripts/publish_new_work_proposal.py",
        "render_summary": repo_rel(render_summary_path),
        "render_ok": bool(render_summary.get("ok")),
        "review_class": sample.get("review_class"),
        "missing_sample_fields": missing_sample_fields,
        "missing_evidence_surfaces": missing_evidence_surfaces,
        "render_result": {
            "exit_code": render_result["exit_code"],
            "ok": render_result["ok"],
        },
        **required_mentions,
        "failures": failures,
    }
    summary["ok"] = not failures

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())

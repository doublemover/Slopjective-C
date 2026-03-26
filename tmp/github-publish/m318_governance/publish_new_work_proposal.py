#!/usr/bin/env python3
"""Render or publish a governance-compliant new-work proposal."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[3]
ISSUE_BODY_CONTRACT = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_c001_planning_packet_and_issue_template_simplification_contract_and_architecture_freeze_contract.json"
PLANNING_HYGIENE_POLICY = ROOT / "spec" / "governance" / "objc3c_planning_hygiene_policy.json"
EXCEPTION_PROCESS = ROOT / "spec" / "governance" / "objc3c_anti_noise_exception_process.json"
EXCEPTION_REGISTRY = ROOT / "spec" / "governance" / "objc3c_anti_noise_exception_registry.json"
DEFAULT_TEMPLATE = ROOT / "tmp" / "planning" / "m318_governance" / "new_work_proposal_template.json"
DEFAULT_OUTPUT_DIR = ROOT / "tmp" / "reports" / "m318" / "governance" / "new_work_proposal"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--proposal", type=Path, required=True)
    parser.add_argument("--template", type=Path, default=DEFAULT_TEMPLATE)
    parser.add_argument("--output-dir", type=Path, default=DEFAULT_OUTPUT_DIR)
    parser.add_argument("--publish", action="store_true")
    parser.add_argument("--repo")
    return parser.parse_args(argv)


def validate_required_fields(proposal: dict[str, Any], template: dict[str, Any]) -> list[str]:
    failures: list[str] = []
    for field in template["required_issue_fields"]:
        if field not in proposal:
            failures.append(f"missing required proposal field `{field}`")
    return failures


def validate_proposal(proposal: dict[str, Any], template: dict[str, Any]) -> tuple[dict[str, Any], list[str]]:
    failures = validate_required_fields(proposal, template)
    allowed_fields = set(template["required_issue_fields"]) | set(template["optional_issue_fields"])
    for key in proposal:
        if key not in allowed_fields:
            failures.append(f"unknown proposal field `{key}`")

    issue_code = proposal.get("issue_code", "")
    resolved = {"milestone_code": "", "short_code": "", "lane": "", "exception_status": None}
    if not re.match(template["issue_code_pattern"], issue_code):
        failures.append("issue_code does not match required Mxxx-Lnnn pattern")
    else:
        milestone_code, short_code = issue_code.split("-", 1)
        lane = short_code[0]
        resolved.update({"milestone_code": milestone_code, "short_code": short_code, "lane": lane})
        if lane not in "ABCDE":
            failures.append("issue_code lane must be A-E")

    if proposal.get("validation_posture") not in set(template["validation_postures"]):
        failures.append("validation_posture is not allowed by template")
    if proposal.get("budget_impact") not in set(template["budget_impact_options"]):
        failures.append("budget_impact is not allowed by template")

    if not isinstance(proposal.get("acceptance_criteria"), list) or not proposal["acceptance_criteria"]:
        failures.append("acceptance_criteria must be a non-empty list")
    if not isinstance(proposal.get("primary_implementation_surfaces"), list) or not proposal["primary_implementation_surfaces"]:
        failures.append("primary_implementation_surfaces must be a non-empty list")
    if not isinstance(proposal.get("dependencies"), list):
        failures.append("dependencies must be a list")
    if not isinstance(proposal.get("label_names"), list) or not proposal["label_names"]:
        failures.append("label_names must be a non-empty list")

    planning_policy = read_json(PLANNING_HYGIENE_POLICY)
    if proposal.get("budget_impact") not in set(planning_policy["budget_impact_options"]):
        failures.append("budget_impact is not allowed by planning hygiene policy")

    issue_body_contract = read_json(ISSUE_BODY_CONTRACT)
    allowed_postures = {item["code"] for item in issue_body_contract["validation_postures"]}
    if proposal.get("validation_posture") not in allowed_postures:
        failures.append("validation_posture is not allowed by issue body contract")

    exception_process = read_json(EXCEPTION_PROCESS)
    exception_registry = read_json(EXCEPTION_REGISTRY)
    exception_id = proposal.get("exception_record_id")
    if proposal.get("budget_impact") == "requires_exception_record":
        if not exception_id:
            failures.append("exception_record_id is required when budget_impact is requires_exception_record")
        else:
            match = next((item for item in exception_registry.get("exceptions", []) if item.get("id") == exception_id), None)
            if match is None:
                failures.append(f"exception record `{exception_id}` was not found in the registry")
            else:
                resolved["exception_status"] = match.get("status")
                if match.get("status") != "active":
                    failures.append(f"exception record `{exception_id}` is not active")
                required_fields = set(exception_process["required_record_fields"])
                if not required_fields.issubset(match.keys()):
                    failures.append(f"exception record `{exception_id}` is missing required fields")
    elif exception_id:
        match = next((item for item in exception_registry.get("exceptions", []) if item.get("id") == exception_id), None)
        resolved["exception_status"] = match.get("status") if match else "missing"

    execution_order = proposal.get("execution_order")
    if execution_order is not None:
        if not isinstance(execution_order, dict):
            failures.append("execution_order must be an object when present")
        else:
            missing = [field for field in template["execution_order_fields"] if field not in execution_order]
            if missing:
                failures.append("execution_order is missing required fields: " + ", ".join(missing))
    return resolved, failures


def render_issue_title(proposal: dict[str, Any], template: dict[str, Any], resolved: dict[str, Any]) -> str:
    return template["title_format"].format(
        milestone_code=resolved["milestone_code"],
        lane=resolved["lane"],
        short_code=resolved["short_code"],
        title_core=proposal["title_core"],
        kind=proposal["kind"],
    )


def render_issue_body(proposal: dict[str, Any]) -> str:
    lines = [
        "## Outcome",
        f"Deliver `{proposal['issue_code']}` for **{proposal['title_core']}** within the milestone focus: {proposal['milestone_focus_summary']}",
        "",
        "## Why this matters",
        proposal["why_it_matters"],
    ]

    if proposal.get("design_corrections_folded_in"):
        lines += ["", "## Design corrections folded in"] + [f"- {item}" for item in proposal["design_corrections_folded_in"]]

    lines += ["", "## Acceptance criteria"] + [f"- {item}" for item in proposal["acceptance_criteria"]]
    lines += ["", "## Primary implementation surfaces"] + [f"- `{item}`" for item in proposal["primary_implementation_surfaces"]]
    lines += ["", "## Dependencies"]
    if proposal["dependencies"]:
        lines += [f"- `{item}`" for item in proposal["dependencies"]]
    else:
        lines += ["- None."]
    lines += ["", "## Validation posture", f"- Class: `{proposal['validation_posture']}`", f"- Budget impact: `{proposal['budget_impact']}`"]

    if proposal.get("boundary_note"):
        lines += ["", "## Boundary note", proposal["boundary_note"]]
    if proposal.get("post_cleanup_dependency_rewrite"):
        lines += ["", "## Post-cleanup dependency rewrite", proposal["post_cleanup_dependency_rewrite"]]

    if proposal.get("execution_order"):
        order = proposal["execution_order"]
        lines += [
            "",
            "<!-- EXECUTION-ORDER-START -->",
            "## Execution Order",
            f"- Cleanup program slot: `{order['cleanup_program_slot']}`",
            f"- Cleanup program phase: `{order['cleanup_program_phase']}`",
            f"- Global cleanup-first sequence: `{order['global_sequence']}`",
            "- Blocked by prior cleanup milestones: " + (", ".join(f"`{item}`" for item in order["blocked_by_prior_milestones"]) if order["blocked_by_prior_milestones"] else "`None`"),
            "- Direct issue blockers: " + (", ".join(order["direct_issue_blockers"]) if order["direct_issue_blockers"] else "`None`"),
            "- Directly unblocks: " + (", ".join(order["directly_unblocks"]) if order["directly_unblocks"] else "`None`"),
            f"- Execution instruction: {order['instruction']}",
            "<!-- EXECUTION-ORDER-END -->",
        ]

    if proposal.get("notes"):
        lines += ["", "## Notes"] + [f"- {item}" for item in proposal["notes"]]
    return "\n".join(lines) + "\n"


def validate_rendered_sections(body: str) -> list[str]:
    contract = read_json(ISSUE_BODY_CONTRACT)
    missing = [section for section in contract["issue_body_contract"]["required_sections"] if section not in body]
    return [f"rendered issue body is missing required section `{section}`" for section in missing]


def maybe_publish(repo: str, title: str, body: str, proposal: dict[str, Any]) -> dict[str, Any]:
    args = ["gh", "api", f"repos/{repo}/issues", "-X", "POST", "-f", f"title={title}", "-f", f"body={body}"]
    milestone_number = proposal.get("milestone_number")
    if milestone_number is not None:
        args += ["-F", f"milestone={milestone_number}"]
    for label in proposal["label_names"]:
        args += ["-f", f"labels[]={label}"]
    completed = subprocess.run(args, cwd=ROOT, capture_output=True, text=True, check=False)
    if completed.returncode != 0:
        raise RuntimeError((completed.stderr or completed.stdout).strip())
    return json.loads(completed.stdout)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    template = read_json(args.template)
    proposal = read_json(args.proposal)
    resolved, failures = validate_proposal(proposal, template)

    title = render_issue_title(proposal, template, resolved) if not failures else ""
    body = render_issue_body(proposal) if not failures else ""
    if body:
        failures.extend(validate_rendered_sections(body))

    args.output_dir.mkdir(parents=True, exist_ok=True)
    body_path = args.output_dir / template["render_defaults"]["write_paths"]["issue_body"]
    payload_path = args.output_dir / template["render_defaults"]["write_paths"]["issue_payload"]
    summary_path = args.output_dir / template["render_defaults"]["write_paths"]["summary"]

    if body:
        body_path.write_text(title + "\n\n" + body, encoding="utf-8")
    payload = {
        "title": title,
        "body_path": rel(body_path),
        "labels": proposal.get("label_names", []),
        "milestone_title": proposal.get("milestone_title"),
        "milestone_number": proposal.get("milestone_number"),
        "issue_code": proposal.get("issue_code"),
    }
    write_json(payload_path, payload)

    created_issue = None
    publication_mode = "publish" if args.publish else template["render_defaults"]["publish_mode"]
    if args.publish and not failures:
        repo = args.repo
        if not repo:
            completed = subprocess.run(["gh", "repo", "view", "--json", "nameWithOwner"], cwd=ROOT, capture_output=True, text=True, check=False)
            if completed.returncode != 0:
                failures.append((completed.stderr or completed.stdout).strip())
            else:
                repo = json.loads(completed.stdout)["nameWithOwner"]
        if repo and not failures:
            created_issue = maybe_publish(repo, title, body, proposal)

    summary = {
        "mode": "m318-c003-new-work-proposal-tooling-v1",
        "contract_id": template["contract_id"],
        "proposal_path": rel(args.proposal),
        "template_path": rel(args.template),
        "publication_mode": publication_mode,
        "consumed_contract_ids": template["consumed_contract_ids"],
        "issue_code": proposal.get("issue_code"),
        "title": title,
        "labels": proposal.get("label_names", []),
        "milestone_title": proposal.get("milestone_title"),
        "milestone_number": proposal.get("milestone_number"),
        "budget_impact": proposal.get("budget_impact"),
        "validation_posture": proposal.get("validation_posture"),
        "exception_record_id": proposal.get("exception_record_id"),
        "exception_record_status": resolved.get("exception_status"),
        "output_paths": {
            "issue_body": rel(body_path),
            "issue_payload": rel(payload_path),
            "summary": rel(summary_path)
        },
        "published_issue_number": created_issue.get("number") if created_issue else None,
        "ok": not failures,
        "failures": failures
    }
    write_json(summary_path, summary)
    if failures:
        for failure in failures:
            print(f"[fail] {failure}", file=sys.stderr)
        return 1
    print(json.dumps(summary, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

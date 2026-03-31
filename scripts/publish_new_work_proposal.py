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

ROOT = Path(__file__).resolve().parents[1]
GOVERNANCE_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "governance_sustainability" / "sustainable_progress_policy.json"
WAIVER_REGISTRY = ROOT / "tests" / "tooling" / "fixtures" / "governance_sustainability" / "waiver_registry.json"
DEFAULT_TEMPLATE = ROOT / "tests" / "tooling" / "fixtures" / "governance_sustainability" / "new_work_proposal_template.json"
DEFAULT_OUTPUT_DIR = ROOT / "tmp" / "reports" / "m318" / "governance" / "new_work_proposal"
MILESTONE_CODE_RE = re.compile(r"^(M\d{3})")


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


def run_json(command: list[str]) -> Any:
    completed = subprocess.run(
        command,
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if completed.returncode != 0:
        raise RuntimeError(completed.stderr or completed.stdout or f"command failed: {' '.join(command)}")
    return json.loads(completed.stdout)


def run_preflight(command_text: str) -> dict[str, Any]:
    completed = subprocess.run(
        command_text,
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        shell=True,
        check=False,
    )
    return {
        "command": command_text,
        "exit_code": completed.returncode,
        "ok": completed.returncode == 0,
        "stdout": completed.stdout.strip(),
        "stderr": completed.stderr.strip(),
    }


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

    issue_code = str(proposal.get("issue_code", ""))
    resolved = {"milestone_code": "", "short_code": "", "lane": "", "waiver_status": None}
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

    governance_policy = read_json(GOVERNANCE_POLICY)
    waiver_registry = read_json(WAIVER_REGISTRY)

    if "issue_numbers" in proposal or "milestone_numbers" in proposal:
        failures.append("proposal must not carry predicted GitHub numeric identifiers")
    forbidden_source_keys = {"issue_numbers", "milestone_numbers"}
    if forbidden_source_keys & set(proposal.keys()):
        failures.append("proposal includes forbidden source keys")

    if proposal.get("budget_impact") not in set(template["budget_impact_options"]):
        failures.append("budget impact not allowed by template")
    waiver_id = proposal.get("waiver_id")
    if proposal.get("budget_impact") == "requires_exception_record":
        if not waiver_id:
            failures.append("waiver_id is required when budget_impact is requires_exception_record")
        else:
            match = next((item for item in waiver_registry.get("waivers", []) if item.get("waiver_id") == waiver_id), None)
            if match is None:
                failures.append(f"waiver `{waiver_id}` was not found in the waiver registry")
            else:
                resolved["waiver_status"] = match.get("status")
                if match.get("status") != "active":
                    failures.append(f"waiver `{waiver_id}` is not active")
    elif waiver_id:
        match = next((item for item in waiver_registry.get("waivers", []) if item.get("waiver_id") == waiver_id), None)
        resolved["waiver_status"] = match.get("status") if match else "missing"

    if proposal.get("milestone_title"):
        milestone_code = resolved["milestone_code"]
        title_code = MILESTONE_CODE_RE.match(str(proposal["milestone_title"]))
        if title_code and title_code.group(1) != milestone_code:
            failures.append("milestone_title code must match issue_code milestone")

    execution_order = proposal.get("execution_order")
    if execution_order is not None:
        if not isinstance(execution_order, dict):
            failures.append("execution_order must be an object when present")
        else:
            missing = [field for field in template["execution_order_fields"] if field not in execution_order]
            if missing:
                failures.append("execution_order is missing required fields: " + ", ".join(missing))

    if not governance_policy.get("exception_requirements", {}).get("expiry_required", False):
        failures.append("governance policy drifted: expiry requirement must stay enabled")
    return resolved, failures


def render_issue_title(proposal: dict[str, Any], template: dict[str, Any], resolved: dict[str, Any]) -> str:
    return template["title_format"].format(
        milestone_code=resolved["milestone_code"],
        lane=resolved["lane"],
        short_code=resolved["short_code"],
        title_core=proposal["title_core"],
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
    lines += [f"- `{item}`" for item in proposal["dependencies"]] if proposal["dependencies"] else ["- None."]
    lines += ["", "## Validation posture", f"- Class: `{proposal['validation_posture']}`", f"- Budget impact: `{proposal['budget_impact']}`"]
    if proposal.get("waiver_id"):
        lines.append(f"- Waiver: `{proposal['waiver_id']}`")
    if proposal.get("boundary_note"):
        lines += ["", "## Boundary note", proposal["boundary_note"]]
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


def resolve_live_milestone_number(repo: str, milestone_title: str | None, milestone_code: str) -> int | None:
    if not milestone_title:
        return None
    payload = run_json([
        "gh",
        "api",
        f"repos/{repo}/milestones?state=all&per_page=100",
        "--paginate",
        "--slurp",
    ])
    for page in payload:
        if not isinstance(page, list):
            continue
        for item in page:
            if not isinstance(item, dict):
                continue
            title = item.get("title")
            if title == milestone_title:
                return int(item["number"])
            if isinstance(title, str) and title.startswith(milestone_code + " ") and title == milestone_title:
                return int(item["number"])
    return None


def maybe_publish(repo: str, title: str, body: str, proposal: dict[str, Any], milestone_code: str) -> dict[str, Any]:
    args = ["gh", "api", f"repos/{repo}/issues", "-X", "POST", "-f", f"title={title}", "-f", f"body={body}"]
    milestone_number = resolve_live_milestone_number(repo, proposal.get("milestone_title"), milestone_code)
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
    if not args.template.is_absolute():
        args.template = ROOT / args.template
    if not args.proposal.is_absolute():
        args.proposal = ROOT / args.proposal
    if not args.output_dir.is_absolute():
        args.output_dir = ROOT / args.output_dir
    template = read_json(args.template)
    proposal = read_json(args.proposal)
    resolved, failures = validate_proposal(proposal, template)

    title = render_issue_title(proposal, template, resolved) if not failures else ""
    body = render_issue_body(proposal) if not failures else ""

    preflight_results = [run_preflight(command) for command in template.get("governance_preflight_commands", [])]
    preflight_failures = [result["command"] for result in preflight_results if not result["ok"]]
    if preflight_failures:
        failures.append("governance preflight failed: " + ", ".join(preflight_failures))

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
            created_issue = maybe_publish(repo, title, body, proposal, resolved["milestone_code"])

    summary = {
        "mode": template["mode"],
        "contract_id": template["contract_id"],
        "proposal_path": rel(args.proposal),
        "template_path": rel(args.template),
        "publication_mode": publication_mode,
        "consumed_contract_ids": template["consumed_contract_ids"],
        "issue_code": proposal.get("issue_code"),
        "title": title,
        "labels": proposal.get("label_names", []),
        "milestone_title": proposal.get("milestone_title"),
        "budget_impact": proposal.get("budget_impact"),
        "validation_posture": proposal.get("validation_posture"),
        "waiver_id": proposal.get("waiver_id"),
        "waiver_status": resolved.get("waiver_status"),
        "governance_preflight": preflight_results,
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

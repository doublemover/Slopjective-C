#!/usr/bin/env python3
"""Render or publish a governance-compliant new-work proposal."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path
from typing import Any, Sequence
from objc3c_tooling.json_io import write_json_file as write_json

try:
    from new_work_proposal_contracts import (
        ProposalPolicyInputs,
        build_publication_summary,
        render_proposal_artifacts,
        validate_proposal,
    )
except ModuleNotFoundError:
    from scripts.new_work_proposal_contracts import (
        ProposalPolicyInputs,
        build_publication_summary,
        render_proposal_artifacts,
        validate_proposal,
    )

ROOT = Path(__file__).resolve().parents[1]
GOVERNANCE_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "governance_sustainability" / "sustainable_progress_policy.json"
WAIVER_REGISTRY = ROOT / "tests" / "tooling" / "fixtures" / "governance_sustainability" / "waiver_registry.json"
EXTENSION_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "governance_sustainability" / "extension_review_policy.json"
DEFAULT_TEMPLATE = ROOT / "tests" / "tooling" / "fixtures" / "governance_sustainability" / "new_work_proposal_template.json"
DEFAULT_OUTPUT_DIR = ROOT / "tmp" / "reports" / "governance-sustainability" / "new-work-proposal"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))



def rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--proposal", type=Path, required=True)
    parser.add_argument("--template", type=Path, default=DEFAULT_TEMPLATE)
    parser.add_argument("--output-dir", type=Path, default=DEFAULT_OUTPUT_DIR)
    parser.add_argument("--preflight-mode", choices=["full", "proposal-only"], default="full")
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
    governance_policy = read_json(GOVERNANCE_POLICY)
    waiver_registry = read_json(WAIVER_REGISTRY)
    extension_policy = read_json(EXTENSION_POLICY)
    validation = validate_proposal(
        proposal,
        ProposalPolicyInputs(
            template=template,
            governance_policy=governance_policy,
            waiver_registry=waiver_registry,
            extension_policy=extension_policy,
        ),
    )
    resolved = validation.resolved
    failures = validation.failures

    body_path = args.output_dir / template["render_defaults"]["write_paths"]["issue_body"]
    payload_path = args.output_dir / template["render_defaults"]["write_paths"]["issue_payload"]
    summary_path = args.output_dir / template["render_defaults"]["write_paths"]["summary"]
    rendered = render_proposal_artifacts(
        proposal=proposal,
        template=template,
        resolved=resolved,
        body_path=rel(body_path),
        failures=failures,
    )

    preflight_commands = [] if args.preflight_mode == "proposal-only" else template.get("governance_preflight_commands", [])
    preflight_results = [run_preflight(command) for command in preflight_commands]
    preflight_failures = [result["command"] for result in preflight_results if not result["ok"]]
    if preflight_failures:
        failures.append("governance preflight failed: " + ", ".join(preflight_failures))

    args.output_dir.mkdir(parents=True, exist_ok=True)
    if rendered.body:
        body_path.write_text(rendered.title + "\n\n" + rendered.body, encoding="utf-8")
    write_json(payload_path, rendered.payload)

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
            created_issue = maybe_publish(
                repo,
                rendered.title,
                rendered.body,
                proposal,
                resolved["milestone_code"],
            )

    summary = build_publication_summary(
        template=template,
        proposal=proposal,
        resolved=resolved,
        proposal_path=rel(args.proposal),
        template_path=rel(args.template),
        publication_mode=publication_mode,
        preflight_mode=args.preflight_mode,
        title=rendered.title,
        preflight_results=preflight_results,
        output_paths={
            "issue_body": rel(body_path),
            "issue_payload": rel(payload_path),
            "summary": rel(summary_path),
        },
        published_issue_number=created_issue.get("number") if created_issue else None,
        failures=failures,
    )
    write_json(summary_path, summary)
    if failures:
        for failure in failures:
            print(f"[fail] {failure}", file=sys.stderr)
        return 1
    print(json.dumps(summary, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

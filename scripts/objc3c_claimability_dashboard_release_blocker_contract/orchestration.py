from __future__ import annotations

import sys
from pathlib import Path
from typing import Sequence

from .cli import build_arg_parser
from .config import resolve_repo_path
from .dashboard_payload import build_summary
from .errors import ContractError
from .output import write_outputs
from .rendering import render_markdown
from .tooling import expected_json_report, repo_rel


def _report_mismatches(json_out: Path, md_out: Path, next_json: str, next_md: str) -> list[str]:
    mismatches = []
    if not json_out.is_file() or json_out.read_text(encoding="utf-8") != next_json:
        mismatches.append(repo_rel(json_out))
    if not md_out.is_file() or md_out.read_text(encoding="utf-8") != next_md:
        mismatches.append(repo_rel(md_out))
    return mismatches


def main(argv: Sequence[str] | None = None) -> int:
    args = build_arg_parser().parse_args(argv)
    policy_path = resolve_repo_path(args.policy)
    json_out = resolve_repo_path(args.summary_json)
    md_out = resolve_repo_path(args.summary_md)
    try:
        summary = build_summary(policy_path)
    except ContractError as exc:
        print(f"dashboard release-blocker contract error: {exc}", file=sys.stderr)
        return 1

    next_json = expected_json_report(summary, sort_keys=False)
    next_md = render_markdown(summary)
    if args.check:
        mismatches = _report_mismatches(json_out, md_out, next_json, next_md)
        if mismatches:
            print(
                "dashboard release-blocker contract output drift: "
                + ", ".join(mismatches),
                file=sys.stderr,
            )
            return 1
    else:
        write_outputs(summary, json_out, md_out)

    print(
        "dashboard release-blocker contract: {status} ({path})".format(
            status=summary["status"],
            path=repo_rel(json_out),
        )
    )
    return 0 if summary["status"] == "PASS" else 1

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import resolve_repo_path
from objc3c_tooling.public_workflow_output import normalize_newlines

from .blockers import count_open_blockers
from .catalog import count_open_catalog_tasks
from .constants import EXIT_BLOCKED, EXIT_BOOTSTRAPPABLE, EXIT_HARD_FAILURE
from .json_loading import load_json
from .payloads import build_payload
from .rendering import render_markdown
from .snapshots import count_items


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_bootstrap_readiness.py",
        description=(
            "Compute deterministic bootstrap readiness from offline issue, "
            "milestone, catalog, and blocker snapshots."
        ),
    )
    parser.add_argument("--issues-json", type=Path, required=True)
    parser.add_argument("--milestones-json", type=Path, required=True)
    parser.add_argument("--catalog-json", type=Path, required=True)
    parser.add_argument("--open-blockers-json", type=Path)
    parser.add_argument("--format", choices=("json", "md"), default="json")
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)

    issues_path = resolve_repo_path(args.issues_json)
    milestones_path = resolve_repo_path(args.milestones_json)
    catalog_path = resolve_repo_path(args.catalog_json)
    open_blockers_path = (
        resolve_repo_path(args.open_blockers_json)
        if args.open_blockers_json is not None
        else None
    )

    try:
        issues_open_count = count_items(
            load_json(issues_path),
            path=issues_path,
            label="issues",
        )
        milestones_open_count = count_items(
            load_json(milestones_path),
            path=milestones_path,
            label="milestones",
        )
        catalog_open_task_count = count_open_catalog_tasks(
            load_json(catalog_path),
            path=catalog_path,
        )
        blockers_open_count = 0
        if open_blockers_path is not None:
            blockers_open_count = count_open_blockers(
                load_json(open_blockers_path),
                path=open_blockers_path,
            )
    except ValueError as exc:
        print(str(exc), file=sys.stderr)
        return EXIT_HARD_FAILURE

    payload = build_payload(
        issues_open_count=issues_open_count,
        milestones_open_count=milestones_open_count,
        catalog_open_task_count=catalog_open_task_count,
        blockers_open_count=blockers_open_count,
    )
    output = (
        json.dumps(payload, indent=2) + "\n"
        if args.format == "json"
        else render_markdown(payload)
    )
    sys.stdout.write(normalize_newlines(output))
    return (
        EXIT_BOOTSTRAPPABLE
        if payload["readiness_state"] == "bootstrappable"
        else EXIT_BLOCKED
    )

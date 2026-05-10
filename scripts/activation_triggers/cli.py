from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import resolve_repo_path
from objc3c_tooling.public_workflow_output import normalize_newlines

from activation_triggers.inputs import count_actionable_catalog_rows
from activation_triggers.inputs import count_items
from activation_triggers.inputs import load_json
from activation_triggers.inputs import normalize_actionable_statuses
from activation_triggers.inputs import parse_open_blockers
from activation_triggers.inputs import parse_t4_overlay
from activation_triggers.model import EXIT_HARD_FAILURE
from activation_triggers.payload import build_payload
from activation_triggers.rendering import render_markdown


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_activation_triggers.py",
        description="Compute deterministic activation gate state from offline issue, milestone, catalog, and blocker snapshots.",
    )
    parser.add_argument("--issues-json", type=Path, required=True)
    parser.add_argument("--milestones-json", type=Path, required=True)
    parser.add_argument("--catalog-json", type=Path, required=True)
    parser.add_argument("--open-blockers-json", type=Path)
    parser.add_argument("--actionable-status", action="append", dest="actionable_statuses")
    parser.add_argument("--issues-max-age-seconds", type=int)
    parser.add_argument("--milestones-max-age-seconds", type=int)
    parser.add_argument("--t4-governance-overlay-json", type=Path)
    parser.add_argument("--t4-new-scope-publish", action="store_true")
    parser.add_argument("--format", choices=("json", "markdown"), default="json")
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    if args.t4_governance_overlay_json is not None and args.t4_new_scope_publish:
        print(
            "error: --t4-governance-overlay-json and --t4-new-scope-publish are mutually exclusive",
            file=sys.stderr,
        )
        return EXIT_HARD_FAILURE
    if args.issues_max_age_seconds is not None and args.issues_max_age_seconds < 0:
        print("error: --issues-max-age-seconds must be >= 0", file=sys.stderr)
        return EXIT_HARD_FAILURE
    if args.milestones_max_age_seconds is not None and args.milestones_max_age_seconds < 0:
        print("error: --milestones-max-age-seconds must be >= 0", file=sys.stderr)
        return EXIT_HARD_FAILURE

    issues_path = resolve_repo_path(args.issues_json)
    milestones_path = resolve_repo_path(args.milestones_json)
    catalog_path = resolve_repo_path(args.catalog_json)
    open_blockers_path = (
        resolve_repo_path(args.open_blockers_json) if args.open_blockers_json is not None else None
    )
    t4_overlay_path = (
        resolve_repo_path(args.t4_governance_overlay_json)
        if args.t4_governance_overlay_json is not None
        else None
    )

    try:
        actionable_statuses = normalize_actionable_statuses(args.actionable_statuses)
        issues_count = count_items(load_json(issues_path), path=issues_path, label="issues")
        milestones_count = count_items(
            load_json(milestones_path), path=milestones_path, label="milestones"
        )
        actionable_count = count_actionable_catalog_rows(
            load_json(catalog_path), path=catalog_path, actionable_statuses=actionable_statuses
        )
        open_blocker_count = 0
        if open_blockers_path is not None:
            open_blocker_count, _ = parse_open_blockers(
                load_json(open_blockers_path), path=open_blockers_path
            )
        t4_overlay_root = load_json(t4_overlay_path) if t4_overlay_path is not None else None
        t4_new_scope_publish, t4_source = parse_t4_overlay(
            t4_overlay_root, path=t4_overlay_path, cli_flag=bool(args.t4_new_scope_publish)
        )
    except ValueError as exc:
        print(str(exc), file=sys.stderr)
        return EXIT_HARD_FAILURE

    payload = build_payload(
        issues_path=issues_path,
        milestones_path=milestones_path,
        catalog_path=catalog_path,
        open_blockers_path=open_blockers_path,
        t4_overlay_path=t4_overlay_path,
        actionable_statuses=actionable_statuses,
        issues_count=issues_count,
        milestones_count=milestones_count,
        actionable_count=actionable_count,
        open_blocker_count=open_blocker_count,
        t4_new_scope_publish=t4_new_scope_publish,
        t4_source=t4_source,
        issues_max_age_seconds=args.issues_max_age_seconds,
        milestones_max_age_seconds=args.milestones_max_age_seconds,
    )
    output = (
        json.dumps(payload, indent=2) + "\n"
        if args.format == "json"
        else render_markdown(payload)
    )
    sys.stdout.write(normalize_newlines(output))
    return payload["exit_code"]

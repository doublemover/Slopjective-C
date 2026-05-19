#!/usr/bin/env python3
"""Review unchecked spec tasks and seed organized GitHub issues.

This script does three things in one deterministic pass:
1. Extract every unchecked checkbox task from spec/*.md files.
2. Generate a comprehensive review catalog with improved task definitions.
3. Optionally create GitHub issues for each task with lane/milestone metadata.
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

SCRIPT_ROOT = Path(__file__).resolve().parent
if str(SCRIPT_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPT_ROOT))

from remaining_task_extraction.seed_config import (
    ConfigError,
    load_seed_tooling_config,
    resolve_generated_on,
)
from remaining_task_extraction.seed_github import (
    GhError,
    ensure_labels,
    ensure_lane_milestones,
    fetch_milestone_map,
    seed_issues,
)
from remaining_task_extraction.seed_outputs import (
    render_catalog_markdown,
    serialize_catalog_json,
)
from remaining_task_extraction.seed_review import (
    RawTask,
    review_tasks,
)

ROOT = SCRIPT_ROOT.parents[0]
SPEC_ROOT = ROOT / "spec"

CHECKBOX_PATTERN = re.compile(r"^- \[ \]\s+(.*)$")
PLANNING_ISSUE_PATTERN = re.compile(r"issue_(\d+)_")
LEGACY_CONFORMANCE_PROFILE_SOURCE = (
    "docs/reference/legacy_spec_anchor_index.md#conformance-profile-checklist"
)
LEGACY_PLANNING_SOURCE = "docs/reference/legacy_spec_anchor_index.md"


def extract_raw_tasks() -> list[RawTask]:
    tasks: list[RawTask] = []
    for path in sorted(SPEC_ROOT.rglob("*.md")):
        rel = path.relative_to(ROOT).as_posix()
        if rel == "spec/CONFORMANCE_PROFILE_CHECKLIST.md":
            source_rel = LEGACY_CONFORMANCE_PROFILE_SOURCE
        elif PLANNING_ISSUE_PATTERN.search(rel):
            source_rel = LEGACY_PLANNING_SOURCE
        else:
            source_rel = rel
        lines = path.read_text(encoding="utf-8").splitlines()
        for idx, line in enumerate(lines, start=1):
            match = CHECKBOX_PATTERN.match(line)
            if not match:
                continue
            tasks.append(
                RawTask(
                    path=source_rel,
                    line=idx,
                    text=match.group(1).strip(),
                    origin_path=rel,
                )
            )
    return tasks


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        prog="seed_remaining_spec_tasks.py",
        description=(
            "Review all unchecked spec tasks, generate improved task definitions, "
            "and optionally seed GitHub issues with milestones/lanes."
        ),
    )
    parser.add_argument(
        "--config",
        type=Path,
        default=None,
        help=(
            "Optional path to tooling config JSON. When omitted, built-in defaults are used; "
            "built-in defaults match pre-HB-06 behavior."
        ),
    )
    parser.add_argument(
        "--repo",
        default=None,
        help=(
            "GitHub repository in owner/name format. "
            "Default resolves from config/built-in value."
        ),
    )
    parser.add_argument(
        "--catalog-md",
        type=Path,
        default=None,
        help=(
            "Path to write markdown review catalog. "
            "Default resolves from config/built-in value."
        ),
    )
    parser.add_argument(
        "--catalog-json",
        type=Path,
        default=None,
        help=(
            "Path to write JSON review catalog. "
            "Default resolves from config/built-in value."
        ),
    )
    parser.add_argument(
        "--create-issues",
        action="store_true",
        help="Create GitHub issues for reviewed tasks.",
    )
    parser.add_argument(
        "--limit",
        type=int,
        default=None,
        help="Optional limit for issue creation (for staged runs).",
    )
    parser.add_argument(
        "--sleep-seconds",
        type=float,
        default=None,
        help=(
            "Delay between issue creations to reduce rate-limit risk. "
            "Default resolves from config/built-in value."
        ),
    )
    parser.add_argument(
        "--generated-on",
        default=None,
        help=(
            "Date to embed in catalog outputs in YYYY-MM-DD format. "
            "When omitted, SOURCE_DATE_EPOCH is honored if present; otherwise today's date is used."
        ),
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    config = load_seed_tooling_config(args.config, root=ROOT)

    repo = args.repo if args.repo is not None else config.repo_default
    catalog_md = args.catalog_md if args.catalog_md is not None else config.catalog_md_default
    catalog_json = args.catalog_json if args.catalog_json is not None else config.catalog_json_default
    sleep_seconds = args.sleep_seconds if args.sleep_seconds is not None else config.sleep_seconds_default
    generated_on = resolve_generated_on(args.generated_on)

    raw_tasks = extract_raw_tasks()
    reviewed = review_tasks(raw_tasks, config)

    if len(reviewed) != config.expected_task_count:
        print(
            f"warning: expected {config.expected_task_count} tasks, found {len(reviewed)}",
            file=sys.stderr,
        )

    catalog_md.parent.mkdir(parents=True, exist_ok=True)
    catalog_json.parent.mkdir(parents=True, exist_ok=True)

    catalog_md.write_text(
        render_catalog_markdown(reviewed, config.lane_name, generated_on),
        encoding="utf-8",
    )
    catalog_json.write_text(
        serialize_catalog_json(reviewed, config.lane_name, generated_on),
        encoding="utf-8",
    )

    print(
        f"catalog_written tasks={len(reviewed)} md={catalog_md.as_posix()} json={catalog_json.as_posix()}"
    )

    if not args.create_issues:
        return 0

    ensure_labels(repo, config.label_defs)
    milestone_map = ensure_lane_milestones(
        repo,
        config.lane_milestone_titles,
        config.lane_milestone_due_on,
        config.lane_name,
    )
    milestone_map = fetch_milestone_map(repo)

    created, skipped = seed_issues(
        repo=repo,
        tasks=reviewed,
        milestone_map=milestone_map,
        lane_name=config.lane_name,
        sleep_seconds=sleep_seconds,
        limit=args.limit,
    )

    print(
        f"issue_seed_complete created={created} skipped_existing={skipped} total_reviewed={len(reviewed)}"
    )
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (ConfigError, GhError, RuntimeError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        raise SystemExit(2)

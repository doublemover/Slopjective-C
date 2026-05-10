"""Command-line orchestration for planning issue publication."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Sequence

from .constants import DEFAULT_PAYLOAD, DEFAULT_REPORT, DESCRIPTION, ROOT
from .contracts import PublicationError, collect_label_definitions
from .json_loading import load_publication_inputs, write_json
from .publication import apply_publication, dry_run_publication


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=DESCRIPTION)
    parser.add_argument("--payload", type=Path, default=DEFAULT_PAYLOAD, help="Checked-in planning GitHub payload JSON.")
    parser.add_argument("--report", type=Path, default=DEFAULT_REPORT, help="Durable publication mapping report JSON.")
    parser.add_argument("--repo", default="", help="Override repository owner/name. Defaults to payload.repository.")
    parser.add_argument("--apply", action="store_true", help="Create/update GitHub labels, milestones, and issues.")
    parser.add_argument("--update-existing", action="store_true", help="Patch mapped existing issues with current title/body/labels/milestone.")
    parser.add_argument("--limit", type=int, default=None, help="Maximum number of new issues to create in this run.")
    parser.add_argument("--sleep-seconds", type=float, default=1.0, help="Pause between GitHub write operations.")
    parser.add_argument("--write-report", action="store_true", help="Write the durable report during dry-run validation.")
    return parser.parse_args(argv)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    try:
        inputs = load_publication_inputs(args.payload, args.report, args.repo)

        if args.apply:
            result = apply_publication(
                inputs.repo,
                inputs.payload,
                inputs.existing_report,
                args.update_existing,
                args.limit,
                args.sleep_seconds,
            )
            write_json(inputs.report_path, result.report)
            print(
                "planning-issue-publisher: OK "
                f"repo={inputs.repo} labels={len(result.labels)} milestones={len(result.milestone_report)} "
                f"issues={len(result.issue_report)} dependencies={len(result.report['dependencies'])} "
                f"report={inputs.report_path.relative_to(ROOT).as_posix()}"
            )
            return 0

        result = dry_run_publication(inputs.repo, inputs.payload, inputs.existing_report)
        if args.write_report:
            write_json(inputs.report_path, result.report)
        print(
            "planning-issue-publisher: OK dry-run "
            f"repo={inputs.repo} milestones={len(inputs.payload['milestones'])} issues={len(inputs.payload['issues'])} "
            f"labels={len(collect_label_definitions(inputs.payload))} dependencies={len(result.report['dependencies'])} "
            f"unresolved_dependencies={len(result.unresolved_dependencies)}"
        )
        return 0
    except PublicationError as exc:
        print(f"planning-issue-publisher: ERROR {exc}", file=sys.stderr)
        return 1

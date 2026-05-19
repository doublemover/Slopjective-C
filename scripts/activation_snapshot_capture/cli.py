"""Command-line orchestration for activation snapshot capture."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Sequence

from activation_snapshot_capture.command_capture import (
    collect_open_issues,
    collect_open_milestones,
)
from activation_snapshot_capture.constants import (
    ISSUES_SOURCE,
    MILESTONES_SOURCE,
    PROGRAM_NAME,
    ROOT,
    SCRIPT_NAME,
)
from activation_snapshot_capture.models import (
    GhClientFactory,
    SnapshotCaptureResult,
    SnapshotError,
)
from activation_snapshot_capture.paths import resolve_snapshot_outputs
from activation_snapshot_capture.snapshot_shaping import (
    build_snapshot,
    resolve_generated_at_utc,
)
from lib.gh_client import GhClient, GhClientError
from objc3c_tooling.json_io import render_json
from objc3c_tooling.json_io import write_text_file as write_text
from objc3c_tooling.paths import display_path


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog=SCRIPT_NAME,
        description=(
            "Capture deterministic JSON snapshots of open issues and open milestones "
            "for activation preflight workflows."
        ),
    )
    parser.add_argument(
        "--issues-output",
        type=Path,
        required=True,
        help="Output path for open-issues snapshot JSON.",
    )
    parser.add_argument(
        "--milestones-output",
        type=Path,
        required=True,
        help="Output path for open-milestones snapshot JSON.",
    )
    parser.add_argument(
        "--generated-at-utc",
        help=(
            "Optional explicit generated-at timestamp (RFC3339 UTC, "
            "YYYY-MM-DDTHH:MM:SSZ). If omitted, SOURCE_DATE_EPOCH is used when set; "
            "otherwise current UTC time is used."
        ),
    )
    return parser


def capture_snapshots(
    *,
    issues_output: Path,
    milestones_output: Path,
    generated_at_utc: str | None,
    client_cls: GhClientFactory = GhClient,
) -> SnapshotCaptureResult:
    output_paths = resolve_snapshot_outputs(
        issues_output=issues_output,
        milestones_output=milestones_output,
    )
    resolved_generated_at_utc = resolve_generated_at_utc(generated_at_utc)
    client = client_cls(root=ROOT)

    issue_items = collect_open_issues(client)
    milestone_items = collect_open_milestones(client)

    issues_snapshot = build_snapshot(
        generated_at_utc=resolved_generated_at_utc,
        source=ISSUES_SOURCE,
        items=issue_items,
    )
    milestones_snapshot = build_snapshot(
        generated_at_utc=resolved_generated_at_utc,
        source=MILESTONES_SOURCE,
        items=milestone_items,
    )

    write_text(output_paths.issues_output, render_json(issues_snapshot))
    write_text(output_paths.milestones_output, render_json(milestones_snapshot))

    return SnapshotCaptureResult(
        issues_snapshot=issues_snapshot,
        milestones_snapshot=milestones_snapshot,
        output_paths=output_paths,
    )


def main(
    argv: Sequence[str] | None = None,
    *,
    client_cls: GhClientFactory = GhClient,
) -> int:
    args = build_parser().parse_args(argv)

    try:
        result = capture_snapshots(
            issues_output=args.issues_output,
            milestones_output=args.milestones_output,
            generated_at_utc=args.generated_at_utc,
            client_cls=client_cls,
        )
    except (GhClientError, SnapshotError, OSError, ValueError) as exc:
        print(f"{PROGRAM_NAME}: {exc}", file=sys.stderr)
        return 1

    print(
        f"{PROGRAM_NAME}: OK "
        f"(issues={result.issues_snapshot['count']}, "
        f"milestones={result.milestones_snapshot['count']}, "
        f"issues_output={display_path(result.output_paths.issues_output)}, "
        f"milestones_output={display_path(result.output_paths.milestones_output)})"
    )
    return 0


__all__ = ["build_parser", "capture_snapshots", "main"]

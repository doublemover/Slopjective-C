"""Planning-publication drift audit orchestration."""

from __future__ import annotations

import json
import sys
from typing import Any, Sequence

from objc3c_tooling.json_io import load_json_any as load_json, write_json_file as write_json

from .cli import parse_args
from .errors import DriftAuditError
from .live_github import compare_live_github
from .paths import ROOT, repo_path
from .shared import (
    PlanningPublicationDriftInputs,
    PlanningPublicationDriftPaths,
    append_failure,
    build_drift_report,
    compare_publication_references,
    compare_report_contract,
    publication_snapshot,
    publisher,
    render_markdown_report,
    repo_relative_path,
    require_dict,
    success_status_line,
)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    try:
        payload_path = repo_path(args.payload)
        publication_report_path = repo_path(args.publication_report)
        markdown_path = repo_path(args.markdown_report)
        output_path = repo_path(args.output)
        payload, report = load_validated_inputs(payload_path, publication_report_path)

        if args.update_payload_published:
            payload["published"] = publication_snapshot(report)
            write_json(payload_path, payload)
        if args.update_markdown_report:
            markdown_path.parent.mkdir(parents=True, exist_ok=True)
            markdown_path.write_text(render_markdown_report(report), encoding="utf-8", newline="\n")

        failures: list[dict[str, Any]] = []
        compare_report_contract(
            payload=payload,
            report=report,
            expected_dependencies=publisher.build_dependency_report(payload, report["issues"]),
            failures=failures,
        )
        compare_publication_references(
            payload=payload,
            report=report,
            markdown_text=markdown_path.read_text(encoding="utf-8")
            if markdown_path.is_file()
            else None,
            markdown_display_path=str(markdown_path),
            markdown_report_path=repo_relative_path(markdown_path, ROOT),
            failures=failures,
        )
        live_summary = {"enabled": False, "checked_milestones": 0, "checked_issues": 0}
        if args.live:
            live_summary = compare_live_github(payload, report, failures, args.limit_live)
        drift_report = build_drift_payload(payload_path, publication_report_path, markdown_path, payload, report, failures, live_summary)
        rendered = json.dumps(drift_report, indent=2) + "\n"
        write_or_check_report(output_path, rendered, failures, write=args.write, check=args.check)

        if failures:
            print(f"planning-publication-drift-audit: FAIL failures={len(failures)}", file=sys.stderr)
            for failure in failures[:10]:
                print(f"- {failure['code']}: {failure['detail']}", file=sys.stderr)
            return 1
        print(success_status_line(payload, live_summary))
        return 0
    except (DriftAuditError, publisher.PublicationError, FileNotFoundError, json.JSONDecodeError) as exc:
        print(f"planning-publication-drift-audit: ERROR {exc}", file=sys.stderr)
        return 1


def load_validated_inputs(payload_path, publication_report_path) -> tuple[dict[str, Any], dict[str, Any]]:
    publisher.assert_not_tmp_source(payload_path)
    payload = require_dict(load_json(payload_path), "payload")
    report = require_dict(load_json(publication_report_path), "publication report")
    publisher.validate_payload(payload)
    publisher.validate_existing_report(report, payload)
    return payload, report


def build_drift_payload(payload_path, publication_report_path, markdown_path, payload, report, failures, live_summary):
    return build_drift_report(
        PlanningPublicationDriftInputs(
            paths=PlanningPublicationDriftPaths(
                payload_path=repo_relative_path(payload_path, ROOT),
                publication_report_path=repo_relative_path(publication_report_path, ROOT),
                markdown_report_path=repo_relative_path(markdown_path, ROOT),
            ),
            payload=payload,
            report=report,
            failures=failures,
            live_summary=live_summary,
        )
    )


def write_or_check_report(output_path, rendered: str, failures: list[dict[str, Any]], *, write: bool, check: bool) -> None:
    if write:
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(rendered, encoding="utf-8", newline="\n")
    if check:
        if not output_path.is_file():
            append_failure(failures, "drift-report-missing", f"missing drift report: {output_path}")
        elif output_path.read_text(encoding="utf-8") != rendered:
            append_failure(failures, "drift-report-stale", "checked-in publication drift report is stale")

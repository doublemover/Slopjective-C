from __future__ import annotations

import argparse
from pathlib import Path

from .config import (
    DEFAULT_ALLOWLIST,
    DEFAULT_JSON_REPORT,
    DEFAULT_TEXT_REPORT,
)
from .report_writer import write_reports
from .scanner import build_report


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Run the hard-cutover source hygiene gate.")
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[2])
    parser.add_argument("--json", type=Path, default=DEFAULT_JSON_REPORT)
    parser.add_argument("--text", type=Path, default=DEFAULT_TEXT_REPORT)
    parser.add_argument("--allowlist", type=Path)
    parser.add_argument("--no-default-allowlist", action="store_true")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    root = args.root.resolve()
    allowlist_path = args.allowlist
    if allowlist_path is None and not args.no_default_allowlist:
        candidate = root / DEFAULT_ALLOWLIST
        if candidate.is_file():
            allowlist_path = candidate
    report = build_report(root=root, allowlist_path=allowlist_path)
    json_path = args.json if args.json.is_absolute() else root / args.json
    text_path = args.text if args.text.is_absolute() else root / args.text
    write_reports(report, json_path, text_path)
    print(f"source_hygiene_json: {json_path.relative_to(root).as_posix()}")
    print(f"source_hygiene_text: {text_path.relative_to(root).as_posix()}")
    print(f"source_hygiene_ok: {str(report['ok']).lower()}")
    print(f"active_findings: {report['stats']['active_finding_count']}")
    print(f"tracked_generated_reports: {report['stats']['tracked_generated_report_count']}")
    return 0 if report["ok"] else 1

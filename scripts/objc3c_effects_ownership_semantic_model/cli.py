from __future__ import annotations

import argparse

from objc3c_tooling.cli import add_check_argument
from objc3c_tooling.reports import expected_json_report

from objc3c_effects_ownership_semantic_model.paths import JSON_OUT
from objc3c_effects_ownership_semantic_model.paths import MD_OUT
from objc3c_effects_ownership_semantic_model.paths import rel
from objc3c_effects_ownership_semantic_model.rendering import render_markdown
from objc3c_effects_ownership_semantic_model.rendering import write_outputs
from objc3c_effects_ownership_semantic_model.semantic_model import build_summary


def main() -> int:
    parser = argparse.ArgumentParser()
    add_check_argument(parser)
    args = parser.parse_args()
    summary = build_summary()
    expected_json = expected_json_report(summary)
    expected_md = render_markdown(summary)
    if args.check:
        if not JSON_OUT.is_file() or JSON_OUT.read_text(encoding="utf-8") != expected_json:
            raise SystemExit(f"{rel(JSON_OUT)} is stale; run this script without --check")
        if not MD_OUT.is_file() or MD_OUT.read_text(encoding="utf-8") != expected_md:
            raise SystemExit(f"{rel(MD_OUT)} is stale; run this script without --check")
        if summary["status"] != "PASS":
            raise SystemExit("effects ownership semantic model summary failed")
        print(f"status: {summary['status']}")
        print(f"summary_path: {rel(JSON_OUT)}")
        return 0
    write_outputs(summary)
    print(f"wrote: {rel(JSON_OUT)}")
    print(f"wrote: {rel(MD_OUT)}")
    print(f"status: {summary['status']}")
    return 0 if summary["status"] == "PASS" else 1

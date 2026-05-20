from __future__ import annotations

import argparse

from objc3c_tooling.cli import add_check_argument
from objc3c_tooling.reports import expected_json_report

from objc3c_effects_ownership_semantic_model.paths import JSON_OUT
from objc3c_effects_ownership_semantic_model.paths import MD_OUT
from objc3c_effects_ownership_semantic_model.paths import REPORT_DIR
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
        REPORT_DIR.mkdir(parents=True, exist_ok=True)
        JSON_OUT.write_text(expected_json, encoding="utf-8")
        MD_OUT.write_text(expected_md, encoding="utf-8")
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

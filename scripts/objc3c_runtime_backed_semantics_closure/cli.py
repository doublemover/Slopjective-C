from __future__ import annotations

import argparse
import json

from objc3c_runtime_backed_semantics_closure.paths import JSON_OUT
from objc3c_runtime_backed_semantics_closure.paths import MD_OUT
from objc3c_runtime_backed_semantics_closure.paths import REPORT_DIR
from objc3c_runtime_backed_semantics_closure.paths import rel
from objc3c_runtime_backed_semantics_closure.reporting import render_markdown
from objc3c_runtime_backed_semantics_closure.summary import build_summary


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--check",
        action="store_true",
        help="validate the computed summary and refresh temp reports",
    )
    args = parser.parse_args()

    summary = build_summary()
    json_text = json.dumps(summary, indent=2, sort_keys=True) + "\n"
    md_text = render_markdown(summary)

    if args.check:
        REPORT_DIR.mkdir(parents=True, exist_ok=True)
        JSON_OUT.write_text(json_text, encoding="utf-8")
        MD_OUT.write_text(md_text, encoding="utf-8")
        print(f"status: {summary['status']}")
        print(f"summary_path: {rel(JSON_OUT)}")
        return 0 if summary["status"] == "PASS" else 1

    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json_text, encoding="utf-8")
    MD_OUT.write_text(md_text, encoding="utf-8")
    print(f"status: {summary['status']}")
    print(f"wrote {rel(JSON_OUT)}")
    print(f"wrote {rel(MD_OUT)}")
    return 0 if summary["status"] == "PASS" else 1

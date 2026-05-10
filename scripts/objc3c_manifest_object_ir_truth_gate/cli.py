from __future__ import annotations

import argparse
import json

from .manifest_loading import read
from .paths import JSON_OUT, MD_OUT, REPORT_DIR, rel
from .rendering import render_markdown
from .summary import build_summary


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true", help="fail if committed report files are stale")
    args = parser.parse_args()

    summary = build_summary()
    json_text = json.dumps(summary, indent=2, sort_keys=True) + "\n"
    md_text = render_markdown(summary)

    if args.check:
        stale = [
            rel(path)
            for path, text in ((JSON_OUT, json_text), (MD_OUT, md_text))
            if not path.is_file() or read(path) != text
        ]
        if stale:
            print("status: FAIL")
            print("stale reports:")
            for path in stale:
                print(f"- {path}")
            return 1
        print(f"status: {summary['status']}")
        return 0 if summary["status"] == "PASS" else 1

    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json_text, encoding="utf-8")
    MD_OUT.write_text(md_text, encoding="utf-8")
    print(f"status: {summary['status']}")
    print(f"wrote {rel(JSON_OUT)}")
    print(f"wrote {rel(MD_OUT)}")
    return 0 if summary["status"] == "PASS" else 1

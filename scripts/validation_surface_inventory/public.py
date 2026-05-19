"""Public facade helpers for building and publishing the inventory report."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.json_io import write_json_file

from .model import build_report
from .paths import JSON_OUT, MD_OUT, REPORT_DIR
from .rendering import render_markdown


def write_report(*, report_dir: Path = REPORT_DIR, json_out: Path = JSON_OUT, md_out: Path = MD_OUT) -> dict[str, object]:
    report_dir.mkdir(parents=True, exist_ok=True)
    json_out.parent.mkdir(parents=True, exist_ok=True)
    md_out.parent.mkdir(parents=True, exist_ok=True)
    report = build_report()
    write_json_file(json_out, report)
    md_out.write_text(render_markdown(report), encoding="utf-8")
    return report

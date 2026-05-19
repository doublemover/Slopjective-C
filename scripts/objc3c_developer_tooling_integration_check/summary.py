"""Summary creation for developer-tooling integration."""

from __future__ import annotations

from typing import Any

from objc3c_tooling.json_io import write_json_file

from .constants import ROOT, SUMMARY_OUT
from .reports import report_path_payload


def write_summary(steps: list[dict[str, Any]], failures: list[str]) -> None:
    payload = {
        "mode": "objc3c-developer-tooling-integration-v1",
        "ok": not failures,
        "failures": failures,
        "steps": steps,
        "reports": report_path_payload(),
    }
    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_OUT, payload)
    print(f"summary_path: {SUMMARY_OUT.relative_to(ROOT).as_posix()}")

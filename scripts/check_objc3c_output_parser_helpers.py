#!/usr/bin/env python3
"""Fast checks for shared public workflow and probe output parsers."""

from __future__ import annotations

import subprocess
from pathlib import Path
from typing import Sequence

from objc3c_tooling.public_workflow_output import (
    case_ids_from_acceptance,
    extract_line_value,
    extract_output_value,
    extract_public_workflow_report_paths,
    extract_report_paths,
    normalize_newlines,
)
from objc3c_tooling.probe_output import parse_json_output, parse_key_value_output


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def completed(stdout: str, *, stderr: str = "", returncode: int = 0) -> subprocess.CompletedProcess[str]:
    return subprocess.CompletedProcess(["probe"], returncode, stdout=stdout, stderr=stderr)


def main(argv: Sequence[str] | None = None) -> int:
    _ = argv
    expect(normalize_newlines("a\r\nb\rc") == "a\nb\nc", "normalize_newlines should normalize CRLF and CR")
    expect(extract_line_value("noise\nanswer= 42\n", "answer=") == "42", "extract_line_value should strip values")
    expect(extract_output_value("summary_path: tmp/report.json\n", "summary_path") == "tmp/report.json", "extract_output_value should parse colon keys")
    expect(extract_output_value("", "summary_path") is None, "missing output values should return None")

    report_stdout = "\n".join(
        [
            "summary_path: tmp/reports/a.json",
            "runtime-acceptance: PASS (C:\\repo\\runtime.json)",
            "public-workflow-report: tmp/reports/workflow.json",
        ]
    )
    paths = extract_report_paths(report_stdout)
    expect(paths[0] == "tmp/reports/a.json", "summary_path should be captured")
    expect(paths[-1] == "tmp/reports/workflow.json", "public workflow report should be captured")
    public_paths = extract_public_workflow_report_paths("workspace_path: workspace/demo\nout_dir: tmp/out\n")
    expect(public_paths == ["workspace/demo", "tmp/out"], "public workflow paths should include workspace/out_dir")

    payload = parse_json_output(completed('{"ok": true, "count": 2}'), "json-smoke")
    expect(payload == {"ok": True, "count": 2}, "parse_json_output should parse objects")
    kv = parse_key_value_output(completed("count=2\nname=value\nnegative=-3\n"), "kv-smoke", required_keys=("count",))
    expect(kv == {"count": 2, "name": "value", "negative": -3}, "parse_key_value_output should coerce integers")

    try:
        parse_key_value_output(completed("a=1\na=2\n"), "dup-smoke")
    except RuntimeError as exc:
        expect("duplicate key" in str(exc), "duplicate keys should be diagnosed")
    else:
        raise RuntimeError("duplicate keys should fail")

    try:
        parse_json_output(completed("not-json"), "bad-json")
    except RuntimeError as exc:
        expect("invalid JSON" in str(exc), "invalid JSON should be diagnosed")
    else:
        raise RuntimeError("invalid JSON should fail")

    cases = case_ids_from_acceptance({"cases": [{"case_id": "a"}, {"case_id": ""}, {"case_id": "b"}]})
    expect(cases == {"a", "b"}, "case_ids_from_acceptance should collect non-empty string ids")

    # Keep Path imported in this smoke so py_compile catches accidental path-parser dependency regressions.
    expect(Path("tmp").as_posix() == "tmp", "path smoke")

    print("objc3c-output-parser-helpers: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

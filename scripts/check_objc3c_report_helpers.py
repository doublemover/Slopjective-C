#!/usr/bin/env python3
"""Fast coverage for shared report and Markdown primitives."""

from __future__ import annotations

import tempfile
from pathlib import Path

from objc3c_tooling.json_io import load_json_object
from objc3c_tooling.reports import escape_markdown_table_cell
from objc3c_tooling.reports import expected_json_report
from objc3c_tooling.reports import markdown_bullet
from objc3c_tooling.reports import markdown_bool
from objc3c_tooling.reports import markdown_heading
from objc3c_tooling.reports import markdown_optional_code
from objc3c_tooling.reports import markdown_table
from objc3c_tooling.reports import write_report_outputs


def test_markdown_primitives() -> None:
    assert markdown_heading("Title") == "# Title"
    assert markdown_heading("Section", level=2) == "## Section"
    assert markdown_bool(True) == "true"
    assert markdown_bool(False) == "false"
    assert markdown_optional_code(None) == "_none_"
    assert markdown_optional_code("abc") == "`abc`"
    assert markdown_bullet("Path", None) == "- Path: _none_"
    assert markdown_bullet("Path", "a/b", code=False) == "- Path: a/b"


def test_table_escaping() -> None:
    assert escape_markdown_table_cell("a|b") == "a\\|b"
    assert escape_markdown_table_cell("a\nb") == "a<br>b"
    assert markdown_table(["A|B", "C"], [["x|y", "z"]]) == [
        "| A\\|B | C |",
        "| --- | --- |",
        "| x\\|y | z |",
    ]
    try:
        markdown_table(["A", "B"], [["only-one"]])
    except ValueError as exc:
        assert "expected 2" in str(exc)
    else:
        raise AssertionError("markdown_table accepted a row with the wrong column count")


def test_deterministic_outputs() -> None:
    with tempfile.TemporaryDirectory() as tmp:
        root = Path(tmp)
        summary = {"b": 2, "a": 1}
        json_path = root / "nested" / "summary.json"
        markdown_path = root / "nested" / "summary.md"
        write_report_outputs(
            summary=summary,
            json_path=json_path,
            markdown_path=markdown_path,
            markdown="# Report\n",
        )
        assert json_path.read_text(encoding="utf-8") == expected_json_report(summary)
        assert markdown_path.read_text(encoding="utf-8") == "# Report\n"
        assert load_json_object(json_path) == {"a": 1, "b": 2}


def main() -> int:
    test_markdown_primitives()
    test_table_escaping()
    test_deterministic_outputs()
    print("objc3c-report-helpers: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

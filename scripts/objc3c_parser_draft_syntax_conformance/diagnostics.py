from __future__ import annotations

import re
from pathlib import Path
from typing import Any

from objc3c_parser_draft_syntax_conformance.paths import read

HEADER_RE = re.compile(r"(?mi)^\s*//\s*Expected diagnostic code\(s\):\s*(.+?)\s*$")
CODE_RE = re.compile(r"O3[A-Z]\d{3}")


def find_parser_manifest(node: Any) -> dict[str, Any] | None:
    if isinstance(node, dict):
        if {
            "draft_syntax_surface_count",
            "draft_syntax_surface_fingerprint",
            "draft_syntax_surface_handoff_key",
            "draft_syntax_surface_deterministic",
        }.issubset(node.keys()):
            return node
        for value in node.values():
            found = find_parser_manifest(value)
            if found is not None:
                return found
    if isinstance(node, list):
        for value in node:
            found = find_parser_manifest(value)
            if found is not None:
                return found
    return None


def parse_replay_key(key: str) -> dict[str, int]:
    fields: dict[str, int] = {}
    for part in key.split(";"):
        if "=" not in part:
            continue
        name, value = part.split("=", 1)
        try:
            fields[name] = int(value)
        except ValueError:
            continue
    return fields


def expected_code_header(path: Path) -> list[str]:
    text = read(path)
    match = HEADER_RE.search(text)
    if not match:
        return []
    return [code.upper() for code in CODE_RE.findall(match.group(1))]


def diagnostic_matches(diagnostics: list[dict[str, Any]], expected: dict[str, Any]) -> bool:
    return any(
        diag.get("code") == expected["code"]
        and int(diag.get("line", -1)) == int(expected["line"])
        and int(diag.get("column", -1)) == int(expected["column"])
        for diag in diagnostics
    )

#!/usr/bin/env python3
"""Format objc3c source on the supported preview subset."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
REPORT_ROOT = ROOT / "tmp" / "reports" / "developer-tooling" / "formatter"
UNSUPPORTED_MARKERS = ("\"", "@interface", "@implementation", "@protocol", "^", "await ", "actor ", "macro ")
PREVIEW_SUBSET_ID = "objc3c.format.preview.canonical-structured-subset.v1"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source")
    return parser.parse_args()


def repo_rel(path: Path) -> str:
    path = path.resolve()
    try:
        return path.relative_to(ROOT).as_posix()
    except ValueError:
        return path.as_posix()


def resolve_source(source_text: str) -> tuple[Path, str]:
    candidate = Path(source_text)
    resolved = candidate if candidate.is_absolute() else (ROOT / candidate)
    resolved = resolved.resolve()
    if not resolved.is_file():
        raise FileNotFoundError(f"source not found: {source_text}")
    return resolved, repo_rel(resolved)


def slugify(display_path: str) -> str:
    digest = hashlib.sha256(display_path.encode("utf-8")).hexdigest()[:12]
    stem = Path(display_path).stem.lower()
    safe = "".join(ch if ch.isalnum() else "-" for ch in stem).strip("-")
    if not safe:
        safe = "source"
    return f"{safe}-{digest}"


def can_format(text: str) -> tuple[bool, str]:
    for marker in UNSUPPORTED_MARKERS:
        if marker in text:
            return False, f"unsupported marker for preview formatter: {marker}"
    return True, ""


def normalize_statement(line: str) -> str:
    line = re.sub(r"\s+", " ", line.strip())
    line = re.sub(r"\s*:\s*", ": ", line)
    line = re.sub(r"\s*,\s*", ", ", line)
    line = re.sub(r"\s*\+\s*", " + ", line)
    line = re.sub(r"\s*=\s*", " = ", line)
    line = re.sub(r"\b(fn\s+[A-Za-z_][A-Za-z0-9_]*)\s+\(", r"\1(", line)
    line = re.sub(r"\(\s+", "(", line)
    line = re.sub(r"\s+\)", ")", line)
    line = re.sub(r"^if\(", "if (", line)
    line = re.sub(r"^while\(", "while (", line)
    line = re.sub(r"\)\s*\{", ") {", line)
    line = re.sub(r"\}\s*else\s*\{", "} else {", line)
    line = re.sub(r"\s*;\s*$", ";", line)
    line = re.sub(r" {2,}", " ", line)
    return line


def format_source_text(text: str) -> tuple[str, bool, str]:
    supported, reason = can_format(text)
    if not supported:
        return text if text.endswith("\n") else text + "\n", False, reason

    formatted_lines: list[str] = []
    indent = 0
    for raw_line in text.splitlines():
        stripped = raw_line.strip()
        if not stripped:
            formatted_lines.append("")
            continue
        if stripped.startswith("//"):
            formatted_lines.append(("  " * indent) + stripped)
            continue
        if stripped.startswith("}"):
            indent = max(indent - 1, 0)
        normalized = normalize_statement(stripped)
        formatted_lines.append(("  " * indent) + normalized)
        if normalized.endswith("{"):
            indent += 1
    return "\n".join(formatted_lines).rstrip() + "\n", True, ""


def build_format_summary(
    source_display: str,
    formatted_output_path: str,
    changed: bool,
    supported: bool,
    reason: str,
    *,
    source_line_count: int,
    formatted_line_count: int,
) -> dict[str, object]:
    return {
        "contract_id": "objc3c.developer.tooling.formatter.surface.v1",
        "source_path": source_display,
        "supported": supported,
        "support_class": "preview" if supported else "fail-closed",
        "preview_subset_id": PREVIEW_SUBSET_ID if supported else None,
        "formatted_output_path": formatted_output_path,
        "changed": changed,
        "source_line_count": source_line_count,
        "formatted_line_count": formatted_line_count,
        "fallback_reason": reason,
    }


def build_format_summary_for_source(
    source_display: str,
    source_text: str,
    formatted_output_path: str,
) -> tuple[str, dict[str, object]]:
    formatted_text, supported, reason = format_source_text(source_text)
    summary = build_format_summary(
        source_display,
        formatted_output_path,
        formatted_text != source_text,
        supported,
        reason,
        source_line_count=len(source_text.splitlines()),
        formatted_line_count=len(formatted_text.splitlines()),
    )
    return formatted_text, summary


def main() -> int:
    args = parse_args()
    source_path, source_display = resolve_source(args.source)
    slug = slugify(source_display)
    report_dir = REPORT_ROOT / slug
    report_dir.mkdir(parents=True, exist_ok=True)
    formatted_output_path = report_dir / "formatted.objc3"
    summary_path = report_dir / "formatter-output.json"

    source_text = source_path.read_text(encoding="utf-8")
    formatted_text, summary = build_format_summary_for_source(
        source_display,
        source_text,
        repo_rel(formatted_output_path),
    )
    formatted_output_path.write_text(formatted_text, encoding="utf-8")
    summary_path.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(summary_path)}")
    print(f"formatted_output_path: {repo_rel(formatted_output_path)}")
    print(json.dumps(summary, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

#!/usr/bin/env python3

from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SPEC_DIR = ROOT / "spec"
TOC_PATH = SPEC_DIR / "TABLE_OF_CONTENTS.md"

DOC_PREFIX = {
    "TABLE_OF_CONTENTS.md": "toc",
    "INTRODUCTION.md": "intro",
    "DECISIONS_LOG.md": "decisions",
    "ATTRIBUTE_AND_SYNTAX_CATALOG.md": "b",
    "LOWERING_AND_RUNTIME_CONTRACTS.md": "c",
    "MODULE_METADATA_AND_ABI_TABLES.md": "d",
    "CONFORMANCE_PROFILE_CHECKLIST.md": "e",
}


def read_text_raw(path: Path) -> str:
    with path.open("r", encoding="utf-8", newline="") as handle:
        return handle.read()


def write_text_raw(path: Path, text: str) -> None:
    with path.open("w", encoding="utf-8", newline="") as handle:
        handle.write(text)


def parse_toc_files() -> list[Path]:
    text = read_text_raw(TOC_PATH)
    names = re.findall(r"(?m)^-\s+.*?([A-Za-z0-9_.-]+\.md)", text)
    if not names:
        raise RuntimeError("No .md entries found in TABLE_OF_CONTENTS.md")
    return [SPEC_DIR / name for name in names]


def part_prefix(name: str) -> str | None:
    match = re.match(r"^PART_(\d+)_", name)
    if not match:
        return None
    return f"part-{match.group(1)}"


def link_section(match: re.Match[str]) -> str:
    number = match.group(1)
    parts = number.split(".")
    part_num = parts[0]
    rest = "-".join(parts[1:])
    anchor = f"part-{part_num}" + (f"-{rest}" if rest else "")
    return f"[§{number}](#{anchor})"


def link_part(match: re.Match[str]) -> str:
    part_num = match.group(1)
    return f"[Part {part_num}](#part-{part_num})"


def link_letter_section(match: re.Match[str]) -> str:
    letter = match.group(1).lower()
    numbers = match.group(2).replace(".", "-")
    return f"[{match.group(1)}.{match.group(2)}](#{letter}-{numbers})"


def link_decision(match: re.Match[str]) -> str:
    digits = match.group(1)
    return f"[Decision D-{digits}](#decisions-d-{digits})"


def link_decision_only(match: re.Match[str]) -> str:
    digits = match.group(1)
    return f"[D-{digits}](#decisions-d-{digits})"


def link_table(match: re.Match[str]) -> str:
    prefix = match.group(1) or ""
    table = match.group(2)
    target = {"A": "d-3-1", "B": "d-3-2", "C": "d-3-3"}[table]
    label = f"{prefix}Table {table}".strip()
    return f"[{label}](#{target})"


def protect(text: str, pattern: str, token: str) -> tuple[str, list[str]]:
    items: list[str] = []

    def repl(match: re.Match[str]) -> str:
        items.append(match.group(0))
        return f"@@{token}{len(items) - 1}@@"

    return re.sub(pattern, repl, text), items


def restore(text: str, token: str, items: list[str]) -> str:
    for index, value in enumerate(items):
        text = text.replace(f"@@{token}{index}@@", value)
    return text


def linkify_line(line: str, file_map: dict[str, str]) -> str:
    if re.match(r"^#{1,6}\s", line):
        return re.sub(r"\[([^\]]+)\]\([^)]+\)", r"\1", line)

    protected, code_spans = protect(line, r"`[^`]*`", "CODE")
    protected, links = protect(protected, r"\[[^\]]+\]\([^)]+\)", "LINK")

    text = protected

    text = text.replace("B/C/D", "[B](#b)/[C](#c)/[D](#d)")
    text = text.replace("B/C", "[B](#b)/[C](#c)")

    for filename, anchor in file_map.items():
        text = text.replace(f"**{filename}**", f"**[{filename}](#{anchor})**")
        text = re.sub(rf"(?<!\[){re.escape(filename)}(?!\])", f"[{filename}](#{anchor})", text)

    text = re.sub(r"\bPart\s+(\d{1,2})\b", link_part, text)

    def link_parts(match: re.Match[str]) -> str:
        numbers = match.group(1).split("/")
        linked = "/".join(f"[{num}](#part-{num})" for num in numbers)
        return f"Parts {linked}"

    text = re.sub(r"\bParts\s+(\d+(?:/\d+)+)\b", link_parts, text)

    text = re.sub(r"§\s*(\d+(?:\.\d+)*)", link_section, text)
    text = re.sub(r"\b([BCDE])\.(\d+(?:\.\d+)*)\b", link_letter_section, text)
    text = re.sub(r"\bDecision\s+D[\u2011\u2013\u2014\u2012-](\d{3})\b", link_decision, text)
    text = re.sub(r"\bD[\u2011\u2013\u2014\u2012-](\d{3})\b", link_decision_only, text)
    text = re.sub(r"\b(D\s+)?Table\s+([ABC])\b", link_table, text)

    text = re.sub(r"\*\*([BCDE])\*\*", lambda m: f"**[{m.group(1)}](#{m.group(1).lower()})**", text)
    text = re.sub(r"\s\((B|C|D)\)", lambda m: f" ([{m.group(1)}](#{m.group(1).lower()}))", text)

    text = re.sub(r"\[\[([^\]]+)\]\((#[^)]+)\)\]\(\2\)", r"[\1](\2)", text)

    text = restore(text, "LINK", links)
    text = re.sub(r"\[Decision\s+\[([^\]]+)\]\((#[^)]+)\)\]\(\2\)", r"[Decision \1](\2)", text)
    text = re.sub(r"Decision\s+\[([^\]]+)\]\((#[^)]+)\)", r"[Decision \1](\2)", text)
    text = re.sub(r"\[\[([^\]]+)\]\((#[^)]+)\)\]\(\2\)", r"[\1](\2)", text)
    text = restore(text, "CODE", code_spans)
    return text


def main() -> None:
    files = parse_toc_files()

    file_map: dict[str, str] = {}
    for path in files:
        name = path.name
        prefix = DOC_PREFIX.get(name)
        if prefix is None:
            prefix = part_prefix(name)
        if prefix is None:
            raise RuntimeError(f"No prefix mapping for {name}")
        file_map[name] = prefix

    for path in files:
        text = read_text_raw(path)
        lines = text.splitlines(keepends=True)
        in_fence = False
        updated_lines: list[str] = []

        for line in lines:
            fence_match = re.match(r"^\s*(```|~~~)", line)
            if fence_match:
                in_fence = not in_fence

            if in_fence:
                updated_lines.append(line)
                continue

            updated_lines.append(linkify_line(line, file_map))

        write_text_raw(path, "".join(updated_lines))


if __name__ == "__main__":
    main()

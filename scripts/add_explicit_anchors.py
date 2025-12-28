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


def slugify(text: str) -> str:
    lowered = text.lower()
    lowered = lowered.replace("’", "'")
    lowered = re.sub(r"[^a-z0-9]+", "-", lowered)
    return lowered.strip("-")


def anchor_for_heading(prefix: str, heading: str, first_heading: bool) -> str:
    if first_heading:
        return prefix

    if prefix == "decisions":
        decision = re.match(r"^(D[\u2011\u2013\u2014\u2012-]\d{3})\b", heading)
        if decision:
            normalized = decision.group(1).lower().replace("\u2011", "-").replace("\u2013", "-").replace("\u2014", "-").replace("\u2012", "-")
            return f"decisions-{normalized}"

    letter_section = re.match(r"^([BCDE])\.(\d+(?:\.\d+)*)\b", heading)
    if letter_section:
        letter = letter_section.group(1).lower()
        numbers = letter_section.group(2).replace(".", "-")
        return f"{letter}-{numbers}"

    numbered = re.match(r"^(\d+(?:\.\d+)*)\b", heading)
    if numbered:
        numbers = numbered.group(1).split(".")
        if prefix.startswith("part-"):
            part_num = prefix.split("-")[1]
            if numbers[0] == part_num:
                numbers = numbers[1:]
            if not numbers:
                return prefix
        return f"{prefix}-" + "-".join(numbers)

    return f"{prefix}-{slugify(heading)}"


def process_file(path: Path, prefix: str, used_ids: set[str]) -> None:
    text = read_text_raw(path)
    lines = text.splitlines(keepends=True)

    in_fence = False
    first_heading = True
    updated_lines: list[str] = []

    for line in lines:
        fence_match = re.match(r"^\s*(```|~~~)", line)
        if fence_match:
            in_fence = not in_fence

        if not in_fence:
            heading_match = re.match(r"^(#{1,6})\s+(.*)$", line)
            if heading_match:
                hashes = heading_match.group(1)
                heading_text = heading_match.group(2).rstrip()

                if "{#" in heading_text or "id=\"" in heading_text:
                    updated_lines.append(line)
                    first_heading = False
                    continue

                anchor = anchor_for_heading(prefix, heading_text, first_heading)
                if anchor in used_ids:
                    suffix = 2
                    candidate = f"{anchor}-{suffix}"
                    while candidate in used_ids:
                        suffix += 1
                        candidate = f"{anchor}-{suffix}"
                    anchor = candidate
                used_ids.add(anchor)

                suffix = "\n" if line.endswith("\n") else ""
                updated_lines.append(f"{hashes} {heading_text} {{#{anchor}}}{suffix}")
                first_heading = False
                continue

        updated_lines.append(line)

    write_text_raw(path, "".join(updated_lines))


def main() -> None:
    files = parse_toc_files()
    used_ids: set[str] = set()

    for path in files:
        name = path.name
        prefix = DOC_PREFIX.get(name)
        if prefix is None:
            prefix = part_prefix(name)
        if prefix is None:
            raise RuntimeError(f"No prefix mapping for {name}")
        process_file(path, prefix, used_ids)


if __name__ == "__main__":
    main()

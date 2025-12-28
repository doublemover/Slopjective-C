#!/usr/bin/env python3

from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TOC_PATH = ROOT / "TABLE_OF_CONTENTS.md"
OUTPUT_DIR = ROOT / "site"
OUTPUT_PATH = OUTPUT_DIR / "index.md"
EXCLUDE = {"README.md"}


def parse_toc(path: Path) -> list[str]:
    text = path.read_text(encoding="utf-8")
    names = re.findall(r"(?m)^- \*\*([A-Za-z0-9_.-]+\.md)\*\*", text)
    if not names:
        raise RuntimeError("No .md entries found in TABLE_OF_CONTENTS.md")

    counts: dict[str, int] = {}
    for name in names:
        counts[name] = counts.get(name, 0) + 1

    duplicates = [name for name, count in counts.items() if count > 1]
    if duplicates:
        dupes = ", ".join(sorted(duplicates))
        raise RuntimeError(f"Duplicate entries in TABLE_OF_CONTENTS.md: {dupes}")

    return names


def validate_files(names: list[str]) -> list[Path]:
    if "TABLE_OF_CONTENTS.md" not in names:
        raise RuntimeError("TABLE_OF_CONTENTS.md is missing from the file list.")
    if names[0] != "TABLE_OF_CONTENTS.md":
        raise RuntimeError("TABLE_OF_CONTENTS.md must be the first entry in TABLE_OF_CONTENTS.md.")
    if any(name in EXCLUDE for name in names):
        raise RuntimeError("README.md must not be included in the stitched output.")

    ordered = ["TABLE_OF_CONTENTS.md"] + [name for name in names if name != "TABLE_OF_CONTENTS.md"]
    missing = [name for name in ordered if not (ROOT / name).is_file()]
    if missing:
        raise RuntimeError(f"Missing input files: {', '.join(missing)}")

    return [ROOT / name for name in ordered]


def stitch(paths: list[Path]) -> str:
    sections: list[str] = []
    for path in paths:
        text = path.read_text(encoding="utf-8").rstrip("\n")
        header = f"<!-- BEGIN {path.name} -->"
        footer = f"<!-- END {path.name} -->"
        sections.append(f"{header}\n{text}\n{footer}")
    return "\n\n---\n\n".join(sections) + "\n"


def main() -> None:
    names = parse_toc(TOC_PATH)
    paths = validate_files(names)

    front_matter = "\n".join(
        [
            "---",
            "title: Objective-C 3.0 Draft Specification",
            "layout: default",
            "---",
            "",
        ]
    )

    output = front_matter + stitch(paths)
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    OUTPUT_PATH.write_text(output, encoding="utf-8")

    print(f"Wrote {OUTPUT_PATH} ({len(paths)} documents).")


if __name__ == "__main__":
    main()

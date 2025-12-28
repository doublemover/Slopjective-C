#!/usr/bin/env python3

from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SPEC_DIR = ROOT / "spec"
OUTPUT_DIR = ROOT / "site"
OUTPUT_PATH = OUTPUT_DIR / "index.md"
EXCLUDE = {"README.md"}
HEADING_ANCHOR_RE = re.compile(r"^(#{1,6})\s+(.*?)\s+\{#([A-Za-z0-9_.-]+)\}\s*$")


def parse_toc(path: Path) -> list[str]:
    text = path.read_text(encoding="utf-8")
    names = re.findall(r"(?m)^-\s+.*?([A-Za-z0-9_.-]+\.md)", text)
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


def validate_files(names: list[str], root: Path) -> list[Path]:
    if "TABLE_OF_CONTENTS.md" not in names:
        raise RuntimeError("TABLE_OF_CONTENTS.md is missing from the file list.")
    if names[0] != "TABLE_OF_CONTENTS.md":
        raise RuntimeError("TABLE_OF_CONTENTS.md must be the first entry in TABLE_OF_CONTENTS.md.")
    if any(name in EXCLUDE for name in names):
        raise RuntimeError("README.md must not be included in the stitched output.")

    ordered = ["TABLE_OF_CONTENTS.md"] + [name for name in names if name != "TABLE_OF_CONTENTS.md"]
    missing = [name for name in ordered if not (root / name).is_file()]
    if missing:
        raise RuntimeError(f"Missing input files: {', '.join(missing)}")

    return [root / name for name in ordered]


def stitch(paths: list[Path]) -> str:
    sections: list[str] = []
    for path in paths:
        text = path.read_text(encoding="utf-8").rstrip("\n")
        text = rewrite_heading_anchors(text)
        header = f"<!-- BEGIN {path.name} -->"
        footer = f"<!-- END {path.name} -->"
        sections.append(f"{header}\n{text}\n{footer}")
    return "\n\n---\n\n".join(sections) + "\n"


def rewrite_heading_anchors(text: str) -> str:
    lines = text.splitlines(keepends=True)
    in_fence = False
    updated: list[str] = []

    for line in lines:
        fence_match = re.match(r"^\s*(```|~~~)", line)
        if fence_match:
            in_fence = not in_fence
            updated.append(line)
            continue

        if not in_fence:
            match = HEADING_ANCHOR_RE.match(line.rstrip("\n"))
            if match:
                hashes, heading, anchor = match.groups()
                newline = "\n" if line.endswith("\n") else ""
                updated.append(f"{hashes} {heading} <a id=\"{anchor}\"></a>{newline}")
                continue

        updated.append(line)

    return "".join(updated)

def build_pages(root: Path) -> tuple[Path, int]:
    spec_dir = root / "spec"
    toc_path = spec_dir / "TABLE_OF_CONTENTS.md"
    output_dir = root / "site"
    output_path = output_dir / "index.md"

    names = parse_toc(toc_path)
    paths = validate_files(names, spec_dir)

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
    output_dir.mkdir(parents=True, exist_ok=True)
    output_path.write_text(output, encoding="utf-8")

    return output_path, len(paths)


def main() -> None:
    output_path, count = build_pages(ROOT)
    print(f"Wrote {output_path} ({count} documents).")


if __name__ == "__main__":
    main()

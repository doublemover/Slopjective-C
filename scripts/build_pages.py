#!/usr/bin/env python3

from __future__ import annotations

import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUTPUT_DIR = ROOT / "site"
OUTPUT_PATH = OUTPUT_DIR / "index.md"
SRC_DIR = ROOT / "site" / "src"
CONFIG_PATH = SRC_DIR / "index.contract.json"
HEADING_ANCHOR_RE = re.compile(r"^(#{1,6})\s+(.*?)\s+\{#([A-Za-z0-9_.-]+)\}\s*$")
SITE_EXCLUDE_START = "<!-- SITE:EXCLUDE-START -->"
SITE_EXCLUDE_END = "<!-- SITE:EXCLUDE-END -->"


class ContractConfig:
    def __init__(self, *, output_path: Path, body_path: Path, front_matter: str) -> None:
        self.output_path = output_path
        self.body_path = body_path
        self.front_matter = front_matter


def strip_site_excluded_blocks(text: str) -> str:
    lines = text.splitlines(keepends=True)
    filtered: list[str] = []
    skipping = False

    for line in lines:
        marker = line.strip()
        if marker == SITE_EXCLUDE_START:
            skipping = True
            continue
        if marker == SITE_EXCLUDE_END:
            skipping = False
            continue
        if not skipping:
            filtered.append(line)

    return "".join(filtered)


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


def render_markdown_source(path: Path) -> str:
    text = path.read_text(encoding="utf-8").rstrip("\n")
    text = strip_site_excluded_blocks(text)
    text = rewrite_heading_anchors(text)
    return text + "\n"


def load_contract(root: Path = ROOT) -> ContractConfig:
    config_path = root / "site" / "src" / "index.contract.json"
    payload = json.loads(config_path.read_text(encoding="utf-8"))

    if payload.get("contract_id") != "site-index-generator/v2":
        raise RuntimeError(
            "site index contract drift: expected 'site-index-generator/v2' "
            f"observed {payload.get('contract_id')!r}"
        )

    output_path = root / payload["output_path"]
    body_path = root / payload["body_path"]
    front_matter = "\n".join(payload["front_matter"])
    if not front_matter.endswith("\n"):
        front_matter += "\n"

    return ContractConfig(
        output_path=output_path,
        body_path=body_path,
        front_matter=front_matter,
    )


def build_pages(root: Path) -> tuple[Path, int]:
    config = load_contract(root)
    output = config.front_matter + render_markdown_source(config.body_path)
    config.output_path.parent.mkdir(parents=True, exist_ok=True)
    config.output_path.write_text(output, encoding="utf-8")
    return config.output_path, 1


def main() -> None:
    output_path, count = build_pages(ROOT)
    print(f"Wrote {output_path} ({count} document).")


if __name__ == "__main__":
    main()

from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_INCLUDE_GLOBS = (
    "spec/*.md",
    "spec/conformance/**/*.md",
    "spec/governance/**/*.md",
    "spec/process/**/*.md",
)

HEADING_WITH_NUMBER_RE = re.compile(r"^(#{1,6})\s+([0-9]+(?:\.[0-9]+)*)\b")
HEADING_ANCHOR_RE = re.compile(r"^(#{1,6})\s+.*\{#([A-Za-z0-9_.-]+)\}\s*$")
TOP_LEVEL_SECTION_RE = re.compile(r"^##\s+([0-9]+)\.([0-9]+)(?!\.)\b")
BULLET_LINE_RE = re.compile(r"^\s*[-*]\s+(.+?)\s*$")
CONJUNCTION_TAIL_RE = re.compile(r"\b(and|or)\s*$", re.IGNORECASE)
HEADING_LINE_RE = re.compile(r"^#{1,6}\s")
HORIZONTAL_RULE_RE = re.compile(r"^\s*---+\s*$")
MARKDOWN_LINK_RE = re.compile(r"\[[^\]]+\]\(([^)]+)\)")

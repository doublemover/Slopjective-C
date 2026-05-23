"""Output parsing helpers for LLVM command probes."""

from __future__ import annotations

import re


def first_non_empty_line(text: str) -> str:
    for line in text.splitlines():
        stripped = line.strip()
        if stripped:
            return stripped
    return ""


def llc_help_mentions_filetype_obj(help_text: str) -> tuple[bool, bool]:
    mentions_filetype = "--filetype" in help_text or "-filetype" in help_text
    mentions_obj = re.search(r"\bobj\b", help_text) is not None
    return mentions_filetype, mentions_obj


def tool_version_from_text(text: str) -> str:
    match = re.search(r"\b(?:LLVM|clang)\s+version\s+([0-9]+(?:\.[0-9]+){0,3})", text, re.IGNORECASE)
    return match.group(1) if match else ""


def tool_vendor_from_headline(headline: str) -> str:
    lowered = headline.lower()
    if "apple clang" in lowered:
        return "apple-clang"
    if "debian llvm" in lowered:
        return "debian-llvm"
    if "ubuntu clang" in lowered or "ubuntu llvm" in lowered:
        return "ubuntu-llvm"
    if "clang" in lowered:
        return "llvm-clang"
    if "llvm" in lowered:
        return "llvm"
    return "unknown"

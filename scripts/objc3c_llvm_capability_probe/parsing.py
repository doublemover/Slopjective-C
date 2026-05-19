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

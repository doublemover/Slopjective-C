from __future__ import annotations


def normalize_line_endings(value: str) -> str:
    return value.replace("\r\n", "\n").replace("\r", "\n")


def normalize_inline_text(value: str) -> str:
    normalized = normalize_line_endings(value).strip()
    if "\n" not in normalized:
        return normalized
    parts = [part.strip() for part in normalized.split("\n") if part.strip()]
    return " ".join(parts).strip()

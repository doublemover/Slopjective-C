from __future__ import annotations

from typing import Any

from seed_issue_payloads.model import ParseError


def expect_dict(value: Any, context: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise ParseError(f"{context} must be an object")
    return value


def expect_list(value: Any, context: str) -> list[Any]:
    if not isinstance(value, list):
        raise ParseError(f"{context} must be an array")
    return value


def expect_nonempty_str(value: Any, context: str) -> str:
    if not isinstance(value, str):
        raise ParseError(f"{context} must be a string")
    stripped = value.strip()
    if not stripped:
        raise ParseError(f"{context} must be a non-empty string")
    return stripped


def expect_int(value: Any, context: str) -> int:
    if isinstance(value, bool) or not isinstance(value, int):
        raise ParseError(f"{context} must be an integer")
    return value


def parse_string_list(value: Any, context: str) -> tuple[str, ...]:
    items = expect_list(value, context)
    if not items:
        raise ParseError(f"{context} must not be empty")
    parsed: list[str] = []
    for idx, item in enumerate(items):
        parsed.append(expect_nonempty_str(item, f"{context}[{idx}]"))
    return tuple(parsed)


def parse_optional_nonempty_str(value: Any, context: str) -> str | None:
    if value is None:
        return None
    return expect_nonempty_str(value, context)

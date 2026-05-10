from __future__ import annotations

from typing import Any

from .models import JsonObject


def append_failure(
    failures: list[JsonObject],
    code: str,
    detail: str,
    **extra: Any,
) -> None:
    failures.append({"code": code, "detail": detail, **extra})

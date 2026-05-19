"""Typed model for objc3c workflow actions."""

from __future__ import annotations

from collections.abc import Callable
from dataclasses import dataclass


@dataclass(frozen=True)
class ActionSpec:
    action: str
    summary: str
    backend: str
    validation_tier: str = ""
    guarantee_owner: str = ""
    pass_through_args: bool = False


ActionHandler = Callable[[list[str]], int]

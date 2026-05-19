"""Shared activation preflight orchestration types."""

from __future__ import annotations

from typing import Callable

from scripts.activation_preflight.contracts import CommandResult, CommandSpec


CommandRunner = Callable[[CommandSpec], CommandResult]


__all__ = ["CommandRunner"]

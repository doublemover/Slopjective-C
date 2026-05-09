"""CLI dispatch owner for objc3c workflow entrypoints."""

from __future__ import annotations

from collections.abc import Sequence

from .cli_argv import workflow_argv
from .request_dispatch import parse_and_dispatch_workflow_request


def dispatch_cli(argv: Sequence[str] | None = None) -> int:
    return parse_and_dispatch_workflow_request(workflow_argv(argv))


__all__ = ["dispatch_cli"]

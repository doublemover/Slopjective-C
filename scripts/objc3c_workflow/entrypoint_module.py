"""Module entrypoint for `python -m scripts.objc3c_workflow`."""

from __future__ import annotations

from collections.abc import Sequence

from .cli_dispatch import dispatch_cli
from .entrypoint_policy import dispatch_entrypoint


def main(argv: Sequence[str] | None = None) -> int:
    return dispatch_entrypoint(dispatch_cli, argv)


__all__ = ["main"]

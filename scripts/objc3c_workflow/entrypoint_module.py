"""Module entrypoint for `python -m scripts.objc3c_workflow`."""

from __future__ import annotations

from collections.abc import Sequence

from .cli_dispatch import dispatch_cli


def main(argv: Sequence[str] | None = None) -> int:
    return dispatch_cli(argv)


__all__ = ["main"]

"""Direct script entrypoint for the objc3c workflow runner."""

from __future__ import annotations

from collections.abc import Sequence

from scripts.objc3c_workflow.cli_dispatch import dispatch_cli
from scripts.objc3c_workflow.entrypoint_policy import dispatch_entrypoint


def main(argv: Sequence[str]) -> int:
    return dispatch_entrypoint(dispatch_cli, argv)


__all__ = ["main"]

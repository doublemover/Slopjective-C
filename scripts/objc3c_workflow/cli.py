"""Command-line entrypoint for the objc3c workflow action registry."""

from __future__ import annotations

import sys
from collections.abc import Sequence

from .request_dispatch import parse_and_dispatch_workflow_request


def main(argv: Sequence[str] | None = None) -> int:
    return parse_and_dispatch_workflow_request(
        sys.argv[1:] if argv is None else argv
    )

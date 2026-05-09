#!/usr/bin/env python3
"""Retired direct script entrypoint for the objc3c workflow CLI."""

from __future__ import annotations

from collections.abc import Sequence
from typing import NoReturn

DIRECT_RUNNER_ERROR = (
    "error: scripts/objc3c_workflow/runner.py is not a public command surface; "
    "use `npm run objc3c -- <action>`."
)


def reject_direct_runner() -> NoReturn:
    raise SystemExit(DIRECT_RUNNER_ERROR)


def main(_argv: Sequence[str]) -> NoReturn:
    return reject_direct_runner()


if __name__ == "__main__":
    reject_direct_runner()

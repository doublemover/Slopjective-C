#!/usr/bin/env python3
"""Retired direct script entrypoint for the objc3c workflow CLI."""

from __future__ import annotations

from collections.abc import Sequence

from scripts.objc3c_workflow.entrypoint_rejection import reject_direct_runner


def main(_argv: Sequence[str]) -> None:
    return reject_direct_runner()


if __name__ == "__main__":
    reject_direct_runner()

"""CLI argument parsing for runnable block/ARC end-to-end validation."""

from __future__ import annotations

import argparse
import sys
from dataclasses import dataclass
from typing import Sequence


@dataclass(frozen=True)
class BlockArcCliOptions:
    ignored_args: tuple[str, ...] = ()


def parse_args(argv: Sequence[str] | None = None) -> BlockArcCliOptions:
    raw_args = tuple(sys.argv[1:] if argv is None else argv)
    parser = argparse.ArgumentParser(add_help=False)
    _, ignored_args = parser.parse_known_args(raw_args)
    return BlockArcCliOptions(ignored_args=tuple(ignored_args))


__all__ = ["BlockArcCliOptions", "parse_args"]

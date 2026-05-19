"""CLI argument parsing for runnable metaprogramming end-to-end validation."""

from __future__ import annotations

import argparse
import sys
from dataclasses import dataclass
from typing import Sequence


@dataclass(frozen=True)
class RunnableMetaprogrammingCliOptions:
    ignored_args: tuple[str, ...] = ()


def parse_args(argv: Sequence[str] | None = None) -> RunnableMetaprogrammingCliOptions:
    raw_args = tuple(sys.argv[1:] if argv is None else argv)
    parser = argparse.ArgumentParser(add_help=False)
    _, ignored_args = parser.parse_known_args(raw_args)
    return RunnableMetaprogrammingCliOptions(ignored_args=tuple(ignored_args))


__all__ = ["RunnableMetaprogrammingCliOptions", "parse_args"]

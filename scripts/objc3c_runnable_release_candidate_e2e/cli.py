"""CLI argument parsing for runnable release-candidate end-to-end validation."""

from __future__ import annotations

import argparse
import sys
from dataclasses import dataclass
from typing import Sequence


@dataclass(frozen=True)
class ReleaseCandidateCliOptions:
    ignored_args: tuple[str, ...] = ()


def parse_args(argv: Sequence[str] | None = None) -> ReleaseCandidateCliOptions:
    raw_args = tuple(sys.argv[1:] if argv is None else argv)
    parser = argparse.ArgumentParser(add_help=False)
    _, ignored_args = parser.parse_known_args(raw_args)
    return ReleaseCandidateCliOptions(ignored_args=tuple(ignored_args))


__all__ = ["ReleaseCandidateCliOptions", "parse_args"]

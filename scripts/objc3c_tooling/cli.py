"""Tiny argparse option helpers for repeated, identical CLI fragments."""

from __future__ import annotations

import argparse


def add_check_argument(parser: argparse.ArgumentParser) -> None:
    parser.add_argument("--check", action="store_true")


from __future__ import annotations

import argparse
from pathlib import Path

from .config import DEFAULT_JSON_OUT, DEFAULT_MD_OUT, DEFAULT_POLICY


def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--policy", type=Path, default=DEFAULT_POLICY)
    parser.add_argument("--summary-json", type=Path, default=DEFAULT_JSON_OUT)
    parser.add_argument("--summary-md", type=Path, default=DEFAULT_MD_OUT)
    parser.add_argument(
        "--check",
        action="store_true",
        help="fail if checked-in reports differ from generated output",
    )
    return parser

#!/usr/bin/env python3
"""Script entrypoint for the objc3c workflow CLI."""

from __future__ import annotations

import sys
from collections.abc import Sequence
from pathlib import Path

SCRIPT_ROOT = Path(__file__).resolve().parents[1]
ROOT = Path(__file__).resolve().parents[2]
for import_root in (ROOT, SCRIPT_ROOT):
    import_root_text = str(import_root)
    if import_root_text not in sys.path:
        sys.path.insert(0, import_root_text)

from scripts.objc3c_workflow import cli


def main(argv: Sequence[str]) -> int:
    return cli.main(argv)


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

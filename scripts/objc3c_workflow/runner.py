#!/usr/bin/env python3
"""Script entrypoint for the objc3c workflow CLI."""

from __future__ import annotations

import sys
from collections.abc import Sequence

if not __package__:
    raise SystemExit(
        "error: scripts/objc3c_workflow/runner.py is not a public command surface; "
        "use `npm run objc3c -- <action>`."
    )

from .path_bootstrap import install_workflow_import_roots

install_workflow_import_roots()

from scripts.objc3c_workflow.entrypoint_script import main as script_main


def main(argv: Sequence[str]) -> int:
    return script_main(argv)


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

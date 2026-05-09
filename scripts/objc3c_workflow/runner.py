#!/usr/bin/env python3
"""Script entrypoint for the objc3c workflow CLI."""

from __future__ import annotations

import sys
from collections.abc import Sequence

if __package__:
    from .path_bootstrap import install_workflow_import_roots
else:
    from path_bootstrap import install_workflow_import_roots

install_workflow_import_roots()

from scripts.objc3c_workflow import cli


def main(argv: Sequence[str]) -> int:
    return cli.main(argv)


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

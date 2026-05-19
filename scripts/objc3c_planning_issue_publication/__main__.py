"""Run the Objective-C 3 planning issue publisher as a module."""

from __future__ import annotations

import sys

from .cli import main


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

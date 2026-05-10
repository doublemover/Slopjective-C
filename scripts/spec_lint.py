#!/usr/bin/env python3

from __future__ import annotations

import sys

if __package__:
    from .spec_linting.cli import main
else:
    from spec_linting.cli import main


if __name__ == "__main__":
    sys.exit(main())

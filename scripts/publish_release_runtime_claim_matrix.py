#!/usr/bin/env python3
"""Publish the release/runtime claim matrix."""

from __future__ import annotations

import sys

from objc3c_release_claim_matrix.cli import main


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

#!/usr/bin/env python3
"""Compatibility entry point for the live objc3 runtime acceptance workload."""

from __future__ import annotations

from objc3c_runtime_acceptance.core import *  # noqa: F401,F403
from objc3c_runtime_acceptance.core import main


if __name__ == "__main__":
    raise SystemExit(main())

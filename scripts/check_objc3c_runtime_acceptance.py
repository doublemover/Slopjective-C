#!/usr/bin/env python3
"""Entry point for the live objc3 runtime acceptance workload."""

from __future__ import annotations

from objc3c_runtime_acceptance.cli_orchestration import main


if __name__ == "__main__":
    raise SystemExit(main())

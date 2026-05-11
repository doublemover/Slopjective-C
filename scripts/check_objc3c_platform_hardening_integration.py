#!/usr/bin/env python3
"""Validate platform-hardening publication across the live public package and release workflow."""

from __future__ import annotations

from scripts.objc3c_platform_hardening_integration_check.runner import main


if __name__ == "__main__":
    raise SystemExit(main())

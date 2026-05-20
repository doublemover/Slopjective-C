#!/usr/bin/env python3
"""Validate release ABI/API drift blockers before public publication."""

from __future__ import annotations

from objc3c_release_manifest.abi_api_drift import main


if __name__ == "__main__":
    raise SystemExit(main())

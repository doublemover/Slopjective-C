#!/usr/bin/env python3
"""Validate runnable metaprogramming execution end to end from the staged package root."""

from __future__ import annotations

from objc3c_runnable_metaprogramming_e2e import PACKAGE_CONTRACT_ID
from objc3c_runnable_metaprogramming_e2e import PACKAGE_PS1
from objc3c_runnable_metaprogramming_e2e import PWSH
from objc3c_runnable_metaprogramming_e2e import REPORT_PATH
from objc3c_runnable_metaprogramming_e2e import REQUIRED_MANIFEST_KEYS
from objc3c_runnable_metaprogramming_e2e import ROOT
from objc3c_runnable_metaprogramming_e2e import SUMMARY_CONTRACT_ID
from objc3c_runnable_metaprogramming_e2e import compile_fixture
from objc3c_runnable_metaprogramming_e2e import expect
from objc3c_runnable_metaprogramming_e2e import load_and_validate_manifest
from objc3c_runnable_metaprogramming_e2e import main
from objc3c_runnable_metaprogramming_e2e import manifest_path
from objc3c_runnable_metaprogramming_e2e import parse_args
from objc3c_runnable_metaprogramming_e2e import validate_package_manifest

__all__ = [
    "PACKAGE_CONTRACT_ID",
    "PACKAGE_PS1",
    "PWSH",
    "REPORT_PATH",
    "REQUIRED_MANIFEST_KEYS",
    "ROOT",
    "SUMMARY_CONTRACT_ID",
    "compile_fixture",
    "expect",
    "load_and_validate_manifest",
    "main",
    "manifest_path",
    "parse_args",
    "validate_package_manifest",
]


if __name__ == "__main__":
    raise SystemExit(main())

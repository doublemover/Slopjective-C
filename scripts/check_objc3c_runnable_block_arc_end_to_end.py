#!/usr/bin/env python3
"""Validate runnable block/ARC execution end to end from the staged package root."""

from __future__ import annotations

from objc3c_runnable_block_arc_e2e import BLOCK_ARC_SMOKE_FIXTURES
from objc3c_runnable_block_arc_e2e import PACKAGE_CONTRACT_ID
from objc3c_runnable_block_arc_e2e import PACKAGE_PS1
from objc3c_runnable_block_arc_e2e import PWSH
from objc3c_runnable_block_arc_e2e import REPORT_PATH
from objc3c_runnable_block_arc_e2e import REQUIRED_MANIFEST_KEYS
from objc3c_runnable_block_arc_e2e import REQUIRED_DIRECTORY_MANIFEST_KEYS
from objc3c_runnable_block_arc_e2e import REQUIRED_FILE_MANIFEST_KEYS
from objc3c_runnable_block_arc_e2e import ROOT
from objc3c_runnable_block_arc_e2e import RUNNER_PATH
from objc3c_runnable_block_arc_e2e import SUMMARY_CONTRACT_ID
from objc3c_runnable_block_arc_e2e import BlockArcCliOptions
from objc3c_runnable_block_arc_e2e import assert_runtime_abi_probe_payload
from objc3c_runnable_block_arc_e2e import assert_byref_forwarding_probe_payload
from objc3c_runnable_block_arc_e2e import build_summary_payload
from objc3c_runnable_block_arc_e2e import compile_block_arc_fixture
from objc3c_runnable_block_arc_e2e import compile_byref_forwarding_probe
from objc3c_runnable_block_arc_e2e import compile_runtime_abi_probe
from objc3c_runnable_block_arc_e2e import expect
from objc3c_runnable_block_arc_e2e import link_packaged_block_arc_fixture
from objc3c_runnable_block_arc_e2e import load_and_validate_manifest
from objc3c_runnable_block_arc_e2e import main
from objc3c_runnable_block_arc_e2e import manifest_path
from objc3c_runnable_block_arc_e2e import package_runnable_toolchain
from objc3c_runnable_block_arc_e2e import parse_args
from objc3c_runnable_block_arc_e2e import parse_byref_forwarding_probe_payload
from objc3c_runnable_block_arc_e2e import parse_runtime_abi_probe_payload
from objc3c_runnable_block_arc_e2e import run_packaged_execution_replay
from objc3c_runnable_block_arc_e2e import run_packaged_execution_smoke
from objc3c_runnable_block_arc_e2e import run_packaged_executable
from objc3c_runnable_block_arc_e2e import validate_package_manifest
from objc3c_runnable_block_arc_e2e import write_smoke_fixture_list
from objc3c_runnable_block_arc_e2e import write_summary_report

__all__ = [
    "BLOCK_ARC_SMOKE_FIXTURES",
    "BlockArcCliOptions",
    "PACKAGE_CONTRACT_ID",
    "PACKAGE_PS1",
    "PWSH",
    "REPORT_PATH",
    "REQUIRED_MANIFEST_KEYS",
    "REQUIRED_DIRECTORY_MANIFEST_KEYS",
    "REQUIRED_FILE_MANIFEST_KEYS",
    "ROOT",
    "RUNNER_PATH",
    "SUMMARY_CONTRACT_ID",
    "assert_byref_forwarding_probe_payload",
    "assert_runtime_abi_probe_payload",
    "build_summary_payload",
    "compile_block_arc_fixture",
    "compile_byref_forwarding_probe",
    "compile_runtime_abi_probe",
    "expect",
    "link_packaged_block_arc_fixture",
    "load_and_validate_manifest",
    "main",
    "manifest_path",
    "package_runnable_toolchain",
    "parse_args",
    "parse_byref_forwarding_probe_payload",
    "parse_runtime_abi_probe_payload",
    "run_packaged_execution_replay",
    "run_packaged_execution_smoke",
    "run_packaged_executable",
    "validate_package_manifest",
    "write_smoke_fixture_list",
    "write_summary_report",
]


if __name__ == "__main__":
    raise SystemExit(main())

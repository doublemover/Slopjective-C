"""Runnable block/ARC end-to-end checker helpers."""

from __future__ import annotations

from .assertions import expect
from .catalog import BLOCK_ARC_SMOKE_FIXTURES
from .cli import BlockArcCliOptions
from .cli import parse_args
from .commands import compile_block_arc_fixture
from .commands import compile_byref_forwarding_probe
from .commands import compile_runtime_abi_probe
from .commands import link_packaged_block_arc_fixture
from .commands import package_runnable_toolchain
from .commands import run_packaged_execution_replay
from .commands import run_packaged_execution_smoke
from .commands import run_packaged_executable
from .commands import write_smoke_fixture_list
from .manifest import REQUIRED_DIRECTORY_MANIFEST_KEYS
from .manifest import REQUIRED_FILE_MANIFEST_KEYS
from .manifest import REQUIRED_MANIFEST_KEYS
from .manifest import load_and_validate_manifest
from .manifest import manifest_path
from .manifest import validate_package_manifest
from .paths import PACKAGE_CONTRACT_ID
from .paths import PACKAGE_PS1
from .paths import PWSH
from .paths import REPORT_PATH
from .paths import ROOT
from .paths import RUNNER_PATH
from .paths import SUMMARY_CONTRACT_ID
from .report import build_summary_payload
from .report import write_summary_report
from .results import assert_byref_forwarding_probe_payload
from .results import assert_runtime_abi_probe_payload
from .results import parse_byref_forwarding_probe_payload
from .results import parse_runtime_abi_probe_payload
from .runner import main

__all__ = [
    "BLOCK_ARC_SMOKE_FIXTURES",
    "BlockArcCliOptions",
    "PACKAGE_CONTRACT_ID",
    "PACKAGE_PS1",
    "PWSH",
    "REPORT_PATH",
    "REQUIRED_DIRECTORY_MANIFEST_KEYS",
    "REQUIRED_FILE_MANIFEST_KEYS",
    "REQUIRED_MANIFEST_KEYS",
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

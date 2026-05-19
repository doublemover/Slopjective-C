"""Runnable metaprogramming end-to-end checker helpers."""

from __future__ import annotations

from .assertions import expect
from .catalog import FIRST_HOST_CACHE_EXPECTATION
from .catalog import PRESERVED_HOST_CACHE_FIELDS
from .catalog import REQUIRED_MANIFEST_KEYS
from .catalog import RUNTIME_PROBE_EXPECTATION
from .catalog import SECOND_HOST_CACHE_EXPECTATION
from .catalog import HostCacheExpectation
from .cli import RunnableMetaprogrammingCliOptions
from .cli import parse_args
from .commands import compile_fixture
from .commands import compile_packaged_probe
from .commands import package_runnable_toolchain
from .commands import run_packaged_execution_replay
from .commands import run_packaged_execution_smoke
from .commands import run_packaged_probe
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
from .results import assert_consumer_link_plan
from .results import assert_host_cache_expectation
from .results import assert_preserved_host_cache_fields
from .results import assert_provider_module_name
from .results import assert_runtime_probe_payload
from .results import parse_runtime_probe_payload
from .runner import main

__all__ = [
    "FIRST_HOST_CACHE_EXPECTATION",
    "HostCacheExpectation",
    "PACKAGE_CONTRACT_ID",
    "PACKAGE_PS1",
    "PRESERVED_HOST_CACHE_FIELDS",
    "PWSH",
    "REPORT_PATH",
    "REQUIRED_MANIFEST_KEYS",
    "ROOT",
    "RUNNER_PATH",
    "RUNTIME_PROBE_EXPECTATION",
    "RunnableMetaprogrammingCliOptions",
    "SECOND_HOST_CACHE_EXPECTATION",
    "SUMMARY_CONTRACT_ID",
    "assert_consumer_link_plan",
    "assert_host_cache_expectation",
    "assert_preserved_host_cache_fields",
    "assert_provider_module_name",
    "assert_runtime_probe_payload",
    "build_summary_payload",
    "compile_fixture",
    "compile_packaged_probe",
    "expect",
    "load_and_validate_manifest",
    "main",
    "manifest_path",
    "package_runnable_toolchain",
    "parse_args",
    "parse_runtime_probe_payload",
    "run_packaged_execution_replay",
    "run_packaged_execution_smoke",
    "run_packaged_probe",
    "validate_package_manifest",
    "write_summary_report",
]

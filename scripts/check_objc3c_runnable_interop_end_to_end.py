#!/usr/bin/env python3
"""Validate runnable mixed-module interop execution end to end from the staged package root."""

from __future__ import annotations

from objc3c_runnable_interop_end_to_end.assertions import expect
from objc3c_runnable_interop_end_to_end.compilation import assert_artifacts_exist
from objc3c_runnable_interop_end_to_end.compilation import compile_consumer_fixture
from objc3c_runnable_interop_end_to_end.compilation import compile_fixture
from objc3c_runnable_interop_end_to_end.compilation import consumer_artifact_paths
from objc3c_runnable_interop_end_to_end.manifest import REQUIRED_MANIFEST_KEYS
from objc3c_runnable_interop_end_to_end.manifest import manifest_path
from objc3c_runnable_interop_end_to_end.manifest import validate_package_manifest
from objc3c_runnable_interop_end_to_end.paths import PACKAGE_CONTRACT_ID
from objc3c_runnable_interop_end_to_end.paths import PACKAGE_PS1
from objc3c_runnable_interop_end_to_end.paths import PWSH
from objc3c_runnable_interop_end_to_end.paths import REPORT_PATH
from objc3c_runnable_interop_end_to_end.paths import ROOT
from objc3c_runnable_interop_end_to_end.paths import SUMMARY_CONTRACT_ID
from objc3c_runnable_interop_end_to_end.probes import assert_bridge_probe_payload
from objc3c_runnable_interop_end_to_end.probes import assert_packaging_probe_payload
from objc3c_runnable_interop_end_to_end.probes import compile_and_run_probe
from objc3c_runnable_interop_end_to_end.runner import assert_header_bridge_link_plan
from objc3c_runnable_interop_end_to_end.runner import assert_runtime_interop_link_plan
from objc3c_runnable_interop_end_to_end.runner import main

__all__ = [
    "PACKAGE_CONTRACT_ID",
    "PACKAGE_PS1",
    "PWSH",
    "REPORT_PATH",
    "REQUIRED_MANIFEST_KEYS",
    "ROOT",
    "SUMMARY_CONTRACT_ID",
    "assert_artifacts_exist",
    "assert_bridge_probe_payload",
    "assert_header_bridge_link_plan",
    "assert_packaging_probe_payload",
    "assert_runtime_interop_link_plan",
    "compile_and_run_probe",
    "compile_consumer_fixture",
    "compile_fixture",
    "consumer_artifact_paths",
    "expect",
    "main",
    "manifest_path",
    "validate_package_manifest",
]


if __name__ == "__main__":
    raise SystemExit(main())

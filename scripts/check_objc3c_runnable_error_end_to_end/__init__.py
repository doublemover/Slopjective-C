"""Runnable error end-to-end checker package facade."""

from __future__ import annotations

from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import normalize_rel_path, repo_rel
from objc3c_tooling.probe_compile import find_clangxx
from objc3c_tooling.probe_output import parse_json_output
from objc3c_tooling.public_workflow_output import extract_output_value
from objc3c_tooling.public_workflow_output import extract_report_paths
from objc3c_tooling.subprocesses import run_capture

from .artifacts import assert_compile_artifacts_exist, compile_artifact_paths
from .commands import (
    compile_error_fixture,
    compile_packaged_error_probe,
    package_runnable_toolchain,
    run_packaged_error_probe,
    run_packaged_execution_replay,
    run_packaged_execution_smoke,
)
from .manifest import REQUIRED_MANIFEST_KEYS, manifest_path, validate_package_manifest
from .paths import (
    PACKAGE_CONTRACT_ID,
    PACKAGE_PS1,
    PWSH,
    REPORT_PATH,
    ROOT,
    SUMMARY_CONTRACT_ID,
)
from .probes import EXPECTED_PROBE_INTEGER_FIELDS, assert_probe_payload
from .report import build_summary_payload, write_summary_report
from .runner import main
from .assertions import expect

__all__ = [
    "EXPECTED_PROBE_INTEGER_FIELDS",
    "PACKAGE_CONTRACT_ID",
    "PACKAGE_PS1",
    "PWSH",
    "REPORT_PATH",
    "REQUIRED_MANIFEST_KEYS",
    "ROOT",
    "SUMMARY_CONTRACT_ID",
    "assert_compile_artifacts_exist",
    "assert_probe_payload",
    "build_summary_payload",
    "compile_artifact_paths",
    "compile_error_fixture",
    "compile_packaged_error_probe",
    "expect",
    "extract_output_value",
    "extract_report_paths",
    "find_clangxx",
    "load_json",
    "main",
    "manifest_path",
    "normalize_rel_path",
    "package_runnable_toolchain",
    "parse_json_output",
    "repo_rel",
    "run_capture",
    "run_packaged_error_probe",
    "run_packaged_execution_replay",
    "run_packaged_execution_smoke",
    "validate_package_manifest",
    "write_json_file",
    "write_summary_report",
]

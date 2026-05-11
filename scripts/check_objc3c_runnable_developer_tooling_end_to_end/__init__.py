"""Runnable developer-tooling end-to-end checker internals."""

from __future__ import annotations

from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import normalize_rel_path
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.public_workflow_output import extract_output_value
from objc3c_tooling.public_workflow_output import extract_report_paths
from objc3c_tooling.subprocesses import run_capture
from scripts.objc3c_workflow.public_command_api import public_workflow_command

from .assertions import expect
from .cli import main
from .commands import run_package_command
from .constants import (
    CONTRACT_PATH,
    PACKAGE_CONTRACT_ID,
    PACKAGE_PS1,
    PWSH,
    REPORT_PATH,
    ROOT,
    RUNNER_PATH,
    SUMMARY_CONTRACT_ID,
)
from .manifest import load_and_validate_manifest
from .manifest import validate_manifest_contract
from .outputs import extract_last_output_value
from .paths import package_path
from .summary import build_summary_payload
from .summary import write_summary
from .tooling import run_format_check
from .tooling import run_inspect_editor_tooling_check
from .tooling import run_integrated_validation_check
from .tooling import run_workspace_check


__all__ = [
    "CONTRACT_PATH",
    "PACKAGE_CONTRACT_ID",
    "PACKAGE_PS1",
    "PWSH",
    "REPORT_PATH",
    "ROOT",
    "RUNNER_PATH",
    "SUMMARY_CONTRACT_ID",
    "build_summary_payload",
    "expect",
    "extract_last_output_value",
    "extract_output_value",
    "extract_report_paths",
    "load_and_validate_manifest",
    "load_json",
    "main",
    "normalize_rel_path",
    "package_path",
    "public_workflow_command",
    "repo_rel",
    "run_capture",
    "run_format_check",
    "run_inspect_editor_tooling_check",
    "run_integrated_validation_check",
    "run_package_command",
    "run_workspace_check",
    "validate_manifest_contract",
    "write_json_file",
    "write_summary",
]

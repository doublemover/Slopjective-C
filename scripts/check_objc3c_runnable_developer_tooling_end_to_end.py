#!/usr/bin/env python3
"""Validate packaged developer-tooling behavior end to end from the staged package root."""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SCRIPT_ROOT = ROOT / "scripts"
for import_root in (ROOT, SCRIPT_ROOT):
    import_root_text = str(import_root)
    if import_root_text not in sys.path:
        sys.path.insert(0, import_root_text)

try:
    from scripts.check_objc3c_runnable_developer_tooling_end_to_end import (
        CONTRACT_PATH,
        PACKAGE_CONTRACT_ID,
        PACKAGE_PS1,
        PWSH,
        REPORT_PATH,
        ROOT,
        RUNNER_PATH,
        SUMMARY_CONTRACT_ID,
        build_summary_payload,
        expect,
        extract_last_output_value,
        extract_output_value,
        extract_report_paths,
        load_and_validate_manifest,
        load_json,
        main,
        normalize_rel_path,
        package_path,
        public_workflow_command,
        repo_rel,
        run_capture,
        run_format_check,
        run_inspect_editor_tooling_check,
        run_integrated_validation_check,
        run_package_command,
        run_workspace_check,
        validate_manifest_contract,
        write_json_file,
        write_summary,
    )
except ModuleNotFoundError:
    from check_objc3c_runnable_developer_tooling_end_to_end import (
        CONTRACT_PATH,
        PACKAGE_CONTRACT_ID,
        PACKAGE_PS1,
        PWSH,
        REPORT_PATH,
        ROOT,
        RUNNER_PATH,
        SUMMARY_CONTRACT_ID,
        build_summary_payload,
        expect,
        extract_last_output_value,
        extract_output_value,
        extract_report_paths,
        load_and_validate_manifest,
        load_json,
        main,
        normalize_rel_path,
        package_path,
        public_workflow_command,
        repo_rel,
        run_capture,
        run_format_check,
        run_inspect_editor_tooling_check,
        run_integrated_validation_check,
        run_package_command,
        run_workspace_check,
        validate_manifest_contract,
        write_json_file,
        write_summary,
    )


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


if __name__ == "__main__":
    raise SystemExit(main())

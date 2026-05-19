#!/usr/bin/env python3
"""Validate runnable error execution end to end from the staged package root."""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SCRIPT_ROOT = ROOT / "scripts"
for import_root in (SCRIPT_ROOT, ROOT):
    import_root_text = str(import_root)
    if import_root_text not in sys.path:
        sys.path.insert(0, import_root_text)

from check_objc3c_runnable_error_end_to_end import (
    EXPECTED_PROBE_INTEGER_FIELDS,
    PACKAGE_CONTRACT_ID,
    PACKAGE_PS1,
    PWSH,
    REPORT_PATH,
    REQUIRED_MANIFEST_KEYS,
    SUMMARY_CONTRACT_ID,
    assert_compile_artifacts_exist,
    assert_probe_payload,
    build_summary_payload,
    compile_artifact_paths,
    compile_error_fixture,
    compile_packaged_error_probe,
    expect,
    extract_output_value,
    extract_report_paths,
    find_clangxx,
    load_json,
    main,
    manifest_path,
    normalize_rel_path,
    package_runnable_toolchain,
    parse_json_output,
    repo_rel,
    run_capture,
    run_packaged_error_probe,
    run_packaged_execution_replay,
    run_packaged_execution_smoke,
    validate_package_manifest,
    write_json_file,
    write_summary_report,
)

__all__ = [
    "EXPECTED_PROBE_INTEGER_FIELDS",
    "PACKAGE_CONTRACT_ID",
    "PACKAGE_PS1",
    "PWSH",
    "REPORT_PATH",
    "REQUIRED_MANIFEST_KEYS",
    "ROOT",
    "SCRIPT_ROOT",
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


if __name__ == "__main__":
    raise SystemExit(main())

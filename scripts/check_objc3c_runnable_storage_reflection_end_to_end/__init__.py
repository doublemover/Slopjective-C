"""Public import surface for the runnable storage/reflection E2E checker."""

from __future__ import annotations

import re
import shutil
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import normalize_rel_path, repo_rel
from objc3c_tooling.probe_compile import find_clangxx
from objc3c_tooling.probe_output import parse_json_output
from objc3c_tooling.public_workflow_output import extract_output_value
from objc3c_tooling.public_workflow_output import extract_report_paths
from objc3c_tooling.subprocesses import run_capture

from .config import (
    MANIFEST_KEYS,
    PACKAGE_CONTRACT_ID,
    PACKAGE_PS1,
    PWSH,
    REPORT_PATH,
    ROOT,
    RUNNER_PATH,
    SUMMARY_CONTRACT_ID,
)
from .expectations import (
    expect,
    expect_compile_artifacts,
    expect_manifest_contract,
    expect_manifest_files,
    expect_probe_payload,
)
from .models import CompileArtifacts, E2ERunPaths, PackagedToolchain
from .pipeline import (
    build_payload,
    build_run_paths,
    compile_artifacts_for,
    compile_packaged_probe,
    compile_storage_fixture,
    load_package_manifest,
    main,
    package_toolchain,
    resolve_packaged_toolchain,
    run_packaged_probe,
    run_packaged_replay,
    run_packaged_smoke,
    run_storage_reflection_end_to_end,
    write_summary,
)

__all__ = [
    "Any",
    "CompileArtifacts",
    "E2ERunPaths",
    "MANIFEST_KEYS",
    "PACKAGE_CONTRACT_ID",
    "PACKAGE_PS1",
    "PWSH",
    "PackagedToolchain",
    "Path",
    "REPORT_PATH",
    "ROOT",
    "RUNNER_PATH",
    "SUMMARY_CONTRACT_ID",
    "Sequence",
    "build_payload",
    "build_run_paths",
    "compile_artifacts_for",
    "compile_packaged_probe",
    "compile_storage_fixture",
    "datetime",
    "expect",
    "expect_compile_artifacts",
    "expect_manifest_contract",
    "expect_manifest_files",
    "expect_probe_payload",
    "extract_output_value",
    "extract_report_paths",
    "find_clangxx",
    "load_json",
    "load_package_manifest",
    "main",
    "normalize_rel_path",
    "package_toolchain",
    "parse_json_output",
    "re",
    "repo_rel",
    "resolve_packaged_toolchain",
    "run_capture",
    "run_packaged_probe",
    "run_packaged_replay",
    "run_packaged_smoke",
    "run_storage_reflection_end_to_end",
    "shutil",
    "sys",
    "timezone",
    "write_json_file",
    "write_summary",
]

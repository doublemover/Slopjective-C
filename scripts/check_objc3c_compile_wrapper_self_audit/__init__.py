"""Public import surface for the objc3c compile-wrapper self-audit checker."""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
from datetime import datetime
from pathlib import Path
from time import perf_counter
from typing import Any

from objc3c_tooling.json_io import require_json_object as load_json

from .audit import (
    build_command,
    build_payload,
    main,
    run_compile_wrapper,
    run_self_audit,
    write_report,
)
from .config import (
    FIXTURE,
    PROVENANCE_CONTRACT_ID,
    REPORT_ROOT,
    ROOT,
    RUN_ROOT,
    SELF_AUDIT_CONTRACT_ID,
    TRUTHFULNESS_CONTRACT_ID,
    WRAPPER,
    WRAPPER_ARTIFACT_OWNER,
    WRAPPER_RESULT_OWNER,
    WRAPPER_STATUS_OWNER,
    WRAPPER_TRUTH_OWNER,
)
from .contracts import wrapper_truth_owner_contract
from .environment import find_pwsh
from .expectations import expect, validate_compile_output
from .paths import repo_display_path

__all__ = [
    "Any",
    "FIXTURE",
    "PROVENANCE_CONTRACT_ID",
    "Path",
    "REPORT_ROOT",
    "ROOT",
    "RUN_ROOT",
    "SELF_AUDIT_CONTRACT_ID",
    "TRUTHFULNESS_CONTRACT_ID",
    "WRAPPER",
    "WRAPPER_ARTIFACT_OWNER",
    "WRAPPER_RESULT_OWNER",
    "WRAPPER_STATUS_OWNER",
    "WRAPPER_TRUTH_OWNER",
    "build_command",
    "build_payload",
    "datetime",
    "expect",
    "find_pwsh",
    "json",
    "load_json",
    "main",
    "perf_counter",
    "repo_display_path",
    "run_compile_wrapper",
    "run_self_audit",
    "shutil",
    "subprocess",
    "sys",
    "validate_compile_output",
    "wrapper_truth_owner_contract",
    "write_report",
]

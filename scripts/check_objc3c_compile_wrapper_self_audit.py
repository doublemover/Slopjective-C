#!/usr/bin/env python3
"""CLI facade for the objc3c compile-wrapper self-audit checker."""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from scripts.check_objc3c_compile_wrapper_self_audit import (  # noqa: E402
    Any,
    FIXTURE,
    PROVENANCE_CONTRACT_ID,
    REPORT_ROOT,
    RUN_ROOT,
    SELF_AUDIT_CONTRACT_ID,
    TRUTHFULNESS_CONTRACT_ID,
    WRAPPER,
    WRAPPER_ARTIFACT_OWNER,
    WRAPPER_RESULT_OWNER,
    WRAPPER_STATUS_OWNER,
    WRAPPER_TRUTH_OWNER,
    build_command,
    build_payload,
    datetime,
    expect,
    find_pwsh,
    json,
    load_json,
    main,
    perf_counter,
    repo_display_path,
    run_compile_wrapper,
    run_self_audit,
    shutil,
    subprocess,
    sys,
    validate_compile_output,
    wrapper_truth_owner_contract,
    write_report,
)
from scripts.check_objc3c_compile_wrapper_self_audit import __all__  # noqa: E402


if __name__ == "__main__":
    raise SystemExit(main())

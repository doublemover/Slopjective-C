#!/usr/bin/env python3
"""Validate runnable interop conformance against the integrated live workflow."""

from __future__ import annotations

from check_objc3c_runnable_interop_conformance import (
    ACCEPTANCE_REPORT,
    INTEGRATION_REPORT,
    REPORT_PATH,
    REQUIRED_CASES,
    REQUIRED_SURFACE_CONTRACTS,
    ROOT,
    SUMMARY_CONTRACT_ID,
    build_live_surfaces,
    build_summary_payload,
    collect_live_results,
    expect,
    integration_surface_comparison,
    load_acceptance_surfaces,
    load_json,
    main,
    report_has_required_inputs,
    runtime_acceptance,
    validate_live_results,
    validate_required_surface_contracts,
    validate_runtime_package_loading_surfaces,
    write_json_file,
    write_summary,
)


__all__ = [
    "ACCEPTANCE_REPORT",
    "INTEGRATION_REPORT",
    "REPORT_PATH",
    "REQUIRED_CASES",
    "REQUIRED_SURFACE_CONTRACTS",
    "ROOT",
    "SUMMARY_CONTRACT_ID",
    "build_live_surfaces",
    "build_summary_payload",
    "collect_live_results",
    "expect",
    "integration_surface_comparison",
    "load_acceptance_surfaces",
    "load_json",
    "main",
    "report_has_required_inputs",
    "runtime_acceptance",
    "validate_live_results",
    "validate_required_surface_contracts",
    "validate_runtime_package_loading_surfaces",
    "write_json_file",
    "write_summary",
]


if __name__ == "__main__":
    raise SystemExit(main())

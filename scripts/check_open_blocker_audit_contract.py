#!/usr/bin/env python3
"""Validate open-blocker audit runner artifacts against the checker contract."""

from __future__ import annotations

try:
    from open_blocker_audit_contract_checker import (
        CHECKER_MODE,
        EXIT_CONTRACT_DRIFT,
        EXIT_OK,
        FINAL_STATUS_TO_EXIT,
        SNAPSHOT_KEYS,
        SUMMARY_KEYS,
        build_output,
        check_key_order,
        load_json,
        main,
        parse_args,
        validate_extract_log,
        validate_snapshot,
        validate_summary,
    )
except ModuleNotFoundError:
    from scripts.open_blocker_audit_contract_checker import (
        CHECKER_MODE,
        EXIT_CONTRACT_DRIFT,
        EXIT_OK,
        FINAL_STATUS_TO_EXIT,
        SNAPSHOT_KEYS,
        SUMMARY_KEYS,
        build_output,
        check_key_order,
        load_json,
        main,
        parse_args,
        validate_extract_log,
        validate_snapshot,
        validate_summary,
    )


__all__ = [
    "CHECKER_MODE",
    "EXIT_CONTRACT_DRIFT",
    "EXIT_OK",
    "FINAL_STATUS_TO_EXIT",
    "SNAPSHOT_KEYS",
    "SUMMARY_KEYS",
    "build_output",
    "check_key_order",
    "load_json",
    "main",
    "parse_args",
    "validate_extract_log",
    "validate_snapshot",
    "validate_summary",
]


if __name__ == "__main__":
    raise SystemExit(main())

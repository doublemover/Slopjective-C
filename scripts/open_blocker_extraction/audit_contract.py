"""Open-blocker audit contract-check result validation."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import display_path


def validate_contract_check_result(
    result: Any,
    *,
    summary_json_path: Path,
    snapshot_json_path: Path,
    extract_log_path: Path,
    checker_mode: str,
    runner_contract_id: str,
    runner_contract_version: str,
    exit_ok: int,
) -> list[str]:
    errors: list[str] = []

    if result.exit_code != exit_ok:
        errors.append(
            "check_open_blocker_audit_contract returned unexpected exit code "
            f"{result.exit_code}."
        )
        if not result.stderr.strip():
            errors.append(
                "check_open_blocker_audit_contract exited non-zero without stderr diagnostics."
            )
        return errors

    if result.stderr:
        errors.append(
            "check_open_blocker_audit_contract emitted stderr despite exit code 0."
        )

    try:
        payload = json.loads(result.stdout)
    except json.JSONDecodeError as exc:
        errors.append(
            "check_open_blocker_audit_contract emitted invalid JSON: "
            f"{exc.msg} at {exc.lineno}:{exc.colno}."
        )
        return errors

    if not isinstance(payload, dict):
        errors.append("check_open_blocker_audit_contract output root must be an object.")
        return errors

    if payload.get("mode") != checker_mode:
        errors.append(
            "check_open_blocker_audit_contract output mode drift: "
            f"expected={checker_mode!r} observed={payload.get('mode')!r}."
        )

    contract = payload.get("contract")
    if not isinstance(contract, dict):
        errors.append("check_open_blocker_audit_contract output.contract must be an object.")
    else:
        expected_runner = f"{runner_contract_id}/{runner_contract_version}"
        if contract.get("expected_runner") != expected_runner:
            errors.append(
                "check_open_blocker_audit_contract expected_runner drift: "
                f"expected={expected_runner!r} observed={contract.get('expected_runner')!r}."
            )
        if contract.get("contract_id") != runner_contract_id:
            errors.append(
                "check_open_blocker_audit_contract contract_id drift: "
                f"expected={runner_contract_id!r} observed={contract.get('contract_id')!r}."
            )
        if contract.get("contract_version") != runner_contract_version:
            errors.append(
                "check_open_blocker_audit_contract contract_version drift: "
                f"expected={runner_contract_version!r} observed={contract.get('contract_version')!r}."
            )

    artifacts = payload.get("artifacts")
    if not isinstance(artifacts, dict):
        errors.append("check_open_blocker_audit_contract output.artifacts must be an object.")
    else:
        expected_summary = display_path(summary_json_path)
        expected_snapshot = display_path(snapshot_json_path)
        expected_extract_log = display_path(extract_log_path)
        if artifacts.get("summary") != expected_summary:
            errors.append(
                "check_open_blocker_audit_contract output.artifacts.summary drift: "
                f"expected={expected_summary!r} observed={artifacts.get('summary')!r}."
            )
        if artifacts.get("snapshot") != expected_snapshot:
            errors.append(
                "check_open_blocker_audit_contract output.artifacts.snapshot drift: "
                f"expected={expected_snapshot!r} observed={artifacts.get('snapshot')!r}."
            )
        if artifacts.get("extract_log") != expected_extract_log:
            errors.append(
                "check_open_blocker_audit_contract output.artifacts.extract_log drift: "
                f"expected={expected_extract_log!r} observed={artifacts.get('extract_log')!r}."
            )

    if payload.get("ok") is not True:
        errors.append(
            "check_open_blocker_audit_contract output.ok must be true when exit code is 0."
        )
    if payload.get("exit_code") != exit_ok:
        errors.append(
            "check_open_blocker_audit_contract output.exit_code drift: "
            f"expected={exit_ok} observed={payload.get('exit_code')!r}."
        )
    if payload.get("finding_count") != 0:
        errors.append(
            "check_open_blocker_audit_contract output.finding_count drift: "
            f"expected=0 observed={payload.get('finding_count')!r}."
        )

    findings = payload.get("findings")
    if not isinstance(findings, list):
        errors.append("check_open_blocker_audit_contract output.findings must be a list.")
    elif findings:
        errors.append(
            "check_open_blocker_audit_contract output.findings must be empty on success."
        )

    return errors


__all__ = ["validate_contract_check_result"]

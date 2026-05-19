from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.paths import display_path

from .constants import FINAL_STATUS_TO_EXIT, SUMMARY_KEYS
from .key_order import check_key_order


def validate_summary(
    summary: dict[str, Any],
    *,
    expected_runner: str,
    contract_id: str,
    contract_version: str,
    summary_path: Path,
    snapshot_path: Path,
    extract_log_path: Path,
) -> list[str]:
    findings = check_key_order(summary, expected=SUMMARY_KEYS, label="summary")

    if summary.get("runner") != expected_runner:
        findings.append(
            "summary.runner drift: "
            f"expected={expected_runner!r} observed={summary.get('runner')!r}."
        )
    if summary.get("contract_id") != contract_id:
        findings.append(
            "summary.contract_id drift: "
            f"expected={contract_id!r} observed={summary.get('contract_id')!r}."
        )
    if summary.get("contract_version") != contract_version:
        findings.append(
            "summary.contract_version drift: "
            f"expected={contract_version!r} "
            f"observed={summary.get('contract_version')!r}."
        )

    artifacts = summary.get("artifacts")
    if not isinstance(artifacts, dict):
        findings.append("summary.artifacts must be an object.")
    else:
        expected_artifacts = {
            "summary_json": summary_path.name,
            "snapshot_json": display_path(snapshot_path),
            "extract_log": extract_log_path.name,
        }
        for key, expected_value in expected_artifacts.items():
            observed = artifacts.get(key)
            if observed != expected_value:
                findings.append(
                    f"summary.artifacts.{key} drift: "
                    f"expected={expected_value!r} observed={observed!r}."
                )

    final_status = summary.get("final_status")
    final_exit_code = summary.get("final_exit_code")
    expected_exit_code = FINAL_STATUS_TO_EXIT.get(final_status)
    if expected_exit_code is None:
        findings.append(f"summary.final_status is invalid: {final_status!r}.")
    elif final_exit_code != expected_exit_code:
        findings.append(
            "summary final status/exit mismatch: "
            f"status={final_status!r} expected_exit={expected_exit_code} "
            f"observed_exit={final_exit_code!r}."
        )

    audit = summary.get("audit")
    if not isinstance(audit, dict):
        findings.append("summary.audit must be an object.")
    elif audit.get("extract_exit_code") != 0:
        findings.append(
            "summary.audit.extract_exit_code drift: "
            f"expected=0 observed={audit.get('extract_exit_code')!r}."
        )

    return findings

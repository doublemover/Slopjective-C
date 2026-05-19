from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.paths import display_path

from .constants import CHECKER_MODE, EXIT_CONTRACT_DRIFT, EXIT_OK


def build_output(
    *,
    expected_runner: str,
    contract_id: str,
    contract_version: str,
    summary_path: Path,
    snapshot_path: Path,
    extract_log_path: Path,
    findings: list[str],
) -> dict[str, Any]:
    exit_code = EXIT_OK if not findings else EXIT_CONTRACT_DRIFT
    return {
        "mode": CHECKER_MODE,
        "contract": {
            "expected_runner": expected_runner,
            "contract_id": contract_id,
            "contract_version": contract_version,
        },
        "artifacts": {
            "summary": display_path(summary_path),
            "snapshot": display_path(snapshot_path),
            "extract_log": display_path(extract_log_path),
        },
        "ok": not findings,
        "exit_code": exit_code,
        "finding_count": len(findings),
        "findings": findings,
    }

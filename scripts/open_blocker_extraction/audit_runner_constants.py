"""Open-blocker audit runner constants."""

from __future__ import annotations

from pathlib import Path


DEFAULT_SNAPSHOT_RELATIVE_PATH = Path("inputs") / "open_blockers.snapshot.json"
EXTRACT_LOG_FILENAME = "extract_open_blockers.log"
SUMMARY_JSON_FILENAME = "open_blocker_audit_summary.json"
REPORT_MD_FILENAME = "open_blocker_audit_report.md"
CONTRACT_CHECK_TRANSCRIPT_FILENAME = "open_blocker_audit_contract_check_transcript.txt"
CONTRACT_CHECK_STDERR_FILENAME = "open_blocker_audit_contract_check.stderr.txt"
RUNNER_CONTRACT_ID = "open-blocker-audit-runner"
RUNNER_CONTRACT_VERSION = "v0.1"
RUNNER_ID = f"{RUNNER_CONTRACT_ID}/{RUNNER_CONTRACT_VERSION}"
CHECKER_MODE = "open-blocker-audit-contract-v1"
DEFAULT_COMMAND_TIMEOUT_SECONDS = 600

EXIT_OK = 0
EXIT_OPEN_BLOCKERS = 1
EXIT_RUNNER_ERROR = 2

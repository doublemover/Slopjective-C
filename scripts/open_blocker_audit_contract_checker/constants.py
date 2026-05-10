from __future__ import annotations


CHECKER_MODE = "open-blocker-audit-contract-v1"
EXIT_OK = 0
EXIT_CONTRACT_DRIFT = 2

SUMMARY_KEYS = [
    "runner",
    "contract_id",
    "contract_version",
    "inputs",
    "scope",
    "artifacts",
    "audit",
    "commands",
    "errors",
    "final_status",
    "final_exit_code",
]
SNAPSHOT_KEYS = [
    "contract_id",
    "contract_version",
    "generated_at_utc",
    "source",
    "open_blocker_count",
    "open_blockers",
]
FINAL_STATUS_TO_EXIT = {
    "ok": 0,
    "open-blockers": 1,
    "runner-error": 2,
}

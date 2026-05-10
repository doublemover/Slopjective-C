"""Open-blocker audit runner payload shaping."""

from __future__ import annotations

from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import display_path

from .audit_runner_commands import summarize_command
from .audit_runner_constants import (
    EXIT_OK,
    EXIT_OPEN_BLOCKERS,
    EXIT_RUNNER_ERROR,
    EXTRACT_LOG_FILENAME,
    REPORT_MD_FILENAME,
    RUNNER_CONTRACT_ID,
    RUNNER_CONTRACT_VERSION,
    RUNNER_ID,
    SUMMARY_JSON_FILENAME,
)
from .audit_runner_models import CommandResult


def build_runner_snapshot_payload(
    extract_payload: dict[str, object],
) -> dict[str, object]:
    return {
        "contract_id": RUNNER_CONTRACT_ID,
        "contract_version": RUNNER_CONTRACT_VERSION,
        "generated_at_utc": extract_payload["generated_at_utc"],
        "source": extract_payload["source"],
        "open_blocker_count": extract_payload["open_blocker_count"],
        "open_blockers": extract_payload["open_blockers"],
    }


def determine_final_exit(*, errors: Sequence[str], blocker_count: int | None) -> tuple[int, str]:
    if errors:
        return EXIT_RUNNER_ERROR, "runner-error"
    if blocker_count is None:
        return EXIT_RUNNER_ERROR, "runner-error"
    if blocker_count > 0:
        return EXIT_OPEN_BLOCKERS, "open-blockers"
    return EXIT_OK, "ok"


def build_summary_payload(
    *,
    audit_root: Path,
    effective_audit_root: Path,
    include_globs: Sequence[str],
    exclude_paths: Sequence[str],
    extractor_exclude_paths: Sequence[str],
    generated_at_utc: str | None,
    source: str | None,
    output_dir: Path,
    snapshot_json_path: Path,
    included_markdown_paths: Sequence[str],
    excluded_markdown_paths: Sequence[str],
    extract_result: CommandResult | None,
    blocker_count: int | None,
    errors: Sequence[str],
    final_status: str,
    final_exit_code: int,
) -> dict[str, object]:
    commands: dict[str, object] = {}
    if extract_result is not None:
        commands["extract_open_blockers_snapshot_json"] = summarize_command(
            extract_result
        )

    return {
        "runner": RUNNER_ID,
        "contract_id": RUNNER_CONTRACT_ID,
        "contract_version": RUNNER_CONTRACT_VERSION,
        "inputs": {
            "audit_root": display_path(audit_root),
            "effective_audit_root": display_path(effective_audit_root),
            "include_globs": list(include_globs),
            "exclude_paths": list(exclude_paths),
            "extractor_exclude_paths": list(extractor_exclude_paths),
            "generated_at_utc": generated_at_utc,
            "source": source,
        },
        "scope": {
            "included_markdown_count": len(included_markdown_paths),
            "excluded_markdown_count": len(excluded_markdown_paths),
        },
        "artifacts": {
            "output_dir": display_path(output_dir),
            "snapshot_json": display_path(snapshot_json_path),
            "extract_log": EXTRACT_LOG_FILENAME if extract_result is not None else None,
            "summary_json": SUMMARY_JSON_FILENAME,
            "report_markdown": REPORT_MD_FILENAME,
        },
        "audit": {
            "extract_attempted": extract_result is not None,
            "extract_exit_code": extract_result.exit_code if extract_result is not None else None,
            "open_blocker_count": blocker_count,
        },
        "commands": commands,
        "errors": list(errors),
        "final_status": final_status,
        "final_exit_code": final_exit_code,
    }

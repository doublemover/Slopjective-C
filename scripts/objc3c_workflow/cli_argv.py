"""CLI argv normalization for objc3c workflow entrypoints."""

from __future__ import annotations

from collections.abc import Sequence

from .cli_argv_source import (
    CLI_ARGV_SOURCE_CONTRACT_ID,
    CLI_ARGV_SOURCE_OWNER_SURFACE,
    process_argv,
)

CLI_ARGV_NORMALIZATION_CONTRACT_ID = "objc3c-workflow-cli-argv-normalization-v1"
CLI_ARGV_NORMALIZATION_OWNER_SURFACE = "scripts/objc3c_workflow/cli_argv.py"


def workflow_argv_contract_payload() -> dict[str, object]:
    return {
        "contract_id": CLI_ARGV_NORMALIZATION_CONTRACT_ID,
        "owner_surface": CLI_ARGV_NORMALIZATION_OWNER_SURFACE,
        "argv_source_contract_id": CLI_ARGV_SOURCE_CONTRACT_ID,
        "argv_source_owner_surface": CLI_ARGV_SOURCE_OWNER_SURFACE,
        "explicit_argv_is_copied": True,
        "process_argv_is_copied": True,
        "retired_passthrough_mutation_allowed": False,
    }


def workflow_argv(argv: Sequence[str] | None = None) -> list[str]:
    return process_argv() if argv is None else list(argv)


__all__ = [
    "CLI_ARGV_NORMALIZATION_CONTRACT_ID",
    "CLI_ARGV_NORMALIZATION_OWNER_SURFACE",
    "process_argv",
    "workflow_argv",
    "workflow_argv_contract_payload",
]

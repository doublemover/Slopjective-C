"""Process argv source for workflow CLI entrypoints."""

from __future__ import annotations

import sys

CLI_ARGV_SOURCE_CONTRACT_ID = "objc3c-workflow-cli-argv-source-v1"
CLI_ARGV_SOURCE_OWNER_SURFACE = "scripts/objc3c_workflow/cli_argv_source.py"


def process_argv_contract_payload() -> dict[str, object]:
    return {
        "contract_id": CLI_ARGV_SOURCE_CONTRACT_ID,
        "owner_surface": CLI_ARGV_SOURCE_OWNER_SURFACE,
        "source": "sys.argv[1:]",
        "normalization_owner_surface": "scripts/objc3c_workflow/cli_argv.py",
        "retired_direct_script_arguments_allowed": False,
    }


def process_argv() -> list[str]:
    return list(sys.argv[1:])


__all__ = [
    "CLI_ARGV_SOURCE_CONTRACT_ID",
    "CLI_ARGV_SOURCE_OWNER_SURFACE",
    "process_argv",
    "process_argv_contract_payload",
]

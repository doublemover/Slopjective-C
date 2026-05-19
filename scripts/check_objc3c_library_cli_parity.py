#!/usr/bin/env python3
"""Compare library and CLI artifact outputs for deterministic parity."""

from __future__ import annotations

from objc3c_library_cli_parity.artifacts import DEFAULT_DIMENSION_MAP
from objc3c_library_cli_parity.artifacts import build_dimension_results
from objc3c_library_cli_parity.artifacts import default_dimension_map_for_emit_prefix
from objc3c_library_cli_parity.artifacts import ensure_directory
from objc3c_library_cli_parity.artifacts import normalize_artifact_name
from objc3c_library_cli_parity.artifacts import normalize_artifacts
from objc3c_library_cli_parity.artifacts import parse_dimension_map
from objc3c_library_cli_parity.artifacts import resolve_artifact_digest
from objc3c_library_cli_parity.artifacts import sha256_text
from objc3c_library_cli_parity.cli import DEFAULT_ARTIFACTS
from objc3c_library_cli_parity.cli import MODE
from objc3c_library_cli_parity.cli import main
from objc3c_library_cli_parity.cli import parse_args
from objc3c_library_cli_parity.cli import run
from objc3c_library_cli_parity.fixtures import synthetic_fixture_contract_payload
from objc3c_library_cli_parity.fixtures import synthetic_fixture_manifest_envelope
from objc3c_library_cli_parity.fixtures import synthetic_fixture_summary_envelope
from objc3c_library_cli_parity.fixtures import SYNTHETIC_FIXTURE_FAMILY_ID
from objc3c_library_cli_parity.fixtures import SYNTHETIC_FIXTURE_LABEL
from objc3c_library_cli_parity.fixtures import validate_synthetic_fixture_contract
from objc3c_library_cli_parity.source_mode import build_source_mode_artifacts
from objc3c_library_cli_parity.source_mode import prepare_source_mode
from objc3c_library_cli_parity.subprocesses import CommandResult

__all__ = [
    "DEFAULT_ARTIFACTS",
    "DEFAULT_DIMENSION_MAP",
    "MODE",
    "CommandResult",
    "build_dimension_results",
    "build_source_mode_artifacts",
    "default_dimension_map_for_emit_prefix",
    "ensure_directory",
    "main",
    "normalize_artifact_name",
    "normalize_artifacts",
    "parse_args",
    "parse_dimension_map",
    "prepare_source_mode",
    "resolve_artifact_digest",
    "run",
    "sha256_text",
    "SYNTHETIC_FIXTURE_FAMILY_ID",
    "SYNTHETIC_FIXTURE_LABEL",
    "synthetic_fixture_contract_payload",
    "synthetic_fixture_manifest_envelope",
    "synthetic_fixture_summary_envelope",
    "validate_synthetic_fixture_contract",
]


if __name__ == "__main__":
    main()

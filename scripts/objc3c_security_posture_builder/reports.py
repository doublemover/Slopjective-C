"""Required report loading for the security posture builder."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.subprocesses import python_script_command, run_capture

from .constants import (
    ARTIFACT_CONTRACT_SUMMARY,
    ARTIFACT_CONTRACT_SUMMARY_BUILD,
    MACRO_SUMMARY,
    MACRO_SUMMARY_BUILD,
    RELEASE_KEY_SUMMARY,
    RELEASE_KEY_SUMMARY_BUILD,
    RESPONSE_SUMMARY,
    RESPONSE_SUMMARY_BUILD,
    RUNTIME_HARDENING_CHECK,
    RUNTIME_HARDENING_SUMMARY,
    SCHEMA_CHECK,
    SCHEMA_SUMMARY,
    SOURCE_CHECK,
    SOURCE_SUMMARY,
    SUPPLY_CHAIN_AUDIT,
    SUPPLY_CHAIN_SUMMARY,
)

REQUIRED_REPORTS = {
    "source": (SOURCE_SUMMARY, SOURCE_CHECK),
    "schema": (SCHEMA_SUMMARY, SCHEMA_CHECK),
    "response": (RESPONSE_SUMMARY, RESPONSE_SUMMARY_BUILD),
    "macro": (MACRO_SUMMARY, MACRO_SUMMARY_BUILD),
    "release_key": (RELEASE_KEY_SUMMARY, RELEASE_KEY_SUMMARY_BUILD),
    "artifact_contract": (ARTIFACT_CONTRACT_SUMMARY, ARTIFACT_CONTRACT_SUMMARY_BUILD),
    "supply_chain": (SUPPLY_CHAIN_SUMMARY, SUPPLY_CHAIN_AUDIT),
    "runtime_hardening": (RUNTIME_HARDENING_SUMMARY, RUNTIME_HARDENING_CHECK),
}


def ensure_success(path: Path, script: Path) -> dict[str, Any]:
    if not path.is_file():
        result = run_capture(python_script_command(script))
        if result.returncode != 0:
            raise RuntimeError(f"failed to build required report via {repo_rel(script)}")
    payload = load_json(path)
    if payload.get("status") not in {"PASS", "OK"} and payload.get("ok") is not True:
        raise RuntimeError(f"required report did not pass: {repo_rel(path)}")
    return payload


def load_required_reports() -> dict[str, dict[str, Any]]:
    return {
        name: ensure_success(path, script)
        for name, (path, script) in REQUIRED_REPORTS.items()
    }

"""Catalog metadata for the shared executable runtime acceptance harness."""

from __future__ import annotations

from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.paths import repo_rel
from objc3c_tooling.subprocesses import python_script_command_tuple
from scripts.objc3c_workflow.public_command_api import public_workflow_command_tuple
from shared_compiler_runtime_acceptance_catalog_surfaces import (
    AUTHORITATIVE_CHILD_REPORT_CONTRACTS,
    COMMON_SURFACES,
    COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID,
    COMPILE_PROVENANCE_CONTRACT_ID,
    SurfaceRequirement,
)


ROOT = Path(__file__).resolve().parents[1]
HARNESS_REPORT_ROOT = (
    ROOT / "tmp" / "reports" / "runtime" / "shared-executable-acceptance-harness"
)
HARNESS_CONTRACT_ID = "objc3c.shared.executable.acceptance.harness.v1"
HARNESS_SUMMARY_CONTRACT_ID = "objc3c.shared.executable.acceptance.harness.summary.v1"


@dataclass(frozen=True)
class SuiteEntry:
    suite_id: str
    summary: str
    execution_kind: str
    command: tuple[str, ...]
    report_path: str
    guarantee_owner: str
    validation_owner: str
    required_surfaces: tuple[SurfaceRequirement, ...]


SUITES: tuple[SuiteEntry, ...] = (
    SuiteEntry(
        suite_id="runtime-acceptance",
        summary="direct runtime acceptance suite over the live compile and runtime path",
        execution_kind="direct-executable-suite",
        command=python_script_command_tuple("scripts/check_objc3c_runtime_acceptance.py"),
        report_path="tmp/reports/runtime/acceptance/summary.json",
        guarantee_owner="runtime acceptance and compile-coupled runtime publication proof",
        validation_owner="scripts/check_objc3c_runtime_acceptance.py",
        required_surfaces=COMMON_SURFACES,
    ),
    SuiteEntry(
        suite_id="public-test-smoke",
        summary="composite smoke public workflow carrying runtime acceptance surfaces forward",
        execution_kind="composite-executable-suite",
        command=public_workflow_command_tuple("test-smoke"),
        report_path="tmp/reports/objc3c-public-workflow/test-smoke.json",
        guarantee_owner="behavior smoke, runtime acceptance, and replay through the public entrypoint",
        validation_owner="scripts.objc3c_workflow",
        required_surfaces=COMMON_SURFACES,
    ),
    SuiteEntry(
        suite_id="public-test-full",
        summary="composite full developer workflow carrying runtime acceptance surfaces forward",
        execution_kind="composite-executable-suite",
        command=public_workflow_command_tuple("test-full"),
        report_path="tmp/reports/objc3c-public-workflow/test-full.json",
        guarantee_owner="full developer validation without recovery fan-out",
        validation_owner="scripts.objc3c_workflow",
        required_surfaces=COMMON_SURFACES,
    ),
)

SUITE_MAP = {entry.suite_id: entry for entry in SUITES}


def build_harness_surface(selected: Sequence[SuiteEntry]) -> dict[str, Any]:
    return {
        "contract_id": HARNESS_CONTRACT_ID,
        "harness_path": "scripts/shared_compiler_runtime_acceptance_harness.py",
        "suite_ids": [entry.suite_id for entry in selected],
        "requires_real_executable_suite_reports": True,
        "authoritative_child_report_contracts": AUTHORITATIVE_CHILD_REPORT_CONTRACTS,
        "shared_compile_truth_contracts": [
            COMPILE_PROVENANCE_CONTRACT_ID,
            COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID,
        ],
    }


def build_catalog_payload(selected: Sequence[SuiteEntry]) -> dict[str, Any]:
    return {
        "contract_id": HARNESS_CONTRACT_ID,
        "harness_path": "scripts/shared_compiler_runtime_acceptance_harness.py",
        "suite_count": len(selected),
        "suites": [asdict(entry) for entry in selected],
    }


def check_catalog(selected: Sequence[SuiteEntry]) -> dict[str, Any]:
    suite_results: list[dict[str, Any]] = []
    ok = True
    for entry in selected:
        command_results = []
        for token in entry.command[1:]:
            candidate = ROOT / token
            if candidate.exists():
                command_results.append({"path": token, "exists": True})
            elif token.startswith("scripts/") or token.startswith("tests/"):
                command_results.append({"path": token, "exists": False})
                ok = False
        report_parent = (ROOT / entry.report_path).parent
        report_parent_exists = report_parent.exists()
        ok = ok and report_parent_exists
        suite_results.append(
            {
                "suite_id": entry.suite_id,
                "command_paths": command_results,
                "report_parent": repo_rel(report_parent),
                "report_parent_exists": report_parent_exists,
            }
        )
    return {"ok": ok, "suite_results": suite_results}


__all__ = [
    "COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID",
    "COMPILE_PROVENANCE_CONTRACT_ID",
    "HARNESS_CONTRACT_ID",
    "HARNESS_REPORT_ROOT",
    "HARNESS_SUMMARY_CONTRACT_ID",
    "SUITE_MAP",
    "SUITES",
    "SuiteEntry",
    "SurfaceRequirement",
    "build_catalog_payload",
    "build_harness_surface",
    "check_catalog",
]

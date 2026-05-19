"""Dependency summary loading for release/runtime claim matrix publication."""

from __future__ import annotations

from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.paths import display_path

from .paths import RELEASE_CLAIMS_ROOT


DEPENDENCY_SUMMARY_FILENAMES: dict[str, str] = {
    "objc3c.releaseclaims.frontendfeaturetruth.surface.v1": (
        "frontend_feature_claim_and_strictness_truthfulness_wiring_summary.json"
    ),
    "objc3c.releaseclaims.canonicalinterface.featuremacro.v1": (
        "canonical_interface_and_feature_macro_truthfulness_summary.json"
    ),
    "objc3c.releaseclaims.runtimecapabilityreporting.surface.v1": (
        "machine_readable_runtime_capability_reporting_summary.json"
    ),
    "objc3c.releaseclaims.toolchainconformance.surface.v1": (
        "cli_and_toolchain_conformance_claim_operations_summary.json"
    ),
    "objc3c.releaseclaims.versioningtruthgate.surface.v1": (
        "versioning_and_conformance_truth_gate_summary.json"
    ),
}


def find_summary_by_name(filename: str):
    matches = sorted(RELEASE_CLAIMS_ROOT.rglob(filename))
    if not matches:
        raise SystemExit(f"missing summary artifact: {filename}")
    if len(matches) > 1:
        rendered = ", ".join(display_path(path) for path in matches)
        raise SystemExit(f"ambiguous summary artifact {filename}: {rendered}")
    return matches[0]


def summary_status(payload: dict[str, Any]) -> bool:
    if payload.get("ok") is True:
        return True
    return (
        isinstance(payload.get("checks_total"), int)
        and isinstance(payload.get("checks_passed"), int)
        and payload.get("checks_total") == payload.get("checks_passed")
        and not payload.get("failures")
    )


def load_dependency_cases() -> dict[str, dict[str, Any]]:
    dependency_cases: dict[str, dict[str, Any]] = {
        contract_id: {"summary_path": find_summary_by_name(filename)}
        for contract_id, filename in DEPENDENCY_SUMMARY_FILENAMES.items()
    }
    for case in dependency_cases.values():
        case["payload"] = load_json(case["summary_path"])
    for name, case in dependency_cases.items():
        if not summary_status(case["payload"]):
            raise SystemExit(f"{name} summary is not green: {display_path(case['summary_path'])}")
    return dependency_cases

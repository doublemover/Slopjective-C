#!/usr/bin/env python3
"""Fail-closed checker for M275-D001 advanced-feature operator contract."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m275-d001-advanced-feature-ops-contract-v1"
CONTRACT_ID = "objc3c-advanced-feature-ci-runbook-dashboard-contract/m275-d001-v1"
REPORTING_CONTRACT_ID = "objc3c-part12-feature-aware-conformance-report-emission/m275-c002-v1"
RELEASE_EVIDENCE_CONTRACT_ID = "objc3c-part12-corpus-sharding-release-evidence-packaging/m275-c003-v1"
GATE_SCRIPT_PATH = "scripts/check_release_evidence.py"
RUNBOOK_PATH = "spec/conformance/release_evidence_gate_maintenance.md"
DASHBOARD_SCHEMA_PATH = "schemas/objc3-conformance-dashboard-status-v1.schema.json"
TARGETED_PROFILES = ["strict", "strict-concurrency", "strict-system"]
EXPECTED_SUMMARY = ROOT / "tmp" / "reports" / "m275" / "M275-D001" / "ci_runbook_dashboard_contract_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m275_d001_advanced_feature_ops_contract_positive.objc3"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m275_ci_runbook_and_dashboard_contract_and_architecture_freeze_d001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m275" / "m275_d001_ci_runbook_and_dashboard_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_CHECKLIST = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
SPEC_CORE = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
PACKAGE_JSON = ROOT / "package.json"
PROCESS_H = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
RUNTIME_BOOTSTRAP_H = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=EXPECTED_SUMMARY)
    parser.add_argument("--native-exe", type=Path, default=NATIVE_EXE)
    parser.add_argument("--runner-exe", type=Path, default=RUNNER_EXE)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M275-D001-MISSING", f"missing artifact: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_command(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=str(ROOT),
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected object JSON in {display_path(path)}")
    return payload


def validate_publication_artifact(payload: dict[str, Any], artifact: str, expected_surface_kind: str, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    expected = {
        "advanced_feature_ops_contract_id": CONTRACT_ID,
        "advanced_feature_reporting_contract_id": REPORTING_CONTRACT_ID,
        "advanced_feature_release_evidence_contract_id": RELEASE_EVIDENCE_CONTRACT_ID,
        "ci_release_evidence_gate_script_path": GATE_SCRIPT_PATH,
        "runbook_reference_path": RUNBOOK_PATH,
        "dashboard_schema_path": DASHBOARD_SCHEMA_PATH,
        "publication_surface_kind": expected_surface_kind,
        "selected_profile": "core",
        "report_artifact": "module.objc3-conformance-report.json",
    }
    for key, expected_value in expected.items():
        checks_total += 1
        checks_passed += require(payload.get(key) == expected_value, artifact, f"M275-D001-PUB-{expected_surface_kind}-{key}", f"{key} mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("advanced_feature_targeted_profile_ids") == TARGETED_PROFILES, artifact, f"M275-D001-PUB-{expected_surface_kind}-profiles", "advanced_feature_targeted_profile_ids mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("ready") is True, artifact, f"M275-D001-PUB-{expected_surface_kind}-ready", "ready mismatch", failures)
    return checks_total, checks_passed


def validate_validation_artifact(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    expected = {
        "contract_id": "objc3c-toolchain-conformance-claim-operations/m264-d002-v1",
        "schema_id": "objc3c-driver-conformance-validation-v1",
        "advanced_feature_ops_contract_id": CONTRACT_ID,
        "advanced_feature_reporting_contract_id": REPORTING_CONTRACT_ID,
        "advanced_feature_release_evidence_contract_id": RELEASE_EVIDENCE_CONTRACT_ID,
        "ci_release_evidence_gate_script_path": GATE_SCRIPT_PATH,
        "runbook_reference_path": RUNBOOK_PATH,
        "dashboard_schema_path": DASHBOARD_SCHEMA_PATH,
        "publication_surface_kind": "native-cli",
        "selected_profile": "core",
    }
    for key, expected_value in expected.items():
        checks_total += 1
        checks_passed += require(payload.get(key) == expected_value, artifact, f"M275-D001-VAL-{key}", f"{key} mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("advanced_feature_targeted_profile_ids") == TARGETED_PROFILES, artifact, "M275-D001-VAL-profiles", "advanced_feature_targeted_profile_ids mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("ready") is True, artifact, "M275-D001-VAL-ready", "ready mismatch", failures)
    return checks_total, checks_passed


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    helper_summary = ROOT / "tmp" / "reports" / "m275" / "M275-D001" / "ensure_build_summary.json"
    build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m275-d001",
        "--summary-out",
        str(helper_summary),
    ])
    checks_total += 1
    checks_passed += require(build.returncode == 0, "ensure_objc3c_native_build.py", "M275-D001-DYN-BUILD", f"fast build failed: {build.stderr or build.stdout}", failures)

    native_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m275" / "d001" / "native"
    native_out.mkdir(parents=True, exist_ok=True)
    emit = run_command([
        str(args.native_exe),
        str(FIXTURE),
        "--out-dir",
        str(native_out),
        "--emit-prefix",
        "module",
        "--emit-objc3-conformance",
        "--emit-objc3-conformance-format",
        "json",
    ])
    checks_total += 1
    checks_passed += require(emit.returncode == 0, "native-emit", "M275-D001-DYN-NATIVE-EMIT", f"native emit failed: {emit.stderr or emit.stdout}", failures)
    report_path = native_out / "module.objc3-conformance-report.json"
    publication_path = native_out / "module.objc3-conformance-publication.json"
    checks_total += 1
    checks_passed += require(report_path.exists(), display_path(report_path), "M275-D001-DYN-NATIVE-REPORT", "report missing", failures)
    checks_total += 1
    checks_passed += require(publication_path.exists(), display_path(publication_path), "M275-D001-DYN-NATIVE-PUBLICATION", "publication missing", failures)
    if report_path.exists():
        report_payload = load_json(report_path)
        checks_total += 1
        checks_passed += require("advanced_feature_reporting" in report_payload, display_path(report_path), "M275-D001-DYN-REPORT-reporting", "advanced_feature_reporting missing", failures)
        checks_total += 1
        checks_passed += require("advanced_feature_release_evidence" in report_payload, display_path(report_path), "M275-D001-DYN-REPORT-release", "advanced_feature_release_evidence missing", failures)
    if publication_path.exists():
        payload = load_json(publication_path)
        total, passed = validate_publication_artifact(payload, display_path(publication_path), "native-cli", failures)
        checks_total += total
        checks_passed += passed

    validate_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m275" / "d001" / "validate"
    validate_out.mkdir(parents=True, exist_ok=True)
    validate = run_command([
        str(args.native_exe),
        "--validate-objc3-conformance",
        str(report_path),
        "--out-dir",
        str(validate_out),
        "--emit-prefix",
        "module",
        "--emit-objc3-conformance-format",
        "json",
    ])
    checks_total += 1
    checks_passed += require(validate.returncode == 0, "native-validate", "M275-D001-DYN-VALIDATE", f"native validate failed: {validate.stderr or validate.stdout}", failures)
    validation_path = validate_out / "module.objc3-conformance-validation.json"
    checks_total += 1
    checks_passed += require(validation_path.exists(), display_path(validation_path), "M275-D001-DYN-VALIDATION", "validation artifact missing", failures)
    if validation_path.exists():
        payload = load_json(validation_path)
        total, passed = validate_validation_artifact(payload, display_path(validation_path), failures)
        checks_total += total
        checks_passed += passed

    runner_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m275" / "d001" / "frontend"
    runner_out.mkdir(parents=True, exist_ok=True)
    runner = run_command([
        str(args.runner_exe),
        str(FIXTURE),
        "--out-dir",
        str(runner_out),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    checks_total += 1
    checks_passed += require(runner.returncode == 0, "frontend-runner", "M275-D001-DYN-FRONTEND", f"frontend runner failed: {runner.stderr or runner.stdout}", failures)
    frontend_publication_path = runner_out / "module.objc3-conformance-publication.json"
    checks_total += 1
    checks_passed += require(frontend_publication_path.exists(), display_path(frontend_publication_path), "M275-D001-DYN-FRONTEND-PUBLICATION", "frontend publication missing", failures)
    if frontend_publication_path.exists():
        payload = load_json(frontend_publication_path)
        total, passed = validate_publication_artifact(payload, display_path(frontend_publication_path), "frontend-c-api", failures)
        checks_total += total
        checks_passed += passed

    evidence = {
        "build_summary_path": display_path(helper_summary),
        "native_report": display_path(report_path),
        "native_publication": display_path(publication_path),
        "native_validation": display_path(validation_path),
        "frontend_publication": display_path(frontend_publication_path),
    }
    return checks_total, checks_passed, evidence


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    static_snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M275-D001-DOC-CONTRACT", CONTRACT_ID),
            SnippetCheck("M275-D001-DOC-GATE", GATE_SCRIPT_PATH),
        ],
        PACKET_DOC: [
            SnippetCheck("M275-D001-PACKET-ID", "M275-D001"),
            SnippetCheck("M275-D001-PACKET-ISSUE", "M275-D002"),
        ],
        PROCESS_H: [
            SnippetCheck("M275-D001-PROCESS-H-OPS", "advanced_feature_ops_contract_id"),
            SnippetCheck("M275-D001-PROCESS-H-PROFILES", "advanced_feature_targeted_profile_ids"),
        ],
        PROCESS_CPP: [
            SnippetCheck("M275-D001-PROCESS-CPP-CONTRACT", CONTRACT_ID),
            SnippetCheck("M275-D001-PROCESS-CPP-GATE", GATE_SCRIPT_PATH),
            SnippetCheck("M275-D001-PROCESS-CPP-DASHBOARD", DASHBOARD_SCHEMA_PATH),
            SnippetCheck("M275-D001-PROCESS-CPP-BUILD", "TryBuildObjc3ConformanceReportPublicationArtifact"),
            SnippetCheck("M275-D001-PROCESS-CPP-VALIDATE", "TryBuildObjc3ConformanceClaimValidationArtifact"),
        ],
        DRIVER_CPP: [
            SnippetCheck("M275-D001-DRIVER-CPP-CONTRACT", CONTRACT_ID),
            SnippetCheck("M275-D001-DRIVER-CPP-GATE", GATE_SCRIPT_PATH),
        ],
        FRONTEND_ANCHOR_CPP: [
            SnippetCheck("M275-D001-FRONTEND-CONTRACT", CONTRACT_ID),
            SnippetCheck("M275-D001-FRONTEND-DASHBOARD", DASHBOARD_SCHEMA_PATH),
        ],
        RUNTIME_CPP: [
            SnippetCheck("M275-D001-RUNTIME-CONTRACT", CONTRACT_ID),
            SnippetCheck("M275-D001-RUNTIME-REPORTING", REPORTING_CONTRACT_ID),
        ],
        RUNTIME_BOOTSTRAP_H: [
            SnippetCheck("M275-D001-RUNTIME-H-ANCHOR", "publication/validation pair now also carries the advanced-feature CI gate"),
        ],
        DOC_SOURCE: [
            SnippetCheck("M275-D001-DOCSOURCE-TITLE", "## Part 12 CI/runbook/dashboard operator contract (M275-D001)"),
            SnippetCheck("M275-D001-DOCSOURCE-GATE", GATE_SCRIPT_PATH),
        ],
        DOC_NATIVE: [
            SnippetCheck("M275-D001-DOCNATIVE-TITLE", "## Part 12 CI/runbook/dashboard operator contract (M275-D001)"),
            SnippetCheck("M275-D001-DOCNATIVE-RUNBOOK", RUNBOOK_PATH),
        ],
        SPEC_CHECKLIST: [
            SnippetCheck("M275-D001-SPEC-CHECKLIST", "Part 12 CI/runbook/dashboard operator contract"),
        ],
        SPEC_CORE: [
            SnippetCheck("M275-D001-SPEC-CORE", "M275-D001 CI/runbook/dashboard operator contract note:"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M275-D001-PACKAGE-CHECK", '"check:objc3c:m275-d001-ci-runbook-and-dashboard-contract"'),
            SnippetCheck("M275-D001-PACKAGE-READINESS", '"check:objc3c:m275-d001-lane-d-readiness"'),
        ],
    }
    for path, snippets in static_snippets.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_summary: dict[str, Any] = {}
    if args.skip_dynamic_probes:
        dynamic_executed = False
    else:
        dynamic_executed = True
        total, passed, dynamic_summary = run_dynamic_probes(args, failures)
        checks_total += total
        checks_passed += passed

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "dynamic_probes_executed": dynamic_executed,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [finding.__dict__ for finding in failures],
        "evidence": dynamic_summary,
        "next_issue": "M275-D002",
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    if failures:
        print(json.dumps(payload, indent=2))
        return 1
    print(json.dumps(payload, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

#!/usr/bin/env python3
"""Checker for M275-D002 toolchain operations and evidence publication."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m275-d002-release-evidence-publication-v1"
CONTRACT_ID = "objc3c-part12-release-evidence-toolchain-operations/m275-d002-v1"
DASHBOARD_CONTRACT_ID = "objc3c-part12-dashboard-status-publication/m275-d002-v1"
EXPECTED_SUMMARY = ROOT / "tmp" / "reports" / "m275" / "M275-D002" / "toolchain_operations_and_evidence_publication_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m275_d002_release_evidence_publication_positive.objc3"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m275_toolchain_operations_and_evidence_publication_core_feature_implementation_d002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m275" / "m275_d002_toolchain_operations_and_evidence_publication_core_feature_implementation_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_CHECKLIST = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
SPEC_CORE = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
PACKAGE_JSON = ROOT / "package.json"
MANIFEST_H = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.h"
MANIFEST_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
PROCESS_H = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
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
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M275-D002-MISSING", f"missing artifact: {display_path(path)}"))
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


def validate_operation_artifact(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    expected = {
        "contract_id": CONTRACT_ID,
        "schema_id": "objc3c-part12-release-evidence-operation-v1",
        "dashboard_contract_id": DASHBOARD_CONTRACT_ID,
        "gate_script_path": "scripts/check_release_evidence.py",
        "runbook_reference_path": "spec/conformance/release_evidence_gate_maintenance.md",
        "dashboard_schema_path": "schemas/objc3-conformance-dashboard-status-v1.schema.json",
        "release_evidence_checklist_path": "spec/conformance/profile_release_evidence_checklist.md",
        "release_evidence_schema_path": "spec/conformance/objc3_conformance_evidence_bundle_schema.md",
        "release_label": "v0.11",
    }
    for key, expected_value in expected.items():
        checks_total += 1
        checks_passed += require(payload.get(key) == expected_value, artifact, f"M275-D002-OP-{key}", f"{key} mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("command_tokens") == ["python", "scripts/check_release_evidence.py"], artifact, "M275-D002-OP-command", "command_tokens mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("targeted_profile_ids") == ["strict", "strict-concurrency", "strict-system"], artifact, "M275-D002-OP-profiles", "targeted_profile_ids mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("corpus_shard_ids") == ["parser", "semantic", "lowering_abi", "module_roundtrip", "diagnostics"], artifact, "M275-D002-OP-shards", "corpus_shard_ids mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("ready") is True, artifact, "M275-D002-OP-ready", "ready mismatch", failures)
    return checks_total, checks_passed


def validate_dashboard_artifact(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    expected = {
        "contract_id": DASHBOARD_CONTRACT_ID,
        "schema_id": "objc3c-part12-dashboard-status-publication-v1",
        "dashboard_schema_path": "schemas/objc3-conformance-dashboard-status-v1.schema.json",
        "release_label": "v0.11",
        "source_revision": "0000000",
        "gate_script_path": "scripts/check_release_evidence.py",
    }
    for key, expected_value in expected.items():
        checks_total += 1
        checks_passed += require(payload.get(key) == expected_value, artifact, f"M275-D002-DASH-{key}", f"{key} mismatch", failures)
    statuses = payload.get("profile_statuses")
    checks_total += 1
    checks_passed += require(isinstance(statuses, list) and len(statuses) == 4, artifact, "M275-D002-DASH-count", "profile_statuses shape mismatch", failures)
    if isinstance(statuses, list) and len(statuses) == 4:
        by_id = {entry.get("profile_id"): entry.get("status") for entry in statuses if isinstance(entry, dict)}
        checks_total += 4
        checks_passed += require(by_id.get("core") == "pass", artifact, "M275-D002-DASH-core", "core status mismatch", failures)
        checks_passed += require(by_id.get("strict") == "blocked", artifact, "M275-D002-DASH-strict", "strict status mismatch", failures)
        checks_passed += require(by_id.get("strict-concurrency") == "blocked", artifact, "M275-D002-DASH-strict-concurrency", "strict-concurrency status mismatch", failures)
        checks_passed += require(by_id.get("strict-system") == "blocked", artifact, "M275-D002-DASH-strict-system", "strict-system status mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("ready") is True, artifact, "M275-D002-DASH-ready", "ready mismatch", failures)
    return checks_total, checks_passed


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    helper_summary = ROOT / "tmp" / "reports" / "m275" / "M275-D002" / "ensure_build_summary.json"
    build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m275-d002",
        "--summary-out",
        str(helper_summary),
    ])
    checks_total += 1
    checks_passed += require(build.returncode == 0, "ensure_objc3c_native_build.py", "M275-D002-DYN-build", f"fast build failed: {build.stderr or build.stdout}", failures)

    emit_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m275" / "d002" / "emit"
    emit_dir.mkdir(parents=True, exist_ok=True)
    emit = run_command([
        str(args.native_exe),
        str(FIXTURE),
        "--out-dir",
        str(emit_dir),
        "--emit-prefix",
        "module",
        "--emit-objc3-conformance",
        "--emit-objc3-conformance-format",
        "json",
    ])
    checks_total += 1
    checks_passed += require(emit.returncode == 0, "emit", "M275-D002-DYN-emit", f"native emit failed: {emit.stderr or emit.stdout}", failures)
    report_path = emit_dir / "module.objc3-conformance-report.json"
    checks_total += 1
    checks_passed += require(report_path.exists(), display_path(report_path), "M275-D002-DYN-report", "report missing", failures)

    validate_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m275" / "d002" / "validate"
    validate_dir.mkdir(parents=True, exist_ok=True)
    validate = run_command([
        str(args.native_exe),
        "--validate-objc3-conformance",
        str(report_path),
        "--out-dir",
        str(validate_dir),
        "--emit-prefix",
        "module",
        "--emit-objc3-conformance-format",
        "json",
    ])
    checks_total += 1
    checks_passed += require(validate.returncode == 0, "validate", "M275-D002-DYN-validate", f"native validate failed: {validate.stderr or validate.stdout}", failures)
    validation_path = validate_dir / "module.objc3-conformance-validation.json"
    operation_path = validate_dir / "module.objc3-release-evidence-operation.json"
    dashboard_path = validate_dir / "module.objc3-dashboard-status.json"
    for path, check_id in [(validation_path, "validation"), (operation_path, "operation"), (dashboard_path, "dashboard")]:
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), f"M275-D002-DYN-{check_id}", f"{check_id} artifact missing", failures)
    if operation_path.exists():
        payload = load_json(operation_path)
        total, passed = validate_operation_artifact(payload, display_path(operation_path), failures)
        checks_total += total
        checks_passed += passed
    if dashboard_path.exists():
        payload = load_json(dashboard_path)
        total, passed = validate_dashboard_artifact(payload, display_path(dashboard_path), failures)
        checks_total += total
        checks_passed += passed

    return checks_total, checks_passed, {
        "build_summary_path": display_path(helper_summary),
        "report_path": display_path(report_path),
        "validation_path": display_path(validation_path),
        "operation_path": display_path(operation_path),
        "dashboard_path": display_path(dashboard_path),
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    static_snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M275-D002-DOC-contract", CONTRACT_ID),
            SnippetCheck("M275-D002-DOC-dashboard", DASHBOARD_CONTRACT_ID),
        ],
        PACKET_DOC: [
            SnippetCheck("M275-D002-PKT-id", "M275-D002"),
            SnippetCheck("M275-D002-PKT-next", "M275-E001"),
        ],
        MANIFEST_H: [
            SnippetCheck("M275-D002-MANIFEST-H-op", "BuildReleaseEvidenceOperationArtifactPath"),
            SnippetCheck("M275-D002-MANIFEST-H-dashboard", "BuildDashboardStatusArtifactPath"),
        ],
        MANIFEST_CPP: [
            SnippetCheck("M275-D002-MANIFEST-CPP-op", ".objc3-release-evidence-operation.json"),
            SnippetCheck("M275-D002-MANIFEST-CPP-dashboard", ".objc3-dashboard-status.json"),
        ],
        PROCESS_H: [
            SnippetCheck("M275-D002-PROCESS-H-op", "Objc3ReleaseEvidenceOperationArtifactInputs"),
            SnippetCheck("M275-D002-PROCESS-H-dashboard", "Objc3DashboardStatusArtifactInputs"),
        ],
        PROCESS_CPP: [
            SnippetCheck("M275-D002-PROCESS-CPP-contract", CONTRACT_ID),
            SnippetCheck("M275-D002-PROCESS-CPP-dashboard-contract", DASHBOARD_CONTRACT_ID),
            SnippetCheck("M275-D002-PROCESS-CPP-gate", "scripts/check_release_evidence.py"),
            SnippetCheck("M275-D002-PROCESS-CPP-builder", "TryBuildObjc3ReleaseEvidenceOperationArtifact"),
        ],
        DRIVER_CPP: [
            SnippetCheck("M275-D002-DRIVER-op-write", "WriteReleaseEvidenceOperationArtifact"),
            SnippetCheck("M275-D002-DRIVER-dashboard-write", "WriteDashboardStatusArtifact"),
        ],
        RUNTIME_CPP: [
            SnippetCheck("M275-D002-RUNTIME-contract", CONTRACT_ID),
            SnippetCheck("M275-D002-RUNTIME-dashboard", DASHBOARD_CONTRACT_ID),
        ],
        RUNTIME_BOOTSTRAP_H: [
            SnippetCheck("M275-D002-RUNTIME-H-anchor", "release-evidence operation sidecar plus one"),
        ],
        DOC_SOURCE: [
            SnippetCheck("M275-D002-DOCSRC-title", "## Part 12 release-evidence operations and dashboard publication (M275-D002)"),
            SnippetCheck("M275-D002-DOCSRC-op", "module.objc3-release-evidence-operation.json"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M275-D002-DOC-title", "## Part 12 release-evidence operations and dashboard publication (M275-D002)"),
            SnippetCheck("M275-D002-DOC-dashboard", "module.objc3-dashboard-status.json"),
        ],
        SPEC_CHECKLIST: [
            SnippetCheck("M275-D002-SPEC-checklist", "Part 12 release-evidence operations and dashboard publication"),
        ],
        SPEC_CORE: [
            SnippetCheck("M275-D002-SPEC-core", "M275-D002 release-evidence operations and dashboard publication note:"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M275-D002-PKG-check", '"check:objc3c:m275-d002-release-evidence-operations-and-dashboard-publication"'),
            SnippetCheck("M275-D002-PKG-readiness", '"check:objc3c:m275-d002-lane-d-readiness"'),
        ],
    }
    for path, snippets in static_snippets.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    evidence: dict[str, Any] = {}
    if args.skip_dynamic_probes:
        dynamic_executed = False
    else:
        dynamic_executed = True
        total, passed, evidence = run_dynamic_probes(args, failures)
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
        "evidence": evidence,
        "next_issue": "M275-E001",
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    if failures:
        print(json.dumps(payload, indent=2))
        return 1
    print(json.dumps(payload, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

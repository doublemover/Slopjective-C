#!/usr/bin/env python3
"""Checker for M275-E002 release-candidate execution matrix."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m275-e002-release-candidate-execution-matrix-v1"
CONTRACT_ID = "objc3c-part12-release-candidate-execution-matrix/m275-e002-v1"
EXPECTED_SUMMARY = ROOT / "tmp" / "reports" / "m275" / "M275-E002" / "release_candidate_execution_matrix_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m275_e002_release_candidate_matrix_positive.objc3"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m275_release_candidate_execution_matrix_cross_lane_integration_sync_e002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m275" / "m275_e002_release_candidate_execution_matrix_cross_lane_integration_sync_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_CHECKLIST = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
SPEC_CORE = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
PACKAGE_JSON = ROOT / "package.json"
MANIFEST_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"

EXPECTED_ROW_CONTRACTS = [
    "objc3c-part12-migration-canonicalization-source-completion/m275-a002-v1",
    "objc3c-part12-legacy-canonical-migration-semantics/m275-b003-v1",
    "objc3c-part12-corpus-sharding-release-evidence-packaging/m275-c003-v1",
    "objc3c-part12-release-evidence-toolchain-operations/m275-d002-v1",
    "objc3c-part12-integrated-advanced-feature-gate/m275-e001-v1",
]


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
        failures.append(Finding(display_path(path), "M275-E002-MISSING", f"missing artifact: {display_path(path)}"))
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


def validate_matrix_artifact(payload: dict[str, Any], artifact: str, expected_surface_kind: str, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    expected = {
        "contract_id": CONTRACT_ID,
        "schema_id": "objc3c-part12-release-candidate-execution-matrix-v1",
        "surface_kind": expected_surface_kind,
        "release_label": "v0.11",
        "validation_artifact_expected": "module.objc3-conformance-validation.json",
        "release_evidence_operation_artifact_expected": "module.objc3-release-evidence-operation.json",
        "dashboard_artifact_expected": "module.objc3-dashboard-status.json",
    }
    for key, expected_value in expected.items():
        checks_total += 1
        checks_passed += require(payload.get(key) == expected_value, artifact, f"M275-E002-{expected_surface_kind}-{key}", f"{key} mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("targeted_profile_ids") == ["strict", "strict-concurrency", "strict-system"], artifact, f"M275-E002-{expected_surface_kind}-profiles", "targeted_profile_ids mismatch", failures)
    rows = payload.get("matrix_rows")
    checks_total += 1
    checks_passed += require(isinstance(rows, list) and len(rows) == 5, artifact, f"M275-E002-{expected_surface_kind}-row-count", "matrix_rows length mismatch", failures)
    if isinstance(rows, list):
        row_contracts = [row.get("contract_id") for row in rows if isinstance(row, dict)]
        checks_total += 1
        checks_passed += require(row_contracts == EXPECTED_ROW_CONTRACTS, artifact, f"M275-E002-{expected_surface_kind}-row-contracts", "matrix row contracts mismatch", failures)
        checks_total += 1
        checks_passed += require(all(isinstance(row, dict) and row.get("status") == "pass" for row in rows), artifact, f"M275-E002-{expected_surface_kind}-row-status", "matrix row status mismatch", failures)
    else:
        checks_total += 2
        failures.append(Finding(artifact, f"M275-E002-{expected_surface_kind}-row-contracts", "matrix_rows is not a list"))
        failures.append(Finding(artifact, f"M275-E002-{expected_surface_kind}-row-status", "matrix_rows is not a list"))
    checks_total += 1
    checks_passed += require(payload.get("ready") is True, artifact, f"M275-E002-{expected_surface_kind}-ready", "ready mismatch", failures)
    return checks_total, checks_passed


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    helper_summary = ROOT / "tmp" / "reports" / "m275" / "M275-E002" / "ensure_build_summary.json"
    build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m275-e002",
        "--summary-out",
        str(helper_summary),
    ])
    checks_total += 1
    checks_passed += require(build.returncode == 0, "ensure_objc3c_native_build.py", "M275-E002-DYN-build", f"fast build failed: {build.stderr or build.stdout}", failures)

    native_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m275" / "e002" / "native"
    native_dir.mkdir(parents=True, exist_ok=True)
    native = run_command([
        str(args.native_exe),
        str(FIXTURE),
        "--out-dir",
        str(native_dir),
        "--emit-prefix",
        "module",
        "--emit-objc3-conformance",
        "--emit-objc3-conformance-format",
        "json",
    ])
    checks_total += 1
    checks_passed += require(native.returncode == 0, "native", "M275-E002-DYN-native", f"native emit failed: {native.stderr or native.stdout}", failures)
    report_path = native_dir / "module.objc3-conformance-report.json"
    native_matrix_path = native_dir / "module.objc3-release-candidate-matrix.json"
    native_gate_path = native_dir / "module.objc3-advanced-feature-gate.json"
    for path, check_id in [(report_path, "report"), (native_gate_path, "native-gate"), (native_matrix_path, "native-matrix")]:
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), f"M275-E002-DYN-{check_id}", f"{check_id} artifact missing", failures)
    if native_matrix_path.exists():
        payload = load_json(native_matrix_path)
        total, passed = validate_matrix_artifact(payload, display_path(native_matrix_path), "native-cli", failures)
        checks_total += total
        checks_passed += passed

    frontend_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m275" / "e002" / "frontend"
    frontend_dir.mkdir(parents=True, exist_ok=True)
    frontend = run_command([
        str(args.runner_exe),
        str(FIXTURE),
        "--out-dir",
        str(frontend_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    checks_total += 1
    checks_passed += require(frontend.returncode == 0, "frontend", "M275-E002-DYN-frontend", f"frontend runner failed: {frontend.stderr or frontend.stdout}", failures)
    frontend_matrix_path = frontend_dir / "module.objc3-release-candidate-matrix.json"
    checks_total += 1
    checks_passed += require(frontend_matrix_path.exists(), display_path(frontend_matrix_path), "M275-E002-DYN-frontend-matrix", "frontend matrix artifact missing", failures)
    if frontend_matrix_path.exists():
        payload = load_json(frontend_matrix_path)
        total, passed = validate_matrix_artifact(payload, display_path(frontend_matrix_path), "frontend-c-api", failures)
        checks_total += total
        checks_passed += passed

    validate_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m275" / "e002" / "validate"
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
    checks_passed += require(validate.returncode == 0, "validate", "M275-E002-DYN-validate", f"native validate failed: {validate.stderr or validate.stdout}", failures)
    for path, check_id in [
        (validate_dir / "module.objc3-conformance-validation.json", "validation"),
        (validate_dir / "module.objc3-release-evidence-operation.json", "operation"),
        (validate_dir / "module.objc3-dashboard-status.json", "dashboard"),
    ]:
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), f"M275-E002-DYN-{check_id}", f"{check_id} artifact missing", failures)

    return checks_total, checks_passed, {
        "build_summary_path": display_path(helper_summary),
        "native_matrix_path": display_path(native_matrix_path),
        "frontend_matrix_path": display_path(frontend_matrix_path),
        "validation_dir": display_path(validate_dir),
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    static_snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M275-E002-DOC-contract", CONTRACT_ID),
            SnippetCheck("M275-E002-DOC-artifact", "module.objc3-release-candidate-matrix.json"),
        ],
        PACKET_DOC: [
            SnippetCheck("M275-E002-PKT-id", "M275-E002"),
            SnippetCheck("M275-E002-PKT-next", "M275 milestone closeout"),
        ],
        MANIFEST_CPP: [
            SnippetCheck("M275-E002-MANIFEST", ".objc3-release-candidate-matrix.json"),
        ],
        DRIVER_CPP: [
            SnippetCheck("M275-E002-DRIVER-build", "TryBuildObjc3ReleaseCandidateMatrixArtifact"),
            SnippetCheck("M275-E002-DRIVER-write", "WriteReleaseCandidateMatrixArtifact"),
        ],
        FRONTEND_ANCHOR_CPP: [
            SnippetCheck("M275-E002-FRONTEND-build", "TryBuildObjc3ReleaseCandidateMatrixArtifact"),
            SnippetCheck("M275-E002-FRONTEND-write", "BuildReleaseCandidateMatrixArtifactPath"),
        ],
        PROCESS_CPP: [
            SnippetCheck("M275-E002-PROCESS-contract", CONTRACT_ID),
            SnippetCheck("M275-E002-PROCESS-builder", "TryBuildObjc3ReleaseCandidateMatrixArtifact"),
        ],
        DOC_SOURCE: [
            SnippetCheck("M275-E002-DOCSRC-title", "## Part 12 release-candidate execution matrix (M275-E002)"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M275-E002-DOC-title", "## Part 12 release-candidate execution matrix (M275-E002)"),
        ],
        SPEC_CHECKLIST: [
            SnippetCheck("M275-E002-SPEC-checklist", "Part 12 release-candidate execution matrix"),
        ],
        SPEC_CORE: [
            SnippetCheck("M275-E002-SPEC-core", "M275-E002 release-candidate execution matrix note:"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M275-E002-PKG-check", '"check:objc3c:m275-e002-release-candidate-execution-matrix"'),
            SnippetCheck("M275-E002-PKG-readiness", '"check:objc3c:m275-e002-lane-e-readiness"'),
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
        "next_issue": "none",
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    if failures:
        print(json.dumps(payload, indent=2))
        return 1
    print(json.dumps(payload, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

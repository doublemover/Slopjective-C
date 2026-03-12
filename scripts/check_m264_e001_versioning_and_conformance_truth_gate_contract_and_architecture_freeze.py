#!/usr/bin/env python3
"""Fail-closed checker for M264-E001 versioning/conformance truth gate."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m264-e001-versioning-conformance-truth-gate-v1"
CONTRACT_ID = "objc3c-versioning-conformance-truth-gate/m264-e001-v1"
SUPPORTED_MODEL = "core-profile-json-only-conformance-surface-stays-bounded-to-the-runnable-native-subset"
EVIDENCE_MODEL = "gate-consumes-a002-b003-c002-d002-summaries-plus-live-native-and-frontend-publication-probes"
FAIL_CLOSED_MODEL = "strict-profiles-feature-macro-surfaces-and-yaml-operations-remain-unclaimed"
NEXT_CLOSEOUT_ISSUE = "M264-E002"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m264" / "M264-E001" / "versioning_and_conformance_truth_gate_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m264_versioning_and_conformance_truth_gate_contract_and_architecture_freeze_e001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m264" / "m264_e001_versioning_and_conformance_truth_gate_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_CHECKLIST = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
SPEC_DECISIONS = ROOT / "spec" / "DECISIONS_LOG.md"
PACKAGE_JSON = ROOT / "package.json"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MANIFEST_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
HELLO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
METADATA_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
A002_SUMMARY = ROOT / "tmp" / "reports" / "m264" / "M264-A002" / "frontend_feature_claim_and_strictness_truthfulness_wiring_summary.json"
B003_SUMMARY = ROOT / "tmp" / "reports" / "m264" / "M264-B003" / "canonical_interface_and_feature_macro_truthfulness_summary.json"
C002_SUMMARY = ROOT / "tmp" / "reports" / "m264" / "M264-C002" / "machine_readable_runtime_capability_reporting_summary.json"
D002_SUMMARY = ROOT / "tmp" / "reports" / "m264" / "M264-D002" / "cli_and_toolchain_conformance_claim_operations_summary.json"


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
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M264-E001-MISSING", f"missing artifact: {display_path(path)}"))
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
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def validate_dependency_summary(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    total = 2
    passed = 0
    summary_ok = payload.get("ok") is True or (
        isinstance(payload.get("checks_total"), int)
        and isinstance(payload.get("checks_passed"), int)
        and payload.get("checks_total") == payload.get("checks_passed")
        and not payload.get("failures")
    )
    passed += require(summary_ok, display_path(path), f"M264-E001-{path.stem}-OK", "dependency summary is not ok", failures)
    passed += require(payload.get("dynamic_probes_executed") in {True, False}, display_path(path), f"M264-E001-{path.stem}-DYN", "dependency summary missing dynamic probe field", failures)
    return total, passed, {"summary_path": display_path(path), "mode": payload.get("mode")}


def run_dynamic_probes(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    helper_summary = ROOT / "tmp" / "reports" / "m264" / "M264-E001" / "ensure_build_summary.json"
    build = run_command([
        "python",
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m264-e001",
        "--summary-out",
        str(helper_summary),
    ])
    checks_total += 1
    checks_passed += require(build.returncode == 0, "ensure_objc3c_native_build.py", "M264-E001-DYN-BUILD", f"fast build failed: {build.stderr or build.stdout}", failures)

    native_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m264" / "e001" / "native"
    native_dir.mkdir(parents=True, exist_ok=True)
    native = run_command([
        str(NATIVE_EXE),
        str(HELLO_FIXTURE),
        "--out-dir",
        str(native_dir),
        "--emit-prefix",
        "module",
        "--emit-objc3-conformance",
        "--emit-objc3-conformance-format",
        "json",
    ])
    checks_total += 1
    checks_passed += require(native.returncode == 0, "native-probe", "M264-E001-DYN-NATIVE", f"native probe failed: {native.stderr or native.stdout}", failures)
    for artifact_name in [
        "module.objc3-conformance-report.json",
        "module.objc3-conformance-publication.json",
    ]:
        artifact_path = native_dir / artifact_name
        checks_total += 1
        checks_passed += require(artifact_path.exists(), display_path(artifact_path), f"M264-E001-DYN-{artifact_name}", f"missing {artifact_name}", failures)

    validate_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m264" / "e001" / "validate"
    validate_dir.mkdir(parents=True, exist_ok=True)
    validate = run_command([
        str(NATIVE_EXE),
        "--validate-objc3-conformance",
        str(native_dir / "module.objc3-conformance-report.json"),
        "--out-dir",
        str(validate_dir),
        "--emit-prefix",
        "module",
        "--emit-objc3-conformance-format",
        "json",
    ])
    checks_total += 1
    checks_passed += require(validate.returncode == 0, "native-validate", "M264-E001-DYN-VALIDATE", f"validation probe failed: {validate.stderr or validate.stdout}", failures)
    validation_path = validate_dir / "module.objc3-conformance-validation.json"
    checks_total += 1
    checks_passed += require(validation_path.exists(), display_path(validation_path), "M264-E001-DYN-VALIDATION-ARTIFACT", "missing validation artifact", failures)
    if validation_path.exists():
        validation_payload = load_json(validation_path)
        checks_total += 3
        checks_passed += require(validation_payload.get("selected_profile") == "core", display_path(validation_path), "M264-E001-DYN-VALIDATION-PROFILE", "validation profile drifted", failures)
        checks_passed += require(validation_payload.get("format") == "json", display_path(validation_path), "M264-E001-DYN-VALIDATION-FORMAT", "validation format drifted", failures)
        checks_passed += require(validation_payload.get("publication_surface_kind") == "native-cli", display_path(validation_path), "M264-E001-DYN-VALIDATION-SURFACE", "validation surface kind drifted", failures)

    runner_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m264" / "e001" / "frontend-runner"
    runner_dir.mkdir(parents=True, exist_ok=True)
    runner = run_command([
        str(RUNNER_EXE),
        str(METADATA_FIXTURE),
        "--out-dir",
        str(runner_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    checks_total += 1
    checks_passed += require(runner.returncode == 0, "frontend-runner", "M264-E001-DYN-FRONTEND", f"frontend runner failed: {runner.stderr or runner.stdout}", failures)
    runner_report = runner_dir / "module.objc3-conformance-report.json"
    runner_publication = runner_dir / "module.objc3-conformance-publication.json"
    checks_total += 2
    checks_passed += require(runner_report.exists(), display_path(runner_report), "M264-E001-DYN-FRONTEND-REPORT", "frontend conformance report missing", failures)
    checks_passed += require(runner_publication.exists(), display_path(runner_publication), "M264-E001-DYN-FRONTEND-PUBLICATION", "frontend publication missing", failures)
    if runner_publication.exists():
        runner_payload = load_json(runner_publication)
        checks_total += 2
        checks_passed += require(runner_payload.get("publication_surface_kind") == "frontend-c-api", display_path(runner_publication), "M264-E001-DYN-FRONTEND-SURFACE", "frontend publication surface drifted", failures)
        checks_passed += require(runner_payload.get("selected_profile") == "core", display_path(runner_publication), "M264-E001-DYN-FRONTEND-PROFILE", "frontend selected profile drifted", failures)

    return checks_total, checks_passed, {
        "build_summary_path": display_path(helper_summary),
        "native_dir": display_path(native_dir),
        "validate_dir": display_path(validate_dir),
        "frontend_runner_dir": display_path(runner_dir),
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M264-E001-EXP-CONTRACT", CONTRACT_ID),
            SnippetCheck("M264-E001-EXP-VALIDATION", "module.objc3-conformance-validation.json"),
            SnippetCheck("M264-E001-EXP-NEXT", "M264-E002"),
        ],
        PACKET_DOC: [
            SnippetCheck("M264-E001-PKT-ISSUE", "Issue: `#7242`"),
            SnippetCheck("M264-E001-PKT-D002", "`M264-D002`"),
            SnippetCheck("M264-E001-PKT-NEXT", "`M264-E002`"),
        ],
        DRIVER_CPP: [
            SnippetCheck("M264-E001-DRV-ANCHOR", "M264-E001 versioning/conformance truth-gate anchor"),
        ],
        MANIFEST_CPP: [
            SnippetCheck("M264-E001-MAN-ANCHOR", "M264-E001 versioning/conformance truth-gate anchor"),
        ],
        FRONTEND_ANCHOR_CPP: [
            SnippetCheck("M264-E001-FRONTEND-ANCHOR", "M264-E001 versioning/conformance truth-gate anchor"),
        ],
        DOC_SOURCE: [
            SnippetCheck("M264-E001-DOCSOURCE", "## Versioning and conformance truth gate (M264-E001)"),
            SnippetCheck("M264-E001-DOCSOURCE-VALIDATION", "module.objc3-conformance-validation.json"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M264-E001-DOCNATIVE", "Versioning and conformance truth gate (M264-E001)"),
        ],
        SPEC_CHECKLIST: [
            SnippetCheck("M264-E001-SPEC-CHECKLIST", "## M264 versioning and conformance truth gate (implementation note)"),
        ],
        SPEC_DECISIONS: [
            SnippetCheck("M264-E001-SPEC-DECISION", "## D-024: The M264 milestone gate freezes one core/json-only conformance boundary"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M264-E001-PKG-CHECK", "check:objc3c:m264-e001-versioning-conformance-truth-gate"),
            SnippetCheck("M264-E001-PKG-READINESS", "check:objc3c:m264-e001-lane-e-readiness"),
        ],
    }

    for path, snippets in static_snippets.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dependency_cases: list[dict[str, Any]] = []
    for dependency_summary in [A002_SUMMARY, B003_SUMMARY, C002_SUMMARY, D002_SUMMARY]:
        total, passed, case = validate_dependency_summary(dependency_summary, failures)
        checks_total += total
        checks_passed += passed
        dependency_cases.append(case)

    dynamic_cases: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        total, passed, dynamic_cases = run_dynamic_probes(failures)
        checks_total += total
        checks_passed += passed

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "supported_model": SUPPORTED_MODEL,
        "evidence_model": EVIDENCE_MODEL,
        "fail_closed_model": FAIL_CLOSED_MODEL,
        "next_closeout_issue": NEXT_CLOSEOUT_ISSUE,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "ok": not failures,
        "dependencies": dependency_cases,
        "dynamic": dynamic_cases,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    if failures:
        json.dump(payload, sys.stdout, indent=2)
        sys.stdout.write("\n")
        return 1
    print(f"[ok] M264-E001 versioning/conformance truth gate checks passed ({checks_passed}/{checks_total})")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

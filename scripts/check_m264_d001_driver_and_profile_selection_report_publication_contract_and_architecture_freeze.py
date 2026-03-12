#!/usr/bin/env python3
"""Fail-closed checker for M264-D001 driver/publication contract freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m264-d001-driver-publication-contract-v1"
CONTRACT_ID = "objc3c-driver-conformance-report-publication/m264-d001-v1"
SCHEMA_ID = "objc3c-driver-conformance-publication-v1"
LOWERED_REPORT_CONTRACT_ID = "objc3c-versioned-conformance-report-lowering/m264-c001-v1"
RUNTIME_CAPABILITY_CONTRACT_ID = "objc3c-runtime-capability-reporting/m264-c002-v1"
PUBLIC_SCHEMA_ID = "objc3-conformance-report/v1"
EXPECTED_SUMMARY = ROOT / "tmp" / "reports" / "m264" / "M264-D001" / "driver_and_profile_selection_report_publication_summary.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
HELLO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
METADATA_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m264_driver_and_profile_selection_report_publication_contract_and_architecture_freeze_d001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m264" / "m264_d001_driver_and_profile_selection_report_publication_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_CHECKLIST = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
SPEC_DECISIONS = ROOT / "spec" / "DECISIONS_LOG.md"
PACKAGE_JSON = ROOT / "package.json"
CLI_OPTIONS_H = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.h"
CLI_OPTIONS_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.cpp"
NATIVE_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
MANIFEST_ARTIFACTS_H = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.h"
MANIFEST_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
PROCESS_H = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"


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
    parser.add_argument("--runner-exe", type=Path, default=RUNNER_EXE)
    parser.add_argument("--native-exe", type=Path, default=NATIVE_EXE)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M264-D001-MISSING", f"missing artifact: {display_path(path)}"))
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
    expected_values = {
        "contract_id": CONTRACT_ID,
        "schema_id": SCHEMA_ID,
        "selected_profile": "core",
        "effective_compatibility_mode": "canonical",
        "publication_model": "driver-publishes-lowered-conformance-sidecar-and-runtime-capability-sidecar-next-to-manifest",
        "publication_surface_kind": expected_surface_kind,
        "fail_closed_diagnostic_model": "core-profile-live-other-known-profiles-fail-closed-before-publication",
        "lowered_report_contract_id": LOWERED_REPORT_CONTRACT_ID,
        "runtime_capability_contract_id": RUNTIME_CAPABILITY_CONTRACT_ID,
        "public_conformance_schema_id": PUBLIC_SCHEMA_ID,
        "report_artifact": "module.objc3-conformance-report.json",
    }
    for key, expected in expected_values.items():
        checks_total += 1
        checks_passed += require(payload.get(key) == expected, artifact, f"M264-D001-{expected_surface_kind}-{key}", f"{key} mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("selected_profile_supported") is True, artifact, f"M264-D001-{expected_surface_kind}-SUPPORTED", "selected_profile_supported mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("migration_assist_enabled") is False, artifact, f"M264-D001-{expected_surface_kind}-MIGRATION", "migration_assist_enabled mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("ready") is True, artifact, f"M264-D001-{expected_surface_kind}-READY", "ready mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("supported_profile_ids") == ["core"], artifact, f"M264-D001-{expected_surface_kind}-SUPPORTED-PROFILES", "supported profiles mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("rejected_profile_ids") == ["strict", "strict-concurrency", "strict-system"], artifact, f"M264-D001-{expected_surface_kind}-REJECTED-PROFILES", "rejected profiles mismatch", failures)
    return checks_total, checks_passed


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    helper_summary = ROOT / "tmp" / "reports" / "m264" / "M264-D001" / "ensure_build_summary.json"
    build = run_command([
        "python",
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m264-d001",
        "--summary-out",
        str(helper_summary),
    ])
    checks_total += 1
    checks_passed += require(build.returncode == 0, "ensure_objc3c_native_build.py", "M264-D001-DYN-BUILD", f"fast build failed: {build.stderr or build.stdout}", failures)

    cases = []
    case_specs = [
        {
            "case_id": "hello-native",
            "tool": str(args.native_exe),
            "fixture": HELLO_FIXTURE,
            "extra_args": [],
            "surface_kind": "native-cli",
        },
        {
            "case_id": "metadata-runner",
            "tool": str(args.runner_exe),
            "fixture": METADATA_FIXTURE,
            "extra_args": ["--no-emit-ir", "--no-emit-object"],
            "surface_kind": "frontend-c-api",
        },
    ]
    for spec in case_specs:
        out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m264" / "d001" / spec["case_id"]
        out_dir.mkdir(parents=True, exist_ok=True)
        command = [
            spec["tool"],
            str(spec["fixture"]),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
            *spec["extra_args"],
        ]
        completed = run_command(command)
        checks_total += 1
        checks_passed += require(completed.returncode == 0, spec["case_id"], f"M264-D001-{spec['case_id']}-RC", f"probe failed: {completed.stderr or completed.stdout}", failures)
        publication_path = out_dir / "module.objc3-conformance-publication.json"
        report_path = out_dir / "module.objc3-conformance-report.json"
        checks_total += 1
        checks_passed += require(publication_path.exists(), display_path(publication_path), f"M264-D001-{spec['case_id']}-PUBLICATION", "publication artifact missing", failures)
        checks_total += 1
        checks_passed += require(report_path.exists(), display_path(report_path), f"M264-D001-{spec['case_id']}-REPORT", "conformance report missing", failures)
        if publication_path.exists():
            payload = load_json(publication_path)
            total, passed = validate_publication_artifact(payload, display_path(publication_path), spec["surface_kind"], failures)
            checks_total += total
            checks_passed += passed
            cases.append({
                "case_id": spec["case_id"],
                "publication_path": display_path(publication_path),
                "surface_kind": payload.get("publication_surface_kind"),
            })

    reject_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m264" / "d001" / "strict-reject"
    reject_dir.mkdir(parents=True, exist_ok=True)
    reject = run_command([
        str(args.native_exe),
        str(HELLO_FIXTURE),
        "--out-dir",
        str(reject_dir),
        "--emit-prefix",
        "module",
        "--objc3-conformance-profile",
        "strict",
    ])
    checks_total += 1
    checks_passed += require(reject.returncode != 0, "strict-reject", "M264-D001-STRICT-RETURN", "strict profile unexpectedly succeeded", failures)
    checks_total += 1
    checks_passed += require("unsupported --objc3-conformance-profile selection: strict" in (reject.stderr + reject.stdout), "strict-reject", "M264-D001-STRICT-DIAGNOSTIC", "strict rejection diagnostic missing", failures)
    return checks_total, checks_passed, {"cases": cases, "build_summary_path": display_path(helper_summary)}


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    static_snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M264-D001-DOC-CONTRACT", CONTRACT_ID),
            SnippetCheck("M264-D001-DOC-ARTIFACT", "module.objc3-conformance-publication.json"),
        ],
        PACKET_DOC: [
            SnippetCheck("M264-D001-PACKET-ID", "Packet: `M264-D001`"),
            SnippetCheck("M264-D001-PACKET-ISSUE", "Issue: `#7240`"),
        ],
        CLI_OPTIONS_H: [
            SnippetCheck("M264-D001-CLI-H-ENUM", "enum class Objc3ConformanceProfile"),
            SnippetCheck("M264-D001-CLI-H-FIELD", "conformance_profile"),
        ],
        CLI_OPTIONS_CPP: [
            SnippetCheck("M264-D001-CLI-CPP-PARSE", "ParseConformanceProfile"),
            SnippetCheck("M264-D001-CLI-CPP-FLAG", "--objc3-conformance-profile"),
        ],
        MANIFEST_ARTIFACTS_H: [
            SnippetCheck("M264-D001-MANIFEST-H-PATH", "BuildConformancePublicationArtifactPath"),
            SnippetCheck("M264-D001-MANIFEST-H-WRITE", "WriteConformancePublicationArtifact"),
        ],
        MANIFEST_ARTIFACTS_CPP: [
            SnippetCheck("M264-D001-MANIFEST-CPP-SUFFIX", ".objc3-conformance-publication.json"),
            SnippetCheck("M264-D001-MANIFEST-CPP-WRITE", "WriteConformancePublicationArtifact("),
        ],
        PROCESS_H: [
            SnippetCheck("M264-D001-PROCESS-H-INPUTS", "Objc3ConformanceReportPublicationArtifactInputs"),
            SnippetCheck("M264-D001-PROCESS-H-BUILD", "TryBuildObjc3ConformanceReportPublicationArtifact"),
        ],
        PROCESS_CPP: [
            SnippetCheck("M264-D001-PROCESS-CPP-BUILD", "TryBuildObjc3ConformanceReportPublicationArtifact("),
            SnippetCheck("M264-D001-PROCESS-CPP-MODEL", "core-profile-live-other-known-profiles-fail-closed-before-publication"),
        ],
        NATIVE_DRIVER_CPP: [
            SnippetCheck("M264-D001-DRIVER-REJECT", "unsupported --objc3-conformance-profile selection"),
            SnippetCheck("M264-D001-DRIVER-WRITE", "WriteConformancePublicationArtifact("),
        ],
        FRONTEND_ANCHOR_CPP: [
            SnippetCheck("M264-D001-FRONTEND-WRITE", "BuildConformancePublicationArtifactPath"),
            SnippetCheck("M264-D001-FRONTEND-KIND", "frontend-c-api"),
        ],
        DOC_SOURCE: [
            SnippetCheck("M264-D001-DOC-SOURCE-SECTION", "## Driver publication contract (M264-D001)"),
            SnippetCheck("M264-D001-DOC-SOURCE-FLAG", "--objc3-conformance-profile"),
        ],
        DOC_NATIVE: [SnippetCheck("M264-D001-DOC-NATIVE", "## Driver publication contract (M264-D001)")],
        SPEC_CHECKLIST: [SnippetCheck("M264-D001-SPEC-CHECKLIST", "## M264 driver publication contract (implementation note)")],
        SPEC_DECISIONS: [SnippetCheck("M264-D001-SPEC-DECISION", "## D-022: Driver publication and profile selection stay fail-closed until richer conformance operations land")],
        PACKAGE_JSON: [
            SnippetCheck("M264-D001-PKG-CHECK", "check:objc3c:m264-d001-driver-publication-contract"),
            SnippetCheck("M264-D001-PKG-READINESS", "check:objc3c:m264-d001-lane-d-readiness"),
        ],
    }
    for path, snippets in static_snippets.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_details: dict[str, Any] = {"cases": []}
    if not args.skip_dynamic_probes:
        total, passed, dynamic_details = run_dynamic_probes(args, failures)
        checks_total += total
        checks_passed += passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "ok": not failures,
        "failures": [finding.__dict__ for finding in failures],
        **dynamic_details,
    }
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        json.dump(summary, sys.stderr, indent=2)
        sys.stderr.write("\n")
        return 1
    print(f"[ok] M264-D001 driver publication contract checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

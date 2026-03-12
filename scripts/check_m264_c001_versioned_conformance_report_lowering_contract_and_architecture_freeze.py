#!/usr/bin/env python3
"""Fail-closed checker for M264-C001 versioned conformance-report lowering."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m264-c001-versioned-conformance-report-lowering-v1"
CONTRACT_ID = "objc3c-versioned-conformance-report-lowering/m264-c001-v1"
SEMANTIC_CONTRACT_ID = "objc3c-compatibility-strictness-claim-semantics/m264-b001-v1"
A001_CONTRACT_ID = "objc3c-runnable-feature-claim-inventory/m264-a001-v1"
A002_CONTRACT_ID = "objc3c-feature-claim-strictness-truth-surface/m264-a002-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_versioned_conformance_report_lowering_contract"
ARTIFACT_SUFFIX = ".objc3-conformance-report.json"
SCHEMA_ID = "objc3c-versioned-conformance-report-v1"
CANONICAL_INTERFACE_MODE = "no-standalone-interface-payload-yet"
PUBLICATION_MODEL = "written-next-to-manifest-when-out-dir-is-present"
PAYLOAD_MODEL = "frontend-truth-packets-lower-into-one-versioned-machine-readable-conformance-sidecar"
AUTHORITY_MODEL = "runnable-feature-inventory-plus-truth-surface-plus-fail-closed-semantics"
KNOWN_UNSUPPORTED_MODEL = "unsupported-claims-remain-published-as-known-unsupported-without-runnable-overclaim"
SELECTION_MODEL = "canonical-and-legacy-compatibility-selection-only-strictness-and-concurrency-claims-remain-fail-closed"
EXPECTED_SUMMARY = ROOT / "tmp" / "reports" / "m264" / "M264-C001" / "versioned_conformance_report_lowering_summary.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
HELLO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
METADATA_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m264_versioned_conformance_report_lowering_contract_and_architecture_freeze_c001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m264" / "m264_c001_versioned_conformance_report_lowering_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_CHECKLIST = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
SPEC_DECISIONS = ROOT / "spec" / "DECISIONS_LOG.md"
PACKAGE_JSON = ROOT / "package.json"
LOWERING_CONTRACT_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
FRONTEND_TYPES_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
MANIFEST_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"


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
        failures.append(Finding(display_path(path), "M264-C001-MISSING", f"missing artifact: {display_path(path)}"))
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


def validate_report(report: dict[str, Any], artifact: str, expected_mode: str, expected_migration_assist: bool, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    expected_values = {
        "schema_id": SCHEMA_ID,
        "contract_id": CONTRACT_ID,
        "semantic_contract_id": SEMANTIC_CONTRACT_ID,
        "frontend_surface_path": SURFACE_PATH,
        "canonical_interface_mode": CANONICAL_INTERFACE_MODE,
        "publication_model": PUBLICATION_MODEL,
        "payload_model": PAYLOAD_MODEL,
        "authority_model": AUTHORITY_MODEL,
        "known_unsupported_model": KNOWN_UNSUPPORTED_MODEL,
        "selection_model": SELECTION_MODEL,
        "effective_compatibility_mode": expected_mode,
    }
    for key, expected in expected_values.items():
        checks_total += 1
        checks_passed += require(report.get(key) == expected, artifact, f"M264-C001-REPORT-{key}", f"{key} mismatch", failures)
    bool_expectations = {
        "ready": True,
        "migration_assist_enabled": expected_migration_assist,
    }
    for key, expected in bool_expectations.items():
        checks_total += 1
        checks_passed += require(report.get(key) == expected, artifact, f"M264-C001-BOOL-{key}", f"{key} mismatch", failures)
    checks_total += 1
    checks_passed += require(isinstance(report.get("runnable_feature_claim_inventory"), dict), artifact, "M264-C001-NESTED-A001", "runnable feature claim inventory missing", failures)
    checks_total += 1
    checks_passed += require(isinstance(report.get("feature_claim_truth_surface"), dict), artifact, "M264-C001-NESTED-A002", "feature claim truth surface missing", failures)
    checks_total += 1
    checks_passed += require(isinstance(report.get("compatibility_strictness_claim_semantics"), dict), artifact, "M264-C001-NESTED-B001", "compatibility/strictness semantic packet missing", failures)
    a001 = report.get("runnable_feature_claim_inventory", {})
    a002 = report.get("feature_claim_truth_surface", {})
    b001 = report.get("compatibility_strictness_claim_semantics", {})
    nested_expectations = [
        (a001.get("contract_id") == A001_CONTRACT_ID, "M264-C001-A001-CONTRACT", "A001 contract id mismatch"),
        (a002.get("contract_id") == A002_CONTRACT_ID, "M264-C001-A002-CONTRACT", "A002 contract id mismatch"),
        (b001.get("contract_id") == SEMANTIC_CONTRACT_ID, "M264-C001-B001-CONTRACT", "B001 contract id mismatch"),
        (b001.get("canonical_interface_payload_mode") == CANONICAL_INTERFACE_MODE, "M264-C001-B001-CANONICAL", "canonical interface mode mismatch"),
    ]
    for condition, check_id, detail in nested_expectations:
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)
    checks_total += 1
    checks_passed += require(len(a001.get("runnable_feature_claim_ids", [])) == 7, artifact, "M264-C001-A001-RUNNABLE", "unexpected runnable feature claim count", failures)
    checks_total += 1
    checks_passed += require(len(a001.get("source_only_feature_claim_ids", [])) == 6, artifact, "M264-C001-A001-SOURCE-ONLY", "unexpected source-only feature claim count", failures)
    checks_total += 1
    checks_passed += require(len(a001.get("unsupported_feature_claim_ids", [])) == 7, artifact, "M264-C001-A001-UNSUPPORTED", "unexpected unsupported feature claim count", failures)
    checks_total += 1
    checks_passed += require(a002.get("language_version_selection_supported") is True, artifact, "M264-C001-A002-LANG", "language-version selection truth mismatch", failures)
    checks_total += 1
    checks_passed += require(a002.get("compatibility_selection_supported") is True, artifact, "M264-C001-A002-COMPAT", "compatibility selection truth mismatch", failures)
    checks_total += 1
    checks_passed += require(a002.get("migration_assist_selection_supported") is True, artifact, "M264-C001-A002-MIGRATION", "migration-assist selection truth mismatch", failures)
    checks_total += 1
    checks_passed += require(a002.get("strictness_selection_supported") is False, artifact, "M264-C001-A002-STRICTNESS", "strictness selection truth mismatch", failures)
    checks_total += 1
    checks_passed += require(a002.get("strict_concurrency_selection_supported") is False, artifact, "M264-C001-A002-CONCURRENCY", "strict-concurrency selection truth mismatch", failures)
    checks_total += 1
    checks_passed += require(a002.get("feature_macro_surface_supported") is False, artifact, "M264-C001-A002-MACRO-SURFACE", "feature-macro surface truth mismatch", failures)
    checks_total += 1
    checks_passed += require(len(a002.get("suppressed_macro_claim_ids", [])) == 3, artifact, "M264-C001-A002-MACROS", "unexpected suppressed macro claim count", failures)
    checks_total += 1
    checks_passed += require(b001.get("ready") is True, artifact, "M264-C001-B001-READY", "semantic packet not ready", failures)
    return checks_total, checks_passed


def extract_lowering_packet(manifest: dict[str, Any], manifest_path: Path) -> dict[str, Any]:
    frontend = manifest.get("frontend")
    if not isinstance(frontend, dict):
        raise TypeError(f"missing frontend block in {display_path(manifest_path)}")
    pipeline = frontend.get("pipeline")
    if not isinstance(pipeline, dict):
        raise TypeError(f"missing frontend.pipeline block in {display_path(manifest_path)}")
    semantic_surface = pipeline.get("semantic_surface")
    if not isinstance(semantic_surface, dict):
        raise TypeError(f"missing frontend.pipeline.semantic_surface block in {display_path(manifest_path)}")
    packet = semantic_surface.get("objc_versioned_conformance_report_lowering_contract")
    if not isinstance(packet, dict):
        raise TypeError(f"missing {SURFACE_PATH} in {display_path(manifest_path)}")
    return packet


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    helper_summary = ROOT / "tmp" / "reports" / "m264" / "M264-C001" / "ensure_build_summary.json"
    build = run_command([
        "python",
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m264-c001",
        "--summary-out",
        str(helper_summary),
    ])
    checks_total += 1
    checks_passed += require(build.returncode == 0, "ensure_objc3c_native_build.py", "M264-C001-DYN-BUILD", f"fast build failed: {build.stderr or build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M264-C001-DYN-NATIVE", "native executable missing after build", failures)
    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M264-C001-DYN-RUNNER", "frontend runner missing after build", failures)
    cases = []
    case_specs = [
        {
            "case_id": "hello-native",
            "tool": str(args.native_exe),
            "fixture": HELLO_FIXTURE,
            "extra_args": [],
            "expect_ir": True,
            "expect_mode": "canonical",
            "expect_migration_assist": False,
        },
        {
            "case_id": "metadata-runner",
            "tool": str(args.runner_exe),
            "fixture": METADATA_FIXTURE,
            "extra_args": ["--no-emit-ir", "--no-emit-object"],
            "expect_ir": False,
            "expect_mode": "canonical",
            "expect_migration_assist": False,
        },
    ]
    for spec in case_specs:
        out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m264" / "c001" / spec["case_id"]
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
        artifact_label = f"{spec['case_id']} command"
        checks_total += 1
        checks_passed += require(completed.returncode == 0, artifact_label, f"M264-C001-DYN-{spec['case_id']}-RC", f"probe failed: {completed.stderr or completed.stdout}", failures)
        report_path = out_dir / f"module{ARTIFACT_SUFFIX}"
        manifest_path = out_dir / "module.manifest.json"
        checks_total += 1
        checks_passed += require(report_path.exists(), display_path(report_path), f"M264-C001-DYN-{spec['case_id']}-REPORT", "lowered conformance report missing", failures)
        checks_total += 1
        checks_passed += require(manifest_path.exists(), display_path(manifest_path), f"M264-C001-DYN-{spec['case_id']}-MANIFEST", "manifest missing", failures)
        report_payload = load_json(report_path)
        manifest_payload = load_json(manifest_path)
        packet = extract_lowering_packet(manifest_payload, manifest_path)
        total, passed = validate_report(report_payload, display_path(report_path), spec["expect_mode"], spec["expect_migration_assist"], failures)
        checks_total += total
        checks_passed += passed
        checks_total += 1
        checks_passed += require(packet.get("contract_id") == CONTRACT_ID, display_path(manifest_path), f"M264-C001-DYN-{spec['case_id']}-PACKET-CONTRACT", "manifest packet contract mismatch", failures)
        checks_total += 1
        checks_passed += require(packet.get("ready") is True, display_path(manifest_path), f"M264-C001-DYN-{spec['case_id']}-PACKET-READY", "manifest packet not ready", failures)
        case_record: dict[str, Any] = {
            "case_id": spec["case_id"],
            "command": command,
            "report_path": display_path(report_path),
            "manifest_path": display_path(manifest_path),
            "report_contract_id": report_payload.get("contract_id"),
            "semantic_contract_id": report_payload.get("semantic_contract_id"),
            "effective_compatibility_mode": report_payload.get("effective_compatibility_mode"),
            "migration_assist_enabled": report_payload.get("migration_assist_enabled"),
        }
        if spec["expect_ir"]:
            ir_path = out_dir / "module.ll"
            checks_total += 1
            checks_passed += require(ir_path.exists(), display_path(ir_path), "M264-C001-DYN-IR-PRESENT", "IR output missing", failures)
            if ir_path.exists():
                ir_text = read_text(ir_path)
                checks_total += 1
                checks_passed += require("versioned_conformance_report_lowering = " in ir_text, display_path(ir_path), "M264-C001-DYN-IR-ANCHOR", "IR lowering summary missing", failures)
                case_record["ir_path"] = display_path(ir_path)
        if spec["case_id"] == "metadata-runner":
            inventory = report_payload.get("runnable_feature_claim_inventory", {})
            checks_total += 1
            checks_passed += require(inventory.get("declared_protocol_count") == 2, display_path(report_path), "M264-C001-DYN-META-PROTOCOLS", "unexpected declared protocol count", failures)
            checks_total += 1
            checks_passed += require(inventory.get("declared_interface_count") == 1, display_path(report_path), "M264-C001-DYN-META-INTERFACES", "unexpected declared interface count", failures)
            checks_total += 1
            checks_passed += require(inventory.get("declared_implementation_count") == 1, display_path(report_path), "M264-C001-DYN-META-IMPLEMENTATIONS", "unexpected declared implementation count", failures)
            case_record["declared_protocol_count"] = inventory.get("declared_protocol_count")
            case_record["declared_interface_count"] = inventory.get("declared_interface_count")
            case_record["declared_implementation_count"] = inventory.get("declared_implementation_count")
        cases.append(case_record)
    return checks_total, checks_passed, {"cases": cases, "build_summary_path": display_path(helper_summary)}


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    static_snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M264-C001-DOC-EXPECT", "objc3c-versioned-conformance-report-lowering/m264-c001-v1"),
            SnippetCheck("M264-C001-DOC-SURFACE", "frontend.pipeline.semantic_surface.objc_versioned_conformance_report_lowering_contract"),
        ],
        PACKET_DOC: [
            SnippetCheck("M264-C001-PACKET-ID", "Packet: `M264-C001`"),
            SnippetCheck("M264-C001-PACKET-ISSUE", "Issue: `#7238`"),
        ],
        LOWERING_CONTRACT_H: [
            SnippetCheck("M264-C001-H-CONTRACT", "kObjc3VersionedConformanceReportLoweringContractId"),
            SnippetCheck("M264-C001-H-SUFFIX", "kObjc3VersionedConformanceReportLoweringArtifactSuffix"),
        ],
        LOWERING_CONTRACT_CPP: [
            SnippetCheck("M264-C001-CPP-SUMMARY", "Objc3VersionedConformanceReportLoweringContractSummary"),
            SnippetCheck("M264-C001-CPP-PUBLICATION", "publication_model="),
        ],
        FRONTEND_TYPES_H: [
            SnippetCheck("M264-C001-TYPES-SUMMARY", "struct Objc3VersionedConformanceReportLoweringSummary"),
            SnippetCheck("M264-C001-TYPES-READY", "IsReadyObjc3VersionedConformanceReportLoweringSummary"),
        ],
        FRONTEND_ARTIFACTS_CPP: [
            SnippetCheck("M264-C001-ARTIFACTS-SUMMARY", "BuildVersionedConformanceReportLoweringSummary("),
            SnippetCheck("M264-C001-ARTIFACTS-JSON", "BuildVersionedConformanceReportArtifactJson("),
            SnippetCheck("M264-C001-ARTIFACTS-SURFACE", '\\"objc_versioned_conformance_report_lowering_contract\\":'),
        ],
        MANIFEST_ARTIFACTS_CPP: [
            SnippetCheck("M264-C001-MANIFEST-PATH", "BuildVersionedConformanceReportArtifactPath"),
            SnippetCheck("M264-C001-MANIFEST-WRITE", "WriteVersionedConformanceReportArtifact"),
        ],
        DRIVER_CPP: [SnippetCheck("M264-C001-DRIVER-MISSING", "versioned conformance-report artifact payload missing")],
        FRONTEND_ANCHOR_CPP: [SnippetCheck("M264-C001-FRONTEND-NOTREADY", "versioned conformance-report lowering summary not ready")],
        IR_EMITTER_CPP: [SnippetCheck("M264-C001-IR-ANCHOR", "versioned_conformance_report_lowering = ")],
        DOC_SOURCE: [
            SnippetCheck("M264-C001-DOCSOURCE-SECTION", "## Versioned conformance-report lowering (M264-C001)"),
            SnippetCheck("M264-C001-DOCSOURCE-SURFACE", "`frontend.pipeline.semantic_surface.objc_versioned_conformance_report_lowering_contract`")
        ],
        SPEC_CHECKLIST: [SnippetCheck("M264-C001-SPEC-CHECKLIST", "## M264 versioned conformance-report lowering (implementation note)")],
        SPEC_DECISIONS: [SnippetCheck("M264-C001-SPEC-DECISION", "## D-020: Lowered versioned conformance reports must remain bounded to the truthful frontend claim surface")],
        PACKAGE_JSON: [
            SnippetCheck("M264-C001-PKG-CHECK", "check:objc3c:m264-c001-versioned-conformance-report-lowering-contract"),
            SnippetCheck("M264-C001-PKG-READINESS", "check:objc3c:m264-c001-lane-c-readiness"),
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
        "surface_path": SURFACE_PATH,
        "artifact_suffix": ARTIFACT_SUFFIX,
        "ok": not failures,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "checks_failed": len(failures),
        "failures": [finding.__dict__ for finding in failures],
        "dynamic_details": dynamic_details,
    }
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        print(canonical_json(summary), file=sys.stderr, end="")
        return 1
    print("[ok] M264-C001 checker passed")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

#!/usr/bin/env python3
"""Fail-closed checker for M264-C002 runtime capability reporting."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m264-c002-machine-readable-runtime-capability-reporting-v1"
CONTRACT_ID = "objc3c-runtime-capability-reporting/m264-c002-v1"
LOWERING_CONTRACT_ID = "objc3c-versioned-conformance-report-lowering/m264-c001-v1"
SEMANTIC_CONTRACT_ID = "objc3c-compatibility-strictness-claim-semantics/m264-b001-v1"
A001_CONTRACT_ID = "objc3c-runnable-feature-claim-inventory/m264-a001-v1"
A002_CONTRACT_ID = "objc3c-feature-claim-strictness-truth-surface/m264-a002-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_runtime_capability_report"
ARTIFACT_SUFFIX = ".objc3-conformance-report.json"
SCHEMA_ID = "objc3c-runtime-capability-report-v1"
PUBLIC_SCHEMA_ID = "objc3-conformance-report/v1"
PROFILE_MODEL = "core-profile-claimed-strict-profiles-not-claimed-until-runtime-backed"
OPTIONAL_FEATURE_MODEL = "unsupported-runtime-feature-ids-lower-into-not-claimed-public-optional-features"
VERSION_MODEL = "deterministic-dev-version-surface-for-frontend-runtime-stdlib-and-module-format"
STRICTNESS_MODE = "permissive"
CONCURRENCY_MODE = "off"
TARGET_TRIPLE = "x86_64-pc-windows-msvc"
GENERATED_AT = "1970-01-01T00:00:00Z"
EXPECTED_SUMMARY = ROOT / "tmp" / "reports" / "m264" / "M264-C002" / "machine_readable_runtime_capability_reporting_summary.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
HELLO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
METADATA_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m264_machine_readable_runtime_capability_reporting_core_feature_implementation_c002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m264" / "m264_c002_machine_readable_runtime_capability_reporting_core_feature_implementation_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_CHECKLIST = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
SPEC_DECISIONS = ROOT / "spec" / "DECISIONS_LOG.md"
PACKAGE_JSON = ROOT / "package.json"
LOWERING_CONTRACT_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
FRONTEND_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
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
        failures.append(Finding(display_path(path), "M264-C002-MISSING", f"missing artifact: {display_path(path)}"))
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
    base_expectations = {
        "schema_id": "objc3c-versioned-conformance-report-v1",
        "semantic_contract_id": SEMANTIC_CONTRACT_ID,
        "effective_compatibility_mode": expected_mode,
    }
    for key, expected in base_expectations.items():
        checks_total += 1
        checks_passed += require(report.get(key) == expected, artifact, f"M264-C002-BASE-{key}", f"{key} mismatch", failures)
    checks_total += 1
    checks_passed += require(report.get("migration_assist_enabled") == expected_migration_assist, artifact, "M264-C002-BASE-MIGRATION", "migration-assist mismatch", failures)
    checks_total += 1
    checks_passed += require(report.get("ready") is True, artifact, "M264-C002-BASE-READY", "top-level report not ready", failures)

    a001 = report.get("runnable_feature_claim_inventory", {})
    a002 = report.get("feature_claim_truth_surface", {})
    runtime_report = report.get("runtime_capability_report", {})
    public_report = report.get("public_conformance_report", {})
    for key, payload in {
        "A001": a001,
        "A002": a002,
        "RUNTIME": runtime_report,
        "PUBLIC": public_report,
    }.items():
        checks_total += 1
        checks_passed += require(isinstance(payload, dict), artifact, f"M264-C002-NESTED-{key}", f"missing nested payload {key}", failures)

    checks_total += 1
    checks_passed += require(a001.get("contract_id") == A001_CONTRACT_ID, artifact, "M264-C002-A001-CONTRACT", "A001 contract mismatch", failures)
    checks_total += 1
    checks_passed += require(a002.get("contract_id") == A002_CONTRACT_ID, artifact, "M264-C002-A002-CONTRACT", "A002 contract mismatch", failures)

    runtime_expectations = {
        "contract_id": CONTRACT_ID,
        "schema_id": SCHEMA_ID,
        "source_contract_id": LOWERING_CONTRACT_ID,
        "frontend_surface_path": SURFACE_PATH,
        "source_frontend_surface_path": "frontend.pipeline.semantic_surface.objc_versioned_conformance_report_lowering_contract",
        "profile_model": PROFILE_MODEL,
        "optional_feature_model": OPTIONAL_FEATURE_MODEL,
        "version_model": VERSION_MODEL,
        "strictness_mode": STRICTNESS_MODE,
        "concurrency_mode": CONCURRENCY_MODE,
        "public_schema_id": PUBLIC_SCHEMA_ID,
        "replay_generated_at": GENERATED_AT,
    }
    for key, expected in runtime_expectations.items():
        checks_total += 1
        checks_passed += require(runtime_report.get(key) == expected, artifact, f"M264-C002-RUNTIME-{key}", f"runtime field {key} mismatch", failures)
    checks_total += 1
    checks_passed += require(runtime_report.get("claimed_profile_ids") == ["core"], artifact, "M264-C002-RUNTIME-CLAIMED", "claimed profiles mismatch", failures)
    checks_total += 1
    checks_passed += require(runtime_report.get("not_claimed_profile_ids") == ["strict", "strict-concurrency", "strict-system"], artifact, "M264-C002-RUNTIME-NOT-CLAIMED", "not-claimed profiles mismatch", failures)
    checks_total += 1
    checks_passed += require(len(runtime_report.get("runtime_capability_ids", [])) == 7, artifact, "M264-C002-RUNTIME-CAPABILITIES", "unexpected runtime capability count", failures)
    checks_total += 1
    checks_passed += require(len(runtime_report.get("source_only_feature_claim_ids", [])) == 6, artifact, "M264-C002-RUNTIME-SOURCE-ONLY", "unexpected runtime source-only count", failures)
    checks_total += 1
    checks_passed += require(len(runtime_report.get("unsupported_feature_claim_ids", [])) == 7, artifact, "M264-C002-RUNTIME-UNSUPPORTED", "unexpected runtime unsupported count", failures)
    runtime_optional = runtime_report.get("optional_features", [])
    checks_total += 1
    checks_passed += require([entry.get("id") for entry in runtime_optional] == ["throws", "async-await", "actors", "blocks", "arc"], artifact, "M264-C002-RUNTIME-OPTIONAL-IDS", "runtime optional ids mismatch", failures)
    checks_total += 1
    checks_passed += require(all(entry.get("status") == "not-claimed" for entry in runtime_optional), artifact, "M264-C002-RUNTIME-OPTIONAL-STATUS", "runtime optional statuses mismatch", failures)
    checks_total += 1
    checks_passed += require(runtime_report.get("ready") is True, artifact, "M264-C002-RUNTIME-READY", "runtime report not ready", failures)

    checks_total += 1
    checks_passed += require(public_report.get("schema_id") == PUBLIC_SCHEMA_ID, artifact, "M264-C002-PUBLIC-SCHEMA", "public schema mismatch", failures)
    checks_total += 1
    checks_passed += require(public_report.get("generated_at") == GENERATED_AT, artifact, "M264-C002-PUBLIC-GENERATED", "public timestamp mismatch", failures)
    checks_total += 1
    checks_passed += require(public_report.get("known_deviations") == [], artifact, "M264-C002-PUBLIC-DEVIATIONS", "public known deviations mismatch", failures)
    toolchain = public_report.get("toolchain", {})
    language = public_report.get("language", {})
    mode = public_report.get("mode", {})
    checks_total += 1
    checks_passed += require(toolchain.get("name") == "objc3c", artifact, "M264-C002-PUBLIC-TOOLCHAIN-NAME", "toolchain name mismatch", failures)
    checks_total += 1
    checks_passed += require(toolchain.get("vendor") == "doublemover", artifact, "M264-C002-PUBLIC-TOOLCHAIN-VENDOR", "toolchain vendor mismatch", failures)
    checks_total += 1
    checks_passed += require(toolchain.get("version") == "0.0.0-dev", artifact, "M264-C002-PUBLIC-TOOLCHAIN-VERSION", "toolchain version mismatch", failures)
    checks_total += 1
    checks_passed += require(toolchain.get("target_triple") == TARGET_TRIPLE, artifact, "M264-C002-PUBLIC-TOOLCHAIN-TARGET", "target triple mismatch", failures)
    checks_total += 1
    checks_passed += require(language.get("language_family") == "objective-c", artifact, "M264-C002-PUBLIC-LANGUAGE-FAMILY", "language family mismatch", failures)
    checks_total += 1
    checks_passed += require(language.get("language_version") == "3.0", artifact, "M264-C002-PUBLIC-LANGUAGE-VERSION", "language version mismatch", failures)
    checks_total += 1
    checks_passed += require(language.get("spec_revision") == "v1", artifact, "M264-C002-PUBLIC-LANGUAGE-SPEC", "spec revision mismatch", failures)
    checks_total += 1
    checks_passed += require(mode.get("strictness") == STRICTNESS_MODE, artifact, "M264-C002-PUBLIC-MODE-STRICTNESS", "public strictness mismatch", failures)
    checks_total += 1
    checks_passed += require(mode.get("concurrency") == CONCURRENCY_MODE, artifact, "M264-C002-PUBLIC-MODE-CONCURRENCY", "public concurrency mismatch", failures)
    checks_total += 1
    checks_passed += require(mode.get("compatibility") == expected_mode, artifact, "M264-C002-PUBLIC-MODE-COMPATIBILITY", "public compatibility mismatch", failures)
    checks_total += 1
    checks_passed += require(mode.get("migration_assist") == expected_migration_assist, artifact, "M264-C002-PUBLIC-MODE-MIGRATION", "public migration-assist mismatch", failures)
    public_profiles = public_report.get("profiles", [])
    checks_total += 1
    checks_passed += require([entry.get("id") for entry in public_profiles] == ["core", "strict", "strict-concurrency", "strict-system"], artifact, "M264-C002-PUBLIC-PROFILES", "public profile ids mismatch", failures)
    checks_total += 1
    checks_passed += require([entry.get("status") for entry in public_profiles] == ["claimed", "not-claimed", "not-claimed", "not-claimed"], artifact, "M264-C002-PUBLIC-PROFILE-STATUS", "public profile statuses mismatch", failures)
    public_optional = public_report.get("optional_features", [])
    checks_total += 1
    checks_passed += require([entry.get("id") for entry in public_optional] == ["throws", "async-await", "actors", "blocks", "arc"], artifact, "M264-C002-PUBLIC-OPTIONAL-IDS", "public optional ids mismatch", failures)
    checks_total += 1
    checks_passed += require(all(entry.get("status") == "not-claimed" for entry in public_optional), artifact, "M264-C002-PUBLIC-OPTIONAL-STATUS", "public optional statuses mismatch", failures)
    return checks_total, checks_passed


def extract_runtime_capability_packet(manifest: dict[str, Any], manifest_path: Path) -> dict[str, Any]:
    frontend = manifest.get("frontend")
    if not isinstance(frontend, dict):
        raise TypeError(f"missing frontend block in {display_path(manifest_path)}")
    pipeline = frontend.get("pipeline")
    if not isinstance(pipeline, dict):
        raise TypeError(f"missing frontend.pipeline block in {display_path(manifest_path)}")
    semantic_surface = pipeline.get("semantic_surface")
    if not isinstance(semantic_surface, dict):
        raise TypeError(f"missing frontend.pipeline.semantic_surface block in {display_path(manifest_path)}")
    packet = semantic_surface.get("objc_runtime_capability_report")
    if not isinstance(packet, dict):
        raise TypeError(f"missing {SURFACE_PATH} in {display_path(manifest_path)}")
    return packet


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    helper_summary = ROOT / "tmp" / "reports" / "m264" / "M264-C002" / "ensure_build_summary.json"
    build = run_command([
        "python",
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m264-c002",
        "--summary-out",
        str(helper_summary),
    ])
    checks_total += 1
    checks_passed += require(build.returncode == 0, "ensure_objc3c_native_build.py", "M264-C002-DYN-BUILD", f"fast build failed: {build.stderr or build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M264-C002-DYN-NATIVE", "native executable missing after build", failures)
    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M264-C002-DYN-RUNNER", "frontend runner missing after build", failures)
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
        out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m264" / "c002" / spec["case_id"]
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
        checks_passed += require(completed.returncode == 0, artifact_label, f"M264-C002-DYN-{spec['case_id']}-RC", f"probe failed: {completed.stderr or completed.stdout}", failures)
        report_path = out_dir / f"module{ARTIFACT_SUFFIX}"
        manifest_path = out_dir / "module.manifest.json"
        checks_total += 1
        checks_passed += require(report_path.exists(), display_path(report_path), f"M264-C002-DYN-{spec['case_id']}-REPORT", "conformance report missing", failures)
        checks_total += 1
        checks_passed += require(manifest_path.exists(), display_path(manifest_path), f"M264-C002-DYN-{spec['case_id']}-MANIFEST", "manifest missing", failures)
        report_payload = load_json(report_path)
        manifest_payload = load_json(manifest_path)
        packet = extract_runtime_capability_packet(manifest_payload, manifest_path)
        total, passed = validate_report(report_payload, display_path(report_path), spec["expect_mode"], spec["expect_migration_assist"], failures)
        checks_total += total
        checks_passed += passed
        checks_total += 1
        checks_passed += require(packet.get("contract_id") == CONTRACT_ID, display_path(manifest_path), f"M264-C002-DYN-{spec['case_id']}-PACKET-CONTRACT", "manifest packet contract mismatch", failures)
        checks_total += 1
        checks_passed += require(packet.get("ready") is True, display_path(manifest_path), f"M264-C002-DYN-{spec['case_id']}-PACKET-READY", "manifest packet not ready", failures)
        case_record: dict[str, Any] = {
            "case_id": spec["case_id"],
            "command": command,
            "report_path": display_path(report_path),
            "manifest_path": display_path(manifest_path),
            "runtime_contract_id": report_payload.get("runtime_capability_report", {}).get("contract_id"),
            "public_schema_id": report_payload.get("public_conformance_report", {}).get("schema_id"),
        }
        if spec["expect_ir"]:
            ir_path = out_dir / "module.ll"
            checks_total += 1
            checks_passed += require(ir_path.exists(), display_path(ir_path), "M264-C002-DYN-IR-PRESENT", "IR output missing", failures)
            if ir_path.exists():
                ir_text = read_text(ir_path)
                checks_total += 1
                checks_passed += require("runtime_capability_reporting = " in ir_text, display_path(ir_path), "M264-C002-DYN-IR-ANCHOR", "IR capability summary missing", failures)
                case_record["ir_path"] = display_path(ir_path)
        if spec["case_id"] == "metadata-runner":
            inventory = report_payload.get("runnable_feature_claim_inventory", {})
            checks_total += 1
            checks_passed += require(inventory.get("declared_protocol_count") == 2, display_path(report_path), "M264-C002-DYN-META-PROTOCOLS", "unexpected declared protocol count", failures)
            checks_total += 1
            checks_passed += require(inventory.get("declared_interface_count") == 1, display_path(report_path), "M264-C002-DYN-META-INTERFACES", "unexpected declared interface count", failures)
            checks_total += 1
            checks_passed += require(inventory.get("declared_implementation_count") == 1, display_path(report_path), "M264-C002-DYN-META-IMPLEMENTATIONS", "unexpected declared implementation count", failures)
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
            SnippetCheck("M264-C002-DOC-EXPECT", "objc3c-runtime-capability-reporting/m264-c002-v1"),
            SnippetCheck("M264-C002-DOC-SURFACE", "frontend.pipeline.semantic_surface.objc_runtime_capability_report"),
        ],
        PACKET_DOC: [
            SnippetCheck("M264-C002-PACKET-ID", "Packet: `M264-C002`"),
            SnippetCheck("M264-C002-PACKET-ISSUE", "Issue: `#7239`"),
        ],
        LOWERING_CONTRACT_H: [
            SnippetCheck("M264-C002-H-CONTRACT", "kObjc3RuntimeCapabilityReportingContractId"),
            SnippetCheck("M264-C002-H-SCHEMA", "kObjc3RuntimeCapabilityPublicSchemaId"),
        ],
        LOWERING_CONTRACT_CPP: [
            SnippetCheck("M264-C002-CPP-SUMMARY", "Objc3RuntimeCapabilityReportingContractSummary"),
            SnippetCheck("M264-C002-CPP-STRICTNESS", "strictness_mode="),
        ],
        FRONTEND_ARTIFACTS_CPP: [
            SnippetCheck("M264-C002-ARTIFACTS-RUNTIME", "BuildRuntimeCapabilityReportJson("),
            SnippetCheck("M264-C002-ARTIFACTS-PUBLIC", "BuildPublicConformanceReportJson("),
            SnippetCheck("M264-C002-ARTIFACTS-SURFACE", 'objc_runtime_capability_report'),
        ],
        IR_EMITTER_CPP: [SnippetCheck("M264-C002-IR-ANCHOR", "runtime_capability_reporting = ")],
        DOC_SOURCE: [
            SnippetCheck("M264-C002-DOCSOURCE-SECTION", "## Runtime capability reporting (M264-C002)"),
            SnippetCheck("M264-C002-DOCSOURCE-SURFACE", "`public_conformance_report`")
        ],
        DOC_NATIVE: [SnippetCheck("M264-C002-DOC-NATIVE", "## Runtime capability reporting (M264-C002)")],
        SPEC_CHECKLIST: [SnippetCheck("M264-C002-SPEC-CHECKLIST", "## M264 machine-readable runtime capability reporting (implementation note)")],
        SPEC_DECISIONS: [SnippetCheck("M264-C002-SPEC-DECISION", "## D-021: Runtime/public capability reports must remain a truthful projection of the lowered conformance sidecar")],
        PACKAGE_JSON: [
            SnippetCheck("M264-C002-PKG-CHECK", "check:objc3c:m264-c002-runtime-capability-reporting"),
            SnippetCheck("M264-C002-PKG-READINESS", "check:objc3c:m264-c002-lane-c-readiness"),
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
        "schema_id": SCHEMA_ID,
        "surface_path": SURFACE_PATH,
        "artifact_suffix": ARTIFACT_SUFFIX,
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
    print(f"[ok] M264-C002 runtime capability reporting checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

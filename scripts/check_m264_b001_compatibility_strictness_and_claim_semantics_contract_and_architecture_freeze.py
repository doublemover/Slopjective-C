#!/usr/bin/env python3
"""Fail-closed checker for M264-B001 compatibility/strictness claim semantics."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m264-b001-compatibility-strictness-claim-semantics-v1"
CONTRACT_ID = "objc3c-compatibility-strictness-claim-semantics/m264-b001-v1"
A001_CONTRACT_ID = "objc3c-runnable-feature-claim-inventory/m264-a001-v1"
A002_CONTRACT_ID = "objc3c-feature-claim-strictness-truth-surface/m264-a002-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_compatibility_strictness_claim_semantics"
SEMANTIC_MODEL = "live-compatibility-and-migration-selection-source-only-downgrade-unsupported-fail-closed"
DOWNGRADE_MODEL = "source-only-claims-remain-recognized-but-never-promote-to-runnable"
REJECTION_MODEL = "strictness-strict-concurrency-and-feature-macro-claims-remain-fail-closed"
EXPECTED_SUMMARY = ROOT / "tmp" / "reports" / "m264" / "M264-B001" / "compatibility_strictness_and_claim_semantics_summary.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
FIXTURE_HELLO = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
FIXTURE_METADATA = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"

EXPECTED_COUNTS = {
    "valid_compatibility_mode_count": 2,
    "live_selection_surface_count": 3,
    "valid_selection_combination_count": 4,
    "runnable_feature_claim_count": 7,
    "downgraded_source_only_claim_count": 6,
    "rejected_unsupported_feature_claim_count": 7,
    "rejected_selection_surface_count": 2,
    "suppressed_macro_claim_count": 3,
}

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m264_compatibility_strictness_and_claim_semantics_contract_and_architecture_freeze_b001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m264" / "m264_b001_compatibility_strictness_and_claim_semantics_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_CHECKLIST = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
SPEC_DECISIONS = ROOT / "spec" / "DECISIONS_LOG.md"
SEMA_CONTRACT_H = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASSES_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
SEMA_PASS_FLOW_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_flow_scaffold.cpp"
FRONTEND_TYPES_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"


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


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M264-B001-MISSING", f"missing artifact: {display_path(path)}"))
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


def extract_manifest_surfaces(manifest_path: Path) -> tuple[dict[str, Any], dict[str, Any], dict[str, Any]]:
    manifest = load_json(manifest_path)
    frontend = manifest.get("frontend")
    if not isinstance(frontend, dict):
        raise TypeError(f"missing frontend block in {display_path(manifest_path)}")
    pipeline = frontend.get("pipeline")
    if not isinstance(pipeline, dict):
        raise TypeError(f"missing frontend.pipeline block in {display_path(manifest_path)}")
    sema_pass_manager = pipeline.get("sema_pass_manager")
    if not isinstance(sema_pass_manager, dict):
        raise TypeError(f"missing frontend.pipeline.sema_pass_manager block in {display_path(manifest_path)}")
    semantic_surface = pipeline.get("semantic_surface")
    if not isinstance(semantic_surface, dict):
        raise TypeError(f"missing frontend.pipeline.semantic_surface block in {display_path(manifest_path)}")
    packet = semantic_surface.get("objc_compatibility_strictness_claim_semantics")
    if not isinstance(packet, dict):
        raise TypeError(f"missing {SURFACE_PATH} in {display_path(manifest_path)}")
    return frontend, sema_pass_manager, packet


def validate_packet(packet: dict[str, Any], artifact: str, expected_mode: str, expected_migration_assist: bool, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    expected_values = {
        "contract_id": CONTRACT_ID,
        "runnable_feature_claim_inventory_contract_id": A001_CONTRACT_ID,
        "feature_claim_truth_surface_contract_id": A002_CONTRACT_ID,
        "frontend_surface_path": SURFACE_PATH,
        "semantic_model": SEMANTIC_MODEL,
        "downgrade_model": DOWNGRADE_MODEL,
        "rejection_model": REJECTION_MODEL,
        "effective_compatibility_mode": expected_mode,
    }
    for key, value in expected_values.items():
        checks_total += 1
        checks_passed += require(packet.get(key) == value, artifact, f"M264-B001-PKT-{checks_total:02d}", f"{key} drifted", failures)
    checks_total += 1
    checks_passed += require(packet.get("migration_assist_enabled") is expected_migration_assist, artifact, "M264-B001-PKT-MIG", "migration assist truth mismatch", failures)
    for key, expected in EXPECTED_COUNTS.items():
        checks_total += 1
        checks_passed += require(packet.get(key) == expected, artifact, f"M264-B001-PKT-{key}", f"{key} mismatch", failures)
    bool_expectations = {
        "fail_closed": True,
        "semantic_boundary_ready": True,
        "compatibility_mode_semantics_landed": True,
        "migration_assist_semantics_landed": True,
        "source_only_claim_downgrade_semantics_landed": True,
        "unsupported_feature_claim_rejection_semantics_landed": True,
        "strictness_selection_rejection_semantics_landed": True,
        "feature_macro_claim_suppression_semantics_landed": True,
        "selected_configuration_valid": True,
        "selected_configuration_downgraded": False,
        "selected_configuration_rejected": False,
        "ready_for_lowering_and_runtime": True,
        "ready": True,
    }
    for key, expected in bool_expectations.items():
        checks_total += 1
        checks_passed += require(packet.get(key) is expected, artifact, f"M264-B001-PKT-{key}", f"{key} mismatch", failures)
    checks_total += 1
    checks_passed += require(bool(packet.get("semantic_boundary_replay_key")), artifact, "M264-B001-PKT-REPLAY-01", "semantic boundary replay key missing", failures)
    checks_total += 1
    checks_passed += require(bool(packet.get("replay_key")), artifact, "M264-B001-PKT-REPLAY-02", "frontend replay key missing", failures)
    return checks_total, checks_passed


def validate_sema_pass_manager(sema_pass_manager: dict[str, Any], artifact: str, expected_mode: str, expected_migration_assist: bool, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    expectations = {
        "pass_flow_compatibility_mode": expected_mode,
        "pass_flow_migration_assist_enabled": expected_migration_assist,
        "pass_flow_compatibility_handoff_consistent": True,
        "pass_flow_order_matches_contract": True,
        "pass_flow_recovery_replay_contract_satisfied": True,
    }
    for key, expected in expectations.items():
        checks_total += 1
        checks_passed += require(sema_pass_manager.get(key) == expected, artifact, f"M264-B001-SEMA-{key}", f"{key} mismatch", failures)
    return checks_total, checks_passed


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    build = run_command(["npm.cmd", "run", "build:objc3c-native"])
    checks_total += 1
    checks_passed += require(build.returncode == 0, "npm run build:objc3c-native", "M264-B001-DYN-01", f"native build failed: {build.stderr or build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M264-B001-DYN-02", "frontend runner missing after build", failures)
    checks_total += 1
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M264-B001-DYN-03", "native executable missing after build", failures)

    case_specs = [
        {
            "case_id": "hello-canonical-native",
            "fixture": FIXTURE_HELLO,
            "extra_args": [],
            "expected_mode": "canonical",
            "expected_migration_assist": False,
            "tool": "native",
        },
        {
            "case_id": "hello-legacy-migration-runner",
            "fixture": FIXTURE_HELLO,
            "extra_args": ["--objc3-compat-mode", "legacy", "--objc3-migration-assist"],
            "expected_mode": "legacy",
            "expected_migration_assist": True,
            "tool": "runner",
        },
        {
            "case_id": "metadata-canonical-runner",
            "fixture": FIXTURE_METADATA,
            "extra_args": [],
            "expected_mode": "canonical",
            "expected_migration_assist": False,
            "tool": "runner",
        },
    ]
    cases: list[dict[str, Any]] = []
    for spec in case_specs:
        out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m264" / "b001" / spec["case_id"]
        out_dir.mkdir(parents=True, exist_ok=True)
        if spec["tool"] == "runner":
            command = [
                str(args.runner_exe),
                str(spec["fixture"]),
                "--out-dir",
                str(out_dir),
                "--emit-prefix",
                "module",
                "--no-emit-ir",
                "--no-emit-object",
                *spec["extra_args"],
            ]
        else:
            command = [
                str(args.native_exe),
                str(spec["fixture"]),
                "--out-dir",
                str(out_dir),
                "--emit-prefix",
                "module",
                *spec["extra_args"],
            ]
        manifest_path = out_dir / "module.manifest.json"
        run = run_command(command)
        checks_total += 1
        checks_passed += require(run.returncode == 0, spec["case_id"], "M264-B001-DYN-RUN", f"probe failed: {run.stderr or run.stdout}", failures)
        checks_total += 1
        checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M264-B001-DYN-MANIFEST", "manifest missing after probe", failures)
        if not manifest_path.exists():
            continue
        frontend, sema_pass_manager, packet = extract_manifest_surfaces(manifest_path)
        case_total, case_passed = validate_packet(packet, spec["case_id"], spec["expected_mode"], spec["expected_migration_assist"], failures)
        checks_total += case_total
        checks_passed += case_passed
        case_total, case_passed = validate_sema_pass_manager(sema_pass_manager, spec["case_id"], spec["expected_mode"], spec["expected_migration_assist"], failures)
        checks_total += case_total
        checks_passed += case_passed
        cases.append({
            "case_id": spec["case_id"],
            "tool": spec["tool"],
            "manifest": display_path(manifest_path),
            "compatibility_mode": frontend.get("compatibility_mode"),
            "migration_assist": frontend.get("migration_assist"),
            "packet_ready": packet.get("ready"),
            "packet_replay_key": packet.get("replay_key"),
            "sema_pass_manager_present": True,
            "pass_flow_compatibility_mode": sema_pass_manager.get("pass_flow_compatibility_mode"),
            "pass_flow_migration_assist_enabled": sema_pass_manager.get("pass_flow_migration_assist_enabled"),
        })
    return checks_total, checks_passed, {"cases": cases}


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    checks_total += 3
    checks_passed += ensure_snippets(EXPECTATIONS_DOC, [SnippetCheck("M264-B001-DOC-01", CONTRACT_ID), SnippetCheck("M264-B001-DOC-02", SURFACE_PATH), SnippetCheck("M264-B001-DOC-03", "tmp/reports/m264/M264-B001/compatibility_strictness_and_claim_semantics_summary.json")], failures)
    checks_total += 2
    checks_passed += ensure_snippets(PACKET_DOC, [SnippetCheck("M264-B001-PACKET-01", "#7235"), SnippetCheck("M264-B001-PACKET-02", SURFACE_PATH)], failures)
    checks_total += 2
    checks_passed += ensure_snippets(DOC_SOURCE, [SnippetCheck("M264-B001-SRC-01", "Compatibility, strictness, and claim semantics (M264-B001)"), SnippetCheck("M264-B001-SRC-02", SURFACE_PATH)], failures)
    checks_total += 2
    checks_passed += ensure_snippets(DOC_NATIVE, [SnippetCheck("M264-B001-DOCN-01", "Compatibility, strictness, and claim semantics (M264-B001)"), SnippetCheck("M264-B001-DOCN-02", SURFACE_PATH)], failures)
    checks_total += 2
    checks_passed += ensure_snippets(SPEC_CHECKLIST, [SnippetCheck("M264-B001-SPEC-01", "M264 semantic claim legality packet"), SnippetCheck("M264-B001-SPEC-02", SURFACE_PATH)], failures)
    checks_total += 2
    checks_passed += ensure_snippets(SPEC_DECISIONS, [SnippetCheck("M264-B001-DEC-01", "D-018"), SnippetCheck("M264-B001-DEC-02", "Compatibility selections are live")], failures)
    checks_total += 3
    checks_passed += ensure_snippets(SEMA_CONTRACT_H, [SnippetCheck("M264-B001-CODE-01", "kObjc3CompatibilityStrictnessClaimSemanticsContractId"), SnippetCheck("M264-B001-CODE-02", "struct Objc3CompatibilityStrictnessClaimSemanticsSummary"), SnippetCheck("M264-B001-CODE-03", "compatibility_strictness_claim_semantics_summary")], failures)
    checks_total += 3
    checks_passed += ensure_snippets(SEMA_PASSES_CPP, [SnippetCheck("M264-B001-CODE-04", "BuildCompatibilityStrictnessClaimSemanticsSummaryFromIntegrationSurface"), SnippetCheck("M264-B001-CODE-05", "M264-B001 semantic freeze anchor"), SnippetCheck("M264-B001-CODE-06", "compatibility_strictness_claim_semantics_summary")], failures)
    checks_total += 2
    checks_passed += ensure_snippets(SEMA_PASS_FLOW_CPP, [SnippetCheck("M264-B001-CODE-07", "summary.compatibility_mode == Objc3SemaCompatibilityMode::Canonical ||"), SnippetCheck("M264-B001-CODE-08", "summary.compatibility_mode == Objc3SemaCompatibilityMode::Legacy")], failures)
    checks_total += 2
    checks_passed += ensure_snippets(FRONTEND_TYPES_H, [SnippetCheck("M264-B001-CODE-09", "struct Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary"), SnippetCheck("M264-B001-CODE-10", "IsReadyObjc3FrontendCompatibilityStrictnessClaimSemanticsSummary")], failures)
    checks_total += 3
    checks_passed += ensure_snippets(ARTIFACTS_CPP, [SnippetCheck("M264-B001-CODE-11", "BuildFrontendCompatibilityStrictnessClaimSemanticsSummary"), SnippetCheck("M264-B001-CODE-12", "BuildFrontendCompatibilityStrictnessClaimSemanticsSummaryJson"), SnippetCheck("M264-B001-CODE-13", "frontend_compatibility_strictness_claim_semantics_summary =")], failures)
    checks_total += 3
    checks_passed += ensure_snippets(PACKAGE_JSON, [SnippetCheck("M264-B001-PKG-01", 'check:objc3c:m264-b001-compatibility-strictness-and-claim-semantics-contract'), SnippetCheck("M264-B001-PKG-02", 'test:tooling:m264-b001-compatibility-strictness-and-claim-semantics-contract'), SnippetCheck("M264-B001-PKG-03", 'check:objc3c:m264-b001-lane-b-readiness')], failures)

    probe_details: dict[str, Any] = {"cases": []}
    if not args.skip_dynamic_probes:
        dyn_total, dyn_passed, probe_details = run_dynamic_probes(args, failures)
        checks_total += dyn_total
        checks_passed += dyn_passed

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "semantic_model": SEMANTIC_MODEL,
        "downgrade_model": DOWNGRADE_MODEL,
        "rejection_model": REJECTION_MODEL,
        "surface_path": SURFACE_PATH,
        "probe_details": probe_details,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        print(f"[fail] M264-B001 found {len(failures)} issue(s); see {display_path(args.summary_out)}", file=sys.stderr)
        return 1
    print(f"[ok] M264-B001 compatibility/strictness claim semantics summary verified -> {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

#!/usr/bin/env python3
"""Fail-closed checker for M264-B003 canonical-interface/macro truthfulness."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m264-b003-canonical-interface-and-feature-macro-truthfulness-v1"
CONTRACT_ID = "objc3c-canonical-interface-and-feature-macro-truthfulness/m264-b003-v1"
SEMA_CONTRACT_ID = "objc3c-compatibility-strictness-claim-semantics/m264-b001-v1"
TRUTH_SURFACE_CONTRACT_ID = "objc3c-feature-claim-strictness-truth-surface/m264-a002-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_compatibility_strictness_claim_semantics"
EXPECTED_SUMMARY = ROOT / "tmp" / "reports" / "m264" / "M264-B003" / "canonical_interface_and_feature_macro_truthfulness_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m264_canonical_interface_and_feature_macro_truthfulness_edge_case_and_compatibility_completion_b003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m264" / "m264_b003_canonical_interface_and_feature_macro_truthfulness_edge_case_and_compatibility_completion_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_CHECKLIST = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
SPEC_DECISIONS = ROOT / "spec" / "DECISIONS_LOG.md"
SEMA_CONTRACT_H = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASSES_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
FIXTURE_HELLO = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
FIXTURE_METADATA = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m264" / "b003"

EXPECTED_CANONICAL_INTERFACE_TRUTH_MODEL = (
    "no-standalone-interface-payload-yet-and-any-future-canonical-interface-must-stay-bounded-to-runnable-and-source-downgraded-claims"
)
EXPECTED_SEPARATE_COMPILATION_MACRO_TRUTH_MODEL = (
    "suppressed-feature-macro-claims-remain-unpublished-across-manifest-interface-and-conformance-surfaces-until-executable"
)
EXPECTED_CANONICAL_INTERFACE_PAYLOAD_MODE = "no-standalone-interface-payload-yet"
EXPECTED_SUPPRESSED_MACRO_CLAIMS = [
    "macro-claim:__OBJC3_STRICTNESS_LEVEL__",
    "macro-claim:__OBJC3_CONCURRENCY_MODE__",
    "macro-claim:__OBJC3_CONCURRENCY_STRICT__",
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
    parser.add_argument("--runner-exe", type=Path, default=RUNNER_EXE)
    parser.add_argument("--native-exe", type=Path, default=NATIVE_EXE)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected object JSON in {display_path(path)}")
    return payload


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M264-B003-MISSING", f"missing artifact: {display_path(path)}"))
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


def ensure_native_build(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = checks_passed = 0
    completed = run_command([sys.executable, str(BUILD_HELPER), "--mode", "fast", "--reason", "m264-b003-dynamic-check"])
    payload: dict[str, Any] = {
        "returncode": completed.returncode,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    checks_total += 3
    checks_passed += require(completed.returncode == 0, display_path(BUILD_HELPER), "M264-B003-BUILD-01", f"fast native build failed: {completed.stdout}{completed.stderr}", failures)
    checks_passed += require(RUNNER_EXE.exists(), display_path(RUNNER_EXE), "M264-B003-BUILD-02", "frontend runner missing after fast build", failures)
    checks_passed += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M264-B003-BUILD-03", "native executable missing after fast build", failures)
    return checks_total, checks_passed, payload


def extract_packet(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    frontend = manifest.get("frontend")
    if not isinstance(frontend, dict):
        raise TypeError(f"missing frontend block in {display_path(manifest_path)}")
    pipeline = frontend.get("pipeline")
    if not isinstance(pipeline, dict):
        raise TypeError(f"missing frontend.pipeline block in {display_path(manifest_path)}")
    semantic_surface = pipeline.get("semantic_surface")
    if not isinstance(semantic_surface, dict):
        raise TypeError(f"missing frontend.pipeline.semantic_surface block in {display_path(manifest_path)}")
    packet = semantic_surface.get("objc_compatibility_strictness_claim_semantics")
    if not isinstance(packet, dict):
        raise TypeError(f"missing {SURFACE_PATH} in {display_path(manifest_path)}")
    return packet


def compile_case(tool: str, fixture: Path, out_dir: Path, extra_args: Sequence[str]) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    emit_prefix = ["--emit-prefix", "module"]
    if tool == "native":
        command = [str(NATIVE_EXE), str(fixture), "--out-dir", str(out_dir), *emit_prefix, *extra_args]
    elif tool == "runner":
        command = [str(RUNNER_EXE), str(fixture), "--out-dir", str(out_dir), *emit_prefix, "--no-emit-ir", "--no-emit-object", *extra_args]
    else:
        raise ValueError(f"unsupported tool: {tool}")
    return run_command(command)


def validate_packet(packet: dict[str, Any], artifact: str, expected_mode: str, expected_migration_assist: bool, failures: list[Finding]) -> tuple[int, int]:
    checks_total = checks_passed = 0
    exact = {
        "contract_id": SEMA_CONTRACT_ID,
        "feature_claim_truth_surface_contract_id": TRUTH_SURFACE_CONTRACT_ID,
        "frontend_surface_path": SURFACE_PATH,
        "canonical_interface_truth_model": EXPECTED_CANONICAL_INTERFACE_TRUTH_MODEL,
        "separate_compilation_macro_truth_model": EXPECTED_SEPARATE_COMPILATION_MACRO_TRUTH_MODEL,
        "canonical_interface_payload_mode": EXPECTED_CANONICAL_INTERFACE_PAYLOAD_MODE,
        "effective_compatibility_mode": expected_mode,
    }
    for key, expected in exact.items():
        checks_total += 1
        checks_passed += require(packet.get(key) == expected, artifact, f"M264-B003-PKT-{key}", f"{key} drifted", failures)

    checks_total += 1
    checks_passed += require(packet.get("migration_assist_enabled") is expected_migration_assist, artifact, "M264-B003-PKT-migration_assist_enabled", "migration assist truth mismatch", failures)
    checks_total += 1
    checks_passed += require(packet.get("suppressed_macro_claim_count") == len(EXPECTED_SUPPRESSED_MACRO_CLAIMS), artifact, "M264-B003-PKT-suppressed_macro_claim_count", "suppressed macro claim count drifted", failures)
    checks_total += 1
    checks_passed += require(packet.get("suppressed_macro_claim_ids") == EXPECTED_SUPPRESSED_MACRO_CLAIMS, artifact, "M264-B003-PKT-suppressed_macro_claim_ids", "suppressed macro claim ids drifted", failures)

    bools = {
        "fail_closed": True,
        "semantic_boundary_ready": True,
        "compatibility_mode_semantics_landed": True,
        "migration_assist_semantics_landed": True,
        "source_only_claim_downgrade_semantics_landed": True,
        "unsupported_feature_claim_rejection_semantics_landed": True,
        "live_unsupported_feature_source_rejection_landed": True,
        "strictness_selection_rejection_semantics_landed": True,
        "feature_macro_claim_suppression_semantics_landed": True,
        "canonical_interface_truth_semantics_landed": True,
        "separate_compilation_macro_truth_semantics_landed": True,
        "selected_configuration_valid": True,
        "selected_configuration_downgraded": False,
        "selected_configuration_rejected": False,
        "ready_for_lowering_and_runtime": True,
        "ready": True,
    }
    for key, expected in bools.items():
        checks_total += 1
        checks_passed += require(packet.get(key) is expected, artifact, f"M264-B003-PKT-{key}", f"{key} mismatch", failures)

    replay_key = str(packet.get("replay_key", ""))
    checks_total += 4
    checks_passed += require(EXPECTED_CANONICAL_INTERFACE_PAYLOAD_MODE in replay_key, artifact, "M264-B003-PKT-replay_key-payload", "replay key missing payload mode", failures)
    checks_passed += require(EXPECTED_SUPPRESSED_MACRO_CLAIMS[0] in replay_key, artifact, "M264-B003-PKT-replay_key-macro-0", "replay key missing first suppressed macro claim", failures)
    checks_passed += require(EXPECTED_SUPPRESSED_MACRO_CLAIMS[1] in replay_key, artifact, "M264-B003-PKT-replay_key-macro-1", "replay key missing second suppressed macro claim", failures)
    checks_passed += require(EXPECTED_SUPPRESSED_MACRO_CLAIMS[2] in replay_key, artifact, "M264-B003-PKT-replay_key-macro-2", "replay key missing third suppressed macro claim", failures)
    return checks_total, checks_passed


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = checks_passed = 0
    build_total, build_passed, build_payload = ensure_native_build(failures)
    checks_total += build_total
    checks_passed += build_passed

    case_specs = [
        {
            "case_id": "hello-canonical-native",
            "tool": "native",
            "fixture": FIXTURE_HELLO,
            "extra_args": [],
            "expected_mode": "canonical",
            "expected_migration_assist": False,
        },
        {
            "case_id": "hello-legacy-migration-runner",
            "tool": "runner",
            "fixture": FIXTURE_HELLO,
            "extra_args": ["--objc3-compat-mode", "legacy", "--objc3-migration-assist"],
            "expected_mode": "legacy",
            "expected_migration_assist": True,
        },
        {
            "case_id": "metadata-canonical-runner",
            "tool": "runner",
            "fixture": FIXTURE_METADATA,
            "extra_args": [],
            "expected_mode": "canonical",
            "expected_migration_assist": False,
        },
    ]

    cases: list[dict[str, Any]] = []
    for spec in case_specs:
        out_dir = PROBE_ROOT / spec["case_id"]
        completed = compile_case(spec["tool"], spec["fixture"], out_dir, spec["extra_args"])
        case_payload: dict[str, Any] = {
            "case_id": spec["case_id"],
            "tool": spec["tool"],
            "fixture": display_path(spec["fixture"]),
            "returncode": completed.returncode,
            "stdout": completed.stdout,
            "stderr": completed.stderr,
        }
        cases.append(case_payload)
        checks_total += 1
        checks_passed += require(completed.returncode == 0, spec["case_id"], "M264-B003-DYN-compile", f"probe compile failed: {completed.stdout}{completed.stderr}", failures)
        if completed.returncode != 0:
            continue
        manifest_path = out_dir / "module.manifest.json"
        checks_total += 1
        checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M264-B003-DYN-manifest", "probe manifest missing", failures)
        if not manifest_path.exists():
            continue
        packet = extract_packet(manifest_path)
        case_payload["packet"] = {
            "effective_compatibility_mode": packet.get("effective_compatibility_mode"),
            "migration_assist_enabled": packet.get("migration_assist_enabled"),
            "canonical_interface_payload_mode": packet.get("canonical_interface_payload_mode"),
            "suppressed_macro_claim_ids": packet.get("suppressed_macro_claim_ids"),
        }
        sub_total, sub_passed = validate_packet(packet, spec["case_id"], spec["expected_mode"], spec["expected_migration_assist"], failures)
        checks_total += sub_total
        checks_passed += sub_passed

    return checks_total, checks_passed, {
        "build": build_payload,
        "cases": cases,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = checks_passed = 0

    checks_passed += ensure_snippets(EXPECTATIONS_DOC, [
        SnippetCheck("M264-B003-DOC-01", CONTRACT_ID),
        SnippetCheck("M264-B003-DOC-02", SURFACE_PATH),
        SnippetCheck("M264-B003-DOC-03", EXPECTED_CANONICAL_INTERFACE_PAYLOAD_MODE),
    ], failures)
    checks_total += 3
    checks_passed += ensure_snippets(PACKET_DOC, [
        SnippetCheck("M264-B003-PKTDOC-01", "Issue: `#7237`"),
        SnippetCheck("M264-B003-PKTDOC-02", "Dependencies:"),
        SnippetCheck("M264-B003-PKTDOC-03", "M264-B002"),
    ], failures)
    checks_total += 3
    checks_passed += ensure_snippets(DOC_SOURCE, [
        SnippetCheck("M264-B003-SRC-01", "## Canonical interface and feature-macro truthfulness (M264-B003)"),
        SnippetCheck("M264-B003-SRC-02", EXPECTED_CANONICAL_INTERFACE_PAYLOAD_MODE),
    ], failures)
    checks_total += 2
    checks_passed += ensure_snippets(DOC_NATIVE, [
        SnippetCheck("M264-B003-DOCN-01", "## Canonical interface and feature-macro truthfulness (M264-B003)"),
        SnippetCheck("M264-B003-DOCN-02", EXPECTED_CANONICAL_INTERFACE_PAYLOAD_MODE),
    ], failures)
    checks_total += 2
    checks_passed += ensure_snippets(SPEC_CHECKLIST, [
        SnippetCheck("M264-B003-SPC-01", "Current native implementation note (`M264-B003`)"),
        SnippetCheck("M264-B003-SPC-02", SURFACE_PATH),
    ], failures)
    checks_total += 2
    checks_passed += ensure_snippets(SPEC_DECISIONS, [
        SnippetCheck("M264-B003-DEC-01", "Current native implementation note (`M264-B003`)"),
        SnippetCheck("M264-B003-DEC-02", EXPECTED_CANONICAL_INTERFACE_PAYLOAD_MODE),
    ], failures)
    checks_total += 2
    checks_passed += ensure_snippets(SEMA_CONTRACT_H, [
        SnippetCheck("M264-B003-CODE-01", "kObjc3CompatibilityStrictnessClaimCanonicalInterfaceTruthModel"),
        SnippetCheck("M264-B003-CODE-02", "suppressed_macro_claim_ids;"),
        SnippetCheck("M264-B003-CODE-03", "canonical_interface_truth_semantics_landed"),
    ], failures)
    checks_total += 3
    checks_passed += ensure_snippets(SEMA_PASSES_CPP, [
        SnippetCheck("M264-B003-CODE-04", "M264-B003 semantic truth anchor"),
        SnippetCheck("M264-B003-CODE-05", "summary.suppressed_macro_claim_ids = {"),
        SnippetCheck("M264-B003-CODE-06", "summary.canonical_interface_truth_semantics_landed = true;"),
    ], failures)
    checks_total += 3
    checks_passed += ensure_snippets(FRONTEND_TYPES_H, [
        SnippetCheck("M264-B003-CODE-07", "canonical_interface_truth_model"),
        SnippetCheck("M264-B003-CODE-08", "separate_compilation_macro_truth_semantics_landed"),
        SnippetCheck("M264-B003-CODE-09", "suppressed_macro_claim_ids[2] =="),
    ], failures)
    checks_total += 3
    checks_passed += ensure_snippets(ARTIFACTS_CPP, [
        SnippetCheck("M264-B003-CODE-10", "\\\"canonical_interface_truth_model\\\""),
        SnippetCheck("M264-B003-CODE-11", "BuildStringArrayJson(summary.suppressed_macro_claim_ids)"),
        SnippetCheck("M264-B003-CODE-12", "summary.canonical_interface_truth_semantics_landed ="),
    ], failures)
    checks_total += 3
    checks_passed += ensure_snippets(PACKAGE_JSON, [
        SnippetCheck("M264-B003-PKG-01", 'check:objc3c:m264-b003-canonical-interface-and-feature-macro-truthfulness'),
        SnippetCheck("M264-B003-PKG-02", 'test:tooling:m264-b003-canonical-interface-and-feature-macro-truthfulness'),
        SnippetCheck("M264-B003-PKG-03", 'check:objc3c:m264-b003-lane-b-readiness'),
    ], failures)
    checks_total += 3

    dynamic_payload: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        sub_total, sub_passed, dynamic_payload = run_dynamic_probes(args, failures)
        checks_total += sub_total
        checks_passed += sub_passed

    payload = {
        "contract_id": CONTRACT_ID,
        "mode": MODE,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_payload": dynamic_payload,
        "cases": dynamic_payload.get("cases", []),
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.check_id} {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1

    print(f"[ok] {CONTRACT_ID} passed {checks_passed}/{checks_total} checks")
    print(f"[ok] summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

#!/usr/bin/env python3
"""Validate M259-B002 fail-closed unsupported advanced-feature diagnostics."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
import uuid
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m259-b002-fail-closed-unsupported-advanced-feature-diagnostics-v1"
CONTRACT_ID = "objc3c-runnable-core-unsupported-advanced-feature-diagnostics/m259-b002-v1"
GUARD_MODEL = "runnable-core-crossing-into-unsupported-advanced-surfaces-fails-before-lowering-runtime-handoff"
EVIDENCE_MODEL = "a002-runnable-proof-plus-b001-guard-plus-live-o3s221-negative-source-probes"
FAILURE_MODEL = "fail-closed-on-runnable-core-unsupported-advanced-feature-diagnostic-drift"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_compatibility_strictness_claim_semantics"
SEMANTIC_PACKET_CONTRACT_ID = "objc3c-compatibility-strictness-claim-semantics/m264-b001-v1"
A002_CONTRACT_ID = "objc3c-canonical-runnable-sample-set/m259-a002-v1"
B001_CONTRACT_ID = "objc3c-runnable-core-compatibility-guard/m259-b001-v1"
NEXT_ISSUE = "M259-C001"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m259" / "M259-B002" / "fail_closed_unsupported_advanced_feature_diagnostics_summary.json"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m259_b002_fail_closed_unsupported_advanced_feature_diagnostics_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m259" / "m259_b002_fail_closed_unsupported_advanced_feature_diagnostics_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
SEMA_CONTRACT_H = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASSES_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
EXECUTION_SMOKE = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
EXECUTION_REPLAY = ROOT / "scripts" / "check_objc3c_execution_replay_proof.ps1"
PACKAGE_JSON = ROOT / "package.json"
A002_SUMMARY = ROOT / "tmp" / "reports" / "m259" / "M259-A002" / "canonical_runnable_sample_set_summary.json"
B001_SUMMARY = ROOT / "tmp" / "reports" / "m259" / "M259-B001" / "runnable_core_compatibility_guard_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m259_a002_canonical_runnable_sample_set.objc3"
FIXTURE_THROWS = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m259_b002_unsupported_feature_claim_throws.objc3"
FIXTURE_AUTORELEASEPOOL = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m259_b002_unsupported_feature_claim_autoreleasepool.objc3"
FIXTURE_ARC = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m259_b002_unsupported_feature_claim_arc_ownership_qualifier.objc3"

NEGATIVE_EXPECTATIONS = {
    "throws": {
        "fixture": FIXTURE_THROWS,
        "message": "unsupported feature claim: 'throws' is not yet runnable in Objective-C 3 native mode",
    },
    "autoreleasepool": {
        "fixture": FIXTURE_AUTORELEASEPOOL,
        "message": "unsupported feature claim: '@autoreleasepool' is not yet runnable in Objective-C 3 native mode",
    },
    "arc-ownership": {
        "fixture": FIXTURE_ARC,
        "message": "unsupported feature claim: ARC ownership qualifiers are not yet runnable in Objective-C 3 native mode",
    },
}


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--native-exe", type=Path, default=NATIVE_EXE)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
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


def extract_manifest_packet(manifest_path: Path) -> dict[str, Any]:
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


def validate_positive_packet(packet: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    exact_values = {
        "contract_id": SEMANTIC_PACKET_CONTRACT_ID,
        "effective_compatibility_mode": "canonical",
        "selected_configuration_valid": True,
        "selected_configuration_downgraded": False,
        "selected_configuration_rejected": False,
        "ready_for_lowering_and_runtime": True,
        "fail_closed": True,
        "live_unsupported_feature_source_rejection_landed": True,
        "live_unsupported_feature_family_count": 0,
        "live_unsupported_feature_site_count": 0,
        "live_unsupported_feature_diagnostic_count": 0,
        "throws_source_rejection_site_count": 0,
        "blocks_source_rejection_site_count": 0,
        "arc_source_rejection_site_count": 0,
    }
    for key, expected in exact_values.items():
        checks_total += 1
        checks_passed += require(packet.get(key) == expected, artifact, f"M259-B002-POS-{key}", f"{key} drifted", failures)
    checks_total += 1
    checks_passed += require(packet.get("failure_reason", "") == "", artifact, "M259-B002-POS-failure_reason", "failure_reason must stay empty on positive probe", failures)
    checks_total += 1
    checks_passed += require("live_unsupported_feature_sites=0" in str(packet.get("replay_key", "")), artifact, "M259-B002-POS-replay_key", "replay key missing zero-site proof", failures)
    return checks_total, checks_passed


def validate_negative_diagnostics(out_dir: Path, artifact: str, expected_message: str, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    manifest_path = out_dir / "module.manifest.json"
    diagnostics_txt = out_dir / "module.diagnostics.txt"
    diagnostics_json = out_dir / "module.diagnostics.json"

    checks_total += 1
    checks_passed += require(not manifest_path.exists(), artifact, "M259-B002-NEG-manifest", "negative probe must not publish a manifest", failures)
    checks_total += 1
    checks_passed += require(diagnostics_txt.exists(), artifact, "M259-B002-NEG-diagnostics_txt", "diagnostics text artifact missing", failures)
    checks_total += 1
    checks_passed += require(diagnostics_json.exists(), artifact, "M259-B002-NEG-diagnostics_json", "diagnostics json artifact missing", failures)

    if not diagnostics_json.exists():
        return checks_total, checks_passed, {
            "diagnostics_text": display_path(diagnostics_txt),
            "diagnostics_json": display_path(diagnostics_json),
        }

    payload = load_json(diagnostics_json)
    diagnostics = payload.get("diagnostics")
    checks_total += 1
    checks_passed += require(isinstance(diagnostics, list), artifact, "M259-B002-NEG-diagnostics_list", "diagnostics array missing", failures)
    if not isinstance(diagnostics, list):
        diagnostics = []

    checks_total += 1
    checks_passed += require(len(diagnostics) == 1, artifact, "M259-B002-NEG-diagnostic_count", "negative probe must emit exactly one deterministic diagnostic", failures)

    first_diag = diagnostics[0] if diagnostics else {}
    matched = False
    if isinstance(first_diag, dict):
        matched = first_diag.get("code") == "O3S221" and first_diag.get("message") == expected_message
    checks_total += 1
    checks_passed += require(matched, artifact, "M259-B002-NEG-diagnostic_match", "expected O3S221 unsupported-feature diagnostic missing", failures)

    return checks_total, checks_passed, {
        "diagnostics_text": display_path(diagnostics_txt),
        "diagnostics_json": display_path(diagnostics_json),
        "diagnostic": first_diag if isinstance(first_diag, dict) else {},
    }


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    probe_root = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m259" / "b002" / f"probe-{uuid.uuid4().hex}"
    probe_root.mkdir(parents=True, exist_ok=True)

    build = run_command(["npm.cmd", "run", "build:objc3c-native"])
    checks_total += 1
    checks_passed += require(build.returncode == 0, "npm run build:objc3c-native", "M259-B002-DYN-build", f"native build failed: {build.stderr or build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M259-B002-DYN-native", "native executable missing after build", failures)

    positive_out = probe_root / "positive-runnable-core"
    positive_out.mkdir(parents=True, exist_ok=True)
    positive_run = run_command([
        str(args.native_exe),
        str(POSITIVE_FIXTURE),
        "--out-dir",
        str(positive_out),
        "--emit-prefix",
        "module",
    ])
    checks_total += 1
    checks_passed += require(positive_run.returncode == 0, "positive-runnable-core", "M259-B002-DYN-positive_run", f"positive probe failed: {positive_run.stderr or positive_run.stdout}", failures)
    positive_manifest = positive_out / "module.manifest.json"
    checks_total += 1
    checks_passed += require(positive_manifest.exists(), "positive-runnable-core", "M259-B002-DYN-positive_manifest", "positive probe manifest missing", failures)
    positive_details: dict[str, Any] = {
        "fixture": display_path(POSITIVE_FIXTURE),
        "out_dir": display_path(positive_out),
        "manifest": display_path(positive_manifest),
    }
    if positive_manifest.exists():
        packet = extract_manifest_packet(positive_manifest)
        sub_total, sub_passed = validate_positive_packet(packet, "positive-runnable-core", failures)
        checks_total += sub_total
        checks_passed += sub_passed
        positive_details.update(
            {
                "live_unsupported_feature_site_count": packet.get("live_unsupported_feature_site_count"),
                "live_unsupported_feature_diagnostic_count": packet.get("live_unsupported_feature_diagnostic_count"),
                "throws_source_rejection_site_count": packet.get("throws_source_rejection_site_count"),
                "blocks_source_rejection_site_count": packet.get("blocks_source_rejection_site_count"),
                "arc_source_rejection_site_count": packet.get("arc_source_rejection_site_count"),
                "ready_for_lowering_and_runtime": packet.get("ready_for_lowering_and_runtime"),
            }
        )
    checks_total += 1
    checks_passed += require((positive_out / "module.ll").exists(), "positive-runnable-core", "M259-B002-DYN-positive_ir", "positive probe must emit IR", failures)
    checks_total += 1
    checks_passed += require((positive_out / "module.obj").exists(), "positive-runnable-core", "M259-B002-DYN-positive_obj", "positive probe must emit object output", failures)

    negative_cases: list[dict[str, Any]] = []
    for case_id, spec in NEGATIVE_EXPECTATIONS.items():
        out_dir = probe_root / case_id
        out_dir.mkdir(parents=True, exist_ok=True)
        run = run_command([
            str(args.native_exe),
            str(spec["fixture"]),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        ])
        checks_total += 1
        checks_passed += require(run.returncode != 0, case_id, "M259-B002-DYN-negative_run", "negative probe must fail closed", failures)
        sub_total, sub_passed, details = validate_negative_diagnostics(out_dir, case_id, str(spec["message"]), failures)
        checks_total += sub_total
        checks_passed += sub_passed
        negative_cases.append(
            {
                "case_id": case_id,
                "fixture": display_path(Path(spec["fixture"])),
                "expected_message": spec["message"],
                "exit_code": run.returncode,
                **details,
            }
        )

    return checks_total, checks_passed, {
        "probe_root": display_path(probe_root),
        "positive_case": positive_details,
        "negative_cases": negative_cases,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    checks_total += 4
    checks_passed += ensure_snippets(
        EXPECTATIONS_DOC,
        (
            SnippetCheck("M259-B002-DOC-01", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M259-B002-DOC-02", "deterministic `O3S221` diagnostics"),
            SnippetCheck("M259-B002-DOC-03", "block literals remain documented as unsupported"),
            SnippetCheck("M259-B002-DOC-04", "The contract must explicitly hand off to `M259-C001`."),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        PACKET_DOC,
        (
            SnippetCheck("M259-B002-PKT-01", "Packet: `M259-B002`"),
            SnippetCheck("M259-B002-PKT-02", "Issue: `#7211`"),
            SnippetCheck("M259-B002-PKT-03", "frontend.pipeline.semantic_surface.objc_compatibility_strictness_claim_semantics"),
            SnippetCheck("M259-B002-PKT-04", "Next issue: `M259-C001`."),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        DOC_SOURCE,
        (
            SnippetCheck("M259-B002-SRC-01", "## M259 fail-closed unsupported advanced-feature diagnostics (B002)"),
            SnippetCheck("M259-B002-SRC-02", "`O3S221`"),
            SnippetCheck("M259-B002-SRC-03", "`M259-C001`"),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        DOC_NATIVE,
        (
            SnippetCheck("M259-B002-NDOC-01", "## M259 fail-closed unsupported advanced-feature diagnostics (B002)"),
            SnippetCheck("M259-B002-NDOC-02", "ARC ownership qualifiers"),
            SnippetCheck("M259-B002-NDOC-03", "`O3S221`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        LOWERING_SPEC,
        (
            SnippetCheck("M259-B002-SPC-01", "## M259 fail-closed unsupported advanced-feature diagnostics (B002)"),
            SnippetCheck("M259-B002-SPC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-B002-SPC-03", "`throws`"),
            SnippetCheck("M259-B002-SPC-04", "`M259-C001`"),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        METADATA_SPEC,
        (
            SnippetCheck("M259-B002-META-01", "## M259 fail-closed unsupported advanced-feature diagnostics metadata anchors (B002)"),
            SnippetCheck("M259-B002-META-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-B002-META-03", "tmp/reports/m259/M259-B002/fail_closed_unsupported_advanced_feature_diagnostics_summary.json"),
        ),
        failures,
    )
    checks_total += 2
    checks_passed += ensure_snippets(
        SEMA_CONTRACT_H,
        (
            SnippetCheck("M259-B002-SEMA-01", "M259-B002/M264-B002 unsupported-feature enforcement anchor"),
            SnippetCheck("M259-B002-SEMA-02", "live_unsupported_feature_site_count"),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        SEMA_PASSES_CPP,
        (
            SnippetCheck("M259-B002-SEMPASS-01", "M259-B002/M264-B002 unsupported-feature enforcement anchor"),
            SnippetCheck("M259-B002-SEMPASS-02", "DiagnoseUnsupportedFeatureClaimSources"),
            SnippetCheck("M259-B002-SEMPASS-03", "unsupported feature claim: '@autoreleasepool' is not yet runnable in Objective-C 3 native mode"),
        ),
        failures,
    )
    checks_total += 2
    checks_passed += ensure_snippets(
        FRONTEND_TYPES_H,
        (
            SnippetCheck("M259-B002-FRONTEND-01", "M259-B002/M264-B002 unsupported-feature enforcement anchor"),
            SnippetCheck("M259-B002-FRONTEND-02", "live_unsupported_feature_source_rejection_landed"),
        ),
        failures,
    )
    checks_total += 2
    checks_passed += ensure_snippets(
        ARTIFACTS_CPP,
        (
            SnippetCheck("M259-B002-ART-01", "M259-B002/M264-B002 unsupported-feature enforcement anchor"),
            SnippetCheck("M259-B002-ART-02", "live_unsupported_feature_site_count"),
        ),
        failures,
    )
    checks_total += 2
    checks_passed += ensure_snippets(
        EXECUTION_SMOKE,
        (
            SnippetCheck("M259-B002-SMOKE-01", "M259-B002 unsupported-advanced-feature-diagnostics anchor:"),
            SnippetCheck("M259-B002-SMOKE-02", "`O3S221` fail-closed diagnostics"),
        ),
        failures,
    )
    checks_total += 2
    checks_passed += ensure_snippets(
        EXECUTION_REPLAY,
        (
            SnippetCheck("M259-B002-REPLAY-01", "M259-B002 unsupported-advanced-feature-diagnostics anchor:"),
            SnippetCheck("M259-B002-REPLAY-02", "`O3S221` fail-closed diagnostics"),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        PACKAGE_JSON,
        (
            SnippetCheck("M259-B002-PKG-01", '"check:objc3c:m259-b002-fail-closed-unsupported-advanced-feature-diagnostics"'),
            SnippetCheck("M259-B002-PKG-02", '"test:tooling:m259-b002-fail-closed-unsupported-advanced-feature-diagnostics"'),
            SnippetCheck("M259-B002-PKG-03", '"check:objc3c:m259-b002-lane-b-readiness"'),
        ),
        failures,
    )

    a002_summary = load_json(A002_SUMMARY)
    b001_summary = load_json(B001_SUMMARY)
    checks_total += 2
    checks_passed += require(a002_summary.get("contract_id") == A002_CONTRACT_ID, display_path(A002_SUMMARY), "M259-B002-A002-contract_id", "M259-A002 contract drift", failures)
    checks_passed += require(a002_summary.get("ok") is True, display_path(A002_SUMMARY), "M259-B002-A002-ok", "M259-A002 summary must remain green", failures)
    checks_total += 2
    checks_passed += require(b001_summary.get("contract_id") == B001_CONTRACT_ID, display_path(B001_SUMMARY), "M259-B002-B001-contract_id", "M259-B001 contract drift", failures)
    checks_passed += require(b001_summary.get("ok") is True, display_path(B001_SUMMARY), "M259-B002-B001-ok", "M259-B001 summary must remain green", failures)

    probe_details: dict[str, Any] = {"positive_case": {}, "negative_cases": []}
    if not args.skip_dynamic_probes:
        dyn_total, dyn_passed, probe_details = run_dynamic_probes(args, failures)
        checks_total += dyn_total
        checks_passed += dyn_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "guard_model": GUARD_MODEL,
        "evidence_model": EVIDENCE_MODEL,
        "failure_model": FAILURE_MODEL,
        "surface_path": SURFACE_PATH,
        "next_issue": NEXT_ISSUE,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "ok": not failures,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "failures": [finding.__dict__ for finding in failures],
        "dependency": {
            "M259-A002": {
                "summary": display_path(A002_SUMMARY),
                "contract_id": a002_summary.get("contract_id"),
                "ok": a002_summary.get("ok"),
            },
            "M259-B001": {
                "summary": display_path(B001_SUMMARY),
                "contract_id": b001_summary.get("contract_id"),
                "ok": b001_summary.get("ok"),
            },
        },
        "probe_details": probe_details,
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    print(canonical_json(summary), end="")
    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

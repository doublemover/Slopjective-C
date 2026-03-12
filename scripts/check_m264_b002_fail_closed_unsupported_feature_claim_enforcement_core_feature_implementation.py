#!/usr/bin/env python3
"""Fail-closed checker for M264-B002 unsupported-feature claim enforcement."""

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
MODE = "m264-b002-fail-closed-unsupported-feature-claim-enforcement-v1"
CONTRACT_ID = "objc3c-fail-closed-unsupported-feature-claim-enforcement/m264-b002-v1"
B001_CONTRACT_ID = "objc3c-compatibility-strictness-claim-semantics/m264-b001-v1"
A001_CONTRACT_ID = "objc3c-runnable-feature-claim-inventory/m264-a001-v1"
A002_CONTRACT_ID = "objc3c-feature-claim-strictness-truth-surface/m264-a002-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_compatibility_strictness_claim_semantics"
REJECTION_MODEL = "accepted-unsupported-source-surfaces-fail-before-lowering-runtime-handoff"
EXPECTED_SUMMARY = ROOT / "tmp" / "reports" / "m264" / "M264-B002" / "fail_closed_unsupported_feature_claim_enforcement_summary.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
FIXTURE_HELLO = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
FIXTURE_METADATA = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
FIXTURE_THROWS = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m264_unsupported_feature_claim_throws.objc3"
FIXTURE_ARC = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m264_unsupported_feature_claim_arc_ownership_qualifier.objc3"
FIXTURE_ARC_RETURN = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m264_unsupported_feature_claim_arc_return_ownership.objc3"

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

NEGATIVE_EXPECTATIONS = {
    "throws": {
        "fixture": FIXTURE_THROWS,
        "message": "unsupported feature claim: 'throws' is not yet runnable in Objective-C 3 native mode",
    },
    "arc-ownership": {
        "fixture": FIXTURE_ARC,
        "message": "unsupported feature claim: ARC ownership qualifiers are not yet runnable in Objective-C 3 native mode",
    },
    "arc-return-ownership": {
        "fixture": FIXTURE_ARC_RETURN,
        "message": "unsupported feature claim: ARC ownership qualifiers are not yet runnable in Objective-C 3 native mode",
    },
}

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m264_fail_closed_unsupported_feature_claim_enforcement_core_feature_implementation_b002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m264" / "m264_b002_fail_closed_unsupported_feature_claim_enforcement_core_feature_implementation_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_CHECKLIST = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
SPEC_DECISIONS = ROOT / "spec" / "DECISIONS_LOG.md"
SEMA_CONTRACT_H = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASSES_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
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
        failures.append(Finding(display_path(path), "M264-B002-MISSING", f"missing artifact: {display_path(path)}"))
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
        "contract_id": B001_CONTRACT_ID,
        "runnable_feature_claim_inventory_contract_id": A001_CONTRACT_ID,
        "feature_claim_truth_surface_contract_id": A002_CONTRACT_ID,
        "frontend_surface_path": SURFACE_PATH,
        "rejection_model": "strictness-strict-concurrency-and-feature-macro-claims-remain-fail-closed",
    }
    for key, expected in exact_values.items():
        checks_total += 1
        checks_passed += require(packet.get(key) == expected, artifact, f"M264-B002-POS-{key}", f"{key} drifted", failures)

    for key, expected in EXPECTED_COUNTS.items():
        checks_total += 1
        checks_passed += require(packet.get(key) == expected, artifact, f"M264-B002-POS-{key}", f"{key} mismatch", failures)

    zero_fields = [
        "live_unsupported_feature_family_count",
        "live_unsupported_feature_site_count",
        "live_unsupported_feature_diagnostic_count",
        "throws_source_rejection_site_count",
        "blocks_source_rejection_site_count",
        "arc_source_rejection_site_count",
    ]
    for key in zero_fields:
        checks_total += 1
        checks_passed += require(packet.get(key) == 0, artifact, f"M264-B002-POS-{key}", f"{key} must stay zero on positive probes", failures)

    bool_expectations = {
        "fail_closed": True,
        "semantic_boundary_ready": True,
        "compatibility_mode_semantics_landed": True,
        "migration_assist_semantics_landed": True,
        "source_only_claim_downgrade_semantics_landed": True,
        "unsupported_feature_claim_rejection_semantics_landed": True,
        "live_unsupported_feature_source_rejection_landed": True,
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
        checks_passed += require(packet.get(key) is expected, artifact, f"M264-B002-POS-{key}", f"{key} mismatch", failures)

    checks_total += 1
    checks_passed += require(packet.get("failure_reason", "") == "", artifact, "M264-B002-POS-failure_reason", "failure_reason must stay empty on positive probes", failures)
    checks_total += 1
    checks_passed += require("live_unsupported_feature_sites=0" in str(packet.get("replay_key", "")), artifact, "M264-B002-POS-replay_key", "replay key missing zero-site proof", failures)
    checks_total += 1
    checks_passed += require(
        "live_unsupported_feature_sites=0"
        in str(packet.get("semantic_boundary_replay_key", "")),
        artifact,
        "M264-B002-POS-semantic_boundary_replay_key",
        "semantic boundary replay key missing zero-site proof",
        failures,
    )
    return checks_total, checks_passed


def validate_negative_diagnostics(out_dir: Path, artifact: str, expected_message: str, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    manifest_path = out_dir / "module.manifest.json"
    diagnostics_txt = out_dir / "module.diagnostics.txt"
    diagnostics_json = out_dir / "module.diagnostics.json"

    checks_total += 1
    checks_passed += require(not manifest_path.exists(), artifact, "M264-B002-NEG-manifest", "negative probe must not publish a manifest", failures)
    checks_total += 1
    checks_passed += require(diagnostics_txt.exists(), artifact, "M264-B002-NEG-diagnostics_txt", "diagnostics text artifact missing", failures)
    checks_total += 1
    checks_passed += require(diagnostics_json.exists(), artifact, "M264-B002-NEG-diagnostics_json", "diagnostics json artifact missing", failures)

    if not diagnostics_json.exists():
        return checks_total, checks_passed, {
            "diagnostics_text": display_path(diagnostics_txt),
            "diagnostics_json": display_path(diagnostics_json),
        }

    payload = load_json(diagnostics_json)
    diagnostics = payload.get("diagnostics")
    checks_total += 1
    checks_passed += require(isinstance(diagnostics, list), artifact, "M264-B002-NEG-diagnostics_list", "diagnostics array missing", failures)
    if not isinstance(diagnostics, list):
        diagnostics = []

    checks_total += 1
    checks_passed += require(len(diagnostics) == 1, artifact, "M264-B002-NEG-diagnostic_count", "negative probe must emit exactly one deterministic diagnostic", failures)

    matched = False
    first_diag = diagnostics[0] if diagnostics else {}
    if isinstance(first_diag, dict):
        matched = first_diag.get("code") == "O3S221" and first_diag.get("message") == expected_message
    checks_total += 1
    checks_passed += require(matched, artifact, "M264-B002-NEG-diagnostic_match", "expected O3S221 unsupported-feature diagnostic missing", failures)

    return checks_total, checks_passed, {
        "diagnostics_text": display_path(diagnostics_txt),
        "diagnostics_json": display_path(diagnostics_json),
        "diagnostic": first_diag if isinstance(first_diag, dict) else {},
    }


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    probe_root = (
        ROOT
        / "tmp"
        / "artifacts"
        / "compilation"
        / "objc3c-native"
        / "m264"
        / "b002"
        / f"probe-{uuid.uuid4().hex}"
    )
    probe_root.mkdir(parents=True, exist_ok=True)
    build = run_command(["npm.cmd", "run", "build:objc3c-native"])
    checks_total += 1
    checks_passed += require(build.returncode == 0, "npm run build:objc3c-native", "M264-B002-DYN-build", f"native build failed: {build.stderr or build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M264-B002-DYN-native", "native executable missing after build", failures)
    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M264-B002-DYN-runner", "frontend runner missing after build", failures)

    positive_cases: list[dict[str, Any]] = []
    for case_id, fixture, tool in [
        ("hello-canonical-native", FIXTURE_HELLO, "native"),
        ("metadata-canonical-runner", FIXTURE_METADATA, "runner"),
    ]:
        out_dir = probe_root / case_id
        out_dir.mkdir(parents=True, exist_ok=True)
        if tool == "runner":
            command = [
                str(args.runner_exe),
                str(fixture),
                "--out-dir",
                str(out_dir),
                "--emit-prefix",
                "module",
                "--no-emit-ir",
                "--no-emit-object",
            ]
        else:
            command = [
                str(args.native_exe),
                str(fixture),
                "--out-dir",
                str(out_dir),
                "--emit-prefix",
                "module",
            ]
        run = run_command(command)
        checks_total += 1
        checks_passed += require(run.returncode == 0, case_id, "M264-B002-DYN-positive_run", f"positive probe failed: {run.stderr or run.stdout}", failures)
        manifest_path = out_dir / "module.manifest.json"
        checks_total += 1
        checks_passed += require(manifest_path.exists(), case_id, "M264-B002-DYN-positive_manifest", "positive probe manifest missing", failures)
        if not manifest_path.exists():
            continue
        packet = extract_manifest_packet(manifest_path)
        sub_total, sub_passed = validate_positive_packet(packet, case_id, failures)
        checks_total += sub_total
        checks_passed += sub_passed
        if tool == "native":
            checks_total += 1
            checks_passed += require((out_dir / "module.ll").exists(), case_id, "M264-B002-DYN-positive_ir", "native positive probe must emit IR", failures)
            checks_total += 1
            checks_passed += require((out_dir / "module.obj").exists(), case_id, "M264-B002-DYN-positive_obj", "native positive probe must emit object output", failures)
        positive_cases.append(
            {
                "case_id": case_id,
                "tool": tool,
                "fixture": display_path(fixture),
                "manifest": display_path(manifest_path),
                "live_unsupported_feature_site_count": packet.get("live_unsupported_feature_site_count"),
                "live_unsupported_feature_diagnostic_count": packet.get("live_unsupported_feature_diagnostic_count"),
                "throws_source_rejection_site_count": packet.get("throws_source_rejection_site_count"),
                "blocks_source_rejection_site_count": packet.get("blocks_source_rejection_site_count"),
                "arc_source_rejection_site_count": packet.get("arc_source_rejection_site_count"),
                "ready_for_lowering_and_runtime": packet.get("ready_for_lowering_and_runtime"),
                "ready": packet.get("ready"),
            }
        )

    negative_cases: list[dict[str, Any]] = []
    for case_id, spec in NEGATIVE_EXPECTATIONS.items():
        out_dir = probe_root / case_id
        out_dir.mkdir(parents=True, exist_ok=True)
        command = [
            str(args.native_exe),
            str(spec["fixture"]),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        ]
        run = run_command(command)
        checks_total += 1
        checks_passed += require(run.returncode != 0, case_id, "M264-B002-DYN-negative_run", "negative probe must fail closed", failures)
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
        "positive_cases": positive_cases,
        "negative_cases": negative_cases,
        "rejection_model": REJECTION_MODEL,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    checks_total += 3
    checks_passed += ensure_snippets(
        EXPECTATIONS_DOC,
        [
            SnippetCheck("M264-B002-DOC-01", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M264-B002-DOC-02", SURFACE_PATH),
            SnippetCheck("M264-B002-DOC-03", "O3S221"),
        ],
        failures,
    )
    checks_total += 2
    checks_passed += ensure_snippets(
        PACKET_DOC,
        [
            SnippetCheck("M264-B002-PACKET-01", "#7236"),
            SnippetCheck("M264-B002-PACKET-02", SURFACE_PATH),
        ],
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        DOC_SOURCE,
        [
            SnippetCheck("M264-B002-SRC-01", "Fail-closed unsupported-feature claim enforcement (M264-B002)"),
            SnippetCheck("M264-B002-SRC-02", "throws"),
            SnippetCheck("M264-B002-SRC-03", "ARC ownership-qualified returns"),
        ],
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        DOC_NATIVE,
        [
            SnippetCheck("M264-B002-DOCN-01", "Fail-closed unsupported-feature claim enforcement (M264-B002)"),
            SnippetCheck("M264-B002-DOCN-02", "ARC ownership qualifiers"),
            SnippetCheck("M264-B002-DOCN-03", "ARC ownership-qualified returns"),
        ],
        failures,
    )
    checks_total += 2
    checks_passed += ensure_snippets(
        SPEC_CHECKLIST,
        [
            SnippetCheck("M264-B002-SPEC-01", "accepted unsupported-source rejection gate"),
            SnippetCheck("M264-B002-SPEC-02", "ARC ownership-qualified returns"),
        ],
        failures,
    )
    checks_total += 2
    checks_passed += ensure_snippets(
        SPEC_DECISIONS,
        [
            SnippetCheck("M264-B002-DEC-01", "D-019"),
            SnippetCheck("M264-B002-DEC-02", "ARC ownership-qualified returns"),
        ],
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        SEMA_CONTRACT_H,
        [
            SnippetCheck("M264-B002-CODE-01", "live_unsupported_feature_site_count"),
            SnippetCheck("M264-B002-CODE-02", "throws_source_rejection_site_count"),
            SnippetCheck("M264-B002-CODE-03", "blocks_source_rejection_site_count"),
            SnippetCheck("M264-B002-CODE-04", "arc_source_rejection_site_count"),
        ],
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        SEMA_PASSES_CPP,
        [
            SnippetCheck("M264-B002-CODE-05", "BuildCompatibilityStrictnessClaimSemanticsSummaryFromIntegrationSurface"),
            SnippetCheck("M264-B002-CODE-06", "unsupported_feature_enforcement.live_unsupported_feature_site_count == 0u"),
            SnippetCheck("M264-B002-CODE-07", "DiagnoseUnsupportedFeatureClaimSources"),
            SnippetCheck("M264-B002-CODE-08", "unsupported feature claim: block literals are not yet runnable in Objective-C 3 native mode"),
            SnippetCheck("M264-B002-CODE-09", "unsupported feature claim: ARC ownership qualifiers are not yet runnable in Objective-C 3 native mode"),
        ],
        failures,
    )
    checks_total += 2
    checks_passed += ensure_snippets(
        FRONTEND_TYPES_H,
        [
            SnippetCheck("M264-B002-CODE-10", "live_unsupported_feature_site_count"),
            SnippetCheck("M264-B002-CODE-11", "live_unsupported_feature_source_rejection_landed"),
        ],
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        ARTIFACTS_CPP,
        [
            SnippetCheck("M264-B002-CODE-12", '\\"live_unsupported_feature_site_count\\":'),
            SnippetCheck("M264-B002-CODE-13", '\\"throws_source_rejection_site_count\\":'),
            SnippetCheck("M264-B002-CODE-14", '\\"arc_source_rejection_site_count\\":'),
        ],
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        PACKAGE_JSON,
        [
            SnippetCheck("M264-B002-PKG-01", "check:objc3c:m264-b002-fail-closed-unsupported-feature-claim-enforcement"),
            SnippetCheck("M264-B002-PKG-02", "test:tooling:m264-b002-fail-closed-unsupported-feature-claim-enforcement"),
            SnippetCheck("M264-B002-PKG-03", "check:objc3c:m264-b002-lane-b-readiness"),
        ],
        failures,
    )

    probe_details: dict[str, Any] = {"positive_cases": [], "negative_cases": []}
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
        "surface_path": SURFACE_PATH,
        "rejection_model": REJECTION_MODEL,
        "probe_details": probe_details,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        print(f"[fail] M264-B002 found {len(failures)} issue(s); see {display_path(args.summary_out)}", file=sys.stderr)
        return 1
    print(f"[ok] M264-B002 unsupported-feature claim enforcement verified -> {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

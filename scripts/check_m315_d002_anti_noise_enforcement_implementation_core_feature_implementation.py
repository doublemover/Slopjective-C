#!/usr/bin/env python3
"""Checker for M315-D002 anti-noise enforcement implementation."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence


ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m315" / "M315-D002" / "anti_noise_enforcement_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m315_anti_noise_enforcement_implementation_core_feature_implementation_d002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_d002_anti_noise_enforcement_implementation_core_feature_implementation_packet.md"
RESULT_JSON = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_d002_anti_noise_enforcement_implementation_core_feature_implementation_result.json"
B005_CHECKER = ROOT / "scripts" / "check_m315_b005_comment_constexpr_and_contract_string_decontamination_sweep_edge_case_and_compatibility_completion.py"
B005_SUMMARY = ROOT / "tmp" / "reports" / "m315" / "M315-B005" / "comment_constexpr_contract_string_decontamination_summary.json"
WORKFLOW_PATH = ROOT / ".github" / "workflows" / "m315-source-hygiene-proof-policy-enforcement.yml"
PACKAGE_JSON = ROOT / "package.json"

TARGET_FILES = [
    ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h",
    ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp",
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_final_readiness_gate_core_feature_implementation_surface.h",
]
TARGET_LEGACY_LITERALS = [
    "generalized-foreign-exception-abi-and-runtime-bridge-helper-contract-remain-deferred-to-m267-d001",
    "m260-d002-runtime-baseline-plus-m262-c004-lowering-plus-m262-d001-private-helper-surface",
    "m262-d002-live-helper-runtime-plus-private-bootstrap-internal-debug-snapshots",
    "no-runnable-arc-closeout-matrix-no-public-runtime-abi-widening-no-cross-module-arc-claims-before-m262-e002",
    "gate-consumes-m260-c002-d001-d002-contract-summaries-and-runtime-probe-evidence",
    "live-actor-thunk-bodies-mailbox-runtime-entrypoints-and-runnable-cross-actor-scheduling-remain-later-m270-c002-and-m270-c003-work",
    "live-cleanup-runtime-carriers-borrowed-lifetime-enforcement-and-runnable-retainable-family-runtime-interop-remain-later-m271-lane-d-work",
    "live-direct-call-selector-bypass-runtime-dispatch-boundary-realization-and-runnable-metadata-consumption-remain-later-m272-lane-c-and-lane-d-work",
    "runnable-derive-body-emission-macro-execution-and-property-behavior-runtime-materialization-remain-later-m273-lane-c-and-lane-d-work",
    "live-ffi-call-lowering-ownership-bridge-helper-emission-error-runtime-integration-and-cross-module-runtime-consumption-remain-later-m274-lane-c-and-lane-d-work",
    "cross-module-runtime-consumption-live-foreign-linking-and-runnable-host-language-integration-remain-later-m274-lane-d-and-lane-e-work",
    "byref-forwarding-and-owned-capture-escaping-block-lifetimes-remain-deferred-until-next-runtime-phase-and-m261-d003",
]
PARITY_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "native" / "library_cli_parity"
PARITY_LLS = [PARITY_ROOT / "library" / "module.ll", PARITY_ROOT / "cli" / "module.ll"]
PARITY_JSONS = [
    PARITY_ROOT / "library" / "module.manifest.json",
    PARITY_ROOT / "cli" / "module.manifest.json",
    PARITY_ROOT / "golden_summary.json",
]


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def read_json(path: Path) -> dict[str, object]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise ValueError(f"expected JSON object at {path}")
    return payload


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def tracked_paths(glob: str) -> list[str]:
    output = subprocess.check_output(["git", "ls-files", glob], cwd=ROOT, text=True)
    return [line.replace("\\", "/") for line in output.splitlines() if line.strip()]


def run_checker(path: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run([sys.executable, str(path)], cwd=ROOT, capture_output=True, text=True, check=False)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    result = read_json(RESULT_JSON)
    b005_run = run_checker(B005_CHECKER)
    b005_summary = read_json(B005_SUMMARY)
    tracked_compiler_sources = tracked_paths("compiler/objc3c/*.py")
    tracked_parity_files = tracked_paths("tests/tooling/fixtures/native/library_cli_parity/**")
    workflow = read_text(WORKFLOW_PATH)
    package_json = read_text(PACKAGE_JSON)

    checks_total += 6
    checks_passed += require("objc3c.cleanup.anti-noise.enforcement/m315-d002-v1" in expectations, str(EXPECTATIONS_DOC), "M315-D002-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("advanced_integration_closeout_signoff" in expectations, str(EXPECTATIONS_DOC), "M315-D002-EXP-02", "expectations missing stable identifier family", failures)
    checks_passed += require("B005 native-source sweep reports only the quarantined residual classes and `57` remaining milestone-token lines" in packet, str(PACKET_DOC), "M315-D002-PKT-01", "packet missing B005 target", failures)
    checks_passed += require(result.get("stable_identifier_family") == "advanced_integration_closeout_signoff", str(RESULT_JSON), "M315-D002-RES-01", "stable identifier family drifted", failures)
    checks_passed += require(result.get("post_cleanup_native_source_milestone_token_lines") == 57, str(RESULT_JSON), "M315-D002-RES-02", "post-cleanup milestone-token target drifted", failures)
    checks_passed += require(result.get("next_issue") == "M315-E001", str(RESULT_JSON), "M315-D002-RES-03", "next issue drifted", failures)

    checks_total += 6
    checks_passed += require(b005_run.returncode == 0, str(B005_CHECKER), "M315-D002-B005-01", f"B005 checker failed: {b005_run.stderr.strip()}", failures)
    checks_passed += require(b005_summary.get("match_count") == 57, str(B005_SUMMARY), "M315-D002-B005-02", "B005 match count drifted", failures)
    checks_passed += require(b005_summary.get("disallowed_count") == 0, str(B005_SUMMARY), "M315-D002-B005-03", "B005 disallowed count drifted", failures)
    checks_passed += require(
        b005_summary.get("residual_class_counts") == {
            "dependency_issue_array": 3,
            "issue_key_schema_field": 8,
            "legacy_fixture_path_reference": 6,
            "next_issue_schema_field": 40,
        },
        str(B005_SUMMARY),
        "M315-D002-B005-04",
        "B005 residual class counts drifted",
        failures,
    )
    checks_passed += require("legacy_m248_surface_identifier" not in b005_summary.get("residual_class_counts", {}), str(B005_SUMMARY), "M315-D002-B005-05", "legacy m248 residual class still present in B005 summary", failures)
    checks_passed += require("transitional_source_model" not in b005_summary.get("residual_class_counts", {}), str(B005_SUMMARY), "M315-D002-B005-06", "transitional source-model residual class still present in B005 summary", failures)

    zero_target_hits: dict[str, list[dict[str, object]]] = {}
    checks_total += 11
    for path in TARGET_FILES:
        rel = path.relative_to(ROOT).as_posix()
        text = read_text(path)
        hits = []
        for lineno, line in enumerate(text.splitlines(), start=1):
            if "m248_integration_closeout_signoff" in line or "BuildObjc3FinalReadinessGateM248IntegrationCloseoutSignoffKey" in line:
                hits.append({"line": lineno, "text": line.strip()})
            elif any(literal in line for literal in TARGET_LEGACY_LITERALS):
                hits.append({"line": lineno, "text": line.strip()})
        zero_target_hits[rel] = hits
        checks_passed += require(not hits, rel, "M315-D002-SRC-01", "target product-code file still contains zero-target residue", failures)
    frontend_types = read_text(TARGET_FILES[2])
    gate_surface = read_text(TARGET_FILES[3])
    checks_passed += require("advanced_integration_closeout_signoff_consistent" in frontend_types, str(TARGET_FILES[2]), "M315-D002-SRC-02", "frontend types missing stable consistency field", failures)
    checks_passed += require("advanced_integration_closeout_signoff_ready" in frontend_types, str(TARGET_FILES[2]), "M315-D002-SRC-03", "frontend types missing stable ready field", failures)
    checks_passed += require("advanced_integration_closeout_signoff_key" in frontend_types, str(TARGET_FILES[2]), "M315-D002-SRC-04", "frontend types missing stable key field", failures)
    checks_passed += require("BuildObjc3FinalReadinessGateAdvancedIntegrationCloseoutSignoffKey" in gate_surface, str(TARGET_FILES[3]), "M315-D002-SRC-05", "gate surface missing stable builder", failures)
    checks_passed += require("final-readiness-gate-advanced-integration-closeout-signoff:v1:" in gate_surface, str(TARGET_FILES[3]), "M315-D002-SRC-06", "gate surface missing stable key literal", failures)
    checks_passed += require("m248_integration_closeout_signoff" not in gate_surface and "m248_integration_closeout_signoff" not in frontend_types, str(TARGET_FILES[3]), "M315-D002-SRC-07", "legacy m248 identifiers still present in product code", failures)

    checks_total += 11
    checks_passed += require(tracked_compiler_sources == [], "compiler/objc3c/*.py", "M315-D002-GRD-01", "tracked prototype compiler Python sources reappeared", failures)
    checks_passed += require(len(tracked_parity_files) == 9, str(PARITY_ROOT), "M315-D002-GRD-02", "tracked parity fixture footprint drifted", failures)
    for ll in PARITY_LLS:
        head = "\n".join(read_text(ll).splitlines()[:6])
        checks_passed += require(
            "artifact_family_id: objc3c.fixture.synthetic.librarycliparity.v1" in head
            and "provenance_class: synthetic_fixture" in head,
            str(ll),
            "M315-D002-GRD-03",
            "synthetic ll authenticity header drifted",
            failures,
        )
    for path in PARITY_JSONS:
        payload = read_json(path)
        checks_passed += require(isinstance(payload.get("artifact_authenticity"), dict), str(path), "M315-D002-GRD-04", "parity JSON missing artifact_authenticity envelope", failures)
    checks_passed += require("python scripts/check_m315_d002_anti_noise_enforcement_implementation_core_feature_implementation.py" in workflow, str(WORKFLOW_PATH), "M315-D002-WF-01", "workflow missing D002 checker command", failures)
    checks_passed += require("workflow_dispatch:" in workflow and "push:" in workflow, str(WORKFLOW_PATH), "M315-D002-WF-02", "workflow missing dispatch or push trigger", failures)
    checks_passed += require("\"check:objc3c:m315-d002-anti-noise-enforcement-implementation-core-feature-implementation\"" in package_json, str(PACKAGE_JSON), "M315-D002-PKG-01", "package.json missing D002 checker script", failures)
    checks_passed += require("\"test:tooling:m315-d002-anti-noise-enforcement-implementation-core-feature-implementation\"" in package_json, str(PACKAGE_JSON), "M315-D002-PKG-02", "package.json missing D002 pytest script", failures)
    checks_passed += require("\"check:objc3c:m315-d002-lane-d-readiness\"" in package_json, str(PACKAGE_JSON), "M315-D002-PKG-03", "package.json missing D002 readiness script", failures)

    summary = {
        "mode": result["mode"],
        "contract_id": result["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "post_cleanup_native_source_milestone_token_lines": b005_summary.get("match_count"),
        "remaining_quarantined_residual_classes": b005_summary.get("residual_class_counts"),
        "target_file_zero_target_hits": zero_target_hits,
        "stable_identifier_family": result.get("stable_identifier_family"),
        "tracked_compiler_python_sources": tracked_compiler_sources,
        "tracked_parity_fixture_files": tracked_parity_files,
        "workflow_path": result.get("workflow_path"),
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M315-D002 anti-noise enforcement checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

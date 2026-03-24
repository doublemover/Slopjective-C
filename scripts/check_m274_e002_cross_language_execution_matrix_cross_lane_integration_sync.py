#!/usr/bin/env python3
"""Fail-closed checker for M274-E002 cross-language execution matrix closeout."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m274-e002-cross-language-execution-matrix-v1"
CONTRACT_ID = "objc3c-part11-cross-language-execution-matrix/m274-e002-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m274" / "M274-E002" / "cross_language_execution_matrix_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m274_cross_language_execution_matrix_cross_lane_integration_sync_e002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m274" / "m274_e002_cross_language_execution_matrix_cross_lane_integration_sync_packet.md"
PACKAGE_JSON = ROOT / "package.json"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
ATTRIBUTE_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
TABLES_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MANIFEST_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
FRONTEND_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
READINESS_RUNNER = ROOT / "scripts" / "run_m274_e002_lane_e_readiness.py"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
DOC_BUILDER = ROOT / "scripts" / "build_objc3c_native_docs.py"
ENSURE_BUILD_SUMMARY = ROOT / "tmp" / "reports" / "m274" / "M274-E002" / "ensure_objc3c_native_build_summary.json"

A003_CONTRACT_ID = "objc3c-part11-foreign-surface-interface-preservation/m274-a003-v1"
B004_CONTRACT_ID = "objc3c-part11-swift-metadata-and-isolation-mapping-completion/m274-b004-v1"
C003_CONTRACT_ID = "objc3c-part11-ffi-metadata-interface-preservation/m274-c003-v1"
D002_CONTRACT_ID = "objc3c-part11-header-module-and-bridge-generation/m274-d002-v1"
E001_CONTRACT_ID = "objc3c-part11-interop-conformance-gate/m274-e001-v1"

A003_CHECKER = ROOT / "scripts" / "check_m274_a003_foreign_surfaces_interface_and_module_preservation_completion_core_feature_expansion.py"
B004_CHECKER = ROOT / "scripts" / "check_m274_b004_part11_swift_metadata_and_isolation_mapping_completion_edge_case_and_compatibility_completion.py"
C003_CHECKER = ROOT / "scripts" / "check_m274_c003_metadata_and_interface_preservation_across_ffi_boundaries_core_feature_expansion.py"
D002_CHECKER = ROOT / "scripts" / "check_m274_d002_header_module_and_bridge_generation_implementation_core_feature_implementation.py"
E001_CHECKER = ROOT / "scripts" / "check_m274_e001_interop_conformance_gate_contract_and_architecture_freeze.py"

NEXT_ISSUE = "M275-A001"
UPSTREAM_HANDOFF_ISSUE = "M274-E001"
EVIDENCE_MODEL = "a003-through-e001-summary-chain-plus-d002-live-header-module-bridge-proof"
FAILURE_MODEL = "fail-closed-on-part11-closeout-matrix-drift"
CLOSEOUT_MODEL = "lane-e-closeout-replays-supported-c-cpp-swift-facing-interop-slice-without-claiming-broader-foreign-runtime-execution-than-d002-proves"

UPSTREAM_SPECS: tuple[tuple[str, Path, str], ...] = (
    ("M274-A003", ROOT / "tmp" / "reports" / "m274" / "M274-A003" / "foreign_surfaces_interface_module_preservation_summary.json", A003_CONTRACT_ID),
    ("M274-B004", ROOT / "tmp" / "reports" / "m274" / "M274-B004" / "part11_swift_metadata_and_isolation_mapping_summary.json", B004_CONTRACT_ID),
    ("M274-C003", ROOT / "tmp" / "reports" / "m274" / "M274-C003" / "ffi_metadata_interface_preservation_summary.json", C003_CONTRACT_ID),
    ("M274-D002", ROOT / "tmp" / "reports" / "m274" / "M274-D002" / "header_module_bridge_generation_summary.json", D002_CONTRACT_ID),
    ("M274-E001", ROOT / "tmp" / "reports" / "m274" / "M274-E001" / "interop_conformance_gate_summary.json", E001_CONTRACT_ID),
)


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M274-E002-DOC-EXP-01", "# M274 Cross-Language Execution Matrix Cross-Lane Integration Sync Expectations (E002)"),
        SnippetCheck("M274-E002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M274-E002-DOC-EXP-03", "M274-E001"),
        SnippetCheck("M274-E002-DOC-EXP-04", "M275-A001"),
        SnippetCheck("M274-E002-DOC-EXP-05", "tmp/reports/m274/M274-E002/cross_language_execution_matrix_summary.json"),
    ),
    PACKET_DOC: (
        SnippetCheck("M274-E002-DOC-PKT-01", "# M274-E002 Packet: Cross-Language Execution Matrix - Cross-Lane Integration Sync"),
        SnippetCheck("M274-E002-DOC-PKT-02", "Issue: `#7373`"),
        SnippetCheck("M274-E002-DOC-PKT-03", "Dependencies: `M274-A003`, `M274-B004`, `M274-C003`, `M274-D002`, `M274-E001`"),
        SnippetCheck("M274-E002-DOC-PKT-04", f"Next issue: `{NEXT_ISSUE}`"),
        SnippetCheck("M274-E002-DOC-PKT-05", "cross-module runtime-import and link-plan continuity"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M274-E002-SRC-01", "## M274 cross-language execution matrix closeout (M274-E002)"),
        SnippetCheck("M274-E002-SRC-02", "C/C++ foreign-surface continuity"),
        SnippetCheck("M274-E002-SRC-03", "M275-A001"),
    ),
    DOC_NATIVE: (
        SnippetCheck("M274-E002-NDOC-01", "## M274 cross-language execution matrix closeout (M274-E002)"),
        SnippetCheck("M274-E002-NDOC-02", "C/C++ foreign-surface continuity"),
        SnippetCheck("M274-E002-NDOC-03", "M275-A001"),
    ),
    ATTRIBUTE_SPEC: (
        SnippetCheck("M274-E002-ATTR-01", "## M274 cross-language execution matrix closeout (E002)"),
        SnippetCheck("M274-E002-ATTR-02", EVIDENCE_MODEL),
        SnippetCheck("M274-E002-ATTR-03", "M275-A001"),
    ),
    TABLES_SPEC: (
        SnippetCheck("M274-E002-TBL-01", "## M274 cross-language execution matrix closeout note"),
        SnippetCheck("M274-E002-TBL-02", CONTRACT_ID),
    ),
    DRIVER_CPP: (
        SnippetCheck("M274-E002-DRV-01", "M274-E001/M274-E002 Part 11 lane-E anchors"),
        SnippetCheck("M274-E002-DRV-02", "gate and the closeout matrix"),
    ),
    MANIFEST_CPP: (
        SnippetCheck("M274-E002-MAN-01", "M274-E001/M274-E002 Part 11 lane-E anchors"),
        SnippetCheck("M274-E002-MAN-02", "gate and closeout evidence chain"),
    ),
    FRONTEND_CPP: (
        SnippetCheck("M274-E002-FAPI-01", "M274-E001/M274-E002 Part 11 lane-E anchors"),
        SnippetCheck("M274-E002-FAPI-02", "same D002 generated-bridge artifacts as the native driver path"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M274-E002-RUN-01", "run_m274_a003_lane_a_readiness.py"),
        SnippetCheck("M274-E002-RUN-02", "run_m274_b004_lane_b_readiness.py"),
        SnippetCheck("M274-E002-RUN-03", "run_m274_c003_lane_c_readiness.py"),
        SnippetCheck("M274-E002-RUN-04", "run_m274_d002_lane_d_readiness.py"),
        SnippetCheck("M274-E002-RUN-05", "run_m274_e001_lane_e_readiness.py"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M274-E002-PKG-01", '"check:objc3c:m274-e002-cross-language-execution-matrix-cross-lane-integration-sync"'),
        SnippetCheck("M274-E002-PKG-02", '"test:tooling:m274-e002-cross-language-execution-matrix-cross-lane-integration-sync"'),
        SnippetCheck("M274-E002-PKG-03", '"check:objc3c:m274-e002-lane-e-readiness"'),
    ),
}


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


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


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_process(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M274-E002-MISSING", f"required artifact missing: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def refresh_upstream_evidence(skip_dynamic_probes: bool, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0

    build = run_process([
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m274-e002-closeout-refresh",
        "--summary-out",
        str(ENSURE_BUILD_SUMMARY),
    ])
    checks_total += 1
    checks_passed += require(build.returncode == 0, display_path(BUILD_HELPER), "M274-E002-UP-01", build.stderr or build.stdout or "fast build failed", failures)

    docs = run_process([sys.executable, str(DOC_BUILDER)])
    checks_total += 1
    checks_passed += require(docs.returncode == 0, display_path(DOC_BUILDER), "M274-E002-UP-02", docs.stderr or docs.stdout or "docs rebuild failed", failures)

    for index, checker in enumerate((A003_CHECKER, B004_CHECKER, C003_CHECKER, D002_CHECKER, E001_CHECKER), start=3):
        command = [sys.executable, str(checker)]
        if skip_dynamic_probes:
            command.append("--skip-dynamic-probes")
        completed = run_process(command)
        checks_total += 1
        checks_passed += require(completed.returncode == 0, display_path(checker), f"M274-E002-UP-{index:02d}", completed.stderr or completed.stdout or "upstream refresh failed", failures)
    return checks_total, checks_passed


def summary_check_counts(payload: dict[str, Any]) -> tuple[int, int]:
    total = payload.get("checks_total", payload.get("total_checks", 0))
    passed = payload.get("checks_passed", payload.get("passed_checks", -1))
    if not isinstance(total, int):
        total = 0
    if not isinstance(passed, int):
        passed = -1
    return total, passed


def validate_upstream_summaries(failures: list[Finding]) -> tuple[int, int, list[dict[str, Any]], dict[str, dict[str, Any]]]:
    checks_total = 0
    checks_passed = 0
    proof_chain: list[dict[str, Any]] = []
    raw_payloads: dict[str, dict[str, Any]] = {}

    for issue, path, contract_id in UPSTREAM_SPECS:
        artifact = display_path(path)
        checks_total += 1
        checks_passed += require(path.exists(), artifact, f"{issue}-SUM-01", "missing upstream summary", failures)
        if not path.exists():
            proof_chain.append({"issue": issue, "summary_path": artifact, "missing": True})
            continue

        payload = load_json(path)
        raw_payloads[issue] = payload
        total, passed = summary_check_counts(payload)

        checks_total += 1
        checks_passed += require(payload.get("contract_id") == contract_id, artifact, f"{issue}-SUM-02", "upstream contract drifted", failures)
        checks_total += 1
        checks_passed += require(total == passed, artifact, f"{issue}-SUM-03", "upstream summary lost full coverage", failures)
        checks_total += 1
        checks_passed += require(total > 0, artifact, f"{issue}-SUM-04", "upstream summary reports zero checks", failures)

        proof_chain.append(
            {
                "issue": issue,
                "summary_path": artifact,
                "contract_id": payload.get("contract_id"),
                "checks_total": total,
                "checks_passed": passed,
            }
        )

    return checks_total, checks_passed, proof_chain, raw_payloads


def validate_closeout_models(raw_payloads: dict[str, dict[str, Any]], skip_dynamic_probes: bool, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0

    d002 = raw_payloads.get("M274-D002", {})
    e001 = raw_payloads.get("M274-E001", {})

    checks_total += 1
    checks_passed += require(e001.get("next_closeout_issue") == "M274-E002", display_path(UPSTREAM_SPECS[-1][1]), "M274-E002-E001-01", "E001 next_closeout_issue drifted", failures)
    checks_total += 1
    checks_passed += require(e001.get("evidence_model") == "a003-b004-c003-summary-chain-plus-d002-live-header-module-bridge-proof", display_path(UPSTREAM_SPECS[-1][1]), "M274-E002-E001-02", "E001 evidence model drifted", failures)
    checks_total += 1
    checks_passed += require(d002.get("contract_id") == D002_CONTRACT_ID, display_path(UPSTREAM_SPECS[3][1]), "M274-E002-D002-01", "D002 contract drifted", failures)
    if not skip_dynamic_probes:
        runtime = d002.get("dynamic_probes", {}).get("runtime_probe", {}) if isinstance(d002.get("dynamic_probes"), dict) else {}
        checks_total += 1
        checks_passed += require(runtime.get("bridge_generation_ready") == "1", display_path(UPSTREAM_SPECS[3][1]), "M274-E002-D002-02", "D002 bridge_generation_ready drifted", failures)
        checks_total += 1
        checks_passed += require(runtime.get("cross_module_packaging_ready") == "1", display_path(UPSTREAM_SPECS[3][1]), "M274-E002-D002-03", "D002 cross_module_packaging_ready drifted", failures)

    runnable_matrix = {
        "c_cpp_foreign_surface_continuity": "M274-A003 + M274-C003",
        "swift_metadata_isolation_continuity": "M274-B004",
        "generated_bridge_artifact_continuity": "M274-D002",
        "cross_module_runtime_import_and_link_plan_continuity": "M274-D002 + M274-E001",
        "e002_executable_boundary": "M274-D002",
    }
    return checks_total, checks_passed, runnable_matrix


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_contract: dict[str, Any] = {}
    for path, snippets in STATIC_SNIPPETS.items():
        path_total = len(snippets)
        path_passed = ensure_snippets(path, snippets, failures)
        checks_total += path_total
        checks_passed += path_passed
        static_contract[display_path(path)] = {
            "checks": path_total,
            "passed": path_passed,
            "ok": path_total == path_passed,
        }

    refresh_total, refresh_passed = refresh_upstream_evidence(args.skip_dynamic_probes, failures)
    checks_total += refresh_total
    checks_passed += refresh_passed

    upstream_total, upstream_passed, proof_chain, raw_payloads = validate_upstream_summaries(failures)
    checks_total += upstream_total
    checks_passed += upstream_passed

    model_total, model_passed, runnable_matrix = validate_closeout_models(raw_payloads, args.skip_dynamic_probes, failures)
    checks_total += model_total
    checks_passed += model_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "upstream_handoff_issue": UPSTREAM_HANDOFF_ISSUE,
        "next_issue": NEXT_ISSUE,
        "evidence_model": EVIDENCE_MODEL,
        "failure_model": FAILURE_MODEL,
        "closeout_model": CLOSEOUT_MODEL,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "checks_failed": len(failures),
        "ok": not failures,
        "static_contract": static_contract,
        "proof_chain": proof_chain,
        "runnable_matrix": runnable_matrix,
        "dynamic": {
            "skipped": args.skip_dynamic_probes,
            "refreshed_upstream": True,
        },
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    print(canonical_json(summary), end="")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

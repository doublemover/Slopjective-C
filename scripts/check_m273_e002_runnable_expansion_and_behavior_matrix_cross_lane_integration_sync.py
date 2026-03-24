#!/usr/bin/env python3
"""Fail-closed checker for M273-E002 runnable metaprogramming closeout."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m273-e002-runnable-metaprogramming-matrix-cross-lane-integration-sync-v1"
CONTRACT_ID = "objc3c-part10-runnable-metaprogramming-matrix/m273-e002-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m273" / "M273-E002" / "runnable_metaprogramming_matrix_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m273_runnable_expansion_and_behavior_matrix_cross_lane_integration_sync_e002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m273" / "m273_e002_runnable_expansion_and_behavior_matrix_cross_lane_integration_sync_packet.md"
PACKAGE_JSON = ROOT / "package.json"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
ATTRIBUTE_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
TABLES_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MANIFEST_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
FRONTEND_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
READINESS_RUNNER = ROOT / "scripts" / "run_m273_e002_lane_e_readiness.py"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
DOC_BUILDER = ROOT / "scripts" / "build_objc3c_native_docs.py"
ENSURE_BUILD_SUMMARY = ROOT / "tmp" / "reports" / "m273" / "M273-E002" / "ensure_objc3c_native_build_summary.json"

A003_CONTRACT_ID = "objc3c-part10-property-behavior-source-completion/m273-a003-v1"
B004_CONTRACT_ID = "objc3c-part10-property-behavior-legality-interaction-completion/m273-b004-v1"
C003_CONTRACT_ID = "objc3c-part10-module-interface-replay-preservation/m273-c003-v1"
D002_CONTRACT_ID = "objc3c-part10-macro-host-process-cache-runtime-integration/m273-d002-v1"
E001_CONTRACT_ID = "objc3c-part10-metaprogramming-conformance-gate/m273-e001-v1"

A003_CHECKER = ROOT / "scripts" / "check_m273_a003_property_behavior_source_surface_completion_core_feature_expansion.py"
B004_CHECKER = ROOT / "scripts" / "check_m273_b004_property_behavior_legality_and_interaction_completion_edge_case_and_compatibility_completion.py"
C003_CHECKER = ROOT / "scripts" / "check_m273_c003_module_interface_and_replay_preservation_completion_core_feature_expansion.py"
D002_CHECKER = ROOT / "scripts" / "check_m273_d002_macro_host_process_and_toolchain_integration_core_feature_implementation.py"
E001_CHECKER = ROOT / "scripts" / "check_m273_e001_metaprogramming_conformance_gate_contract_and_architecture_freeze.py"

NEXT_ISSUE = "M274-A001"
UPSTREAM_HANDOFF_ISSUE = "M273-E001"
EVIDENCE_MODEL = "a003-through-e001-summary-chain-plus-d002-live-host-cache-proof"
FAILURE_MODEL = "fail-closed-on-runnable-metaprogramming-closeout-drift"
CLOSEOUT_MODEL = "lane-e-closeout-replays-supported-part10-derive-macro-property-behavior-slice-without-widening-deferred-runtime-package-loading"

UPSTREAM_SPECS: tuple[tuple[str, Path, str], ...] = (
    ("M273-A003", ROOT / "tmp" / "reports" / "m273" / "M273-A003" / "property_behavior_source_completion_summary.json", A003_CONTRACT_ID),
    ("M273-B004", ROOT / "tmp" / "reports" / "m273" / "M273-B004" / "property_behavior_legality_completion_summary.json", B004_CONTRACT_ID),
    ("M273-C003", ROOT / "tmp" / "reports" / "m273" / "M273-C003" / "module_interface_replay_preservation_summary.json", C003_CONTRACT_ID),
    ("M273-D002", ROOT / "tmp" / "reports" / "m273" / "M273-D002" / "macro_host_process_cache_runtime_integration_summary.json", D002_CONTRACT_ID),
    ("M273-E001", ROOT / "tmp" / "reports" / "m273" / "M273-E001" / "metaprogramming_conformance_gate_summary.json", E001_CONTRACT_ID),
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
        SnippetCheck("M273-E002-DOC-EXP-01", "# M273 Runnable Expansion And Behavior Matrix Cross-Lane Integration Sync Expectations (E002)"),
        SnippetCheck("M273-E002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M273-E002-DOC-EXP-03", "M273-E001"),
        SnippetCheck("M273-E002-DOC-EXP-04", "M274-A001"),
        SnippetCheck("M273-E002-DOC-EXP-05", "tmp/reports/m273/M273-E002/runnable_metaprogramming_matrix_summary.json"),
    ),
    PACKET_DOC: (
        SnippetCheck("M273-E002-DOC-PKT-01", "# M273-E002 Packet: Runnable Expansion And Behavior Matrix - Cross-Lane Integration Sync"),
        SnippetCheck("M273-E002-DOC-PKT-02", "Issue: `#7359`"),
        SnippetCheck("M273-E002-DOC-PKT-03", "Dependencies: `M273-A003`, `M273-B004`, `M273-C003`, `M273-D002`, `M273-E001`"),
        SnippetCheck("M273-E002-DOC-PKT-04", f"Next issue: `{NEXT_ISSUE}`"),
        SnippetCheck("M273-E002-DOC-PKT-05", "macro package/provenance plus host-cache continuity"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M273-E002-SRC-01", "## M273 runnable metaprogramming matrix closeout (M273-E002)"),
        SnippetCheck("M273-E002-SRC-02", "derive expansion and replay continuity"),
        SnippetCheck("M273-E002-SRC-03", "M274-A001"),
    ),
    DOC_NATIVE: (
        SnippetCheck("M273-E002-NDOC-01", "## M273 runnable metaprogramming matrix closeout (M273-E002)"),
        SnippetCheck("M273-E002-NDOC-02", "derive expansion and replay continuity"),
        SnippetCheck("M273-E002-NDOC-03", "M274-A001"),
    ),
    ATTRIBUTE_SPEC: (
        SnippetCheck("M273-E002-ATTR-01", "## M273 runnable metaprogramming matrix closeout (E002)"),
        SnippetCheck("M273-E002-ATTR-02", EVIDENCE_MODEL),
        SnippetCheck("M273-E002-ATTR-03", "M274-A001"),
    ),
    TABLES_SPEC: (
        SnippetCheck("M273-E002-TBL-01", "## M273 runnable metaprogramming matrix closeout note"),
        SnippetCheck("M273-E002-TBL-02", CONTRACT_ID),
    ),
    DRIVER_CPP: (
        SnippetCheck("M273-E002-DRV-01", "M273-E002 runnable metaprogramming closeout matrix anchor"),
        SnippetCheck("M273-E002-DRV-02", "parallel lane-E publication channel"),
    ),
    MANIFEST_CPP: (
        SnippetCheck("M273-E002-MAN-01", "M273-E002 runnable metaprogramming closeout matrix anchor"),
        SnippetCheck("M273-E002-MAN-02", "matrix-only publication path for derives/macros/property behaviors"),
    ),
    FRONTEND_CPP: (
        SnippetCheck("M273-E002-FAPI-01", "M273-E002 runnable metaprogramming closeout matrix anchor"),
        SnippetCheck("M273-E002-FAPI-02", "second frontend-only Part 10 matrix reporting path"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M273-E002-RUN-01", "run_m273_a003_lane_a_readiness.py"),
        SnippetCheck("M273-E002-RUN-02", "run_m273_b004_lane_b_readiness.py"),
        SnippetCheck("M273-E002-RUN-03", "run_m273_c003_lane_c_readiness.py"),
        SnippetCheck("M273-E002-RUN-04", "run_m273_d002_lane_d_readiness.py"),
        SnippetCheck("M273-E002-RUN-05", "run_m273_e001_lane_e_readiness.py"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M273-E002-PKG-01", '"check:objc3c:m273-e002-runnable-expansion-and-behavior-matrix-cross-lane-integration-sync"'),
        SnippetCheck("M273-E002-PKG-02", '"test:tooling:m273-e002-runnable-expansion-and-behavior-matrix-cross-lane-integration-sync"'),
        SnippetCheck("M273-E002-PKG-03", '"check:objc3c:m273-e002-lane-e-readiness"'),
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
        failures.append(Finding(display_path(path), "M273-E002-MISSING", f"required artifact missing: {display_path(path)}"))
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
        "m273-e002-closeout-refresh",
        "--summary-out",
        str(ENSURE_BUILD_SUMMARY),
    ])
    checks_total += 1
    checks_passed += require(build.returncode == 0, display_path(BUILD_HELPER), "M273-E002-UP-01", f"fast build failed: {build.stdout}{build.stderr}", failures)

    docs = run_process([sys.executable, str(DOC_BUILDER)])
    checks_total += 1
    checks_passed += require(docs.returncode == 0, display_path(DOC_BUILDER), "M273-E002-UP-02", f"docs rebuild failed: {docs.stdout}{docs.stderr}", failures)

    def checker_command(path: Path) -> list[str]:
        command = [sys.executable, str(path)]
        if skip_dynamic_probes:
            command.append("--skip-dynamic-probes")
        return command

    commands = [A003_CHECKER, B004_CHECKER, C003_CHECKER, D002_CHECKER, E001_CHECKER]
    for index, artifact in enumerate(commands, start=3):
        completed = run_process(checker_command(artifact))
        checks_total += 1
        checks_passed += require(completed.returncode == 0, display_path(artifact), f"M273-E002-UP-{index:02d}", f"upstream refresh failed: {completed.stdout}{completed.stderr}", failures)
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
                "dynamic_probes_executed": payload.get("dynamic_probes_executed", payload.get("dynamic", {}).get("skipped") is False if isinstance(payload.get("dynamic"), dict) else None),
            }
        )

    return checks_total, checks_passed, proof_chain, raw_payloads


def validate_closeout_models(raw_payloads: dict[str, dict[str, Any]], skip_dynamic_probes: bool, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0

    d002 = raw_payloads.get("M273-D002", {})
    e001 = raw_payloads.get("M273-E001", {})

    checks_total += 1
    checks_passed += require(e001.get("next_closeout_issue") == "M273-E002", display_path(UPSTREAM_SPECS[-1][1]), "M273-E002-E001-01", "E001 next_closeout_issue drifted", failures)
    checks_total += 1
    checks_passed += require(e001.get("evidence_model") == "a003-b004-c003-static-summary-chain-plus-d002-live-host-cache-proof", display_path(UPSTREAM_SPECS[-1][1]), "M273-E002-E001-02", "E001 evidence model drifted", failures)
    checks_total += 1
    checks_passed += require(d002.get("contract_id") == D002_CONTRACT_ID, display_path(UPSTREAM_SPECS[3][1]), "M273-E002-D002-01", "D002 contract drifted", failures)
    checks_total += 1
    checks_passed += require(d002.get("implementation_ready") is True or skip_dynamic_probes, display_path(UPSTREAM_SPECS[3][1]), "M273-E002-D002-02", "D002 implementation proof missing", failures)
    if not skip_dynamic_probes:
        checks_total += 1
        checks_passed += require(d002.get("dynamic_probes_executed") is True, display_path(UPSTREAM_SPECS[3][1]), "M273-E002-D002-03", "D002 live proof missing", failures)

    runnable_matrix = {
        "derive_expansion_replay_continuity": "M273-C003",
        "macro_package_provenance_host_cache_continuity": "M273-D002",
        "property_behavior_legality_replay_continuity": "M273-B004 + M273-C003",
        "cross_module_preservation_and_host_cache_reuse": "M273-C003 + M273-D002",
        "d002_executable_boundary": "M273-D002",
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

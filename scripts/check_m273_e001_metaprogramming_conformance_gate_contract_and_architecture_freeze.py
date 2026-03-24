#!/usr/bin/env python3
"""Fail-closed checker for M273-E001 metaprogramming conformance gate."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m273-e001-metaprogramming-conformance-gate-v1"
CONTRACT_ID = "objc3c-part10-metaprogramming-conformance-gate/m273-e001-v1"
EVIDENCE_MODEL = "a003-b004-c003-static-summary-chain-plus-d002-live-host-cache-proof"
NEXT_CLOSEOUT_ISSUE = "M273-E002"

A003_CONTRACT_ID = "objc3c-part10-property-behavior-source-completion/m273-a003-v1"
B004_CONTRACT_ID = "objc3c-part10-property-behavior-legality-interaction-completion/m273-b004-v1"
C003_CONTRACT_ID = "objc3c-part10-module-interface-replay-preservation/m273-c003-v1"
D002_CONTRACT_ID = "objc3c-part10-macro-host-process-cache-runtime-integration/m273-d002-v1"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m273_metaprogramming_conformance_gate_contract_and_architecture_freeze_e001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m273" / "m273_e001_metaprogramming_conformance_gate_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_TABLES = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MANIFEST_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
READINESS_RUNNER = ROOT / "scripts" / "run_m273_e001_lane_e_readiness.py"
PACKAGE_JSON = ROOT / "package.json"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
DOC_BUILDER = ROOT / "scripts" / "build_objc3c_native_docs.py"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m273" / "M273-E001" / "metaprogramming_conformance_gate_summary.json"
ENSURE_BUILD_SUMMARY = ROOT / "tmp" / "reports" / "m273" / "M273-E001" / "ensure_objc3c_native_build_summary.json"

A003_CHECKER = ROOT / "scripts" / "check_m273_a003_property_behavior_source_surface_completion_core_feature_expansion.py"
B004_CHECKER = ROOT / "scripts" / "check_m273_b004_property_behavior_legality_and_interaction_completion_edge_case_and_compatibility_completion.py"
C003_CHECKER = ROOT / "scripts" / "check_m273_c003_module_interface_and_replay_preservation_completion_core_feature_expansion.py"
D002_CHECKER = ROOT / "scripts" / "check_m273_d002_macro_host_process_and_toolchain_integration_core_feature_implementation.py"

UPSTREAM_SUMMARIES: tuple[tuple[str, Path, str], ...] = (
    ("M273-A003", ROOT / "tmp" / "reports" / "m273" / "M273-A003" / "property_behavior_source_completion_summary.json", A003_CONTRACT_ID),
    ("M273-B004", ROOT / "tmp" / "reports" / "m273" / "M273-B004" / "property_behavior_legality_completion_summary.json", B004_CONTRACT_ID),
    ("M273-C003", ROOT / "tmp" / "reports" / "m273" / "M273-C003" / "module_interface_replay_preservation_summary.json", C003_CONTRACT_ID),
    ("M273-D002", ROOT / "tmp" / "reports" / "m273" / "M273-D002" / "macro_host_process_cache_runtime_integration_summary.json", D002_CONTRACT_ID),
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
        SnippetCheck("M273-E001-EXP-01", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M273-E001-EXP-02", "`M273-D002`"),
        SnippetCheck("M273-E001-EXP-03", "`M273-E002`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M273-E001-PKT-01", "Packet: `M273-E001`"),
        SnippetCheck("M273-E001-PKT-02", "Issue: `#7358`"),
        SnippetCheck("M273-E001-PKT-03", "`M273-D002`"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M273-E001-DOCSRC-01", "## M273 metaprogramming conformance gate"),
        SnippetCheck("M273-E001-DOCSRC-02", "`M273-D002` live macro host-process/cache summary"),
        SnippetCheck("M273-E001-DOCSRC-03", "`M273-E002`"),
    ),
    DOC_NATIVE: (
        SnippetCheck("M273-E001-DOC-01", "## M273 metaprogramming conformance gate"),
        SnippetCheck("M273-E001-DOC-02", "`M273-D002` live macro host-process/cache summary"),
        SnippetCheck("M273-E001-DOC-03", "`M273-E002`"),
    ),
    SPEC_ATTR: (
        SnippetCheck("M273-E001-ATTR-01", "## M273 metaprogramming conformance gate (E001)"),
        SnippetCheck("M273-E001-ATTR-02", EVIDENCE_MODEL),
    ),
    SPEC_TABLES: (
        SnippetCheck("M273-E001-TBL-01", "## M273 metaprogramming conformance gate note"),
        SnippetCheck("M273-E001-TBL-02", CONTRACT_ID),
    ),
    DRIVER_CPP: (
        SnippetCheck("M273-E001-DRV-01", "M273-E001 metaprogramming conformance gate anchor"),
        SnippetCheck("M273-E001-DRV-02", "D002 live macro host-process/cache proof remains"),
    ),
    MANIFEST_CPP: (
        SnippetCheck("M273-E001-MAN-01", "M273-E001 metaprogramming conformance gate anchor"),
        SnippetCheck("M273-E001-MAN-02", "canonical executable evidence"),
    ),
    FRONTEND_ANCHOR_CPP: (
        SnippetCheck("M273-E001-FAPI-01", "M273-E001 metaprogramming conformance gate anchor"),
        SnippetCheck("M273-E001-FAPI-02", "truthful runnable Part 10 capability anchor"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M273-E001-RUN-01", "run_m273_a003_lane_a_readiness.py"),
        SnippetCheck("M273-E001-RUN-02", "run_m273_b004_lane_b_readiness.py"),
        SnippetCheck("M273-E001-RUN-03", "run_m273_c003_lane_c_readiness.py"),
        SnippetCheck("M273-E001-RUN-04", "run_m273_d002_lane_d_readiness.py"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M273-E001-PKG-01", '"check:objc3c:m273-e001-metaprogramming-conformance-gate-contract-and-architecture-freeze"'),
        SnippetCheck("M273-E001-PKG-02", '"test:tooling:m273-e001-metaprogramming-conformance-gate-contract-and-architecture-freeze"'),
        SnippetCheck("M273-E001-PKG-03", '"check:objc3c:m273-e001-lane-e-readiness"'),
    ),
}


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def read_json(path: Path) -> dict[str, Any]:
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
        failures.append(Finding(display_path(path), "M273-E001-MISSING", f"required artifact missing: {display_path(path)}"))
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
        "m273-e001-gate-refresh",
        "--summary-out",
        str(ENSURE_BUILD_SUMMARY),
    ])
    checks_total += 1
    checks_passed += require(build.returncode == 0, display_path(BUILD_HELPER), "M273-E001-UP-01", f"fast build failed: {build.stdout}{build.stderr}", failures)

    docs = run_process([sys.executable, str(DOC_BUILDER)])
    checks_total += 1
    checks_passed += require(docs.returncode == 0, display_path(DOC_BUILDER), "M273-E001-UP-02", f"docs rebuild failed: {docs.stdout}{docs.stderr}", failures)

    def checker_command(path: Path) -> list[str]:
        command = [sys.executable, str(path)]
        if skip_dynamic_probes:
            command.append("--skip-dynamic-probes")
        return command

    upstream_commands = [A003_CHECKER, B004_CHECKER, C003_CHECKER, D002_CHECKER]
    for index, artifact in enumerate(upstream_commands, start=3):
        completed = run_process(checker_command(artifact))
        checks_total += 1
        checks_passed += require(completed.returncode == 0, display_path(artifact), f"M273-E001-UP-{index:02d}", f"upstream refresh failed: {completed.stdout}{completed.stderr}", failures)
    return checks_total, checks_passed


def summary_check_counts(payload: dict[str, Any]) -> tuple[int, int]:
    total = payload.get("checks_total", payload.get("total_checks", 0))
    passed = payload.get("checks_passed", payload.get("passed_checks", -1))
    if not isinstance(total, int):
        total = 0
    if not isinstance(passed, int):
        passed = -1
    return total, passed


def validate_upstream_summaries(skip_dynamic_probes: bool, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    upstream: dict[str, Any] = {}
    for issue, path, contract_id in UPSTREAM_SUMMARIES:
        artifact = display_path(path)
        checks_total += 1
        checks_passed += require(path.exists(), artifact, f"{issue}-SUM-01", "missing upstream summary", failures)
        if not path.exists():
            upstream[issue] = {"missing": True}
            continue
        payload = read_json(path)
        total, passed = summary_check_counts(payload)
        checks_total += 1
        checks_passed += require(payload.get("contract_id") == contract_id, artifact, f"{issue}-SUM-02", "upstream contract drifted", failures)
        checks_total += 1
        checks_passed += require(total == passed, artifact, f"{issue}-SUM-03", "upstream summary lost full coverage", failures)
        checks_total += 1
        checks_passed += require(total > 0, artifact, f"{issue}-SUM-04", "upstream summary reports zero checks", failures)
        if not skip_dynamic_probes and issue == "M273-D002":
            checks_total += 1
            checks_passed += require(payload.get("dynamic_probes_executed") is True, artifact, f"{issue}-SUM-05", "expected live D002 runtime evidence", failures)
        upstream[issue] = {
            "contract_id": payload.get("contract_id"),
            "checks_total": total,
            "checks_passed": passed,
            "dynamic_probes_executed": payload.get("dynamic_probes_executed"),
        }
    return checks_total, checks_passed, upstream


def validate_d002_runtime_proof(skip_dynamic_probes: bool, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    issue, path, _ = UPSTREAM_SUMMARIES[-1]
    payload = read_json(path)
    artifact = display_path(path)
    dynamic = payload.get("dynamic", {}) if isinstance(payload, dict) else {}
    probe_payload = dynamic.get("payload", {}) if isinstance(dynamic, dict) else {}
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("implementation_ready") is True or skip_dynamic_probes, artifact, "M273-E001-D002-01", "expected D002 implementation_ready proof", failures)
    if not skip_dynamic_probes:
        checks_total += 1
        checks_passed += require(bool(probe_payload.get("provider_first")), artifact, "M273-E001-D002-02", "missing provider_first proof path", failures)
        checks_total += 1
        checks_passed += require(bool(probe_payload.get("provider_second")), artifact, "M273-E001-D002-03", "missing provider_second proof path", failures)
        checks_total += 1
        checks_passed += require(bool(probe_payload.get("consumer")), artifact, "M273-E001-D002-04", "missing consumer proof path", failures)
        checks_total += 1
        checks_passed += require(bool(probe_payload.get("cache_backup")), artifact, "M273-E001-D002-05", "missing cold-run cache backup proof", failures)
    return checks_total, checks_passed, {issue: {"contract_id": payload.get("contract_id"), "dynamic_probes_executed": payload.get("dynamic_probes_executed"), "implementation_ready": payload.get("implementation_ready")}}


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_contracts: dict[str, Any] = {}
    for path, snippets in STATIC_SNIPPETS.items():
        path_total = len(snippets)
        path_passed = ensure_snippets(path, snippets, failures)
        checks_total += path_total
        checks_passed += path_passed
        static_contracts[display_path(path)] = {
            "checks": path_total,
            "passed": path_passed,
            "ok": path_total == path_passed,
        }

    refresh_total, refresh_passed = refresh_upstream_evidence(args.skip_dynamic_probes, failures)
    checks_total += refresh_total
    checks_passed += refresh_passed

    upstream_total, upstream_passed, upstream = validate_upstream_summaries(args.skip_dynamic_probes, failures)
    checks_total += upstream_total
    checks_passed += upstream_passed

    d002_total, d002_passed, d002_boundary = validate_d002_runtime_proof(args.skip_dynamic_probes, failures)
    checks_total += d002_total
    checks_passed += d002_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "evidence_model": EVIDENCE_MODEL,
        "next_closeout_issue": NEXT_CLOSEOUT_ISSUE,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "checks_failed": len(failures),
        "ok": not failures,
        "static_contracts": static_contracts,
        "upstream": upstream,
        "d002_runtime_boundary": d002_boundary,
        "dynamic": {
            "skipped": args.skip_dynamic_probes,
            "refreshed_upstream": True,
        },
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

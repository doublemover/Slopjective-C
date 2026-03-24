#!/usr/bin/env python3
"""Fail-closed checker for M274-E001 interop conformance gate."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m274-e001-part11-interop-conformance-gate-v1"
CONTRACT_ID = "objc3c-part11-interop-conformance-gate/m274-e001-v1"
EVIDENCE_MODEL = "a003-b004-c003-summary-chain-plus-d002-live-header-module-bridge-proof"
NEXT_CLOSEOUT_ISSUE = "M274-E002"

A003_CONTRACT_ID = "objc3c-part11-foreign-surface-interface-preservation/m274-a003-v1"
B004_CONTRACT_ID = "objc3c-part11-swift-metadata-and-isolation-mapping-completion/m274-b004-v1"
C003_CONTRACT_ID = "objc3c-part11-ffi-metadata-interface-preservation/m274-c003-v1"
D002_CONTRACT_ID = "objc3c-part11-header-module-and-bridge-generation/m274-d002-v1"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m274_interop_conformance_gate_contract_and_architecture_freeze_e001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m274" / "m274_e001_interop_conformance_gate_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_TABLES = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCH_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MANIFEST_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
READINESS_RUNNER = ROOT / "scripts" / "run_m274_e001_lane_e_readiness.py"
PACKAGE_JSON = ROOT / "package.json"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
DOC_BUILDER = ROOT / "scripts" / "build_objc3c_native_docs.py"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m274" / "M274-E001" / "interop_conformance_gate_summary.json"
ENSURE_BUILD_SUMMARY = ROOT / "tmp" / "reports" / "m274" / "M274-E001" / "ensure_objc3c_native_build_summary.json"

A003_CHECKER = ROOT / "scripts" / "check_m274_a003_foreign_surfaces_interface_and_module_preservation_completion_core_feature_expansion.py"
B004_CHECKER = ROOT / "scripts" / "check_m274_b004_part11_swift_metadata_and_isolation_mapping_completion_edge_case_and_compatibility_completion.py"
C003_CHECKER = ROOT / "scripts" / "check_m274_c003_metadata_and_interface_preservation_across_ffi_boundaries_core_feature_expansion.py"
D002_CHECKER = ROOT / "scripts" / "check_m274_d002_header_module_and_bridge_generation_implementation_core_feature_implementation.py"

UPSTREAM_SUMMARIES: tuple[tuple[str, Path, str], ...] = (
    ("M274-A003", ROOT / "tmp" / "reports" / "m274" / "M274-A003" / "foreign_surfaces_interface_module_preservation_summary.json", A003_CONTRACT_ID),
    ("M274-B004", ROOT / "tmp" / "reports" / "m274" / "M274-B004" / "part11_swift_metadata_and_isolation_mapping_summary.json", B004_CONTRACT_ID),
    ("M274-C003", ROOT / "tmp" / "reports" / "m274" / "M274-C003" / "ffi_metadata_interface_preservation_summary.json", C003_CONTRACT_ID),
    ("M274-D002", ROOT / "tmp" / "reports" / "m274" / "M274-D002" / "header_module_bridge_generation_summary.json", D002_CONTRACT_ID),
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
        SnippetCheck("M274-E001-EXP-01", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M274-E001-EXP-02", "`M274-D002`"),
        SnippetCheck("M274-E001-EXP-03", "`M274-E002`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M274-E001-PKT-01", "# M274-E001 Packet: Interop Conformance Gate - Contract And Architecture Freeze"),
        SnippetCheck("M274-E001-PKT-02", "Issue: `#7372`"),
        SnippetCheck("M274-E001-PKT-03", "Dependencies: `M274-A003`, `M274-B004`, `M274-C003`, `M274-D002`"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M274-E001-DOCSRC-01", "## M274 interop conformance gate"),
        SnippetCheck("M274-E001-DOCSRC-02", "`M274-D002` remains the executable evidence boundary"),
        SnippetCheck("M274-E001-DOCSRC-03", "`M274-E002`"),
    ),
    DOC_NATIVE: (
        SnippetCheck("M274-E001-DOC-01", "## M274 interop conformance gate"),
        SnippetCheck("M274-E001-DOC-02", "`M274-D002` remains the executable evidence boundary"),
        SnippetCheck("M274-E001-DOC-03", "`M274-E002`"),
    ),
    SPEC_ATTR: (
        SnippetCheck("M274-E001-ATTR-01", "## M274 interop conformance gate (E001)"),
        SnippetCheck("M274-E001-ATTR-02", "`M274-D002` remains the live executable boundary"),
    ),
    SPEC_TABLES: (
        SnippetCheck("M274-E001-TBL-01", "## M274 interop conformance gate note"),
        SnippetCheck("M274-E001-TBL-02", CONTRACT_ID),
    ),
    ARCH_DOC: (
        SnippetCheck("M274-E001-ARCH-01", "## M274 interop conformance gate (E001)"),
        SnippetCheck("M274-E001-ARCH-02", "`M274-E001` adds explicit driver/manifest/frontend publication anchors"),
    ),
    RUNTIME_README: (
        SnippetCheck("M274-E001-RTR-01", "## M274 interop conformance gate"),
        SnippetCheck("M274-E001-RTR-02", "`M274-E001` does not add a new runtime probe."),
    ),
    DRIVER_CPP: (
        SnippetCheck("M274-E001-DRV-01", "M274-E001 interop conformance gate anchor"),
        SnippetCheck("M274-E001-DRV-02", "live D002 bridge sidecars consumed by the lane-E gate"),
    ),
    MANIFEST_CPP: (
        SnippetCheck("M274-E001-MAN-01", "M274-E001 interop conformance gate anchor"),
        SnippetCheck("M274-E001-MAN-02", "canonical lane-E evidence chain"),
    ),
    FRONTEND_ANCHOR_CPP: (
        SnippetCheck("M274-E001-FAPI-01", "M274-E001 interop conformance gate anchor"),
        SnippetCheck("M274-E001-FAPI-02", "same D002 generated-bridge artifacts as the native driver path"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M274-E001-RUN-01", "run_m274_a003_lane_a_readiness.py"),
        SnippetCheck("M274-E001-RUN-02", "run_m274_b004_lane_b_readiness.py"),
        SnippetCheck("M274-E001-RUN-03", "run_m274_c003_lane_c_readiness.py"),
        SnippetCheck("M274-E001-RUN-04", "run_m274_d002_lane_d_readiness.py"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M274-E001-PKG-01", '"check:objc3c:m274-e001-interop-conformance-gate-contract-and-architecture-freeze"'),
        SnippetCheck("M274-E001-PKG-02", '"test:tooling:m274-e001-interop-conformance-gate-contract-and-architecture-freeze"'),
        SnippetCheck("M274-E001-PKG-03", '"check:objc3c:m274-e001-lane-e-readiness"'),
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
        failures.append(Finding(display_path(path), "M274-E001-MISSING", f"required artifact missing: {display_path(path)}"))
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
        "m274-e001-gate-refresh",
        "--summary-out",
        str(ENSURE_BUILD_SUMMARY),
    ])
    checks_total += 1
    checks_passed += require(build.returncode == 0, display_path(BUILD_HELPER), "M274-E001-UP-01", build.stderr or build.stdout or "fast build failed", failures)

    docs = run_process([sys.executable, str(DOC_BUILDER)])
    checks_total += 1
    checks_passed += require(docs.returncode == 0, display_path(DOC_BUILDER), "M274-E001-UP-02", docs.stderr or docs.stdout or "docs rebuild failed", failures)

    for index, checker in enumerate((A003_CHECKER, B004_CHECKER, C003_CHECKER, D002_CHECKER), start=3):
        command = [sys.executable, str(checker)]
        if skip_dynamic_probes:
            command.append("--skip-dynamic-probes")
        completed = run_process(command)
        checks_total += 1
        checks_passed += require(completed.returncode == 0, display_path(checker), f"M274-E001-UP-{index:02d}", completed.stderr or completed.stdout or "upstream refresh failed", failures)
    return checks_total, checks_passed


def summary_counts(payload: dict[str, Any]) -> tuple[int, int]:
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
        total, passed = summary_counts(payload)
        checks_total += 1
        checks_passed += require(payload.get("contract_id") == contract_id, artifact, f"{issue}-SUM-02", "upstream contract drifted", failures)
        checks_total += 1
        checks_passed += require(total == passed, artifact, f"{issue}-SUM-03", "upstream summary lost full coverage", failures)
        checks_total += 1
        checks_passed += require(total > 0, artifact, f"{issue}-SUM-04", "upstream summary reports zero checks", failures)
        if issue == "M274-D002" and not skip_dynamic_probes:
            checks_total += 1
            checks_passed += require(isinstance(payload.get("dynamic_probes"), dict), artifact, f"{issue}-SUM-05", "expected D002 dynamic probes payload", failures)
        upstream[issue] = {
            "contract_id": payload.get("contract_id"),
            "checks_total": total,
            "checks_passed": passed,
        }
    return checks_total, checks_passed, upstream


def validate_d002_live_proof(skip_dynamic_probes: bool, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    payload = read_json(UPSTREAM_SUMMARIES[-1][1])
    artifact = display_path(UPSTREAM_SUMMARIES[-1][1])
    dynamic = payload.get("dynamic_probes", {}) if isinstance(payload, dict) else {}
    provider = dynamic.get("provider_import_surface", {}) if isinstance(dynamic, dict) else {}
    consumer = dynamic.get("consumer_link_plan", {}) if isinstance(dynamic, dict) else {}
    runtime = dynamic.get("runtime_probe", {}) if isinstance(dynamic, dict) else {}

    checks_total += 1
    checks_passed += require(payload.get("contract_id") == D002_CONTRACT_ID, artifact, "M274-E001-D002-01", "D002 contract drifted", failures)
    if skip_dynamic_probes:
        return checks_total, checks_passed, {"skipped": True, "contract_id": payload.get("contract_id")}

    required_provider_pairs = {
        "contract_id": D002_CONTRACT_ID,
        "header_artifact_relative_path": "module.part11-bridge.h",
        "module_artifact_relative_path": "module.part11-bridge.modulemap",
        "bridge_artifact_relative_path": "module.part11-bridge.json",
    }
    for index, (key, expected) in enumerate(required_provider_pairs.items(), start=2):
        checks_total += 1
        checks_passed += require(provider.get(key) == expected, artifact, f"M274-E001-D002-{index:02d}", f"unexpected provider {key}: {provider.get(key)!r}", failures)

    checks_total += 1
    checks_passed += require(provider.get("runtime_generation_ready") is True, artifact, "M274-E001-D002-06", "provider runtime_generation_ready not true", failures)
    checks_total += 1
    checks_passed += require(provider.get("cross_module_packaging_ready") is True, artifact, "M274-E001-D002-07", "provider cross_module_packaging_ready not true", failures)
    checks_total += 1
    checks_passed += require(consumer.get("part11_header_module_bridge_cross_module_packaging_ready") is True, artifact, "M274-E001-D002-08", "consumer link-plan packaging flag not true", failures)
    checks_total += 1
    checks_passed += require(consumer.get("part11_header_module_bridge_imported_module_count") == 1, artifact, "M274-E001-D002-09", "consumer imported module count mismatch", failures)

    required_runtime_pairs = {
        "runtime_generation_ready": "1",
        "cross_module_packaging_ready": "1",
        "header_generation_ready": "1",
        "module_generation_ready": "1",
        "bridge_generation_ready": "1",
        "deterministic": "1",
        "header_artifact_relative_path": "module.part11-bridge.h",
        "module_artifact_relative_path": "module.part11-bridge.modulemap",
        "bridge_artifact_relative_path": "module.part11-bridge.json",
    }
    for index, (key, expected) in enumerate(required_runtime_pairs.items(), start=10):
        checks_total += 1
        checks_passed += require(runtime.get(key) == expected, artifact, f"M274-E001-D002-{index:02d}", f"unexpected runtime {key}: {runtime.get(key)!r}", failures)

    return checks_total, checks_passed, {
        "provider_import_surface": provider,
        "consumer_link_plan": {
            "part11_header_module_bridge_cross_module_packaging_ready": consumer.get("part11_header_module_bridge_cross_module_packaging_ready"),
            "part11_header_module_bridge_imported_module_count": consumer.get("part11_header_module_bridge_imported_module_count"),
        },
        "runtime_probe": runtime,
    }


def build_summary(skip_dynamic_probes: bool) -> tuple[dict[str, object], list[Finding]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_contracts: dict[str, Any] = {}
    for path, snippets in STATIC_SNIPPETS.items():
        total = len(snippets)
        passed = ensure_snippets(path, snippets, failures)
        checks_total += total
        checks_passed += passed
        static_contracts[display_path(path)] = {"checks": total, "passed": passed, "ok": total == passed}

    refresh_total, refresh_passed = refresh_upstream_evidence(skip_dynamic_probes, failures)
    checks_total += refresh_total
    checks_passed += refresh_passed

    upstream_total, upstream_passed, upstream = validate_upstream_summaries(skip_dynamic_probes, failures)
    checks_total += upstream_total
    checks_passed += upstream_passed

    d002_total, d002_passed, d002_runtime_boundary = validate_d002_live_proof(skip_dynamic_probes, failures)
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
        "d002_runtime_boundary": d002_runtime_boundary,
        "dynamic": {"skipped": skip_dynamic_probes, "refreshed_upstream": True},
        "failures": [finding.__dict__ for finding in failures],
    }
    return summary, failures


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    summary, failures = build_summary(args.skip_dynamic_probes)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

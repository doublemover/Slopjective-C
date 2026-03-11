#!/usr/bin/env python3
"""Fail-closed checker for M276-C003 packet dependency-shape decomposition."""

from __future__ import annotations

import json
import os
import subprocess
import sys
import time
from dataclasses import dataclass
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
MODE = "m276-c003-frontend-packet-dependency-shape-decomposition-v1"
CONTRACT_ID = "objc3c-frontend-packet-dependency-shape-decomposition/m276-c003-v1"
NEXT_ISSUE = "M276-C002"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m276" / "M276-C003" / "frontend_packet_dependency_shape_decomposition_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m276_frontend_packet_dependency_shape_decomposition_for_incremental_build_split_c003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m276" / "m276_c003_frontend_packet_dependency_shape_decomposition_for_incremental_build_split_packet.md"
README = ROOT / "README.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m276_c003_lane_c_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m276_c003_frontend_packet_dependency_shape_decomposition_for_incremental_build_split.py"

ARTIFACT_NATIVE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
ARTIFACT_CAPI = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
ARTIFACT_RUNTIME = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
PACKET_DIR = ROOT / "tmp" / "artifacts" / "objc3c-native"
PACKET_MAP = {
    "frontend_scaffold": PACKET_DIR / "frontend_modular_scaffold.json",
    "frontend_invocation_lock": PACKET_DIR / "frontend_invocation_lock.json",
    "frontend_core_feature_expansion": PACKET_DIR / "frontend_core_feature_expansion.json",
    "frontend_edge_compat": PACKET_DIR / "frontend_edge_compat.json",
    "frontend_edge_robustness": PACKET_DIR / "frontend_edge_robustness.json",
    "frontend_diagnostics_hardening": PACKET_DIR / "frontend_diagnostics_hardening.json",
    "frontend_recovery_determinism_hardening": PACKET_DIR / "frontend_recovery_determinism_hardening.json",
    "frontend_conformance_matrix": PACKET_DIR / "frontend_conformance_matrix.json",
    "frontend_conformance_corpus": PACKET_DIR / "frontend_conformance_corpus.json",
    "frontend_integration_closeout": PACKET_DIR / "frontend_integration_closeout.json",
}
SOURCE_DERIVED = ["frontend_scaffold"]
BINARY_DERIVED = ["frontend_invocation_lock", "frontend_core_feature_expansion"]
CLOSEOUT_DERIVED = [
    "frontend_edge_compat",
    "frontend_edge_robustness",
    "frontend_diagnostics_hardening",
    "frontend_recovery_determinism_hardening",
    "frontend_conformance_matrix",
    "frontend_conformance_corpus",
    "frontend_integration_closeout",
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


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M276-C003-EXP-01", "Contract ID: `objc3c-frontend-packet-dependency-shape-decomposition/m276-c003-v1`"),
        SnippetCheck("M276-C003-EXP-02", "source-derived"),
        SnippetCheck("M276-C003-EXP-03", "binary-derived"),
        SnippetCheck("M276-C003-EXP-04", "closeout-derived"),
    ),
    PACKET_DOC: (
        SnippetCheck("M276-C003-PKT-01", "Issue: `#7393`"),
        SnippetCheck("M276-C003-PKT-02", "Unblocks `M276-C002`"),
        SnippetCheck("M276-C003-PKT-03", "source-derived: scaffold"),
    ),
    BUILD_SCRIPT: (
        SnippetCheck("M276-C003-BUILD-01", '[ValidateSet("full", "binaries-only", "packets-source", "packets-binary", "packets-closeout", "packets-all")]'),
        SnippetCheck("M276-C003-BUILD-02", "M276-C003 packet-dependency-shape anchor"),
        SnippetCheck("M276-C003-BUILD-03", 'Family = "source-derived"'),
        SnippetCheck("M276-C003-BUILD-04", 'Family = "binary-derived"'),
        SnippetCheck("M276-C003-BUILD-05", 'Family = "closeout-derived"'),
        SnippetCheck("M276-C003-BUILD-06", 'function Invoke-FrontendPacketGeneration'),
    ),
    README: (
        SnippetCheck("M276-C003-README-01", "source-derived"),
        SnippetCheck("M276-C003-README-02", "binary-derived"),
        SnippetCheck("M276-C003-README-03", "closeout-derived"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M276-C003-DOC-01", "## Frontend packet dependency-shape decomposition (M276-C003)"),
        SnippetCheck("M276-C003-DOC-02", "source-derived"),
        SnippetCheck("M276-C003-DOC-03", "binary-derived"),
        SnippetCheck("M276-C003-DOC-04", "closeout-derived"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M276-C003-NDOC-01", "## Frontend packet dependency-shape decomposition (M276-C003)"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M276-C003-ARCH-01", "## M276 Frontend Packet Dependency-Shape Decomposition (C003)"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M276-C003-PKG-01", '"check:objc3c:m276-c003-frontend-packet-dependency-shape-decomposition": "python scripts/check_m276_c003_frontend_packet_dependency_shape_decomposition_for_incremental_build_split.py"'),
        SnippetCheck("M276-C003-PKG-02", '"test:tooling:m276-c003-frontend-packet-dependency-shape-decomposition": "python -m pytest tests/tooling/test_check_m276_c003_frontend_packet_dependency_shape_decomposition_for_incremental_build_split.py -q"'),
        SnippetCheck("M276-C003-PKG-03", '"check:objc3c:m276-c003-lane-c-readiness": "python scripts/run_m276_c003_lane_c_readiness.py"'),
    ),
}


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    findings: list[Finding] = []
    checks_total = len(snippets) + 1
    if not path.exists():
        findings.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return checks_total, findings
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def run(cmd: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, check=False)


def packet_mtime_map() -> dict[str, float]:
    return {name: path.stat().st_mtime for name, path in PACKET_MAP.items()}


def ensure_outputs_exist() -> tuple[int, list[Finding], Path]:
    summary_dir = SUMMARY_OUT.parent
    summary_dir.mkdir(parents=True, exist_ok=True)
    log_path = summary_dir / "binaries_only.log"
    completed = run(["pwsh", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(BUILD_SCRIPT), "-ExecutionMode", "binaries-only"])
    log_path.write_text(completed.stdout + completed.stderr, encoding="utf-8")
    findings: list[Finding] = []
    checks_total = 1
    if completed.returncode != 0:
        findings.append(Finding("dynamic/binaries-only", "M276-C003-DYN-BINARIES", f"binaries-only failed: {completed.returncode}"))
        return checks_total, findings, log_path
    return checks_total, findings, log_path


def backdate_packets(epoch: float) -> None:
    for path in PACKET_MAP.values():
        if path.exists():
            os.utime(path, (epoch, epoch))


def classify_changes(before: dict[str, float], after: dict[str, float], threshold: float = 0.5) -> set[str]:
    changed: set[str] = set()
    for name, previous in before.items():
        current = after[name]
        if current - previous > threshold:
            changed.add(name)
    return changed


def run_packet_mode(mode: str, expected_changed: set[str], summary_dir: Path, binary_mtimes: dict[str, float]) -> tuple[int, list[Finding], dict[str, Any]]:
    checks_total = 0
    findings: list[Finding] = []
    baseline_time = time.time() - 120.0
    backdate_packets(baseline_time)
    before_packets = packet_mtime_map()
    time.sleep(1.2)
    log_path = summary_dir / f"{mode}.log"
    completed = run(["pwsh", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(BUILD_SCRIPT), "-ExecutionMode", mode])
    log_path.write_text(completed.stdout + completed.stderr, encoding="utf-8")
    after_packets = packet_mtime_map()
    changed = classify_changes(before_packets, after_packets)
    checks_total += 1
    if completed.returncode != 0:
        findings.append(Finding(f"dynamic/{mode}", f"M276-C003-DYN-{mode.upper()}-RC", f"{mode} failed: {completed.returncode}"))
        return checks_total, findings, {"log": display_path(log_path), "changed": sorted(changed)}

    checks_total += 2
    combined_output = completed.stdout + completed.stderr
    if "cmake_build_skip=native-binaries" not in combined_output:
        findings.append(Finding(f"dynamic/{mode}", f"M276-C003-DYN-{mode.upper()}-SKIP", f"{mode} did not skip native binary compilation"))
    if f"artifact_generation_mode={mode}" not in combined_output:
        findings.append(Finding(f"dynamic/{mode}", f"M276-C003-DYN-{mode.upper()}-MODE", f"{mode} did not report artifact generation mode"))

    checks_total += len(binary_mtimes)
    for label, path in {
        "native": ARTIFACT_NATIVE,
        "capi": ARTIFACT_CAPI,
        "runtime": ARTIFACT_RUNTIME,
    }.items():
        current = path.stat().st_mtime
        if abs(current - binary_mtimes[label]) > 0.01:
            findings.append(Finding(f"dynamic/{mode}", f"M276-C003-DYN-{mode.upper()}-{label.upper()}-MUTATED", f"{mode} unexpectedly changed {display_path(path)}"))

    checks_total += 2
    if changed != expected_changed:
        findings.append(Finding(f"dynamic/{mode}", f"M276-C003-DYN-{mode.upper()}-SHAPE", f"{mode} changed {sorted(changed)} instead of {sorted(expected_changed)}"))
    for packet_name in expected_changed:
        if packet_name not in changed:
            findings.append(Finding(f"dynamic/{mode}", f"M276-C003-DYN-{mode.upper()}-{packet_name.upper()}-MISS", f"{mode} did not regenerate expected packet {packet_name}"))

    return checks_total, findings, {
        "log": display_path(log_path),
        "changed": sorted(changed),
    }


def dynamic_proof() -> tuple[int, list[Finding], dict[str, Any]]:
    summary_dir = SUMMARY_OUT.parent
    summary_dir.mkdir(parents=True, exist_ok=True)
    checks_total, findings, binaries_log = ensure_outputs_exist()
    if findings:
        return checks_total, findings, {"binaries_only_log": display_path(binaries_log)}

    checks_total += 3
    binary_mtimes = {
        "native": ARTIFACT_NATIVE.stat().st_mtime,
        "capi": ARTIFACT_CAPI.stat().st_mtime,
        "runtime": ARTIFACT_RUNTIME.stat().st_mtime,
    }
    for path in (ARTIFACT_NATIVE, ARTIFACT_CAPI, ARTIFACT_RUNTIME):
        if not path.exists():
            findings.append(Finding("dynamic/binaries-only", "M276-C003-DYN-BINARIES-MISSING", f"missing required binary output: {display_path(path)}"))

    checks_total += len(PACKET_MAP)
    for packet_name, packet_path in PACKET_MAP.items():
        if not packet_path.exists():
            findings.append(Finding("dynamic/packets", f"M276-C003-DYN-{packet_name.upper()}-MISSING", f"missing required packet output: {display_path(packet_path)}"))
    if findings:
        return checks_total, findings, {"binaries_only_log": display_path(binaries_log)}

    source_total, source_findings, source_summary = run_packet_mode(
        "packets-source", set(SOURCE_DERIVED), summary_dir, binary_mtimes
    )
    binary_total, binary_findings, binary_summary = run_packet_mode(
        "packets-binary", set(SOURCE_DERIVED + BINARY_DERIVED), summary_dir, binary_mtimes
    )
    closeout_total, closeout_findings, closeout_summary = run_packet_mode(
        "packets-closeout", set(SOURCE_DERIVED + BINARY_DERIVED + CLOSEOUT_DERIVED), summary_dir, binary_mtimes
    )

    checks_total += source_total + binary_total + closeout_total
    findings.extend(source_findings)
    findings.extend(binary_findings)
    findings.extend(closeout_findings)

    return checks_total, findings, {
        "binaries_only_log": display_path(binaries_log),
        "family_map": {
            "source-derived": SOURCE_DERIVED,
            "binary-derived": BINARY_DERIVED,
            "closeout-derived": CLOSEOUT_DERIVED,
        },
        "packets_source": source_summary,
        "packets_binary": binary_summary,
        "packets_closeout": closeout_summary,
    }


def main() -> int:
    failures: list[Finding] = []
    checks_total = 0
    for path, snippets in STATIC_SNIPPETS.items():
        total, local_failures = check_static_contract(path, snippets)
        checks_total += total
        failures.extend(local_failures)

    dynamic_summary: dict[str, Any] = {}
    if not failures:
        dynamic_total, dynamic_failures, dynamic_summary = dynamic_proof()
        checks_total += dynamic_total
        failures.extend(dynamic_failures)

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "next_issue": NEXT_ISSUE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "dynamic_summary": dynamic_summary,
        "findings": [finding.__dict__ for finding in failures],
    }
    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_OUT.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        print(f"[info] wrote summary to {display_path(SUMMARY_OUT)}", file=sys.stderr)
        return 1
    print(f"[ok] M276-C003 packet dependency-shape decomposition validated ({checks_total} checks)")
    print(f"[info] wrote summary to {display_path(SUMMARY_OUT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

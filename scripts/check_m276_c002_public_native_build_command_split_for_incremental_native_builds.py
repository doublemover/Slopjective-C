#!/usr/bin/env python3
"""Fail-closed checker for M276-C002 public native build command split."""

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
MODE = "m276-c002-public-native-build-command-split-v1"
CONTRACT_ID = "objc3c-public-native-build-command-split/m276-c002-v1"
NEXT_ISSUE = "M276-D001"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m276" / "M276-C002" / "public_native_build_command_split_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m276_public_native_build_command_split_for_incremental_native_builds_c002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m276" / "m276_c002_public_native_build_command_split_for_incremental_native_builds_packet.md"
README = ROOT / "README.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m276_c002_lane_c_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m276_c002_public_native_build_command_split_for_incremental_native_builds.py"
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
SOURCE_BINARY_PACKETS = {
    "frontend_scaffold",
    "frontend_invocation_lock",
    "frontend_core_feature_expansion",
}
FULL_PACKETS = set(PACKET_MAP)
M226_EXPECTED = {
    ROOT / "docs" / "contracts" / "m226_frontend_build_invocation_modular_scaffold_expectations.md": "npm run build:objc3c-native:contracts",
    ROOT / "docs" / "contracts" / "m226_frontend_build_invocation_manifest_guard_d003_expectations.md": "npm run build:objc3c-native:contracts",
    ROOT / "docs" / "contracts" / "m226_frontend_build_invocation_core_feature_expansion_d004_expectations.md": "npm run build:objc3c-native:contracts",
    ROOT / "docs" / "contracts" / "m226_frontend_build_invocation_edge_case_compatibility_d005_expectations.md": "npm run build:objc3c-native:full",
    ROOT / "docs" / "contracts" / "m226_frontend_build_invocation_edge_robustness_d006_expectations.md": "npm run build:objc3c-native:full",
    ROOT / "docs" / "contracts" / "m226_frontend_build_invocation_diagnostics_hardening_d007_expectations.md": "npm run build:objc3c-native:full",
    ROOT / "docs" / "contracts" / "m226_frontend_build_invocation_recovery_determinism_hardening_d008_expectations.md": "npm run build:objc3c-native:full",
    ROOT / "docs" / "contracts" / "m226_frontend_build_invocation_conformance_matrix_d009_expectations.md": "npm run build:objc3c-native:full",
    ROOT / "docs" / "contracts" / "m226_frontend_build_invocation_conformance_corpus_d010_expectations.md": "npm run build:objc3c-native:full",
    ROOT / "docs" / "contracts" / "m226_frontend_build_invocation_integration_closeout_d011_expectations.md": "npm run build:objc3c-native:full",
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


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M276-C002-EXP-01", "Contract ID: `objc3c-public-native-build-command-split/m276-c002-v1`"),
        SnippetCheck("M276-C002-EXP-02", "npm run build:objc3c-native:contracts"),
        SnippetCheck("M276-C002-EXP-03", "npm run build:objc3c-native:full"),
    ),
    PACKET_DOC: (
        SnippetCheck("M276-C002-PKT-01", "Issue: `#7388`"),
        SnippetCheck("M276-C002-PKT-02", "M276-D001"),
        SnippetCheck("M276-C002-PKT-03", "scripts/build_objc3c_native.ps1 -ExecutionMode packets-binary"),
    ),
    BUILD_SCRIPT: (
        SnippetCheck("M276-C002-BUILD-01", "build:objc3c-native              => fast binary-build default"),
        SnippetCheck("M276-C002-BUILD-02", "build:objc3c-native:contracts   => source-derived + binary-derived packet path"),
        SnippetCheck("M276-C002-BUILD-03", "build:objc3c-native:full        => binary + full packet-family path"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M276-C002-PKG-01", '"build:objc3c-native": "pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/build_objc3c_native.ps1 -ExecutionMode binaries-only"'),
        SnippetCheck("M276-C002-PKG-02", '"build:objc3c-native:contracts": "pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/build_objc3c_native.ps1 -ExecutionMode packets-binary"'),
        SnippetCheck("M276-C002-PKG-03", '"build:objc3c-native:full": "pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/build_objc3c_native.ps1 -ExecutionMode full"'),
        SnippetCheck("M276-C002-PKG-04", '"check:objc3c:m276-c002-public-native-build-command-split": "python scripts/check_m276_c002_public_native_build_command_split_for_incremental_native_builds.py"'),
    ),
    README: (
        SnippetCheck("M276-C002-README-01", "npm run build:objc3c-native:contracts"),
        SnippetCheck("M276-C002-README-02", "npm run build:objc3c-native:full"),
        SnippetCheck("M276-C002-README-03", "direct script callers still default to the full wrapper path"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M276-C002-DOC-01", "## Public native build command split (M276-C002)"),
        SnippetCheck("M276-C002-DOC-02", "scripts/build_objc3c_native.ps1 -ExecutionMode packets-binary"),
        SnippetCheck("M276-C002-DOC-03", "M276-D001"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M276-C002-NDOC-01", "## Public native build command split (M276-C002)"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M276-C002-ARCH-01", "## M276 Public Native Build Command Split (C002)"),
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


def backdate_packets(epoch: float) -> None:
    for path in PACKET_MAP.values():
        if path.exists():
            os.utime(path, (epoch, epoch))


def changed_packets(before: dict[str, float], after: dict[str, float], threshold: float = 0.5) -> set[str]:
    return {name for name, previous in before.items() if after[name] - previous > threshold}


def check_m226_surface() -> tuple[int, list[Finding]]:
    checks_total = 0
    findings: list[Finding] = []
    for path, expected_command in M226_EXPECTED.items():
        text = path.read_text(encoding="utf-8")
        checks_total += 1
        if expected_command not in text:
            findings.append(Finding(display_path(path), "M276-C002-M226-CMD", f"expected command missing: {expected_command}"))
    return checks_total, findings


def ensure_outputs() -> tuple[int, list[Finding], Path]:
    summary_dir = SUMMARY_OUT.parent
    summary_dir.mkdir(parents=True, exist_ok=True)
    log_path = summary_dir / "build_fast.log"
    completed = run(["npm.cmd", "run", "build:objc3c-native"])
    log_path.write_text(completed.stdout + completed.stderr, encoding="utf-8")
    checks_total = 1
    findings: list[Finding] = []
    if completed.returncode != 0:
        findings.append(Finding("dynamic/build-fast", "M276-C002-DYN-FAST-RC", f"fast build failed: {completed.returncode}"))
    return checks_total, findings, log_path


def dynamic_proof() -> tuple[int, list[Finding], dict[str, Any]]:
    checks_total = 0
    findings: list[Finding] = []
    summary_dir = SUMMARY_OUT.parent
    summary_dir.mkdir(parents=True, exist_ok=True)

    total, local_findings, fast_log = ensure_outputs()
    checks_total += total
    findings.extend(local_findings)
    if findings:
        return checks_total, findings, {"fast_log": display_path(fast_log)}

    for path in (ARTIFACT_NATIVE, ARTIFACT_CAPI, ARTIFACT_RUNTIME):
        checks_total += 1
        if not path.exists():
            findings.append(Finding("dynamic/build-fast", "M276-C002-DYN-BIN-MISSING", f"missing binary output: {display_path(path)}"))
    for path in PACKET_MAP.values():
        checks_total += 1
        if not path.exists():
            findings.append(Finding("dynamic/packets", "M276-C002-DYN-PACKET-MISSING", f"missing packet output: {display_path(path)}"))
    if findings:
        return checks_total, findings, {"fast_log": display_path(fast_log)}

    baseline_time = time.time() - 120.0
    backdate_packets(baseline_time)
    before_fast_packets = packet_mtime_map()
    time.sleep(1.2)
    fast_again = run(["npm.cmd", "run", "build:objc3c-native"])
    fast_again_log = summary_dir / "build_fast_again.log"
    fast_again_log.write_text(fast_again.stdout + fast_again.stderr, encoding="utf-8")
    after_fast_packets = packet_mtime_map()
    fast_changed = changed_packets(before_fast_packets, after_fast_packets)
    checks_total += 3
    if fast_again.returncode != 0:
        findings.append(Finding("dynamic/build-fast", "M276-C002-DYN-FAST-SECOND-RC", f"second fast build failed: {fast_again.returncode}"))
    if "artifact_generation_mode=none" not in fast_again.stdout + fast_again.stderr:
        findings.append(Finding("dynamic/build-fast", "M276-C002-DYN-FAST-NONE", "fast build did not report packet generation mode none"))
    if fast_changed:
        findings.append(Finding("dynamic/build-fast", "M276-C002-DYN-FAST-PACKETS", f"fast build changed packet outputs: {sorted(fast_changed)}"))

    binary_mtimes = {
        "native": ARTIFACT_NATIVE.stat().st_mtime,
        "capi": ARTIFACT_CAPI.stat().st_mtime,
        "runtime": ARTIFACT_RUNTIME.stat().st_mtime,
    }

    baseline_time = time.time() - 240.0
    backdate_packets(baseline_time)
    before_contracts = packet_mtime_map()
    time.sleep(1.2)
    contracts = run(["npm.cmd", "run", "build:objc3c-native:contracts"])
    contracts_log = summary_dir / "build_contracts.log"
    contracts_log.write_text(contracts.stdout + contracts.stderr, encoding="utf-8")
    after_contracts = packet_mtime_map()
    contracts_changed = changed_packets(before_contracts, after_contracts)
    checks_total += 6
    if contracts.returncode != 0:
        findings.append(Finding("dynamic/build-contracts", "M276-C002-DYN-CONTRACTS-RC", f"contracts build failed: {contracts.returncode}"))
    combined = contracts.stdout + contracts.stderr
    if "artifact_generation_mode=packets-binary" not in combined:
        findings.append(Finding("dynamic/build-contracts", "M276-C002-DYN-CONTRACTS-MODE", "contracts build did not report packets-binary mode"))
    if "cmake_build_skip=native-binaries" not in combined:
        findings.append(Finding("dynamic/build-contracts", "M276-C002-DYN-CONTRACTS-SKIP", "contracts build did not skip native binary compilation"))
    if contracts_changed != SOURCE_BINARY_PACKETS:
        findings.append(Finding("dynamic/build-contracts", "M276-C002-DYN-CONTRACTS-SHAPE", f"contracts build changed {sorted(contracts_changed)} instead of {sorted(SOURCE_BINARY_PACKETS)}"))
    for label, path in {"native": ARTIFACT_NATIVE, "capi": ARTIFACT_CAPI, "runtime": ARTIFACT_RUNTIME}.items():
        if abs(path.stat().st_mtime - binary_mtimes[label]) > 0.01:
            findings.append(Finding("dynamic/build-contracts", "M276-C002-DYN-CONTRACTS-BIN-MUTATED", f"contracts build mutated {display_path(path)}"))
    if not SOURCE_BINARY_PACKETS.issubset(contracts_changed):
        findings.append(Finding("dynamic/build-contracts", "M276-C002-DYN-CONTRACTS-MISS", "contracts build missed expected source/binary packet outputs"))

    baseline_time = time.time() - 360.0
    backdate_packets(baseline_time)
    before_full = packet_mtime_map()
    time.sleep(1.2)
    full = run(["npm.cmd", "run", "build:objc3c-native:full"])
    full_log = summary_dir / "build_full.log"
    full_log.write_text(full.stdout + full.stderr, encoding="utf-8")
    after_full = packet_mtime_map()
    full_changed = changed_packets(before_full, after_full)
    checks_total += 4
    if full.returncode != 0:
        findings.append(Finding("dynamic/build-full", "M276-C002-DYN-FULL-RC", f"full build failed: {full.returncode}"))
    combined = full.stdout + full.stderr
    if "cmake_build_start=native-binaries" not in combined:
        findings.append(Finding("dynamic/build-full", "M276-C002-DYN-FULL-BUILD", "full build did not report native binary build start"))
    if "artifact_generation_mode=full" not in combined:
        findings.append(Finding("dynamic/build-full", "M276-C002-DYN-FULL-MODE", "full build did not report full packet generation mode"))
    if full_changed != FULL_PACKETS:
        findings.append(Finding("dynamic/build-full", "M276-C002-DYN-FULL-SHAPE", f"full build changed {sorted(full_changed)} instead of the full packet set"))

    return checks_total, findings, {
        "fast_log": display_path(fast_log),
        "fast_again_log": display_path(fast_again_log),
        "contracts_log": display_path(contracts_log),
        "full_log": display_path(full_log),
        "fast_changed_packets": sorted(fast_changed),
        "contracts_changed_packets": sorted(contracts_changed),
        "full_changed_packets": sorted(full_changed),
    }


def main() -> int:
    failures: list[Finding] = []
    checks_total = 0
    for path, snippets in STATIC_SNIPPETS.items():
        total, local_failures = check_static_contract(path, snippets)
        checks_total += total
        failures.extend(local_failures)
    m226_total, m226_failures = check_m226_surface()
    checks_total += m226_total
    failures.extend(m226_failures)

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
    print(f"[ok] M276-C002 public native build command split validated ({checks_total} checks)")
    print(f"[info] wrote summary to {display_path(SUMMARY_OUT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

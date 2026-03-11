#!/usr/bin/env python3
"""Fail-closed checker for M276-E002 incremental native build closeout."""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
import time
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MODE = "m276-e002-incremental-build-closeout-v1"
CONTRACT_ID = "objc3c-incremental-native-build-closeout/m276-e002-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m276" / "M276-E002" / "incremental_build_closeout_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m276_ci_adoption_closeout_evidence_and_fallback_hardening_for_incremental_native_builds_e002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m276" / "m276_e002_ci_adoption_closeout_evidence_and_fallback_hardening_for_incremental_native_builds_packet.md"
README = ROOT / "README.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE = ROOT / ".github" / "workflows" / "task-hygiene.yml"
COMPILER_CLOSEOUT = ROOT / ".github" / "workflows" / "compiler-closeout.yml"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m276_e002_ci_adoption_closeout_evidence_and_fallback_hardening_for_incremental_native_builds.py"
READINESS_RUNNER = ROOT / "scripts" / "run_m276_e002_lane_e_readiness.py"
BUILD_DIR = ROOT / "tmp" / "build-objc3c-native"
FINGERPRINT = BUILD_DIR / "native_build_backend_fingerprint.json"
COMPILE_COMMANDS = BUILD_DIR / "compile_commands.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
CAPI_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
RUNTIME_LIB = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
SOURCE_BINARY_PACKETS = [
    ROOT / "tmp" / "artifacts" / "objc3c-native" / "frontend_modular_scaffold.json",
    ROOT / "tmp" / "artifacts" / "objc3c-native" / "frontend_invocation_lock.json",
    ROOT / "tmp" / "artifacts" / "objc3c-native" / "frontend_core_feature_expansion.json",
]
CLOSEOUT_PACKETS = [
    ROOT / "tmp" / "artifacts" / "objc3c-native" / "frontend_edge_compat.json",
    ROOT / "tmp" / "artifacts" / "objc3c-native" / "frontend_edge_robustness.json",
    ROOT / "tmp" / "artifacts" / "objc3c-native" / "frontend_diagnostics_hardening.json",
    ROOT / "tmp" / "artifacts" / "objc3c-native" / "frontend_recovery_determinism_hardening.json",
    ROOT / "tmp" / "artifacts" / "objc3c-native" / "frontend_conformance_matrix.json",
    ROOT / "tmp" / "artifacts" / "objc3c-native" / "frontend_conformance_corpus.json",
    ROOT / "tmp" / "artifacts" / "objc3c-native" / "frontend_integration_closeout.json",
]
ALL_PACKETS = SOURCE_BINARY_PACKETS + CLOSEOUT_PACKETS


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
        SnippetCheck("M276-E002-EXP-01", "Contract ID: `objc3c-incremental-native-build-closeout/m276-e002-v1`"),
        SnippetCheck("M276-E002-EXP-02", "cold build proof from a missing persistent build tree"),
        SnippetCheck("M276-E002-EXP-03", "The issue-local evidence must live under `tmp/reports/m276/M276-E002/`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M276-E002-PKT-01", "Issue: `#7391`"),
        SnippetCheck("M276-E002-PKT-02", "`.github/workflows/task-hygiene.yml` is the active fast-path workflow and must also prove the contracts-only packet path."),
        SnippetCheck("M276-E002-PKT-03", "`.github/workflows/compiler-closeout.yml` is the manual closeout workflow and must prove the full build path."),
    ),
    README: (
        SnippetCheck("M276-E002-README-01", "CI and closeout semantics:"),
        SnippetCheck("M276-E002-README-02", "the active Windows core workflow proves `build:objc3c-native` plus"),
        SnippetCheck("M276-E002-README-03", "the manual compiler closeout workflow proves `build:objc3c-native:full`"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M276-E002-DOC-01", "## CI and closeout semantics (M276-E002)"),
        SnippetCheck("M276-E002-DOC-02", "`.github/workflows/task-hygiene.yml`"),
        SnippetCheck("M276-E002-DOC-03", "`.github/workflows/compiler-closeout.yml`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M276-E002-NDOC-01", "## CI and closeout semantics (M276-E002)"),
        SnippetCheck("M276-E002-NDOC-02", "cold-build, warm-build, invalid-fingerprint fallback,"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M276-E002-ARCH-01", "## M276 CI And Closeout Semantics (E002)"),
        SnippetCheck("M276-E002-ARCH-02", "hosted runners start from clean workspaces"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M276-E002-PKG-01", '"check:objc3c:m276-e002-ci-closeout": "python scripts/check_m276_e002_ci_adoption_closeout_evidence_and_fallback_hardening_for_incremental_native_builds.py"'),
        SnippetCheck("M276-E002-PKG-02", '"test:tooling:m276-e002-ci-closeout": "python -m pytest tests/tooling/test_check_m276_e002_ci_adoption_closeout_evidence_and_fallback_hardening_for_incremental_native_builds.py -q"'),
        SnippetCheck("M276-E002-PKG-03", '"check:objc3c:m276-e002-lane-e-readiness": "python scripts/run_m276_e002_lane_e_readiness.py"'),
    ),
    TASK_HYGIENE: (
        SnippetCheck("M276-E002-TH-01", "run: npm run build:objc3c-native"),
        SnippetCheck("M276-E002-TH-02", "run: npm run build:objc3c-native:contracts"),
    ),
    COMPILER_CLOSEOUT: (
        SnippetCheck("M276-E002-CC-01", "runs-on: windows-latest"),
        SnippetCheck("M276-E002-CC-02", "run: npm run build:objc3c-native:full"),
    ),
    TEST_FILE: (
        SnippetCheck("M276-E002-TST-01", 'CONTRACT_ID = "objc3c-incremental-native-build-closeout/m276-e002-v1"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M276-E002-RDY-01", "check_m276_e002_ci_adoption_closeout_evidence_and_fallback_hardening_for_incremental_native_builds.py"),
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


def run_with_log(command: list[str], log_name: str) -> tuple[subprocess.CompletedProcess[str], str]:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    completed = subprocess.run(command, cwd=ROOT, capture_output=True, text=True, check=False)
    log_path = REPORT_DIR / log_name
    log_path.write_text(completed.stdout + completed.stderr, encoding="utf-8")
    return completed, display_path(log_path)


def get_mtimes(paths: list[Path]) -> dict[str, int | None]:
    payload: dict[str, int | None] = {}
    for path in paths:
        payload[display_path(path)] = path.stat().st_mtime_ns if path.exists() else None
    return payload


def expect_log_contains(findings: list[Finding], check_id: str, log_text: str, snippet: str, artifact: str) -> None:
    if snippet not in log_text:
        findings.append(Finding(artifact, check_id, f"expected log snippet missing: {snippet}"))


def expect_exists(findings: list[Finding], check_id: str, path: Path) -> None:
    if not path.exists():
        findings.append(Finding(display_path(path), check_id, "required artifact missing"))


def expect_unchanged(findings: list[Finding], check_id: str, before: dict[str, int | None], after: dict[str, int | None]) -> None:
    for key, previous in before.items():
        if previous != after.get(key):
            findings.append(Finding(key, check_id, "mtime changed unexpectedly"))


def expect_advanced(findings: list[Finding], check_id: str, before: dict[str, int | None], after: dict[str, int | None], *, allow_missing_before: bool = False) -> None:
    for key, previous in before.items():
        current = after.get(key)
        if previous is None:
            if allow_missing_before:
                if current is None:
                    findings.append(Finding(key, check_id, "artifact still missing after required refresh"))
            else:
                findings.append(Finding(key, check_id, "artifact missing before comparison"))
            continue
        if current is None or current <= previous:
            findings.append(Finding(key, check_id, "mtime did not advance"))


def mutate_fingerprint_for_refresh() -> tuple[dict[str, object], str]:
    payload = json.loads(FINGERPRINT.read_text(encoding="utf-8"))
    original_generator = str(payload.get("generator", ""))
    payload["generator"] = original_generator + "-mutated"
    FINGERPRINT.write_text(canonical_json(payload), encoding="utf-8")
    return payload, original_generator


REPORT_DIR = SUMMARY_OUT.parent
BACKUP_DIR = REPORT_DIR / "backups"


def dynamic_proof() -> tuple[int, list[Finding], dict[str, object]]:
    findings: list[Finding] = []
    checks_total = 24
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    BACKUP_DIR.mkdir(parents=True, exist_ok=True)
    evidence: dict[str, object] = {}

    backup_path = None
    if BUILD_DIR.exists():
        backup_path = BACKUP_DIR / f"build-objc3c-native-pre-closeout-{int(time.time())}"
        shutil.move(str(BUILD_DIR), str(backup_path))
    evidence["build_tree_backup"] = display_path(backup_path) if backup_path else None

    cold_completed, cold_log = run_with_log(["npm.cmd", "run", "build:objc3c-native"], "cold_fast_build.log")
    evidence["cold_log"] = cold_log
    if cold_completed.returncode != 0:
        findings.append(Finding("dynamic/cold-fast", "M276-E002-DYN-COLD-RC", f"cold fast build failed with exit code {cold_completed.returncode}"))
        return checks_total, findings, evidence
    cold_text = cold_completed.stdout + cold_completed.stderr
    expect_log_contains(findings, "M276-E002-DYN-COLD-CONFIG", cold_text, "cmake_configure=cold", cold_log)
    expect_log_contains(findings, "M276-E002-DYN-COLD-MODE", cold_text, "execution_mode=binaries-only", cold_log)
    for path, check_id in ((BUILD_DIR, "M276-E002-DYN-COLD-BUILD-DIR"), (COMPILE_COMMANDS, "M276-E002-DYN-COLD-COMPILE"), (FINGERPRINT, "M276-E002-DYN-COLD-FINGERPRINT"), (NATIVE_EXE, "M276-E002-DYN-COLD-NATIVE"), (CAPI_EXE, "M276-E002-DYN-COLD-CAPI"), (RUNTIME_LIB, "M276-E002-DYN-COLD-RUNTIME")):
        expect_exists(findings, check_id, path)

    binary_paths = [NATIVE_EXE, CAPI_EXE, RUNTIME_LIB]
    warm_before = get_mtimes(binary_paths)
    time.sleep(1.2)
    warm_completed, warm_log = run_with_log(["npm.cmd", "run", "build:objc3c-native"], "warm_fast_build.log")
    evidence["warm_log"] = warm_log
    if warm_completed.returncode != 0:
        findings.append(Finding("dynamic/warm-fast", "M276-E002-DYN-WARM-RC", f"warm fast build failed with exit code {warm_completed.returncode}"))
        return checks_total, findings, evidence
    warm_text = warm_completed.stdout + warm_completed.stderr
    warm_after = get_mtimes(binary_paths)
    expect_log_contains(findings, "M276-E002-DYN-WARM-CONFIG", warm_text, "cmake_configure=reuse", warm_log)
    expect_unchanged(findings, "M276-E002-DYN-WARM-MTIME", warm_before, warm_after)

    mutated_payload, original_generator = mutate_fingerprint_for_refresh()
    evidence["mutated_generator"] = mutated_payload["generator"]
    time.sleep(1.2)
    refresh_completed, refresh_log = run_with_log(["npm.cmd", "run", "build:objc3c-native"], "refresh_fingerprint_fast_build.log")
    evidence["refresh_log"] = refresh_log
    if refresh_completed.returncode != 0:
        findings.append(Finding("dynamic/refresh-fingerprint", "M276-E002-DYN-REFRESH-RC", f"refresh build failed with exit code {refresh_completed.returncode}"))
        return checks_total, findings, evidence
    refresh_text = refresh_completed.stdout + refresh_completed.stderr
    expect_log_contains(findings, "M276-E002-DYN-REFRESH-CONFIG", refresh_text, "cmake_configure=refresh-fingerprint", refresh_log)
    refreshed_payload = json.loads(FINGERPRINT.read_text(encoding="utf-8"))
    if refreshed_payload.get("generator") != original_generator:
        findings.append(Finding(display_path(FINGERPRINT), "M276-E002-DYN-REFRESH-GENERATOR", "fingerprint generator was not restored by refresh-fingerprint rebuild"))

    contracts_binary_before = get_mtimes(binary_paths)
    contracts_source_before = get_mtimes(SOURCE_BINARY_PACKETS)
    contracts_closeout_before = get_mtimes(CLOSEOUT_PACKETS)
    time.sleep(1.2)
    contracts_completed, contracts_log = run_with_log(["npm.cmd", "run", "build:objc3c-native:contracts"], "contracts_only_build.log")
    evidence["contracts_log"] = contracts_log
    if contracts_completed.returncode != 0:
        findings.append(Finding("dynamic/contracts", "M276-E002-DYN-CONTRACTS-RC", f"contracts-only build failed with exit code {contracts_completed.returncode}"))
        return checks_total, findings, evidence
    contracts_text = contracts_completed.stdout + contracts_completed.stderr
    expect_log_contains(findings, "M276-E002-DYN-CONTRACTS-MODE", contracts_text, "artifact_generation_mode=packets-binary", contracts_log)
    expect_log_contains(findings, "M276-E002-DYN-CONTRACTS-SKIP", contracts_text, "cmake_build_skip=native-binaries", contracts_log)
    contracts_binary_after = get_mtimes(binary_paths)
    contracts_source_after = get_mtimes(SOURCE_BINARY_PACKETS)
    contracts_closeout_after = get_mtimes(CLOSEOUT_PACKETS)
    expect_unchanged(findings, "M276-E002-DYN-CONTRACTS-BINARIES", contracts_binary_before, contracts_binary_after)
    expect_advanced(findings, "M276-E002-DYN-CONTRACTS-SOURCE-BINARY", contracts_source_before, contracts_source_after, allow_missing_before=True)
    expect_unchanged(findings, "M276-E002-DYN-CONTRACTS-CLOSEOUT", contracts_closeout_before, contracts_closeout_after)

    full_before = get_mtimes(ALL_PACKETS)
    time.sleep(1.2)
    full_completed, full_log = run_with_log(["npm.cmd", "run", "build:objc3c-native:full"], "full_build.log")
    evidence["full_log"] = full_log
    if full_completed.returncode != 0:
        findings.append(Finding("dynamic/full", "M276-E002-DYN-FULL-RC", f"full build failed with exit code {full_completed.returncode}"))
        return checks_total, findings, evidence
    full_text = full_completed.stdout + full_completed.stderr
    expect_log_contains(findings, "M276-E002-DYN-FULL-MODE", full_text, "artifact_generation_mode=full", full_log)
    full_after = get_mtimes(ALL_PACKETS)
    expect_advanced(findings, "M276-E002-DYN-FULL-CLOSEOUT", {key: full_before[key] for key in get_mtimes(CLOSEOUT_PACKETS).keys()}, {key: full_after[key] for key in get_mtimes(CLOSEOUT_PACKETS).keys()}, allow_missing_before=True)

    evidence["cold_binary_mtimes"] = get_mtimes(binary_paths)
    evidence["warm_binary_mtimes_before"] = warm_before
    evidence["warm_binary_mtimes_after"] = warm_after
    evidence["contracts_binary_mtimes_before"] = contracts_binary_before
    evidence["contracts_binary_mtimes_after"] = contracts_binary_after
    evidence["contracts_source_binary_mtimes_before"] = contracts_source_before
    evidence["contracts_source_binary_mtimes_after"] = contracts_source_after
    evidence["contracts_closeout_mtimes_before"] = contracts_closeout_before
    evidence["contracts_closeout_mtimes_after"] = contracts_closeout_after
    evidence["full_packet_mtimes_before"] = full_before
    evidence["full_packet_mtimes_after"] = full_after
    return checks_total, findings, evidence


def main() -> int:
    failures: list[Finding] = []
    checks_total = 0
    for path, snippets in STATIC_SNIPPETS.items():
        total, local_failures = check_static_contract(path, snippets)
        checks_total += total
        failures.extend(local_failures)

    dynamic_summary: dict[str, object] = {}
    if not failures:
        dynamic_total, dynamic_failures, dynamic_summary = dynamic_proof()
        checks_total += dynamic_total
        failures.extend(dynamic_failures)

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
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
    print(f"[ok] M276-E002 closeout validated ({checks_total} checks)")
    print(f"[info] wrote summary to {display_path(SUMMARY_OUT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

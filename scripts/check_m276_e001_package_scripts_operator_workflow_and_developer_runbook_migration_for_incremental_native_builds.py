#!/usr/bin/env python3
"""Fail-closed checker for M276-E001 operator surface migration."""

from __future__ import annotations

import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MODE = "m276-e001-operator-surface-migration-v1"
CONTRACT_ID = "objc3c-incremental-native-build-operator-surface/m276-e001-v1"
NEXT_ISSUE = "M276-E002"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m276" / "M276-E001" / "operator_surface_migration_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m276_package_scripts_operator_workflow_and_developer_runbook_migration_for_incremental_native_builds_e001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m276" / "m276_e001_package_scripts_operator_workflow_and_developer_runbook_migration_for_incremental_native_builds_packet.md"
RUNBOOK = ROOT / "docs" / "runbooks" / "m276_incremental_native_build_operator_runbook.md"
README = ROOT / "README.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
PACKAGE_JSON = ROOT / "package.json"
FINGERPRINT = ROOT / "tmp" / "build-objc3c-native" / "native_build_backend_fingerprint.json"
COMPILE_COMMANDS = ROOT / "tmp" / "build-objc3c-native" / "compile_commands.json"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m276_e001_package_scripts_operator_workflow_and_developer_runbook_migration_for_incremental_native_builds.py"
READINESS_RUNNER = ROOT / "scripts" / "run_m276_e001_lane_e_readiness.py"


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
        SnippetCheck("M276-E001-EXP-01", "Contract ID: `objc3c-incremental-native-build-operator-surface/m276-e001-v1`"),
        SnippetCheck("M276-E001-EXP-02", "`build:objc3c-native:reconfigure`"),
        SnippetCheck("M276-E001-EXP-03", "The issue-local evidence must live under `tmp/reports/m276/M276-E001/`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M276-E001-PKT-01", "Issue: `#7390`"),
        SnippetCheck("M276-E001-PKT-02", "`npm run build:objc3c-native:reconfigure`"),
        SnippetCheck("M276-E001-PKT-03", "Hands off CI adoption and closeout proof to `M276-E002`."),
    ),
    RUNBOOK: (
        SnippetCheck("M276-E001-RBK-01", "# M276 Incremental Native Build Operator Runbook"),
        SnippetCheck("M276-E001-RBK-02", "tmp/build-objc3c-native/native_build_backend_fingerprint.json"),
        SnippetCheck("M276-E001-RBK-03", "npm run build:objc3c-native:reconfigure"),
    ),
    README: (
        SnippetCheck("M276-E001-README-01", "`npm run build:objc3c-native:reconfigure`"),
        SnippetCheck("M276-E001-README-02", "The command split is fully implemented now."),
        SnippetCheck("M276-E001-README-03", "When to use each command"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M276-E001-DOC-01", "## Operator workflow and public reconfigure path (M276-E001)"),
        SnippetCheck("M276-E001-DOC-02", "tmp/build-objc3c-native/native_build_backend_fingerprint.json"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M276-E001-NDOC-01", "## Operator workflow and public reconfigure path (M276-E001)"),
        SnippetCheck("M276-E001-NDOC-02", "`build:objc3c-native:reconfigure`"),
    ),
    RUNTIME_README: (
        SnippetCheck("M276-E001-RT-01", "## Incremental native build commands"),
        SnippetCheck("M276-E001-RT-02", "build:objc3c-native:reconfigure"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M276-E001-ARCH-01", "## M276 Operator Workflow And Public Reconfigure Path (E001)"),
        SnippetCheck("M276-E001-ARCH-02", "tmp/build-objc3c-native/compile_commands.json"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M276-E001-PKG-01", '"build:objc3c-native:reconfigure": "pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/build_objc3c_native.ps1 -ExecutionMode binaries-only -ForceReconfigure"'),
        SnippetCheck("M276-E001-PKG-02", '"check:objc3c:m276-e001-operator-surface": "python scripts/check_m276_e001_package_scripts_operator_workflow_and_developer_runbook_migration_for_incremental_native_builds.py"'),
        SnippetCheck("M276-E001-PKG-03", '"check:objc3c:m276-e001-lane-e-readiness": "python scripts/run_m276_e001_lane_e_readiness.py"'),
    ),
    TEST_FILE: (
        SnippetCheck("M276-E001-TST-01", 'CONTRACT_ID = "objc3c-incremental-native-build-operator-surface/m276-e001-v1"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M276-E001-RDY-01", "check_m276_e001_package_scripts_operator_workflow_and_developer_runbook_migration_for_incremental_native_builds.py"),
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


def dynamic_proof() -> tuple[int, list[Finding], dict[str, object]]:
    summary_dir = SUMMARY_OUT.parent
    summary_dir.mkdir(parents=True, exist_ok=True)
    findings: list[Finding] = []
    checks_total = 4
    completed = run(["npm.cmd", "run", "build:objc3c-native:reconfigure"])
    log_path = summary_dir / "build_objc3c_native_reconfigure.log"
    log_path.write_text(completed.stdout + completed.stderr, encoding="utf-8")
    if completed.returncode != 0:
        findings.append(Finding("dynamic/reconfigure", "M276-E001-DYN-RC", f"reconfigure command failed with exit code {completed.returncode}"))
        return checks_total, findings, {"log": display_path(log_path)}
    if not COMPILE_COMMANDS.exists():
        findings.append(Finding(display_path(COMPILE_COMMANDS), "M276-E001-DYN-COMPILE-COMMANDS", "compile_commands.json missing after reconfigure"))
    if not FINGERPRINT.exists():
        findings.append(Finding(display_path(FINGERPRINT), "M276-E001-DYN-FINGERPRINT", "fingerprint file missing after reconfigure"))
        payload = {}
    else:
        payload = json.loads(FINGERPRINT.read_text(encoding="utf-8"))
        for key in (
            "generator",
            "cmake",
            "ninja",
            "clangxx",
            "llvm_root",
            "llvm_include_dir",
            "libclang",
            "build_dir",
            "source_dir",
            "runtime_output_dir",
            "library_output_dir",
            "direct_object_emission",
            "warning_parity",
        ):
            if key not in payload:
                findings.append(Finding(display_path(FINGERPRINT), "M276-E001-DYN-FINGERPRINT-KEY", f"missing fingerprint key: {key}"))
    return checks_total, findings, {
        "log": display_path(log_path),
        "compile_commands": display_path(COMPILE_COMMANDS),
        "fingerprint": display_path(FINGERPRINT),
        "fingerprint_keys": sorted(payload.keys()),
    }


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
    print(f"[ok] M276-E001 operator surface validated ({checks_total} checks)")
    print(f"[info] wrote summary to {display_path(SUMMARY_OUT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

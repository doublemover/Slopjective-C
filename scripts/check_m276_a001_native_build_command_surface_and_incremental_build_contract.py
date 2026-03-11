#!/usr/bin/env python3
"""Fail-closed checker for M276-A001 native build command surface freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
MODE = "m276-a001-native-build-command-surface-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-native-build-command-surface/m276-a001-v1"
CONTRACT_MODEL = "monolithic-build-remains-authoritative-until-parity-proven-incremental-command-surface-replaces-it"
ARTIFACT_MODEL = "binary-publication-under-artifacts-packet-generation-under-tmp-with-explicit-source-binary-closeout-classes"
PERSISTENCE_MODEL = "persistent-local-build-tree-under-tmp-with-ephemeral-ci-semantics-and-no-delete-based-healing"
VALIDATION_MODEL = "fast-local-issue-work-full-closeout-and-ci-with-default-flip-deferred-until-parity-proof"
NEXT_ISSUE = "M276-A002"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m276" / "M276-A001" / "native_build_command_surface_contract_summary.json"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m276_native_build_command_surface_and_incremental_build_contract_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m276" / "m276_a001_native_build_command_surface_and_incremental_build_contract_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
README = ROOT / "README.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m276_a001_lane_a_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m276_a001_native_build_command_surface_and_incremental_build_contract.py"


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
        SnippetCheck("M276-A001-EXP-01", "# M276 Native Build Command Surface And Incremental Build Contract Expectations (A001)"),
        SnippetCheck("M276-A001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M276-A001-EXP-03", "The contract must explicitly hand off to `M276-A002`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M276-A001-PKT-01", "# M276-A001 Native Build Command Surface And Incremental Build Contract Packet"),
        SnippetCheck("M276-A001-PKT-02", "Issue: `#7386`"),
        SnippetCheck("M276-A001-PKT-03", "`M276-A002` is the explicit next handoff after this freeze closes."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M276-A001-DOCSRC-01", "## Incremental native build surface freeze (M276-A001)"),
        SnippetCheck("M276-A001-DOCSRC-02", "`objc3c-native-build-command-surface/m276-a001-v1`"),
        SnippetCheck("M276-A001-DOCSRC-03", "`build:objc3c-native:contracts`"),
        SnippetCheck("M276-A001-DOCSRC-04", "`build:objc3c-native:reconfigure`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M276-A001-NDOC-01", "## Incremental native build surface freeze (M276-A001)"),
        SnippetCheck("M276-A001-NDOC-02", "future command taxonomy"),
        SnippetCheck("M276-A001-NDOC-03", "`M276-A002`"),
    ),
    README: (
        SnippetCheck("M276-A001-README-01", "## Native Build Surface Modernization"),
        SnippetCheck("M276-A001-README-02", "`npm run build:objc3c-native:contracts`"),
        SnippetCheck("M276-A001-README-03", "persistent local build trees will live under `tmp/`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M276-A001-ARCH-01", "## M276 Native Build Command Surface Freeze (A001)"),
        SnippetCheck("M276-A001-ARCH-02", "current authoritative build path"),
        SnippetCheck("M276-A001-ARCH-03", "`build:objc3c-native:full`"),
    ),
    BUILD_SCRIPT: (
        SnippetCheck("M276-A001-BUILD-01", "M276-A001 native-build-command-surface anchor"),
        SnippetCheck("M276-A001-BUILD-02", "future command taxonomy reserved by contract only at this stage"),
        SnippetCheck("M276-A001-BUILD-03", "build:objc3c-native:reconfigure"),
    ),
    CMAKE_FILE: (
        SnippetCheck("M276-A001-CMAKE-01", "M276-A001 incremental-build-parity anchor"),
        SnippetCheck("M276-A001-CMAKE-02", "reserved future candidate backend for the incremental"),
        SnippetCheck("M276-A001-CMAKE-03", "artifacts/bin and artifacts/lib"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M276-A001-PKG-01", '"check:objc3c:m276-a001-native-build-command-surface-contract": "python scripts/check_m276_a001_native_build_command_surface_and_incremental_build_contract.py"'),
        SnippetCheck("M276-A001-PKG-02", '"test:tooling:m276-a001-native-build-command-surface-contract": "python -m pytest tests/tooling/test_check_m276_a001_native_build_command_surface_and_incremental_build_contract.py -q"'),
        SnippetCheck("M276-A001-PKG-03", '"check:objc3c:m276-a001-lane-a-readiness": "python scripts/run_m276_a001_lane_a_readiness.py"'),
        SnippetCheck("M276-A001-PKG-04", '"build:objc3c-native": "pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/build_objc3c_native.ps1"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M276-A001-RUN-01", "scripts/build_objc3c_native_docs.py"),
        SnippetCheck("M276-A001-RUN-02", "check_m276_a001_native_build_command_surface_and_incremental_build_contract.py"),
        SnippetCheck("M276-A001-RUN-03", "test_check_m276_a001_native_build_command_surface_and_incremental_build_contract.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M276-A001-TEST-01", "def test_m276_a001_checker_emits_summary() -> None:"),
        SnippetCheck("M276-A001-TEST-02", CONTRACT_ID),
    ),
}


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


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


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    failures: list[Finding] = []
    checks_total = 0
    for path, snippets in STATIC_SNIPPETS.items():
        total, local_failures = check_static_contract(path, snippets)
        checks_total += total
        failures.extend(local_failures)

    summary: dict[str, Any] = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "contract_model": CONTRACT_MODEL,
        "artifact_model": ARTIFACT_MODEL,
        "persistence_model": PERSISTENCE_MODEL,
        "validation_model": VALIDATION_MODEL,
        "next_issue": NEXT_ISSUE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "findings": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        print(f"[info] wrote summary to {display_path(args.summary_out)}", file=sys.stderr)
        return 1
    print(f"[ok] M276-A001 native build command surface contract validated ({checks_total} checks)")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

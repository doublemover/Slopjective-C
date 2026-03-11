#!/usr/bin/env python3
"""Fail-closed checker for M276-A002 build graph and toolchain parity freeze."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
MODE = "m276-a002-build-graph-toolchain-parity-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-native-build-graph-toolchain-parity/m276-a002-v1"
CONTRACT_MODEL = "same-source-set-but-not-yet-same-toolchain-link-publication-or-packet-behavior"
PARITY_MODEL = "behavioral-build-product-parity-with-stable-publication-paths-not-identical-object-topology"
TOOLCHAIN_MODEL = "wrapper-owned-llvm-root-and-libclang-discovery-flow-into-future-configure-steps"
COMPILE_DATABASE_MODEL = "future-incremental-backend-emits-compile-commands-under-tmp-build-objc3c-native"
NEXT_ISSUE = "M276-C001"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m276" / "M276-A002" / "build_graph_and_toolchain_parity_summary.json"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m276_build_graph_and_toolchain_parity_for_incremental_native_builds_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m276" / "m276_a002_build_graph_and_toolchain_parity_for_incremental_native_builds_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
README = ROOT / "README.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"
OBJECTIVEC_PATH = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objectivec_path.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m276_a002_lane_a_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m276_a002_build_graph_and_toolchain_parity_for_incremental_native_builds.py"

SCRIPT_SOURCE_RE = re.compile(r'"(native/objc3c/src/[^"]+\.(?:cpp|h))"')
CMAKE_SOURCE_RE = re.compile(r'\bsrc/[^\s)]+\.(?:cpp|h)\b')


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
        SnippetCheck("M276-A002-EXP-01", "# M276 Build Graph And Toolchain Parity For Incremental Native Builds Expectations (A002)"),
        SnippetCheck("M276-A002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M276-A002-EXP-03", "The contract must explicitly hand off to `M276-C001`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M276-A002-PKT-01", "# M276-A002 Build Graph And Toolchain Parity For Incremental Native Builds Packet"),
        SnippetCheck("M276-A002-PKT-02", "Issue: `#7392`"),
        SnippetCheck("M276-A002-PKT-03", "`M276-C001` is the explicit next handoff after this freeze closes."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M276-A002-DOCSRC-01", "## Build graph and toolchain parity freeze (M276-A002)"),
        SnippetCheck("M276-A002-DOCSRC-02", "`objc3c-native-build-graph-toolchain-parity/m276-a002-v1`"),
        SnippetCheck("M276-A002-DOCSRC-03", "same in-tree native source set"),
        SnippetCheck("M276-A002-DOCSRC-04", "compile database emission at `tmp/build-objc3c-native/compile_commands.json`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M276-A002-NDOC-01", "## Build graph and toolchain parity freeze (M276-A002)"),
        SnippetCheck("M276-A002-NDOC-02", "same in-tree native source set"),
        SnippetCheck("M276-A002-NDOC-03", "`M276-C001`"),
    ),
    README: (
        SnippetCheck("M276-A002-README-01", "Parity freeze (`M276-A002`):"),
        SnippetCheck("M276-A002-README-02", "same in-tree native source set"),
        SnippetCheck("M276-A002-README-03", "`tmp/build-objc3c-native/compile_commands.json`"),
        SnippetCheck("M276-A002-README-04", "`LLVM_ROOT`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M276-A002-ARCH-01", "## M276 Build Graph And Toolchain Parity Freeze (A002)"),
        SnippetCheck("M276-A002-ARCH-02", "same in-tree"),
        SnippetCheck("M276-A002-ARCH-03", "compile-database emission at"),
        SnippetCheck("M276-A002-ARCH-04", "`M276-C001`"),
    ),
    BUILD_SCRIPT: (
        SnippetCheck("M276-A002-BUILD-01", "M276-A002 build-graph-and-toolchain-parity anchor"),
        SnippetCheck("M276-A002-BUILD-02", "authoritative toolchain flow today is `LLVM_ROOT`"),
        SnippetCheck("M276-A002-BUILD-03", "compile database parity frozen to `tmp/build-objc3c-native/compile_commands.json`"),
    ),
    CMAKE_FILE: (
        SnippetCheck("M276-A002-CMAKE-01", "M276-A002 incremental-build-parity anchor"),
        SnippetCheck("M276-A002-CMAKE-02", "compile database emission at `tmp/build-objc3c-native/compile_commands.json`"),
        SnippetCheck("M276-A002-CMAKE-03", "wrapper-owned `LLVM_ROOT` / `libclang` discovery"),
    ),
    OBJECTIVEC_PATH: (
        SnippetCheck("M276-A002-OBJCPATH-01", "M276-A002 parity anchor"),
        SnippetCheck("M276-A002-OBJCPATH-02", "concrete libclang"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M276-A002-PKG-01", '"check:objc3c:m276-a002-build-graph-toolchain-parity-contract": "python scripts/check_m276_a002_build_graph_and_toolchain_parity_for_incremental_native_builds.py"'),
        SnippetCheck("M276-A002-PKG-02", '"test:tooling:m276-a002-build-graph-toolchain-parity-contract": "python -m pytest tests/tooling/test_check_m276_a002_build_graph_and_toolchain_parity_for_incremental_native_builds.py -q"'),
        SnippetCheck("M276-A002-PKG-03", '"check:objc3c:m276-a002-lane-a-readiness": "python scripts/run_m276_a002_lane_a_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M276-A002-RUN-01", "scripts/build_objc3c_native_docs.py"),
        SnippetCheck("M276-A002-RUN-02", "check_m276_a002_build_graph_and_toolchain_parity_for_incremental_native_builds.py"),
        SnippetCheck("M276-A002-RUN-03", "test_check_m276_a002_build_graph_and_toolchain_parity_for_incremental_native_builds.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M276-A002-TEST-01", "def test_m276_a002_checker_emits_summary() -> None:"),
        SnippetCheck("M276-A002-TEST-02", CONTRACT_ID),
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


def parse_script_inventory(text: str) -> list[str]:
    return sorted(set(SCRIPT_SOURCE_RE.findall(text)))


def parse_cmake_inventory(text: str) -> list[str]:
    return sorted({f"native/objc3c/{match}" for match in CMAKE_SOURCE_RE.findall(text)})


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


def dynamic_findings() -> tuple[int, list[Finding], dict[str, Any]]:
    findings: list[Finding] = []
    checks_total = 0
    script_text = BUILD_SCRIPT.read_text(encoding="utf-8")
    cmake_text = CMAKE_FILE.read_text(encoding="utf-8")
    script_inventory = parse_script_inventory(script_text)
    cmake_inventory = parse_cmake_inventory(cmake_text)
    missing_in_cmake = sorted(set(script_inventory) - set(cmake_inventory))
    extra_in_cmake = sorted(set(cmake_inventory) - set(script_inventory))
    checks_total += 1
    if missing_in_cmake or extra_in_cmake:
        findings.append(Finding("dynamic/source-inventory", "M276-A002-DYN-INV", "source inventory parity mismatch between PowerShell build and CMakeLists.txt"))

    expected_gap_checks = {
        "llvm_root_and_tools": "LLVM_ROOT",
        "llvm_libclang": "libclang",
        "artifact_publication": "artifacts/",
        "packet_generation": "packet family",
        "compile_database": "tmp/build-objc3c-native/compile_commands.json",
    }
    doc_texts = {
        "doc_source": DOC_SOURCE.read_text(encoding="utf-8"),
        "readme": README.read_text(encoding="utf-8"),
        "architecture": ARCHITECTURE_DOC.read_text(encoding="utf-8"),
        "packet": PACKET_DOC.read_text(encoding="utf-8"),
    }
    for check_id, needle in expected_gap_checks.items():
        checks_total += 1
        if not any(needle in text for text in doc_texts.values()):
            findings.append(Finding("dynamic/parity-gaps", f"M276-A002-DYN-{check_id.upper()}", f"expected documented parity gap marker missing: {needle}"))

    checks_total += 1
    if 'target_link_libraries(objc3c-native PRIVATE libclang)' in cmake_text or 'target_link_libraries(objc3c-frontend-c-api-runner PRIVATE libclang)' in cmake_text:
        findings.append(Finding("dynamic/cmake", "M276-A002-DYN-LIBCLANG", "CMake unexpectedly claims direct libclang linkage parity already exists"))

    checks_total += 1
    if 'RUNTIME_OUTPUT_DIRECTORY' in cmake_text or 'ARCHIVE_OUTPUT_DIRECTORY' in cmake_text or 'LIBRARY_OUTPUT_DIRECTORY' in cmake_text:
        findings.append(Finding("dynamic/cmake", "M276-A002-DYN-OUTPUTDIR", "CMake unexpectedly claims output publication parity already exists"))

    checks_total += 1
    if 'CMAKE_EXPORT_COMPILE_COMMANDS' in cmake_text and 'tmp/build-objc3c-native/compile_commands.json' not in cmake_text:
        findings.append(Finding("dynamic/cmake", "M276-A002-DYN-COMPILE-COMMANDS", "compile database policy appears partially implemented without frozen path text"))

    summary = {
        "script_inventory_count": len(script_inventory),
        "cmake_inventory_count": len(cmake_inventory),
        "missing_in_cmake": missing_in_cmake,
        "extra_in_cmake": extra_in_cmake,
        "script_inventory": script_inventory,
        "cmake_inventory": cmake_inventory,
    }
    return checks_total, findings, summary


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

    dynamic_total, dynamic_failures, inventory_summary = dynamic_findings()
    checks_total += dynamic_total
    failures.extend(dynamic_failures)

    summary: dict[str, Any] = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "contract_model": CONTRACT_MODEL,
        "parity_model": PARITY_MODEL,
        "toolchain_model": TOOLCHAIN_MODEL,
        "compile_database_model": COMPILE_DATABASE_MODEL,
        "next_issue": NEXT_ISSUE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "inventory_summary": inventory_summary,
        "findings": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        print(f"[info] wrote summary to {display_path(args.summary_out)}", file=sys.stderr)
        return 1
    print(f"[ok] M276-A002 build graph and toolchain parity contract validated ({checks_total} checks)")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

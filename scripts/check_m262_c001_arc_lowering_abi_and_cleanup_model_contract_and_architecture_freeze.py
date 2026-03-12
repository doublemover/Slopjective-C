#!/usr/bin/env python3
"""Deterministic checker for M262-C001 ARC lowering ABI and cleanup model freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m262-c001-arc-lowering-abi-and-cleanup-model-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-arc-lowering-abi-cleanup-model/m262-c001-v1"
ABI_MODEL = "owned-value-lowering-targets-private-runtime-retain-release-autorelease-and-weak-helper-entrypoints-without-public-runtime-abi-expansion"
CLEANUP_MODEL = "cleanup-scheduling-remains-explicit-summary-and-helper-boundary-only-until-m262-c002"
FAIL_CLOSED_MODEL = "no-implicit-cleanup-scope-insertion-no-helper-rebinding-no-public-runtime-abi-widening-before-later-lane-c-runtime-work"
NON_GOAL_MODEL = "no-general-arc-cleanup-emission-no-weak-load-store-lowering-no-autorelease-return-rewrite-automation-yet"
NEXT_ISSUE = "M262-C002"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m262_arc_lowering_abi_and_cleanup_model_contract_and_architecture_freeze_c001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m262" / "m262_c001_arc_lowering_abi_and_cleanup_model_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
ATTRIBUTE_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
SEMA_PASS_MANAGER_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SCAFFOLD_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_ownership_aware_lowering_behavior_scaffold.h"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m262_c001_lane_c_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m262_c001_arc_lowering_abi_and_cleanup_model_contract_and_architecture_freeze.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
PROPERTY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_property_interaction_positive.objc3"
AUTORELEASE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_autorelease_return_positive.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m262" / "c001-arc-lowering-abi"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m262" / "M262-C001" / "arc_lowering_abi_cleanup_model_summary.json"
BOUNDARY_PREFIX = "; arc_lowering_abi_cleanup_model = "
RETAIN_SYMBOL = "objc3_runtime_retain_i32"
RELEASE_SYMBOL = "objc3_runtime_release_i32"
AUTORELEASE_SYMBOL = "objc3_runtime_autorelease_i32"
WEAK_LOAD_SYMBOL = "objc3_runtime_load_weak_current_property_i32"
WEAK_STORE_SYMBOL = "objc3_runtime_store_weak_current_property_i32"
PUSH_POOL_SYMBOL = "objc3_runtime_push_autoreleasepool_scope"
POP_POOL_SYMBOL = "objc3_runtime_pop_autoreleasepool_scope"


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
        SnippetCheck("M262-C001-EXP-01", "# M262 ARC Lowering ABI And Cleanup Model Contract And Architecture Freeze Expectations (C001)"),
        SnippetCheck("M262-C001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M262-C001-EXP-03", "`objc3_runtime_retain_i32`"),
        SnippetCheck("M262-C001-EXP-04", "The contract must explicitly hand off to `M262-C002`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M262-C001-PKT-01", "# M262-C001 ARC Lowering ABI And Cleanup Model Contract And Architecture Freeze Packet"),
        SnippetCheck("M262-C001-PKT-02", "Issue: `#7199`"),
        SnippetCheck("M262-C001-PKT-03", "Packet: `M262-C001`"),
        SnippetCheck("M262-C001-PKT-04", "`M262-C002` is the explicit next handoff after this freeze closes."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M262-C001-SRC-01", "## M262 ARC lowering ABI and cleanup model (M262-C001)"),
        SnippetCheck("M262-C001-SRC-02", "`objc3c-arc-lowering-abi-cleanup-model/m262-c001-v1`"),
        SnippetCheck("M262-C001-SRC-03", "`; arc_lowering_abi_cleanup_model = ...`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M262-C001-NDOC-01", "## M262 ARC lowering ABI and cleanup model (M262-C001)"),
        SnippetCheck("M262-C001-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M262-C001-NDOC-03", "`M262-C002` is the next issue."),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M262-C001-SPC-01", "## M262 ARC lowering ABI and cleanup model (C001)"),
        SnippetCheck("M262-C001-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M262-C001-SPC-03", f"`{ABI_MODEL}`"),
        SnippetCheck("M262-C001-SPC-04", "`M262-C002` is the next issue."),
    ),
    ATTRIBUTE_SPEC: (
        SnippetCheck("M262-C001-ATTR-01", "### B.2.7 ARC interaction semantics (implementation note) {#b-2-7}"),
        SnippetCheck("M262-C001-ATTR-02", "full ARC cleanup synthesis and broader ARC interaction surfaces remain"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M262-C001-ARCH-01", "## M262 ARC Lowering ABI And Cleanup Model (C001)"),
        SnippetCheck("M262-C001-ARCH-02", "current ARC lowering boundary as the combination of ARC"),
        SnippetCheck("M262-C001-ARCH-03", "the next issue is `M262-C002`"),
    ),
    SEMA_CPP: (
        SnippetCheck("M262-C001-SEMA-01", "M262-C001 ARC lowering ABI/cleanup freeze anchor"),
        SnippetCheck("M262-C001-SEMA-02", "it does not schedule cleanups or choose lowering helper"),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M262-C001-PM-01", "M262-C001 ARC lowering ABI/cleanup freeze anchor"),
        SnippetCheck("M262-C001-PM-02", "semantic-only and must not silently claim cleanup scheduling"),
    ),
    SCAFFOLD_HEADER: (
        SnippetCheck("M262-C001-SCAF-01", "M262-C001 ARC lowering ABI/cleanup freeze anchor"),
        SnippetCheck("M262-C001-SCAF-02", "does not by itself schedule ARC cleanup"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M262-C001-LHDR-01", "kObjc3ArcLoweringAbiCleanupModelContractId"),
        SnippetCheck("M262-C001-LHDR-02", "std::string Objc3ArcLoweringAbiCleanupModelSummary();"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M262-C001-LCPP-01", "std::string Objc3ArcLoweringAbiCleanupModelSummary()"),
        SnippetCheck("M262-C001-LCPP-02", ";cleanup_model="),
        SnippetCheck("M262-C001-LCPP-03", ";next_issue=M262-C002"),
    ),
    IR_CPP: (
        SnippetCheck("M262-C001-IR-01", "M262-C001 ARC lowering ABI/cleanup freeze anchor"),
        SnippetCheck("M262-C001-IR-02", "; arc_lowering_abi_cleanup_model = "),
        SnippetCheck("M262-C001-IR-03", "Objc3ArcLoweringAbiCleanupModelSummary()"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M262-C001-PKG-01", '"check:objc3c:m262-c001-arc-lowering-abi-and-cleanup-model-contract": "python scripts/check_m262_c001_arc_lowering_abi_and_cleanup_model_contract_and_architecture_freeze.py"'),
        SnippetCheck("M262-C001-PKG-02", '"test:tooling:m262-c001-arc-lowering-abi-and-cleanup-model-contract": "python -m pytest tests/tooling/test_check_m262_c001_arc_lowering_abi_and_cleanup_model_contract_and_architecture_freeze.py -q"'),
        SnippetCheck("M262-C001-PKG-03", '"check:objc3c:m262-c001-lane-c-readiness": "python scripts/run_m262_c001_lane_c_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M262-C001-RUN-01", "scripts/build_objc3c_native_docs.py"),
        SnippetCheck("M262-C001-RUN-02", "ensure_objc3c_native_build.py"),
        SnippetCheck("M262-C001-RUN-03", "test_check_m262_c001_arc_lowering_abi_and_cleanup_model_contract_and_architecture_freeze.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M262-C001-TEST-01", "def test_m262_c001_checker_emits_summary() -> None:"),
        SnippetCheck("M262-C001-TEST-02", CONTRACT_ID),
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


def run_process(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


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


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 1


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def compile_fixture(fixture: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [str(NATIVE_EXE), str(fixture), "-fobjc-arc", "--out-dir", str(out_dir), "--emit-prefix", "module"]
    return run_process(command)


def run_dynamic_probes(failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    payload: dict[str, Any] = {}

    property_out_dir = PROBE_ROOT / "property"
    autorelease_out_dir = PROBE_ROOT / "autorelease"

    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M262-C001-DYN-01", "native binary is missing", failures)
    checks_total += require(PROPERTY_FIXTURE.exists(), display_path(PROPERTY_FIXTURE), "M262-C001-DYN-02", "property fixture is missing", failures)
    checks_total += require(AUTORELEASE_FIXTURE.exists(), display_path(AUTORELEASE_FIXTURE), "M262-C001-DYN-03", "autorelease fixture is missing", failures)
    if failures:
        return checks_total, payload

    property_completed = compile_fixture(PROPERTY_FIXTURE, property_out_dir)
    property_ir = property_out_dir / "module.ll"
    property_obj = property_out_dir / "module.obj"
    checks_total += require(property_completed.returncode == 0, display_path(property_out_dir), "M262-C001-DYN-04", f"property ARC compile failed: {property_completed.stdout}{property_completed.stderr}", failures)
    checks_total += require(property_ir.exists(), display_path(property_ir), "M262-C001-DYN-05", "property IR is missing", failures)
    checks_total += require(property_obj.exists(), display_path(property_obj), "M262-C001-DYN-06", "property object is missing", failures)
    if property_ir.exists():
        property_ir_text = property_ir.read_text(encoding="utf-8")
        boundary_line = next((line for line in property_ir_text.splitlines() if line.startswith(BOUNDARY_PREFIX)), "")
        checks_total += require(bool(boundary_line), display_path(property_ir), "M262-C001-DYN-07", "property IR is missing the ARC lowering ABI boundary line", failures)
        checks_total += require(f"contract={CONTRACT_ID}" in boundary_line, display_path(property_ir), "M262-C001-DYN-08", "property boundary line must carry the contract id", failures)
        checks_total += require(RETAIN_SYMBOL in property_ir_text, display_path(property_ir), "M262-C001-DYN-09", "property IR must retain the current retain helper symbol", failures)
        checks_total += require(RELEASE_SYMBOL in property_ir_text, display_path(property_ir), "M262-C001-DYN-10", "property IR must retain the current release helper symbol", failures)
        checks_total += require(WEAK_STORE_SYMBOL in property_ir_text, display_path(property_ir), "M262-C001-DYN-11", "property IR must retain the current weak-store helper symbol", failures)
        payload["property_case"] = {
            "out_dir": display_path(property_out_dir),
            "boundary_line": boundary_line,
            "retain_symbol_present": RETAIN_SYMBOL in property_ir_text,
            "release_symbol_present": RELEASE_SYMBOL in property_ir_text,
            "weak_store_symbol_present": WEAK_STORE_SYMBOL in property_ir_text,
        }

    autorelease_completed = compile_fixture(AUTORELEASE_FIXTURE, autorelease_out_dir)
    autorelease_ir = autorelease_out_dir / "module.ll"
    autorelease_obj = autorelease_out_dir / "module.obj"
    checks_total += require(autorelease_completed.returncode == 0, display_path(autorelease_out_dir), "M262-C001-DYN-12", f"autorelease ARC compile failed: {autorelease_completed.stdout}{autorelease_completed.stderr}", failures)
    checks_total += require(autorelease_ir.exists(), display_path(autorelease_ir), "M262-C001-DYN-13", "autorelease IR is missing", failures)
    checks_total += require(autorelease_obj.exists(), display_path(autorelease_obj), "M262-C001-DYN-14", "autorelease object is missing", failures)
    if autorelease_ir.exists():
        autorelease_ir_text = autorelease_ir.read_text(encoding="utf-8")
        boundary_line = next((line for line in autorelease_ir_text.splitlines() if line.startswith(BOUNDARY_PREFIX)), "")
        checks_total += require(bool(boundary_line), display_path(autorelease_ir), "M262-C001-DYN-15", "autorelease IR is missing the ARC lowering ABI boundary line", failures)
        checks_total += require(AUTORELEASE_SYMBOL in autorelease_ir_text, display_path(autorelease_ir), "M262-C001-DYN-16", "autorelease IR must retain the current autorelease helper symbol", failures)
        payload["autorelease_case"] = {
            "out_dir": display_path(autorelease_out_dir),
            "boundary_line": boundary_line,
            "autorelease_symbol_present": AUTORELEASE_SYMBOL in autorelease_ir_text,
        }

    return checks_total, payload


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0

    for path, snippets in STATIC_SNIPPETS.items():
        total, path_failures = check_static_contract(path, snippets)
        checks_total += total
        failures.extend(path_failures)

    dynamic_payload: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        dynamic_total, dynamic_payload = run_dynamic_probes(failures)
        checks_total += dynamic_total

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": MODE,
        "issue": "M262-C001",
        "contract_id": CONTRACT_ID,
        "abi_model": ABI_MODEL,
        "cleanup_model": CLEANUP_MODEL,
        "fail_closed_model": FAIL_CLOSED_MODEL,
        "non_goal_model": NON_GOAL_MODEL,
        "next_issue": NEXT_ISSUE,
        "checks_total": checks_total,
        "checks_failed": len(failures),
        "failures": [finding.__dict__ for finding in failures],
        "dynamic": dynamic_payload,
    }
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    print(f"[m262-c001] summary written to {display_path(args.summary_out)}")
    if failures:
        for finding in failures:
            print(f"[m262-c001] {finding.artifact}: {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

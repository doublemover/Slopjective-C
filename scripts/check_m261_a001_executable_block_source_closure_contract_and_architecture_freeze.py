#!/usr/bin/env python3
"""Fail-closed checker for M261-A001 executable block source closure."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m261-a001-executable-block-source-closure-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-executable-block-source-closure/m261-a001-v1"
SOURCE_MODEL = (
    "parser-owned-block-literal-source-closure-freezes-capture-abi-storage-copy-dispose-and-baseline-profiles-before-runnable-block-realization"
)
EVIDENCE_MODEL = "hello-ir-boundary-plus-block-literal-o3s221-fail-closed-native-probe"
FAIL_CLOSED_MODEL = "fail-closed-on-block-source-surface-drift-before-block-runtime-realization"
NON_GOAL_MODEL = "no-block-pointer-declarator-spellings-no-explicit-byref-storage-spellings-no-block-runtime-lowering"
NEXT_ISSUE = "M261-A002"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m261" / "M261-A001" / "executable_block_source_closure_summary.json"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m261_executable_block_source_closure_contract_and_architecture_freeze_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m261" / "m261_a001_executable_block_source_closure_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
PART0_SPEC = ROOT / "spec" / "PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
SEMA_PASS_MANAGER_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m261_a001_lane_a_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m261_a001_executable_block_source_closure_contract_and_architecture_freeze.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
HELLO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
BLOCK_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_executable_block_source_closure_positive.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m261" / "a001-block-source-closure"
EXPECTED_BOUNDARY = (
    "; executable_block_source_closure = "
    f"contract={CONTRACT_ID};source_model={SOURCE_MODEL};evidence_model={EVIDENCE_MODEL};"
    f"non_goal_model={NON_GOAL_MODEL};fail_closed_model={FAIL_CLOSED_MODEL};"
    "capture_lane_contract=m166-block-literal-capture-lowering-v1"
)
FORBIDDEN_PARSER_CODES = {"O3P166", "O3P110", "O3P104", "O3P100"}


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
        SnippetCheck("M261-A001-EXP-01", "# M261 Executable Block Source Closure Contract And Architecture Freeze Expectations (A001)"),
        SnippetCheck("M261-A001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M261-A001-EXP-03", "`O3S221`"),
        SnippetCheck("M261-A001-EXP-04", "The contract must explicitly hand off to `M261-A002`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M261-A001-PKT-01", "# M261-A001 Executable Block Source Closure Contract And Architecture Freeze Packet"),
        SnippetCheck("M261-A001-PKT-02", "Issue: `#7179`"),
        SnippetCheck("M261-A001-PKT-03", "Packet: `M261-A001`"),
        SnippetCheck("M261-A001-PKT-04", "`M261-A002` is the explicit next handoff after this freeze closes."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M261-A001-SRC-01", "## M261 executable block source closure (M261-A001)"),
        SnippetCheck("M261-A001-SRC-02", "semantic validation still rejects block literals with `O3S221`"),
        SnippetCheck("M261-A001-SRC-03", "no explicit `__block` byref storage spellings yet."),
    ),
    NATIVE_DOC: (
        SnippetCheck("M261-A001-NDOC-01", "## M261 executable block source closure (M261-A001)"),
        SnippetCheck("M261-A001-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-A001-NDOC-03", "`M261-A002` is the next issue."),
    ),
    PART0_SPEC: (
        SnippetCheck("M261-A001-P0-01", "Current implementation boundary:"),
        SnippetCheck("M261-A001-P0-02", "(`M261-A001`)"),
        SnippetCheck("M261-A001-P0-03", "explicit byref storage"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M261-A001-SPC-01", "## M261 executable block source closure (A001)"),
        SnippetCheck("M261-A001-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-A001-SPC-03", f"`{FAIL_CLOSED_MODEL}`"),
        SnippetCheck("M261-A001-SPC-04", "`M261-A002` is the next issue."),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M261-A001-ARCH-01", "## M261 Executable Block Source Closure (A001)"),
        SnippetCheck("M261-A001-ARCH-02", "semantic validation still rejects block literals"),
        SnippetCheck("M261-A001-ARCH-03", "the next issue is `M261-A002`"),
    ),
    PARSER_CPP: (
        SnippetCheck("M261-A001-PARSER-01", "M261-A001 executable-block-source-closure anchor"),
        SnippetCheck("M261-A001-PARSER-02", "if (!At(TokenKind::LBrace))"),
    ),
    AST_HEADER: (
        SnippetCheck("M261-A001-AST-01", "kObjc3ExecutableBlockSourceClosureContractId"),
        SnippetCheck("M261-A001-AST-02", "kObjc3ExecutableBlockSourceNonGoalModel"),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M261-A001-SEMA-PM-01", "M261-A001 executable-block-source-closure anchor"),
        SnippetCheck("M261-A001-SEMA-PM-02", "IsEquivalentBlockLiteralCaptureSemanticsSummary"),
    ),
    SEMA_CPP: (
        SnippetCheck("M261-A001-SEMA-01", "M261-A001 executable-block-source-closure anchor"),
        SnippetCheck("M261-A001-SEMA-02", "unsupported feature claim: block literals are not yet runnable in Objective-C 3 native mode"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M261-A001-LOWER-H-01", "std::string Objc3ExecutableBlockSourceClosureSummary();"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M261-A001-LOWER-CPP-01", "std::string Objc3ExecutableBlockSourceClosureSummary()"),
        SnippetCheck("M261-A001-LOWER-CPP-02", "capture_lane_contract="),
    ),
    IR_CPP: (
        SnippetCheck("M261-A001-IR-01", "M261-A001 executable-block-source-closure anchor"),
        SnippetCheck("M261-A001-IR-02", "; executable_block_source_closure = "),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M261-A001-PKG-01", '"check:objc3c:m261-a001-executable-block-source-closure-contract": "python scripts/check_m261_a001_executable_block_source_closure_contract_and_architecture_freeze.py"'),
        SnippetCheck("M261-A001-PKG-02", '"test:tooling:m261-a001-executable-block-source-closure-contract": "python -m pytest tests/tooling/test_check_m261_a001_executable_block_source_closure_contract_and_architecture_freeze.py -q"'),
        SnippetCheck("M261-A001-PKG-03", '"check:objc3c:m261-a001-lane-a-readiness": "python scripts/run_m261_a001_lane_a_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M261-A001-RUN-01", "scripts/build_objc3c_native_docs.py"),
        SnippetCheck("M261-A001-RUN-02", "build:objc3c-native"),
        SnippetCheck("M261-A001-RUN-03", "test_check_m261_a001_executable_block_source_closure_contract_and_architecture_freeze.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M261-A001-TEST-01", "def test_m261_a001_checker_emits_summary() -> None:"),
        SnippetCheck("M261-A001-TEST-02", CONTRACT_ID),
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


def run_dynamic_probes(failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    payload: dict[str, Any] = {}
    hello_out_dir = PROBE_ROOT / "hello"
    block_out_dir = PROBE_ROOT / "block-literal"
    hello_out_dir.mkdir(parents=True, exist_ok=True)
    block_out_dir.mkdir(parents=True, exist_ok=True)

    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M261-A001-DYN-01", "native binary is missing", failures)
    checks_total += require(HELLO_FIXTURE.exists(), display_path(HELLO_FIXTURE), "M261-A001-DYN-02", "hello fixture is missing", failures)
    checks_total += require(BLOCK_FIXTURE.exists(), display_path(BLOCK_FIXTURE), "M261-A001-DYN-03", "block fixture is missing", failures)
    if failures:
        return checks_total, payload

    hello_completed = run_process([
        str(NATIVE_EXE),
        str(HELLO_FIXTURE),
        "--out-dir",
        str(hello_out_dir),
        "--emit-prefix",
        "module",
    ])
    hello_ir = hello_out_dir / "module.ll"
    hello_diag = hello_out_dir / "module.diagnostics.json"
    checks_total += require(hello_completed.returncode == 0, display_path(hello_out_dir), "M261-A001-DYN-04", f"hello compile failed: {hello_completed.stdout}{hello_completed.stderr}", failures)
    checks_total += require(hello_ir.exists(), display_path(hello_ir), "M261-A001-DYN-05", "hello IR is missing", failures)
    checks_total += require(hello_diag.exists(), display_path(hello_diag), "M261-A001-DYN-06", "hello diagnostics are missing", failures)
    if hello_completed.returncode == 0 and hello_ir.exists() and hello_diag.exists():
        hello_diag_payload = load_json(hello_diag)
        hello_ir_text = hello_ir.read_text(encoding="utf-8")
        checks_total += require(hello_diag_payload.get("diagnostics") == [], display_path(hello_diag), "M261-A001-DYN-07", "hello fixture must stay diagnostics-clean", failures)
        checks_total += require(EXPECTED_BOUNDARY in hello_ir_text, display_path(hello_ir), "M261-A001-DYN-08", "hello IR is missing the executable block source closure boundary", failures)
        payload["hello_case"] = {
            "out_dir": display_path(hello_out_dir),
            "ir_path": display_path(hello_ir),
            "boundary_present": EXPECTED_BOUNDARY in hello_ir_text,
        }

    block_completed = run_process([
        str(NATIVE_EXE),
        str(BLOCK_FIXTURE),
        "--out-dir",
        str(block_out_dir),
        "--emit-prefix",
        "module",
    ])
    block_diag = block_out_dir / "module.diagnostics.json"
    manifest_path = block_out_dir / "module.manifest.json"
    ir_path = block_out_dir / "module.ll"
    obj_path = block_out_dir / "module.obj"
    checks_total += require(block_completed.returncode != 0, display_path(block_out_dir), "M261-A001-DYN-09", "block literal fixture must still fail closed", failures)
    checks_total += require(block_diag.exists(), display_path(block_diag), "M261-A001-DYN-10", "block literal diagnostics are missing", failures)
    diagnostic_codes: list[str] = []
    if block_diag.exists():
        block_diag_payload = load_json(block_diag)
        diagnostics = block_diag_payload.get("diagnostics", [])
        if isinstance(diagnostics, list):
            diagnostic_codes = [str(diag.get("code", "")) for diag in diagnostics if isinstance(diag, dict)]
        checks_total += require("O3S221" in diagnostic_codes, display_path(block_diag), "M261-A001-DYN-11", f"expected O3S221 semantic rejection, observed {diagnostic_codes}", failures)
        forbidden_hits = sorted(code for code in diagnostic_codes if code in FORBIDDEN_PARSER_CODES)
        checks_total += require(not forbidden_hits, display_path(block_diag), "M261-A001-DYN-12", f"unexpected parser regressions surfaced: {forbidden_hits}", failures)
    checks_total += require(not manifest_path.exists(), display_path(manifest_path), "M261-A001-DYN-13", "block literal fixture should not emit a manifest after fail-closed rejection", failures)
    checks_total += require(not ir_path.exists(), display_path(ir_path), "M261-A001-DYN-14", "block literal fixture should not emit IR after fail-closed rejection", failures)
    checks_total += require(not obj_path.exists(), display_path(obj_path), "M261-A001-DYN-15", "block literal fixture should not emit an object after fail-closed rejection", failures)
    payload["block_case"] = {
        "out_dir": display_path(block_out_dir),
        "diagnostics_path": display_path(block_diag),
        "diagnostic_codes": diagnostic_codes,
        "manifest_emitted": manifest_path.exists(),
        "ir_emitted": ir_path.exists(),
        "obj_emitted": obj_path.exists(),
    }
    return checks_total, payload


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0

    for path, snippets in STATIC_SNIPPETS.items():
        static_total, static_failures = check_static_contract(path, snippets)
        checks_total += static_total
        failures.extend(static_failures)

    dynamic_payload: dict[str, Any]
    if args.skip_dynamic_probes:
        dynamic_payload = {"skipped": True}
    else:
        dynamic_total, dynamic_payload = run_dynamic_probes(failures)
        checks_total += dynamic_total

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "source_model": SOURCE_MODEL,
        "evidence_model": EVIDENCE_MODEL,
        "non_goal_model": NON_GOAL_MODEL,
        "fail_closed_model": FAIL_CLOSED_MODEL,
        "next_issue": NEXT_ISSUE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_case": dynamic_payload,
        "findings": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        print(f"[info] wrote summary to {display_path(args.summary_out)}", file=sys.stderr)
        return 1
    print(f"[ok] M261-A001 executable block source closure validated ({checks_total} checks)")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

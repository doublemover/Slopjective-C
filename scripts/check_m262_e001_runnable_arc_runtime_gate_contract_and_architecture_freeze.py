#!/usr/bin/env python3
"""Fail-closed checker for M262-E001 runnable ARC runtime gate."""

from __future__ import annotations

import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m262-e001-runnable-arc-runtime-gate-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-runnable-arc-runtime-gate/m262-e001-v1"
EVIDENCE_MODEL = "a002-b003-c004-d003-summary-chain"
ACTIVE_GATE_MODEL = (
    "runnable-arc-gate-consumes-arc-mode-semantics-lowering-and-runtime-proofs-rather-than-parser-only-or-metadata-only-claims"
)
NON_GOAL_MODEL = (
    "no-runnable-arc-closeout-matrix-no-public-runtime-abi-widening-no-cross-module-arc-claims-before-m262-e002"
)
FAIL_CLOSED_MODEL = "fail-closed-on-runnable-arc-runtime-evidence-drift"
NEXT_CLOSEOUT_ISSUE = "M262-E002"

M262_A002_CONTRACT_ID = "objc3c-arc-mode-handling/m262-a002-v1"
M262_B003_CONTRACT_ID = "objc3c-arc-interaction-semantics/m262-b003-v1"
M262_C004_CONTRACT_ID = "objc3c-arc-block-autorelease-return-lowering/m262-c004-v1"
M262_D003_CONTRACT_ID = "objc3c-runtime-arc-debug-instrumentation/m262-d003-v1"

BOUNDARY_PREFIX = "; runnable_arc_runtime_gate = "
NAMED_METADATA_LINE = "!objc3.objc_runnable_arc_runtime_gate = !{!86}"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m262_runnable_arc_runtime_gate_contract_and_architecture_freeze_e001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m262" / "m262_e001_runnable_arc_runtime_gate_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
SYNTAX_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
SEMA_PASS_MANAGER_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SCAFFOLD_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_ownership_aware_lowering_behavior_scaffold.h"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
PROPERTY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_property_interaction_positive.objc3"
BLOCK_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_block_autorelease_return_positive.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m262" / "e001"
A002_SUMMARY = ROOT / "tmp" / "reports" / "m262" / "M262-A002" / "arc_mode_handling_summary.json"
B003_SUMMARY = ROOT / "tmp" / "reports" / "m262" / "M262-B003" / "arc_interaction_semantics_summary.json"
C004_SUMMARY = ROOT / "tmp" / "reports" / "m262" / "M262-C004" / "arc_block_autorelease_return_lowering_summary.json"
D003_SUMMARY = ROOT / "tmp" / "reports" / "m262" / "M262-D003" / "arc_debug_instrumentation_summary.json"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m262" / "M262-E001" / "arc_runtime_gate_summary.json"


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
        SnippetCheck("M262-E001-EXP-01", "# M262 Runnable ARC Runtime Gate Contract And Architecture Freeze Expectations (E001)"),
        SnippetCheck("M262-E001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M262-E001-EXP-03", "tmp/reports/m262/M262-D003/arc_debug_instrumentation_summary.json"),
        SnippetCheck("M262-E001-EXP-04", "The gate must explicitly hand off to `M262-E002`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M262-E001-PKT-01", "# M262-E001 Runnable ARC Runtime Gate Contract And Architecture Freeze Packet"),
        SnippetCheck("M262-E001-PKT-02", "Packet: `M262-E001`"),
        SnippetCheck("M262-E001-PKT-03", "Issue: `#7206`"),
        SnippetCheck("M262-E001-PKT-04", "- `M262-C004`"),
        SnippetCheck("M262-E001-PKT-05", "- `M262-D003`"),
        SnippetCheck("M262-E001-PKT-06", "`M262-E002` is the first issue allowed"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M262-E001-SRC-01", "## M262 runnable ARC runtime gate (M262-E001)"),
        SnippetCheck("M262-E001-SRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M262-E001-SRC-03", "`!objc3.objc_runnable_arc_runtime_gate`"),
        SnippetCheck("M262-E001-SRC-04", "`M262-E002` is the next issue."),
    ),
    NATIVE_DOC: (
        SnippetCheck("M262-E001-NDOC-01", "## M262 runnable ARC runtime gate (M262-E001)"),
        SnippetCheck("M262-E001-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M262-E001-NDOC-03", "`!objc3.objc_runnable_arc_runtime_gate`"),
        SnippetCheck("M262-E001-NDOC-04", "`M262-E002` is the next issue."),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M262-E001-SPC-01", "## M262 runnable ARC runtime gate (E001)"),
        SnippetCheck("M262-E001-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M262-E001-SPC-03", f"`{EVIDENCE_MODEL}`"),
        SnippetCheck("M262-E001-SPC-04", "`!objc3.objc_runnable_arc_runtime_gate`"),
    ),
    SYNTAX_SPEC: (
        SnippetCheck("M262-E001-SYN-01", "`M262-E001` freezes the runnable ARC gate"),
        SnippetCheck("M262-E001-SYN-02", "`A002/B003/C004/D003` proof chain"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M262-E001-ARCH-01", "## M262 Runnable ARC Runtime Gate (E001)"),
        SnippetCheck("M262-E001-ARCH-02", "`A002/B003/C004/D003`"),
        SnippetCheck("M262-E001-ARCH-03", "`!objc3.objc_runnable_arc_runtime_gate`"),
        SnippetCheck("M262-E001-ARCH-04", "the next issue is `M262-E002`"),
    ),
    SEMA_CPP: (
        SnippetCheck("M262-E001-SEMA-01", "M262-E001 runnable-arc-runtime gate anchor"),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M262-E001-SEMAPM-01", "M262-E001 runnable-arc-runtime gate anchor"),
        SnippetCheck("M262-E001-SEMAPM-02", "parser-only or metadata-only"),
    ),
    SCAFFOLD_HEADER: (
        SnippetCheck("M262-E001-SCAF-01", "M262-E001 runnable-arc-runtime gate anchor"),
        SnippetCheck("M262-E001-SCAF-02", "already-proven ARC source, semantic, lowering, and runtime summaries"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M262-E001-LHDR-01", "kObjc3RunnableArcRuntimeGateContractId"),
        SnippetCheck("M262-E001-LHDR-02", "kObjc3RunnableArcRuntimeGateEvidenceModel"),
        SnippetCheck("M262-E001-LHDR-03", "std::string Objc3RunnableArcRuntimeGateSummary();"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M262-E001-LCPP-01", "std::string Objc3RunnableArcRuntimeGateSummary()"),
        SnippetCheck("M262-E001-LCPP-02", "M262-E001 runnable-arc-runtime gate anchor"),
        SnippetCheck("M262-E001-LCPP-03", "mode_contract="),
        SnippetCheck("M262-E001-LCPP-04", "runtime_contract="),
    ),
    IR_CPP: (
        SnippetCheck("M262-E001-IR-01", "M262-E001 runnable-arc-runtime gate anchor"),
        SnippetCheck("M262-E001-IR-02", "runnable_arc_runtime_gate = "),
        SnippetCheck("M262-E001-IR-03", "objc_runnable_arc_runtime_gate"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M262-E001-PKG-01", '"check:objc3c:m262-e001-runnable-arc-runtime-gate": "python scripts/check_m262_e001_runnable_arc_runtime_gate_contract_and_architecture_freeze.py"'),
        SnippetCheck("M262-E001-PKG-02", '"test:tooling:m262-e001-runnable-arc-runtime-gate": "python -m pytest tests/tooling/test_check_m262_e001_runnable_arc_runtime_gate_contract_and_architecture_freeze.py -q"'),
        SnippetCheck("M262-E001-PKG-03", '"check:objc3c:m262-e001-lane-e-readiness": "python scripts/run_m262_e001_lane_e_readiness.py"'),
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


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def run_process(command: Sequence[str], cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=ROOT if cwd is None else cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, int, list[Finding]]:
    failures: list[Finding] = []
    total = len(snippets) + 1
    if not path.exists():
        failures.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return total, 0, failures
    text = read_text(path)
    passed = 1
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return total, passed, failures


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def compile_fixture(fixture: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_process([str(NATIVE_EXE), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module"])


def boundary_line(ir_text: str) -> str:
    for line in ir_text.splitlines():
        if line.startswith(BOUNDARY_PREFIX + "contract="):
            return line
    return ""


def validate_upstream_summary(
    issue: str,
    path: Path,
    expected_contract: str,
    failures: list[Finding],
) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    total = 0
    passed = 0
    checks = (
        (f"{issue}-SUM-01", payload.get("contract_id") == expected_contract, f"expected contract_id {expected_contract!r}"),
        (f"{issue}-SUM-02", payload.get("checks_passed") == payload.get("checks_total"), "summary must report checks_passed == checks_total"),
        (f"{issue}-SUM-03", payload.get("checks_total", 0) > 0, "summary must report at least one check"),
    )
    ok_value = payload.get("ok")
    if ok_value is not None:
        checks += ((f"{issue}-SUM-04", ok_value is True, "summary must report ok=true when the field is present"),)
    for check_id, condition, detail in checks:
        total += 1
        passed += require(condition, artifact, check_id, detail, failures)
    return total, passed, payload


def ensure_native_build(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    summary_path = SUMMARY_OUT.parent / "ensure_objc3c_native_build_summary.json"
    completed = run_process(
        [
            sys.executable,
            str(BUILD_HELPER),
            "--mode",
            "fast",
            "--reason",
            "m262-e001-runnable-arc-runtime-gate",
            "--summary-out",
            str(summary_path),
        ]
    )
    payload: dict[str, Any] = {
        "returncode": completed.returncode,
        "summary_out": display_path(summary_path),
    }
    if summary_path.exists():
        payload["summary"] = load_json(summary_path)
    total = 1
    passed = require(completed.returncode == 0, display_path(BUILD_HELPER), "M262-E001-DYN-01", f"fast native build failed: {completed.stdout}{completed.stderr}", failures)
    return total, passed, payload


def check_compiled_fixture(
    case_id: str,
    fixture: Path,
    helper_snippet: str,
    failures: list[Finding],
) -> tuple[int, int, dict[str, Any]]:
    out_dir = PROBE_ROOT / fixture.stem
    completed = compile_fixture(fixture, out_dir)
    ir_path = out_dir / "module.ll"
    manifest_path = out_dir / "module.manifest.json"
    obj_path = out_dir / "module.obj"
    case: dict[str, Any] = {
        "fixture": display_path(fixture),
        "out_dir": display_path(out_dir),
        "compile_returncode": completed.returncode,
        "compile_stdout": completed.stdout,
        "compile_stderr": completed.stderr,
    }
    total = 4
    passed = 0
    passed += require(completed.returncode == 0, display_path(fixture), f"{case_id}-01", f"fixture compile failed: {completed.stdout}{completed.stderr}", failures)
    passed += require(ir_path.exists(), display_path(ir_path), f"{case_id}-02", "IR output missing", failures)
    passed += require(manifest_path.exists(), display_path(manifest_path), f"{case_id}-03", "manifest output missing", failures)
    passed += require(obj_path.exists(), display_path(obj_path), f"{case_id}-04", "object output missing", failures)
    if completed.returncode != 0 or not ir_path.exists() or not manifest_path.exists():
        return total, passed, case
    ir_text = read_text(ir_path)
    manifest = load_json(manifest_path)
    boundary = boundary_line(ir_text)
    case["boundary_line"] = boundary
    case["module_name"] = manifest.get("module")
    total += 4
    passed += require(boundary != "", display_path(ir_path), f"{case_id}-05", "IR missing runnable ARC gate boundary line", failures)
    passed += require(NAMED_METADATA_LINE in ir_text, display_path(ir_path), f"{case_id}-06", "IR missing runnable ARC gate named metadata", failures)
    passed += require(helper_snippet in ir_text, display_path(ir_path), f"{case_id}-07", f"IR missing expected ARC helper call {helper_snippet}", failures)
    passed += require(manifest.get("module") == fixture.stem, display_path(manifest_path), f"{case_id}-08", "manifest module name drifted", failures)
    return total, passed, case


def main(argv: Sequence[str] | None = None) -> int:
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    dynamic: dict[str, Any] = {}

    for path, snippets in STATIC_SNIPPETS.items():
        total, passed, static_failures = check_static_contract(path, snippets)
        checks_total += total
        checks_passed += passed
        failures.extend(static_failures)

    for issue, path, contract in (
        ("M262-A002", A002_SUMMARY, M262_A002_CONTRACT_ID),
        ("M262-B003", B003_SUMMARY, M262_B003_CONTRACT_ID),
        ("M262-C004", C004_SUMMARY, M262_C004_CONTRACT_ID),
        ("M262-D003", D003_SUMMARY, M262_D003_CONTRACT_ID),
    ):
        total, passed, payload = validate_upstream_summary(issue, path, contract, failures)
        checks_total += total
        checks_passed += passed
        dynamic[f"{issue.lower()}_summary"] = payload

    total, passed, build_case = ensure_native_build(failures)
    checks_total += total
    checks_passed += passed
    dynamic["build"] = build_case

    if passed == 1:
        total, passed, property_case = check_compiled_fixture(
            "M262-E001-PROP",
            PROPERTY_FIXTURE,
            "call void @objc3_runtime_store_weak_current_property_i32",
            failures,
        )
        checks_total += total
        checks_passed += passed
        dynamic["property_case"] = property_case

        total, passed, block_case = check_compiled_fixture(
            "M262-E001-BLOCK",
            BLOCK_FIXTURE,
            "call i32 @objc3_runtime_autorelease_i32",
            failures,
        )
        checks_total += total
        checks_passed += passed
        dynamic["block_case"] = block_case

    summary = {
        "ok": not failures,
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [finding.__dict__ for finding in failures],
        "evidence_model": EVIDENCE_MODEL,
        "active_gate_model": ACTIVE_GATE_MODEL,
        "non_goal_model": NON_GOAL_MODEL,
        "fail_closed_model": FAIL_CLOSED_MODEL,
        "next_issue": NEXT_CLOSEOUT_ISSUE,
        "dynamic": dynamic,
    }
    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_OUT.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.check_id} {finding.artifact}: {finding.detail}", file=sys.stderr)
        print(f"[fail] M262-E001 runnable ARC runtime gate check failed ({checks_passed}/{checks_total} checks passed)", file=sys.stderr)
        return 1

    print(f"[ok] M262-E001 runnable ARC runtime gate validated ({checks_passed}/{checks_total} checks)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

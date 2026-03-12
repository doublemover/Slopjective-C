#!/usr/bin/env python3
"""Validate M262-E002 runnable ARC closeout evidence."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m262-e002-runnable-arc-closeout-v1"
CONTRACT_ID = "objc3c-runnable-arc-closeout/m262-e002-v1"
MATRIX_MODEL = (
    "closeout-matrix-consumes-a002-b003-c004-d003-and-e001-evidence-without-widening-the-supported-runnable-arc-slice"
)
SMOKE_MODEL = (
    "integrated-arc-fixtures-and-private-property-runtime-probes-prove-supported-cleanup-block-and-property-behavior-through-native-toolchain-and-runtime"
)
FAILURE_MODEL = "fail-closed-on-runnable-arc-closeout-drift-or-runbook-mismatch"
NEXT_ISSUE = "M263-A001"
RUN_ID = "m262_e002_arc_closeout"

M262_A002_CONTRACT_ID = "objc3c-arc-mode-handling/m262-a002-v1"
M262_B003_CONTRACT_ID = "objc3c-arc-interaction-semantics/m262-b003-v1"
M262_C004_CONTRACT_ID = "objc3c-arc-block-autorelease-return-lowering/m262-c004-v1"
M262_D003_CONTRACT_ID = "objc3c-runtime-arc-debug-instrumentation/m262-d003-v1"
M262_E001_CONTRACT_ID = "objc3c-runnable-arc-runtime-gate/m262-e001-v1"

EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m262_runnable_arc_conformance_matrix_execution_smoke_and_operator_docs_cross_lane_integration_sync_e002_expectations.md"
)
RUNBOOK_DOC = ROOT / "docs" / "runbooks" / "m262_arc_runtime_closeout_runbook.md"
PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m262"
    / "m262_e002_runnable_arc_conformance_matrix_execution_smoke_and_operator_docs_cross_lane_integration_sync_packet.md"
)
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
SYNTAX_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
SCAFFOLD_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_ownership_aware_lowering_behavior_scaffold.h"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
SEMA_PASS_MANAGER_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
A002_SUMMARY = ROOT / "tmp" / "reports" / "m262" / "M262-A002" / "arc_mode_handling_summary.json"
B003_SUMMARY = ROOT / "tmp" / "reports" / "m262" / "M262-B003" / "arc_interaction_semantics_summary.json"
C004_SUMMARY = ROOT / "tmp" / "reports" / "m262" / "M262-C004" / "arc_block_autorelease_return_lowering_summary.json"
D003_SUMMARY = ROOT / "tmp" / "reports" / "m262" / "M262-D003" / "arc_debug_instrumentation_summary.json"
E001_SUMMARY = ROOT / "tmp" / "reports" / "m262" / "M262-E001" / "arc_runtime_gate_summary.json"
SMOKE_SUMMARY = (
    ROOT
    / "tmp"
    / "artifacts"
    / "objc3c-native"
    / "execution-smoke"
    / RUN_ID
    / "summary.json"
)
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m262" / "M262-E002" / "runnable_arc_closeout_summary.json"


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
        SnippetCheck("M262-E002-EXP-01", "# M262 Runnable ARC Conformance Matrix, Execution Smoke, And Operator Docs Cross-Lane Integration Sync Expectations (E002)"),
        SnippetCheck("M262-E002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M262-E002-EXP-03", "tmp/artifacts/objc3c-native/execution-smoke/m262_e002_arc_closeout/summary.json"),
        SnippetCheck("M262-E002-EXP-04", "The closeout must explicitly hand off to `M263-A001`."),
    ),
    RUNBOOK_DOC: (
        SnippetCheck("M262-E002-RBK-01", "# M262 ARC Runtime Closeout Runbook"),
        SnippetCheck("M262-E002-RBK-02", "python scripts/ensure_objc3c_native_build.py --mode full --reason m262-e002-runbook"),
        SnippetCheck("M262-E002-RBK-03", "$env:OBJC3C_NATIVE_EXECUTION_RUN_ID='m262_e002_arc_closeout'"),
        SnippetCheck("M262-E002-RBK-04", "The next implementation issue is `M263-A001`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M262-E002-PKT-01", "# M262-E002 Runnable ARC Conformance Matrix, Execution Smoke, And Operator Docs Cross-Lane Integration Sync Packet"),
        SnippetCheck("M262-E002-PKT-02", "Issue: `#7207`"),
        SnippetCheck("M262-E002-PKT-03", "Packet: `M262-E002`"),
        SnippetCheck("M262-E002-PKT-04", "tmp/artifacts/objc3c-native/execution-smoke/m262_e002_arc_closeout/summary.json"),
        SnippetCheck("M262-E002-PKT-05", "`M263-A001` is the next issue after `M262` closeout."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M262-E002-SRC-01", "## M262 runnable ARC closeout matrix and runbook (M262-E002)"),
        SnippetCheck("M262-E002-SRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M262-E002-SRC-03", "private `M262-D003` probe-backed row"),
        SnippetCheck("M262-E002-SRC-04", "`M263-A001`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M262-E002-NDOC-01", "## M262 runnable ARC closeout matrix and runbook (M262-E002)"),
        SnippetCheck("M262-E002-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M262-E002-NDOC-03", "three ARC-positive execution-smoke"),
        SnippetCheck("M262-E002-NDOC-04", "`M263-A001`"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M262-E002-SPC-01", "## M262 runnable ARC closeout matrix and runbook (E002)"),
        SnippetCheck("M262-E002-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M262-E002-SPC-03", f"`{MATRIX_MODEL}`"),
        SnippetCheck("M262-E002-SPC-04", f"`{SMOKE_MODEL}`"),
        SnippetCheck("M262-E002-SPC-05", f"`{FAILURE_MODEL}`"),
    ),
    SYNTAX_SPEC: (
        SnippetCheck("M262-E002-SYN-01", "`M262-E002` closes only the currently supported ARC slice"),
        SnippetCheck("M262-E002-SYN-02", "private `M262-D003` property/runtime probe row"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M262-E002-ARCH-01", "## M262 Runnable ARC Closeout Matrix And Runbook (E002)"),
        SnippetCheck("M262-E002-ARCH-02", "the dedicated ARC execution-smoke run `m262_e002_arc_closeout`"),
        SnippetCheck("M262-E002-ARCH-03", "the next issue is `M263-A001`"),
    ),
    SCAFFOLD_HEADER: (
        SnippetCheck("M262-E002-SCAF-01", "M262-E002 runnable-arc-closeout anchor"),
        SnippetCheck("M262-E002-SCAF-02", "integrated execution"),
    ),
    SEMA_CPP: (
        SnippetCheck("M262-E002-SEMA-01", "M262-E002 runnable-arc-closeout anchor"),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M262-E002-SEMAPM-01", "M262-E002 runnable-arc-closeout anchor"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M262-E002-LHDR-01", "kObjc3RunnableArcCloseoutContractId"),
        SnippetCheck("M262-E002-LHDR-02", "kObjc3RunnableArcCloseoutMatrixModel"),
        SnippetCheck("M262-E002-LHDR-03", "std::string Objc3RunnableArcCloseoutSummary();"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M262-E002-LCPP-01", "std::string Objc3RunnableArcCloseoutSummary()"),
        SnippetCheck("M262-E002-LCPP-02", "M262-E002 runnable-arc-closeout anchor"),
        SnippetCheck("M262-E002-LCPP-03", ";next_issue=M263-A001"),
    ),
    IR_CPP: (
        SnippetCheck("M262-E002-IR-01", "M262-E002 runnable-arc-closeout anchor"),
        SnippetCheck("M262-E002-IR-02", "; runnable_arc_closeout = "),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M262-E002-PKG-01", '"check:objc3c:m262-e002-runnable-arc-closeout"'),
        SnippetCheck("M262-E002-PKG-02", '"test:tooling:m262-e002-runnable-arc-closeout"'),
        SnippetCheck("M262-E002-PKG-03", '"check:objc3c:m262-e002-lane-e-readiness"'),
    ),
}

EXPECTED_SMOKE_ROWS = {
    "tests/tooling/fixtures/native/execution/positive/arc_cleanup_scope_positive.objc3": 9,
    "tests/tooling/fixtures/native/execution/positive/arc_implicit_cleanup_void_positive.objc3": 0,
    "tests/tooling/fixtures/native/execution/positive/arc_block_autorelease_return_positive.objc3": 9,
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


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    text = read_text(path)
    passed = 0
    artifact = display_path(path)
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(artifact, snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=EXPECTATIONS_DOC)
    parser.add_argument("--runbook-doc", type=Path, default=RUNBOOK_DOC)
    parser.add_argument("--packet-doc", type=Path, default=PACKET_DOC)
    parser.add_argument("--doc-source", type=Path, default=DOC_SOURCE)
    parser.add_argument("--native-doc", type=Path, default=NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=LOWERING_SPEC)
    parser.add_argument("--syntax-spec", type=Path, default=SYNTAX_SPEC)
    parser.add_argument("--architecture-doc", type=Path, default=ARCHITECTURE_DOC)
    parser.add_argument("--scaffold-header", type=Path, default=SCAFFOLD_HEADER)
    parser.add_argument("--sema-cpp", type=Path, default=SEMA_CPP)
    parser.add_argument("--sema-pass-manager-cpp", type=Path, default=SEMA_PASS_MANAGER_CPP)
    parser.add_argument("--lowering-header", type=Path, default=LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=LOWERING_CPP)
    parser.add_argument("--ir-cpp", type=Path, default=IR_CPP)
    parser.add_argument("--package-json", type=Path, default=PACKAGE_JSON)
    parser.add_argument("--a002-summary", type=Path, default=A002_SUMMARY)
    parser.add_argument("--b003-summary", type=Path, default=B003_SUMMARY)
    parser.add_argument("--c004-summary", type=Path, default=C004_SUMMARY)
    parser.add_argument("--d003-summary", type=Path, default=D003_SUMMARY)
    parser.add_argument("--e001-summary", type=Path, default=E001_SUMMARY)
    parser.add_argument("--smoke-summary", type=Path, default=SMOKE_SUMMARY)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def validate_summary(
    name: str,
    path: Path,
    expected_contract_id: str,
    failures: list[Finding],
) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    total = 0
    passed = 0
    total += 1
    passed += require(payload.get("contract_id") == expected_contract_id, artifact, f"{name}-01", "contract id drifted", failures)
    total += 1
    if "ok" in payload:
        passed += require(payload.get("ok") is True, artifact, f"{name}-02", "summary must report ok=true", failures)
    else:
        passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, f"{name}-02", "checks coverage drifted", failures)
    total += 1
    passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, f"{name}-03", "checks_passed must equal checks_total", failures)
    return total, passed, payload


def validate_smoke_summary(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any], list[dict[str, Any]]]:
    payload = load_json(path)
    artifact = display_path(path)
    total = 0
    passed = 0
    total += 1
    passed += require(payload.get("status") == "PASS", artifact, "M262-E002-SMOKE-01", "execution smoke must report PASS", failures)
    total += 1
    passed += require(str(payload.get("run_dir", "")).endswith(f"execution-smoke/{RUN_ID}"), artifact, "M262-E002-SMOKE-02", "run_dir must end with the dedicated closeout run id", failures)
    results = payload.get("results")
    total += 1
    passed += require(isinstance(results, list) and len(results) >= len(EXPECTED_SMOKE_ROWS), artifact, "M262-E002-SMOKE-03", "execution smoke must publish concrete result rows", failures)
    smoke_rows: list[dict[str, Any]] = []
    if not isinstance(results, list):
        return total, passed, payload, smoke_rows
    result_map = {str(item.get("fixture")): item for item in results if isinstance(item, dict)}
    for fixture, expected_exit in EXPECTED_SMOKE_ROWS.items():
        row = result_map.get(fixture)
        total += 6
        passed += require(row is not None, artifact, f"M262-E002-SMOKE-{fixture}-01", f"missing smoke row for {fixture}", failures)
        if row is None:
            continue
        passed += require(row.get("passed") is True, artifact, f"M262-E002-SMOKE-{fixture}-02", f"row for {fixture} must pass", failures)
        native_compile_args = row.get("native_compile_args")
        passed += require(isinstance(native_compile_args, list) and "-fobjc-arc" in native_compile_args, artifact, f"M262-E002-SMOKE-{fixture}-03", f"row for {fixture} must carry -fobjc-arc", failures)
        passed += require(row.get("compile_exit") == 0, artifact, f"M262-E002-SMOKE-{fixture}-04", f"compile must succeed for {fixture}", failures)
        passed += require(row.get("link_exit") == 0, artifact, f"M262-E002-SMOKE-{fixture}-05", f"link must succeed for {fixture}", failures)
        passed += require(row.get("run_exit") == expected_exit and row.get("expected_exit") == expected_exit, artifact, f"M262-E002-SMOKE-{fixture}-06", f"run exit mismatch for {fixture}", failures)
        smoke_rows.append({
            "row": Path(fixture).stem,
            "status": "ok",
            "fixture": fixture,
            "expected_exit": expected_exit,
            "out_dir": row.get("out_dir"),
        })
    return total, passed, payload, smoke_rows


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    file_checks: tuple[tuple[Path, Sequence[SnippetCheck]], ...] = (
        (args.expectations_doc, STATIC_SNIPPETS[EXPECTATIONS_DOC]),
        (args.runbook_doc, STATIC_SNIPPETS[RUNBOOK_DOC]),
        (args.packet_doc, STATIC_SNIPPETS[PACKET_DOC]),
        (args.doc_source, STATIC_SNIPPETS[DOC_SOURCE]),
        (args.native_doc, STATIC_SNIPPETS[NATIVE_DOC]),
        (args.lowering_spec, STATIC_SNIPPETS[LOWERING_SPEC]),
        (args.syntax_spec, STATIC_SNIPPETS[SYNTAX_SPEC]),
        (args.architecture_doc, STATIC_SNIPPETS[ARCHITECTURE_DOC]),
        (args.scaffold_header, STATIC_SNIPPETS[SCAFFOLD_HEADER]),
        (args.sema_cpp, STATIC_SNIPPETS[SEMA_CPP]),
        (args.sema_pass_manager_cpp, STATIC_SNIPPETS[SEMA_PASS_MANAGER_CPP]),
        (args.lowering_header, STATIC_SNIPPETS[LOWERING_HEADER]),
        (args.lowering_cpp, STATIC_SNIPPETS[LOWERING_CPP]),
        (args.ir_cpp, STATIC_SNIPPETS[IR_CPP]),
        (args.package_json, STATIC_SNIPPETS[PACKAGE_JSON]),
    )
    for path, snippets in file_checks:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    upstream: dict[str, Any] = {}
    for name, path, contract_id in (
        ("M262-A002", args.a002_summary, M262_A002_CONTRACT_ID),
        ("M262-B003", args.b003_summary, M262_B003_CONTRACT_ID),
        ("M262-C004", args.c004_summary, M262_C004_CONTRACT_ID),
        ("M262-D003", args.d003_summary, M262_D003_CONTRACT_ID),
        ("M262-E001", args.e001_summary, M262_E001_CONTRACT_ID),
    ):
        total, passed, payload = validate_summary(name, path, contract_id, failures)
        checks_total += total
        checks_passed += passed
        upstream[name] = payload

    smoke_total, smoke_passed, smoke_summary, smoke_rows = validate_smoke_summary(args.smoke_summary, failures)
    checks_total += smoke_total
    checks_passed += smoke_passed

    matrix_rows: list[dict[str, Any]] = [
        {"row": "A002-mode-handling", "status": "ok", "evidence": display_path(args.a002_summary)},
        {"row": "B003-arc-interaction-semantics", "status": "ok", "evidence": display_path(args.b003_summary)},
        {"row": "C004-block-autorelease-lowering", "status": "ok", "evidence": display_path(args.c004_summary)},
        {"row": "E001-runnable-arc-gate", "status": "ok", "evidence": display_path(args.e001_summary)},
    ]

    d003 = upstream["M262-D003"]
    artifact = "matrix"
    checks_total += 8
    checks_passed += require(d003.get("dynamic_probes_executed") is True, artifact, "M262-E002-MATRIX-01", "D003 must preserve live dynamic probes", failures)
    payload = d003.get("dynamic_payload", {}).get("runtime_probe", {}).get("payload", {})
    checks_passed += require(d003.get("dynamic_payload", {}).get("skipped") is False, artifact, "M262-E002-MATRIX-02", "D003 runtime payload must not be skipped", failures)
    checks_passed += require(d003.get("dynamic_payload", {}).get("runtime_probe", {}).get("compile_returncode") == 0, artifact, "M262-E002-MATRIX-03", "D003 runtime probe compile must succeed", failures)
    checks_passed += require(d003.get("dynamic_payload", {}).get("runtime_probe", {}).get("run_returncode") == 0, artifact, "M262-E002-MATRIX-04", "D003 runtime probe run must succeed", failures)
    checks_passed += require(payload.get("getter_value") == payload.get("child"), artifact, "M262-E002-MATRIX-05", "D003 getter must return the live property value", failures)
    checks_passed += require(payload.get("inside", {}).get("current_property_read_count", 0) >= 2, artifact, "M262-E002-MATRIX-06", "D003 must record current-property reads", failures)
    checks_passed += require(payload.get("inside", {}).get("current_property_write_count", 0) >= 1, artifact, "M262-E002-MATRIX-07", "D003 must record current-property writes", failures)
    checks_passed += require(payload.get("inside", {}).get("weak_current_property_store_count", 0) >= 1, artifact, "M262-E002-MATRIX-08", "D003 must record weak-property stores", failures)
    matrix_rows.append({
        "row": "D003-private-property-runtime-probe",
        "status": "ok",
        "evidence": display_path(args.d003_summary),
        "property_name": payload.get("inside", {}).get("last_property_name"),
    })
    matrix_rows.extend(smoke_rows)

    summary_payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "matrix_model": MATRIX_MODEL,
        "smoke_model": SMOKE_MODEL,
        "failure_model": FAILURE_MODEL,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "matrix_rows": matrix_rows,
        "upstream_summaries": {
            key: {"contract_id": value.get("contract_id")}
            for key, value in upstream.items()
        },
        "smoke_summary": {
            "path": display_path(args.smoke_summary),
            "status": smoke_summary.get("status"),
            "run_dir": smoke_summary.get("run_dir"),
        },
        "next_issue": NEXT_ISSUE,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in failures
        ],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary_payload), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.check_id} {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {CONTRACT_ID} passed {checks_passed}/{checks_total} checks", flush=True)
    print(f"[ok] summary: {display_path(args.summary_out)}", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

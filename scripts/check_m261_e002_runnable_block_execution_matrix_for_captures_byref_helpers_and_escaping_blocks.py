#!/usr/bin/env python3
"""Validate M261-E002 runnable block execution matrix closeout."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m261-e002-runnable-block-execution-matrix-closeout-v1"
CONTRACT_ID = "objc3c-runnable-block-execution-matrix/m261-e002-v1"
EVIDENCE_MODEL = (
    "a003-b003-c004-d003-e001-summary-plus-integrated-native-block-smoke-matrix"
)
ACTIVE_MODEL = (
    "closeout-matrix-runs-owned-nonowning-byref-and-escaping-block-fixtures-against-the-native-runtime"
)
FAILURE_MODEL = "fail-closed-on-runnable-block-execution-matrix-drift-or-doc-mismatch"
NEXT_ISSUE = "M262-A001"

A003_CONTRACT_ID = "objc3c-executable-block-source-storage-annotation/m261-a003-v1"
B003_CONTRACT_ID = "objc3c-executable-block-byref-copy-dispose-and-object-capture-ownership/m261-b003-v1"
C004_CONTRACT_ID = "objc3c-executable-block-escape-runtime-hook-lowering/m261-c004-v1"
D003_CONTRACT_ID = "objc3c-runtime-block-byref-forwarding-heap-promotion-interop/m261-d003-v1"
E001_CONTRACT_ID = "objc3c-runnable-block-runtime-gate/m261-e001-v1"

EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m261_runnable_block_execution_matrix_for_captures_byref_helpers_and_escaping_blocks_cross_lane_integration_sync_e002_expectations.md"
)
PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m261"
    / "m261_e002_runnable_block_execution_matrix_for_captures_byref_helpers_and_escaping_blocks_cross_lane_integration_sync_packet.md"
)
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
PART0_SPEC = ROOT / "spec" / "PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
SEMA_PM_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m261_e002_lane_e_readiness.py"
TEST_FILE = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m261_e002_runnable_block_execution_matrix_for_captures_byref_helpers_and_escaping_blocks.py"
)

A003_SUMMARY = ROOT / "tmp" / "reports" / "m261" / "M261-A003" / "block_source_storage_annotations_summary.json"
B003_SUMMARY = ROOT / "tmp" / "reports" / "m261" / "M261-B003" / "byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_summary.json"
C004_SUMMARY = ROOT / "tmp" / "reports" / "m261" / "M261-C004" / "escaping_block_runtime_hook_lowering_summary.json"
D003_SUMMARY = ROOT / "tmp" / "reports" / "m261" / "M261-D003" / "block_runtime_byref_forwarding_heap_promotion_ownership_interop_summary.json"
E001_SUMMARY = ROOT / "tmp" / "reports" / "m261" / "M261-E001" / "block_runtime_gate_summary.json"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m261" / "M261-E002" / "runnable_block_execution_matrix_summary.json"

NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m261" / "e002-runnable-block-execution-matrix"

OWNED_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_owned_object_capture_runtime_positive.objc3"
NONOWNING_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_nonowning_object_capture_runtime_positive.objc3"
BYREF_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_byref_cell_copy_dispose_runtime_positive.objc3"
ARGUMENT_ESCAPE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_escaping_block_runtime_hook_argument_positive.objc3"
RETURN_ESCAPE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_escaping_block_runtime_hook_return_positive.objc3"


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
        SnippetCheck("M261-E002-EXP-01", "# M261 Runnable Block Execution Matrix For Captures, Byref, Helpers, And Escaping Blocks Cross-Lane Integration Sync Expectations (E002)"),
        SnippetCheck("M261-E002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M261-E002-EXP-03", "Issue: `#7193`"),
        SnippetCheck("M261-E002-EXP-04", "`M262-A001`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M261-E002-PKT-01", "# M261-E002 Runnable Block Execution Matrix For Captures, Byref, Helpers, And Escaping Blocks Cross-Lane Integration Sync Packet"),
        SnippetCheck("M261-E002-PKT-02", "Issue: `#7193`"),
        SnippetCheck("M261-E002-PKT-03", "Packet: `M261-E002`"),
        SnippetCheck("M261-E002-PKT-04", "`M262-A001` is the explicit next issue after this closeout lands."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M261-E002-SRC-01", "## M261 runnable block execution matrix and docs (M261-E002)"),
        SnippetCheck("M261-E002-SRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-E002-SRC-03", "`m261_owned_object_capture_runtime_positive.objc3` with exit `11`"),
        SnippetCheck("M261-E002-SRC-04", "`M262-A001` is the next issue."),
    ),
    NATIVE_DOC: (
        SnippetCheck("M261-E002-NDOC-01", "## M261 runnable block execution matrix and docs (M261-E002)"),
        SnippetCheck("M261-E002-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-E002-NDOC-03", "`m261_escaping_block_runtime_hook_return_positive.objc3` with exit `0`"),
        SnippetCheck("M261-E002-NDOC-04", "`M262-A001` is the next issue."),
    ),
    PART0_SPEC: (
        SnippetCheck("M261-E002-P0-01", "(`M261-E002`)"),
        SnippetCheck("M261-E002-P0-02", "owned-capture, nonowning-capture, byref,"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M261-E002-SPC-01", "## M261 runnable block execution matrix and docs (E002)"),
        SnippetCheck("M261-E002-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-E002-SPC-03", "!objc3.objc_runnable_block_execution_matrix"),
        SnippetCheck("M261-E002-SPC-04", "`M262-A001` is the next issue."),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M261-E002-ARCH-01", "## M261 Runnable Block Execution Matrix And Docs (E002)"),
        SnippetCheck("M261-E002-ARCH-02", "real native fixture runs for:"),
        SnippetCheck("M261-E002-ARCH-03", "the next issue is `M262-A001`"),
    ),
    AST_HEADER: (
        SnippetCheck("M261-E002-AST-01", "kObjc3RunnableBlockExecutionMatrixContractId"),
        SnippetCheck("M261-E002-AST-02", "kObjc3RunnableBlockExecutionMatrixActiveModel"),
        SnippetCheck("M261-E002-AST-03", "kObjc3RunnableBlockExecutionMatrixFailClosedModel"),
    ),
    PARSER_CPP: (
        SnippetCheck("M261-E002-PARSE-01", "M261-E002 runnable-block execution-matrix anchor"),
        SnippetCheck("M261-E002-PARSE-02", "live executable block programs"),
    ),
    SEMA_PM_CPP: (
        SnippetCheck("M261-E002-SEMA-01", "M261-E002 runnable-block execution-matrix anchor"),
        SnippetCheck("M261-E002-SEMA-02", "integrated executable block fixtures"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M261-E002-LOWER-H-01", "kObjc3RunnableBlockExecutionMatrixContractId"),
        SnippetCheck("M261-E002-LOWER-H-02", "std::string Objc3RunnableBlockExecutionMatrixSummary();"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M261-E002-LOWER-CPP-01", "std::string Objc3RunnableBlockExecutionMatrixSummary()"),
        SnippetCheck("M261-E002-LOWER-CPP-02", ";next_issue=M262-A001"),
    ),
    IR_CPP: (
        SnippetCheck("M261-E002-IR-01", "M261-E002 runnable-block execution-matrix anchor"),
        SnippetCheck("M261-E002-IR-02", "runnable_block_execution_matrix = "),
        SnippetCheck("M261-E002-IR-03", "!objc3.objc_runnable_block_execution_matrix = !{!74}"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M261-E002-PKG-01", '"check:objc3c:m261-e002-runnable-block-execution-matrix": "python scripts/check_m261_e002_runnable_block_execution_matrix_for_captures_byref_helpers_and_escaping_blocks.py"'),
        SnippetCheck("M261-E002-PKG-02", '"test:tooling:m261-e002-runnable-block-execution-matrix": "python -m pytest tests/tooling/test_check_m261_e002_runnable_block_execution_matrix_for_captures_byref_helpers_and_escaping_blocks.py -q"'),
        SnippetCheck("M261-E002-PKG-03", '"check:objc3c:m261-e002-lane-e-readiness": "python scripts/run_m261_e002_lane_e_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M261-E002-RUN-01", "build_objc3c_native_docs.py"),
        SnippetCheck("M261-E002-RUN-02", "build:objc3c-native"),
        SnippetCheck("M261-E002-RUN-03", "check_m261_d003_byref_forwarding_cells_heap_promotion_and_ownership_interop_for_escaping_blocks_core_feature_expansion.py"),
        SnippetCheck("M261-E002-RUN-04", "check_m261_e001_runnable_block_runtime_gate_contract_and_architecture_freeze.py"),
        SnippetCheck("M261-E002-RUN-05", "check_m261_e002_runnable_block_execution_matrix_for_captures_byref_helpers_and_escaping_blocks.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M261-E002-TEST-01", "def test_m261_e002_checker_emits_summary() -> None:"),
        SnippetCheck("M261-E002-TEST-02", CONTRACT_ID),
    ),
}


CASE_SPECS = (
    {
        "case_id": "owned_capture_runtime",
        "fixture": OWNED_FIXTURE,
        "expected_exit": 11,
    },
    {
        "case_id": "nonowning_capture_runtime",
        "fixture": NONOWNING_FIXTURE,
        "expected_exit": 9,
    },
    {
        "case_id": "byref_helper_runtime",
        "fixture": BYREF_FIXTURE,
        "expected_exit": 14,
    },
    {
        "case_id": "escaping_argument_runtime",
        "fixture": ARGUMENT_ESCAPE_FIXTURE,
        "expected_exit": 14,
    },
    {
        "case_id": "escaping_return_runtime",
        "fixture": RETURN_ESCAPE_FIXTURE,
        "expected_exit": 0,
    },
)


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


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    findings: list[Finding] = []
    checks_total = len(snippets) + 1
    if not path.exists():
        findings.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return checks_total, findings
    text = read_text(path)
    for snippet in snippets:
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def resolve_clang() -> str:
    candidates = (
        ROOT / "tmp" / "llvm-build-21.1.8-ninja-dia" / "bin" / "clang.exe",
        Path(r"C:\Program Files\LLVM\bin\clang.exe"),
    )
    for candidate in candidates:
        if candidate.exists():
            return str(candidate)
    return "clang"


def compile_fixture(fixture: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_process([str(NATIVE_EXE), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module"])


def link_executable(out_dir: Path) -> tuple[Path | None, subprocess.CompletedProcess[str] | None]:
    obj_path = out_dir / "module.obj"
    rsp_path = out_dir / "module.runtime-metadata-linker-options.rsp"
    registration_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    exe_path = out_dir / "module.exe"
    if not obj_path.exists() or not rsp_path.exists() or not registration_manifest_path.exists():
        return None, None
    registration_manifest = load_json(registration_manifest_path)
    runtime_library_relative_path = registration_manifest.get("runtime_support_library_archive_relative_path")
    if not isinstance(runtime_library_relative_path, str) or not runtime_library_relative_path.strip():
        return None, None
    runtime_library_path = (ROOT / runtime_library_relative_path).resolve()
    if not runtime_library_path.exists():
        return None, None
    clang = resolve_clang()
    completed = run_process([
        clang,
        str(obj_path),
        f"@{rsp_path}",
        str(runtime_library_path),
        "-o",
        str(exe_path),
    ])
    return exe_path if exe_path.exists() else None, completed


def run_executable(exe_path: Path) -> subprocess.CompletedProcess[str]:
    return run_process([str(exe_path)], cwd=exe_path.parent)


def validate_summary(
    label: str,
    path: Path,
    expected_contract_id: str,
    failures: list[Finding],
) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    total = 0
    passed = 0
    total += 1
    passed += require(payload.get("contract_id") == expected_contract_id, artifact, f"{label}-01", "contract id drifted", failures)
    total += 1
    passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, f"{label}-02", "checks coverage drifted", failures)
    if "ok" in payload:
        total += 1
        passed += require(payload.get("ok") is True, artifact, f"{label}-03", "summary must report ok=true", failures)
    return total, passed, payload


def validate_case(case: dict[str, Any], failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    fixture: Path = case["fixture"]
    case_id = str(case["case_id"])
    expected_exit = int(case["expected_exit"])
    out_dir = PROBE_ROOT / case_id
    artifact = display_path(fixture)
    row: dict[str, Any] = {
        "case_id": case_id,
        "fixture": display_path(fixture),
        "expected_exit": expected_exit,
    }

    compile_result = compile_fixture(fixture, out_dir)
    row["compile_rc"] = compile_result.returncode
    checks_total += 1
    if compile_result.returncode == 0:
        checks_passed += 1
    else:
        failures.append(Finding(artifact, f"M261-E002-{case_id}-COMPILE", f"compile failed with rc={compile_result.returncode}: {compile_result.stdout}{compile_result.stderr}"))
        return checks_passed, checks_total, row

    ll_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"
    ir_text = read_text(ll_path) if ll_path.exists() else ""
    backend_text = read_text(backend_path).strip() if backend_path.exists() else ""

    for check_id, condition, detail in (
        (f"M261-E002-{case_id}-IR", ll_path.exists(), "missing emitted LLVM IR"),
        (f"M261-E002-{case_id}-OBJ", obj_path.exists() and obj_path.stat().st_size > 0, "missing emitted object"),
        (f"M261-E002-{case_id}-BACKEND", backend_text == "llvm-direct", f"expected llvm-direct backend, got {backend_text!r}"),
        (f"M261-E002-{case_id}-GATE", f"; runnable_block_runtime_gate = contract={E001_CONTRACT_ID}" in ir_text, "missing E001 gate summary in emitted IR"),
        (f"M261-E002-{case_id}-MATRIX", f"; runnable_block_execution_matrix = contract={CONTRACT_ID}" in ir_text, "missing E002 matrix summary in emitted IR"),
        (f"M261-E002-{case_id}-MD-GATE", "!objc3.objc_runnable_block_runtime_gate = !{!73}" in ir_text, "missing E001 named metadata in emitted IR"),
        (f"M261-E002-{case_id}-MD-MATRIX", "!objc3.objc_runnable_block_execution_matrix = !{!74}" in ir_text, "missing E002 named metadata in emitted IR"),
    ):
        checks_total += 1
        if condition:
            checks_passed += 1
        else:
            failures.append(Finding(display_path(out_dir), check_id, detail))

    exe_path, link_result = link_executable(out_dir)
    row["link_rc"] = None if link_result is None else link_result.returncode
    checks_total += 1
    if exe_path is not None and link_result is not None and link_result.returncode == 0:
        checks_passed += 1
    else:
        failures.append(Finding(display_path(out_dir), f"M261-E002-{case_id}-LINK", "failed to link integrated block executable"))
        return checks_passed, checks_total, row

    run_result = run_executable(exe_path)
    row["run_rc"] = run_result.returncode
    row["backend"] = backend_text
    checks_total += 1
    if run_result.returncode == expected_exit:
        checks_passed += 1
    else:
        failures.append(Finding(display_path(exe_path), f"M261-E002-{case_id}-RUN", f"expected exit {expected_exit}, got {run_result.returncode}"))
    return checks_passed, checks_total, row


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(sys.argv[1:] if argv is None else argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        total, static_failures = check_static_contract(path, snippets)
        checks_total += total
        checks_passed += total - len(static_failures)
        failures.extend(static_failures)

    upstream_summaries: dict[str, Any] = {}
    for label, path, contract_id in (
        ("M261-A003", A003_SUMMARY, A003_CONTRACT_ID),
        ("M261-B003", B003_SUMMARY, B003_CONTRACT_ID),
        ("M261-C004", C004_SUMMARY, C004_CONTRACT_ID),
        ("M261-D003", D003_SUMMARY, D003_CONTRACT_ID),
        ("M261-E001", E001_SUMMARY, E001_CONTRACT_ID),
    ):
        total, passed, payload = validate_summary(label, path, contract_id, failures)
        checks_total += total
        checks_passed += passed
        upstream_summaries[label] = {
            "path": display_path(path),
            "contract_id": payload.get("contract_id"),
        }
        if label == "M261-D003":
            checks_total += 4
            dynamic = payload.get("dynamic", {}) if isinstance(payload.get("dynamic"), dict) else {}
            probe = dynamic.get("probe_payload", {}) if isinstance(dynamic.get("probe_payload"), dict) else {}
            checks_passed += require(payload.get("dynamic_probes_executed") is True, display_path(path), "M261-E002-D003-DYN", "D003 must retain live runtime probe evidence", failures)
            checks_passed += require(probe.get("copy_count_after_promotion") == 1, display_path(path), "M261-E002-D003-COPY", "D003 probe must report copy_count_after_promotion=1", failures)
            checks_passed += require(probe.get("second_invoke_result") == 25, display_path(path), "M261-E002-D003-INVOKE", "D003 probe must report second_invoke_result=25", failures)
            checks_passed += require(probe.get("dispose_count_after_final_release") == 1, display_path(path), "M261-E002-D003-DISPOSE", "D003 probe must report dispose_count_after_final_release=1", failures)
        if label == "M261-E001":
            checks_total += 3
            dynamic = payload.get("dynamic", {}) if isinstance(payload.get("dynamic"), dict) else {}
            checks_passed += require(payload.get("next_closeout_issue") == "M261-E002", display_path(path), "M261-E002-E001-NEXT", "E001 must hand off to M261-E002", failures)
            checks_passed += require(payload.get("dynamic_probes_executed") is True, display_path(path), "M261-E002-E001-DYN", "E001 must retain live gate proof", failures)
            checks_passed += require(dynamic.get("compile_rc") == 0, display_path(path), "M261-E002-E001-COMPILE", "E001 gate compile probe must succeed", failures)

    matrix_rows: list[dict[str, Any]] = []
    if not args.skip_dynamic_probes:
        checks_total += 1
        checks_passed += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M261-E002-NATIVE-EXE", "native compiler executable is missing", failures)
        for case in CASE_SPECS:
            checks_total += 1
            checks_passed += require(case["fixture"].exists(), display_path(case["fixture"]), f"M261-E002-{case['case_id']}-FIXTURE", "required fixture is missing", failures)
        if not failures:
            for case in CASE_SPECS:
                passed, total, row = validate_case(case, failures)
                checks_passed += passed
                checks_total += total
                matrix_rows.append(row)
    else:
        matrix_rows = [
            {
                "case_id": str(case["case_id"]),
                "fixture": display_path(case["fixture"]),
                "expected_exit": int(case["expected_exit"]),
                "skipped": True,
            }
            for case in CASE_SPECS
        ]

    d003_payload = load_json(D003_SUMMARY).get("dynamic", {}).get("probe_payload", {})
    retained_d003_runtime_probe = {
        "copy_count_after_promotion": d003_payload.get("copy_count_after_promotion"),
        "first_invoke_result": d003_payload.get("first_invoke_result"),
        "second_invoke_result": d003_payload.get("second_invoke_result"),
        "dispose_count_after_final_release": d003_payload.get("dispose_count_after_final_release"),
    }

    summary = {
        "ok": not failures,
        "mode": MODE,
        "issue": "M261-E002",
        "contract_id": CONTRACT_ID,
        "evidence_model": EVIDENCE_MODEL,
        "active_model": ACTIVE_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_issue": NEXT_ISSUE,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "upstream_summaries": upstream_summaries,
        "retained_d003_runtime_probe": retained_d003_runtime_probe,
        "matrix_rows": matrix_rows,
        "failures": [
            {
                "artifact": failure.artifact,
                "check_id": failure.check_id,
                "detail": failure.detail,
            }
            for failure in failures
        ],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        for failure in failures:
            print(f"[{failure.check_id}] {failure.artifact}: {failure.detail}", file=sys.stderr)
        return 1
    print(args.summary_out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

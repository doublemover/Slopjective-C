#!/usr/bin/env python3
"""Checker for M261-C002 executable block object/invoke-thunk lowering."""

from __future__ import annotations

import argparse
import json
import re
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m261-c002-executable-block-object-and-invoke-thunk-lowering-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-executable-block-object-and-invoke-thunk-lowering/m261-c002-v1"
BOUNDARY_CONTRACT_ID = "objc3c-executable-block-lowering-abi-artifact-boundary/m261-c001-v1"
ACTIVE_MODEL = (
    "native-lowering-emits-stack-block-objects-and-direct-local-invoke-thunks-for-readonly-scalar-captures"
)
DEFERRED_MODEL = (
    "byref-cells-copy-dispose-helpers-owned-object-captures-and-heap-promotion-stay-fail-closed-until-m261-c003"
)
EXECUTION_EVIDENCE_MODEL = (
    "native-compile-link-run-proves-local-block-invocation-through-emitted-block-storage-and-invoke-thunk"
)
NEXT_ISSUE = "M261-C003"
EXPECTED_EXIT = 15
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m261" / "M261-C002" / "executable_block_object_invoke_thunk_lowering_summary.json"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m261_executable_block_object_and_invoke_thunk_lowering_core_feature_implementation_c002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m261" / "m261_c002_executable_block_object_and_invoke_thunk_lowering_core_feature_implementation_packet.md"
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
READINESS_RUNNER = ROOT / "scripts" / "run_m261_c002_lane_c_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m261_c002_executable_block_object_and_invoke_thunk_lowering_core_feature_implementation.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_executable_block_object_invoke_thunk_positive.objc3"
DEFERRED_BYREF_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_capture_legality_escape_invocation_positive.objc3"
DEFERRED_HELPER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_owned_object_capture_helper_positive.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m261" / "c002-executable-block-object-invoke-thunk"


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
        SnippetCheck("M261-C002-EXP-01", "# M261 Executable Block Object And Invoke-Thunk Lowering Core Feature Implementation Expectations (C002)"),
        SnippetCheck("M261-C002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M261-C002-EXP-03", "`15`"),
        SnippetCheck("M261-C002-EXP-04", "`M261-C003`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M261-C002-PKT-01", "# M261-C002 Executable Block Object And Invoke-Thunk Lowering Core Feature Implementation Packet"),
        SnippetCheck("M261-C002-PKT-02", "Issue: `#7186`"),
        SnippetCheck("M261-C002-PKT-03", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M261-C002-PKT-04", "`M261-C003` is the explicit next issue after this implementation lands."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M261-C002-SRC-01", "## M261 executable block object and invoke-thunk lowering (M261-C002)"),
        SnippetCheck("M261-C002-SRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-C002-SRC-03", "exit `15`"),
        SnippetCheck("M261-C002-SRC-04", "those cases still fail closed with `O3S221`."),
    ),
    NATIVE_DOC: (
        SnippetCheck("M261-C002-NDOC-01", "## M261 executable block object and invoke-thunk lowering (M261-C002)"),
        SnippetCheck("M261-C002-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-C002-NDOC-03", "`M261-C003` is the next issue."),
    ),
    PART0_SPEC: (
        SnippetCheck("M261-C002-P0-01", "(`M261-C002`): native lowering emits"),
        SnippetCheck("M261-C002-P0-02", "until `M261-C003`."),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M261-C002-SPC-01", "## M261 executable block object and invoke-thunk lowering (C002)"),
        SnippetCheck("M261-C002-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-C002-SPC-03", "`15`"),
        SnippetCheck("M261-C002-SPC-04", "those cases still fail closed with `O3S221`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M261-C002-ARCH-01", "## M261 Executable Block Object And Invoke-Thunk Lowering (C002)"),
        SnippetCheck("M261-C002-ARCH-02", "readonly scalar captures only"),
        SnippetCheck("M261-C002-ARCH-03", "the next issue is `M261-C003`"),
    ),
    PARSER_CPP: (
        SnippetCheck("M261-C002-PARSE-01", "M261-C002 executable-block-object/invoke-thunk anchor"),
        SnippetCheck("M261-C002-PARSE-02", "emits one stack-resident block object plus one internal invoke thunk"),
    ),
    AST_HEADER: (
        SnippetCheck("M261-C002-AST-01", "kObjc3ExecutableBlockObjectInvokeThunkLoweringContractId"),
        SnippetCheck("M261-C002-AST-02", "kObjc3ExecutableBlockObjectInvokeThunkDeferredModel"),
        SnippetCheck("M261-C002-AST-03", "kObjc3ExecutableBlockObjectInvokeThunkExecutionEvidenceModel"),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M261-C002-SEMA-PM-01", "M261-C002 executable-block-object/invoke-thunk anchor"),
        SnippetCheck("M261-C002-SEMA-PM-02", "allows readonly scalar captures without forcing the old"),
    ),
    SEMA_CPP: (
        SnippetCheck("M261-C002-SEMA-01", "M261-C002 executable-block-object/invoke-thunk anchor"),
        SnippetCheck("M261-C002-SEMA-02", "BlockLiteralUsesRunnableC002Subset"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M261-C002-LOWER-H-01", "kObjc3ExecutableBlockObjectInvokeThunkLoweringLaneContract"),
        SnippetCheck("M261-C002-LOWER-H-02", "std::string Objc3ExecutableBlockObjectInvokeThunkLoweringSummary();"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M261-C002-LOWER-CPP-01", "M261-C002 executable-block-object/invoke-thunk implementation anchor"),
        SnippetCheck("M261-C002-LOWER-CPP-02", "std::string Objc3ExecutableBlockObjectInvokeThunkLoweringSummary()"),
        SnippetCheck("M261-C002-LOWER-CPP-03", "execution_evidence_model="),
    ),
    IR_CPP: (
        SnippetCheck("M261-C002-IR-01", "M261-C002 executable-block-object/invoke-thunk anchor"),
        SnippetCheck("M261-C002-IR-02", "; executable_block_object_invoke_thunk_lowering = "),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M261-C002-PKG-01", '"check:objc3c:m261-c002-executable-block-object-invoke-thunk-lowering": "python scripts/check_m261_c002_executable_block_object_and_invoke_thunk_lowering_core_feature_implementation.py"'),
        SnippetCheck("M261-C002-PKG-02", '"test:tooling:m261-c002-executable-block-object-invoke-thunk-lowering": "python -m pytest tests/tooling/test_check_m261_c002_executable_block_object_and_invoke_thunk_lowering_core_feature_implementation.py -q"'),
        SnippetCheck("M261-C002-PKG-03", '"check:objc3c:m261-c002-lane-c-readiness": "python scripts/run_m261_c002_lane_c_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M261-C002-RUN-01", "scripts/build_objc3c_native_docs.py"),
        SnippetCheck("M261-C002-RUN-02", "build:objc3c-native"),
        SnippetCheck("M261-C002-RUN-03", "check_m261_c001_block_lowering_abi_and_artifact_boundary_contract_and_architecture_freeze.py"),
        SnippetCheck("M261-C002-RUN-04", "check_m261_c002_executable_block_object_and_invoke_thunk_lowering_core_feature_implementation.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M261-C002-TEST-01", "def test_m261_c002_checker_emits_summary() -> None:"),
        SnippetCheck("M261-C002-TEST-02", CONTRACT_ID),
    ),
    POSITIVE_FIXTURE: (
        SnippetCheck("M261-C002-FIX-01", "module m261_executable_block_object_invoke_thunk_positive;"),
        SnippetCheck("M261-C002-FIX-02", "let closure = ^(i32 value) {"),
        SnippetCheck("M261-C002-FIX-03", "return closure(5);"),
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


def load_json(path: Path) -> Any:
    return json.loads(read_text(path))


def diagnostics_codes(path: Path) -> list[str]:
    payload = load_json(path)
    diagnostics = payload.get("diagnostics", [])
    if not isinstance(diagnostics, list):
        return []
    return [str(diag.get("code", "")) for diag in diagnostics if isinstance(diag, dict)]


def semantic_surface(manifest: dict[str, Any], key: str) -> dict[str, Any]:
    frontend = manifest.get("frontend", {})
    pipeline = frontend.get("pipeline", {})
    surfaces = pipeline.get("semantic_surface", {})
    surface = surfaces.get(key)
    if not isinstance(surface, dict):
        return {}
    return surface


def resolve_clang() -> str:
    candidates = (
        shutil.which("clang"),
        shutil.which("clang.exe"),
        r"C:\Program Files\LLVM\bin\clang.exe",
    )
    for candidate in candidates:
        if candidate and Path(candidate).exists():
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
    result = run_process(
        [
            resolve_clang(),
            str(obj_path),
            str(runtime_library_path),
            f"@{rsp_path.resolve()}",
            "-o",
            str(exe_path),
        ],
        cwd=out_dir,
    )
    if result.returncode != 0 or not exe_path.exists():
        return None, result
    return exe_path, result


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def first_regex(text: str, pattern: str) -> str | None:
    match = re.search(pattern, text, re.MULTILINE)
    return match.group(1) if match else None


def run_positive_probe(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    out_dir = PROBE_ROOT / "positive"
    result = compile_fixture(POSITIVE_FIXTURE, out_dir)
    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"
    registration_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    rsp_path = out_dir / "module.runtime-metadata-linker-options.rsp"
    diagnostics_path = out_dir / "module.diagnostics.json"
    exe_path, link_result = link_executable(out_dir)
    run_result = run_process([str(exe_path)]) if exe_path is not None else None

    case: dict[str, Any] = {
        "fixture": display_path(POSITIVE_FIXTURE),
        "out_dir": display_path(out_dir),
        "compile_returncode": result.returncode,
        "manifest_exists": manifest_path.exists(),
        "ir_exists": ir_path.exists(),
        "object_exists": obj_path.exists(),
        "backend_exists": backend_path.exists(),
        "registration_manifest_exists": registration_manifest_path.exists(),
        "linker_rsp_exists": rsp_path.exists(),
        "diagnostics_exists": diagnostics_path.exists(),
        "executable_path": display_path(exe_path) if exe_path is not None else None,
    }
    if link_result is not None:
        case["link_returncode"] = link_result.returncode
        case["link_stdout"] = link_result.stdout
        case["link_stderr"] = link_result.stderr
    if run_result is not None:
        case["run_returncode"] = run_result.returncode
        case["run_stdout"] = run_result.stdout
        case["run_stderr"] = run_result.stderr

    checks_total = 0
    checks_passed = 0
    for check_id, condition, artifact, detail in (
        ("M261-C002-POS-01", result.returncode == 0, display_path(out_dir), "positive native compile failed"),
        ("M261-C002-POS-02", manifest_path.exists(), display_path(manifest_path), "manifest missing"),
        ("M261-C002-POS-03", ir_path.exists(), display_path(ir_path), "IR missing"),
        ("M261-C002-POS-04", obj_path.exists(), display_path(obj_path), "object missing"),
        ("M261-C002-POS-05", backend_path.exists(), display_path(backend_path), "backend marker missing"),
        ("M261-C002-POS-06", registration_manifest_path.exists(), display_path(registration_manifest_path), "registration manifest missing"),
        ("M261-C002-POS-07", rsp_path.exists(), display_path(rsp_path), "linker response file missing"),
        ("M261-C002-POS-08", diagnostics_path.exists(), display_path(diagnostics_path), "diagnostics json missing"),
        ("M261-C002-POS-09", exe_path is not None, display_path(out_dir), "expected linked executable artifact"),
        ("M261-C002-POS-10", run_result is not None, display_path(out_dir), "expected runnable executable result"),
    ):
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)
    if failures and any(f.check_id.startswith("M261-C002-POS") for f in failures):
        return checks_total, checks_passed, case

    manifest = load_json(manifest_path)
    diagnostics_codes_payload = diagnostics_codes(diagnostics_path)
    ir_text = read_text(ir_path)
    backend_text = read_text(backend_path).strip()
    lowering_capture = semantic_surface(manifest, "objc_block_literal_capture_lowering_surface")
    lowering_invoke = semantic_surface(manifest, "objc_block_abi_invoke_trampoline_lowering_surface")
    lowering_storage = semantic_surface(manifest, "objc_block_storage_escape_lowering_surface")
    lowering_copy_dispose = semantic_surface(manifest, "objc_block_copy_dispose_lowering_surface")
    invoke_symbol = first_regex(ir_text, r"define internal i32 @([^\(\s]+)\(ptr %block")
    alloca_line = first_regex(ir_text, r"(%block\.literal\.addr\.\d+ = alloca \{ ptr, \[2 x i32\] \}, align 8)")
    summary_line = next((line for line in ir_text.splitlines() if line.startswith("; executable_block_object_invoke_thunk_lowering = ")), "")
    boundary_line = next((line for line in ir_text.splitlines() if line.startswith("; executable_block_lowering_abi_artifact_boundary = ")), "")

    case.update(
        {
            "backend": backend_text,
            "diagnostic_codes": diagnostics_codes_payload,
            "invoke_symbol": invoke_symbol,
            "stack_alloca_line": alloca_line,
            "summary_line": summary_line,
            "boundary_line": boundary_line,
            "capture_surface": lowering_capture,
            "invoke_surface": lowering_invoke,
            "storage_surface": lowering_storage,
            "copy_dispose_surface": lowering_copy_dispose,
        }
    )

    for check_id, condition, artifact, detail in (
        ("M261-C002-POS-11", backend_text == "llvm-direct", display_path(backend_path), f"expected llvm-direct backend, got {backend_text!r}"),
        ("M261-C002-POS-12", diagnostics_codes_payload == [], display_path(diagnostics_path), f"positive diagnostics must stay empty, observed {diagnostics_codes_payload}"),
        ("M261-C002-POS-13", invoke_symbol is not None, display_path(ir_path), "failed to find emitted block invoke symbol definition"),
        ("M261-C002-POS-14", bool(summary_line), display_path(ir_path), "missing executable block object/invoke-thunk summary line"),
        ("M261-C002-POS-15", f"contract={CONTRACT_ID}" in summary_line, display_path(ir_path), "summary line missing C002 contract id"),
        ("M261-C002-POS-16", f"boundary_contract={BOUNDARY_CONTRACT_ID}" in summary_line, display_path(ir_path), "summary line missing C001 boundary contract id"),
        ("M261-C002-POS-17", f"active_model={ACTIVE_MODEL}" in summary_line, display_path(ir_path), "summary line missing active model"),
        ("M261-C002-POS-18", f"deferred_model={DEFERRED_MODEL}" in summary_line, display_path(ir_path), "summary line missing deferred model"),
        ("M261-C002-POS-19", f"execution_evidence_model={EXECUTION_EVIDENCE_MODEL}" in summary_line, display_path(ir_path), "summary line missing execution evidence model"),
        ("M261-C002-POS-20", bool(boundary_line), display_path(ir_path), "missing C001 boundary summary line"),
        ("M261-C002-POS-21", alloca_line is not None, display_path(ir_path), "missing stack block alloca for readonly scalar capture layout"),
        ("M261-C002-POS-22", invoke_symbol is not None and f"store ptr @{invoke_symbol}" in ir_text, display_path(ir_path), "missing stored block invoke pointer"),
        ("M261-C002-POS-23", "= call i32 %" in ir_text and "(ptr %block.literal.addr." in ir_text, display_path(ir_path), "missing indirect invoke call through block storage"),
        ("M261-C002-POS-24", lowering_capture.get("block_literal_sites") == 1, display_path(manifest_path), "unexpected capture surface literal count"),
        ("M261-C002-POS-25", lowering_invoke.get("invoke_trampoline_symbolized_sites") == 1, display_path(manifest_path), "invoke surface must report one symbolized thunk site"),
        ("M261-C002-POS-26", lowering_storage.get("byref_slot_count_total") == 0, display_path(manifest_path), "storage surface must stay at zero byref slots for the runnable slice"),
        ("M261-C002-POS-27", lowering_copy_dispose.get("copy_helper_required_sites") == 0, display_path(manifest_path), "copy/dispose helper requirement must stay zero for the runnable slice"),
        ("M261-C002-POS-28", lowering_capture.get("deterministic_handoff") is True, display_path(manifest_path), "capture surface must stay deterministic"),
        ("M261-C002-POS-29", lowering_invoke.get("deterministic_handoff") is True, display_path(manifest_path), "invoke surface must stay deterministic"),
        ("M261-C002-POS-30", lowering_storage.get("deterministic_handoff") is True, display_path(manifest_path), "storage surface must stay deterministic for readonly captures"),
        ("M261-C002-POS-31", lowering_copy_dispose.get("deterministic_handoff") is True, display_path(manifest_path), "copy/dispose surface must stay deterministic for readonly captures"),
        ("M261-C002-POS-32", run_result is not None and run_result.returncode == EXPECTED_EXIT, display_path(exe_path if exe_path is not None else out_dir), f"expected run exit {EXPECTED_EXIT}"),
    ):
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)

    return checks_total, checks_passed, case


def run_deferred_probe(fixture: Path, case_id: str, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    out_dir = PROBE_ROOT / case_id
    result = compile_fixture(fixture, out_dir)
    diagnostics_path = out_dir / "module.diagnostics.json"
    object_path = out_dir / "module.obj"
    exe_path = out_dir / "module.exe"
    case = {
        "fixture": display_path(fixture),
        "out_dir": display_path(out_dir),
        "compile_returncode": result.returncode,
        "diagnostics_exists": diagnostics_path.exists(),
        "object_exists": object_path.exists(),
        "executable_exists": exe_path.exists(),
    }
    diag_codes: list[str] = []
    if diagnostics_path.exists():
        diag_codes = diagnostics_codes(diagnostics_path)
        case["diagnostic_codes"] = diag_codes

    checks_total = 0
    checks_passed = 0
    for check_id, condition, artifact, detail in (
        (f"M261-C002-{case_id.upper()}-01", result.returncode != 0, display_path(out_dir), "deferred case must fail closed in native mode"),
        (f"M261-C002-{case_id.upper()}-02", diagnostics_path.exists(), display_path(diagnostics_path), "deferred case diagnostics are missing"),
        (f"M261-C002-{case_id.upper()}-03", "O3S221" in diag_codes, display_path(diagnostics_path), f"expected O3S221 for deferred case, observed {diag_codes}"),
        (f"M261-C002-{case_id.upper()}-04", not object_path.exists(), display_path(object_path), "deferred case must not emit an object artifact"),
        (f"M261-C002-{case_id.upper()}-05", not exe_path.exists(), display_path(exe_path), "deferred case must not emit an executable artifact"),
    ):
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)
    return checks_total, checks_passed, case


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(sys.argv[1:] if argv is None else argv)
    findings: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        total, local_findings = check_static_contract(path, snippets)
        checks_total += total
        checks_passed += total - len(local_findings)
        findings.extend(local_findings)

    dynamic_summary: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        pos_total, pos_passed, positive_case = run_positive_probe(findings)
        byref_total, byref_passed, deferred_byref_case = run_deferred_probe(
            DEFERRED_BYREF_FIXTURE, "deferred-byref", findings
        )
        helper_total, helper_passed, deferred_helper_case = run_deferred_probe(
            DEFERRED_HELPER_FIXTURE, "deferred-helper", findings
        )
        checks_total += pos_total + byref_total + helper_total
        checks_passed += pos_passed + byref_passed + helper_passed
        dynamic_summary = {
            "skipped": False,
            "positive_case": positive_case,
            "deferred_byref_case": deferred_byref_case,
            "deferred_helper_case": deferred_helper_case,
        }

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "boundary_contract_id": BOUNDARY_CONTRACT_ID,
        "active_model": ACTIVE_MODEL,
        "deferred_model": DEFERRED_MODEL,
        "execution_evidence_model": EXECUTION_EVIDENCE_MODEL,
        "next_issue": NEXT_ISSUE,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "ok": not findings,
        "findings": [finding.__dict__ for finding in findings],
        "dynamic_summary": dynamic_summary,
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if findings:
        for finding in findings:
            print(f"{finding.artifact}:{finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {CONTRACT_ID} ({checks_passed}/{checks_total} checks)")
    print(f"[summary] {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

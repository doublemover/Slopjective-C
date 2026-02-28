#!/usr/bin/env python3
"""Fail-closed contract checks for direct LLVM object emission matrix (M145)."""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m145-direct-llvm-matrix-contract-v1"

DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_CLI_OPTIONS_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.cpp"
DEFAULT_ROUTING_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_llvm_capability_routing.cpp"
DEFAULT_FRONTEND_ANCHOR_CPP = (
    ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
)
DEFAULT_FRONTEND_API_H = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "api.h"
DEFAULT_C_API_RUNNER_CPP = (
    ROOT / "native" / "objc3c" / "src" / "tools" / "objc3c_frontend_c_api_runner.cpp"
)
DEFAULT_SEMA_CONTRACT_PS1 = (
    ROOT / "scripts" / "check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1"
)
DEFAULT_SEMANTICS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
DEFAULT_ARTIFACTS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DEFAULT_TESTS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_DIRECT_LLVM_CONTRACT_DOC = ROOT / "docs" / "contracts" / "direct_llvm_emission_expectations.md"
DEFAULT_PERF_BUDGET_PS1 = ROOT / "scripts" / "check_objc3c_native_perf_budget.ps1"
DEFAULT_CONFORMANCE_SUITE_PS1 = ROOT / "scripts" / "check_conformance_suite.ps1"
DEFAULT_CONFORMANCE_COVERAGE_MAP = ROOT / "tests" / "conformance" / "COVERAGE_MAP.md"
DEFAULT_LOWERING_ABI_MANIFEST = ROOT / "tests" / "conformance" / "lowering_abi" / "manifest.json"
DEFAULT_LOWERING_ABI_README = ROOT / "tests" / "conformance" / "lowering_abi" / "README.md"
PRIMARY_M145_D001_FIXTURE = ROOT / "tests" / "conformance" / "lowering_abi" / "M145-D001.json"
FALLBACK_M145_D001_FIXTURE = DEFAULT_DIRECT_LLVM_CONTRACT_DOC
DEFAULT_M145_D001_FIXTURE = (
    PRIMARY_M145_D001_FIXTURE if PRIMARY_M145_D001_FIXTURE.exists() else FALLBACK_M145_D001_FIXTURE
)
HIDDEN_CLANG_FALLBACK_MARKER_PATTERN = re.compile(
    r"\b(hidden_fallback|fallback_status|clang_fallback|fallback_to_clang|fallback_clang)\b",
    re.IGNORECASE,
)


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--cli-options-cpp", type=Path, default=DEFAULT_CLI_OPTIONS_CPP)
    parser.add_argument("--routing-cpp", type=Path, default=DEFAULT_ROUTING_CPP)
    parser.add_argument("--frontend-anchor-cpp", type=Path, default=DEFAULT_FRONTEND_ANCHOR_CPP)
    parser.add_argument("--frontend-api-h", type=Path, default=DEFAULT_FRONTEND_API_H)
    parser.add_argument("--c-api-runner-cpp", type=Path, default=DEFAULT_C_API_RUNNER_CPP)
    parser.add_argument("--sema-contract-ps1", type=Path, default=DEFAULT_SEMA_CONTRACT_PS1)
    parser.add_argument("--semantics-fragment", type=Path, default=DEFAULT_SEMANTICS_FRAGMENT)
    parser.add_argument("--artifacts-fragment", type=Path, default=DEFAULT_ARTIFACTS_FRAGMENT)
    parser.add_argument("--tests-fragment", type=Path, default=DEFAULT_TESTS_FRAGMENT)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--direct-llvm-contract-doc", type=Path, default=DEFAULT_DIRECT_LLVM_CONTRACT_DOC)
    parser.add_argument("--perf-budget-ps1", type=Path, default=DEFAULT_PERF_BUDGET_PS1)
    parser.add_argument("--conformance-suite-ps1", type=Path, default=DEFAULT_CONFORMANCE_SUITE_PS1)
    parser.add_argument("--conformance-coverage-map", type=Path, default=DEFAULT_CONFORMANCE_COVERAGE_MAP)
    parser.add_argument("--lowering-abi-manifest", type=Path, default=DEFAULT_LOWERING_ABI_MANIFEST)
    parser.add_argument("--lowering-abi-readme", type=Path, default=DEFAULT_LOWERING_ABI_README)
    parser.add_argument("--m145-d001-fixture", type=Path, default=DEFAULT_M145_D001_FIXTURE)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path("tmp/m145_direct_llvm_matrix_contract_summary.json"),
    )
    return parser.parse_args(argv)


def require_file(path: Path, *, label: str) -> None:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{label} must be a file: {display_path(path)}")


def extract_function_body(text: str, signature: str) -> str:
    start = text.find(signature)
    if start == -1:
        raise ValueError(f"missing function signature: {signature}")
    brace_start = text.find("{", start)
    if brace_start == -1:
        raise ValueError(f"missing function body for: {signature}")

    depth = 0
    for idx in range(brace_start, len(text)):
        ch = text[idx]
        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0:
                return text[brace_start : idx + 1]
    raise ValueError(f"unterminated function body for: {signature}")


def has_hidden_clang_fallback_marker(text: str) -> bool:
    return HIDDEN_CLANG_FALLBACK_MARKER_PATTERN.search(text) is not None


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    require_file(args.process_cpp, label="process-cpp")
    require_file(args.driver_cpp, label="driver-cpp")
    require_file(args.cli_options_cpp, label="cli-options-cpp")
    require_file(args.routing_cpp, label="routing-cpp")
    require_file(args.frontend_anchor_cpp, label="frontend-anchor-cpp")
    require_file(args.frontend_api_h, label="frontend-api-h")
    require_file(args.c_api_runner_cpp, label="c-api-runner-cpp")
    require_file(args.sema_contract_ps1, label="sema-contract-ps1")
    require_file(args.semantics_fragment, label="semantics-fragment")
    require_file(args.artifacts_fragment, label="artifacts-fragment")
    require_file(args.tests_fragment, label="tests-fragment")
    require_file(args.package_json, label="package-json")
    require_file(args.direct_llvm_contract_doc, label="direct-llvm-contract-doc")
    require_file(args.perf_budget_ps1, label="perf-budget-ps1")
    require_file(args.conformance_suite_ps1, label="conformance-suite-ps1")
    require_file(args.conformance_coverage_map, label="conformance-coverage-map")
    require_file(args.lowering_abi_manifest, label="lowering-abi-manifest")
    require_file(args.lowering_abi_readme, label="lowering-abi-readme")
    require_file(args.m145_d001_fixture, label="m145-d001-fixture")

    process_cpp = args.process_cpp.read_text(encoding="utf-8")
    driver_cpp = args.driver_cpp.read_text(encoding="utf-8")
    cli_options_cpp = args.cli_options_cpp.read_text(encoding="utf-8")
    routing_cpp = args.routing_cpp.read_text(encoding="utf-8")
    frontend_anchor_cpp = args.frontend_anchor_cpp.read_text(encoding="utf-8")
    frontend_api_h = args.frontend_api_h.read_text(encoding="utf-8")
    c_api_runner_cpp = args.c_api_runner_cpp.read_text(encoding="utf-8")
    sema_contract_ps1 = args.sema_contract_ps1.read_text(encoding="utf-8")
    semantics_fragment = args.semantics_fragment.read_text(encoding="utf-8")
    artifacts_fragment = args.artifacts_fragment.read_text(encoding="utf-8")
    tests_fragment = args.tests_fragment.read_text(encoding="utf-8")
    package_payload = json.loads(args.package_json.read_text(encoding="utf-8"))
    direct_llvm_contract_doc = args.direct_llvm_contract_doc.read_text(encoding="utf-8")
    perf_budget_ps1 = args.perf_budget_ps1.read_text(encoding="utf-8")
    conformance_suite_ps1 = args.conformance_suite_ps1.read_text(encoding="utf-8")
    conformance_coverage_map = args.conformance_coverage_map.read_text(encoding="utf-8")
    lowering_abi_manifest = args.lowering_abi_manifest.read_text(encoding="utf-8")
    lowering_abi_readme = args.lowering_abi_readme.read_text(encoding="utf-8")
    m145_d001_fixture = args.m145_d001_fixture.read_text(encoding="utf-8")

    checks: list[tuple[str, bool, str]] = []

    # Process layer: llvm-direct path must be explicit and fail-closed.
    checks.append(
        (
            "process-m145-01",
            "RunProcess(llc_path.string(), {\"-filetype=obj\"" in process_cpp,
            "RunIRCompileLLVMDirect must invoke llc with -filetype=obj",
        )
    )
    checks.append(
        (
            "process-m145-02",
            "if (llc_status == 127)" in process_cpp,
            "RunIRCompileLLVMDirect must map missing llc to explicit status branch",
        )
    )
    checks.append(
        (
            "process-m145-03",
            "return 125;" in process_cpp,
            "RunIRCompileLLVMDirect must fail-closed with status 125 when backend unavailable",
        )
    )

    llvm_direct_body = extract_function_body(
        process_cpp,
        "int RunIRCompileLLVMDirect(",
    )
    checks.append(
        (
            "process-m145-04",
            "RunIRCompile(" not in llvm_direct_body,
            "RunIRCompileLLVMDirect must not fallback to clang RunIRCompile",
        )
    )
    checks.append(
        (
            "process-m145-05",
            not has_hidden_clang_fallback_marker(llvm_direct_body),
            "RunIRCompileLLVMDirect must not contain hidden clang fallback markers (hidden_fallback/fallback_status)",
        )
    )

    # Driver layer: backend routing must be explicit and no hidden fallback.
    checks.append(
        (
            "driver-m145-01",
            "if (cli_options.ir_object_backend == Objc3IrObjectBackend::kClang)" in driver_cpp,
            "driver must branch explicitly on selected object backend",
        )
    )
    checks.append(
        (
            "driver-m145-02",
            "compile_status = RunIRCompile(cli_options.clang_path, ir_out, object_out);" in driver_cpp,
            "driver clang backend branch must call RunIRCompile",
        )
    )
    checks.append(
        (
            "driver-m145-03",
            "compile_status = RunIRCompileLLVMDirect(cli_options.llc_path, ir_out, object_out, backend_error);"
            in driver_cpp,
            "driver llvm-direct backend branch must call RunIRCompileLLVMDirect",
        )
    )
    checks.append(
        (
            "driver-m145-04",
            not has_hidden_clang_fallback_marker(driver_cpp),
            "driver object-backend routing must not contain hidden clang fallback markers (hidden_fallback/fallback_status)",
        )
    )

    # CLI surface must continue exposing explicit backend selection and values.
    checks.append(
        (
            "cli-m145-01",
            "[--objc3-ir-object-backend <clang|llvm-direct>]" in cli_options_cpp,
            "usage text must advertise explicit clang|llvm-direct backend values",
        )
    )
    checks.append(
        (
            "cli-m145-02",
            "if (value == \"clang\")" in cli_options_cpp and "if (value == \"llvm-direct\")" in cli_options_cpp,
            "CLI parser must recognize both backend spellings",
        )
    )
    checks.append(
        (
            "cli-m145-03",
            re.search(r"invalid --objc3-ir-object-backend.*clang\|llvm-direct", cli_options_cpp)
            is not None,
            "CLI parser must fail with deterministic backend validation diagnostic",
        )
    )

    # Capability routing layer: explicit fail-closed behavior and no hidden object compile fallback.
    checks.append(
        (
            "routing-m145-01",
            "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary" in routing_cpp,
            "capability routing must fail-closed when routing is requested without a summary",
        )
    )
    checks.append(
        (
            "routing-m145-02",
            "llvm-direct backend selected but llc --filetype=obj capability is unavailable" in routing_cpp,
            "capability routing must fail-closed when llvm-direct is selected without llc object support",
        )
    )
    checks.append(
        (
            "routing-m145-03",
            "summary.llc_supports_filetype_obj ? Objc3IrObjectBackend::kLLVMDirect : Objc3IrObjectBackend::kClang"
            in routing_cpp,
            "capability routing must use explicit backend matrix selection (llvm-direct vs clang)",
        )
    )

    routing_body = extract_function_body(
        routing_cpp,
        "bool ApplyObjc3LLVMCabilityRouting(",
    )
    checks.append(
        (
            "routing-m145-04",
            "RunIRCompile(" not in routing_body and "RunIRCompileLLVMDirect(" not in routing_body,
            "capability routing must not invoke hidden object-compilation fallback paths",
        )
    )
    checks.append(
        (
            "routing-m145-05",
            not has_hidden_clang_fallback_marker(routing_body),
            "capability routing must not contain hidden clang fallback markers (hidden_fallback/fallback_status)",
        )
    )

    # Runtime ABI layer: frontend anchor emit-object matrix must remain explicit and fail-closed.
    checks.append(
        (
            "frontend-m145-01",
            "options->ir_object_backend == static_cast<uint8_t>(OBJC3C_FRONTEND_IR_OBJECT_BACKEND_CLANG)"
            in frontend_anchor_cpp
            and "options->ir_object_backend == static_cast<uint8_t>(OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT)"
            in frontend_anchor_cpp,
            "frontend runtime ABI must branch explicitly on clang|llvm-direct backend enum values",
        )
    )
    checks.append(
        (
            "frontend-m145-02",
            "emit_object requires a valid ir_object_backend (clang|llvm-direct)." in frontend_anchor_cpp
            and "emit_object requires valid ir_object_backend (clang|llvm-direct) [O3E001]" in frontend_anchor_cpp,
            "frontend runtime ABI must reject invalid emit_object backend with deterministic O3E001 diagnostics",
        )
    )
    checks.append(
        (
            "frontend-m145-03",
            "emit_object requires clang_path in compile options." in frontend_anchor_cpp
            and "emit_object requires clang_path in compile options [O3E001]" in frontend_anchor_cpp,
            "frontend runtime ABI must fail-closed on missing clang_path for clang backend",
        )
    )
    checks.append(
        (
            "frontend-m145-04",
            "emit_object requires llc_path in compile options for llvm-direct backend." in frontend_anchor_cpp
            and "emit_object requires llc_path in compile options for llvm-direct backend [O3E001]"
            in frontend_anchor_cpp,
            "frontend runtime ABI must fail-closed on missing llc_path for llvm-direct backend",
        )
    )
    checks.append(
        (
            "frontend-m145-05",
            "compile_status = RunIRCompile(std::filesystem::path(options->clang_path), ir_out, object_out);"
            in frontend_anchor_cpp,
            "frontend runtime ABI clang branch must call RunIRCompile",
        )
    )
    checks.append(
        (
            "frontend-m145-06",
            "compile_status = RunIRCompileLLVMDirect(std::filesystem::path(options->llc_path), ir_out, object_out, backend_error);"
            in frontend_anchor_cpp,
            "frontend runtime ABI llvm-direct branch must call RunIRCompileLLVMDirect",
        )
    )
    frontend_dispatch_block_match = re.search(
        r"if\s*\(wants_clang_backend\)\s*\{\s*compile_status\s*=\s*RunIRCompile\([^;]*;\s*\}\s*else\s*\{\s*compile_status\s*=\s*RunIRCompileLLVMDirect\(",
        frontend_anchor_cpp,
        re.S,
    )
    checks.append(
        (
            "frontend-m145-07",
            frontend_dispatch_block_match is not None,
            "frontend runtime ABI backend dispatch block must not include hidden fallback statements",
        )
    )
    checks.append(
        (
            "frontend-m145-08",
            "result->status = OBJC3C_FRONTEND_STATUS_EMIT_ERROR;" in frontend_anchor_cpp
            and "[O3E002]" in frontend_anchor_cpp,
            "frontend runtime ABI emit failures must map to OBJC3C_FRONTEND_STATUS_EMIT_ERROR with O3E002 diagnostics",
        )
    )
    checks.append(
        (
            "frontend-m145-09",
            "if (!backend_error.empty()) {" in frontend_anchor_cpp
            and "LLVM object emission failed: " in frontend_anchor_cpp,
            "frontend runtime ABI emit diagnostics must surface backend_error details when available",
        )
    )
    checks.append(
        (
            "frontend-m145-10",
            not has_hidden_clang_fallback_marker(frontend_anchor_cpp),
            "frontend runtime ABI emit path must not contain hidden clang fallback markers (hidden_fallback/fallback_status)",
        )
    )

    # Runtime ABI public header: backend/status surface must stay stable for embedding callers.
    checks.append(
        (
            "api-m145-01",
            "OBJC3C_FRONTEND_IR_OBJECT_BACKEND_CLANG = 0," in frontend_api_h
            and "OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT = 1" in frontend_api_h,
            "frontend API header must preserve deterministic clang|llvm-direct backend enum values",
        )
    )
    checks.append(
        (
            "api-m145-02",
            "const char *llc_path;" in frontend_api_h and "uint8_t ir_object_backend;" in frontend_api_h,
            "frontend API compile options must expose llc_path and ir_object_backend fields",
        )
    )
    checks.append(
        (
            "api-m145-03",
            "OBJC3C_FRONTEND_STATUS_EMIT_ERROR on object emission failures" in frontend_api_h,
            "frontend API contract docs must describe emit-error semantics for object emission failures",
        )
    )

    # Runtime ABI runner: C API path must preserve backend matrix parsing/wiring and fail-closed emit status mapping.
    checks.append(
        (
            "runner-m145-01",
            "--objc3-ir-object-backend <clang|llvm-direct>" in c_api_runner_cpp,
            "C API runner usage must expose explicit clang|llvm-direct backend matrix values",
        )
    )
    checks.append(
        (
            "runner-m145-02",
            "if (value == \"clang\") {" in c_api_runner_cpp and "if (value == \"llvm-direct\") {" in c_api_runner_cpp,
            "C API runner parser must recognize both clang and llvm-direct backend spellings",
        )
    )
    checks.append(
        (
            "runner-m145-03",
            "options.emit_object && options.ir_object_backend == OBJC3C_FRONTEND_IR_OBJECT_BACKEND_CLANG"
            in c_api_runner_cpp
            and "options.emit_object && options.ir_object_backend == OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT"
            in c_api_runner_cpp,
            "C API runner must gate clang_path/llc_path forwarding on selected backend",
        )
    )
    checks.append(
        (
            "runner-m145-04",
            "compile_options.ir_object_backend = options.ir_object_backend;" in c_api_runner_cpp,
            "C API runner must forward backend selection into compile options without hidden remap",
        )
    )
    checks.append(
        (
            "runner-m145-05",
            re.search(
                r"case\s+OBJC3C_FRONTEND_STATUS_EMIT_ERROR:\s*return\s+result\.process_exit_code\s*!=\s*0\s*\?\s*result\.process_exit_code\s*:\s*3;",
                c_api_runner_cpp,
                re.S,
            )
            is not None,
            "C API runner emit-error status must fail-closed with backend process exit code propagation",
        )
    )
    runner_backend_parser_body = extract_function_body(
        c_api_runner_cpp,
        "bool ParseIrObjectBackend(",
    )
    checks.append(
        (
            "runner-m145-06",
            not has_hidden_clang_fallback_marker(runner_backend_parser_body),
            "C API runner backend parser must not contain hidden clang fallback markers (hidden_fallback/fallback_status)",
        )
    )
    checks.append(
        (
            "lanec-m145-01",
            "int RunIRCompileLLVMDirect(" in process_cpp
            and "int RunObjc3LanguagePath(" in driver_cpp
            and "bool ApplyObjc3LLVMCabilityRouting(" in routing_cpp
            and "CompileObjc3SourceImpl(" in frontend_anchor_cpp
            and "bool ParseIrObjectBackend(" in c_api_runner_cpp,
            "lane-C checker anchors must cover process, driver, routing, runtime ABI, and C API runner entrypoints",
        )
    )

    # Lane-B sema/type-system matrix runtime contract must include direct-LLVM matrix + fail-closed paths.
    checks.append(
        (
            "laneb-m145-01",
            "positive_smoke_llvm_direct" in sema_contract_ps1
            and "--objc3-ir-object-backend\", \"llvm-direct\"" in sema_contract_ps1,
            "lane-B sema contract must execute explicit llvm-direct positive matrix replay",
        )
    )
    checks.append(
        (
            "laneb-m145-02",
            '-Id "runtime.positive.matrix.llvm_direct_forced_missing_llc.exit_codes"' in sema_contract_ps1
            and "missing-llc-executable.exe" in sema_contract_ps1
            and "--llc" in sema_contract_ps1,
            "lane-B sema contract must include forced missing-llc fail-closed replay case",
        )
    )
    checks.append(
        (
            "laneb-m145-03",
            "llvm-direct object emission failed: llc executable not found:" in sema_contract_ps1
            and "runtime.positive.matrix.llvm_direct_forced_missing_llc.fail_closed_marker" in sema_contract_ps1,
            "lane-B sema contract must assert deterministic llc-not-found fail-closed diagnostics",
        )
    )
    checks.append(
        (
            "laneb-m145-04",
            "runtime.positive.matrix.llvm_direct_forced_missing_llc.forbidden_artifact.absent.{0}" in sema_contract_ps1
            and "foreach ($forbiddenArtifact in @(\"module.obj\", \"module.object-backend.txt\"))"
            in sema_contract_ps1,
            "lane-B sema contract must fail-closed on object/backend artifact absence when llvm-direct emission fails",
        )
    )
    checks.append(
        (
            "laneb-m145-05",
            "runtime.negative.matrix.backend.exit_codes." in sema_contract_ps1
            and "runtime.negative.matrix.backend.diagnostics_txt.deterministic_sha256." in sema_contract_ps1,
            "lane-B sema contract must prove backend-invariant sema-failure diagnostics across clang/llvm-direct matrix runs",
        )
    )

    # Docs must publish lane-B matrix behavior, artifacts, and closeout commands.
    checks.append(
        (
            "laneb-doc-m145-01",
            "## Direct LLVM object-emission matrix lane-B contract (M145-B001)" in semantics_fragment
            and "forced missing-llc replay" in semantics_fragment,
            "semantics doc fragment must describe lane-B direct LLVM matrix contract and forced missing-llc fail-closed behavior",
        )
    )
    checks.append(
        (
            "laneb-doc-m145-02",
            "tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/positive_smoke_llvm_direct_forced_missing_llc/"
            in artifacts_fragment
            and "runtime.positive.matrix.llvm_direct_forced_missing_llc.exit_codes" in artifacts_fragment,
            "artifacts doc fragment must publish lane-B forced missing-llc matrix artifact root and check id",
        )
    )
    checks.append(
        (
            "laneb-doc-m145-03",
            "npm run test:objc3c:m145-direct-llvm-matrix" in tests_fragment
            and "npm run check:compiler-closeout:m145" in tests_fragment,
            "tests doc fragment must include lane-B M145 matrix command and closeout gate command",
        )
    )

    # Lane-D validation/conformance/perf contract must stay wired.
    checks.append(
        (
            "laned-m145-01",
            '$dispatchRequiredDir = "tests/tooling/fixtures/native/recovery/positive/lowering_dispatch"'
            in perf_budget_ps1
            and "cache-proof PASS fixture=" in perf_budget_ps1
            and "cache_hit=(true|false)" in perf_budget_ps1,
            "perf-budget contract must preserve dispatch fixture coverage and deterministic cache-proof markers",
        )
    )
    checks.append(
        (
            "laned-m145-02",
            'Require-Range "CRPT-" 1 6' in conformance_suite_ps1
            and 'Join-Path $RepoRoot "tests/conformance/$bucket"' in conformance_suite_ps1,
            "conformance suite contract must preserve bucket minima and required family checks",
        )
    )
    checks.append(
        (
            "laned-m145-03",
            "| `M145-D001` | `#4317`" in conformance_coverage_map
            and "`lowering_abi`" in conformance_coverage_map,
            "conformance coverage map must trace M145-D001 issue mapping into lowering_abi bucket",
        )
    )
    checks.append(
        (
            "laned-m145-04",
            '"name": "m145_lane_d_issue_4317_direct_llvm_object_emission_fail_closed_matrix"'
            in lowering_abi_manifest
            and '"M145-D001.json"' in lowering_abi_manifest,
            "lowering_abi manifest must register M145 lane-D direct LLVM matrix conformance fixture group",
        )
    )
    checks.append(
        (
            "laned-m145-05",
            "M145-D001.json" in lowering_abi_readme
            and "direct LLVM object-emission fail-closed matrix" in lowering_abi_readme,
            "lowering_abi README must document M145 lane-D conformance fixture scope",
        )
    )
    checks.append(
        (
            "laned-m145-06",
            '"id": "M145-D001"' in m145_d001_fixture
            and '"lane": "D"' in m145_d001_fixture
            and '"issue": 4317' in m145_d001_fixture
            and re.search(
                r"--objc3-ir-object-backend\s+llvm-direct(?:\s|\\n|\\r|$)",
                m145_d001_fixture,
            )
            is not None
            and "O3E001" in m145_d001_fixture
            and "O3E002" in m145_d001_fixture,
            "M145-D001 conformance fixture must pin llvm-direct fail-closed diagnostics and lane-D issue metadata",
        )
    )
    checks.append(
        (
            "laned-doc-m145-01",
            "## Direct LLVM object-emission matrix lane-D contract (M145-D001)" in semantics_fragment
            and "fixture, determinism, conformance, and perf coverage" in semantics_fragment,
            "semantics doc fragment must describe M145 lane-D validation/conformance/perf contract scope",
        )
    )
    checks.append(
        (
            "laned-doc-m145-02",
            "## Direct LLVM object-emission lane-D validation artifacts (M145-D001)" in artifacts_fragment
            and "tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json" in artifacts_fragment,
            "artifacts doc fragment must publish lane-D perf artifact summary root",
        )
    )
    checks.append(
        (
            "laned-doc-m145-03",
            "npm run test:objc3c:m145-direct-llvm-matrix:lane-d" in tests_fragment
            and (
                "scripts/check_conformance_suite.ps1" in tests_fragment
                or "powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_conformance_suite.ps1"
                in tests_fragment
            )
            and "npm run test:objc3c:perf-budget" in tests_fragment,
            "tests doc fragment must include lane-D validation command and conformance/perf anchors",
        )
    )

    scripts = package_payload.get("scripts", {})
    if not isinstance(scripts, dict):
        raise ValueError("package-json scripts field must be an object")
    closeout_script = scripts.get("check:compiler-closeout:m145", "")
    if not isinstance(closeout_script, str):
        closeout_script = ""
    lane_b_test_script = scripts.get("test:objc3c:m145-direct-llvm-matrix", "")
    if not isinstance(lane_b_test_script, str):
        lane_b_test_script = ""
    lane_d_test_script = scripts.get("test:objc3c:m145-direct-llvm-matrix:lane-d", "")
    if not isinstance(lane_d_test_script, str):
        lane_d_test_script = ""
    task_hygiene_script = scripts.get("check:task-hygiene", "")
    if not isinstance(task_hygiene_script, str):
        task_hygiene_script = ""

    checks.append(
        (
            "laneb-pkg-m145-01",
            "scripts/check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1" in lane_b_test_script
            and "python -m pytest" in lane_b_test_script
            and "tests/tooling/test_check_m145_direct_llvm_matrix_contract.py" in lane_b_test_script,
            "package script test:objc3c:m145-direct-llvm-matrix must run lane-B sema matrix script and M145 matrix contract tests",
        )
    )
    checks.append(
        (
            "laned-pkg-m145-01",
            "scripts/check_conformance_suite.ps1" in lane_d_test_script
            and "npm run test:objc3c:perf-budget" in lane_d_test_script,
            "package script test:objc3c:m145-direct-llvm-matrix:lane-d must run conformance suite and perf budget gates",
        )
    )
    checks.append(
        (
            "laned-pkg-m145-02",
            "python scripts/check_m145_direct_llvm_matrix_contract.py" in closeout_script
            and "npm run test:objc3c:m145-direct-llvm-matrix" in closeout_script
            and "npm run test:objc3c:m145-direct-llvm-matrix:lane-d" in closeout_script
            and '--glob "docs/contracts/direct_llvm_emission_expectations.md"' in closeout_script,
            "check:compiler-closeout:m145 must run matrix checker, lane-B/lane-D matrix test commands, and direct LLVM contract lint",
        )
    )
    checks.append(
        (
            "laned-pkg-m145-03",
            "check:compiler-closeout:m145" in task_hygiene_script,
            "check:task-hygiene must include check:compiler-closeout:m145",
        )
    )

    # Contract doc surface: M145 lane-C matrix coverage must stay documented.
    checks.append(
        (
            "doc-m145-01",
            "| `M145-LLVM-06` |" in direct_llvm_contract_doc,
            "direct LLVM contract doc must include M145 lane-C matrix expectation row",
        )
    )
    checks.append(
        (
            "doc-m145-02",
            "python scripts/check_m145_direct_llvm_matrix_contract.py" in direct_llvm_contract_doc,
            "direct LLVM contract doc must include the M145 matrix checker command",
        )
    )
    checks.append(
        (
            "doc-m145-03",
            "libobjc3c_frontend/frontend_anchor.cpp" in direct_llvm_contract_doc,
            "direct LLVM contract doc must reference runtime ABI frontend anchor coverage",
        )
    )
    checks.append(
        (
            "doc-m145-04",
            "| `M145-LLVM-07` |" in direct_llvm_contract_doc
            and "## M145 Matrix Extension (Lane D)" in direct_llvm_contract_doc,
            "direct LLVM contract doc must include M145 lane-D validation/conformance/perf expectation and section",
        )
    )

    failed = [check for check in checks if not check[1]]
    summary = {
        "mode": MODE,
        "process_cpp": display_path(args.process_cpp),
        "driver_cpp": display_path(args.driver_cpp),
        "cli_options_cpp": display_path(args.cli_options_cpp),
        "routing_cpp": display_path(args.routing_cpp),
        "frontend_anchor_cpp": display_path(args.frontend_anchor_cpp),
        "frontend_api_h": display_path(args.frontend_api_h),
        "c_api_runner_cpp": display_path(args.c_api_runner_cpp),
        "sema_contract_ps1": display_path(args.sema_contract_ps1),
        "semantics_fragment": display_path(args.semantics_fragment),
        "artifacts_fragment": display_path(args.artifacts_fragment),
        "tests_fragment": display_path(args.tests_fragment),
        "package_json": display_path(args.package_json),
        "direct_llvm_contract_doc": display_path(args.direct_llvm_contract_doc),
        "perf_budget_ps1": display_path(args.perf_budget_ps1),
        "conformance_suite_ps1": display_path(args.conformance_suite_ps1),
        "conformance_coverage_map": display_path(args.conformance_coverage_map),
        "lowering_abi_manifest": display_path(args.lowering_abi_manifest),
        "lowering_abi_readme": display_path(args.lowering_abi_readme),
        "m145_d001_fixture": display_path(args.m145_d001_fixture),
        "checks_passed": len(checks) - len(failed),
        "checks_total": len(checks),
        "failures": [{"id": check_id, "message": message} for check_id, _, message in failed],
        "ok": not failed,
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if failed:
        for check_id, _, message in failed:
            print(f"M145-DIRECT-LLVM-FAIL [{check_id}] {message}", file=sys.stderr)
        print(f"wrote summary: {display_path(args.summary_out)}", file=sys.stderr)
        return 1

    print("m145-direct-llvm-matrix-contract: OK")
    print(f"- checks_passed={summary['checks_passed']}")
    print(f"- fail_closed=true")
    print(f"- summary={display_path(args.summary_out)}")
    return 0


def main() -> None:
    raise SystemExit(run(sys.argv[1:]))


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""Validate M262-D001 runtime ARC helper API surface freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m262-d001-runtime-arc-helper-api-surface-freeze-v1"
CONTRACT_ID = "objc3c-runtime-arc-helper-api-surface-freeze/m262-d001-v1"
REFERENCE_MODEL = (
    "public-runtime-abi-stays-register-lookup-dispatch-while-arc-helper-entrypoints-remain-private-bootstrap-internal-runtime-abi"
)
WEAK_MODEL = (
    "weak-storage-and-current-property-access-remain-served-through-private-runtime-helper-entrypoints-and-runtime-side-tables"
)
AUTORELEASEPOOL_MODEL = (
    "autorelease-return-and-autoreleasepool-support-remain-private-runtime-helper-behavior-without-public-abi-widening"
)
FAIL_CLOSED_MODEL = (
    "no-public-runtime-arc-helper-api-no-user-facing-arc-runtime-header-widening-yet"
)
BOUNDARY_PREFIX = "; runtime_arc_helper_api_surface = "
NAMED_METADATA_PREFIX = "!objc3.objc_runtime_arc_helper_api_surface = !{!83}"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m262" / "M262-D001" / "runtime_arc_helper_api_surface_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m262_runtime_arc_helper_api_surface_contract_and_architecture_freeze_d001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m262" / "m262_d001_runtime_arc_helper_api_surface_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
SYNTAX_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
SEMA_PASS_MANAGER_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_compilation_driver.cpp"
RUNTIME_HEADER = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.h"
RUNTIME_INTERNAL_HEADER = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
PACKAGE_JSON = ROOT / "package.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_property_interaction_positive.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m262" / "d001"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
HELPER_SYMBOLS = (
    "objc3_runtime_retain_i32",
    "objc3_runtime_release_i32",
    "objc3_runtime_autorelease_i32",
    "objc3_runtime_read_current_property_i32",
    "objc3_runtime_write_current_property_i32",
    "objc3_runtime_exchange_current_property_i32",
    "objc3_runtime_load_weak_current_property_i32",
    "objc3_runtime_store_weak_current_property_i32",
    "objc3_runtime_push_autoreleasepool_scope",
    "objc3_runtime_pop_autoreleasepool_scope",
)


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
        SnippetCheck("M262-D001-EXP-01", "# M262 Runtime ARC Helper API Surface Contract And Architecture Freeze Expectations (D001)"),
        SnippetCheck("M262-D001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M262-D001-EXP-04", "`tmp/reports/m262/M262-D001/runtime_arc_helper_api_surface_contract_summary.json`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M262-D001-PKT-01", "# M262-D001 Runtime ARC Helper API Surface Contract And Architecture Freeze Packet"),
        SnippetCheck("M262-D001-PKT-02", "Issue: `#7203`"),
        SnippetCheck("M262-D001-PKT-03", "Packet: `M262-D001`"),
        SnippetCheck("M262-D001-PKT-04", "`M262-D002` is the explicit next issue after this freeze lands"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M262-D001-SRC-01", "## M262 runtime ARC helper API surface (M262-D001)"),
        SnippetCheck("M262-D001-SRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M262-D001-SRC-03", "`objc3_runtime_bootstrap_internal.h`"),
        SnippetCheck("M262-D001-SRC-04", "`M262-D002` is the next issue."),
    ),
    NATIVE_DOC: (
        SnippetCheck("M262-D001-NDOC-01", "## M262 runtime ARC helper API surface (M262-D001)"),
        SnippetCheck("M262-D001-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M262-D001-NDOC-03", "`objc3_runtime_push_autoreleasepool_scope` remains a private helper"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M262-D001-SPC-01", "## M262 runtime ARC helper API surface (D001)"),
        SnippetCheck("M262-D001-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M262-D001-SPC-03", f"`{REFERENCE_MODEL}`"),
        SnippetCheck("M262-D001-SPC-04", "`!objc3.objc_runtime_arc_helper_api_surface`"),
    ),
    SYNTAX_SPEC: (
        SnippetCheck("M262-D001-SYN-01", "## B.2.10 Runtime ARC helper API surface (implementation note)"),
        SnippetCheck("M262-D001-SYN-02", "private ARC helper entrypoints and autoreleasepool hooks remain internal runtime ABI"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M262-D001-ARCH-01", "## M262 Runtime ARC Helper API Surface (D001)"),
        SnippetCheck("M262-D001-ARCH-02", "private ARC helper entrypoints remain the canonical runtime-owned lane-D boundary"),
        SnippetCheck("M262-D001-ARCH-03", "the next issue is `M262-D002`"),
    ),
    PARSER_CPP: (
        SnippetCheck("M262-D001-PARSE-01", "M262-D001 runtime ARC helper API surface anchor"),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M262-D001-SEMA-01", "M262-D001 runtime ARC helper API surface anchor"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M262-D001-LHDR-01", "kObjc3RuntimeArcHelperApiSurfaceContractId"),
        SnippetCheck("M262-D001-LHDR-02", "Objc3RuntimeArcHelperApiSurfaceSummary()"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M262-D001-LCPP-01", "std::string Objc3RuntimeArcHelperApiSurfaceSummary()"),
        SnippetCheck("M262-D001-LCPP-02", "M262-D001 runtime ARC helper API surface anchor"),
    ),
    IR_EMITTER: (
        SnippetCheck("M262-D001-IR-01", 'out << "; runtime_arc_helper_api_surface = "'),
        SnippetCheck("M262-D001-IR-02", '!objc3.objc_runtime_arc_helper_api_surface = !{!83}'),
    ),
    DRIVER_CPP: (
        SnippetCheck("M262-D001-DRV-01", "M262-D001 runtime ARC helper API surface anchor"),
    ),
    RUNTIME_HEADER: (
        SnippetCheck("M262-D001-RH-01", "M262-D001 runtime-arc-helper-api-surface anchor"),
    ),
    RUNTIME_INTERNAL_HEADER: (
        SnippetCheck("M262-D001-RIH-01", "M262-D001 runtime-arc-helper-api-surface anchor"),
        SnippetCheck("M262-D001-RIH-02", "int objc3_runtime_push_autoreleasepool_scope(void);".replace("int ", "void ")),
    ),
    RUNTIME_CPP: (
        SnippetCheck("M262-D001-RCPP-01", "M262-D001 runtime ARC helper API surface anchor"),
        SnippetCheck("M262-D001-RCPP-02", "objc3_runtime_autorelease_i32"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M262-D001-PKG-01", '"check:objc3c:m262-d001-runtime-arc-helper-api-surface-contract": "python scripts/check_m262_d001_runtime_arc_helper_api_surface_contract_and_architecture_freeze.py"'),
        SnippetCheck("M262-D001-PKG-02", '"test:tooling:m262-d001-runtime-arc-helper-api-surface-contract": "python -m pytest tests/tooling/test_check_m262_d001_runtime_arc_helper_api_surface_contract_and_architecture_freeze.py -q"'),
        SnippetCheck("M262-D001-PKG-03", '"check:objc3c:m262-d001-lane-d-readiness": "python scripts/run_m262_d001_lane_d_readiness.py"'),
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


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def run_command(command: Sequence[str], cwd: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=cwd, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def validate_public_private_boundary(runtime_header_text: str, runtime_internal_text: str, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    for symbol in HELPER_SYMBOLS:
        checks_total += 1
        checks_passed += require(symbol not in runtime_header_text, display_path(RUNTIME_HEADER), f"M262-D001-PUB-{checks_total:02d}", f"public runtime header must not expose helper symbol {symbol}", failures)
    for symbol in HELPER_SYMBOLS:
        checks_total += 1
        checks_passed += require(symbol in runtime_internal_text, display_path(RUNTIME_INTERNAL_HEADER), f"M262-D001-PRI-{checks_total:02d}", f"private runtime header must expose helper symbol {symbol}", failures)
    return checks_passed, checks_total


def run_dynamic_checks(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, "dynamic_probe", check_id, detail, failures)

    check(NATIVE_EXE.exists(), "M262-D001-DYN-01", f"missing native executable: {display_path(NATIVE_EXE)}")
    check(RUNTIME_LIBRARY.exists(), "M262-D001-DYN-02", f"missing runtime library: {display_path(RUNTIME_LIBRARY)}")
    check(FIXTURE.exists(), "M262-D001-DYN-03", f"missing fixture: {display_path(FIXTURE)}")
    pool_fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m260_d002_reference_counting_weak_autoreleasepool_positive.objc3"
    check(pool_fixture.exists(), "M262-D001-DYN-04", f"missing autoreleasepool fixture: {display_path(pool_fixture)}")
    if failures:
        return checks_passed, checks_total, {"skipped": False}

    ensure_build_command = [
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m262-d001-dynamic-check",
        "--summary-out",
        "tmp/reports/m262/M262-D001/ensure_objc3c_native_build_from_checker_summary.json",
    ]
    ensure_build = run_command(ensure_build_command, ROOT)
    check(ensure_build.returncode == 0, "M262-D001-DYN-05A", f"fast native build failed: {ensure_build.stdout}{ensure_build.stderr}")
    if ensure_build.returncode != 0:
        return checks_passed, checks_total, {"skipped": False, "ensure_build_stdout": ensure_build.stdout, "ensure_build_stderr": ensure_build.stderr}

    positive_dir = PROBE_ROOT / "positive"
    positive_dir.mkdir(parents=True, exist_ok=True)
    compile_command = [str(NATIVE_EXE), str(FIXTURE), "--out-dir", str(positive_dir), "--emit-prefix", "module", "-fobjc-arc"]
    compile_result = run_command(compile_command, ROOT)
    module_ir = positive_dir / "module.ll"
    module_obj = positive_dir / "module.obj"
    check(compile_result.returncode == 0, "M262-D001-DYN-06", f"fixture compile failed: {compile_result.stdout}{compile_result.stderr}")
    check(module_ir.exists(), "M262-D001-DYN-07", f"missing emitted IR: {display_path(module_ir)}")
    check(module_obj.exists(), "M262-D001-DYN-08", f"missing emitted object: {display_path(module_obj)}")
    if compile_result.returncode != 0 or not module_ir.exists() or not module_obj.exists():
        return checks_passed, checks_total, {"skipped": False, "compile_stdout": compile_result.stdout, "compile_stderr": compile_result.stderr}

    ir_text = read_text(module_ir)
    boundary_line = next((line for line in ir_text.splitlines() if line.startswith(BOUNDARY_PREFIX + "contract=")), "")
    check(bool(boundary_line), "M262-D001-DYN-09", "IR must publish the runtime ARC helper API surface boundary line")
    check(NAMED_METADATA_PREFIX in ir_text, "M262-D001-DYN-10", "IR must publish !objc3.objc_runtime_arc_helper_api_surface")
    check(f"contract={CONTRACT_ID}" in boundary_line, "M262-D001-DYN-11", "boundary line must carry the runtime ARC helper API contract id")
    check(f"reference_model={REFERENCE_MODEL}" in boundary_line, "M262-D001-DYN-12", "boundary line must carry the frozen reference model")
    check(f"weak_model={WEAK_MODEL}" in boundary_line, "M262-D001-DYN-13", "boundary line must carry the frozen weak model")
    check(f"autoreleasepool_model={AUTORELEASEPOOL_MODEL}" in boundary_line, "M262-D001-DYN-14", "boundary line must carry the frozen autoreleasepool model")
    check(f"fail_closed_model={FAIL_CLOSED_MODEL}" in boundary_line, "M262-D001-DYN-15", "boundary line must carry the fail-closed model")
    for symbol in (
        "objc3_runtime_retain_i32",
        "objc3_runtime_release_i32",
        "objc3_runtime_autorelease_i32",
        "objc3_runtime_read_current_property_i32",
        "objc3_runtime_write_current_property_i32",
        "objc3_runtime_exchange_current_property_i32",
        "objc3_runtime_load_weak_current_property_i32",
        "objc3_runtime_store_weak_current_property_i32",
    ):
        checks_total += 1
        checks_passed += require(symbol in ir_text, display_path(module_ir), f"M262-D001-DYN-SYM-{checks_total:02d}", f"IR must retain helper symbol {symbol}", failures)

    pool_dir = PROBE_ROOT / "autoreleasepool"
    pool_dir.mkdir(parents=True, exist_ok=True)
    pool_compile_command = [str(NATIVE_EXE), str(pool_fixture), "--out-dir", str(pool_dir), "--emit-prefix", "module"]
    pool_compile_result = run_command(pool_compile_command, ROOT)
    pool_ir = pool_dir / "module.ll"
    pool_obj = pool_dir / "module.obj"
    check(pool_compile_result.returncode == 0, "M262-D001-DYN-16", f"autoreleasepool fixture compile failed: {pool_compile_result.stdout}{pool_compile_result.stderr}")
    check(pool_ir.exists(), "M262-D001-DYN-17", f"missing emitted autoreleasepool IR: {display_path(pool_ir)}")
    check(pool_obj.exists(), "M262-D001-DYN-18", f"missing emitted autoreleasepool object: {display_path(pool_obj)}")
    if pool_compile_result.returncode != 0 or not pool_ir.exists() or not pool_obj.exists():
        return checks_passed, checks_total, {"skipped": False, "pool_compile_stdout": pool_compile_result.stdout, "pool_compile_stderr": pool_compile_result.stderr}

    pool_ir_text = read_text(pool_ir)
    for symbol in ("objc3_runtime_push_autoreleasepool_scope", "objc3_runtime_pop_autoreleasepool_scope"):
        checks_total += 1
        checks_passed += require(symbol in pool_ir_text, display_path(pool_ir), f"M262-D001-DYN-SYM-{checks_total:02d}", f"IR must retain helper symbol {symbol}", failures)
    return checks_passed, checks_total, {
        "skipped": False,
        "ensure_build_command": ensure_build_command,
        "compile_command": compile_command,
        "positive_dir": display_path(positive_dir),
        "module_ir": display_path(module_ir),
        "module_obj": display_path(module_obj),
        "pool_compile_command": pool_compile_command,
        "pool_dir": display_path(pool_dir),
        "pool_ir": display_path(pool_ir),
        "pool_obj": display_path(pool_obj),
    }


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    runtime_header_text = read_text(RUNTIME_HEADER)
    runtime_internal_text = read_text(RUNTIME_INTERNAL_HEADER)
    pub_passed, pub_total = validate_public_private_boundary(runtime_header_text, runtime_internal_text, failures)
    checks_passed += pub_passed
    checks_total += pub_total

    dynamic_payload: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        dyn_passed, dyn_total, dynamic_payload = run_dynamic_checks(failures)
        checks_passed += dyn_passed
        checks_total += dyn_total

    summary = {
        "mode": MODE,
        "issue": "M262-D001",
        "contract_id": CONTRACT_ID,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "failures": [{"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail} for f in failures],
        "dynamic_probe": dynamic_payload,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}")
        print(f"[info] wrote summary to {display_path(args.summary_out)}")
        return 1

    print("[ok] M262-D001 runtime ARC helper API surface contract satisfied")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())


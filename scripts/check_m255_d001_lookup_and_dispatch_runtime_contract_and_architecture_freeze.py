#!/usr/bin/env python3
"""Fail-closed validator for M255-D001 lookup/dispatch runtime freeze."""

from __future__ import annotations

import argparse
import json
import os
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m255-d001-lookup-dispatch-runtime-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-runtime-lookup-dispatch-freeze/m255-d001-v1"
LOOKUP_SYMBOL = "objc3_runtime_lookup_selector"
DISPATCH_SYMBOL = "objc3_runtime_dispatch_i32"
SELECTOR_HANDLE_TYPE = "objc3_runtime_selector_handle"
SELECTOR_INTERNING_MODEL = (
    "process-global-selector-intern-table-stable-id-per-canonical-selector-spelling"
)
LOOKUP_TABLE_MODEL = "metadata-backed-selector-lookup-tables-deferred-until-m255-d002"
CACHE_MODEL = "method-cache-and-runtime-slow-path-deferred-until-m255-d003"
PROTOCOL_CATEGORY_MODEL = (
    "protocol-and-category-aware-method-resolution-deferred-until-m255-d004"
)
COMPATIBILITY_MODEL = "compatibility-shim-remains-test-only-non-authoritative-runtime-surface"
FAILURE_MODEL = "runtime-lookup-and-dispatch-fail-closed-on-unmaterialized-resolution"
PROBE_SOURCE_PATH = "tests/tooling/runtime/m255_d001_lookup_dispatch_runtime_probe.cpp"
RUNTIME_LIBRARY_PATH = "artifacts/lib/objc3_runtime.lib"
SUMMARY_RELATIVE_PATH = "tmp/reports/m255/M255-D001/lookup_dispatch_runtime_contract_summary.json"
DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m255_lookup_and_dispatch_runtime_contract_and_architecture_freeze_d001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m255"
    / "m255_d001_lookup_and_dispatch_runtime_contract_and_architecture_freeze_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_RUNTIME_SURFACE_DOC = ROOT / "native" / "objc3c" / "src" / "runtime" / "README.md"
DEFAULT_TOOLING_RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_LOWERING_CONTRACT = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_RUNTIME_HEADER = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.h"
DEFAULT_RUNTIME_SOURCE = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
DEFAULT_RUNTIME_SHIM = ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c"
DEFAULT_RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m255_d001_lookup_dispatch_runtime_probe.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
DEFAULT_RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
DEFAULT_SUMMARY_OUT = Path(SUMMARY_RELATIVE_PATH)
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m255" / "d001-lookup-dispatch-runtime"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


EXPECTATIONS_SNIPPETS = (
    SnippetCheck("M255-D001-DOC-EXP-01", "# M255 Lookup and Dispatch Runtime Contract and Architecture Freeze Expectations (D001)"),
    SnippetCheck("M255-D001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M255-D001-DOC-EXP-03", f"`{LOOKUP_SYMBOL}`"),
    SnippetCheck("M255-D001-DOC-EXP-04", f"`{DISPATCH_SYMBOL}`"),
    SnippetCheck("M255-D001-DOC-EXP-05", f"`{SELECTOR_INTERNING_MODEL}`"),
    SnippetCheck("M255-D001-DOC-EXP-06", f"`{PROBE_SOURCE_PATH}`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M255-D001-PKT-01", "# M255-D001 Lookup and Dispatch Runtime Contract and Architecture Freeze Packet"),
    SnippetCheck("M255-D001-PKT-02", "Packet: `M255-D001`"),
    SnippetCheck("M255-D001-PKT-03", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M255-D001-PKT-04", f"`{LOOKUP_TABLE_MODEL}`"),
    SnippetCheck("M255-D001-PKT-05", f"`{CACHE_MODEL}`"),
    SnippetCheck("M255-D001-PKT-06", f"`{SUMMARY_RELATIVE_PATH}`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M255-D001-ARCH-01", "## M255 lookup and dispatch runtime freeze (D001)"),
    SnippetCheck("M255-D001-ARCH-02", f"- `{LOOKUP_SYMBOL}`"),
    SnippetCheck("M255-D001-ARCH-03", f"- `{DISPATCH_SYMBOL}`"),
)
RUNTIME_SURFACE_SNIPPETS = (
    SnippetCheck("M255-D001-RTDOC-01", "`M255-D001` then freezes the runtime-owned lookup/dispatch boundary"),
    SnippetCheck("M255-D001-RTDOC-02", f"- canonical selector lookup symbol `{LOOKUP_SYMBOL}`"),
    SnippetCheck("M255-D001-RTDOC-03", f"- metadata-backed selector lookup tables remain deferred to `M255-D002`"),
)
TOOLING_RUNTIME_README_SNIPPETS = (
    SnippetCheck("M255-D001-TRTDOC-01", "`M255-D001` freezes the next runtime-owned boundary after the live dispatch"),
    SnippetCheck("M255-D001-TRTDOC-02", f"- contract id `{CONTRACT_ID}`"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M255-D001-NDOC-01", "## Lookup and dispatch runtime freeze (M255-D001)"),
    SnippetCheck("M255-D001-NDOC-02", f"- contract id `{CONTRACT_ID}`"),
    SnippetCheck("M255-D001-NDOC-03", f"- canonical selector lookup symbol `{LOOKUP_SYMBOL}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M255-D001-SPC-01", "## M255 lookup and dispatch runtime freeze (D001)"),
    SnippetCheck("M255-D001-SPC-02", f"- contract id `{CONTRACT_ID}`"),
    SnippetCheck("M255-D001-SPC-03", f"- metadata-backed selector lookup tables remain deferred to `M255-D002`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M255-D001-META-01", "## M255 lookup/dispatch runtime metadata anchors (D001)"),
    SnippetCheck("M255-D001-META-02", f"- canonical selector lookup symbol `{LOOKUP_SYMBOL}`"),
    SnippetCheck("M255-D001-META-03", f"- canonical dispatch symbol `{DISPATCH_SYMBOL}`"),
)
LOWERING_CONTRACT_SNIPPETS = (
    SnippetCheck("M255-D001-LC-01", "M255-D001 lookup/dispatch runtime freeze anchor"),
    SnippetCheck("M255-D001-LC-02", "kObjc3RuntimeLookupDispatchContractId"),
    SnippetCheck("M255-D001-LC-03", "kObjc3RuntimeLookupDispatchSelectorInterningModel"),
    SnippetCheck("M255-D001-LC-04", "kObjc3RuntimeLookupDispatchLookupTableModel"),
    SnippetCheck("M255-D001-LC-05", "kObjc3RuntimeLookupDispatchCacheModel"),
    SnippetCheck("M255-D001-LC-06", "kObjc3RuntimeLookupDispatchProtocolCategoryModel"),
    SnippetCheck("M255-D001-LC-07", "kObjc3RuntimeLookupDispatchCompatibilityModel"),
    SnippetCheck("M255-D001-LC-08", "kObjc3RuntimeLookupDispatchFailureModel"),
)
IR_EMITTER_SNIPPETS = (
    SnippetCheck("M255-D001-IR-01", "M255-D001 lookup/dispatch runtime freeze anchor"),
    SnippetCheck("M255-D001-IR-02", "objc3_runtime_lookup_selector / objc3_runtime_dispatch_i32 surface"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M255-D001-PAR-01", "M255-D001 lookup/dispatch runtime freeze anchor"),
    SnippetCheck("M255-D001-PAR-02", "selector interning, metadata-backed lookup tables, method caches, or"),
)
RUNTIME_HEADER_SNIPPETS = (
    SnippetCheck("M255-D001-RTH-01", "M255-D001 lookup-dispatch-runtime anchor"),
    SnippetCheck("M255-D001-RTH-02", f"typedef struct {SELECTOR_HANDLE_TYPE} {{"),
    SnippetCheck("M255-D001-RTH-03", LOOKUP_SYMBOL),
    SnippetCheck("M255-D001-RTH-04", DISPATCH_SYMBOL),
)
RUNTIME_SOURCE_SNIPPETS = (
    SnippetCheck("M255-D001-RTS-01", "M255-D001 lookup-dispatch-runtime anchor"),
    SnippetCheck("M255-D001-RTS-02", "selector_index_by_name"),
    SnippetCheck("M255-D001-RTS-03", "selector_slots"),
    SnippetCheck("M255-D001-RTS-04", "LookupSelectorUnlocked"),
    SnippetCheck("M255-D001-RTS-05", "if (receiver == 0) {"),
)
RUNTIME_SHIM_SNIPPETS = (
    SnippetCheck("M255-D001-SHIM-01", "M255-D001 lookup/dispatch runtime freeze"),
    SnippetCheck("M255-D001-SHIM-02", "shim stays test-only compatibility evidence and is not the authoritative live"),
)
PROBE_SNIPPETS = (
    SnippetCheck("M255-D001-PRB-01", '#include "runtime/objc3_runtime.h"'),
    SnippetCheck("M255-D001-PRB-02", 'objc3_runtime_lookup_selector("copy")'),
    SnippetCheck("M255-D001-PRB-03", 'objc3_runtime_dispatch_i32(7, "copy", 1, 2, 3, 4)'),
    SnippetCheck("M255-D001-PRB-04", '"m255-d001-runtime-probe"'),
)
PACKAGE_SNIPPETS = (
    SnippetCheck(
        "M255-D001-PKG-01",
        '"check:objc3c:m255-d001-lookup-and-dispatch-runtime-contract-and-architecture-freeze": "python scripts/check_m255_d001_lookup_and_dispatch_runtime_contract_and_architecture_freeze.py"',
    ),
    SnippetCheck(
        "M255-D001-PKG-02",
        '"test:tooling:m255-d001-lookup-and-dispatch-runtime-contract-and-architecture-freeze": "python -m pytest tests/tooling/test_check_m255_d001_lookup_and_dispatch_runtime_contract_and_architecture_freeze.py -q"',
    ),
    SnippetCheck(
        "M255-D001-PKG-03",
        '"check:objc3c:m255-d001-lane-d-readiness": "python scripts/run_m255_d001_lane_d_readiness.py"',
    ),
)



def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--runtime-surface-doc", type=Path, default=DEFAULT_RUNTIME_SURFACE_DOC)
    parser.add_argument("--tooling-runtime-readme", type=Path, default=DEFAULT_TOOLING_RUNTIME_README)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--lowering-contract", type=Path, default=DEFAULT_LOWERING_CONTRACT)
    parser.add_argument("--ir-emitter-cpp", type=Path, default=DEFAULT_IR_EMITTER_CPP)
    parser.add_argument("--parser-cpp", type=Path, default=DEFAULT_PARSER_CPP)
    parser.add_argument("--runtime-header", type=Path, default=DEFAULT_RUNTIME_HEADER)
    parser.add_argument("--runtime-source", type=Path, default=DEFAULT_RUNTIME_SOURCE)
    parser.add_argument("--runtime-shim", type=Path, default=DEFAULT_RUNTIME_SHIM)
    parser.add_argument("--runtime-probe", type=Path, default=DEFAULT_RUNTIME_PROBE)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--runtime-library", type=Path, default=DEFAULT_RUNTIME_LIBRARY)
    parser.add_argument("--runtime-include-root", type=Path, default=DEFAULT_RUNTIME_INCLUDE_ROOT)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, exists_check_id: str, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    passed = 0
    if path.exists():
        passed += 1
        text = read_text(path)
    else:
        failures.append(Finding(path.as_posix(), exists_check_id, "file is missing"))
        return passed
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(path.as_posix(), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def resolve_tool(executable: str) -> Path | None:
    env_bin = os.environ.get("LLVM_BIN_DIR")
    if env_bin:
        candidate = Path(env_bin) / executable
        if candidate.exists():
            return candidate
    default_candidate = Path("C:/Program Files/LLVM/bin") / executable
    if default_candidate.exists():
        return default_candidate
    which = shutil.which(executable)
    return Path(which) if which else None


def stale_inputs(output: Path, inputs: Sequence[Path]) -> list[str]:
    if not output.exists():
        return [output.as_posix()]
    output_mtime = output.stat().st_mtime
    stale: list[str] = []
    for path in inputs:
        if path.exists() and path.stat().st_mtime > output_mtime:
            stale.append(path.as_posix())
    return stale


def run_command(command: Sequence[str], cwd: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def expected_dispatch(receiver: int, selector: str | None, a0: int, a1: int, a2: int, a3: int) -> int:
    if receiver == 0:
        return 0
    modulus = 2147483629
    selector_score = 0
    if selector is not None:
        for index, byte in enumerate(selector.encode("utf-8"), start=1):
            selector_score = (selector_score + (byte * index)) % modulus
    value = 41
    value += receiver * 97
    value += a0 * 7
    value += a1 * 11
    value += a2 * 13
    value += a3 * 17
    value += selector_score * 19
    value %= modulus
    if value < 0:
        value += modulus
    return value


def run_probe_case(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0
    case_payload: dict[str, object] = {"case_id": "runtime-probe"}

    checks_total += 1
    if args.runtime_library.exists():
        checks_passed += 1
    else:
        failures.append(Finding(args.runtime_library.as_posix(), "M255-D001-DYN-01", "runtime library is missing"))
        return checks_total, checks_passed, case_payload

    clangxx = resolve_tool("clang++.exe") or resolve_tool("clang++")
    checks_total += 1
    if clangxx is not None:
        checks_passed += 1
    else:
        failures.append(Finding("dynamic", "M255-D001-DYN-02", "clang++ is required for runtime probe compilation"))
        return checks_total, checks_passed, case_payload

    probe_dir = args.probe_root.resolve() / "probe"
    probe_dir.mkdir(parents=True, exist_ok=True)
    probe_exe = probe_dir / "m255_d001_lookup_dispatch_runtime_probe.exe"
    compile_command = [
        str(clangxx),
        "-std=c++20",
        "-Wall",
        "-Wextra",
        "-pedantic",
        f"-I{args.runtime_include_root.resolve()}",
        str(args.runtime_probe.resolve()),
        str(args.runtime_library.resolve()),
        "-o",
        str(probe_exe),
    ]
    case_payload["compile_command"] = compile_command
    compile_result = run_command(compile_command, ROOT)
    case_payload["compile_exit_code"] = compile_result.returncode
    checks_total += 2
    if compile_result.returncode == 0:
        checks_passed += 1
    else:
        failures.append(Finding(args.runtime_probe.as_posix(), "M255-D001-DYN-03", f"probe compile exited with {compile_result.returncode}"))
    if probe_exe.exists():
        checks_passed += 1
    else:
        failures.append(Finding(probe_exe.as_posix(), "M255-D001-DYN-04", "probe executable missing after compile"))
        return checks_total, checks_passed, case_payload

    run_result = run_command([str(probe_exe)], ROOT)
    case_payload["run_exit_code"] = run_result.returncode
    checks_total += 1
    if run_result.returncode == 0:
        checks_passed += 1
    else:
        failures.append(Finding(probe_exe.as_posix(), "M255-D001-DYN-05", f"probe run exited with {run_result.returncode}"))
        return checks_total, checks_passed, case_payload

    try:
        payload = json.loads(run_result.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(probe_exe.as_posix(), "M255-D001-DYN-06", f"probe stdout is not JSON: {exc}"))
        checks_total += 1
        return checks_total, checks_passed, case_payload
    checks_total += 1
    checks_passed += 1
    case_payload["probe_payload"] = payload

    expected = {
        "register_status": 0,
        "lookup_null_is_null": True,
        "copy_selector_reused": True,
        "copy_selector_stable_id": 1,
        "gamma_selector_stable_id": 2,
        "copy_selector_spelling_matches": True,
        "dispatch_result": expected_dispatch(7, "copy", 1, 2, 3, 4),
        "expected_dispatch_result": expected_dispatch(7, "copy", 1, 2, 3, 4),
        "nil_dispatch_result": 0,
        "snapshot_status": 0,
        "registered_image_count": 1,
        "registered_descriptor_total": 1,
        "next_expected_registration_order_ordinal": 2,
        "last_successful_registration_order_ordinal": 1,
        "last_registration_status": 0,
        "last_registered_module_name": "m255-d001-runtime-probe",
        "last_registered_translation_unit_identity_key": "m255-d001::translation-unit",
        "copy_after_reset_stable_id": 1,
    }
    for key, expected_value in expected.items():
        checks_total += 1
        if payload.get(key) == expected_value:
            checks_passed += 1
        else:
            failures.append(
                Finding(
                    probe_exe.as_posix(),
                    f"M255-D001-DYN-{key}",
                    f"expected {key}={expected_value!r}, got {payload.get(key)!r}",
                )
            )

    return checks_total, checks_passed, case_payload


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(list(argv) if argv is not None else sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    artifacts: tuple[tuple[Path, str, Sequence[SnippetCheck]], ...] = (
        (args.expectations_doc, "M255-D001-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M255-D001-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M255-D001-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.runtime_surface_doc, "M255-D001-RTDOC-EXISTS", RUNTIME_SURFACE_SNIPPETS),
        (args.tooling_runtime_readme, "M255-D001-TRTDOC-EXISTS", TOOLING_RUNTIME_README_SNIPPETS),
        (args.native_doc, "M255-D001-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M255-D001-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M255-D001-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.lowering_contract, "M255-D001-LC-EXISTS", LOWERING_CONTRACT_SNIPPETS),
        (args.ir_emitter_cpp, "M255-D001-IR-EXISTS", IR_EMITTER_SNIPPETS),
        (args.parser_cpp, "M255-D001-PAR-EXISTS", PARSER_SNIPPETS),
        (args.runtime_header, "M255-D001-RTH-EXISTS", RUNTIME_HEADER_SNIPPETS),
        (args.runtime_source, "M255-D001-RTS-EXISTS", RUNTIME_SOURCE_SNIPPETS),
        (args.runtime_shim, "M255-D001-SHIM-EXISTS", RUNTIME_SHIM_SNIPPETS),
        (args.runtime_probe, "M255-D001-PRB-EXISTS", PROBE_SNIPPETS),
        (args.package_json, "M255-D001-PKG-EXISTS", PACKAGE_SNIPPETS),
    )

    for path, exists_check_id, snippets in artifacts:
        checks_total += 1 + len(snippets)
        checks_passed += ensure_snippets(path, exists_check_id, snippets, failures)

    dynamic_cases: list[dict[str, object]] = []
    if not args.skip_dynamic_probes:
        dynamic_total, dynamic_passed, dynamic_case = run_probe_case(args, failures)
        checks_total += dynamic_total
        checks_passed += dynamic_passed
        dynamic_cases.append(dynamic_case)

    ok = not failures
    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_cases": dynamic_cases,
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in failures
        ],
    }
    summary_out = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if ok:
        return 0
    for finding in failures:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run())

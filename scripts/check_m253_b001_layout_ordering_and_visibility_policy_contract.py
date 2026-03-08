#!/usr/bin/env python3
"""Fail-closed contract checker for M253-B001 layout ordering and visibility policy freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m253-b001-layout-ordering-and-visibility-policy-contract-v1"
CONTRACT_ID = "objc3c-runtime-metadata-layout-ordering-visibility-policy-freeze/m253-b001-v1"
FAMILY_ORDER = [
    "__objc3_image_info",
    "__objc3_sec_class_descriptors",
    "__objc3_sec_protocol_descriptors",
    "__objc3_sec_category_descriptors",
    "__objc3_sec_property_descriptors",
    "__objc3_sec_ivar_descriptors",
]
MATRIX_ROW_KEYS = [
    "interface-node-to-emission",
    "implementation-node-to-emission",
    "class-node-to-emission",
    "metaclass-node-to-emission",
    "protocol-node-to-emission",
    "category-node-to-emission",
    "property-node-to-emission",
    "method-node-to-emission",
    "ivar-node-to-emission",
]
EMITTED_ROW_KEYS = {
    "class-node-to-emission",
    "protocol-node-to-emission",
    "category-node-to-emission",
    "property-node-to-emission",
    "ivar-node-to-emission",
}
NON_STANDALONE_ROW_KEYS = {
    "interface-node-to-emission",
    "implementation-node-to-emission",
    "metaclass-node-to-emission",
    "method-node-to-emission",
}

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m253_layout_ordering_and_visibility_policy_contract_and_architecture_freeze_b001_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m253" / "m253_b001_layout_ordering_and_visibility_policy_contract_and_architecture_freeze_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_CLASS_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
DEFAULT_HELLO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m253" / "layout-policy"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m253/M253-B001/layout_ordering_and_visibility_policy_contract_summary.json")


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B001-DOC-EXP-01", "# M253 Layout Ordering and Visibility Policy Contract and Architecture Freeze Expectations (B001)"),
    SnippetCheck("M253-B001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M253-B001-DOC-EXP-03", "`image-info-then-class-protocol-category-property-ivar`"),
    SnippetCheck("M253-B001-DOC-EXP-04", "`local-linkage-omits-explicit-ir-visibility`"),
    SnippetCheck("M253-B001-DOC-EXP-05", "`M253-B002`"),
    SnippetCheck("M253-B001-DOC-EXP-06", "`M253-B003`"),
    SnippetCheck("M253-B001-DOC-EXP-07", "`tmp/reports/m253/M253-B001/layout_ordering_and_visibility_policy_contract_summary.json`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B001-DOC-PKT-01", "# M253-B001 Layout Ordering and Visibility Policy Contract and Architecture Freeze Packet"),
    SnippetCheck("M253-B001-DOC-PKT-02", "Packet: `M253-B001`"),
    SnippetCheck("M253-B001-DOC-PKT-03", "Dependencies: none"),
    SnippetCheck("M253-B001-DOC-PKT-04", "`llvm.used-emission-order`"),
    SnippetCheck("M253-B001-DOC-PKT-05", "`hello.objc3`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B001-ARCH-01", "M253 lane-B B001 layout ordering and visibility policy freeze anchors explicit"),
    SnippetCheck("M253-B001-ARCH-02", "m253_layout_ordering_and_visibility_policy_contract_and_architecture_freeze_b001_expectations.md"),
    SnippetCheck("M253-B001-ARCH-03", "B002 can semantic-finalize metadata ordering/visibility decisions"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B001-NDOC-01", "## Layout ordering and visibility policy freeze (M253-B001)"),
    SnippetCheck("M253-B001-NDOC-02", "`image-info-then-class-protocol-category-property-ivar`"),
    SnippetCheck("M253-B001-NDOC-03", "`local-linkage-omits-explicit-ir-visibility`"),
    SnippetCheck("M253-B001-NDOC-04", "`M253-B003`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B001-SPC-01", "## M253 layout ordering and visibility policy freeze (B001)"),
    SnippetCheck("M253-B001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-B001-SPC-03", "semantic finalization of layout decisions remains deferred to `M253-B002`"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B001-META-01", "## M253 layout ordering and visibility policy metadata anchors (B001)"),
    SnippetCheck("M253-B001-META-02", "`llvm.used-emission-order`"),
    SnippetCheck("M253-B001-META-03", "`object-format-neutral-until-m253-b003`"),
)

LOWERING_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B001-LHDR-01", "kObjc3RuntimeMetadataLayoutOrderingVisibilityPolicyContractId"),
    SnippetCheck("M253-B001-LHDR-02", "kObjc3RuntimeMetadataLayoutFamilyOrderingModel"),
    SnippetCheck("M253-B001-LHDR-03", "kObjc3RuntimeMetadataVisibilitySpellingPolicy"),
    SnippetCheck("M253-B001-LHDR-04", "M253-B001 layout/visibility policy anchor"),
)

LOWERING_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B001-LCPP-01", "M253-B001 layout/visibility policy anchor: replay keys may not infer"),
    SnippetCheck("M253-B001-LCPP-02", "local-linkage/no-COMDAT policy"),
    SnippetCheck("M253-B001-LCPP-03", "llvm.used retention order"),
)

IR_EMITTER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B001-IR-01", "M253-B001 layout/visibility policy anchor: image-info emits first"),
    SnippetCheck("M253-B001-IR-02", "descriptor ordinals ascend before the family aggregate"),
    SnippetCheck("M253-B001-IR-03", "omit explicit hidden visibility spelling"),
)

PROCESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B001-PROC-01", "M253-B001 layout/visibility policy anchor: llvm-direct object emission must preserve"),
    SnippetCheck("M253-B001-PROC-02", "local-linkage/no-COMDAT policy"),
    SnippetCheck("M253-B001-PROC-03", "object-format-specific rewrites before M253-B003"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M253-B001-PKG-01",
        '"check:objc3c:m253-b001-layout-ordering-and-visibility-policy-contract": "python scripts/check_m253_b001_layout_ordering_and_visibility_policy_contract.py"',
    ),
    SnippetCheck(
        "M253-B001-PKG-02",
        '"test:tooling:m253-b001-layout-ordering-and-visibility-policy-contract": "python -m pytest tests/tooling/test_check_m253_b001_layout_ordering_and_visibility_policy_contract.py -q"',
    ),
    SnippetCheck(
        "M253-B001-PKG-03",
        '"check:objc3c:m253-b001-lane-b-readiness": "npm run check:objc3c:m253-a002-lane-a-readiness && npm run build:objc3c-native && npm run check:objc3c:m253-b001-layout-ordering-and-visibility-policy-contract && npm run test:tooling:m253-b001-layout-ordering-and-visibility-policy-contract"',
    ),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=DEFAULT_LOWERING_CPP)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--runner-exe", type=Path, default=DEFAULT_RUNNER_EXE)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--class-fixture", type=Path, default=DEFAULT_CLASS_FIXTURE)
    parser.add_argument("--hello-fixture", type=Path, default=DEFAULT_HELLO_FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


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


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))


def locate_matrix(payload: dict[str, Any]) -> dict[str, Any] | None:
    current: Any = payload
    for key in ("frontend", "pipeline", "semantic_surface", "objc_runtime_metadata_source_to_section_matrix"):
        if not isinstance(current, dict):
            return None
        current = current.get(key)
        if current is None:
            return None
    return current if isinstance(current, dict) else None


def run_runner_probe(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    out_dir = args.probe_root.resolve() / "runner-manifest"
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(args.runner_exe.resolve()),
        str(args.class_fixture.resolve()),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ]
    result = run_command(command, ROOT)
    summary_path = out_dir / "module.c_api_summary.json"
    manifest_path = out_dir / "module.manifest.json"
    case: dict[str, Any] = {
        "case_id": "M253-B001-CASE-RUNNER",
        "command": command,
        "process_exit_code": result.returncode,
        "summary_path": display_path(summary_path),
        "manifest_path": display_path(manifest_path),
    }

    checks_total += 5
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M253-B001-RUNNER-EXISTS", "frontend C API runner binary is missing", failures)
    checks_passed += require(args.class_fixture.exists(), display_path(args.class_fixture), "M253-B001-RUNNER-FIXTURE", "class fixture is missing", failures)
    checks_passed += require(result.returncode == 0, display_path(summary_path), "M253-B001-RUNNER-STATUS", "runner probe must exit 0", failures)
    checks_passed += require(summary_path.exists(), display_path(summary_path), "M253-B001-RUNNER-SUMMARY", "runner probe must write module.c_api_summary.json", failures)
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M253-B001-RUNNER-MANIFEST", "runner probe must write module.manifest.json", failures)
    if not manifest_path.exists():
        return checks_total, checks_passed, case

    manifest_payload = load_json(manifest_path)
    matrix = locate_matrix(manifest_payload if isinstance(manifest_payload, dict) else {})
    case["matrix_present"] = matrix is not None
    checks_total += 10
    checks_passed += require(isinstance(matrix, dict), display_path(manifest_path), "M253-B001-RUNNER-MATRIX", "matrix semantic surface missing", failures)
    if isinstance(matrix, dict):
        case["row_ordering_model"] = matrix.get("row_ordering_model")
        case["row_keys"] = [row.get("row_key") for row in matrix.get("rows", []) if isinstance(row, dict)]
        checks_passed += require(matrix.get("row_ordering_model") == "source-graph-node-kind-order-v1", display_path(manifest_path), "M253-B001-RUNNER-ORDERING-MODEL", "row ordering model mismatch", failures)
        checks_passed += require(matrix.get("row_ordering_frozen") is True, display_path(manifest_path), "M253-B001-RUNNER-ORDERING-FROZEN", "row ordering must be frozen", failures)
        rows = matrix.get("rows")
        checks_passed += require(isinstance(rows, list) and len(rows) == len(MATRIX_ROW_KEYS), display_path(manifest_path), "M253-B001-RUNNER-ROWS", "rows must be a list of length 9", failures)
        if isinstance(rows, list) and len(rows) == len(MATRIX_ROW_KEYS):
            row_keys = [row.get("row_key") for row in rows if isinstance(row, dict)]
            checks_passed += require(row_keys == MATRIX_ROW_KEYS, display_path(manifest_path), "M253-B001-RUNNER-ROW-ORDER", "row order mismatch", failures)
            by_key = {row.get("row_key"): row for row in rows if isinstance(row, dict)}
            for row_key in EMITTED_ROW_KEYS:
                row = by_key.get(row_key)
                checks_total += 1
                checks_passed += require(isinstance(row, dict) and row.get("relocation_behavior") == "zero-sentinel-or-count-plus-pointer-vector", display_path(manifest_path), f"M253-B001-RUNNER-RELOC-{row_key}", f"expected emitted row {row_key} to preserve relocation model", failures)
            for row_key in NON_STANDALONE_ROW_KEYS:
                row = by_key.get(row_key)
                checks_total += 1
                checks_passed += require(isinstance(row, dict) and row.get("relocation_behavior") == "none", display_path(manifest_path), f"M253-B001-RUNNER-NONSTANDALONE-{row_key}", f"expected non-standalone row {row_key} to preserve relocation none", failures)
        else:
            checks_total += len(EMITTED_ROW_KEYS) + len(NON_STANDALONE_ROW_KEYS) + 1
    else:
        checks_total += 9

    return checks_total, checks_passed, case


def run_native_probe(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    out_dir = args.probe_root.resolve() / "native-hello"
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(args.native_exe.resolve()),
        str(args.hello_fixture.resolve()),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ]
    result = run_command(command, ROOT)
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    case: dict[str, Any] = {
        "case_id": "M253-B001-CASE-NATIVE",
        "command": command,
        "process_exit_code": result.returncode,
        "ir_path": display_path(ir_path),
        "object_path": display_path(obj_path),
    }

    checks_total += 5
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M253-B001-NATIVE-EXISTS", "objc3c-native.exe is missing", failures)
    checks_passed += require(args.hello_fixture.exists(), display_path(args.hello_fixture), "M253-B001-NATIVE-FIXTURE", "hello fixture is missing", failures)
    checks_passed += require(result.returncode == 0, display_path(ir_path), "M253-B001-NATIVE-STATUS", "native probe must exit 0", failures)
    checks_passed += require(ir_path.exists(), display_path(ir_path), "M253-B001-NATIVE-IR", "native probe must emit module.ll", failures)
    checks_passed += require(obj_path.exists(), display_path(obj_path), "M253-B001-NATIVE-OBJ", "native probe must emit module.obj", failures)
    if not ir_path.exists():
        return checks_total, checks_passed, case

    ir_text = read_text(ir_path)
    case["contains_comdat"] = "comdat" in ir_text
    symbol_positions: dict[str, int] = {}
    checks_total += 1
    checks_passed += require("@llvm.used = appending global [" in ir_text, display_path(ir_path), "M253-B001-NATIVE-LLVM-USED", "IR must contain @llvm.used retention root", failures)
    for symbol in FAMILY_ORDER:
        token = f"@{symbol} ="
        position = ir_text.find(token)
        symbol_positions[symbol] = position
        checks_total += 1
        checks_passed += require(position >= 0, display_path(ir_path), f"M253-B001-NATIVE-SYMBOL-{symbol}", f"missing metadata global {symbol}", failures)
    case["symbol_positions"] = symbol_positions
    if all(position >= 0 for position in symbol_positions.values()):
        positions = [symbol_positions[symbol] for symbol in FAMILY_ORDER]
        checks_total += 1
        checks_passed += require(positions == sorted(positions), display_path(ir_path), "M253-B001-NATIVE-FAMILY-ORDER", "metadata globals are out of frozen family order", failures)
    else:
        checks_total += 1

    metadata_lines = {
        symbol: next((line for line in ir_text.splitlines() if line.startswith(f"@{symbol} =")), "")
        for symbol in FAMILY_ORDER
    }
    case["metadata_lines"] = metadata_lines
    checks_total += 1
    checks_passed += require("comdat" not in ir_text, display_path(ir_path), "M253-B001-NATIVE-COMDAT", "metadata IR must not introduce COMDAT", failures)
    checks_total += 1
    checks_passed += require(all("hidden" not in line for line in metadata_lines.values()), display_path(ir_path), "M253-B001-NATIVE-HIDDEN", "metadata global lines must not spell explicit hidden visibility", failures)
    checks_total += 1
    checks_passed += require("@__objc3_image_info = internal global { i32, i32 } zeroinitializer, section \"objc3.runtime.image_info\", align 4" in ir_text, display_path(ir_path), "M253-B001-NATIVE-IMAGE-INFO", "image-info line mismatch", failures)
    for symbol, section in (
        ("__objc3_sec_class_descriptors", "objc3.runtime.class_descriptors"),
        ("__objc3_sec_protocol_descriptors", "objc3.runtime.protocol_descriptors"),
        ("__objc3_sec_category_descriptors", "objc3.runtime.category_descriptors"),
        ("__objc3_sec_property_descriptors", "objc3.runtime.property_descriptors"),
        ("__objc3_sec_ivar_descriptors", "objc3.runtime.ivar_descriptors"),
    ):
        checks_total += 1
        checks_passed += require(f"@{symbol} = internal global {{ i64 }} {{ i64 0 }}, section \"{section}\", align 8" in ir_text, display_path(ir_path), f"M253-B001-NATIVE-AGGREGATE-{symbol}", f"aggregate line mismatch for {symbol}", failures)

    llvm_used_line = next((line for line in ir_text.splitlines() if line.startswith("@llvm.used = appending global [")), "")
    case["llvm_used_line"] = llvm_used_line
    checks_total += 1
    checks_passed += require(bool(llvm_used_line), display_path(ir_path), "M253-B001-NATIVE-LLVM-USED-LINE", "llvm.used line missing", failures)
    if llvm_used_line:
        order_ok = True
        last = -1
        for symbol in FAMILY_ORDER:
            idx = llvm_used_line.find(f"@{symbol}")
            if idx <= last:
                order_ok = False
                break
            last = idx
        checks_total += 1
        checks_passed += require(order_ok, display_path(ir_path), "M253-B001-NATIVE-LLVM-USED-ORDER", "llvm.used retention order mismatch", failures)
    else:
        checks_total += 1

    checks_total += 1
    checks_passed += require(obj_path.stat().st_size > 0 if obj_path.exists() else False, display_path(obj_path), "M253-B001-NATIVE-OBJ-SIZE", "module.obj must be non-empty", failures)
    return checks_total, checks_passed, case


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_artifacts = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.lowering_cpp, LOWERING_CPP_SNIPPETS),
        (args.ir_emitter, IR_EMITTER_SNIPPETS),
        (args.process_cpp, PROCESS_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in static_artifacts:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, Any]] = []
    dynamic_probes_executed = not args.skip_dynamic_probes
    if dynamic_probes_executed:
        runner_total, runner_passed, runner_case = run_runner_probe(args, failures)
        native_total, native_passed, native_case = run_native_probe(args, failures)
        checks_total += runner_total + native_total
        checks_passed += runner_passed + native_passed
        dynamic_cases.extend([runner_case, native_case])

    if not failures:
        checks_passed = checks_total

    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_probes_executed,
        "family_order": FAMILY_ORDER,
        "non_goals": [
            "no semantic finalization yet",
            "no COFF/ELF/Mach-O-specific policy branches yet",
            "no new emitted method/selector/string-pool sections",
        ],
        "evidence_path": str(args.summary_out).replace('\\', '/'),
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
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.check_id} {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

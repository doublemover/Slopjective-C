#!/usr/bin/env python3
"""Fail-closed contract checker for M253-B002 normalized metadata layout policy implementation."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m253-b002-deterministic-ordering-visibility-and-relocation-semantics-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-runtime-metadata-layout-policy/m253-b002-v1"
POLICY_METADATA_NAME = "!objc3.objc_runtime_metadata_layout_policy = !{!55}"
POLICY_COMMENT_PREFIX = "; runtime_metadata_layout_policy = contract=objc3c-runtime-metadata-layout-policy/m253-b002-v1"
FAMILY_ORDER = [
    "__objc3_image_info",
    "__objc3_sec_class_descriptors",
    "__objc3_sec_protocol_descriptors",
    "__objc3_sec_category_descriptors",
    "__objc3_sec_property_descriptors",
    "__objc3_sec_ivar_descriptors",
]
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m253_deterministic_ordering_visibility_and_relocation_semantics_core_feature_implementation_b002_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m253" / "m253_b002_deterministic_ordering_visibility_and_relocation_semantics_core_feature_implementation_packet.md"
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
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m253" / "layout-policy-normalized"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m253/M253-B002/deterministic_ordering_visibility_and_relocation_semantics_core_feature_implementation_summary.json")


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
    SnippetCheck("M253-B002-DOC-EXP-01", "# M253 Deterministic Ordering, Visibility, and Relocation Semantics Core Feature Implementation Expectations (B002)"),
    SnippetCheck("M253-B002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M253-B002-DOC-EXP-03", "`!objc3.objc_runtime_metadata_layout_policy`"),
    SnippetCheck("M253-B002-DOC-EXP-04", "`; runtime_metadata_layout_policy =`"),
    SnippetCheck("M253-B002-DOC-EXP-05", "`M253-B003`"),
    SnippetCheck("M253-B002-DOC-EXP-06", "`tmp/reports/m253/M253-B002/deterministic_ordering_visibility_and_relocation_semantics_core_feature_implementation_summary.json`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B002-DOC-PKT-01", "# M253-B002 Deterministic Ordering, Visibility, and Relocation Semantics Core Feature Implementation Packet"),
    SnippetCheck("M253-B002-DOC-PKT-02", "Packet: `M253-B002`"),
    SnippetCheck("M253-B002-DOC-PKT-03", "Dependencies: `M253-B001`, `M252-B004`"),
    SnippetCheck("M253-B002-DOC-PKT-04", "`hello.objc3`"),
    SnippetCheck("M253-B002-DOC-PKT-05", "`module.manifest.json`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B002-ARCH-01", "M253 lane-B B002 normalized layout policy anchors explicit"),
    SnippetCheck("M253-B002-ARCH-02", "m253_deterministic_ordering_visibility_and_relocation_semantics_core_feature_implementation_b002_expectations.md"),
    SnippetCheck("M253-B002-ARCH-03", "semantic-finalization boundary"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B002-NDOC-01", "## Deterministic ordering, visibility, and relocation semantics (M253-B002)"),
    SnippetCheck("M253-B002-NDOC-02", "`!objc3.objc_runtime_metadata_layout_policy`"),
    SnippetCheck("M253-B002-NDOC-03", "`; runtime_metadata_layout_policy =`"),
    SnippetCheck("M253-B002-NDOC-04", "`check:objc3c:m253-b002-lane-b-readiness`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B002-SPC-01", "## M253 deterministic ordering, visibility, and relocation semantics (B002)"),
    SnippetCheck("M253-B002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-B002-SPC-03", "normalizes one metadata layout policy before IR emission"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B002-META-01", "## M253 normalized metadata layout policy anchors (B002)"),
    SnippetCheck("M253-B002-META-02", "`!objc3.objc_runtime_metadata_layout_policy`"),
    SnippetCheck("M253-B002-META-03", "`; runtime_metadata_layout_policy =`"),
)

LOWERING_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B002-LHDR-01", "kObjc3RuntimeMetadataLayoutPolicyContractId"),
    SnippetCheck("M253-B002-LHDR-02", "struct Objc3RuntimeMetadataLayoutPolicyInput"),
    SnippetCheck("M253-B002-LHDR-03", "struct Objc3RuntimeMetadataLayoutPolicy {"),
    SnippetCheck("M253-B002-LHDR-04", "kObjc3RuntimeMetadataLayoutPolicyClassFamily"),
)

LOWERING_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B002-LCPP-01", "TryBuildObjc3RuntimeMetadataLayoutPolicy("),
    SnippetCheck("M253-B002-LCPP-02", "runtime metadata layout policy requires descriptor linkage private"),
    SnippetCheck("M253-B002-LCPP-03", "Objc3RuntimeMetadataLayoutPolicyReplayKey("),
    SnippetCheck("M253-B002-LCPP-04", "M253-B002 normalized layout policy anchor"),
)

IR_EMITTER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B002-IR-01", POLICY_METADATA_NAME),
    SnippetCheck("M253-B002-IR-02", "TryBuildRuntimeMetadataLayoutPolicy(layout_policy, layout_policy_error)"),
    SnippetCheck("M253-B002-IR-03", "for (const auto &family : layout_policy.families)"),
    SnippetCheck("M253-B002-IR-04", 'out << "; runtime_metadata_layout_policy = "'),
)

PROCESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-B002-PROC-01", "M253-B002 normalized layout policy anchor"),
    SnippetCheck("M253-B002-PROC-02", "normalized metadata layout replay key"),
    SnippetCheck("M253-B002-PROC-03", "semantic finalization boundary"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M253-B002-PKG-01",
        '"check:objc3c:m253-b002-deterministic-ordering-visibility-and-relocation-semantics-core-feature-implementation": "python scripts/check_m253_b002_deterministic_ordering_visibility_and_relocation_semantics_core_feature_implementation.py"',
    ),
    SnippetCheck(
        "M253-B002-PKG-02",
        '"test:tooling:m253-b002-deterministic-ordering-visibility-and-relocation-semantics-core-feature-implementation": "python -m pytest tests/tooling/test_check_m253_b002_deterministic_ordering_visibility_and_relocation_semantics_core_feature_implementation.py -q"',
    ),
    SnippetCheck(
        "M253-B002-PKG-03",
        '"check:objc3c:m253-b002-lane-b-readiness": "npm run check:objc3c:m252-b004-lane-b-readiness && npm run check:objc3c:m253-b001-lane-b-readiness && npm run build:objc3c-native && npm run check:objc3c:m253-b002-deterministic-ordering-visibility-and-relocation-semantics-core-feature-implementation && npm run test:tooling:m253-b002-deterministic-ordering-visibility-and-relocation-semantics-core-feature-implementation"',
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


def locate_sema(payload: dict[str, Any]) -> dict[str, Any] | None:
    current: Any = payload
    for key in ("frontend", "pipeline", "sema_pass_manager"):
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
        "case_id": "M253-B002-CASE-RUNNER",
        "command": command,
        "process_exit_code": result.returncode,
        "summary_path": display_path(summary_path),
        "manifest_path": display_path(manifest_path),
    }

    checks_total += 5
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M253-B002-RUNNER-EXISTS", "frontend C API runner binary is missing", failures)
    checks_passed += require(args.class_fixture.exists(), display_path(args.class_fixture), "M253-B002-RUNNER-FIXTURE", "class fixture is missing", failures)
    checks_passed += require(result.returncode == 0, display_path(summary_path), "M253-B002-RUNNER-STATUS", "runner probe must exit 0", failures)
    checks_passed += require(summary_path.exists(), display_path(summary_path), "M253-B002-RUNNER-SUMMARY", "runner probe must write module.c_api_summary.json", failures)
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M253-B002-RUNNER-MANIFEST", "runner probe must write module.manifest.json", failures)
    if not manifest_path.exists():
        return checks_total, checks_passed, case

    manifest_payload = load_json(manifest_path)
    sema = locate_sema(manifest_payload if isinstance(manifest_payload, dict) else {})
    case["sema_present"] = sema is not None
    checks_total += 11
    checks_passed += require(isinstance(sema, dict), display_path(manifest_path), "M253-B002-RUNNER-SEMA", "sema pass manager surface missing", failures)
    if isinstance(sema, dict):
        case["runtime_export_ready_for_runtime_export"] = sema.get("runtime_export_ready_for_runtime_export")
        case["runtime_metadata_section_scaffold_fail_closed"] = sema.get("runtime_metadata_section_scaffold_fail_closed")
        case["runtime_metadata_section_descriptor_linkage"] = sema.get("runtime_metadata_section_descriptor_linkage")
        case["runtime_metadata_section_aggregate_linkage"] = sema.get("runtime_metadata_section_aggregate_linkage")
        case["runtime_metadata_section_visibility"] = sema.get("runtime_metadata_section_visibility")
        case["runtime_metadata_section_retention_root"] = sema.get("runtime_metadata_section_retention_root")
        case["runtime_metadata_section_scaffold_class_descriptor_count"] = sema.get("runtime_metadata_section_scaffold_class_descriptor_count")
        case["runtime_metadata_section_scaffold_protocol_descriptor_count"] = sema.get("runtime_metadata_section_scaffold_protocol_descriptor_count")
        case["runtime_metadata_section_scaffold_property_descriptor_count"] = sema.get("runtime_metadata_section_scaffold_property_descriptor_count")
        case["runtime_metadata_section_scaffold_ivar_descriptor_count"] = sema.get("runtime_metadata_section_scaffold_ivar_descriptor_count")
        case["runtime_metadata_section_scaffold_total_descriptor_count"] = sema.get("runtime_metadata_section_scaffold_total_descriptor_count")
        case["runtime_metadata_section_scaffold_total_retained_global_count"] = sema.get("runtime_metadata_section_scaffold_total_retained_global_count")

        checks_passed += require(sema.get("runtime_export_ready_for_runtime_export") is True, display_path(manifest_path), "M253-B002-RUNNER-RUNTIME-EXPORT-READY", "runtime export must be ready for runtime export", failures)
        checks_passed += require(sema.get("runtime_metadata_section_scaffold_fail_closed") is True, display_path(manifest_path), "M253-B002-RUNNER-FAIL-CLOSED", "runtime metadata scaffold must stay fail-closed", failures)
        checks_passed += require(sema.get("runtime_metadata_section_descriptor_linkage") == "private", display_path(manifest_path), "M253-B002-RUNNER-DESCRIPTOR-LINKAGE", "descriptor linkage drifted", failures)
        checks_passed += require(sema.get("runtime_metadata_section_aggregate_linkage") == "internal", display_path(manifest_path), "M253-B002-RUNNER-AGGREGATE-LINKAGE", "aggregate linkage drifted", failures)
        checks_passed += require(sema.get("runtime_metadata_section_visibility") == "hidden", display_path(manifest_path), "M253-B002-RUNNER-VISIBILITY", "metadata visibility drifted", failures)
        checks_passed += require(sema.get("runtime_metadata_section_retention_root") == "llvm.used", display_path(manifest_path), "M253-B002-RUNNER-RETENTION", "retention root drifted", failures)
        checks_passed += require(int(sema.get("runtime_metadata_section_scaffold_class_descriptor_count", 0)) > 0, display_path(manifest_path), "M253-B002-RUNNER-CLASS-COUNT", "class descriptor count must be non-zero", failures)
        checks_passed += require(int(sema.get("runtime_metadata_section_scaffold_protocol_descriptor_count", 0)) > 0, display_path(manifest_path), "M253-B002-RUNNER-PROTOCOL-COUNT", "protocol descriptor count must be non-zero", failures)
        checks_passed += require(int(sema.get("runtime_metadata_section_scaffold_property_descriptor_count", 0)) > 0, display_path(manifest_path), "M253-B002-RUNNER-PROPERTY-COUNT", "property descriptor count must be non-zero", failures)
        checks_passed += require(int(sema.get("runtime_metadata_section_scaffold_ivar_descriptor_count", 0)) > 0, display_path(manifest_path), "M253-B002-RUNNER-IVAR-COUNT", "ivar descriptor count must be non-zero", failures)
        total_descriptor_count = int(sema.get("runtime_metadata_section_scaffold_total_descriptor_count", 0))
        total_retained = int(sema.get("runtime_metadata_section_scaffold_total_retained_global_count", 0))
        checks_passed += require(total_retained == total_descriptor_count + 6, display_path(manifest_path), "M253-B002-RUNNER-RETENTION-COUNT", "retained-global count must match descriptor inventory", failures)
    else:
        checks_total += 11

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
        "case_id": "M253-B002-CASE-NATIVE",
        "command": command,
        "process_exit_code": result.returncode,
        "ir_path": display_path(ir_path),
        "object_path": display_path(obj_path),
    }

    checks_total += 5
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M253-B002-NATIVE-EXISTS", "objc3c-native.exe is missing", failures)
    checks_passed += require(args.hello_fixture.exists(), display_path(args.hello_fixture), "M253-B002-NATIVE-FIXTURE", "hello fixture is missing", failures)
    checks_passed += require(result.returncode == 0, display_path(ir_path), "M253-B002-NATIVE-STATUS", "native probe must exit 0", failures)
    checks_passed += require(ir_path.exists(), display_path(ir_path), "M253-B002-NATIVE-IR", "native probe must emit module.ll", failures)
    checks_passed += require(obj_path.exists(), display_path(obj_path), "M253-B002-NATIVE-OBJ", "native probe must emit module.obj", failures)
    if not ir_path.exists():
        return checks_total, checks_passed, case

    ir_text = read_text(ir_path)
    policy_line = next((line for line in ir_text.splitlines() if line.startswith(POLICY_COMMENT_PREFIX)), "")
    llvm_used_line = next((line for line in ir_text.splitlines() if line.startswith("@llvm.used = appending global [")), "")
    case["policy_line"] = policy_line
    case["llvm_used_line"] = llvm_used_line
    checks_total += 15
    checks_passed += require(POLICY_METADATA_NAME in ir_text, display_path(ir_path), "M253-B002-NATIVE-NAMED-METADATA", "IR must publish the named metadata anchor", failures)
    checks_passed += require("!55 = !{!\"objc3c-runtime-metadata-layout-policy/m253-b002-v1\"" in ir_text, display_path(ir_path), "M253-B002-NATIVE-METADATA-NODE", "IR must publish the !55 layout-policy node", failures)
    checks_passed += require(bool(policy_line), display_path(ir_path), "M253-B002-NATIVE-POLICY-COMMENT", "IR must publish the normalized layout-policy comment", failures)
    checks_passed += require("family=class|objc3.runtime.class_descriptors|__objc3_sec_class_descriptors|0" in policy_line, display_path(ir_path), "M253-B002-NATIVE-CLASS-FAMILY", "policy comment must include the normalized class family", failures)
    checks_passed += require("family=ivar|objc3.runtime.ivar_descriptors|__objc3_sec_ivar_descriptors|0" in policy_line, display_path(ir_path), "M253-B002-NATIVE-IVAR-FAMILY", "policy comment must include the normalized ivar family", failures)
    checks_passed += require("aggregate_relocation=zero-sentinel-or-count-plus-pointer-vector" in policy_line, display_path(ir_path), "M253-B002-NATIVE-RELOCATION", "policy comment must include the relocation model", failures)
    checks_passed += require("visibility_spelling=local-linkage-omits-explicit-ir-visibility" in policy_line, display_path(ir_path), "M253-B002-NATIVE-VISIBILITY-SPELLING", "policy comment must include the visibility spelling model", failures)
    checks_passed += require("retention_root=llvm.used" in policy_line, display_path(ir_path), "M253-B002-NATIVE-RETENTION-ROOT", "policy comment must include the retention root", failures)
    checks_passed += require("!\"zero-sentinel-or-count-plus-pointer-vector\"" in ir_text, display_path(ir_path), "M253-B002-NATIVE-METADATA-RELOCATION", "!55 must preserve the relocation token", failures)
    checks_passed += require("!\"local-linkage-omits-explicit-ir-visibility\"" in ir_text, display_path(ir_path), "M253-B002-NATIVE-METADATA-VISIBILITY", "!55 must preserve the visibility spelling token", failures)
    checks_passed += require("!\"llvm.used\"" in ir_text, display_path(ir_path), "M253-B002-NATIVE-METADATA-RETENTION", "!55 must preserve the retention token", failures)
    checks_passed += require("@__objc3_sec_class_descriptors = internal global { i64 } { i64 0 }, section \"objc3.runtime.class_descriptors\", align 8" in ir_text or "@__objc3_sec_class_descriptors = internal constant { i64 } { i64 0 }, section \"objc3.runtime.class_descriptors\", align 8" in ir_text, display_path(ir_path), "M253-B002-NATIVE-ZERO-SENTINEL", "aggregate zero-sentinel payload must stay intact", failures)
    checks_passed += require(bool(llvm_used_line), display_path(ir_path), "M253-B002-NATIVE-LLVM-USED", "IR must contain @llvm.used retention root", failures)
    checks_passed += require(obj_path.stat().st_size > 0 if obj_path.exists() else False, display_path(obj_path), "M253-B002-NATIVE-OBJ-SIZE", "module.obj must be non-empty", failures)

    symbol_positions: dict[str, int] = {}
    metadata_lines = {
        symbol: next((line for line in ir_text.splitlines() if line.startswith(f"@{symbol} =")), "")
        for symbol in FAMILY_ORDER
    }
    case["contains_metadata_comdat"] = any(" comdat" in line for line in metadata_lines.values())
    for symbol in FAMILY_ORDER:
        token = f"@{symbol} ="
        position = ir_text.find(token)
        symbol_positions[symbol] = position
        checks_total += 1
        checks_passed += require(position >= 0, display_path(ir_path), f"M253-B002-NATIVE-SYMBOL-{symbol}", f"missing metadata global {symbol}", failures)
    case["symbol_positions"] = symbol_positions
    checks_total += 2
    if all(position >= 0 for position in symbol_positions.values()):
        positions = [symbol_positions[symbol] for symbol in FAMILY_ORDER]
        checks_passed += require(positions == sorted(positions), display_path(ir_path), "M253-B002-NATIVE-FAMILY-ORDER", "metadata globals are out of normalized family order", failures)
    else:
        checks_passed += 0
    checks_passed += require(not case["contains_metadata_comdat"], display_path(ir_path), "M253-B002-NATIVE-COMDAT", "metadata globals must not spell COMDAT", failures)
    checks_passed += require(all("hidden" not in line for line in metadata_lines.values()), display_path(ir_path), "M253-B002-NATIVE-HIDDEN", "metadata globals must not spell explicit hidden visibility", failures)
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
        checks_passed += require(order_ok, display_path(ir_path), "M253-B002-NATIVE-LLVM-USED-ORDER", "llvm.used retention order mismatch", failures)
    else:
        checks_total += 1

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
        "policy_named_metadata": POLICY_METADATA_NAME,
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

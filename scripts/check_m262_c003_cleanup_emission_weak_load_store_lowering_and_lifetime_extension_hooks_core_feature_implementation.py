#!/usr/bin/env python3
"""Fail-closed contract checker for M262-C003 ARC cleanup/weak/lifetime hooks."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_ID = "objc3c-arc-cleanup-weak-lifetime-hooks/m262-c003-v1"
MODE = "m262-c003-cleanup-emission-weak-load-store-lowering-and-lifetime-extension-hooks-core-feature-implementation-v1"
BOUNDARY_COMMENT_PREFIX = "; arc_cleanup_weak_lifetime_hooks = contract=objc3c-arc-cleanup-weak-lifetime-hooks/m262-c003-v1"
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m262_cleanup_emission_weak_load_store_lowering_and_lifetime_extension_hooks_core_feature_implementation_c003_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m262" / "m262_c003_cleanup_emission_weak_load_store_lowering_and_lifetime_extension_hooks_core_feature_implementation_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_NATIVE_DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_SYNTAX_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
DEFAULT_SCAFFOLD_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_ownership_aware_lowering_behavior_scaffold.h"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_PROPERTY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_property_interaction_positive.objc3"
DEFAULT_SCOPE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_cleanup_scope_positive.objc3"
DEFAULT_IMPLICIT_VOID_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_implicit_cleanup_void_positive.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m262" / "c003-cleanup-weak-lifetime"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m262/M262-C003/arc_cleanup_weak_lifetime_hooks_summary.json")


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
    SnippetCheck("M262-C003-DOC-EXP-01", "# M262 Cleanup Emission Weak Load-Store Lowering And Lifetime Extension Hooks Core Feature Implementation Expectations (C003)"),
    SnippetCheck("M262-C003-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M262-C003-DOC-EXP-03", "`; arc_cleanup_weak_lifetime_hooks = ...`"),
    SnippetCheck("M262-C003-DOC-EXP-04", "`!objc3.objc_arc_cleanup_weak_lifetime_hooks = !{...}`"),
    SnippetCheck("M262-C003-DOC-EXP-05", "`tmp/reports/m262/M262-C003/arc_cleanup_weak_lifetime_hooks_summary.json`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M262-C003-DOC-PKT-01", "# M262-C003 Cleanup Emission Weak Load-Store Lowering And Lifetime Extension Hooks Core Feature Implementation Packet"),
    SnippetCheck("M262-C003-DOC-PKT-02", "Packet: `M262-C003`"),
    SnippetCheck("M262-C003-DOC-PKT-03", "Issue: `#7201`"),
    SnippetCheck("M262-C003-DOC-PKT-04", "tests/tooling/fixtures/native/m262_arc_cleanup_scope_positive.objc3"),
    SnippetCheck("M262-C003-DOC-PKT-05", "tests/tooling/fixtures/native/m262_arc_implicit_cleanup_void_positive.objc3"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M262-C003-ARCH-01", "## M262 ARC Cleanup Emission Weak Load-Store Lowering And Lifetime Extension Hooks (C003)"),
    SnippetCheck("M262-C003-ARCH-02", "scope-exit cleanup scheduling, implicit-exit cleanup"),
    SnippetCheck("M262-C003-ARCH-03", "`!objc3.objc_arc_cleanup_weak_lifetime_hooks`"),
)
NATIVE_DOC_SOURCE_SNIPPETS = (
    SnippetCheck("M262-C003-NSRC-01", "## M262 ARC cleanup emission weak load-store lowering and lifetime extension hooks (M262-C003)"),
    SnippetCheck("M262-C003-NSRC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M262-C003-NSRC-03", "`objc3_runtime_load_weak_current_property_i32`"),
    SnippetCheck("M262-C003-NSRC-04", "`M262-C004` is the next issue."),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M262-C003-NDOC-01", "## M262 ARC cleanup emission weak load-store lowering and lifetime extension hooks (M262-C003)"),
    SnippetCheck("M262-C003-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M262-C003-NDOC-03", "`!objc3.objc_arc_cleanup_weak_lifetime_hooks`"),
    SnippetCheck("M262-C003-NDOC-04", "`M262-C004` is the next issue."),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M262-C003-SPC-01", "## M262 ARC cleanup emission weak load-store lowering and lifetime extension hooks (C003)"),
    SnippetCheck("M262-C003-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M262-C003-SPC-03", "`M262-C004` is the next issue."),
)
SYNTAX_SPEC_SNIPPETS = (
    SnippetCheck("M262-C003-SYN-01", "`M262-C003` now covers the supported cleanup-emission, weak current-property,"),
    SnippetCheck("M262-C003-SYN-02", "block-capture lifetime-extension lowering slice for explicit ARC mode"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M262-C003-LHDR-01", "kObjc3ArcCleanupWeakLifetimeHooksContractId"),
    SnippetCheck("M262-C003-LHDR-02", "kObjc3ArcCleanupWeakLifetimeHooksSourceModel"),
    SnippetCheck("M262-C003-LHDR-03", "kObjc3ArcCleanupWeakLifetimeHooksLoweringModel"),
    SnippetCheck("M262-C003-LHDR-04", "Objc3ArcCleanupWeakLifetimeHooksSummary()"),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M262-C003-LCPP-01", "Objc3ArcCleanupWeakLifetimeHooksSummary()"),
    SnippetCheck("M262-C003-LCPP-02", "M262-C003 ARC cleanup/weak/lifetime lowering anchor"),
    SnippetCheck("M262-C003-LCPP-03", ";next_issue=M262-C004"),
)
SEMA_SNIPPETS = (
    SnippetCheck("M262-C003-SEMA-01", "M262-C003 ARC cleanup/weak/lifetime lowering anchor:"),
)
SEMA_PM_SNIPPETS = (
    SnippetCheck("M262-C003-PM-01", "M262-C003 ARC cleanup/weak/lifetime implementation anchor:"),
)
SCAFFOLD_SNIPPETS = (
    SnippetCheck("M262-C003-SCAF-01", "M262-C003 ARC cleanup/weak/lifetime implementation anchor:"),
)
IR_EMITTER_SNIPPETS = (
    SnippetCheck("M262-C003-IR-01", 'out << "; arc_cleanup_weak_lifetime_hooks = "'),
    SnippetCheck("M262-C003-IR-02", "M262-C003 ARC cleanup/weak/lifetime implementation anchor:"),
    SnippetCheck("M262-C003-IR-03", "!objc3.objc_arc_cleanup_weak_lifetime_hooks = !{!81}"),
    SnippetCheck("M262-C003-IR-04", "EmitPendingBlockDisposeUnwindToDepth"),
    SnippetCheck("M262-C003-IR-05", "EmitArcOwnedCleanupUnwindToDepth"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M262-C003-PKG-01", '"check:objc3c:m262-c003-cleanup-weak-lifetime-hooks-contract": "python scripts/check_m262_c003_cleanup_emission_weak_load_store_lowering_and_lifetime_extension_hooks_core_feature_implementation.py"'),
    SnippetCheck("M262-C003-PKG-02", '"test:tooling:m262-c003-cleanup-weak-lifetime-hooks-contract": "python -m pytest tests/tooling/test_check_m262_c003_cleanup_emission_weak_load_store_lowering_and_lifetime_extension_hooks_core_feature_implementation.py -q"'),
    SnippetCheck("M262-C003-PKG-03", '"check:objc3c:m262-c003-lane-c-readiness": "python scripts/run_m262_c003_lane_c_readiness.py"'),
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
    parser.add_argument("--native-doc-source", type=Path, default=DEFAULT_NATIVE_DOC_SOURCE)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--syntax-spec", type=Path, default=DEFAULT_SYNTAX_SPEC)
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=DEFAULT_LOWERING_CPP)
    parser.add_argument("--sema-cpp", type=Path, default=DEFAULT_SEMA_CPP)
    parser.add_argument("--sema-pass-manager", type=Path, default=DEFAULT_SEMA_PASS_MANAGER)
    parser.add_argument("--scaffold-header", type=Path, default=DEFAULT_SCAFFOLD_HEADER)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--property-fixture", type=Path, default=DEFAULT_PROPERTY_FIXTURE)
    parser.add_argument("--scope-fixture", type=Path, default=DEFAULT_SCOPE_FIXTURE)
    parser.add_argument("--implicit-void-fixture", type=Path, default=DEFAULT_IMPLICIT_VOID_FIXTURE)
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


def extract_function_body(ir_text: str, name: str, return_type: str) -> str:
    pattern = rf"define {re.escape(return_type)} @{re.escape(name)}\([^)]*\) \{{\n(?P<body>.*?)\n\}}"
    match = re.search(pattern, ir_text, flags=re.DOTALL)
    return match.group("body") if match else ""


def run_probe(*, args: argparse.Namespace, fixture: Path, out_leaf: str) -> tuple[subprocess.CompletedProcess[str], Path, Path]:
    probe_dir = args.probe_root.resolve() / out_leaf
    command = [
        str(args.native_exe.resolve()),
        str(fixture.resolve()),
        "--out-dir",
        str(probe_dir),
        "--emit-prefix",
        "module",
        "-fobjc-arc",
    ]
    result = run_command(command, ROOT)
    return result, probe_dir / "module.ll", probe_dir / "module.obj"


def require_boundary(ir_text: str, ir_path: Path, failures: list[Finding], prefix_check_id: str) -> tuple[int, int, str]:
    checks_total = checks_passed = 0
    boundary_line = next((line for line in ir_text.splitlines() if line.startswith(BOUNDARY_COMMENT_PREFIX)), "")
    checks_total += 1
    checks_passed += require(bool(boundary_line), display_path(ir_path), f"{prefix_check_id}-BOUNDARY", "IR must publish the ARC cleanup/weak/lifetime boundary line", failures)
    checks_total += 1
    checks_passed += require("!objc3.objc_arc_cleanup_weak_lifetime_hooks = !{" in ir_text, display_path(ir_path), f"{prefix_check_id}-METADATA", "IR must publish ARC cleanup/weak/lifetime named metadata", failures)
    return checks_total, checks_passed, boundary_line


def validate_property_case(*, args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    result, ir_path, obj_path = run_probe(args=args, fixture=args.property_fixture, out_leaf="property-interaction")
    case: dict[str, Any] = {
        "case_id": "M262-C003-CASE-PROPERTY",
        "fixture": display_path(args.property_fixture),
        "process_exit_code": result.returncode,
        "ir_path": display_path(ir_path),
        "object_path": display_path(obj_path),
    }
    checks_total = checks_passed = 0
    checks_total += 1; checks_passed += require(result.returncode == 0, display_path(ir_path), "M262-C003-PROP-STATUS", "property probe must exit 0", failures)
    checks_total += 1; checks_passed += require(ir_path.exists(), display_path(ir_path), "M262-C003-PROP-IR", "property probe must emit module.ll", failures)
    checks_total += 1; checks_passed += require(obj_path.exists(), display_path(obj_path), "M262-C003-PROP-OBJ", "property probe must emit module.obj", failures)
    if not ir_path.exists():
        case["stdout"] = result.stdout
        case["stderr"] = result.stderr
        return checks_total, checks_passed, case
    ir_text = read_text(ir_path)
    extra_total, extra_passed, boundary_line = require_boundary(ir_text, ir_path, failures, "M262-C003-PROP")
    checks_total += extra_total
    checks_passed += extra_passed
    case["boundary_line"] = boundary_line
    checks_total += 1; checks_passed += require("call i32 @objc3_runtime_load_weak_current_property_i32()" in ir_text, display_path(ir_path), "M262-C003-PROP-WEAK-LOAD", "property fixture must lower the weak current-property load helper", failures)
    checks_total += 1; checks_passed += require("call void @objc3_runtime_store_weak_current_property_i32(i32 %arg0)" in ir_text, display_path(ir_path), "M262-C003-PROP-WEAK-STORE", "property fixture must lower the weak current-property store helper", failures)
    checks_total += 1; checks_passed += require(obj_path.stat().st_size > 0 if obj_path.exists() else False, display_path(obj_path), "M262-C003-PROP-OBJ-SIZE", "property object output must be non-empty", failures)
    return checks_total, checks_passed, case


def validate_scope_case(*, args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    result, ir_path, obj_path = run_probe(args=args, fixture=args.scope_fixture, out_leaf="cleanup-scope")
    case: dict[str, Any] = {
        "case_id": "M262-C003-CASE-SCOPE",
        "fixture": display_path(args.scope_fixture),
        "process_exit_code": result.returncode,
        "ir_path": display_path(ir_path),
        "object_path": display_path(obj_path),
    }
    checks_total = checks_passed = 0
    checks_total += 1; checks_passed += require(result.returncode == 0, display_path(ir_path), "M262-C003-SCOPE-STATUS", "cleanup-scope probe must exit 0", failures)
    checks_total += 1; checks_passed += require(ir_path.exists(), display_path(ir_path), "M262-C003-SCOPE-IR", "cleanup-scope probe must emit module.ll", failures)
    checks_total += 1; checks_passed += require(obj_path.exists(), display_path(obj_path), "M262-C003-SCOPE-OBJ", "cleanup-scope probe must emit module.obj", failures)
    if not ir_path.exists():
        case["stdout"] = result.stdout
        case["stderr"] = result.stderr
        return checks_total, checks_passed, case
    ir_text = read_text(ir_path)
    extra_total, extra_passed, boundary_line = require_boundary(ir_text, ir_path, failures, "M262-C003-SCOPE")
    checks_total += extra_total
    checks_passed += extra_passed
    case["boundary_line"] = boundary_line
    helper_body = extract_function_body(ir_text, "helper", "i32")
    case["helper_body"] = helper_body
    checks_total += 1; checks_passed += require(bool(helper_body), display_path(ir_path), "M262-C003-SCOPE-HELPER-BODY", "cleanup-scope fixture must emit helper body", failures)
    checks_total += 1; checks_passed += require(bool(re.search(r"call void @__objc3_block_dispose_helper_[^(]+\(ptr %block\.literal\.addr\.\d+\)\n\s+br label %if_end_", helper_body)), display_path(ir_path), "M262-C003-SCOPE-DISPOSE-BEFORE-MERGE", "scope cleanup must dispose the block helper before the merge branch", failures)
    checks_total += 1; checks_passed += require(bool(re.search(r"call i32 @objc3_runtime_release_i32\(i32 %t\d+\)\n\s+ret i32", helper_body)), display_path(ir_path), "M262-C003-SCOPE-RETURN-RELEASE", "scope cleanup must release tracked ARC-owned storage before the final return", failures)
    checks_total += 1; checks_passed += require(obj_path.stat().st_size > 0 if obj_path.exists() else False, display_path(obj_path), "M262-C003-SCOPE-OBJ-SIZE", "cleanup-scope object output must be non-empty", failures)
    return checks_total, checks_passed, case


def validate_implicit_void_case(*, args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    result, ir_path, obj_path = run_probe(args=args, fixture=args.implicit_void_fixture, out_leaf="implicit-void")
    case: dict[str, Any] = {
        "case_id": "M262-C003-CASE-IMPLICIT-VOID",
        "fixture": display_path(args.implicit_void_fixture),
        "process_exit_code": result.returncode,
        "ir_path": display_path(ir_path),
        "object_path": display_path(obj_path),
    }
    checks_total = checks_passed = 0
    checks_total += 1; checks_passed += require(result.returncode == 0, display_path(ir_path), "M262-C003-VOID-STATUS", "implicit-void probe must exit 0", failures)
    checks_total += 1; checks_passed += require(ir_path.exists(), display_path(ir_path), "M262-C003-VOID-IR", "implicit-void probe must emit module.ll", failures)
    checks_total += 1; checks_passed += require(obj_path.exists(), display_path(obj_path), "M262-C003-VOID-OBJ", "implicit-void probe must emit module.obj", failures)
    if not ir_path.exists():
        case["stdout"] = result.stdout
        case["stderr"] = result.stderr
        return checks_total, checks_passed, case
    ir_text = read_text(ir_path)
    extra_total, extra_passed, boundary_line = require_boundary(ir_text, ir_path, failures, "M262-C003-VOID")
    checks_total += extra_total
    checks_passed += extra_passed
    case["boundary_line"] = boundary_line
    sink_body = extract_function_body(ir_text, "sink", "void")
    case["sink_body"] = sink_body
    checks_total += 1; checks_passed += require(bool(sink_body), display_path(ir_path), "M262-C003-VOID-SINK-BODY", "implicit-void fixture must emit sink body", failures)
    checks_total += 1; checks_passed += require(bool(re.search(r"call void @__objc3_block_dispose_helper_[^(]+\(ptr %block\.literal\.addr\.\d+\)\n\s+%t\d+ = load i32, ptr %value\.addr\.0, align 4\n\s+%t\d+ = call i32 @objc3_runtime_release_i32\(i32 %t\d+\)\n\s+ret void", sink_body)), display_path(ir_path), "M262-C003-VOID-IMPLICIT-CLEANUP", "implicit void exit must dispose the block helper and release tracked ARC storage before ret void", failures)
    checks_total += 1; checks_passed += require(obj_path.stat().st_size > 0 if obj_path.exists() else False, display_path(obj_path), "M262-C003-VOID-OBJ-SIZE", "implicit-void object output must be non-empty", failures)
    return checks_total, checks_passed, case


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = checks_passed = 0

    static_targets = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.native_doc_source, NATIVE_DOC_SOURCE_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.syntax_spec, SYNTAX_SPEC_SNIPPETS),
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.lowering_cpp, LOWERING_CPP_SNIPPETS),
        (args.sema_cpp, SEMA_SNIPPETS),
        (args.sema_pass_manager, SEMA_PM_SNIPPETS),
        (args.scaffold_header, SCAFFOLD_SNIPPETS),
        (args.ir_emitter, IR_EMITTER_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in static_targets:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, Any]] = []
    if not args.skip_dynamic_probes:
        for validator in (validate_property_case, validate_scope_case, validate_implicit_void_case):
            case_total, case_passed, case_payload = validator(args=args, failures=failures)
            checks_total += case_total
            checks_passed += case_passed
            dynamic_cases.append(case_payload)

    summary = {
        "ok": not failures,
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "checks_failed": len(failures),
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_cases": dynamic_cases,
        "failures": [finding.__dict__ for finding in failures],
    }
    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        print(f"[info] wrote summary to {display_path(summary_path)}", file=sys.stderr)
        return 1
    print(f"[ok] M262-C003 ARC cleanup/weak/lifetime contract satisfied")
    print(f"[info] wrote summary to {display_path(summary_path)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

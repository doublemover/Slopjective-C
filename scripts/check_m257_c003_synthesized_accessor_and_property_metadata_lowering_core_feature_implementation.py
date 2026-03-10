#!/usr/bin/env python3
"""Fail-closed contract checker for M257-C003 synthesized accessor/property lowering."""

from __future__ import annotations

import argparse
import json
import re
import shutil
import subprocess
import sys
import uuid
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m257-c003-synthesized-accessor-and-property-metadata-lowering-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-executable-synthesized-accessor-property-lowering/m257-c003-v1"
BOUNDARY_COMMENT_PREFIX = "; executable_synthesized_accessor_property_lowering = contract=objc3c-executable-synthesized-accessor-property-lowering/m257-c003-v1"
NAMED_METADATA_PREFIX = "!objc3.objc_executable_synthesized_accessor_property_lowering = !{"
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m257_synthesized_accessor_and_property_metadata_lowering_core_feature_implementation_c003_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m257" / "m257_c003_synthesized_accessor_and_property_metadata_lowering_core_feature_implementation_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
DEFAULT_SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
DEFAULT_RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_synthesized_accessor_property_lowering_positive.objc3"
DEFAULT_RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m257_c003_synthesized_accessor_probe.cpp"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m257" / "synthesized-accessor-property-lowering"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m257/M257-C003/synthesized_accessor_property_lowering_summary.json")
DEFAULT_CLANGXX = "clang++"

STORAGE_GLOBAL_RE = re.compile(r"^@objc3_property_storage_[A-Za-z0-9_]+ = private global i32 0, align 4$", re.MULTILINE)


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
    SnippetCheck("M257-C003-DOC-EXP-01", "# M257 Synthesized Accessor And Property Metadata Lowering Core Feature Implementation Expectations (C003)"),
    SnippetCheck("M257-C003-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M257-C003-DOC-EXP-03", "`tests/tooling/fixtures/native/m257_synthesized_accessor_property_lowering_positive.objc3`"),
    SnippetCheck("M257-C003-DOC-EXP-04", "`tests/tooling/runtime/m257_c003_synthesized_accessor_probe.cpp`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C003-DOC-PKT-01", "# M257-C003 Synthesized Accessor And Property Metadata Lowering Core Feature Implementation Packet"),
    SnippetCheck("M257-C003-DOC-PKT-02", "Issue: `#7152`"),
    SnippetCheck("M257-C003-DOC-PKT-03", "`M257-C003`"),
    SnippetCheck("M257-C003-DOC-PKT-04", "`tests/tooling/fixtures/native/m257_synthesized_accessor_property_lowering_positive.objc3`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C003-ARCH-01", "## M257 synthesized accessor and property metadata lowering (C003)"),
    SnippetCheck("M257-C003-ARCH-02", "!objc3.objc_executable_synthesized_accessor_property_lowering"),
    SnippetCheck("M257-C003-ARCH-03", "check:objc3c:m257-c003-lane-c-readiness"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C003-NDOC-01", "## Synthesized accessor and property metadata lowering (M257-C003)"),
    SnippetCheck("M257-C003-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M257-C003-NDOC-03", "`@objc3_property_storage_...`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C003-SPC-01", "## M257 synthesized accessor and property metadata lowering (C003)"),
    SnippetCheck("M257-C003-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M257-C003-SPC-03", "`one-private-i32-storage-global-per-synthesized-binding-symbol-pending-runtime-instance-layout`"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C003-META-01", "## M257 synthesized accessor property metadata anchors (C003)"),
    SnippetCheck("M257-C003-META-02", "`effective_getter_selector`"),
    SnippetCheck("M257-C003-META-03", "`setter_implementation_pointer`"),
)

AST_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C003-AST-01", "M257-C003 synthesized accessor/property lowering anchor"),
)

SEMA_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C003-SEMA-01", "M257-C003 synthesized accessor/property lowering anchor"),
)

IR_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C003-IR-01", 'out << "; executable_synthesized_accessor_property_lowering = "'),
    SnippetCheck("M257-C003-IR-02", '!objc3.objc_executable_synthesized_accessor_property_lowering = !{!68}'),
    SnippetCheck("M257-C003-IR-03", 'define void @'),
    SnippetCheck("M257-C003-IR-04", 'objc3_property_storage_'),
)

ARTIFACTS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C003-ART-01", 'effective_getter_selector'),
    SnippetCheck("M257-C003-ART-02", 'effective_setter_selector'),
    SnippetCheck("M257-C003-ART-03", 'executable_synthesized_binding_symbol'),
)

LOWERING_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C003-LHDR-01", "kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId"),
    SnippetCheck("M257-C003-LHDR-02", "kObjc3ExecutableSynthesizedAccessorPropertyLoweringStorageModel"),
    SnippetCheck("M257-C003-LHDR-03", "Objc3ExecutableSynthesizedAccessorPropertyLoweringSummary()"),
)

LOWERING_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C003-LCPP-01", "Objc3ExecutableSynthesizedAccessorPropertyLoweringSummary()"),
    SnippetCheck("M257-C003-LCPP-02", "M257-C003 synthesized accessor/property lowering anchor"),
)

RUNTIME_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C003-RT-01", "enum class RuntimeMethodReturnKind"),
    SnippetCheck("M257-C003-RT-02", "RuntimeMethodReturnKind ClassifyRuntimeReturnType"),
    SnippetCheck("M257-C003-RT-03", "case RuntimeMethodReturnKind::Void:"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M257-C003-PKG-01", '"check:objc3c:m257-c003-synthesized-accessor-and-property-metadata-lowering": "python scripts/check_m257_c003_synthesized_accessor_and_property_metadata_lowering_core_feature_implementation.py"'),
    SnippetCheck("M257-C003-PKG-02", '"test:tooling:m257-c003-synthesized-accessor-and-property-metadata-lowering": "python -m pytest tests/tooling/test_check_m257_c003_synthesized_accessor_and_property_metadata_lowering_core_feature_implementation.py -q"'),
    SnippetCheck("M257-C003-PKG-03", '"check:objc3c:m257-c003-lane-c-readiness": "python scripts/run_m257_c003_lane_c_readiness.py"'),
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
    parser.add_argument("--ast-header", type=Path, default=DEFAULT_AST_HEADER)
    parser.add_argument("--sema-cpp", type=Path, default=DEFAULT_SEMA_CPP)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=DEFAULT_LOWERING_CPP)
    parser.add_argument("--runtime-cpp", type=Path, default=DEFAULT_RUNTIME_CPP)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--runtime-library", type=Path, default=DEFAULT_RUNTIME_LIBRARY)
    parser.add_argument("--runtime-include-root", type=Path, default=DEFAULT_RUNTIME_INCLUDE_ROOT)
    parser.add_argument("--fixture", type=Path, default=DEFAULT_FIXTURE)
    parser.add_argument("--runtime-probe", type=Path, default=DEFAULT_RUNTIME_PROBE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--clangxx", default=DEFAULT_CLANGXX)
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
    return subprocess.run(
        list(command),
        cwd=cwd,
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


def resolve_tool(name: str) -> str | None:
    resolved = shutil.which(name)
    if resolved:
        return resolved
    return shutil.which(name + ".exe")


def validate_probe_payload(payload: dict[str, Any], failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, "probe_payload", check_id, detail, failures)

    check(int(payload.get("widget_instance", 0)) > 0, "M257-C003-PROBE-01", "alloc must materialize a positive Widget instance identity")
    check(payload.get("set_count_result") == 0, "M257-C003-PROBE-02", "void synthesized setter dispatch must return zero")
    check(payload.get("count_value") == 37, "M257-C003-PROBE-03", "synthesized count getter must return the stored integer")
    check(payload.get("set_enabled_result") == 0, "M257-C003-PROBE-04", "bool synthesized setter dispatch must return zero")
    check(payload.get("enabled_value") == 1, "M257-C003-PROBE-05", "synthesized bool getter must return one after a true write")
    check(payload.get("set_value_result") == 0, "M257-C003-PROBE-06", "id-like synthesized setter dispatch must return zero")
    check(payload.get("value_result") == 55, "M257-C003-PROBE-07", "synthesized id-like getter must return the stored scalar alias value")

    registration = payload.get("registration_state", {})
    check(registration.get("registered_image_count", 0) >= 1, "M257-C003-PROBE-08", "runtime must register at least one image")
    check(registration.get("registered_descriptor_total", 0) >= 1, "M257-C003-PROBE-09", "registered descriptor total must be non-zero")

    selector_state = payload.get("selector_table_state", {})
    check(selector_state.get("selector_table_entry_count", 0) >= 6, "M257-C003-PROBE-10", "selector table must materialize the synthesized accessor selector surface")

    expected_entries = {
        "count_entry": (0, "::instance_method:count"),
        "set_count_entry": (1, "::instance_method:setCount:"),
        "enabled_entry": (0, "::instance_method:enabled"),
        "set_enabled_entry": (1, "::instance_method:setEnabled:"),
        "value_entry": (0, "::instance_method:value"),
        "set_value_entry": (1, "::instance_method:setValue:"),
    }
    for entry_name, (parameter_count, owner_suffix) in expected_entries.items():
        entry = payload.get(entry_name, {})
        check(entry.get("found") == 1 and entry.get("resolved") == 1, f"M257-C003-{entry_name.upper()}-01", f"{entry_name} must resolve from the method cache")
        check(entry.get("parameter_count") == parameter_count, f"M257-C003-{entry_name.upper()}-02", f"{entry_name} must preserve the expected parameter count")
        check(str(entry.get("resolved_owner_identity", "")).endswith(owner_suffix), f"M257-C003-{entry_name.upper()}-03", f"{entry_name} must preserve the synthesized implementation owner identity")

    return checks_passed, checks_total


def run_dynamic_case(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    artifact = "dynamic_case"

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)

    check(args.native_exe.exists(), "M257-C003-DYN-01", f"missing native executable: {display_path(args.native_exe)}")
    check(args.runtime_library.exists(), "M257-C003-DYN-02", f"missing runtime library: {display_path(args.runtime_library)}")
    check(args.fixture.exists(), "M257-C003-DYN-03", f"missing fixture: {display_path(args.fixture)}")
    check(args.runtime_probe.exists(), "M257-C003-DYN-04", f"missing runtime probe: {display_path(args.runtime_probe)}")

    clangxx = resolve_tool(args.clangxx)
    check(clangxx is not None, "M257-C003-DYN-05", f"unable to resolve {args.clangxx}")
    if failures:
        return checks_passed, checks_total, {"skipped": False}

    probe_dir = args.probe_root.resolve() / f"probe-{uuid.uuid4().hex}"
    probe_dir.mkdir(parents=True, exist_ok=True)

    compile_result = run_command(
        [
            str(args.native_exe),
            str(args.fixture),
            "--out-dir",
            str(probe_dir),
            "--emit-prefix",
            "module",
        ],
        ROOT,
    )
    check(compile_result.returncode == 0, "M257-C003-DYN-06", f"fixture compile failed: {compile_result.stdout}{compile_result.stderr}")
    module_ir = probe_dir / "module.ll"
    module_obj = probe_dir / "module.obj"
    check(module_ir.exists(), "M257-C003-DYN-07", f"missing emitted IR: {display_path(module_ir)}")
    check(module_obj.exists(), "M257-C003-DYN-08", f"missing emitted object: {display_path(module_obj)}")
    if compile_result.returncode != 0 or not module_ir.exists() or not module_obj.exists():
        return checks_passed, checks_total, {
            "skipped": False,
            "compile_stdout": compile_result.stdout,
            "compile_stderr": compile_result.stderr,
        }

    ir_text = read_text(module_ir)
    boundary_line = next((line for line in ir_text.splitlines() if line.startswith(BOUNDARY_COMMENT_PREFIX)), "")
    named_metadata_present = NAMED_METADATA_PREFIX in ir_text
    check(bool(boundary_line), "M257-C003-DYN-09", "IR must publish the synthesized accessor/property lowering summary")
    check(named_metadata_present, "M257-C003-DYN-10", "IR must publish the synthesized accessor/property lowering named metadata")
    check("synthesized_accessor_entries=6" in boundary_line, "M257-C003-DYN-11", "IR summary must publish the expected synthesized accessor count")
    check("synthesized_storage_globals=3" in boundary_line, "M257-C003-DYN-12", "IR summary must publish the expected synthesized storage count")
    storage_globals = len(STORAGE_GLOBAL_RE.findall(ir_text))
    check(storage_globals == 3, "M257-C003-DYN-13", "IR must emit exactly three synthesized storage globals for the positive fixture")
    for index, symbol in enumerate((
        "@objc3_method_Widget_instance_count()",
        "@objc3_method_Widget_instance_setCount_(i32 %arg0)",
        "@objc3_method_Widget_instance_enabled()",
        "@objc3_method_Widget_instance_setEnabled_(i1 %arg0)",
        "@objc3_method_Widget_instance_value()",
        "@objc3_method_Widget_instance_setValue_(i32 %arg0)",
    ), start=14):
        check(symbol in ir_text, f"M257-C003-DYN-{index:02d}", f"IR must emit synthesized accessor body {symbol}")
    for index, needle in enumerate((
        "effective_getter_selector",
        "effective_setter_selector",
        "synthesized_binding",
        "ivar_layout",
        "ptr @objc3_method_Widget_instance_count",
        "ptr @objc3_method_Widget_instance_setCount_",
        "ptr @objc3_method_Widget_instance_enabled",
        "ptr @objc3_method_Widget_instance_setEnabled_",
        "ptr @objc3_method_Widget_instance_value",
        "ptr @objc3_method_Widget_instance_setValue_",
    ), start=20):
        check(needle in ir_text, f"M257-C003-DYN-{index:02d}", f"IR must retain {needle}")
    check(module_obj.stat().st_size > 0, "M257-C003-DYN-30", "emitted object must be non-empty")

    probe_exe = probe_dir / "m257_c003_synthesized_accessor_probe.exe"
    probe_compile = run_command(
        [
            str(clangxx),
            "-std=c++20",
            "-I",
            str(args.runtime_include_root),
            str(args.runtime_probe),
            str(module_obj),
            str(args.runtime_library),
            "-o",
            str(probe_exe),
        ],
        ROOT,
    )
    check(probe_compile.returncode == 0, "M257-C003-DYN-31", f"probe compile failed: {probe_compile.stdout}{probe_compile.stderr}")
    if probe_compile.returncode != 0:
        return checks_passed, checks_total, {
            "skipped": False,
            "probe_compile_stdout": probe_compile.stdout,
            "probe_compile_stderr": probe_compile.stderr,
        }

    probe_run = run_command([str(probe_exe)], ROOT)
    check(probe_run.returncode == 0, "M257-C003-DYN-32", f"probe execution failed: {probe_run.stdout}{probe_run.stderr}")
    if probe_run.returncode != 0:
        return checks_passed, checks_total, {
            "skipped": False,
            "probe_stdout": probe_run.stdout,
            "probe_stderr": probe_run.stderr,
        }

    try:
        probe_payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(artifact, "M257-C003-DYN-33", f"probe output is not valid JSON: {exc}"))
        return checks_passed, checks_total + 1, {"skipped": False, "probe_stdout": probe_run.stdout}

    payload_passed, payload_total = validate_probe_payload(probe_payload, failures)
    checks_passed += payload_passed
    checks_total += payload_total
    return checks_passed, checks_total, {
        "skipped": False,
        "probe_dir": display_path(probe_dir),
        "module_ir": display_path(module_ir),
        "module_obj": display_path(module_obj),
        "probe_exe": display_path(probe_exe),
        "boundary_line": boundary_line,
        "storage_global_count": storage_globals,
        "payload": probe_payload,
    }


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.ast_header, AST_SNIPPETS),
        (args.sema_cpp, SEMA_SNIPPETS),
        (args.ir_emitter, IR_SNIPPETS),
        (args.frontend_artifacts, ARTIFACTS_SNIPPETS),
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.lowering_cpp, LOWERING_CPP_SNIPPETS),
        (args.runtime_cpp, RUNTIME_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_case: dict[str, Any]
    if args.skip_dynamic_probes:
        dynamic_case = {"skipped": True}
    else:
        passed, total, dynamic_case = run_dynamic_case(args, failures)
        checks_passed += passed
        checks_total += total

    summary_payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_case": dynamic_case,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in failures
        ],
    }

    summary_path = args.summary_out
    if not summary_path.is_absolute():
        summary_path = ROOT / summary_path
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        print(f"[fail] {MODE} ({checks_passed}/{checks_total} checks passed)", file=sys.stderr)
        for finding in failures:
            print(f"- {finding.check_id} [{finding.artifact}] {finding.detail}", file=sys.stderr)
        print(f"[info] summary: {display_path(summary_path)}", file=sys.stderr)
        return 1

    print(f"[ok] {MODE} ({checks_passed}/{checks_total} checks passed)")
    print(f"[info] summary: {display_path(summary_path)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run())

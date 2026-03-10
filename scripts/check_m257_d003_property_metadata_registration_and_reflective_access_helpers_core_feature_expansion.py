#!/usr/bin/env python3
"""Validate M257-D003 property metadata registration and reflective access helpers."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
import uuid
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "M257-D003"
CONTRACT_ID = "objc3c-runtime-property-metadata-reflection/m257-d003-v1"
REGISTRATION_MODEL = (
    "runtime-registers-reflectable-property-accessor-and-layout-facts-from-emitted-metadata-without-source-rediscovery"
)
QUERY_MODEL = (
    "private-testing-helpers-query-realized-property-metadata-by-class-and-property-name-including-effective-accessors-and-layout-facts"
)
FAIL_CLOSED_MODEL = (
    "no-public-reflection-abi-no-reflective-source-recovery-no-property-query-success-without-realized-runtime-layout"
)
SUMMARY_RELATIVE_PATH = "tmp/reports/m257/M257-D003/property_metadata_reflection_summary.json"
FIXTURE_SOURCE = "tests/tooling/fixtures/native/m257_d003_property_metadata_reflection_positive.objc3"
RUNTIME_PROBE_SOURCE = "tests/tooling/runtime/m257_d003_property_metadata_reflection_probe.cpp"
BOUNDARY_PREFIX = "; runtime_property_metadata_reflection = "
EXPECTED_BASE_IDENTITY = 1024
EXPECTED_INSTANCE_SIZE = 24

EXPECTATIONS_SNIPPETS = (
    "# M257 Property Metadata Registration And Reflective Access Helpers Core Feature Expansion Expectations (D003)",
    f"Contract ID: `{CONTRACT_ID}`",
    f"`{REGISTRATION_MODEL}`",
    f"`{QUERY_MODEL}`",
    f"`{FAIL_CLOSED_MODEL}`",
    f"`{RUNTIME_PROBE_SOURCE}`",
    f"`{SUMMARY_RELATIVE_PATH}`",
)
PACKET_SNIPPETS = (
    "# M257-D003 Property Metadata Registration And Reflective Access Helpers Core Feature Expansion Packet",
    "Packet: `M257-D003`",
    "Issue: `#7155`",
    f"Contract ID: `{CONTRACT_ID}`",
    f"`{FIXTURE_SOURCE}`",
    f"`{RUNTIME_PROBE_SOURCE}`",
    f"`{SUMMARY_RELATIVE_PATH}`",
)
NATIVE_DOC_SNIPPETS = (
    "## Property metadata registration and reflective access helpers (M257-D003)",
    CONTRACT_ID,
    REGISTRATION_MODEL,
    QUERY_MODEL,
)
LOWERING_SPEC_SNIPPETS = (
    "## M257 property metadata registration and reflective access helpers (D003)",
    CONTRACT_ID,
    REGISTRATION_MODEL,
    QUERY_MODEL,
)
METADATA_SPEC_SNIPPETS = (
    "## M257 runtime property metadata reflection anchors (D003)",
    CONTRACT_ID,
    "`; runtime_property_metadata_reflection = contract=objc3c-runtime-property-metadata-reflection/m257-d003-v1`",
    "`objc3_runtime_copy_property_registry_state_for_testing`",
    "`objc3_runtime_copy_property_entry_for_testing`",
)
ARCHITECTURE_SNIPPETS = (
    "## M257 property metadata registration and reflective access helpers (D003)",
    "runtime reflection queries consume realized property/accessor/layout facts by",
    "aggregate registry snapshots publish reflectable-property totals plus",
)
RUNTIME_README_SNIPPETS = (
    "`M257-D003` adds private runtime reflection helpers above that same realized",
    "property graph:",
    CONTRACT_ID,
    "`objc3_runtime_copy_property_registry_state_for_testing`",
)
TOOLING_RUNTIME_README_SNIPPETS = (
    "`M257-D003` adds private property metadata reflection helpers over that same",
    "runtime-owned graph:",
    CONTRACT_ID,
    "`tests/tooling/runtime/m257_d003_property_metadata_reflection_probe.cpp`",
)
LOWERING_HEADER_SNIPPETS = (
    "kObjc3RuntimePropertyMetadataReflectionContractId",
    "kObjc3RuntimePropertyMetadataReflectionRegistrationModel",
    "kObjc3RuntimePropertyMetadataReflectionQueryModel",
    "kObjc3RuntimePropertyMetadataReflectionFailClosedModel",
    "Objc3RuntimePropertyMetadataReflectionSummary()",
)
LOWERING_CPP_SNIPPETS = (
    "std::string Objc3RuntimePropertyMetadataReflectionSummary()",
    'out << "contract=" << kObjc3RuntimePropertyMetadataReflectionContractId',
    '<< ";registration_model="',
    '<< ";query_model="',
    '<< ";fail_closed_model="',
)
AST_SNIPPETS = (
    "M257-D003 property-metadata-reflection anchor",
    "private runtime reflection",
)
SEMA_SNIPPETS = (
    "M257-D003 property-metadata-reflection anchor",
    "runtime reflection",
)
IR_SNIPPETS = (
    "M257-D003 property-metadata-reflection anchor",
    "; runtime_property_metadata_reflection = ",
    '<< ";reflectable_property_entries="',
    '<< ";writable_property_entries="',
)
BOOTSTRAP_HEADER_SNIPPETS = (
    "objc3_runtime_property_registry_state_snapshot",
    "objc3_runtime_property_entry_snapshot",
    "objc3_runtime_copy_property_registry_state_for_testing(",
    "objc3_runtime_copy_property_entry_for_testing(",
    "M257-D003 property-metadata-reflection anchor",
)
RUNTIME_CPP_SNIPPETS = (
    "M257-D003 property-metadata-reflection anchor",
    "FindRuntimePropertyAccessorByNameUnlocked(",
    "objc3_runtime_copy_property_registry_state_for_testing(",
    "objc3_runtime_copy_property_entry_for_testing(",
)
PROBE_SNIPPETS = (
    '#include "runtime/objc3_runtime_bootstrap_internal.h"',
    'objc3_runtime_copy_property_registry_state_for_testing(&registry_before)',
    'objc3_runtime_copy_property_entry_for_testing("Widget", "token",',
    'objc3_runtime_copy_property_entry_for_testing("Widget", "value",',
    'objc3_runtime_copy_property_entry_for_testing("Widget", "count",',
)
PACKAGE_SNIPPETS = (
    '"check:objc3c:m257-d003-property-metadata-reflection": "python scripts/check_m257_d003_property_metadata_registration_and_reflective_access_helpers_core_feature_expansion.py"',
    '"test:tooling:m257-d003-property-metadata-reflection": "python -m pytest tests/tooling/test_check_m257_d003_property_metadata_registration_and_reflective_access_helpers_core_feature_expansion.py -q"',
    '"check:objc3c:m257-d003-lane-d-readiness": "python scripts/run_m257_d003_lane_d_readiness.py"',
)


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    try:
        return str(path.relative_to(ROOT)).replace("\\", "/")
    except ValueError:
        return str(path).replace("\\", "/")


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, artifact: str, check_id: str, detail: str,
            failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def ensure_snippets(path: Path, snippets: Sequence[str], failures: list[Finding]) -> int:
    text = read_text(path)
    passed = 0
    for index, snippet in enumerate(snippets, start=1):
        passed += require(
            snippet in text,
            display_path(path),
            f"{MODE}-DOC-{index:02d}",
            f"missing snippet: {snippet}",
            failures,
        )
    return passed


def resolve_tool(name: str) -> str | None:
    resolved = shutil.which(name)
    if resolved:
        return resolved
    if sys.platform == "win32":
        candidate = Path("C:/Program Files/LLVM/bin") / name
        if candidate.exists():
            return str(candidate)
        candidate = Path("C:/Program Files/LLVM/bin") / f"{name}.exe"
        if candidate.exists():
            return str(candidate)
    return None


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


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=ROOT / "docs/contracts/m257_property_metadata_registration_and_reflective_access_helpers_core_feature_expansion_d003_expectations.md")
    parser.add_argument("--packet-doc", type=Path, default=ROOT / "spec/planning/compiler/m257/m257_d003_property_metadata_registration_and_reflective_access_helpers_core_feature_expansion_packet.md")
    parser.add_argument("--native-doc", type=Path, default=ROOT / "docs/objc3c-native.md")
    parser.add_argument("--lowering-spec", type=Path, default=ROOT / "spec/LOWERING_AND_RUNTIME_CONTRACTS.md")
    parser.add_argument("--metadata-spec", type=Path, default=ROOT / "spec/MODULE_METADATA_AND_ABI_TABLES.md")
    parser.add_argument("--architecture-doc", type=Path, default=ROOT / "native/objc3c/src/ARCHITECTURE.md")
    parser.add_argument("--runtime-readme", type=Path, default=ROOT / "native/objc3c/src/runtime/README.md")
    parser.add_argument("--tooling-runtime-readme", type=Path, default=ROOT / "tests/tooling/runtime/README.md")
    parser.add_argument("--lowering-header", type=Path, default=ROOT / "native/objc3c/src/lower/objc3_lowering_contract.h")
    parser.add_argument("--lowering-cpp", type=Path, default=ROOT / "native/objc3c/src/lower/objc3_lowering_contract.cpp")
    parser.add_argument("--ast-header", type=Path, default=ROOT / "native/objc3c/src/ast/objc3_ast.h")
    parser.add_argument("--sema-cpp", type=Path, default=ROOT / "native/objc3c/src/sema/objc3_semantic_passes.cpp")
    parser.add_argument("--ir-emitter", type=Path, default=ROOT / "native/objc3c/src/ir/objc3_ir_emitter.cpp")
    parser.add_argument("--bootstrap-header", type=Path, default=ROOT / "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h")
    parser.add_argument("--runtime-source", type=Path, default=ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp")
    parser.add_argument("--runtime-probe", type=Path, default=ROOT / RUNTIME_PROBE_SOURCE)
    parser.add_argument("--fixture", type=Path, default=ROOT / FIXTURE_SOURCE)
    parser.add_argument("--package-json", type=Path, default=ROOT / "package.json")
    parser.add_argument("--native-exe", type=Path, default=ROOT / "artifacts/bin/objc3c-native.exe")
    parser.add_argument("--runtime-library", type=Path, default=ROOT / "artifacts/lib/objc3_runtime.lib")
    parser.add_argument("--runtime-include-root", type=Path, default=ROOT / "native/objc3c/src")
    parser.add_argument("--clangxx", default="clang++.exe")
    parser.add_argument("--probe-root", type=Path, default=ROOT / "tmp/artifacts/compilation/objc3c-native/m257/d003-property-metadata-reflection")
    parser.add_argument("--summary-out", type=Path, default=ROOT / SUMMARY_RELATIVE_PATH)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(list(argv))


def boundary_line(ir_text: str, prefix: str) -> str:
    return next((line for line in ir_text.splitlines() if line.startswith(prefix)), "")


def endswith(value: Any, suffix: str) -> bool:
    return isinstance(value, str) and value.endswith(suffix)


def validate_probe_payload(payload: dict[str, Any], failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, "runtime_probe", check_id, detail, failures)

    registry_before = payload.get("registry_state_before", {})
    check(registry_before.get("layout_ready_class_count") == 1,
          "M257-D003-DYN-PAY-01", "registry state must publish one layout-ready class for the positive fixture")
    check(registry_before.get("reflectable_property_count") == 3,
          "M257-D003-DYN-PAY-02", "registry state must publish three reflectable properties")
    check(registry_before.get("writable_property_count") == 2,
          "M257-D003-DYN-PAY-03", "registry state must publish two writable properties")
    check(registry_before.get("slot_backed_property_count") == 3,
          "M257-D003-DYN-PAY-04", "registry state must publish three slot-backed properties")
    check(registry_before.get("last_query_found") == 0,
          "M257-D003-DYN-PAY-05", "registry state must begin with no successful reflective query")

    widget_entry = payload.get("widget_entry", {})
    check(widget_entry.get("found") == 1,
          "M257-D003-DYN-PAY-06", "Widget realized class entry must resolve")
    check(widget_entry.get("base_identity") == EXPECTED_BASE_IDENTITY,
          "M257-D003-DYN-PAY-07", "Widget realized class entry must preserve the base identity")
    check(widget_entry.get("runtime_property_accessor_count") == 3,
          "M257-D003-DYN-PAY-08", "Widget realized class entry must publish all reflectable property accessors")
    check(widget_entry.get("runtime_instance_size_bytes") == EXPECTED_INSTANCE_SIZE,
          "M257-D003-DYN-PAY-09", "Widget realized class entry must publish the expected instance size")

    token_entry = payload.get("token_property", {})
    check(token_entry.get("found") == 1,
          "M257-D003-DYN-PAY-10", "token property reflection entry must resolve")
    check(token_entry.get("setter_available") == 0,
          "M257-D003-DYN-PAY-11", "token property must remain readonly")
    check(token_entry.get("has_runtime_getter") == 1 and token_entry.get("has_runtime_setter") == 0,
          "M257-D003-DYN-PAY-12", "token property must expose only a runtime getter")
    check(token_entry.get("resolved_class_name") == "Widget",
          "M257-D003-DYN-PAY-13", "token property must resolve on Widget")
    check(token_entry.get("effective_getter_selector") == "tokenValue",
          "M257-D003-DYN-PAY-14", "token property must preserve the effective getter selector")
    check(token_entry.get("effective_setter_selector") is None,
          "M257-D003-DYN-PAY-15", "token property must not publish an effective setter selector")
    check(token_entry.get("synthesized_binding_symbol") is not None,
          "M257-D003-DYN-PAY-16", "token property must publish a synthesized binding symbol")
    check(token_entry.get("ivar_layout_symbol") is not None,
          "M257-D003-DYN-PAY-17", "token property must publish an ivar layout symbol")
    check(endswith(token_entry.get("getter_owner_identity"), "implementation:Widget::instance_method:tokenValue"),
          "M257-D003-DYN-PAY-18", "token property must preserve the runtime getter owner identity")

    value_entry = payload.get("value_property", {})
    check(value_entry.get("found") == 1,
          "M257-D003-DYN-PAY-19", "value property reflection entry must resolve")
    check(value_entry.get("setter_available") == 1 and value_entry.get("has_runtime_setter") == 1,
          "M257-D003-DYN-PAY-20", "value property must expose a runtime setter")
    check(value_entry.get("effective_getter_selector") == "currentValue",
          "M257-D003-DYN-PAY-21", "value property must preserve the custom getter selector")
    check(value_entry.get("effective_setter_selector") == "setCurrentValue:",
          "M257-D003-DYN-PAY-22", "value property must preserve the custom setter selector")
    check(endswith(value_entry.get("getter_owner_identity"), "implementation:Widget::instance_method:currentValue"),
          "M257-D003-DYN-PAY-23", "value property must preserve the runtime getter owner identity")
    check(endswith(value_entry.get("setter_owner_identity"), "implementation:Widget::instance_method:setCurrentValue:"),
          "M257-D003-DYN-PAY-24", "value property must preserve the runtime setter owner identity")

    count_entry = payload.get("count_property", {})
    check(count_entry.get("found") == 1,
          "M257-D003-DYN-PAY-25", "count property reflection entry must resolve")
    check(count_entry.get("setter_available") == 1 and count_entry.get("has_runtime_setter") == 1,
          "M257-D003-DYN-PAY-26", "count property must expose a runtime setter")
    check(count_entry.get("effective_getter_selector") == "count",
          "M257-D003-DYN-PAY-27", "count property must preserve the getter selector")
    check(count_entry.get("effective_setter_selector") == "setCount:",
          "M257-D003-DYN-PAY-28", "count property must preserve the setter selector")
    check(endswith(count_entry.get("getter_owner_identity"), "implementation:Widget::instance_method:count"),
          "M257-D003-DYN-PAY-29", "count property must preserve the runtime getter owner identity")
    check(endswith(count_entry.get("setter_owner_identity"), "implementation:Widget::instance_method:setCount:"),
          "M257-D003-DYN-PAY-30", "count property must preserve the runtime setter owner identity")

    distinct_slots = {
        token_entry.get("slot_index"),
        value_entry.get("slot_index"),
        count_entry.get("slot_index"),
    }
    check(len(distinct_slots) == 3,
          "M257-D003-DYN-PAY-31", "reflective property entries must preserve distinct slot indices")
    for check_id, entry in (
        ("M257-D003-DYN-PAY-32", token_entry),
        ("M257-D003-DYN-PAY-33", value_entry),
        ("M257-D003-DYN-PAY-34", count_entry),
    ):
        check(entry.get("base_identity") == EXPECTED_BASE_IDENTITY,
              check_id, "property reflection entries must preserve the Widget base identity")
    for check_id, entry in (
        ("M257-D003-DYN-PAY-35", token_entry),
        ("M257-D003-DYN-PAY-36", value_entry),
        ("M257-D003-DYN-PAY-37", count_entry),
    ):
        check(entry.get("instance_size_bytes") == EXPECTED_INSTANCE_SIZE,
              check_id, "property reflection entries must preserve the realized instance size")

    registry_after_count = payload.get("registry_state_after_count", {})
    check(registry_after_count.get("last_query_found") == 1,
          "M257-D003-DYN-PAY-38", "registry state must record the successful count-property query")
    check(registry_after_count.get("last_queried_class_name") == "Widget",
          "M257-D003-DYN-PAY-39", "registry state must preserve the last queried class name")
    check(registry_after_count.get("last_queried_property_name") == "count",
          "M257-D003-DYN-PAY-40", "registry state must preserve the last queried property name")
    check(registry_after_count.get("last_resolved_class_name") == "Widget",
          "M257-D003-DYN-PAY-41", "registry state must preserve the last resolved class name")
    check(endswith(registry_after_count.get("last_resolved_owner_identity"), "implementation:Widget"),
          "M257-D003-DYN-PAY-42", "registry state must preserve the last resolved owner identity")

    missing_entry = payload.get("missing_property", {})
    check(missing_entry.get("found") == 0,
          "M257-D003-DYN-PAY-43", "missing property query must fail closed")
    check(missing_entry.get("queried_class_name") == "Widget",
          "M257-D003-DYN-PAY-44", "missing property query must preserve the queried class name")

    missing_class_entry = payload.get("missing_class_property", {})
    check(missing_class_entry.get("found") == 0,
          "M257-D003-DYN-PAY-45", "missing class property query must fail closed")
    check(missing_class_entry.get("queried_class_name") == "MissingWidget",
          "M257-D003-DYN-PAY-46", "missing class query must preserve the queried class name")

    registry_after_missing = payload.get("registry_state_after_missing", {})
    check(registry_after_missing.get("last_query_found") == 0,
          "M257-D003-DYN-PAY-47", "registry state must record the final missing-class query as a miss")
    check(registry_after_missing.get("last_queried_class_name") == "MissingWidget",
          "M257-D003-DYN-PAY-48", "registry state must preserve the final missing-class query name")
    check(registry_after_missing.get("last_queried_property_name") == "count",
          "M257-D003-DYN-PAY-49", "registry state must preserve the final missing-class property name")
    check(registry_after_missing.get("last_resolved_class_name") is None,
          "M257-D003-DYN-PAY-50", "registry state must clear the resolved class name after a miss")

    return checks_passed, checks_total


def run_dynamic_case(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    artifact = "dynamic_case"

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)

    check(args.native_exe.exists(), "M257-D003-DYN-01", f"missing native executable: {display_path(args.native_exe)}")
    check(args.runtime_library.exists(), "M257-D003-DYN-02", f"missing runtime library: {display_path(args.runtime_library)}")
    check(args.fixture.exists(), "M257-D003-DYN-03", f"missing fixture: {display_path(args.fixture)}")
    check(args.runtime_probe.exists(), "M257-D003-DYN-04", f"missing runtime probe: {display_path(args.runtime_probe)}")
    clangxx = resolve_tool(args.clangxx)
    check(clangxx is not None, "M257-D003-DYN-05", f"unable to resolve {args.clangxx}")
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
    check(compile_result.returncode == 0, "M257-D003-DYN-06", f"fixture compile failed: {compile_result.stdout}{compile_result.stderr}")
    module_ir = probe_dir / "module.ll"
    module_obj = probe_dir / "module.obj"
    check(module_ir.exists(), "M257-D003-DYN-07", f"missing emitted IR: {display_path(module_ir)}")
    check(module_obj.exists(), "M257-D003-DYN-08", f"missing emitted object: {display_path(module_obj)}")
    if compile_result.returncode != 0 or not module_ir.exists() or not module_obj.exists():
        return checks_passed, checks_total, {
            "skipped": False,
            "compile_stdout": compile_result.stdout,
            "compile_stderr": compile_result.stderr,
        }

    ir_text = read_text(module_ir)
    boundary = boundary_line(ir_text, BOUNDARY_PREFIX)
    check(bool(boundary), "M257-D003-DYN-09", "IR must publish the D003 runtime property reflection summary")
    check("reflectable_property_entries=6" in boundary, "M257-D003-DYN-10", "IR summary must publish the full six-record property descriptor inventory")
    check("writable_property_entries=2" in boundary, "M257-D003-DYN-11", "IR summary must publish two writable property entries")
    check("synthesized_accessor_entries=5" in boundary, "M257-D003-DYN-12", "IR summary must publish five synthesized accessor entries")

    probe_exe = probe_dir / "m257_d003_property_metadata_reflection_probe.exe"
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
    check(probe_compile.returncode == 0, "M257-D003-DYN-13", f"probe compile failed: {probe_compile.stdout}{probe_compile.stderr}")
    if probe_compile.returncode != 0:
        return checks_passed, checks_total, {
            "skipped": False,
            "probe_compile_stdout": probe_compile.stdout,
            "probe_compile_stderr": probe_compile.stderr,
        }

    probe_run = run_command([str(probe_exe)], ROOT)
    check(probe_run.returncode == 0, "M257-D003-DYN-14", f"probe execution failed: {probe_run.stdout}{probe_run.stderr}")
    if probe_run.returncode != 0:
        return checks_passed, checks_total, {
            "skipped": False,
            "probe_stdout": probe_run.stdout,
            "probe_stderr": probe_run.stderr,
        }

    try:
        payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(artifact, "M257-D003-DYN-15", f"probe output is not valid JSON: {exc}"))
        return checks_passed, checks_total + 1, {"skipped": False, "probe_stdout": probe_run.stdout}

    payload_passed, payload_total = validate_probe_payload(payload, failures)
    checks_passed += payload_passed
    checks_total += payload_total
    return checks_passed, checks_total, {
        "skipped": False,
        "probe_dir": display_path(probe_dir),
        "module_ir": display_path(module_ir),
        "module_obj": display_path(module_obj),
        "probe_exe": display_path(probe_exe),
        "boundary": boundary,
        "payload": payload,
    }


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.runtime_readme, RUNTIME_README_SNIPPETS),
        (args.tooling_runtime_readme, TOOLING_RUNTIME_README_SNIPPETS),
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.lowering_cpp, LOWERING_CPP_SNIPPETS),
        (args.ast_header, AST_SNIPPETS),
        (args.sema_cpp, SEMA_SNIPPETS),
        (args.ir_emitter, IR_SNIPPETS),
        (args.bootstrap_header, BOOTSTRAP_HEADER_SNIPPETS),
        (args.runtime_source, RUNTIME_CPP_SNIPPETS),
        (args.runtime_probe, PROBE_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_payload: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        passed, total, dynamic_payload = run_dynamic_case(args, failures)
        checks_passed += passed
        checks_total += total

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "registration_model": REGISTRATION_MODEL,
        "query_model": QUERY_MODEL,
        "fail_closed_model": FAIL_CLOSED_MODEL,
        "dynamic_probe": dynamic_payload,
        "failures": [finding.__dict__ for finding in failures],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        print(f"[fail] {MODE} ({checks_passed}/{checks_total} checks passed)")
        for finding in failures:
            print(f"- {finding.check_id} [{finding.artifact}] {finding.detail}")
        print(f"[info] summary: {display_path(args.summary_out)}")
        return 1

    print(f"[ok] {MODE} ({checks_passed}/{checks_total} checks passed)")
    print(f"[info] summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run())

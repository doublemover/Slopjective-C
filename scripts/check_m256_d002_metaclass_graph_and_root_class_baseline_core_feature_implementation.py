#!/usr/bin/env python3
"""Validate M256-D002 metaclass graph and root-class baseline implementation."""

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
MODE = "M256-D002"
CONTRACT_ID = "objc3c-runtime-metaclass-graph-root-class-baseline/m256-d002-v1"
REALIZED_GRAPH_MODEL = (
    "runtime-owned-realized-class-nodes-bind-receiver-base-identities-to-class-and-metaclass-records"
)
ROOT_CLASS_BASELINE_MODEL = (
    "root-classes-realize-with-null-superclass-links-and-live-instance-plus-class-dispatch"
)
FAIL_CLOSED_MODEL = (
    "missing-receiver-bindings-or-broken-realized-superclass-links-fall-closed-to-compatibility-dispatch"
)
SUMMARY_RELATIVE_PATH = "tmp/reports/m256/M256-D002/metaclass_graph_and_root_class_baseline_summary.json"
FIXTURE_SOURCE = "tests/tooling/fixtures/native/m256_d002_metaclass_graph_root_class_library.objc3"
RUNTIME_PROBE_SOURCE = "tests/tooling/runtime/m256_d002_metaclass_graph_root_class_probe.cpp"
BOUNDARY_PREFIX = "; runtime_metaclass_graph_root_class_baseline = "
EXPECTED_ROOT_SHARED_OWNER = "implementation:RootObject::class_method:shared"
EXPECTED_ROOT_INSTANCE_OWNER = "implementation:RootObject::instance_method:rootValue"
EXPECTED_WIDGET_OWNER = "implementation:Widget::instance_method:widgetValue"

EXPECTATIONS_SNIPPETS = (
    "# M256 Metaclass Graph and Root-Class Baseline Core Feature Implementation Expectations (D002)",
    f"Contract ID: `{CONTRACT_ID}`",
    f"`{REALIZED_GRAPH_MODEL}`",
    f"`{ROOT_CLASS_BASELINE_MODEL}`",
    f"`{FAIL_CLOSED_MODEL}`",
    f"`{FIXTURE_SOURCE}`",
    f"`{RUNTIME_PROBE_SOURCE}`",
    f"`{SUMMARY_RELATIVE_PATH}`",
)
PACKET_SNIPPETS = (
    "# M256-D002 Metaclass Graph and Root-Class Baseline Core Feature Implementation Packet",
    "Packet: `M256-D002`",
    "Issue: `#7140`",
    f"`{CONTRACT_ID}`",
    f"`{FIXTURE_SOURCE}`",
    f"`{RUNTIME_PROBE_SOURCE}`",
    f"`{SUMMARY_RELATIVE_PATH}`",
)
NATIVE_DOC_SNIPPETS = (
    "## Metaclass graph and root-class baseline (M256-D002)",
    CONTRACT_ID,
    REALIZED_GRAPH_MODEL,
    ROOT_CLASS_BASELINE_MODEL,
    f"`{RUNTIME_PROBE_SOURCE}`",
)
LOWERING_SPEC_SNIPPETS = (
    "## M256 metaclass graph and root-class baseline (D002)",
    CONTRACT_ID,
    REALIZED_GRAPH_MODEL,
    ROOT_CLASS_BASELINE_MODEL,
    FAIL_CLOSED_MODEL,
)
METADATA_SPEC_SNIPPETS = (
    "## M256 runtime metaclass graph and root-class anchors (D002)",
    CONTRACT_ID,
    "`; runtime_metaclass_graph_root_class_baseline = contract=objc3c-runtime-metaclass-graph-root-class-baseline/m256-d002-v1`",
    f"`{RUNTIME_PROBE_SOURCE}`",
)
ARCHITECTURE_SNIPPETS = (
    "## M256 metaclass graph and root-class baseline (D002)",
    "runtime owns the realized class graph keyed by receiver base identities",
    "root classes remain explicit graph nodes with null superclass links",
    "known-class and class-self dispatch continue to share the metaclass cache key",
)
RUNTIME_README_SNIPPETS = (
    "`M256-D002` promotes that freeze boundary into a runtime-owned realized graph",
    CONTRACT_ID,
    ROOT_CLASS_BASELINE_MODEL,
)
TOOLING_RUNTIME_README_SNIPPETS = (
    "`M256-D002` proves the runtime-owned realized class graph and root-class",
    CONTRACT_ID,
    "tests/tooling/runtime/m256_d002_metaclass_graph_root_class_probe.cpp",
)
LOWERING_HEADER_SNIPPETS = (
    "kObjc3RuntimeMetaclassGraphRootClassContractId",
    "kObjc3RuntimeRealizedClassGraphModel",
    "kObjc3RuntimeRootClassBaselineModel",
    "kObjc3RuntimeRealizedClassGraphFailClosedModel",
    "Objc3RuntimeMetaclassGraphRootClassSummary();",
)
LOWERING_CPP_SNIPPETS = (
    "std::string Objc3RuntimeMetaclassGraphRootClassSummary()",
    'out << "contract=" << kObjc3RuntimeMetaclassGraphRootClassContractId',
    '<< ";realized_class_graph_model="',
    '<< ";root_class_baseline_model="',
    '<< ";fail_closed_model="',
)
PARSER_SNIPPETS = (
    "M256-D002 metaclass-graph-root-class anchor",
    "superclass spellings explicit so runtime can distinguish root-class",
)
SEMA_SNIPPETS = (
    "M256-D002 metaclass-graph-root-class anchor",
    "root-class empties and canonical superclass/metaclass owner identities",
)
IR_EMITTER_SNIPPETS = (
    "M256-D002 metaclass-graph-root-class anchor",
    "; runtime_metaclass_graph_root_class_baseline = ",
    '<< ";receiver_binding_candidate_count="',
)
RUNTIME_HEADER_SNIPPETS = (
    "M256-D002 metaclass-graph-root-class anchor",
    "root-class baseline also stay behind this same ABI",
)
BOOTSTRAP_HEADER_SNIPPETS = (
    "M256-D002 metaclass-graph-root-class anchor",
    "objc3_runtime_copy_realized_class_graph_state_for_testing",
    "objc3_runtime_copy_realized_class_entry_for_testing",
)
RUNTIME_CPP_SNIPPETS = (
    "M256-D002 metaclass-graph-root-class anchor",
    "M256-D002 metaclass-graph-root-class anchor: runtime now republishes a",
    "M256-D002 metaclass-graph-root-class anchor: successful registration now",
)
PROBE_SNIPPETS = (
    '#include "runtime/objc3_runtime_bootstrap_internal.h"',
    'objc3_runtime_copy_realized_class_graph_state_for_testing(&graph_state)',
    'objc3_runtime_copy_realized_class_entry_for_testing("RootObject",',
    'objc3_runtime_dispatch_i32(1026, "shared", 0, 0, 0, 0)',
    'objc3_runtime_dispatch_i32(1043, "shared", 0, 0, 0, 0)',
    'objc3_runtime_dispatch_i32(1041, "shared", 0, 0, 0, 0)',
    'objc3_runtime_dispatch_i32(1042, "rootValue", 0, 0, 0, 0)',
)
PACKAGE_SNIPPETS = (
    '"check:objc3c:m256-d002-metaclass-graph-root-class-baseline": "python scripts/check_m256_d002_metaclass_graph_and_root_class_baseline_core_feature_implementation.py"',
    '"test:tooling:m256-d002-metaclass-graph-root-class-baseline": "python -m pytest tests/tooling/test_check_m256_d002_metaclass_graph_and_root_class_baseline_core_feature_implementation.py -q"',
    '"check:objc3c:m256-d002-lane-d-readiness": "python scripts/run_m256_d002_lane_d_readiness.py"',
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
        return str(path.relative_to(ROOT))
    except ValueError:
        return str(path)


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


def boundary_line(ir_text: str, prefix: str) -> str:
    for line in ir_text.splitlines():
        if line.startswith(prefix):
            return line
    return ""


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=ROOT / "docs/contracts/m256_metaclass_graph_and_root_class_baseline_core_feature_implementation_d002_expectations.md")
    parser.add_argument("--packet-doc", type=Path, default=ROOT / "spec/planning/compiler/m256/m256_d002_metaclass_graph_and_root_class_baseline_core_feature_implementation_packet.md")
    parser.add_argument("--native-doc", type=Path, default=ROOT / "docs/objc3c-native.md")
    parser.add_argument("--lowering-spec", type=Path, default=ROOT / "spec/LOWERING_AND_RUNTIME_CONTRACTS.md")
    parser.add_argument("--metadata-spec", type=Path, default=ROOT / "spec/MODULE_METADATA_AND_ABI_TABLES.md")
    parser.add_argument("--architecture-doc", type=Path, default=ROOT / "native/objc3c/src/ARCHITECTURE.md")
    parser.add_argument("--runtime-readme", type=Path, default=ROOT / "native/objc3c/src/runtime/README.md")
    parser.add_argument("--tooling-runtime-readme", type=Path, default=ROOT / "tests/tooling/runtime/README.md")
    parser.add_argument("--lowering-header", type=Path, default=ROOT / "native/objc3c/src/lower/objc3_lowering_contract.h")
    parser.add_argument("--lowering-cpp", type=Path, default=ROOT / "native/objc3c/src/lower/objc3_lowering_contract.cpp")
    parser.add_argument("--parser-cpp", type=Path, default=ROOT / "native/objc3c/src/parse/objc3_parser.cpp")
    parser.add_argument("--sema-cpp", type=Path, default=ROOT / "native/objc3c/src/sema/objc3_semantic_passes.cpp")
    parser.add_argument("--ir-emitter", type=Path, default=ROOT / "native/objc3c/src/ir/objc3_ir_emitter.cpp")
    parser.add_argument("--runtime-header", type=Path, default=ROOT / "native/objc3c/src/runtime/objc3_runtime.h")
    parser.add_argument("--bootstrap-header", type=Path, default=ROOT / "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h")
    parser.add_argument("--runtime-source", type=Path, default=ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp")
    parser.add_argument("--runtime-probe", type=Path, default=ROOT / RUNTIME_PROBE_SOURCE)
    parser.add_argument("--fixture", type=Path, default=ROOT / FIXTURE_SOURCE)
    parser.add_argument("--package-json", type=Path, default=ROOT / "package.json")
    parser.add_argument("--native-exe", type=Path, default=ROOT / "artifacts/bin/objc3c-native.exe")
    parser.add_argument("--runtime-library", type=Path, default=ROOT / "artifacts/lib/objc3_runtime.lib")
    parser.add_argument("--runtime-include-root", type=Path, default=ROOT / "native/objc3c/src")
    parser.add_argument("--clangxx", default="clang++.exe")
    parser.add_argument("--probe-root", type=Path, default=ROOT / "tmp/artifacts/compilation/objc3c-native/m256/d002-metaclass-graph-root-class-baseline")
    parser.add_argument("--summary", type=Path, default=ROOT / SUMMARY_RELATIVE_PATH)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(list(argv))


def payload_checks(probe_payload: dict[str, Any], module_manifest: dict[str, Any],
                   runtime_manifest: dict[str, Any], failures: list[Finding]) -> tuple[int, int]:
    checks_passed = 0
    checks_total = 0
    artifact = "runtime_probe"

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_passed, checks_total
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)

    registration_state = probe_payload.get("registration_state", {})
    graph_state = probe_payload.get("graph_state", {})
    root_entry = probe_payload.get("root_entry", {})
    widget_entry = probe_payload.get("widget_entry", {})
    root_class_state = probe_payload.get("root_class_state", {})
    widget_class_state = probe_payload.get("widget_class_state", {})
    widget_known_class_state = probe_payload.get("widget_known_class_state", {})
    widget_inherited_state = probe_payload.get("widget_inherited_state", {})
    widget_own_state = probe_payload.get("widget_own_state", {})
    root_shared_entry = probe_payload.get("root_shared_entry", {})
    widget_shared_entry = probe_payload.get("widget_shared_entry", {})
    widget_inherited_entry = probe_payload.get("widget_inherited_entry", {})
    widget_own_entry = probe_payload.get("widget_own_entry", {})

    check(probe_payload.get("root_class_value") == 19,
          "M256-D002-DYN-PAY-01", "RootObject class dispatch must return 19")
    check(probe_payload.get("widget_class_value") == 19,
          "M256-D002-DYN-PAY-02", "Widget class dispatch must inherit RootObject::shared")
    check(probe_payload.get("widget_known_class_value") == 19,
          "M256-D002-DYN-PAY-03", "known-class receiver must share the metaclass result")
    check(probe_payload.get("widget_inherited_instance_value") == 17,
          "M256-D002-DYN-PAY-04", "Widget instance dispatch must inherit RootObject::rootValue")
    check(probe_payload.get("widget_own_instance_value") == 23,
          "M256-D002-DYN-PAY-05", "Widget instance dispatch must resolve Widget::widgetValue")

    check(registration_state.get("registered_image_count") == 1,
          "M256-D002-DYN-PAY-06", "probe should register exactly one image")
    check(registration_state.get("registered_descriptor_total") == 4,
          "M256-D002-DYN-PAY-07", "probe should register four class descriptors")
    check(registration_state.get("last_registration_status") == 0,
          "M256-D002-DYN-PAY-08", "registration status must be OK")
    check(registration_state.get("last_registered_module_name") == "metaclassGraphRootClassRuntimeLibrary",
          "M256-D002-DYN-PAY-09", "module name must match the root-class library")

    check(graph_state.get("realized_class_count") == 2,
          "M256-D002-DYN-PAY-10", "runtime should publish two realized classes")
    check(graph_state.get("root_class_count") == 1,
          "M256-D002-DYN-PAY-11", "runtime should publish one root class")
    check(graph_state.get("metaclass_edge_count") == 1,
          "M256-D002-DYN-PAY-12", "runtime should publish one metaclass edge")
    check(graph_state.get("receiver_class_binding_count") == 2,
          "M256-D002-DYN-PAY-13", "runtime should publish two receiver-class bindings")
    check(graph_state.get("last_realized_class_name") == "Widget",
          "M256-D002-DYN-PAY-14", "Widget should be the last realized class in the single-image graph")

    check(root_entry.get("found") == 1,
          "M256-D002-DYN-PAY-15", "RootObject entry must be published")
    check(root_entry.get("base_identity") == 1024,
          "M256-D002-DYN-PAY-16", "RootObject base identity must be 1024")
    check(root_entry.get("is_root_class") == 1,
          "M256-D002-DYN-PAY-17", "RootObject must be marked as the root class")
    check(root_entry.get("implementation_backed") == 1,
          "M256-D002-DYN-PAY-18", "RootObject must be implementation-backed")
    check(root_entry.get("class_owner_identity") == "class:RootObject",
          "M256-D002-DYN-PAY-19", "RootObject class owner identity must be preserved")
    check(root_entry.get("metaclass_owner_identity") == "metaclass:RootObject",
          "M256-D002-DYN-PAY-20", "RootObject metaclass owner identity must be preserved")
    check(root_entry.get("super_class_owner_identity") is None,
          "M256-D002-DYN-PAY-21", "RootObject root baseline must retain a null superclass owner")
    check(root_entry.get("super_metaclass_owner_identity") is None,
          "M256-D002-DYN-PAY-22", "RootObject root baseline must retain a null super-metaclass owner")

    check(widget_entry.get("found") == 1,
          "M256-D002-DYN-PAY-23", "Widget entry must be published")
    check(widget_entry.get("base_identity") == 1041,
          "M256-D002-DYN-PAY-24", "Widget base identity must be 1041")
    check(widget_entry.get("is_root_class") == 0,
          "M256-D002-DYN-PAY-25", "Widget must not be marked as a root class")
    check(widget_entry.get("class_owner_identity") == "class:Widget",
          "M256-D002-DYN-PAY-26", "Widget class owner identity must be preserved")
    check(widget_entry.get("metaclass_owner_identity") == "metaclass:Widget",
          "M256-D002-DYN-PAY-27", "Widget metaclass owner identity must be preserved")
    check(widget_entry.get("super_class_owner_identity") == "class:RootObject",
          "M256-D002-DYN-PAY-28", "Widget must point at RootObject as its realized superclass")
    check(widget_entry.get("super_metaclass_owner_identity") == "metaclass:RootObject",
          "M256-D002-DYN-PAY-29", "Widget must point at RootObject metaclass as its realized super-metaclass")

    check(root_class_state.get("last_normalized_receiver_identity") == 1026,
          "M256-D002-DYN-PAY-30", "RootObject class dispatch must normalize to 1026")
    check(root_class_state.get("last_dispatch_used_cache") == 0,
          "M256-D002-DYN-PAY-31", "first root class dispatch must miss the cache")
    check(root_class_state.get("last_resolved_owner_identity") == EXPECTED_ROOT_SHARED_OWNER,
          "M256-D002-DYN-PAY-32", "root class dispatch must resolve RootObject::shared")

    check(widget_class_state.get("last_normalized_receiver_identity") == 1043,
          "M256-D002-DYN-PAY-33", "Widget class dispatch must normalize to 1043")
    check(widget_class_state.get("last_dispatch_used_cache") == 0,
          "M256-D002-DYN-PAY-34", "first Widget class dispatch must miss the cache")
    check(widget_class_state.get("last_resolved_class_name") == "RootObject",
          "M256-D002-DYN-PAY-35", "Widget class dispatch must inherit through RootObject metaclass")
    check(widget_class_state.get("last_resolved_owner_identity") == EXPECTED_ROOT_SHARED_OWNER,
          "M256-D002-DYN-PAY-36", "Widget class dispatch must resolve RootObject::shared")

    check(widget_known_class_state.get("last_normalized_receiver_identity") == 1043,
          "M256-D002-DYN-PAY-37", "known-class receiver must normalize to the same metaclass identity")
    check(widget_known_class_state.get("last_dispatch_used_cache") == 1,
          "M256-D002-DYN-PAY-38", "known-class receiver must hit the metaclass cache")
    check(widget_known_class_state.get("last_resolved_owner_identity") == EXPECTED_ROOT_SHARED_OWNER,
          "M256-D002-DYN-PAY-39", "known-class receiver must preserve the inherited class-method owner")

    check(widget_inherited_state.get("last_normalized_receiver_identity") == 1042,
          "M256-D002-DYN-PAY-40", "Widget instance dispatch must normalize to 1042")
    check(widget_inherited_state.get("last_dispatch_used_cache") == 0,
          "M256-D002-DYN-PAY-41", "first Widget inherited instance dispatch must miss the cache")
    check(widget_inherited_state.get("last_resolved_class_name") == "RootObject",
          "M256-D002-DYN-PAY-42", "Widget instance dispatch must inherit RootObject::rootValue")
    check(widget_inherited_state.get("last_resolved_owner_identity") == EXPECTED_ROOT_INSTANCE_OWNER,
          "M256-D002-DYN-PAY-43", "Widget instance dispatch must resolve RootObject::rootValue")

    check(widget_own_state.get("last_normalized_receiver_identity") == 1042,
          "M256-D002-DYN-PAY-44", "Widget own method dispatch must keep the instance receiver identity")
    check(widget_own_state.get("last_resolved_class_name") == "Widget",
          "M256-D002-DYN-PAY-45", "Widget own method dispatch must resolve Widget")
    check(widget_own_state.get("last_resolved_owner_identity") == EXPECTED_WIDGET_OWNER,
          "M256-D002-DYN-PAY-46", "Widget own method dispatch must resolve Widget::widgetValue")

    check(root_shared_entry.get("resolved_owner_identity") == EXPECTED_ROOT_SHARED_OWNER,
          "M256-D002-DYN-PAY-47", "root shared cache entry must retain RootObject::shared")
    check(widget_shared_entry.get("normalized_receiver_identity") == 1043,
          "M256-D002-DYN-PAY-48", "Widget shared cache entry must retain the normalized metaclass receiver")
    check(widget_shared_entry.get("resolved_owner_identity") == EXPECTED_ROOT_SHARED_OWNER,
          "M256-D002-DYN-PAY-49", "Widget shared cache entry must retain the inherited class owner")
    check(widget_inherited_entry.get("resolved_owner_identity") == EXPECTED_ROOT_INSTANCE_OWNER,
          "M256-D002-DYN-PAY-50", "Widget inherited instance cache entry must retain RootObject::rootValue")
    check(widget_own_entry.get("resolved_owner_identity") == EXPECTED_WIDGET_OWNER,
          "M256-D002-DYN-PAY-51", "Widget own instance cache entry must retain Widget::widgetValue")

    check(runtime_manifest.get("class_descriptor_count") == 4,
          "M256-D002-DYN-PAY-52", "runtime registration manifest must preserve four class descriptors")
    check(len(module_manifest.get("interfaces", [])) == 2 and
          len(module_manifest.get("implementations", [])) == 2,
          "M256-D002-DYN-PAY-53", "module manifest must preserve two interfaces and two implementations for the realized graph fixture")
    return checks_passed, checks_total


def dynamic_checks(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_passed = 0
    checks_total = 0

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_passed, checks_total
        checks_total += 1
        checks_passed += require(condition, "dynamic", check_id, detail, failures)

    check(args.native_exe.exists(), "M256-D002-DYN-01",
          f"missing native executable: {display_path(args.native_exe)}")
    check(args.runtime_library.exists(), "M256-D002-DYN-02",
          f"missing runtime library: {display_path(args.runtime_library)}")
    check(args.fixture.exists(), "M256-D002-DYN-03",
          f"missing fixture: {display_path(args.fixture)}")
    check(args.runtime_probe.exists(), "M256-D002-DYN-04",
          f"missing runtime probe: {display_path(args.runtime_probe)}")
    clangxx = resolve_tool(args.clangxx)
    check(clangxx is not None, "M256-D002-DYN-05",
          f"unable to resolve {args.clangxx}")
    if failures:
        return checks_passed, checks_total, {"skipped": True}

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
    check(compile_result.returncode == 0, "M256-D002-DYN-06",
          f"fixture compile failed: {compile_result.stdout}{compile_result.stderr}")

    module_manifest_path = probe_dir / "module.manifest.json"
    runtime_manifest_path = probe_dir / "module.runtime-registration-manifest.json"
    module_ir = probe_dir / "module.ll"
    module_obj = probe_dir / "module.obj"
    backend_path = probe_dir / "module.object-backend.txt"
    check(module_manifest_path.exists(), "M256-D002-DYN-07",
          f"missing manifest: {display_path(module_manifest_path)}")
    check(runtime_manifest_path.exists(), "M256-D002-DYN-08",
          f"missing runtime registration manifest: {display_path(runtime_manifest_path)}")
    check(module_ir.exists(), "M256-D002-DYN-09",
          f"missing emitted IR: {display_path(module_ir)}")
    check(module_obj.exists(), "M256-D002-DYN-10",
          f"missing emitted object: {display_path(module_obj)}")
    check(backend_path.exists(), "M256-D002-DYN-11",
          f"missing backend marker: {display_path(backend_path)}")
    if failures:
        return checks_passed, checks_total, {
            "skipped": False,
            "compile_stdout": compile_result.stdout,
            "compile_stderr": compile_result.stderr,
        }

    module_manifest = json.loads(read_text(module_manifest_path))
    runtime_manifest = json.loads(read_text(runtime_manifest_path))
    backend_text = read_text(backend_path).strip()
    check(backend_text == "llvm-direct", "M256-D002-DYN-12",
          f"module.object-backend.txt must be llvm-direct, saw {backend_text!r}")

    ir_text = read_text(module_ir)
    runtime_boundary_line = boundary_line(ir_text, BOUNDARY_PREFIX)
    check(bool(runtime_boundary_line), "M256-D002-DYN-13",
          "IR must publish the runtime metaclass-graph/root-class summary")
    for check_id, token in (
        ("M256-D002-DYN-14", CONTRACT_ID),
        ("M256-D002-DYN-15", REALIZED_GRAPH_MODEL),
        ("M256-D002-DYN-16", ROOT_CLASS_BASELINE_MODEL),
        ("M256-D002-DYN-17", FAIL_CLOSED_MODEL),
        ("M256-D002-DYN-18", "class_bundle_count=4"),
        ("M256-D002-DYN-19", "receiver_binding_candidate_count=4"),
    ):
        check(token in runtime_boundary_line, check_id,
              f"runtime boundary line missing token: {token}")

    probe_exe = probe_dir / "m256_d002_metaclass_graph_root_class_probe.exe"
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
    check(probe_compile.returncode == 0, "M256-D002-DYN-20",
          f"probe compile failed: {probe_compile.stdout}{probe_compile.stderr}")
    if probe_compile.returncode != 0:
        return checks_passed, checks_total, {
            "skipped": False,
            "probe_compile_stdout": probe_compile.stdout,
            "probe_compile_stderr": probe_compile.stderr,
        }

    probe_run = run_command([str(probe_exe)], ROOT)
    check(probe_run.returncode == 0, "M256-D002-DYN-21",
          f"probe execution failed: {probe_run.stdout}{probe_run.stderr}")
    if probe_run.returncode != 0:
        return checks_passed, checks_total, {
            "skipped": False,
            "probe_stdout": probe_run.stdout,
            "probe_stderr": probe_run.stderr,
        }

    try:
        probe_payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        check(False, "M256-D002-DYN-22",
              f"probe output is not valid JSON: {exc}")
        return checks_passed, checks_total, {
            "skipped": False,
            "probe_stdout": probe_run.stdout,
            "probe_stderr": probe_run.stderr,
        }

    payload_passed, payload_total = payload_checks(
        probe_payload, module_manifest, runtime_manifest, failures
    )
    checks_passed += payload_passed
    checks_total += payload_total
    return checks_passed, checks_total, {
        "skipped": False,
        "probe_dir": display_path(probe_dir),
        "module_manifest": display_path(module_manifest_path),
        "runtime_manifest": display_path(runtime_manifest_path),
        "module_ir": display_path(module_ir),
        "module_obj": display_path(module_obj),
        "probe_exe": display_path(probe_exe),
        "backend": backend_text,
        "runtime_boundary_line": runtime_boundary_line,
        "probe_payload": probe_payload,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_passed = 0
    checks_total = 0

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
        (args.parser_cpp, PARSER_SNIPPETS),
        (args.sema_cpp, SEMA_SNIPPETS),
        (args.ir_emitter, IR_EMITTER_SNIPPETS),
        (args.runtime_header, RUNTIME_HEADER_SNIPPETS),
        (args.bootstrap_header, BOOTSTRAP_HEADER_SNIPPETS),
        (args.runtime_source, RUNTIME_CPP_SNIPPETS),
        (args.runtime_probe, PROBE_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        checks_passed += ensure_snippets(path, snippets, failures)
        checks_total += len(snippets)

    dynamic_payload: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        dynamic_passed, dynamic_total, dynamic_payload = dynamic_checks(args, failures)
        checks_passed += dynamic_passed
        checks_total += dynamic_total

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic": dynamic_payload,
    }
    args.summary.parent.mkdir(parents=True, exist_ok=True)
    args.summary.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.check_id} {finding.artifact}: {finding.detail}")
        print(f"[summary] {MODE} failed ({checks_passed}/{checks_total} checks passed)")
        print(f"[summary] wrote {display_path(args.summary)}")
        return 1

    print(f"[ok] {MODE} ({checks_passed}/{checks_total} checks passed)")
    print(f"[summary] wrote {display_path(args.summary)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

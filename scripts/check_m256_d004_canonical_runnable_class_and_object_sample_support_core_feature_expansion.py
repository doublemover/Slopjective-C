#!/usr/bin/env python3
"""Validate M256-D004 canonical runnable class/object sample support."""

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
MODE = "M256-D004"
CONTRACT_ID = "objc3c-runtime-canonical-runnable-object-sample-support/m256-d004-v1"
EXECUTION_MODEL = (
    "canonical-object-samples-use-runtime-owned-alloc-new-init-and-realized-class-dispatch"
)
PROBE_SPLIT_MODEL = (
    "metadata-rich-object-samples-prove-category-and-protocol-runtime-behavior-through-library-plus-probe-splits"
)
FAIL_CLOSED_MODEL = (
    "metadata-heavy-executable-samples-stay-library-probed-until-runtime-export-gates-open"
)
SUMMARY_RELATIVE_PATH = (
    "tmp/reports/m256/M256-D004/"
    "canonical_runnable_object_sample_support_summary.json"
)
SAMPLE_FIXTURE_SOURCE = (
    "tests/tooling/fixtures/native/m256_d004_canonical_runnable_object_sample.objc3"
)
RUNTIME_FIXTURE_SOURCE = (
    "tests/tooling/fixtures/native/"
    "m256_d004_canonical_runnable_object_runtime_library.objc3"
)
RUNTIME_PROBE_SOURCE = (
    "tests/tooling/runtime/m256_d004_canonical_runnable_object_probe.cpp"
)
BOUNDARY_PREFIX = "; runtime_canonical_runnable_object_sample_support = "
EXPECTED_SAMPLE_EXIT_CODE = 37
EXPECTED_ALLOC_OWNER = "runtime-builtin:Widget::class_method:alloc"
EXPECTED_INIT_OWNER = "runtime-builtin:Widget::instance_method:init"
EXPECTED_NEW_OWNER = "runtime-builtin:Widget::class_method:new"
EXPECTED_TRACED_OWNER = "implementation:Widget(Tracing)::instance_method:tracedValue"
EXPECTED_INHERITED_OWNER = "implementation:Base::instance_method:inheritedValue"
EXPECTED_CLASS_OWNER = "implementation:Widget::class_method:classValue"

EXPECTATIONS_SNIPPETS = (
    "# M256 Canonical Runnable Class and Object Sample Support Core Feature Expansion Expectations (D004)",
    f"Contract ID: `{CONTRACT_ID}`",
    f"`{EXECUTION_MODEL}`",
    f"`{PROBE_SPLIT_MODEL}`",
    f"`{FAIL_CLOSED_MODEL}`",
    f"`{SAMPLE_FIXTURE_SOURCE}`",
    f"`{RUNTIME_FIXTURE_SOURCE}`",
    f"`{RUNTIME_PROBE_SOURCE}`",
    f"`{SUMMARY_RELATIVE_PATH}`",
)
PACKET_SNIPPETS = (
    "# M256-D004 Canonical Runnable Class and Object Sample Support Core Feature Expansion Packet",
    "Packet: `M256-D004`",
    "Issue: `#7142`",
    "`M256-D003`",
    "Next issue",
    "`M256-E001`",
    f"`{CONTRACT_ID}`",
    f"`{SAMPLE_FIXTURE_SOURCE}`",
    f"`{RUNTIME_FIXTURE_SOURCE}`",
    f"`{RUNTIME_PROBE_SOURCE}`",
    f"`{SUMMARY_RELATIVE_PATH}`",
)
NATIVE_DOC_SNIPPETS = (
    "## Canonical runnable class and object sample support (M256-D004)",
    CONTRACT_ID,
    EXECUTION_MODEL,
    PROBE_SPLIT_MODEL,
    FAIL_CLOSED_MODEL,
    f"`{SAMPLE_FIXTURE_SOURCE}`",
    f"`{RUNTIME_FIXTURE_SOURCE}`",
    f"`{RUNTIME_PROBE_SOURCE}`",
)
LOWERING_SPEC_SNIPPETS = (
    "## M256 canonical runnable class and object sample support (D004)",
    CONTRACT_ID,
    EXECUTION_MODEL,
    PROBE_SPLIT_MODEL,
    FAIL_CLOSED_MODEL,
    "check:objc3c:m256-d004-canonical-runnable-class-and-object-sample-support",
    "check:objc3c:m256-d004-lane-d-readiness",
)
METADATA_SPEC_SNIPPETS = (
    "## M256 canonical runnable class and object sample anchors (D004)",
    CONTRACT_ID,
    "`; runtime_canonical_runnable_object_sample_support = contract=objc3c-runtime-canonical-runnable-object-sample-support/m256-d004-v1`",
    f"`{SAMPLE_FIXTURE_SOURCE}`",
    f"`{RUNTIME_FIXTURE_SOURCE}`",
    f"`{RUNTIME_PROBE_SOURCE}`",
    f"`{SUMMARY_RELATIVE_PATH}`",
)
ARCHITECTURE_SNIPPETS = (
    "## M256 canonical runnable class and object sample support (D004)",
    "runtime-owned builtin `alloc` / `new` / `init` resolution now closes the",
    "metadata-rich category/protocol behavior remains proven through a dedicated",
    "check:objc3c:m256-d004-lane-d-readiness",
)
RUNTIME_README_SNIPPETS = (
    "`M256-D004` closes the smallest truthful executable object sample above that",
    CONTRACT_ID,
    EXECUTION_MODEL,
    PROBE_SPLIT_MODEL,
    "objc3_runtime_copy_method_cache_entry_for_testing",
)
TOOLING_RUNTIME_README_SNIPPETS = (
    "`M256-D004` adds the canonical runnable object sample split above the same",
    CONTRACT_ID,
    SAMPLE_FIXTURE_SOURCE,
    RUNTIME_FIXTURE_SOURCE,
    RUNTIME_PROBE_SOURCE,
)
LOWERING_HEADER_SNIPPETS = (
    "kObjc3RuntimeCanonicalRunnableObjectSampleSupportContractId",
    "kObjc3RuntimeCanonicalRunnableObjectExecutionModel",
    "kObjc3RuntimeCanonicalRunnableObjectProbeSplitModel",
    "kObjc3RuntimeCanonicalRunnableObjectFailClosedModel",
    "Objc3RuntimeCanonicalRunnableObjectSampleSupportSummary();",
)
LOWERING_CPP_SNIPPETS = (
    "std::string Objc3RuntimeCanonicalRunnableObjectSampleSupportSummary()",
    'out << "contract="',
    '<< ";execution_model="',
    '<< ";probe_split_model="',
    '<< ";fail_closed_model="',
)
PARSER_SNIPPETS = (
    "M256-D004 canonical-runnable-object-sample anchor",
    "[[Widget alloc] init]",
    "runtime-owned builtin alloc/new/init boundary truthfully",
)
SEMA_SNIPPETS = (
    "M256-D004 canonical-runnable-object-sample anchor",
    "runtime-built alloc/new/init on",
    "builtin alloc/new support",
)
IR_EMITTER_SNIPPETS = (
    "M256-D004 canonical-runnable-object-sample anchor",
    "; runtime_canonical_runnable_object_sample_support = ",
    '<< ";builtin_object_sample_selector_count=3\\n";',
)
RUNTIME_HEADER_SNIPPETS = (
    "M256-D004 canonical-runnable-object-sample anchor",
    "alloc/new/init resolution and inherited class dispatch for canonical object",
)
BOOTSTRAP_HEADER_SNIPPETS = (
    "M256-D004 canonical-runnable-object-sample anchor",
    "builtin alloc/new/init ownership can be observed",
)
RUNTIME_CPP_SNIPPETS = (
    "TryResolveRuntimeBuiltinObjectSampleMethod(",
    "InvokeRuntimeBuiltinMethod(",
    "runtime-builtin:",
)
SAMPLE_FIXTURE_SNIPPETS = (
    "module canonicalRunnableObjectSample;",
    "@interface Base",
    "@interface Widget : Base",
    "let widget = [[Widget alloc] init];",
    "return [widget value] + [Widget shared] + [Widget classValue];",
)
RUNTIME_FIXTURE_SNIPPETS = (
    "module canonicalRunnableObjectRuntimeLibrary;",
    "@protocol Worker",
    "@protocol Tracer <Worker>",
    "@interface Widget : Base <Worker>",
    "@interface Widget (Tracing) <Tracer>",
    "return 13;",
)
PROBE_SNIPPETS = (
    '#include "runtime/objc3_runtime_bootstrap_internal.h"',
    'objc3_runtime_dispatch_i32(widget_class_receiver, "alloc", 0, 0, 0, 0)',
    'objc3_runtime_dispatch_i32(alloc_value, "init", 0, 0, 0, 0)',
    'objc3_runtime_dispatch_i32(widget_class_receiver, "new", 0, 0, 0, 0)',
    'objc3_runtime_copy_method_cache_entry_for_testing(widget_class_receiver,',
    'objc3_runtime_copy_protocol_conformance_query_for_testing(',
)
PACKAGE_SNIPPETS = (
    '"check:objc3c:m256-d004-canonical-runnable-class-and-object-sample-support": "python scripts/check_m256_d004_canonical_runnable_class_and_object_sample_support_core_feature_expansion.py"',
    '"test:tooling:m256-d004-canonical-runnable-class-and-object-sample-support": "python -m pytest tests/tooling/test_check_m256_d004_canonical_runnable_class_and_object_sample_support_core_feature_expansion.py -q"',
    '"check:objc3c:m256-d004-lane-d-readiness": "python scripts/run_m256_d004_lane_d_readiness.py"',
)
READINESS_RUNNER_SNIPPETS = (
    "M256-A001..A003 + M256-B001..B004 + M256-C001..C003 + M256-D001..D004",
    "check_m256_d003_category_attachment_and_protocol_conformance_runtime_checks_core_feature_implementation.py",
    "check_m256_d004_canonical_runnable_class_and_object_sample_support_core_feature_expansion.py",
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


def require(
    condition: bool,
    artifact: str,
    check_id: str,
    detail: str,
    failures: list[Finding],
) -> int:
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
    parser.add_argument(
        "--expectations-doc",
        type=Path,
        default=ROOT
        / "docs/contracts/m256_canonical_runnable_class_and_object_sample_support_core_feature_expansion_d004_expectations.md",
    )
    parser.add_argument(
        "--packet-doc",
        type=Path,
        default=ROOT
        / "spec/planning/compiler/m256/m256_d004_canonical_runnable_class_and_object_sample_support_core_feature_expansion_packet.md",
    )
    parser.add_argument("--native-doc", type=Path, default=ROOT / "docs/objc3c-native.md")
    parser.add_argument(
        "--lowering-spec",
        type=Path,
        default=ROOT / "spec/LOWERING_AND_RUNTIME_CONTRACTS.md",
    )
    parser.add_argument(
        "--metadata-spec",
        type=Path,
        default=ROOT / "spec/MODULE_METADATA_AND_ABI_TABLES.md",
    )
    parser.add_argument(
        "--architecture-doc",
        type=Path,
        default=ROOT / "native/objc3c/src/ARCHITECTURE.md",
    )
    parser.add_argument(
        "--runtime-readme",
        type=Path,
        default=ROOT / "native/objc3c/src/runtime/README.md",
    )
    parser.add_argument(
        "--tooling-runtime-readme",
        type=Path,
        default=ROOT / "tests/tooling/runtime/README.md",
    )
    parser.add_argument(
        "--lowering-header",
        type=Path,
        default=ROOT / "native/objc3c/src/lower/objc3_lowering_contract.h",
    )
    parser.add_argument(
        "--lowering-cpp",
        type=Path,
        default=ROOT / "native/objc3c/src/lower/objc3_lowering_contract.cpp",
    )
    parser.add_argument(
        "--parser-cpp",
        type=Path,
        default=ROOT / "native/objc3c/src/parse/objc3_parser.cpp",
    )
    parser.add_argument(
        "--sema-cpp",
        type=Path,
        default=ROOT / "native/objc3c/src/sema/objc3_semantic_passes.cpp",
    )
    parser.add_argument(
        "--ir-emitter",
        type=Path,
        default=ROOT / "native/objc3c/src/ir/objc3_ir_emitter.cpp",
    )
    parser.add_argument(
        "--runtime-header",
        type=Path,
        default=ROOT / "native/objc3c/src/runtime/objc3_runtime.h",
    )
    parser.add_argument(
        "--bootstrap-header",
        type=Path,
        default=ROOT / "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
    )
    parser.add_argument(
        "--runtime-source",
        type=Path,
        default=ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp",
    )
    parser.add_argument(
        "--sample-fixture",
        type=Path,
        default=ROOT / SAMPLE_FIXTURE_SOURCE,
    )
    parser.add_argument(
        "--runtime-fixture",
        type=Path,
        default=ROOT / RUNTIME_FIXTURE_SOURCE,
    )
    parser.add_argument(
        "--runtime-probe",
        type=Path,
        default=ROOT / RUNTIME_PROBE_SOURCE,
    )
    parser.add_argument("--package-json", type=Path, default=ROOT / "package.json")
    parser.add_argument(
        "--readiness-runner",
        type=Path,
        default=ROOT / "scripts/run_m256_d004_lane_d_readiness.py",
    )
    parser.add_argument(
        "--native-exe",
        type=Path,
        default=ROOT / "artifacts/bin/objc3c-native.exe",
    )
    parser.add_argument(
        "--runtime-library",
        type=Path,
        default=ROOT / "artifacts/lib/objc3_runtime.lib",
    )
    parser.add_argument(
        "--runtime-include-root",
        type=Path,
        default=ROOT / "native/objc3c/src",
    )
    parser.add_argument("--clangxx", default="clang++.exe")
    parser.add_argument(
        "--probe-root",
        type=Path,
        default=ROOT
        / "tmp/artifacts/compilation/objc3c-native/m256/d004-canonical-runnable-object-sample",
    )
    parser.add_argument(
        "--summary",
        type=Path,
        default=ROOT / SUMMARY_RELATIVE_PATH,
    )
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(list(argv))


def payload_checks(
    probe_payload: dict[str, Any],
    runtime_manifest: dict[str, Any],
    failures: list[Finding],
) -> tuple[int, int]:
    checks_passed = 0
    checks_total = 0
    artifact = "runtime_probe"

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_passed, checks_total
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)

    graph_state = probe_payload.get("graph_state", {})
    widget_entry = probe_payload.get("widget_entry", {})
    worker_query = probe_payload.get("worker_query", {})
    tracer_query = probe_payload.get("tracer_query", {})
    method_state = probe_payload.get("method_state", {})
    alloc_entry = probe_payload.get("alloc_entry", {})
    init_entry = probe_payload.get("init_entry", {})
    new_entry = probe_payload.get("new_entry", {})
    traced_entry = probe_payload.get("traced_entry", {})
    inherited_entry = probe_payload.get("inherited_entry", {})
    class_entry = probe_payload.get("class_entry", {})

    widget_base_identity = widget_entry.get("base_identity")
    widget_instance_receiver = probe_payload.get("widget_instance_receiver")
    widget_class_receiver = probe_payload.get("widget_class_receiver")

    for condition, check_id, detail in (
        (widget_base_identity == 1041, "M256-D004-DYN-PAY-01", "Widget base identity must stay at 1041 for the canonical runtime library"),
        (widget_instance_receiver == 1042, "M256-D004-DYN-PAY-02", "Widget instance receiver must be 1042"),
        (widget_class_receiver == 1043, "M256-D004-DYN-PAY-03", "Widget class receiver must be 1043"),
        (probe_payload.get("alloc_value") == widget_instance_receiver, "M256-D004-DYN-PAY-04", "alloc must return the canonical instance receiver"),
        (probe_payload.get("init_value") == widget_instance_receiver, "M256-D004-DYN-PAY-05", "init must return the same instance receiver"),
        (probe_payload.get("new_value") == widget_instance_receiver, "M256-D004-DYN-PAY-06", "new must return the canonical instance receiver"),
        (probe_payload.get("traced_value") == 13, "M256-D004-DYN-PAY-07", "attached category dispatch must return 13"),
        (probe_payload.get("inherited_value") == 7, "M256-D004-DYN-PAY-08", "inherited instance dispatch must return 7"),
        (probe_payload.get("class_value") == 11, "M256-D004-DYN-PAY-09", "concrete class dispatch must return 11"),
        (graph_state.get("realized_class_count") == 2, "M256-D004-DYN-PAY-10", "runtime must publish two realized classes"),
        (graph_state.get("root_class_count") == 1, "M256-D004-DYN-PAY-11", "runtime must publish one root class"),
        (graph_state.get("metaclass_edge_count") == 1, "M256-D004-DYN-PAY-12", "runtime must publish one metaclass edge"),
        (graph_state.get("receiver_class_binding_count") == 2, "M256-D004-DYN-PAY-13", "runtime must publish two receiver/class bindings"),
        (graph_state.get("attached_category_count") == 1, "M256-D004-DYN-PAY-14", "runtime must publish one attached category"),
        (graph_state.get("protocol_conformance_edge_count") == 2, "M256-D004-DYN-PAY-15", "runtime must publish two protocol-conformance edges"),
        (widget_entry.get("found") == 1, "M256-D004-DYN-PAY-16", "Widget entry must be published"),
        (widget_entry.get("attached_category_count") == 1, "M256-D004-DYN-PAY-17", "Widget must publish one attached category"),
        (widget_entry.get("direct_protocol_count") == 1, "M256-D004-DYN-PAY-18", "Widget must publish one direct protocol"),
        (widget_entry.get("attached_protocol_count") == 1, "M256-D004-DYN-PAY-19", "Widget must publish one attached-category protocol"),
        (widget_entry.get("last_attached_category_owner_identity") == "category:Widget(Tracing)", "M256-D004-DYN-PAY-20", "Widget entry must preserve the attached category owner identity"),
        (widget_entry.get("super_class_owner_identity") == "class:Base", "M256-D004-DYN-PAY-21", "Widget must point at Base as its superclass"),
        (worker_query.get("class_found") == 1 and worker_query.get("protocol_found") == 1 and worker_query.get("conforms") == 1, "M256-D004-DYN-PAY-22", "Widget -> Worker must conform"),
        (worker_query.get("matched_protocol_owner_identity") in {"protocol:Worker", "protocol:Tracer"}, "M256-D004-DYN-PAY-23", "Widget -> Worker must resolve through the direct Worker protocol or the inherited Tracer closure"),
        (worker_query.get("matched_attachment_owner_identity") is None, "M256-D004-DYN-PAY-24", "Widget -> Worker must not require an attachment owner match"),
        (tracer_query.get("class_found") == 1 and tracer_query.get("protocol_found") == 1 and tracer_query.get("conforms") == 1, "M256-D004-DYN-PAY-25", "Widget -> Tracer must conform through the category attachment"),
        (tracer_query.get("matched_protocol_owner_identity") == "protocol:Tracer", "M256-D004-DYN-PAY-26", "Widget -> Tracer must match protocol:Tracer"),
        (tracer_query.get("matched_attachment_owner_identity") == "category:Widget(Tracing)", "M256-D004-DYN-PAY-27", "Widget -> Tracer must match the attached category owner"),
        (method_state.get("cache_entry_count") == 6, "M256-D004-DYN-PAY-28", "runtime cache should retain six canonical object-sample entries"),
        (method_state.get("live_dispatch_count") == 6, "M256-D004-DYN-PAY-29", "runtime should perform six live dispatches for the probe sequence"),
        (method_state.get("fallback_dispatch_count") == 0, "M256-D004-DYN-PAY-30", "no probe dispatch should fall back"),
        (runtime_manifest.get("class_descriptor_count") == 4, "M256-D004-DYN-PAY-37", "runtime registration manifest must preserve four class descriptors"),
        (runtime_manifest.get("protocol_descriptor_count") == 2, "M256-D004-DYN-PAY-38", "runtime registration manifest must preserve two protocol descriptors"),
        (runtime_manifest.get("category_descriptor_count") == 2, "M256-D004-DYN-PAY-39", "runtime registration manifest must preserve two category descriptors"),
    ):
        check(condition, check_id, detail)

    for check_id, entry, expected_owner, expected_receiver, expected_class_flag in (
        ("M256-D004-DYN-PAY-31", alloc_entry, EXPECTED_ALLOC_OWNER, widget_class_receiver, 1),
        ("M256-D004-DYN-PAY-32", init_entry, EXPECTED_INIT_OWNER, widget_instance_receiver, 0),
        ("M256-D004-DYN-PAY-33", new_entry, EXPECTED_NEW_OWNER, widget_class_receiver, 1),
        ("M256-D004-DYN-PAY-34", traced_entry, EXPECTED_TRACED_OWNER, widget_instance_receiver, 0),
        ("M256-D004-DYN-PAY-35", inherited_entry, EXPECTED_INHERITED_OWNER, widget_instance_receiver, 0),
        ("M256-D004-DYN-PAY-36", class_entry, EXPECTED_CLASS_OWNER, widget_class_receiver, 1),
    ):
        check(entry.get("found") == 1 and entry.get("resolved") == 1, check_id, f"cache entry must be found/resolved for {expected_owner}")
        check(entry.get("dispatch_family_is_class") == expected_class_flag, f"{check_id}b", f"dispatch family mismatch for {expected_owner}")
        check(entry.get("normalized_receiver_identity") == expected_receiver, f"{check_id}c", f"normalized receiver mismatch for {expected_owner}")
        check(entry.get("resolved_owner_identity") == expected_owner, f"{check_id}d", f"owner identity mismatch for {expected_owner}")

    return checks_passed, checks_total


def dynamic_checks(
    args: argparse.Namespace,
    failures: list[Finding],
) -> tuple[int, int, dict[str, Any]]:
    checks_passed = 0
    checks_total = 0

    def check(condition: bool, check_id: str, detail: str) -> None:
        nonlocal checks_passed, checks_total
        checks_total += 1
        checks_passed += require(condition, "dynamic", check_id, detail, failures)

    check(args.native_exe.exists(), "M256-D004-DYN-01", f"missing native executable: {display_path(args.native_exe)}")
    check(args.runtime_library.exists(), "M256-D004-DYN-02", f"missing runtime library: {display_path(args.runtime_library)}")
    check(args.sample_fixture.exists(), "M256-D004-DYN-03", f"missing executable sample fixture: {display_path(args.sample_fixture)}")
    check(args.runtime_fixture.exists(), "M256-D004-DYN-04", f"missing runtime library fixture: {display_path(args.runtime_fixture)}")
    check(args.runtime_probe.exists(), "M256-D004-DYN-05", f"missing runtime probe: {display_path(args.runtime_probe)}")
    clangxx = resolve_tool(args.clangxx)
    check(clangxx is not None, "M256-D004-DYN-06", f"unable to resolve {args.clangxx}")
    if failures:
        return checks_passed, checks_total, {"skipped": True}

    sample_dir = args.probe_root.resolve() / f"sample-{uuid.uuid4().hex}"
    runtime_dir = args.probe_root.resolve() / f"probe-{uuid.uuid4().hex}"
    sample_dir.mkdir(parents=True, exist_ok=True)
    runtime_dir.mkdir(parents=True, exist_ok=True)

    sample_compile = run_command([
        str(args.native_exe),
        str(args.sample_fixture),
        "--out-dir",
        str(sample_dir),
        "--emit-prefix",
        "module",
    ], ROOT)
    check(sample_compile.returncode == 0, "M256-D004-DYN-07", f"sample compile failed: {sample_compile.stdout}{sample_compile.stderr}")

    sample_manifest_path = sample_dir / "module.manifest.json"
    sample_runtime_manifest_path = sample_dir / "module.runtime-registration-manifest.json"
    sample_ir_path = sample_dir / "module.ll"
    sample_obj_path = sample_dir / "module.obj"
    sample_backend_path = sample_dir / "module.object-backend.txt"
    for check_id, path in (
        ("M256-D004-DYN-08", sample_manifest_path),
        ("M256-D004-DYN-09", sample_runtime_manifest_path),
        ("M256-D004-DYN-10", sample_ir_path),
        ("M256-D004-DYN-11", sample_obj_path),
        ("M256-D004-DYN-12", sample_backend_path),
    ):
        check(path.exists(), check_id, f"missing sample artifact: {display_path(path)}")

    sample_exe = sample_dir / "m256_d004_canonical_runnable_object_sample.exe"
    sample_link = run_command([
        str(clangxx),
        str(sample_obj_path),
        str(args.runtime_library),
        "-o",
        str(sample_exe),
    ], ROOT)
    check(sample_link.returncode == 0, "M256-D004-DYN-13", f"sample link failed: {sample_link.stdout}{sample_link.stderr}")
    sample_run = run_command([str(sample_exe)], ROOT)
    check(sample_run.returncode == EXPECTED_SAMPLE_EXIT_CODE, "M256-D004-DYN-14", f"sample must exit with {EXPECTED_SAMPLE_EXIT_CODE}, saw {sample_run.returncode}")

    runtime_compile = run_command([
        str(args.native_exe),
        str(args.runtime_fixture),
        "--out-dir",
        str(runtime_dir),
        "--emit-prefix",
        "module",
    ], ROOT)
    check(runtime_compile.returncode == 0, "M256-D004-DYN-15", f"runtime fixture compile failed: {runtime_compile.stdout}{runtime_compile.stderr}")

    runtime_manifest_path = runtime_dir / "module.manifest.json"
    runtime_registration_manifest_path = runtime_dir / "module.runtime-registration-manifest.json"
    runtime_ir_path = runtime_dir / "module.ll"
    runtime_obj_path = runtime_dir / "module.obj"
    runtime_backend_path = runtime_dir / "module.object-backend.txt"
    for check_id, path in (
        ("M256-D004-DYN-16", runtime_manifest_path),
        ("M256-D004-DYN-17", runtime_registration_manifest_path),
        ("M256-D004-DYN-18", runtime_ir_path),
        ("M256-D004-DYN-19", runtime_obj_path),
        ("M256-D004-DYN-20", runtime_backend_path),
    ):
        check(path.exists(), check_id, f"missing runtime artifact: {display_path(path)}")

    if failures:
        return checks_passed, checks_total, {
            "skipped": False,
            "sample_compile_stdout": sample_compile.stdout,
            "sample_compile_stderr": sample_compile.stderr,
            "sample_link_stdout": sample_link.stdout,
            "sample_link_stderr": sample_link.stderr,
            "runtime_compile_stdout": runtime_compile.stdout,
            "runtime_compile_stderr": runtime_compile.stderr,
        }

    sample_manifest = json.loads(read_text(sample_manifest_path))
    sample_runtime_manifest = json.loads(read_text(sample_runtime_manifest_path))
    runtime_manifest = json.loads(read_text(runtime_manifest_path))
    runtime_registration_manifest = json.loads(read_text(runtime_registration_manifest_path))
    sample_backend = read_text(sample_backend_path).strip()
    runtime_backend = read_text(runtime_backend_path).strip()
    check(sample_backend == "llvm-direct", "M256-D004-DYN-21", f"sample backend must be llvm-direct, saw {sample_backend!r}")
    check(runtime_backend == "llvm-direct", "M256-D004-DYN-22", f"runtime library backend must be llvm-direct, saw {runtime_backend!r}")

    sample_boundary_line = boundary_line(read_text(sample_ir_path), BOUNDARY_PREFIX)
    runtime_boundary_line = boundary_line(read_text(runtime_ir_path), BOUNDARY_PREFIX)
    check(bool(sample_boundary_line), "M256-D004-DYN-23", "sample IR must publish the canonical runnable object sample summary")
    check(bool(runtime_boundary_line), "M256-D004-DYN-24", "runtime library IR must publish the canonical runnable object sample summary")
    for check_id, token in (
        ("M256-D004-DYN-25", CONTRACT_ID),
        ("M256-D004-DYN-26", EXECUTION_MODEL),
        ("M256-D004-DYN-27", PROBE_SPLIT_MODEL),
        ("M256-D004-DYN-28", FAIL_CLOSED_MODEL),
        ("M256-D004-DYN-29", "builtin_object_sample_selector_count=3"),
    ):
        check(token in sample_boundary_line and token in runtime_boundary_line, check_id, f"canonical runtime boundary lines must include token: {token}")
    check("class_bundle_count=4" in sample_boundary_line, "M256-D004-DYN-30", "sample runtime boundary line must report four class bundles")
    check("attached_category_candidate_count=0" in sample_boundary_line, "M256-D004-DYN-31", "sample runtime boundary line must report zero attached categories")
    check("attached_category_candidate_count=" in runtime_boundary_line, "M256-D004-DYN-32", "runtime library boundary line must publish attached category candidate count")
    check(sample_runtime_manifest.get("class_descriptor_count") == 4, "M256-D004-DYN-33", "sample runtime registration manifest must preserve four class descriptors")
    check(sample_runtime_manifest.get("protocol_descriptor_count") == 0, "M256-D004-DYN-34", "sample runtime registration manifest must preserve zero protocol descriptors")
    check(sample_runtime_manifest.get("category_descriptor_count") == 0, "M256-D004-DYN-35", "sample runtime registration manifest must preserve zero category descriptors")
    check(len(sample_manifest.get("interfaces", [])) == 2 and len(sample_manifest.get("implementations", [])) == 2, "M256-D004-DYN-36", "sample manifest must preserve two interfaces and two implementations")

    probe_exe = runtime_dir / "m256_d004_canonical_runnable_object_probe.exe"
    probe_compile = run_command([
        str(clangxx),
        "-std=c++20",
        "-I",
        str(args.runtime_include_root),
        str(args.runtime_probe),
        str(runtime_obj_path),
        str(args.runtime_library),
        "-o",
        str(probe_exe),
    ], ROOT)
    check(probe_compile.returncode == 0, "M256-D004-DYN-37", f"probe compile failed: {probe_compile.stdout}{probe_compile.stderr}")
    if probe_compile.returncode != 0:
        return checks_passed, checks_total, {
            "skipped": False,
            "sample_dir": display_path(sample_dir),
            "runtime_dir": display_path(runtime_dir),
            "probe_compile_stdout": probe_compile.stdout,
            "probe_compile_stderr": probe_compile.stderr,
        }

    probe_run = run_command([str(probe_exe)], ROOT)
    check(probe_run.returncode == 0, "M256-D004-DYN-38", f"probe execution failed: {probe_run.stdout}{probe_run.stderr}")
    if probe_run.returncode != 0:
        return checks_passed, checks_total, {
            "skipped": False,
            "sample_dir": display_path(sample_dir),
            "runtime_dir": display_path(runtime_dir),
            "probe_stdout": probe_run.stdout,
            "probe_stderr": probe_run.stderr,
        }

    try:
        probe_payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        check(False, "M256-D004-DYN-39", f"probe output is not valid JSON: {exc}")
        return checks_passed, checks_total, {
            "skipped": False,
            "sample_dir": display_path(sample_dir),
            "runtime_dir": display_path(runtime_dir),
            "probe_stdout": probe_run.stdout,
            "probe_stderr": probe_run.stderr,
        }

    payload_passed, payload_total = payload_checks(probe_payload, runtime_registration_manifest, failures)
    checks_passed += payload_passed
    checks_total += payload_total
    return checks_passed, checks_total, {
        "skipped": False,
        "sample_dir": display_path(sample_dir),
        "runtime_dir": display_path(runtime_dir),
        "sample_backend": sample_backend,
        "runtime_backend": runtime_backend,
        "sample_boundary_line": sample_boundary_line,
        "runtime_boundary_line": runtime_boundary_line,
        "sample_exit_code": sample_run.returncode,
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
        (args.sample_fixture, SAMPLE_FIXTURE_SNIPPETS),
        (args.runtime_fixture, RUNTIME_FIXTURE_SNIPPETS),
        (args.runtime_probe, PROBE_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
        (args.readiness_runner, READINESS_RUNNER_SNIPPETS),
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

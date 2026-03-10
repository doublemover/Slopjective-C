#!/usr/bin/env python3
"""Validate M256-D003 category attachment and protocol conformance runtime checks."""

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
MODE = "M256-D003"
CONTRACT_ID = "objc3c-runtime-category-attachment-protocol-conformance/m256-d003-v1"
CATEGORY_ATTACHMENT_MODEL = (
    "realized-class-nodes-own-preferred-category-attachments-after-registration"
)
PROTOCOL_CONFORMANCE_QUERY_MODEL = (
    "runtime-protocol-conformance-queries-walk-class-category-and-inherited-protocol-closures"
)
FAIL_CLOSED_MODEL = (
    "invalid-attachment-owner-identities-or-broken-protocol-refs-disable-runtime-attachment-queries"
)
SUMMARY_RELATIVE_PATH = (
    "tmp/reports/m256/M256-D003/"
    "category_attachment_protocol_conformance_runtime_checks_summary.json"
)
FIXTURE_SOURCE = (
    "tests/tooling/fixtures/native/"
    "m256_d003_category_attachment_protocol_runtime_library.objc3"
)
RUNTIME_PROBE_SOURCE = (
    "tests/tooling/runtime/"
    "m256_d003_category_attachment_protocol_runtime_probe.cpp"
)
BOUNDARY_PREFIX = "; runtime_category_attachment_protocol_conformance = "

EXPECTATIONS_SNIPPETS = (
    "# M256 Category Attachment and Protocol Conformance Runtime Checks Core Feature Implementation Expectations (D003)",
    f"Contract ID: `{CONTRACT_ID}`",
    f"`{CATEGORY_ATTACHMENT_MODEL}`",
    f"`{PROTOCOL_CONFORMANCE_QUERY_MODEL}`",
    f"`{FAIL_CLOSED_MODEL}`",
    f"`{FIXTURE_SOURCE}`",
    f"`{RUNTIME_PROBE_SOURCE}`",
    f"`{SUMMARY_RELATIVE_PATH}`",
)
PACKET_SNIPPETS = (
    "# M256-D003 Category Attachment and Protocol Conformance Runtime Checks Core Feature Implementation Packet",
    "Packet: `M256-D003`",
    "Issue: `#7141`",
    "## Dependencies",
    "`M256-D002`",
    "`M256-B003`",
    "Next issue",
    "`M256-D004`",
    f"`{CONTRACT_ID}`",
    f"`{FIXTURE_SOURCE}`",
    f"`{RUNTIME_PROBE_SOURCE}`",
    f"`{SUMMARY_RELATIVE_PATH}`",
)
NATIVE_DOC_SNIPPETS = (
    "## Category attachment and protocol conformance runtime checks (M256-D003)",
    CONTRACT_ID,
    CATEGORY_ATTACHMENT_MODEL,
    PROTOCOL_CONFORMANCE_QUERY_MODEL,
    FAIL_CLOSED_MODEL,
    f"`{RUNTIME_PROBE_SOURCE}`",
)
LOWERING_SPEC_SNIPPETS = (
    "## M256 category attachment and protocol conformance runtime checks (D003)",
    CONTRACT_ID,
    CATEGORY_ATTACHMENT_MODEL,
    PROTOCOL_CONFORMANCE_QUERY_MODEL,
    FAIL_CLOSED_MODEL,
    "check:objc3c:m256-d003-category-attachment-protocol-conformance-runtime-checks",
    "check:objc3c:m256-d003-lane-d-readiness",
)
METADATA_SPEC_SNIPPETS = (
    "## M256 runtime category attachment and protocol conformance anchors (D003)",
    CONTRACT_ID,
    "`; runtime_category_attachment_protocol_conformance = contract=objc3c-runtime-category-attachment-protocol-conformance/m256-d003-v1`",
    f"`{FIXTURE_SOURCE}`",
    f"`{RUNTIME_PROBE_SOURCE}`",
    f"`{SUMMARY_RELATIVE_PATH}`",
)
ARCHITECTURE_SNIPPETS = (
    "## M256 category attachment and protocol conformance runtime checks (D003)",
    "runtime attaches preferred category records onto realized class nodes",
    "runtime protocol-conformance queries walk direct class refs, attached",
    "check:objc3c:m256-d003-lane-d-readiness",
)
RUNTIME_README_SNIPPETS = (
    "`M256-D003` extends that runtime-owned realized graph into live category",
    CONTRACT_ID,
    CATEGORY_ATTACHMENT_MODEL,
    PROTOCOL_CONFORMANCE_QUERY_MODEL,
    "objc3_runtime_copy_protocol_conformance_query_for_testing",
)
TOOLING_RUNTIME_README_SNIPPETS = (
    "`M256-D003` proves attached-category dispatch and runtime protocol conformance",
    CONTRACT_ID,
    RUNTIME_PROBE_SOURCE,
    FIXTURE_SOURCE,
)
LOWERING_HEADER_SNIPPETS = (
    "kObjc3RuntimeCategoryAttachmentProtocolConformanceContractId",
    "kObjc3RuntimeCategoryAttachmentRealizedGraphModel",
    "kObjc3RuntimeProtocolConformanceQueryModel",
    "kObjc3RuntimeAttachmentConformanceFailClosedModel",
    "Objc3RuntimeCategoryAttachmentProtocolConformanceSummary();",
)
LOWERING_CPP_SNIPPETS = (
    "std::string Objc3RuntimeCategoryAttachmentProtocolConformanceSummary()",
    'out << "contract="',
    '<< ";category_attachment_model="',
    '<< ";protocol_conformance_query_model="',
    '<< ";fail_closed_model="',
)
PARSER_SNIPPETS = (
    "M256-D003 category-attachment-protocol-conformance anchor",
    "direct class/category adopted-protocol spellings stable so runtime queries",
)
SEMA_SNIPPETS = (
    "M256-D003 category-attachment-protocol-conformance anchor",
    "the adopted-protocol closure and deterministic category merge surface that",
)
IR_EMITTER_SNIPPETS = (
    "M256-D003 category-attachment-protocol-conformance anchor",
    "; runtime_category_attachment_protocol_conformance = ",
    '<< ";attached_category_candidate_count="',
    '<< ";class_protocol_ref_count="',
    '<< ";category_protocol_ref_count="',
)
RUNTIME_HEADER_SNIPPETS = (
    "M256-D003 category-attachment-protocol-conformance anchor",
    "attachment and runtime protocol-conformance queries must continue to consume",
)
BOOTSTRAP_HEADER_SNIPPETS = (
    "M256-D003 category-attachment-protocol-conformance anchor",
    "objc3_runtime_protocol_conformance_query_snapshot",
    "objc3_runtime_copy_protocol_conformance_query_for_testing",
    "attached_category_count",
    "protocol_conformance_edge_count",
)
RUNTIME_CPP_SNIPPETS = (
    "M256-D003 category-attachment-protocol-conformance anchor: realized class",
    "AttachRealizedCategoryRecordsUnlocked",
    "QueryRealizedClassProtocolConformanceUnlocked",
    "objc3_runtime_copy_protocol_conformance_query_for_testing",
)
PROBE_SNIPPETS = (
    '#include "runtime/objc3_runtime_bootstrap_internal.h"',
    'objc3_runtime_dispatch_i32(1042, "tracedValue", 0, 0, 0, 0)',
    'objc3_runtime_dispatch_i32(1043, "classValue", 0, 0, 0, 0)',
    'objc3_runtime_dispatch_i32(1042, "ignoredValue", 0, 0, 0, 0)',
    'objc3_runtime_copy_protocol_conformance_query_for_testing(',
    '"Widget", "Tracer"',
    '"Base", "Worker"',
)
FIXTURE_SNIPPETS = (
    "module categoryAttachmentProtocolRuntimeLibrary;",
    "@protocol Worker",
    "@protocol Tracer <Worker>",
    "@interface Widget : Base <Worker>",
    "@interface Widget (Tracing) <Tracer>",
    "return 13;",
)
PACKAGE_SNIPPETS = (
    '"check:objc3c:m256-d003-category-attachment-protocol-conformance-runtime-checks": "python scripts/check_m256_d003_category_attachment_and_protocol_conformance_runtime_checks_core_feature_implementation.py"',
    '"test:tooling:m256-d003-category-attachment-protocol-conformance-runtime-checks": "python -m pytest tests/tooling/test_check_m256_d003_category_attachment_and_protocol_conformance_runtime_checks_core_feature_implementation.py -q"',
    '"check:objc3c:m256-d003-lane-d-readiness": "python scripts/run_m256_d003_lane_d_readiness.py"',
)
READINESS_RUNNER_SNIPPETS = (
    "check_m256_d002_metaclass_graph_and_root_class_baseline_core_feature_implementation.py",
    "check_m256_d003_category_attachment_and_protocol_conformance_runtime_checks_core_feature_implementation.py",
    "M256-A001..A003 + M256-B001..B004 + M256-C001..C003 + M256-D001..D003",
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
        / "docs/contracts/m256_category_attachment_and_protocol_conformance_runtime_checks_core_feature_implementation_d003_expectations.md",
    )
    parser.add_argument(
        "--packet-doc",
        type=Path,
        default=ROOT
        / "spec/planning/compiler/m256/m256_d003_category_attachment_and_protocol_conformance_runtime_checks_core_feature_implementation_packet.md",
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
        "--runtime-probe",
        type=Path,
        default=ROOT / RUNTIME_PROBE_SOURCE,
    )
    parser.add_argument("--fixture", type=Path, default=ROOT / FIXTURE_SOURCE)
    parser.add_argument("--package-json", type=Path, default=ROOT / "package.json")
    parser.add_argument(
        "--readiness-runner",
        type=Path,
        default=ROOT / "scripts/run_m256_d003_lane_d_readiness.py",
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
        / "tmp/artifacts/compilation/objc3c-native/m256/d003-category-attachment-protocol-runtime",
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
    module_manifest: dict[str, Any],
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
    base_entry = probe_payload.get("base_entry", {})
    worker_query = probe_payload.get("worker_query", {})
    tracer_query = probe_payload.get("tracer_query", {})
    base_worker_query = probe_payload.get("base_worker_query", {})
    method_state = probe_payload.get("method_state", {})

    check(
        probe_payload.get("category_value") == 13,
        "M256-D003-DYN-PAY-01",
        "attached category dispatch must return 13",
    )
    check(
        probe_payload.get("class_value") == 11,
        "M256-D003-DYN-PAY-02",
        "class dispatch must return 11",
    )
    check(
        probe_payload.get("protocol_fallback")
        == probe_payload.get("protocol_fallback_expected"),
        "M256-D003-DYN-PAY-03",
        "protocol-miss dispatch must fail closed to compatibility fallback",
    )

    check(
        graph_state.get("realized_class_count") == 2,
        "M256-D003-DYN-PAY-04",
        "runtime must publish two realized classes",
    )
    check(
        graph_state.get("root_class_count") == 1,
        "M256-D003-DYN-PAY-05",
        "runtime must publish one root class",
    )
    check(
        graph_state.get("metaclass_edge_count") == 1,
        "M256-D003-DYN-PAY-06",
        "runtime must publish one metaclass edge",
    )
    check(
        graph_state.get("receiver_class_binding_count") == 2,
        "M256-D003-DYN-PAY-07",
        "runtime must publish two receiver/class bindings",
    )
    check(
        graph_state.get("attached_category_count") == 1,
        "M256-D003-DYN-PAY-08",
        "runtime must publish one attached category",
    )
    check(
        graph_state.get("protocol_conformance_edge_count") == 2,
        "M256-D003-DYN-PAY-09",
        "runtime must publish two protocol-conformance edges",
    )
    check(
        graph_state.get("last_realized_class_name") == "Widget",
        "M256-D003-DYN-PAY-10",
        "Widget must be the last realized class in the single-image graph",
    )
    check(
        graph_state.get("last_realized_class_owner_identity") == "class:Widget",
        "M256-D003-DYN-PAY-11",
        "graph state must preserve Widget class owner identity",
    )
    check(
        graph_state.get("last_realized_metaclass_owner_identity") == "metaclass:Widget",
        "M256-D003-DYN-PAY-12",
        "graph state must preserve Widget metaclass owner identity",
    )
    check(
        graph_state.get("last_attached_category_owner_identity")
        == "category:Widget(Tracing)",
        "M256-D003-DYN-PAY-13",
        "graph state must preserve the attached category owner identity",
    )
    check(
        graph_state.get("last_attached_category_name") == "Tracing",
        "M256-D003-DYN-PAY-14",
        "graph state must preserve the attached category name",
    )

    check(widget_entry.get("found") == 1, "M256-D003-DYN-PAY-15", "Widget entry must be published")
    check(widget_entry.get("base_identity") == 1041, "M256-D003-DYN-PAY-16", "Widget base identity must be 1041")
    check(widget_entry.get("is_root_class") == 0, "M256-D003-DYN-PAY-17", "Widget must not be a root class")
    check(widget_entry.get("implementation_backed") == 1, "M256-D003-DYN-PAY-18", "Widget must be implementation-backed")
    check(widget_entry.get("attached_category_count") == 1, "M256-D003-DYN-PAY-19", "Widget must retain one attached category")
    check(widget_entry.get("direct_protocol_count") == 1, "M256-D003-DYN-PAY-20", "Widget must retain one direct protocol")
    check(widget_entry.get("attached_protocol_count") == 1, "M256-D003-DYN-PAY-21", "Widget must retain one attached-category protocol")
    check(widget_entry.get("class_owner_identity") == "class:Widget", "M256-D003-DYN-PAY-22", "Widget class owner identity must be preserved")
    check(widget_entry.get("metaclass_owner_identity") == "metaclass:Widget", "M256-D003-DYN-PAY-23", "Widget metaclass owner identity must be preserved")
    check(widget_entry.get("super_class_owner_identity") == "class:Base", "M256-D003-DYN-PAY-24", "Widget must point at Base as its superclass")
    check(widget_entry.get("last_attached_category_owner_identity") == "category:Widget(Tracing)", "M256-D003-DYN-PAY-25", "Widget entry must preserve attached category owner identity")
    check(widget_entry.get("last_attached_category_name") == "Tracing", "M256-D003-DYN-PAY-26", "Widget entry must preserve attached category name")

    check(base_entry.get("found") == 1, "M256-D003-DYN-PAY-27", "Base entry must be published")
    check(base_entry.get("base_identity") == 1024, "M256-D003-DYN-PAY-28", "Base base identity must be 1024")
    check(base_entry.get("is_root_class") == 1, "M256-D003-DYN-PAY-29", "Base must remain the root class")
    check(base_entry.get("attached_category_count") == 0, "M256-D003-DYN-PAY-30", "Base must not retain attached categories")
    check(base_entry.get("direct_protocol_count") == 0, "M256-D003-DYN-PAY-31", "Base must not retain direct protocols")
    check(base_entry.get("attached_protocol_count") == 0, "M256-D003-DYN-PAY-32", "Base must not retain attached protocols")

    check(worker_query.get("class_found") == 1, "M256-D003-DYN-PAY-33", "Widget protocol query must find the class")
    check(worker_query.get("protocol_found") == 1, "M256-D003-DYN-PAY-34", "Worker protocol query must find the protocol")
    check(worker_query.get("conforms") == 1, "M256-D003-DYN-PAY-35", "Widget must conform to Worker")
    check(worker_query.get("visited_protocol_count") == 1, "M256-D003-DYN-PAY-36", "Worker query must visit one protocol")
    check(worker_query.get("attached_category_count") == 1, "M256-D003-DYN-PAY-37", "Worker query must observe one attached category")
    check(worker_query.get("matched_protocol_owner_identity") == "protocol:Worker", "M256-D003-DYN-PAY-38", "Worker query must match protocol:Worker")
    check(worker_query.get("matched_attachment_owner_identity") is None, "M256-D003-DYN-PAY-39", "Worker query must not require an attachment owner match")

    check(tracer_query.get("class_found") == 1, "M256-D003-DYN-PAY-40", "Tracer query must find the class")
    check(tracer_query.get("protocol_found") == 1, "M256-D003-DYN-PAY-41", "Tracer query must find the protocol")
    check(tracer_query.get("conforms") == 1, "M256-D003-DYN-PAY-42", "Widget must conform to Tracer through the category attachment")
    check(tracer_query.get("visited_protocol_count") == 2, "M256-D003-DYN-PAY-43", "Tracer query must walk the inherited protocol closure")
    check(tracer_query.get("attached_category_count") == 1, "M256-D003-DYN-PAY-44", "Tracer query must observe one attached category")
    check(tracer_query.get("matched_protocol_owner_identity") == "protocol:Tracer", "M256-D003-DYN-PAY-45", "Tracer query must match protocol:Tracer")
    check(tracer_query.get("matched_attachment_owner_identity") == "category:Widget(Tracing)", "M256-D003-DYN-PAY-46", "Tracer query must match the attached category owner")

    check(base_worker_query.get("class_found") == 1, "M256-D003-DYN-PAY-47", "Base query must find the class")
    check(base_worker_query.get("protocol_found") == 1, "M256-D003-DYN-PAY-48", "Base query must find the protocol")
    check(base_worker_query.get("conforms") == 0, "M256-D003-DYN-PAY-49", "Base must not conform to Worker")
    check(base_worker_query.get("visited_protocol_count") == 0, "M256-D003-DYN-PAY-50", "Base query must not visit any protocols")
    check(base_worker_query.get("attached_category_count") == 0, "M256-D003-DYN-PAY-51", "Base query must observe zero attached categories")
    check(base_worker_query.get("matched_protocol_owner_identity") is None, "M256-D003-DYN-PAY-52", "Base query must not match a protocol owner")

    check(method_state.get("cache_entry_count") == 3, "M256-D003-DYN-PAY-53", "method cache must retain three entries after the probe")
    check(method_state.get("live_dispatch_count") == 2, "M256-D003-DYN-PAY-54", "probe must perform two live dispatches")
    check(method_state.get("fallback_dispatch_count") == 1, "M256-D003-DYN-PAY-55", "probe must perform one fallback dispatch")
    check(method_state.get("last_category_probe_count") == 1, "M256-D003-DYN-PAY-56", "fallback path must probe one attached category")
    check(method_state.get("last_protocol_probe_count") == 3, "M256-D003-DYN-PAY-57", "fallback path must walk three protocol probe steps")
    check(method_state.get("last_dispatch_resolved_live_method") == 0, "M256-D003-DYN-PAY-58", "ignoredValue must not resolve to a live method")
    check(method_state.get("last_dispatch_fell_back") == 1, "M256-D003-DYN-PAY-59", "ignoredValue must fall back")
    check(method_state.get("last_selector") == "ignoredValue", "M256-D003-DYN-PAY-60", "method cache state must preserve the last selector")

    check(
        widget_entry.get("module_name") == "categoryAttachmentProtocolRuntimeLibrary",
        "M256-D003-DYN-PAY-61",
        "Widget entry must preserve the fixture module name",
    )
    check(
        base_entry.get("module_name") == "categoryAttachmentProtocolRuntimeLibrary",
        "M256-D003-DYN-PAY-62",
        "Base entry must preserve the fixture module name",
    )

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

    check(
        args.native_exe.exists(),
        "M256-D003-DYN-01",
        f"missing native executable: {display_path(args.native_exe)}",
    )
    check(
        args.runtime_library.exists(),
        "M256-D003-DYN-02",
        f"missing runtime library: {display_path(args.runtime_library)}",
    )
    check(
        args.fixture.exists(),
        "M256-D003-DYN-03",
        f"missing fixture: {display_path(args.fixture)}",
    )
    check(
        args.runtime_probe.exists(),
        "M256-D003-DYN-04",
        f"missing runtime probe: {display_path(args.runtime_probe)}",
    )
    clangxx = resolve_tool(args.clangxx)
    check(clangxx is not None, "M256-D003-DYN-05", f"unable to resolve {args.clangxx}")
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
    check(
        compile_result.returncode == 0,
        "M256-D003-DYN-06",
        f"fixture compile failed: {compile_result.stdout}{compile_result.stderr}",
    )

    module_manifest_path = probe_dir / "module.manifest.json"
    runtime_manifest_path = probe_dir / "module.runtime-registration-manifest.json"
    module_ir = probe_dir / "module.ll"
    module_obj = probe_dir / "module.obj"
    backend_path = probe_dir / "module.object-backend.txt"
    check(module_manifest_path.exists(), "M256-D003-DYN-07", f"missing manifest: {display_path(module_manifest_path)}")
    check(runtime_manifest_path.exists(), "M256-D003-DYN-08", f"missing runtime registration manifest: {display_path(runtime_manifest_path)}")
    check(module_ir.exists(), "M256-D003-DYN-09", f"missing emitted IR: {display_path(module_ir)}")
    check(module_obj.exists(), "M256-D003-DYN-10", f"missing emitted object: {display_path(module_obj)}")
    check(backend_path.exists(), "M256-D003-DYN-11", f"missing backend marker: {display_path(backend_path)}")
    if failures:
        return checks_passed, checks_total, {
            "skipped": False,
            "compile_stdout": compile_result.stdout,
            "compile_stderr": compile_result.stderr,
        }

    module_manifest = json.loads(read_text(module_manifest_path))
    runtime_manifest = json.loads(read_text(runtime_manifest_path))
    backend_text = read_text(backend_path).strip()
    check(
        backend_text == "llvm-direct",
        "M256-D003-DYN-12",
        f"module.object-backend.txt must be llvm-direct, saw {backend_text!r}",
    )

    ir_text = read_text(module_ir)
    runtime_boundary_line = boundary_line(ir_text, BOUNDARY_PREFIX)
    check(
        bool(runtime_boundary_line),
        "M256-D003-DYN-13",
        "IR must publish the runtime category-attachment/protocol-conformance summary",
    )
    for check_id, token in (
        ("M256-D003-DYN-14", CONTRACT_ID),
        ("M256-D003-DYN-15", CATEGORY_ATTACHMENT_MODEL),
        ("M256-D003-DYN-16", PROTOCOL_CONFORMANCE_QUERY_MODEL),
        ("M256-D003-DYN-17", FAIL_CLOSED_MODEL),
        ("M256-D003-DYN-18", "attached_category_candidate_count="),
        ("M256-D003-DYN-19", "class_protocol_ref_count="),
        ("M256-D003-DYN-20", "category_protocol_ref_count="),
    ):
        check(
            token in runtime_boundary_line,
            check_id,
            f"runtime boundary line missing token: {token}",
        )

    probe_exe = probe_dir / "m256_d003_category_attachment_protocol_runtime_probe.exe"
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
    check(
        probe_compile.returncode == 0,
        "M256-D003-DYN-21",
        f"probe compile failed: {probe_compile.stdout}{probe_compile.stderr}",
    )
    if probe_compile.returncode != 0:
        return checks_passed, checks_total, {
            "skipped": False,
            "probe_compile_stdout": probe_compile.stdout,
            "probe_compile_stderr": probe_compile.stderr,
        }

    probe_run = run_command([str(probe_exe)], ROOT)
    check(
        probe_run.returncode == 0,
        "M256-D003-DYN-22",
        f"probe execution failed: {probe_run.stdout}{probe_run.stderr}",
    )
    if probe_run.returncode != 0:
        return checks_passed, checks_total, {
            "skipped": False,
            "probe_stdout": probe_run.stdout,
            "probe_stderr": probe_run.stderr,
        }

    try:
        probe_payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        check(False, "M256-D003-DYN-23", f"probe output is not valid JSON: {exc}")
        return checks_passed, checks_total, {
            "skipped": False,
            "probe_stdout": probe_run.stdout,
            "probe_stderr": probe_run.stderr,
        }

    payload_passed, payload_total = payload_checks(
        probe_payload,
        module_manifest,
        runtime_manifest,
        failures,
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
        (args.fixture, FIXTURE_SNIPPETS),
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
        print(
            f"[summary] {MODE} failed ({checks_passed}/{checks_total} checks passed)"
        )
        return 1

    print(canonical_json(summary), end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

#!/usr/bin/env python3
"""Fail-closed contract checker for M252-C003 metadata debug projection."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m252-c003-metadata-debug-projection-and-replay-anchors-v1"
CONTRACT_ID = "objc3c-executable-metadata-debug-projection/m252-c003-v1"
TYPED_HANDOFF_CONTRACT_ID = "objc3c-executable-metadata-typed-lowering-handoff/m252-c002-v1"
SOURCE_GRAPH_CONTRACT_ID = "objc3c-executable-metadata-source-graph-completeness/m252-a002-v1"
NAMED_METADATA_NAME = "!objc3.objc_executable_metadata_debug_projection"
MANIFEST_SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_executable_metadata_debug_projection"
TYPED_HANDOFF_SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_executable_metadata_typed_lowering_handoff"
SOURCE_GRAPH_SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_executable_metadata_source_graph"
CLASS_ROW_KEY = "class-protocol-property-ivar-manifest-projection"
CATEGORY_ROW_KEY = "category-protocol-property-manifest-projection"
IR_ROW_KEY = "hello-ir-named-metadata-anchor"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m252_metadata_debug_projection_and_replay_anchors_c003_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m252" / "m252_c003_metadata_debug_projection_and_replay_anchors_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
DEFAULT_SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_IR_EMITTER_H = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
DEFAULT_IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_CLASS_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
DEFAULT_CATEGORY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_category_protocol_property.objc3"
DEFAULT_IR_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m252" / "c003-metadata-debug-projection"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m252/M252-C003/metadata_debug_projection_and_replay_anchors_summary.json")


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
    SnippetCheck("M252-C003-DOC-EXP-01", "# M252 Metadata Debug Projection And Replay Anchors Expectations (C003)"),
    SnippetCheck("M252-C003-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M252-C003-DOC-EXP-03", "`Objc3ExecutableMetadataDebugProjectionSummary`"),
    SnippetCheck("M252-C003-DOC-EXP-04", f"`{MANIFEST_SURFACE_PATH}`"),
    SnippetCheck("M252-C003-DOC-EXP-05", f"`{NAMED_METADATA_NAME}`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M252-C003-DOC-PKT-01", "# M252-C003 Metadata Debug Projection And Replay Anchors Packet"),
    SnippetCheck("M252-C003-DOC-PKT-02", "Packet: `M252-C003`"),
    SnippetCheck("M252-C003-DOC-PKT-03", "- `M252-C002`"),
    SnippetCheck("M252-C003-DOC-PKT-04", f"- `{CLASS_ROW_KEY}`"),
    SnippetCheck("M252-C003-DOC-PKT-05", f"- `{IR_ROW_KEY}`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M252-C003-ARCH-01", "M252 lane-C C003 metadata debug projection anchors explicit contract artifacts"),
    SnippetCheck("M252-C003-ARCH-02", "deterministic manifest/IR inspection matrix"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M252-C003-NDOC-01", "## Metadata debug projection and replay anchors (M252-C003)"),
    SnippetCheck("M252-C003-NDOC-02", f"`{MANIFEST_SURFACE_PATH}`"),
    SnippetCheck("M252-C003-NDOC-03", f"`{NAMED_METADATA_NAME}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M252-C003-SPC-01", "## M252 metadata debug projection and replay anchors (C003)"),
    SnippetCheck("M252-C003-SPC-02", "`Objc3ExecutableMetadataDebugProjectionSummary`"),
    SnippetCheck("M252-C003-SPC-03", f"`{NAMED_METADATA_NAME}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M252-C003-META-01", "## M252 metadata debug projection metadata anchors (C003)"),
    SnippetCheck("M252-C003-META-02", f"`{MANIFEST_SURFACE_PATH}`"),
    SnippetCheck("M252-C003-META-03", f"`{NAMED_METADATA_NAME}`"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M252-C003-PARSE-01", "M252-C003 debug-projection anchor: the lane-C manifest/IR inspection matrix"),
)
SEMA_CONTRACT_SNIPPETS = (
    SnippetCheck("M252-C003-SEMA-01", "M252-C003 debug-projection anchor: the manifest/IR inspection matrix replays"),
)
SEMA_PASSES_SNIPPETS = (
    SnippetCheck("M252-C003-PASS-01", "M252-C003 debug-projection anchor: the lane-C manifest/IR inspection matrix"),
)
AST_SNIPPETS = (
    SnippetCheck("M252-C003-AST-01", "kObjc3ExecutableMetadataDebugProjectionContractId"),
    SnippetCheck("M252-C003-AST-02", "kObjc3ExecutableMetadataDebugProjectionNamedMetadataName"),
    SnippetCheck("M252-C003-AST-03", "kObjc3ExecutableMetadataDebugProjectionClassManifestRowKey"),
)
FRONTEND_TYPES_SNIPPETS = (
    SnippetCheck("M252-C003-TYPES-01", "struct Objc3ExecutableMetadataDebugProjectionMatrixRow {"),
    SnippetCheck("M252-C003-TYPES-02", "struct Objc3ExecutableMetadataDebugProjectionSummary {"),
    SnippetCheck("M252-C003-TYPES-03", "IsReadyObjc3ExecutableMetadataDebugProjectionSummary("),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M252-C003-ART-01", "BuildExecutableMetadataDebugProjectionSummary("),
    SnippetCheck("M252-C003-ART-02", "BuildExecutableMetadataDebugProjectionSummaryJson("),
    SnippetCheck("M252-C003-ART-03", "objc_executable_metadata_debug_projection"),
)
IR_EMITTER_H_SNIPPETS = (
    SnippetCheck("M252-C003-IRH-01", "std::string executable_metadata_debug_projection_contract_id;"),
    SnippetCheck("M252-C003-IRH-02", "bool executable_metadata_debug_projection_matrix_published = false;"),
    SnippetCheck("M252-C003-IRH-03", "std::string executable_metadata_debug_projection_row2_descriptor;"),
)
IR_EMITTER_CPP_SNIPPETS = (
    SnippetCheck("M252-C003-IRC-01", "!objc3.objc_executable_metadata_debug_projection = !{!54}"),
    SnippetCheck("M252-C003-IRC-02", "; executable_metadata_debug_projection = "),
    SnippetCheck("M252-C003-IRC-03", "executable_metadata_debug_projection_row2_descriptor"),
)
FIXTURE_SNIPPETS = (
    SnippetCheck("M252-C003-FIX-01", "module runtimeMetadataClassRecords;"),
    SnippetCheck("M252-C003-FIX-02", "module runtimeMetadataCategoryRecords;"),
    SnippetCheck("M252-C003-FIX-03", "module Demo;"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M252-C003-PKG-01", '"check:objc3c:m252-c003-metadata-debug-projection-and-replay-anchors": "python scripts/check_m252_c003_metadata_debug_projection_and_replay_anchors.py"'),
    SnippetCheck("M252-C003-PKG-02", '"test:tooling:m252-c003-metadata-debug-projection-and-replay-anchors": "python -m pytest tests/tooling/test_check_m252_c003_metadata_debug_projection_and_replay_anchors.py -q"'),
    SnippetCheck("M252-C003-PKG-03", '"check:objc3c:m252-c003-lane-c-readiness": "npm run check:objc3c:m252-c002-lane-c-readiness && npm run build:objc3c-native && npm run check:objc3c:m252-c003-metadata-debug-projection-and-replay-anchors && npm run test:tooling:m252-c003-metadata-debug-projection-and-replay-anchors"'),
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
    parser.add_argument("--parser-cpp", type=Path, default=DEFAULT_PARSER_CPP)
    parser.add_argument("--sema-contract", type=Path, default=DEFAULT_SEMA_CONTRACT)
    parser.add_argument("--sema-passes", type=Path, default=DEFAULT_SEMA_PASSES)
    parser.add_argument("--ast-header", type=Path, default=DEFAULT_AST_HEADER)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--ir-emitter-h", type=Path, default=DEFAULT_IR_EMITTER_H)
    parser.add_argument("--ir-emitter-cpp", type=Path, default=DEFAULT_IR_EMITTER_CPP)
    parser.add_argument("--class-fixture", type=Path, default=DEFAULT_CLASS_FIXTURE)
    parser.add_argument("--category-fixture", type=Path, default=DEFAULT_CATEGORY_FIXTURE)
    parser.add_argument("--ir-fixture", type=Path, default=DEFAULT_IR_FIXTURE)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--runner-exe", type=Path, default=DEFAULT_RUNNER_EXE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--skip-runner-probes", action="store_true")
    return parser.parse_args(argv)


def check_doc_contract(*, path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(Finding(display_path(path), exists_check_id, f"required artifact is missing: {display_path(path)}"))
        return checks_total, findings
    if not path.is_file():
        findings.append(Finding(display_path(path), exists_check_id, f"required path is not a file: {display_path(path)}"))
        return checks_total, findings
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 1


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def run_runner(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, capture_output=True, text=True, check=False)


def verify_common_debug_projection_packet(*, manifest_path: Path, manifest_payload: dict[str, Any], failures: list[Finding]) -> tuple[int, dict[str, Any], dict[str, Any], list[dict[str, Any]]]:
    checks_total = 0
    pipeline = manifest_payload["frontend"]["pipeline"]
    debug = pipeline["semantic_surface"]["objc_executable_metadata_debug_projection"]
    typed = pipeline["semantic_surface"]["objc_executable_metadata_typed_lowering_handoff"]
    rows = debug.get("rows")
    checks_total += require(debug.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M252-C003-DEBUG-CONTRACT", "debug projection contract id mismatch", failures)
    checks_total += require(debug.get("typed_lowering_handoff_contract_id") == TYPED_HANDOFF_CONTRACT_ID, display_path(manifest_path), "M252-C003-DEBUG-TYPED-CONTRACT", "typed handoff contract id mismatch", failures)
    checks_total += require(debug.get("source_graph_contract_id") == SOURCE_GRAPH_CONTRACT_ID, display_path(manifest_path), "M252-C003-DEBUG-SOURCE-CONTRACT", "source graph contract id mismatch", failures)
    checks_total += require(debug.get("named_metadata_name") == NAMED_METADATA_NAME, display_path(manifest_path), "M252-C003-DEBUG-NAMED-METADATA", "named metadata mismatch", failures)
    checks_total += require(debug.get("manifest_surface_path") == MANIFEST_SURFACE_PATH, display_path(manifest_path), "M252-C003-DEBUG-MANIFEST-PATH", "manifest surface path mismatch", failures)
    checks_total += require(debug.get("typed_handoff_surface_path") == TYPED_HANDOFF_SURFACE_PATH, display_path(manifest_path), "M252-C003-DEBUG-TYPED-PATH", "typed handoff surface path mismatch", failures)
    checks_total += require(debug.get("source_graph_surface_path") == SOURCE_GRAPH_SURFACE_PATH, display_path(manifest_path), "M252-C003-DEBUG-SOURCE-PATH", "source graph surface path mismatch", failures)
    checks_total += require(debug.get("ready") is True, display_path(manifest_path), "M252-C003-DEBUG-READY", "debug projection summary must be ready", failures)
    checks_total += require(debug.get("matrix_published") is True, display_path(manifest_path), "M252-C003-DEBUG-MATRIX", "matrix must be published", failures)
    checks_total += require(debug.get("fail_closed") is True, display_path(manifest_path), "M252-C003-DEBUG-FAIL-CLOSED", "summary must be fail-closed", failures)
    checks_total += require(debug.get("manifest_debug_surface_published") is True, display_path(manifest_path), "M252-C003-DEBUG-MANIFEST-PUBLISHED", "manifest debug surface must be published", failures)
    checks_total += require(debug.get("ir_named_metadata_published") is True, display_path(manifest_path), "M252-C003-DEBUG-IR-PUBLISHED", "IR named metadata must be published", failures)
    checks_total += require(debug.get("replay_anchor_deterministic") is True, display_path(manifest_path), "M252-C003-DEBUG-DETERMINISTIC", "replay anchor must be deterministic", failures)
    checks_total += require(debug.get("matrix_row_count") == 3, display_path(manifest_path), "M252-C003-DEBUG-ROW-COUNT", "matrix row count must be 3", failures)
    checks_total += require(isinstance(rows, list) and len(rows) == 3, display_path(manifest_path), "M252-C003-DEBUG-ROWS", "rows must contain three entries", failures)
    checks_total += require(bool(debug.get("replay_key")), display_path(manifest_path), "M252-C003-DEBUG-REPLAY-KEY", "replay key must be non-empty", failures)
    return checks_total, debug, typed, rows if isinstance(rows, list) else []


def run_manifest_case(*, runner_exe: Path, fixture_path: Path, out_dir: Path, expected_row_key: str) -> tuple[int, list[Finding], dict[str, object] | None]:
    findings: list[Finding] = []
    checks_total = 0
    checks_total += require(fixture_path.exists(), display_path(fixture_path), "M252-C003-FIXTURE-EXISTS", "fixture is missing", findings)
    if findings:
        return checks_total, findings, None

    out_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(runner_exe),
        str(fixture_path),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ]
    completed = run_runner(command)
    summary_path = out_dir / "module.c_api_summary.json"
    manifest_path = out_dir / "module.manifest.json"

    checks_total += require(summary_path.exists(), display_path(summary_path), "M252-C003-MANIFEST-SUMMARY", "runner summary missing", findings)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M252-C003-MANIFEST-FILE", "runner manifest missing", findings)
    if findings:
        return checks_total, findings, None

    summary_payload = load_json(summary_path)
    manifest_payload = load_json(manifest_path)
    checks_total += require(completed.returncode == 0, display_path(summary_path), "M252-C003-MANIFEST-EXIT", "runner process exit mismatch", findings)
    checks_total += require(summary_payload.get("success") is True, display_path(summary_path), "M252-C003-MANIFEST-SUCCESS", "runner success mismatch", findings)
    checks_total += require(summary_payload.get("status") == 0, display_path(summary_path), "M252-C003-MANIFEST-STATUS", "runner status mismatch", findings)

    added_checks, debug, typed, rows = verify_common_debug_projection_packet(
        manifest_path=manifest_path, manifest_payload=manifest_payload, failures=findings
    )
    checks_total += added_checks
    row = rows[0] if expected_row_key == CLASS_ROW_KEY and rows else rows[1] if rows else {}
    checks_total += require(debug.get("active_typed_handoff_ready") is True, display_path(manifest_path), "M252-C003-MANIFEST-ACTIVE-TYPED", "metadata fixture must surface an active typed handoff", findings)
    checks_total += require(debug.get("active_typed_handoff_replay_key") == typed.get("replay_key"), display_path(manifest_path), "M252-C003-MANIFEST-TYPED-KEY", "active typed handoff replay key must match the C002 typed packet", findings)
    checks_total += require(row.get("row_key") == expected_row_key, display_path(manifest_path), "M252-C003-MANIFEST-ROW-KEY", "unexpected row key ordering", findings)
    checks_total += require(row.get("artifact_kind") == "manifest", display_path(manifest_path), "M252-C003-MANIFEST-ROW-KIND", "manifest row must advertise manifest artifact kind", findings)
    checks_total += require(row.get("artifact_relative_path") == "module.manifest.json", display_path(manifest_path), "M252-C003-MANIFEST-ROW-PATH", "manifest row artifact path mismatch", findings)
    checks_total += require(row.get("expected_anchor") in {MANIFEST_SURFACE_PATH, TYPED_HANDOFF_SURFACE_PATH}, display_path(manifest_path), "M252-C003-MANIFEST-ROW-ANCHOR", "manifest row expected anchor mismatch", findings)

    case_payload = {
        "fixture": display_path(fixture_path),
        "manifest_path": display_path(manifest_path),
        "debug_replay_key": debug.get("replay_key"),
        "active_typed_handoff_replay_key": debug.get("active_typed_handoff_replay_key"),
        "row_key": row.get("row_key"),
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    return checks_total, findings, case_payload


def run_ir_case(*, runner_exe: Path, fixture_path: Path, out_dir: Path) -> tuple[int, list[Finding], dict[str, object] | None]:
    findings: list[Finding] = []
    checks_total = 0
    checks_total += require(fixture_path.exists(), display_path(fixture_path), "M252-C003-IR-FIXTURE-EXISTS", "IR fixture is missing", findings)
    if findings:
        return checks_total, findings, None

    out_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(runner_exe),
        str(fixture_path),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-object",
    ]
    completed = run_runner(command)
    summary_path = out_dir / "module.c_api_summary.json"
    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"

    checks_total += require(summary_path.exists(), display_path(summary_path), "M252-C003-IR-SUMMARY", "runner summary missing", findings)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M252-C003-IR-MANIFEST", "runner manifest missing", findings)
    checks_total += require(ir_path.exists(), display_path(ir_path), "M252-C003-IR-FILE", "LLVM IR output missing", findings)
    if findings:
        return checks_total, findings, None

    summary_payload = load_json(summary_path)
    manifest_payload = load_json(manifest_path)
    ir_text = ir_path.read_text(encoding="utf-8")
    checks_total += require(completed.returncode == 0, display_path(summary_path), "M252-C003-IR-EXIT", "runner process exit mismatch", findings)
    checks_total += require(summary_payload.get("success") is True, display_path(summary_path), "M252-C003-IR-SUCCESS", "runner success mismatch", findings)
    checks_total += require(summary_payload.get("status") == 0, display_path(summary_path), "M252-C003-IR-STATUS", "runner status mismatch", findings)

    added_checks, debug, typed, rows = verify_common_debug_projection_packet(
        manifest_path=manifest_path, manifest_payload=manifest_payload, failures=findings
    )
    checks_total += added_checks
    ir_row = rows[2] if len(rows) >= 3 else {}
    if debug.get("active_typed_handoff_ready") is True:
        checks_total += require(debug.get("active_typed_handoff_replay_key") == typed.get("replay_key"), display_path(manifest_path), "M252-C003-IR-ACTIVE-TYPED", "active typed handoff replay key must match the typed packet when the hello fixture materializes one", findings)
    else:
        checks_total += require(debug.get("active_typed_handoff_replay_key") in {"", None}, display_path(manifest_path), "M252-C003-IR-ACTIVE-TYPED", "hello fixture must not publish a stray active typed handoff replay key", findings)
    checks_total += require(ir_row.get("row_key") == IR_ROW_KEY, display_path(manifest_path), "M252-C003-IR-ROW-KEY", "IR row key mismatch", findings)
    checks_total += require(ir_row.get("artifact_kind") == "llvm-ir", display_path(manifest_path), "M252-C003-IR-ROW-KIND", "IR row must advertise llvm-ir artifact kind", findings)
    checks_total += require(ir_row.get("artifact_relative_path") == "module.ll", display_path(manifest_path), "M252-C003-IR-ROW-PATH", "IR row artifact path mismatch", findings)
    checks_total += require(NAMED_METADATA_NAME in ir_text, display_path(ir_path), "M252-C003-IR-ANCHOR", "named metadata anchor missing from LLVM IR", findings)
    checks_total += require(CONTRACT_ID in ir_text, display_path(ir_path), "M252-C003-IR-CONTRACT", "debug projection contract id missing from LLVM IR", findings)
    checks_total += require(CLASS_ROW_KEY in ir_text, display_path(ir_path), "M252-C003-IR-ROW0", "class manifest row descriptor missing from LLVM IR", findings)
    checks_total += require(CATEGORY_ROW_KEY in ir_text, display_path(ir_path), "M252-C003-IR-ROW1", "category manifest row descriptor missing from LLVM IR", findings)
    checks_total += require(IR_ROW_KEY in ir_text, display_path(ir_path), "M252-C003-IR-ROW2", "hello IR row descriptor missing from LLVM IR", findings)
    replay_prefix = str(debug.get("replay_key", "")).split(";row[0]=", 1)[0]
    checks_total += require(bool(replay_prefix) and replay_prefix in ir_text, display_path(ir_path), "M252-C003-IR-REPLAY-KEY", "debug projection replay key prefix missing from LLVM IR", findings)

    case_payload = {
        "fixture": display_path(fixture_path),
        "ir_path": display_path(ir_path),
        "debug_replay_key": debug.get("replay_key"),
        "row_key": ir_row.get("row_key"),
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    return checks_total, findings, case_payload


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M252-C003-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M252-C003-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M252-C003-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M252-C003-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M252-C003-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M252-C003-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.parser_cpp, "M252-C003-PARSE-EXISTS", PARSER_SNIPPETS),
        (args.sema_contract, "M252-C003-SEMA-EXISTS", SEMA_CONTRACT_SNIPPETS),
        (args.sema_passes, "M252-C003-PASS-EXISTS", SEMA_PASSES_SNIPPETS),
        (args.ast_header, "M252-C003-AST-EXISTS", AST_SNIPPETS),
        (args.frontend_types, "M252-C003-TYPES-EXISTS", FRONTEND_TYPES_SNIPPETS),
        (args.frontend_artifacts, "M252-C003-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.ir_emitter_h, "M252-C003-IRH-EXISTS", IR_EMITTER_H_SNIPPETS),
        (args.ir_emitter_cpp, "M252-C003-IRC-EXISTS", IR_EMITTER_CPP_SNIPPETS),
        (args.class_fixture, "M252-C003-FIX-CLASS-EXISTS", (FIXTURE_SNIPPETS[0],)),
        (args.category_fixture, "M252-C003-FIX-CATEGORY-EXISTS", (FIXTURE_SNIPPETS[1],)),
        (args.ir_fixture, "M252-C003-FIX-IR-EXISTS", (FIXTURE_SNIPPETS[2],)),
        (args.package_json, "M252-C003-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        added_checks, added_failures = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += added_checks
        failures.extend(added_failures)

    runner_cases: dict[str, object] = {}
    runner_probes_executed = False
    if not args.skip_runner_probes:
        checks_total += require(args.runner_exe.exists(), display_path(args.runner_exe), "M252-C003-RUNNER-EXE", "native frontend C API runner binary is missing", failures)
        if args.runner_exe.exists():
            runner_probes_executed = True
            args.probe_root.mkdir(parents=True, exist_ok=True)
            class_checks, class_failures, class_payload = run_manifest_case(
                runner_exe=args.runner_exe,
                fixture_path=args.class_fixture,
                out_dir=args.probe_root / "class_protocol_property_ivar",
                expected_row_key=CLASS_ROW_KEY,
            )
            checks_total += class_checks
            failures.extend(class_failures)
            if class_payload is not None:
                runner_cases["class_protocol_property_ivar"] = class_payload

            category_checks, category_failures, category_payload = run_manifest_case(
                runner_exe=args.runner_exe,
                fixture_path=args.category_fixture,
                out_dir=args.probe_root / "category_protocol_property",
                expected_row_key=CATEGORY_ROW_KEY,
            )
            checks_total += category_checks
            failures.extend(category_failures)
            if category_payload is not None:
                runner_cases["category_protocol_property"] = category_payload

            ir_checks, ir_failures, ir_payload = run_ir_case(
                runner_exe=args.runner_exe,
                fixture_path=args.ir_fixture,
                out_dir=args.probe_root / "hello_ir_anchor",
            )
            checks_total += ir_checks
            failures.extend(ir_failures)
            if ir_payload is not None:
                runner_cases["hello_ir_anchor"] = ir_payload

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "failures": [finding.__dict__ for finding in failures],
        "runner_probes_executed": runner_probes_executed,
        "runner_cases": runner_cases,
    }
    summary_path.write_text(canonical_json(payload), encoding="utf-8")

    if failures:
        print(f"[m252-c003] FAIL {checks_total - len(failures)}/{checks_total} -> {display_path(summary_path)}", file=sys.stderr)
        for finding in failures:
            print(f" - {finding.artifact} :: {finding.check_id} :: {finding.detail}", file=sys.stderr)
        return 1

    print(f"[m252-c003] PASS {checks_total}/{checks_total} -> {display_path(summary_path)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

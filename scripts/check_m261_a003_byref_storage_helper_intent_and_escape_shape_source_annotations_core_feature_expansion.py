#!/usr/bin/env python3
"""Checker for M261-A003 block source storage annotations."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m261-a003-block-source-storage-annotations-v1"
CONTRACT_ID = "objc3c-executable-block-source-storage-annotation/m261-a003-v1"
BYREF_STORAGE_MODEL = (
    "block-source-model-publishes-deterministic-byref-capture-candidates-before-runnable-block-byref-lowering"
)
HELPER_INTENT_MODEL = (
    "block-source-model-publishes-copy-dispose-helper-intent-before-runnable-block-helper-lowering"
)
ESCAPE_SHAPE_MODEL = (
    "block-source-model-publishes-heap-promotion-relevant-escape-shape-categories-before-runnable-block-escape-analysis"
)
EVIDENCE_MODEL = (
    "hello-ir-boundary-plus-source-only-positive-probe-plus-native-fail-closed-negative-probe"
)
FAIL_CLOSED_MODEL = (
    "fail-closed-on-block-byref-helper-or-escape-shape-source-annotation-drift-before-runnable-block-lowering"
)
NEXT_ISSUE = "M261-B001"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m261" / "M261-A003" / "block_source_storage_annotations_summary.json"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m261_byref_storage_helper_intent_and_escape_shape_source_annotations_core_feature_expansion_a003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m261" / "m261_a003_byref_storage_helper_intent_and_escape_shape_source_annotations_core_feature_expansion_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
PART0_SPEC = ROOT / "spec" / "PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
SEMA_PASS_MANAGER_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m261_a003_lane_a_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m261_a003_byref_storage_helper_intent_and_escape_shape_source_annotations_core_feature_expansion.py"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
HELLO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_block_source_storage_annotations_positive.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m261" / "a003-block-source-storage-annotations"
PARSER_FAILURE_CODES = {"O3P166", "O3P110", "O3P104", "O3P100"}
EXPECTED_HELLO_REPLAY_KEY = (
    "block_literal_sites=0;capture_entries_total=0;mutated_capture_entries_total=0;"
    "byref_capture_entries_total=0;copy_helper_intent_sites=0;dispose_helper_intent_sites=0;"
    "heap_candidate_sites=0;expression_sites=0;global_initializer_sites=0;"
    "binding_initializer_sites=0;assignment_value_sites=0;return_value_sites=0;"
    "call_argument_sites=0;message_argument_sites=0;deterministic=true;"
    "lane_contract=m261-block-source-storage-annotations-v1"
)
EXPECTED_POSITIVE_REPLAY_KEY = (
    "block_literal_sites=2;capture_entries_total=2;mutated_capture_entries_total=1;"
    "byref_capture_entries_total=1;copy_helper_intent_sites=1;dispose_helper_intent_sites=1;"
    "heap_candidate_sites=1;expression_sites=1;global_initializer_sites=0;"
    "binding_initializer_sites=1;assignment_value_sites=0;return_value_sites=0;"
    "call_argument_sites=0;message_argument_sites=0;deterministic=true;"
    "lane_contract=m261-block-source-storage-annotations-v1"
)


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M261-A003-EXP-01", "# M261 Byref Storage Helper Intent And Escape Shape Source Annotations Core Feature Expansion Expectations (A003)"),
        SnippetCheck("M261-A003-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M261-A003-EXP-03", "`artifacts/bin/objc3c-frontend-c-api-runner.exe --no-emit-ir --no-emit-object`"),
        SnippetCheck("M261-A003-EXP-04", "`O3S221`"),
        SnippetCheck("M261-A003-EXP-05", "`M261-B001`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M261-A003-PKT-01", "# M261-A003 Byref Storage Helper Intent And Escape Shape Source Annotations Core Feature Expansion Packet"),
        SnippetCheck("M261-A003-PKT-02", "Issue: `#7181`"),
        SnippetCheck("M261-A003-PKT-03", "Packet: `M261-A003`"),
        SnippetCheck("M261-A003-PKT-04", "`M261-B001` is the explicit next issue after this implementation lands."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M261-A003-SRC-01", "## M261 block source storage annotations (M261-A003)"),
        SnippetCheck("M261-A003-SRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-A003-SRC-03", "`objc_block_source_storage_annotation_surface`"),
        SnippetCheck("M261-A003-SRC-04", "`; executable_block_source_storage_annotations = ...`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M261-A003-NDOC-01", "## M261 block source storage annotations (M261-A003)"),
        SnippetCheck("M261-A003-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-A003-NDOC-03", "`objc_block_source_storage_annotation_surface`"),
        SnippetCheck("M261-A003-NDOC-04", "`M261-B001` is the next issue."),
    ),
    PART0_SPEC: (
        SnippetCheck("M261-A003-P0-01", "byref/helper/escape-shape"),
        SnippetCheck("M261-A003-P0-02", "(`M261-A003`)"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M261-A003-SPC-01", "## M261 block source storage annotations (A003)"),
        SnippetCheck("M261-A003-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-A003-SPC-03", "`objc_block_source_storage_annotation_surface`"),
        SnippetCheck("M261-A003-SPC-04", "`; executable_block_source_storage_annotations = ...`"),
        SnippetCheck("M261-A003-SPC-05", "`M261-B001` is the next issue."),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M261-A003-ARCH-01", "## M261 Block Source Storage Annotations (A003)"),
        SnippetCheck("M261-A003-ARCH-02", "`; executable_block_source_storage_annotations = ...`"),
        SnippetCheck("M261-A003-ARCH-03", "the next issue is `M261-B001`"),
    ),
    PARSER_CPP: (
        SnippetCheck("M261-A003-PARSER-01", "enum class BlockLiteralSourceUseKind"),
        SnippetCheck("M261-A003-PARSER-02", "MessageArgument"),
        SnippetCheck("M261-A003-PARSER-03", "block_mutated_capture_names_lexicographic"),
        SnippetCheck("M261-A003-PARSER-04", "block_escape_shape_symbol"),
    ),
    AST_HEADER: (
        SnippetCheck("M261-A003-AST-01", "kObjc3ExecutableBlockSourceStorageAnnotationContractId"),
        SnippetCheck("M261-A003-AST-02", "kObjc3ExecutableBlockByrefStorageModel"),
        SnippetCheck("M261-A003-AST-03", "block_byref_capture_names_lexicographic"),
        SnippetCheck("M261-A003-AST-04", "block_source_storage_annotations_are_normalized"),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M261-A003-SEMA-PM-01", "M261-A003 block-source-storage-annotation anchor"),
        SnippetCheck("M261-A003-SEMA-PM-02", "allow_source_only_block_literals"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M261-A003-LOWER-H-01", "struct Objc3BlockSourceStorageAnnotationContract"),
        SnippetCheck("M261-A003-LOWER-H-02", "std::string Objc3ExecutableBlockSourceStorageAnnotationSummary();"),
        SnippetCheck("M261-A003-LOWER-H-03", "std::string Objc3BlockSourceStorageAnnotationReplayKey("),
    ),
    LOWERING_CPP: (
        SnippetCheck("M261-A003-LOWER-CPP-01", "M261-A003 block-source-storage-annotation anchor"),
        SnippetCheck("M261-A003-LOWER-CPP-02", "std::string Objc3ExecutableBlockSourceStorageAnnotationSummary()"),
        SnippetCheck("M261-A003-LOWER-CPP-03", "std::string Objc3BlockSourceStorageAnnotationReplayKey("),
    ),
    IR_CPP: (
        SnippetCheck("M261-A003-IR-01", "M261-A003 block-source-storage-annotation anchor"),
        SnippetCheck("M261-A003-IR-02", "; executable_block_source_storage_annotations = "),
    ),
    ARTIFACTS_CPP: (
        SnippetCheck("M261-A003-ART-01", "BuildBlockSourceStorageAnnotationContract("),
        SnippetCheck("M261-A003-ART-02", "objc_block_source_storage_annotation_surface"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M261-A003-PKG-01", '"check:objc3c:m261-a003-block-source-storage-annotations": "python scripts/check_m261_a003_byref_storage_helper_intent_and_escape_shape_source_annotations_core_feature_expansion.py"'),
        SnippetCheck("M261-A003-PKG-02", '"test:tooling:m261-a003-block-source-storage-annotations": "python -m pytest tests/tooling/test_check_m261_a003_byref_storage_helper_intent_and_escape_shape_source_annotations_core_feature_expansion.py -q"'),
        SnippetCheck("M261-A003-PKG-03", '"check:objc3c:m261-a003-lane-a-readiness": "python scripts/run_m261_a003_lane_a_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M261-A003-RUN-01", "scripts/build_objc3c_native_docs.py"),
        SnippetCheck("M261-A003-RUN-02", "build:objc3c-native"),
        SnippetCheck("M261-A003-RUN-03", "check_m261_a003_byref_storage_helper_intent_and_escape_shape_source_annotations_core_feature_expansion.py"),
        SnippetCheck("M261-A003-RUN-04", "test_check_m261_a003_byref_storage_helper_intent_and_escape_shape_source_annotations_core_feature_expansion.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M261-A003-TEST-01", "def test_m261_a003_checker_emits_summary() -> None:"),
        SnippetCheck("M261-A003-TEST-02", CONTRACT_ID),
    ),
}


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def run_process(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    findings: list[Finding] = []
    checks_total = len(snippets) + 1
    if not path.exists():
        findings.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return checks_total, findings
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 1


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def block_source_storage_surface(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    frontend = manifest.get("frontend", {})
    pipeline = frontend.get("pipeline", {})
    semantic_surface = pipeline.get("semantic_surface", {})
    surface = semantic_surface.get("objc_block_source_storage_annotation_surface")
    if not isinstance(surface, dict):
        raise TypeError(
            f"missing objc_block_source_storage_annotation_surface in {display_path(manifest_path)}"
        )
    return surface


def diagnostics_codes(path: Path) -> list[str]:
    payload = load_json(path)
    diagnostics = payload.get("diagnostics", [])
    if not isinstance(diagnostics, list):
        return []
    return [str(diag.get("code", "")) for diag in diagnostics if isinstance(diag, dict)]


def run_dynamic_probes(failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    payload: dict[str, Any] = {}
    hello_out_dir = PROBE_ROOT / "hello-native"
    source_out_dir = PROBE_ROOT / "source-only-runner"
    native_out_dir = PROBE_ROOT / "native-fail-closed"
    hello_out_dir.mkdir(parents=True, exist_ok=True)
    source_out_dir.mkdir(parents=True, exist_ok=True)
    native_out_dir.mkdir(parents=True, exist_ok=True)

    source_summary = source_out_dir / "runner-summary.json"

    checks_total += require(RUNNER_EXE.exists(), display_path(RUNNER_EXE), "M261-A003-DYN-01", "frontend runner binary is missing", failures)
    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M261-A003-DYN-02", "native binary is missing", failures)
    checks_total += require(HELLO_FIXTURE.exists(), display_path(HELLO_FIXTURE), "M261-A003-DYN-03", "hello fixture is missing", failures)
    checks_total += require(FIXTURE.exists(), display_path(FIXTURE), "M261-A003-DYN-04", "positive fixture is missing", failures)
    if failures:
        return checks_total, payload

    hello_completed = run_process([
        str(NATIVE_EXE),
        str(HELLO_FIXTURE),
        "--out-dir",
        str(hello_out_dir),
        "--emit-prefix",
        "module",
    ])
    hello_manifest = hello_out_dir / "module.manifest.json"
    hello_diag = hello_out_dir / "module.diagnostics.json"
    hello_ir = hello_out_dir / "module.ll"
    hello_codes: list[str] = []
    hello_surface: dict[str, Any] = {}
    hello_ir_text = ""
    checks_total += require(hello_completed.returncode == 0, display_path(hello_out_dir), "M261-A003-DYN-05", f"hello native probe failed: {hello_completed.stdout}{hello_completed.stderr}", failures)
    checks_total += require(hello_manifest.exists(), display_path(hello_manifest), "M261-A003-DYN-06", "hello manifest is missing", failures)
    checks_total += require(hello_diag.exists(), display_path(hello_diag), "M261-A003-DYN-07", "hello diagnostics are missing", failures)
    checks_total += require(hello_ir.exists(), display_path(hello_ir), "M261-A003-DYN-08", "hello IR is missing", failures)
    if hello_manifest.exists() and hello_diag.exists() and hello_ir.exists():
        hello_codes = diagnostics_codes(hello_diag)
        hello_surface = block_source_storage_surface(hello_manifest)
        hello_ir_text = hello_ir.read_text(encoding="utf-8")
        checks_total += require(hello_codes == [], display_path(hello_diag), "M261-A003-DYN-09", f"hello diagnostics must stay empty, observed {hello_codes}", failures)
        checks_total += require(hello_surface.get("block_literal_sites") == 0, display_path(hello_manifest), "M261-A003-DYN-10", "hello block literal count must stay zero", failures)
        checks_total += require(hello_surface.get("capture_entries_total") == 0, display_path(hello_manifest), "M261-A003-DYN-11", "hello capture entry count must stay zero", failures)
        checks_total += require(hello_surface.get("mutated_capture_entries_total") == 0, display_path(hello_manifest), "M261-A003-DYN-12", "hello mutated capture count must stay zero", failures)
        checks_total += require(hello_surface.get("byref_capture_entries_total") == 0, display_path(hello_manifest), "M261-A003-DYN-13", "hello byref count must stay zero", failures)
        checks_total += require(hello_surface.get("heap_candidate_sites") == 0, display_path(hello_manifest), "M261-A003-DYN-14", "hello heap-candidate count must stay zero", failures)
        checks_total += require(hello_surface.get("deterministic_handoff") is True, display_path(hello_manifest), "M261-A003-DYN-15", "hello deterministic handoff must stay true", failures)
        checks_total += require(hello_surface.get("replay_key") == EXPECTED_HELLO_REPLAY_KEY, display_path(hello_manifest), "M261-A003-DYN-16", "hello replay key drifted", failures)
        checks_total += require("; executable_block_source_storage_annotations = " in hello_ir_text, display_path(hello_ir), "M261-A003-DYN-17", "hello IR is missing the source-storage annotation summary line", failures)
        checks_total += require(CONTRACT_ID in hello_ir_text, display_path(hello_ir), "M261-A003-DYN-18", "hello IR is missing the A003 contract id", failures)
        checks_total += require(EXPECTED_HELLO_REPLAY_KEY in hello_ir_text, display_path(hello_ir), "M261-A003-DYN-19", "hello IR is missing the A003 replay key", failures)

    source_completed = run_process([
        str(RUNNER_EXE),
        str(FIXTURE),
        "--out-dir",
        str(source_out_dir),
        "--emit-prefix",
        "module",
        "--summary-out",
        str(source_summary),
        "--no-emit-ir",
        "--no-emit-object",
    ])
    source_manifest = source_out_dir / "module.manifest.json"
    source_diag = source_out_dir / "module.diagnostics.json"
    source_ir = source_out_dir / "module.ll"
    source_obj = source_out_dir / "module.obj"
    source_codes: list[str] = []
    source_surface: dict[str, Any] = {}
    checks_total += require(source_completed.returncode == 0, display_path(source_out_dir), "M261-A003-DYN-20", f"source-only runner probe failed: {source_completed.stdout}{source_completed.stderr}", failures)
    checks_total += require(source_summary.exists(), display_path(source_summary), "M261-A003-DYN-21", "source-only runner summary is missing", failures)
    checks_total += require(source_manifest.exists(), display_path(source_manifest), "M261-A003-DYN-22", "source-only manifest is missing", failures)
    checks_total += require(source_diag.exists(), display_path(source_diag), "M261-A003-DYN-23", "source-only diagnostics are missing", failures)
    checks_total += require(not source_ir.exists(), display_path(source_ir), "M261-A003-DYN-24", "source-only probe must not emit IR", failures)
    checks_total += require(not source_obj.exists(), display_path(source_obj), "M261-A003-DYN-25", "source-only probe must not emit an object", failures)
    if source_summary.exists() and source_manifest.exists() and source_diag.exists():
        source_summary_payload = load_json(source_summary)
        source_codes = diagnostics_codes(source_diag)
        source_surface = block_source_storage_surface(source_manifest)
        checks_total += require(source_summary_payload.get("success") is True, display_path(source_summary), "M261-A003-DYN-26", "source-only summary must report success", failures)
        checks_total += require(source_summary_payload.get("semantic_skipped") is False, display_path(source_summary), "M261-A003-DYN-27", "source-only summary must not skip sema", failures)
        checks_total += require(source_codes == [], display_path(source_diag), "M261-A003-DYN-28", f"source-only diagnostics must stay empty, observed {source_codes}", failures)
        checks_total += require(source_surface.get("block_literal_sites") == 2, display_path(source_manifest), "M261-A003-DYN-29", "unexpected block literal site count", failures)
        checks_total += require(source_surface.get("capture_entries_total") == 2, display_path(source_manifest), "M261-A003-DYN-30", "unexpected capture entry count", failures)
        checks_total += require(source_surface.get("mutated_capture_entries_total") == 1, display_path(source_manifest), "M261-A003-DYN-31", "unexpected mutated capture count", failures)
        checks_total += require(source_surface.get("byref_capture_entries_total") == 1, display_path(source_manifest), "M261-A003-DYN-32", "unexpected byref capture count", failures)
        checks_total += require(source_surface.get("copy_helper_intent_sites") == 1, display_path(source_manifest), "M261-A003-DYN-33", "unexpected copy-helper site count", failures)
        checks_total += require(source_surface.get("dispose_helper_intent_sites") == 1, display_path(source_manifest), "M261-A003-DYN-34", "unexpected dispose-helper site count", failures)
        checks_total += require(source_surface.get("heap_candidate_sites") == 1, display_path(source_manifest), "M261-A003-DYN-35", "unexpected heap-candidate site count", failures)
        checks_total += require(source_surface.get("expression_sites") == 1, display_path(source_manifest), "M261-A003-DYN-36", "unexpected expression-site count", failures)
        checks_total += require(source_surface.get("global_initializer_sites") == 0, display_path(source_manifest), "M261-A003-DYN-37", "unexpected global-initializer site count", failures)
        checks_total += require(source_surface.get("binding_initializer_sites") == 1, display_path(source_manifest), "M261-A003-DYN-38", "unexpected binding-initializer site count", failures)
        checks_total += require(source_surface.get("assignment_value_sites") == 0, display_path(source_manifest), "M261-A003-DYN-39", "unexpected assignment-value site count", failures)
        checks_total += require(source_surface.get("return_value_sites") == 0, display_path(source_manifest), "M261-A003-DYN-40", "unexpected return-value site count", failures)
        checks_total += require(source_surface.get("call_argument_sites") == 0, display_path(source_manifest), "M261-A003-DYN-41", "unexpected call-argument site count", failures)
        checks_total += require(source_surface.get("message_argument_sites") == 0, display_path(source_manifest), "M261-A003-DYN-42", "unexpected message-argument site count", failures)
        checks_total += require(source_surface.get("non_normalized_sites") == 0, display_path(source_manifest), "M261-A003-DYN-43", "non-normalized sites must stay zero", failures)
        checks_total += require(source_surface.get("contract_violation_sites") == 0, display_path(source_manifest), "M261-A003-DYN-44", "contract violation sites must stay zero", failures)
        checks_total += require(source_surface.get("deterministic_handoff") is True, display_path(source_manifest), "M261-A003-DYN-45", "deterministic handoff must stay true", failures)
        checks_total += require(source_surface.get("replay_key") == EXPECTED_POSITIVE_REPLAY_KEY, display_path(source_manifest), "M261-A003-DYN-46", "source-storage replay key drifted", failures)

    native_completed = run_process([
        str(NATIVE_EXE),
        str(FIXTURE),
        "--out-dir",
        str(native_out_dir),
        "--emit-prefix",
        "module",
    ])
    native_diag = native_out_dir / "module.diagnostics.json"
    native_codes: list[str] = []
    checks_total += require(native_completed.returncode != 0, display_path(native_out_dir), "M261-A003-DYN-47", "native emit path must still fail closed", failures)
    checks_total += require(native_diag.exists(), display_path(native_diag), "M261-A003-DYN-48", "native fail-closed diagnostics are missing", failures)
    if native_diag.exists():
        native_codes = diagnostics_codes(native_diag)
        parser_failures = sorted(code for code in native_codes if code in PARSER_FAILURE_CODES)
        checks_total += require("O3S221" in native_codes, display_path(native_diag), "M261-A003-DYN-49", f"expected O3S221 in native diagnostics, observed {native_codes}", failures)
        checks_total += require(not parser_failures, display_path(native_diag), "M261-A003-DYN-50", f"unexpected parser regressions in native fail-closed probe: {parser_failures}", failures)

    payload = {
        "hello_native": {
            "returncode": hello_completed.returncode,
            "diagnostic_codes": hello_codes,
            "surface": hello_surface,
        },
        "source_only_runner": {
            "returncode": source_completed.returncode,
            "diagnostic_codes": source_codes,
            "surface": source_surface,
        },
        "native_fail_closed": {
            "returncode": native_completed.returncode,
            "diagnostic_codes": native_codes,
        },
    }
    return checks_total, payload


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    checks_total = 0

    for path, snippets in STATIC_SNIPPETS.items():
        current_total, current_findings = check_static_contract(path, snippets)
        checks_total += current_total
        findings.extend(current_findings)

    dynamic_payload: dict[str, Any]
    if args.skip_dynamic_probes:
        dynamic_checks = 0
        dynamic_payload = {"skipped": True}
    else:
        dynamic_checks, dynamic_payload = run_dynamic_probes(findings)
    checks_total += dynamic_checks

    summary = {
        "ok": not findings,
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "models": {
            "byref_storage": BYREF_STORAGE_MODEL,
            "helper_intent": HELPER_INTENT_MODEL,
            "escape_shape": ESCAPE_SHAPE_MODEL,
            "evidence": EVIDENCE_MODEL,
            "fail_closed": FAIL_CLOSED_MODEL,
        },
        "next_issue": NEXT_ISSUE,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(findings),
        "findings": [finding.__dict__ for finding in findings],
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_case": dynamic_payload,
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if findings:
        json.dump(summary, sys.stdout, indent=2)
        sys.stdout.write("\n")
        return 1

    json.dump(summary, sys.stdout, indent=2)
    sys.stdout.write("\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

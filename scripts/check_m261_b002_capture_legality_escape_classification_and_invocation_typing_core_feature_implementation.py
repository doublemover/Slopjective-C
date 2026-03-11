#!/usr/bin/env python3
"""Checker for M261-B002 capture legality, escape classification, and invocation typing."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m261-b002-capture-legality-escape-invocation-v1"
CONTRACT_ID = "objc3c-executable-block-capture-legality-escape-and-invocation/m261-b002-v1"
CAPTURE_LEGALITY_MODEL = (
    "source-only-sema-enforces-live-capture-resolution-and-mutability-classification-before-runnable-block-object-lowering"
)
ESCAPE_CLASSIFICATION_MODEL = (
    "source-only-sema-classifies-byref-escape-and-copy-dispose-requirements-from-parser-owned-annotations-before-runnable-helper-lowering"
)
INVOCATION_TYPING_MODEL = (
    "source-only-sema-types-local-block-invocations-as-callable-values-while-native-block-execution-remains-fail-closed"
)
NEXT_ISSUE = "M261-B003"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m261" / "M261-B002" / "capture_legality_escape_classification_and_invocation_typing_summary.json"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m261_capture_legality_escape_classification_and_invocation_typing_core_feature_implementation_b002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m261" / "m261_b002_capture_legality_escape_classification_and_invocation_typing_core_feature_implementation_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
PART0_SPEC = ROOT / "spec" / "PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
SEMA_PASS_MANAGER_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m261_b002_lane_b_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m261_b002_capture_legality_escape_classification_and_invocation_typing_core_feature_implementation.py"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_capture_legality_escape_invocation_positive.objc3"
MISSING_CAPTURE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_capture_legality_escape_invocation_missing_capture.objc3"
BAD_CALL_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_capture_legality_escape_invocation_bad_call.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m261" / "b002-capture-legality-escape-invocation"


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
        SnippetCheck("M261-B002-EXP-01", "# M261 Capture Legality Escape Classification And Invocation Typing Core Feature Implementation Expectations (B002)"),
        SnippetCheck("M261-B002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M261-B002-EXP-03", "`O3S202`"),
        SnippetCheck("M261-B002-EXP-04", "`O3S206`"),
        SnippetCheck("M261-B002-EXP-05", "`O3S221`"),
        SnippetCheck("M261-B002-EXP-06", "`M261-B003`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M261-B002-PKT-01", "# M261-B002 Capture Legality Escape Classification And Invocation Typing Core Feature Implementation Packet"),
        SnippetCheck("M261-B002-PKT-02", "Issue: `#7183`"),
        SnippetCheck("M261-B002-PKT-03", "Packet: `M261-B002`"),
        SnippetCheck("M261-B002-PKT-04", "`M261-B003` is the explicit next issue after this implementation lands."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M261-B002-SRC-01", "## M261 capture legality, escape classification, and invocation typing (M261-B002)"),
        SnippetCheck("M261-B002-SRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-B002-SRC-03", "`O3S202`"),
        SnippetCheck("M261-B002-SRC-04", "`O3S206`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M261-B002-NDOC-01", "## M261 capture legality, escape classification, and invocation typing (M261-B002)"),
        SnippetCheck("M261-B002-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-B002-NDOC-03", "`M261-B003` is the next issue."),
    ),
    PART0_SPEC: (
        SnippetCheck("M261-B002-P0-01", "(`M261-B002`)"),
        SnippetCheck("M261-B002-P0-02", "local block invocation typing"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M261-B002-SPC-01", "## M261 capture legality, escape classification, and invocation typing (B002)"),
        SnippetCheck("M261-B002-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-B002-SPC-03", "`lowering_block_storage_escape`"),
        SnippetCheck("M261-B002-SPC-04", "`M261-B003` is the next issue."),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M261-B002-ARCH-01", "## M261 Capture Legality, Escape Classification, And Invocation Typing (B002)"),
        SnippetCheck("M261-B002-ARCH-02", "local block values now carry callable signatures"),
        SnippetCheck("M261-B002-ARCH-03", "the next issue is `M261-B003`"),
    ),
    PARSER_CPP: (
        SnippetCheck("M261-B002-PARSER-01", "M261-B002 capture-legality/escape/invocation implementation anchor"),
        SnippetCheck("M261-B002-PARSER-02", "local block invocation typing"),
    ),
    AST_HEADER: (
        SnippetCheck("M261-B002-AST-01", "kObjc3ExecutableBlockCaptureLegalityImplementationContractId"),
        SnippetCheck("M261-B002-AST-02", "kObjc3ExecutableBlockInvocationTypingImplementationModel"),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M261-B002-SEMA-PM-01", "M261-B002 capture-legality/escape/invocation implementation"),
        SnippetCheck("M261-B002-SEMA-PM-02", "local callable"),
    ),
    SEMA_CPP: (
        SnippetCheck("M261-B002-SEMA-01", "M261-B002 capture-legality, escape-classification, and invocation"),
        SnippetCheck("M261-B002-SEMA-02", "ValidateBlockLiteralCaptureLegality"),
        SnippetCheck("M261-B002-SEMA-03", "MakeCallableSemanticTypeFromBlockLiteral"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M261-B002-LOWER-H-01", "M261-B002 capture-legality/escape/invocation implementation anchor"),
        SnippetCheck("M261-B002-LOWER-H-02", "kObjc3BlockRuntimeSemanticRulesLaneContract"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M261-B002-LOWER-CPP-01", "M261-B002 capture-legality/escape/invocation implementation anchor"),
        SnippetCheck("M261-B002-LOWER-CPP-02", "contract.byref_slot_count_total > contract.mutable_capture_count_total"),
    ),
    IR_CPP: (
        SnippetCheck("M261-B002-IR-01", "M261-B002 capture-legality/escape/invocation implementation anchor"),
        SnippetCheck("M261-B002-IR-02", "; executable_block_runtime_semantic_rules = "),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M261-B002-PKG-01", '"check:objc3c:m261-b002-capture-legality-escape-invocation": "python scripts/check_m261_b002_capture_legality_escape_classification_and_invocation_typing_core_feature_implementation.py"'),
        SnippetCheck("M261-B002-PKG-02", '"test:tooling:m261-b002-capture-legality-escape-invocation": "python -m pytest tests/tooling/test_check_m261_b002_capture_legality_escape_classification_and_invocation_typing_core_feature_implementation.py -q"'),
        SnippetCheck("M261-B002-PKG-03", '"check:objc3c:m261-b002-lane-b-readiness": "python scripts/run_m261_b002_lane_b_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M261-B002-RUN-01", "scripts/build_objc3c_native_docs.py"),
        SnippetCheck("M261-B002-RUN-02", "build:objc3c-native"),
        SnippetCheck("M261-B002-RUN-03", "check_m261_b002_capture_legality_escape_classification_and_invocation_typing_core_feature_implementation.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M261-B002-TEST-01", "def test_m261_b002_checker_emits_summary() -> None:"),
        SnippetCheck("M261-B002-TEST-02", CONTRACT_ID),
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


def diagnostics_codes(path: Path) -> list[str]:
    payload = load_json(path)
    diagnostics = payload.get("diagnostics", [])
    if not isinstance(diagnostics, list):
        return []
    return [str(diag.get("code", "")) for diag in diagnostics if isinstance(diag, dict)]


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def semantic_surface(manifest_path: Path, key: str) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    frontend = manifest.get("frontend", {})
    pipeline = frontend.get("pipeline", {})
    surfaces = pipeline.get("semantic_surface", {})
    surface = surfaces.get(key)
    if not isinstance(surface, dict):
        raise TypeError(f"missing {key} in {display_path(manifest_path)}")
    return surface


def run_dynamic_probes(failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    payload: dict[str, Any] = {}
    positive_out_dir = PROBE_ROOT / "positive-source-only"
    missing_out_dir = PROBE_ROOT / "missing-capture"
    bad_call_out_dir = PROBE_ROOT / "bad-call"
    native_out_dir = PROBE_ROOT / "native-fail-closed"
    for out_dir in (positive_out_dir, missing_out_dir, bad_call_out_dir, native_out_dir):
        out_dir.mkdir(parents=True, exist_ok=True)

    positive_summary = positive_out_dir / "runner-summary.json"
    missing_summary = missing_out_dir / "runner-summary.json"
    bad_call_summary = bad_call_out_dir / "runner-summary.json"

    checks_total += require(RUNNER_EXE.exists(), display_path(RUNNER_EXE), "M261-B002-DYN-01", "frontend runner binary is missing", failures)
    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M261-B002-DYN-02", "native binary is missing", failures)
    checks_total += require(POSITIVE_FIXTURE.exists(), display_path(POSITIVE_FIXTURE), "M261-B002-DYN-03", "positive fixture is missing", failures)
    checks_total += require(MISSING_CAPTURE_FIXTURE.exists(), display_path(MISSING_CAPTURE_FIXTURE), "M261-B002-DYN-04", "missing-capture fixture is missing", failures)
    checks_total += require(BAD_CALL_FIXTURE.exists(), display_path(BAD_CALL_FIXTURE), "M261-B002-DYN-05", "bad-call fixture is missing", failures)
    if failures:
        return checks_total, payload

    positive_completed = run_process([
        str(RUNNER_EXE),
        str(POSITIVE_FIXTURE),
        "--out-dir",
        str(positive_out_dir),
        "--emit-prefix",
        "module",
        "--summary-out",
        str(positive_summary),
        "--no-emit-ir",
        "--no-emit-object",
    ])
    positive_manifest = positive_out_dir / "module.manifest.json"
    positive_diag = positive_out_dir / "module.diagnostics.json"
    positive_ir = positive_out_dir / "module.ll"
    positive_obj = positive_out_dir / "module.obj"

    checks_total += require(positive_completed.returncode == 0, display_path(positive_out_dir), "M261-B002-DYN-06", f"positive source-only runner probe failed: {positive_completed.stdout}{positive_completed.stderr}", failures)
    checks_total += require(positive_summary.exists(), display_path(positive_summary), "M261-B002-DYN-07", "positive summary is missing", failures)
    checks_total += require(positive_manifest.exists(), display_path(positive_manifest), "M261-B002-DYN-08", "positive manifest is missing", failures)
    checks_total += require(positive_diag.exists(), display_path(positive_diag), "M261-B002-DYN-09", "positive diagnostics are missing", failures)
    checks_total += require(not positive_ir.exists(), display_path(positive_ir), "M261-B002-DYN-10", "positive source-only probe must not emit IR", failures)
    checks_total += require(not positive_obj.exists(), display_path(positive_obj), "M261-B002-DYN-11", "positive source-only probe must not emit an object", failures)

    positive_storage_surface: dict[str, Any] = {}
    positive_copy_surface: dict[str, Any] = {}
    positive_diag_codes: list[str] = []
    if positive_summary.exists() and positive_manifest.exists() and positive_diag.exists():
        positive_summary_payload = load_json(positive_summary)
        positive_storage_surface = semantic_surface(
            positive_manifest, "objc_block_storage_escape_lowering_surface"
        )
        positive_copy_surface = semantic_surface(
            positive_manifest, "objc_block_copy_dispose_lowering_surface"
        )
        positive_diag_codes = diagnostics_codes(positive_diag)
        checks_total += require(positive_summary_payload.get("success") is True, display_path(positive_summary), "M261-B002-DYN-12", "positive summary must report success", failures)
        checks_total += require(positive_summary_payload.get("semantic_skipped") is False, display_path(positive_summary), "M261-B002-DYN-13", "positive summary must not skip sema", failures)
        checks_total += require(positive_summary_payload.get("stages", {}).get("sema", {}).get("attempted") is True, display_path(positive_summary), "M261-B002-DYN-14", "positive summary must attempt sema", failures)
        checks_total += require(positive_diag_codes == [], display_path(positive_diag), "M261-B002-DYN-15", f"positive diagnostics must stay empty, observed {positive_diag_codes}", failures)
        checks_total += require(positive_storage_surface.get("block_literal_sites") == 1, display_path(positive_manifest), "M261-B002-DYN-16", "unexpected block storage literal site count", failures)
        checks_total += require(positive_storage_surface.get("mutable_capture_count_total") == 1, display_path(positive_manifest), "M261-B002-DYN-17", "unexpected mutable capture count", failures)
        checks_total += require(positive_storage_surface.get("byref_slot_count_total") == 1, display_path(positive_manifest), "M261-B002-DYN-18", "unexpected byref slot count", failures)
        checks_total += require(positive_storage_surface.get("capture_entries_total") == 2, display_path(positive_manifest), "M261-B002-DYN-19", "unexpected capture entry count", failures)
        checks_total += require(positive_storage_surface.get("requires_byref_cells_sites") == 1, display_path(positive_manifest), "M261-B002-DYN-20", "unexpected byref-cell requirement count", failures)
        checks_total += require(positive_storage_surface.get("escape_analysis_enabled_sites") == 1, display_path(positive_manifest), "M261-B002-DYN-21", "unexpected escape-analysis site count", failures)
        checks_total += require(positive_storage_surface.get("escape_to_heap_sites") == 1, display_path(positive_manifest), "M261-B002-DYN-22", "unexpected escape-to-heap site count", failures)
        checks_total += require(positive_storage_surface.get("escape_profile_normalized_sites") == 1, display_path(positive_manifest), "M261-B002-DYN-23", "unexpected normalized escape profile count", failures)
        checks_total += require(positive_storage_surface.get("byref_layout_symbolized_sites") == 1, display_path(positive_manifest), "M261-B002-DYN-24", "unexpected byref layout symbolized count", failures)
        checks_total += require(positive_storage_surface.get("contract_violation_sites") == 0, display_path(positive_manifest), "M261-B002-DYN-25", "block storage escape contract violations must stay zero", failures)
        checks_total += require("lane_contract=m168-block-storage-escape-lowering-v1" in str(positive_storage_surface.get("replay_key", "")), display_path(positive_manifest), "M261-B002-DYN-26", "block storage escape replay key drifted", failures)
        checks_total += require(positive_copy_surface.get("block_literal_sites") == 1, display_path(positive_manifest), "M261-B002-DYN-27", "unexpected copy/dispose block literal site count", failures)
        checks_total += require(positive_copy_surface.get("mutable_capture_count_total") == 1, display_path(positive_manifest), "M261-B002-DYN-28", "unexpected copy/dispose mutable capture count", failures)
        checks_total += require(positive_copy_surface.get("byref_slot_count_total") == 1, display_path(positive_manifest), "M261-B002-DYN-29", "unexpected copy/dispose byref count", failures)
        checks_total += require(positive_copy_surface.get("copy_helper_required_sites") == 1, display_path(positive_manifest), "M261-B002-DYN-30", "unexpected copy-helper requirement count", failures)
        checks_total += require(positive_copy_surface.get("dispose_helper_required_sites") == 1, display_path(positive_manifest), "M261-B002-DYN-31", "unexpected dispose-helper requirement count", failures)
        checks_total += require(positive_copy_surface.get("profile_normalized_sites") == 1, display_path(positive_manifest), "M261-B002-DYN-32", "unexpected copy/dispose normalized profile count", failures)
        checks_total += require(positive_copy_surface.get("copy_helper_symbolized_sites") == 1, display_path(positive_manifest), "M261-B002-DYN-33", "unexpected copy-helper symbolized count", failures)
        checks_total += require(positive_copy_surface.get("dispose_helper_symbolized_sites") == 1, display_path(positive_manifest), "M261-B002-DYN-34", "unexpected dispose-helper symbolized count", failures)
        checks_total += require(positive_copy_surface.get("contract_violation_sites") == 0, display_path(positive_manifest), "M261-B002-DYN-35", "block copy/dispose contract violations must stay zero", failures)
        checks_total += require("lane_contract=m169-block-copy-dispose-lowering-v1" in str(positive_copy_surface.get("replay_key", "")), display_path(positive_manifest), "M261-B002-DYN-36", "block copy/dispose replay key drifted", failures)

    missing_completed = run_process([
        str(RUNNER_EXE),
        str(MISSING_CAPTURE_FIXTURE),
        "--out-dir",
        str(missing_out_dir),
        "--emit-prefix",
        "module",
        "--summary-out",
        str(missing_summary),
        "--no-emit-ir",
        "--no-emit-object",
    ])
    missing_diag = missing_out_dir / "module.diagnostics.json"
    missing_diag_codes: list[str] = []
    checks_total += require(missing_completed.returncode != 0, display_path(missing_out_dir), "M261-B002-DYN-37", "missing-capture probe must fail", failures)
    checks_total += require(missing_summary.exists(), display_path(missing_summary), "M261-B002-DYN-38", "missing-capture summary is missing", failures)
    checks_total += require(missing_diag.exists(), display_path(missing_diag), "M261-B002-DYN-39", "missing-capture diagnostics are missing", failures)
    if missing_diag.exists():
        missing_diag_codes = diagnostics_codes(missing_diag)
        checks_total += require("O3S202" in missing_diag_codes, display_path(missing_diag), "M261-B002-DYN-40", f"expected O3S202 in missing-capture diagnostics, observed {missing_diag_codes}", failures)

    bad_call_completed = run_process([
        str(RUNNER_EXE),
        str(BAD_CALL_FIXTURE),
        "--out-dir",
        str(bad_call_out_dir),
        "--emit-prefix",
        "module",
        "--summary-out",
        str(bad_call_summary),
        "--no-emit-ir",
        "--no-emit-object",
    ])
    bad_call_diag = bad_call_out_dir / "module.diagnostics.json"
    bad_call_diag_codes: list[str] = []
    checks_total += require(bad_call_completed.returncode != 0, display_path(bad_call_out_dir), "M261-B002-DYN-41", "bad-call probe must fail", failures)
    checks_total += require(bad_call_summary.exists(), display_path(bad_call_summary), "M261-B002-DYN-42", "bad-call summary is missing", failures)
    checks_total += require(bad_call_diag.exists(), display_path(bad_call_diag), "M261-B002-DYN-43", "bad-call diagnostics are missing", failures)
    if bad_call_diag.exists():
        bad_call_diag_codes = diagnostics_codes(bad_call_diag)
        checks_total += require("O3S206" in bad_call_diag_codes, display_path(bad_call_diag), "M261-B002-DYN-44", f"expected O3S206 in bad-call diagnostics, observed {bad_call_diag_codes}", failures)

    native_completed = run_process([
        str(NATIVE_EXE),
        str(POSITIVE_FIXTURE),
        "--out-dir",
        str(native_out_dir),
        "--emit-prefix",
        "module",
    ])
    native_diag = native_out_dir / "module.diagnostics.json"
    native_manifest = native_out_dir / "module.manifest.json"
    native_ir = native_out_dir / "module.ll"
    native_obj = native_out_dir / "module.obj"
    native_diag_codes: list[str] = []
    checks_total += require(native_completed.returncode != 0, display_path(native_out_dir), "M261-B002-DYN-45", "native block probe must still fail closed", failures)
    checks_total += require(native_diag.exists(), display_path(native_diag), "M261-B002-DYN-46", "native fail-closed diagnostics are missing", failures)
    checks_total += require(not native_manifest.exists(), display_path(native_manifest), "M261-B002-DYN-47", "native fail-closed path must not emit a manifest", failures)
    checks_total += require(not native_ir.exists(), display_path(native_ir), "M261-B002-DYN-48", "native fail-closed path must not emit IR", failures)
    checks_total += require(not native_obj.exists(), display_path(native_obj), "M261-B002-DYN-49", "native fail-closed path must not emit an object", failures)
    if native_diag.exists():
        native_diag_codes = diagnostics_codes(native_diag)
        checks_total += require("O3S221" in native_diag_codes, display_path(native_diag), "M261-B002-DYN-50", f"expected O3S221 in native diagnostics, observed {native_diag_codes}", failures)

    payload["positive_source_only_case"] = {
        "out_dir": display_path(positive_out_dir),
        "summary_path": display_path(positive_summary),
        "manifest_path": display_path(positive_manifest),
        "diagnostics_path": display_path(positive_diag),
        "diagnostic_codes": positive_diag_codes,
        "storage_escape_surface": positive_storage_surface,
        "copy_dispose_surface": positive_copy_surface,
    }
    payload["missing_capture_case"] = {
        "out_dir": display_path(missing_out_dir),
        "summary_path": display_path(missing_summary),
        "diagnostics_path": display_path(missing_diag),
        "diagnostic_codes": missing_diag_codes,
    }
    payload["bad_call_case"] = {
        "out_dir": display_path(bad_call_out_dir),
        "summary_path": display_path(bad_call_summary),
        "diagnostics_path": display_path(bad_call_diag),
        "diagnostic_codes": bad_call_diag_codes,
    }
    payload["native_fail_closed_case"] = {
        "out_dir": display_path(native_out_dir),
        "diagnostics_path": display_path(native_diag),
        "diagnostic_codes": native_diag_codes,
    }
    return checks_total, payload


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    for path, snippets in STATIC_SNIPPETS.items():
        added_checks, added_failures = check_static_contract(path, snippets)
        checks_total += added_checks
        failures.extend(added_failures)

    dynamic_payload: dict[str, Any]
    if args.skip_dynamic_probes:
        dynamic_payload = {
            "positive_source_only_case": {"skipped": True},
            "missing_capture_case": {"skipped": True},
            "bad_call_case": {"skipped": True},
            "native_fail_closed_case": {"skipped": True},
        }
    else:
        added_checks, dynamic_payload = run_dynamic_probes(failures)
        checks_total += added_checks

    checks_passed = checks_total - len(failures)
    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "capture_legality_model": CAPTURE_LEGALITY_MODEL,
        "escape_classification_model": ESCAPE_CLASSIFICATION_MODEL,
        "invocation_typing_model": INVOCATION_TYPING_MODEL,
        "next_issue": NEXT_ISSUE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "failures": [finding.__dict__ for finding in failures],
        **dynamic_payload,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}")
        print(f"wrote summary: {display_path(args.summary_out)}")
        return 1
    print(f"wrote summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

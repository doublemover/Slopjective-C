#!/usr/bin/env python3
"""Checker for M261-B001 block runtime semantic rules freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m261-b001-block-runtime-semantic-rules-freeze-v1"
CONTRACT_ID = "objc3c-executable-block-runtime-semantic-rules/m261-b001-v1"
CAPTURE_LEGALITY_MODEL = (
    "block-runtime-semantics-freeze-capture-legality-on-deterministic-source-owned-capture-inventory"
)
STORAGE_CLASS_MODEL = (
    "block-runtime-semantics-freeze-byref-storage-as-source-annotation-only-until-runnable-byref-lowering-lands"
)
ESCAPE_BEHAVIOR_MODEL = (
    "block-runtime-semantics-freeze-escape-shape-as-source-annotation-only-until-runnable-heap-promotion-lands"
)
HELPER_GENERATION_MODEL = (
    "block-runtime-semantics-freeze-helper-intent-as-source-annotation-only-until-runnable-helper-lowering-lands"
)
INVOCATION_MODEL = (
    "block-runtime-semantics-freeze-block-literals-as-source-only-function-shaped-values-while-runnable-invocation-fails-closed"
)
FAIL_CLOSED_MODEL = (
    "block-runtime-semantics-fail-closed-on-native-emit-before-runnable-block-semantics-land"
)
NEXT_ISSUE = "M261-B002"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m261" / "M261-B001" / "block_runtime_semantic_rules_contract_summary.json"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m261_block_runtime_semantic_rules_contract_and_architecture_freeze_b001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m261" / "m261_b001_block_runtime_semantic_rules_contract_and_architecture_freeze_packet.md"
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
READINESS_RUNNER = ROOT / "scripts" / "run_m261_b001_lane_b_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m261_b001_block_runtime_semantic_rules_contract_and_architecture_freeze.py"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
HELLO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
BLOCK_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_block_source_storage_annotations_positive.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m261" / "b001-block-runtime-semantic-rules"
PARSER_FAILURE_CODES = {"O3P166", "O3P110", "O3P104", "O3P100"}


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
        SnippetCheck("M261-B001-EXP-01", "# M261 Block Runtime Semantic Rules Contract And Architecture Freeze Expectations (B001)"),
        SnippetCheck("M261-B001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M261-B001-EXP-03", "`tests/tooling/fixtures/native/m261_block_source_storage_annotations_positive.objc3`"),
        SnippetCheck("M261-B001-EXP-04", "`O3S221`"),
        SnippetCheck("M261-B001-EXP-05", "`M261-B002`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M261-B001-PKT-01", "# M261-B001 Block Runtime Semantic Rules Contract And Architecture Freeze Packet"),
        SnippetCheck("M261-B001-PKT-02", "Issue: `#7182`"),
        SnippetCheck("M261-B001-PKT-03", "Packet: `M261-B001`"),
        SnippetCheck("M261-B001-PKT-04", "`M261-B002` is the explicit next issue after this freeze lands."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M261-B001-SRC-01", "## M261 block runtime semantic rules (M261-B001)"),
        SnippetCheck("M261-B001-SRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-B001-SRC-03", "`; executable_block_runtime_semantic_rules = ...`"),
        SnippetCheck("M261-B001-SRC-04", "`M261-B002` is the next issue."),
    ),
    NATIVE_DOC: (
        SnippetCheck("M261-B001-NDOC-01", "## M261 block runtime semantic rules (M261-B001)"),
        SnippetCheck("M261-B001-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-B001-NDOC-03", "`; executable_block_runtime_semantic_rules = ...`"),
        SnippetCheck("M261-B001-NDOC-04", "`M261-B002` is the next issue."),
    ),
    PART0_SPEC: (
        SnippetCheck("M261-B001-P0-01", "the current semantic rule boundary is now frozen (`M261-B001`)"),
        SnippetCheck("M261-B001-P0-02", "manifest projection only"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M261-B001-SPC-01", "## M261 block runtime semantic rules (B001)"),
        SnippetCheck("M261-B001-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-B001-SPC-03", "`; executable_block_runtime_semantic_rules = ...`"),
        SnippetCheck("M261-B001-SPC-04", "`M261-B002` is the next issue."),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M261-B001-ARCH-01", "## M261 Block Runtime Semantic Rules (B001)"),
        SnippetCheck("M261-B001-ARCH-02", "`; executable_block_runtime_semantic_rules = ...`"),
        SnippetCheck("M261-B001-ARCH-03", "the next issue is `M261-B002`"),
    ),
    PARSER_CPP: (
        SnippetCheck("M261-B001-PARSER-01", "M261-B001 block-runtime-semantic-rules freeze anchor"),
        SnippetCheck("M261-B001-PARSER-02", "block_copy_helper_intent_required"),
    ),
    AST_HEADER: (
        SnippetCheck("M261-B001-AST-01", "kObjc3ExecutableBlockRuntimeSemanticRulesContractId"),
        SnippetCheck("M261-B001-AST-02", "kObjc3ExecutableBlockRuntimeInvocationModel"),
        SnippetCheck("M261-B001-AST-03", "kObjc3ExecutableBlockRuntimeFailClosedModel"),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M261-B001-SEMA-PM-01", "M261-B001 block-runtime-semantic-rules freeze anchor"),
        SnippetCheck("M261-B001-SEMA-PM-02", "allow_source_only_block_literals"),
    ),
    SEMA_CPP: (
        SnippetCheck("M261-B001-SEMA-01", "M261-B001 block-runtime-semantic-rules freeze anchor"),
        SnippetCheck("M261-B001-SEMA-02", "unsupported feature claim: block literals are not yet runnable in Objective-C 3 native mode"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M261-B001-LOWER-H-01", "kObjc3BlockRuntimeSemanticRulesLaneContract"),
        SnippetCheck("M261-B001-LOWER-H-02", "std::string Objc3ExecutableBlockRuntimeSemanticRulesSummary();"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M261-B001-LOWER-CPP-01", "M261-B001 block-runtime-semantic-rules freeze anchor"),
        SnippetCheck("M261-B001-LOWER-CPP-02", "std::string Objc3ExecutableBlockRuntimeSemanticRulesSummary()"),
    ),
    IR_CPP: (
        SnippetCheck("M261-B001-IR-01", "M261-B001 block-runtime-semantic-rules freeze anchor"),
        SnippetCheck("M261-B001-IR-02", "; executable_block_runtime_semantic_rules = "),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M261-B001-PKG-01", '"check:objc3c:m261-b001-block-runtime-semantic-rules-contract": "python scripts/check_m261_b001_block_runtime_semantic_rules_contract_and_architecture_freeze.py"'),
        SnippetCheck("M261-B001-PKG-02", '"test:tooling:m261-b001-block-runtime-semantic-rules-contract": "python -m pytest tests/tooling/test_check_m261_b001_block_runtime_semantic_rules_contract_and_architecture_freeze.py -q"'),
        SnippetCheck("M261-B001-PKG-03", '"check:objc3c:m261-b001-lane-b-readiness": "python scripts/run_m261_b001_lane_b_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M261-B001-RUN-01", "scripts/build_objc3c_native_docs.py"),
        SnippetCheck("M261-B001-RUN-02", "build:objc3c-native"),
        SnippetCheck("M261-B001-RUN-03", "check_m261_b001_block_runtime_semantic_rules_contract_and_architecture_freeze.py"),
        SnippetCheck("M261-B001-RUN-04", "test_check_m261_b001_block_runtime_semantic_rules_contract_and_architecture_freeze.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M261-B001-TEST-01", "def test_m261_b001_checker_emits_summary() -> None:"),
        SnippetCheck("M261-B001-TEST-02", CONTRACT_ID),
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


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


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

    checks_total += require(RUNNER_EXE.exists(), display_path(RUNNER_EXE), "M261-B001-DYN-01", "frontend runner binary is missing", failures)
    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M261-B001-DYN-02", "native binary is missing", failures)
    checks_total += require(HELLO_FIXTURE.exists(), display_path(HELLO_FIXTURE), "M261-B001-DYN-03", "hello fixture is missing", failures)
    checks_total += require(BLOCK_FIXTURE.exists(), display_path(BLOCK_FIXTURE), "M261-B001-DYN-04", "block fixture is missing", failures)
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
    hello_ir = hello_out_dir / "module.ll"
    hello_diag = hello_out_dir / "module.diagnostics.json"
    hello_codes: list[str] = []
    hello_ir_text = ""
    checks_total += require(hello_completed.returncode == 0, display_path(hello_out_dir), "M261-B001-DYN-05", f"hello native probe failed: {hello_completed.stdout}{hello_completed.stderr}", failures)
    checks_total += require(hello_ir.exists(), display_path(hello_ir), "M261-B001-DYN-06", "hello IR is missing", failures)
    checks_total += require(hello_diag.exists(), display_path(hello_diag), "M261-B001-DYN-07", "hello diagnostics are missing", failures)
    if hello_ir.exists() and hello_diag.exists():
        hello_codes = diagnostics_codes(hello_diag)
        hello_ir_text = hello_ir.read_text(encoding="utf-8")
        checks_total += require(hello_codes == [], display_path(hello_diag), "M261-B001-DYN-08", f"hello diagnostics must stay empty, observed {hello_codes}", failures)
        checks_total += require("; executable_block_runtime_semantic_rules = " in hello_ir_text, display_path(hello_ir), "M261-B001-DYN-09", "hello IR is missing the block runtime semantic-rules summary line", failures)
        checks_total += require(CONTRACT_ID in hello_ir_text, display_path(hello_ir), "M261-B001-DYN-10", "hello IR is missing the B001 contract id", failures)
        checks_total += require(FAIL_CLOSED_MODEL in hello_ir_text, display_path(hello_ir), "M261-B001-DYN-11", "hello IR is missing the B001 fail-closed model", failures)

    source_completed = run_process([
        str(RUNNER_EXE),
        str(BLOCK_FIXTURE),
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
    source_codes: list[str] = []
    source_surface: dict[str, Any] = {}
    checks_total += require(source_completed.returncode == 0, display_path(source_out_dir), "M261-B001-DYN-12", f"source-only block probe failed: {source_completed.stdout}{source_completed.stderr}", failures)
    checks_total += require(source_summary.exists(), display_path(source_summary), "M261-B001-DYN-13", "source-only runner summary is missing", failures)
    checks_total += require(source_manifest.exists(), display_path(source_manifest), "M261-B001-DYN-14", "source-only manifest is missing", failures)
    checks_total += require(source_diag.exists(), display_path(source_diag), "M261-B001-DYN-15", "source-only diagnostics are missing", failures)
    if source_summary.exists() and source_manifest.exists() and source_diag.exists():
        source_summary_payload = load_json(source_summary)
        source_codes = diagnostics_codes(source_diag)
        source_surface = block_source_storage_surface(source_manifest)
        checks_total += require(source_summary_payload.get("success") is True, display_path(source_summary), "M261-B001-DYN-16", "source-only summary must report success", failures)
        checks_total += require(source_summary_payload.get("semantic_skipped") is False, display_path(source_summary), "M261-B001-DYN-17", "source-only summary must not skip sema", failures)
        checks_total += require(source_codes == [], display_path(source_diag), "M261-B001-DYN-18", f"source-only diagnostics must stay empty, observed {source_codes}", failures)
        checks_total += require(source_surface.get("block_literal_sites") == 2, display_path(source_manifest), "M261-B001-DYN-19", "source-only semantic admission should preserve the block fixture", failures)
        checks_total += require(source_surface.get("deterministic_handoff") is True, display_path(source_manifest), "M261-B001-DYN-20", "source-only manifest must preserve deterministic source annotations", failures)

    native_completed = run_process([
        str(NATIVE_EXE),
        str(BLOCK_FIXTURE),
        "--out-dir",
        str(native_out_dir),
        "--emit-prefix",
        "module",
    ])
    native_diag = native_out_dir / "module.diagnostics.json"
    native_codes: list[str] = []
    checks_total += require(native_completed.returncode != 0, display_path(native_out_dir), "M261-B001-DYN-21", "native emit path must still fail closed", failures)
    checks_total += require(native_diag.exists(), display_path(native_diag), "M261-B001-DYN-22", "native fail-closed diagnostics are missing", failures)
    if native_diag.exists():
        native_codes = diagnostics_codes(native_diag)
        parser_failures = sorted(code for code in native_codes if code in PARSER_FAILURE_CODES)
        checks_total += require("O3S221" in native_codes, display_path(native_diag), "M261-B001-DYN-23", f"expected O3S221 in native diagnostics, observed {native_codes}", failures)
        checks_total += require(not parser_failures, display_path(native_diag), "M261-B001-DYN-24", f"unexpected parser regressions in native fail-closed probe: {parser_failures}", failures)

    payload = {
        "hello_native": {
            "returncode": hello_completed.returncode,
            "diagnostic_codes": hello_codes,
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
            "capture_legality": CAPTURE_LEGALITY_MODEL,
            "storage_class": STORAGE_CLASS_MODEL,
            "escape_behavior": ESCAPE_BEHAVIOR_MODEL,
            "helper_generation": HELPER_GENERATION_MODEL,
            "invocation": INVOCATION_MODEL,
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

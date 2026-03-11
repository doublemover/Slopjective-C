#!/usr/bin/env python3
"""Checker for M261-A002 block source-model completion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m261-a002-block-source-model-completion-v1"
CONTRACT_ID = "objc3c-executable-block-source-model-completion/m261-a002-v1"
SIGNATURE_MODEL = (
    "block-signature-source-model-publishes-typed-and-implicit-parameter-shapes-before-runnable-block-lowering"
)
CAPTURE_INVENTORY_MODEL = (
    "block-capture-inventory-publishes-byvalue-readonly-storage-truth-before-explicit-byref-support-lands"
)
INVOKE_MODEL = (
    "block-invoke-surface-publishes-deterministic-descriptor-and-invoke-symbol-identities-before-runtime-helper-emission"
)
EVIDENCE_MODEL = (
    "source-only-frontend-runner-positive-probe-plus-native-o3s221-fail-closed-negative-probe"
)
FAIL_CLOSED_MODEL = (
    "fail-closed-on-runnable-block-lowering-before-m261-b-and-m261-c-runtime-landing"
)
NEXT_ISSUE = "M261-B001"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m261" / "M261-A002" / "block_source_model_completion_summary.json"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m261_block_literal_signature_capture_inventory_and_invoke_surface_source_modeling_core_feature_implementation_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m261" / "m261_a002_block_literal_signature_capture_inventory_and_invoke_surface_source_modeling_core_feature_implementation_packet.md"
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
ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PIPELINE_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m261_a002_lane_a_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m261_a002_block_literal_signature_capture_inventory_and_invoke_surface_source_modeling_core_feature_implementation.py"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m261_block_source_model_completion_positive.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m261" / "a002-block-source-model-completion"
EXPECTED_REPLAY_KEY = (
    "signature_entries=3;explicit_typed_parameters=2;capture_inventory_entries=2;"
    "byvalue_readonly_captures=2;invoke_surface_entries=2;deterministic=true;"
    "lane_contract=m261-block-source-model-v1"
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
        SnippetCheck("M261-A002-EXP-01", "# M261 Block Literal Signature Capture Inventory And Invoke Surface Source Modeling Core Feature Implementation Expectations (A002)"),
        SnippetCheck("M261-A002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M261-A002-EXP-03", "`artifacts/bin/objc3c-frontend-c-api-runner.exe` with `--no-emit-ir --no-emit-object`"),
        SnippetCheck("M261-A002-EXP-04", "`O3S221`"),
        SnippetCheck("M261-A002-EXP-05", "`M261-B001` is the next issue."),
    ),
    PACKET_DOC: (
        SnippetCheck("M261-A002-PKT-01", "# M261-A002 Block Literal Signature Capture Inventory And Invoke Surface Source Modeling Core Feature Implementation Packet"),
        SnippetCheck("M261-A002-PKT-02", "Issue: `#7180`"),
        SnippetCheck("M261-A002-PKT-03", "Packet: `M261-A002`"),
        SnippetCheck("M261-A002-PKT-04", "`M261-B001` is the explicit next issue after this implementation lands."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M261-A002-SRC-01", "## M261 block source model completion (M261-A002)"),
        SnippetCheck("M261-A002-SRC-02", "`objc3c-frontend-c-api-runner.exe --no-emit-ir --no-emit-object`"),
        SnippetCheck("M261-A002-SRC-03", "`objc_block_source_model_completion_surface`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M261-A002-NDOC-01", "## M261 block source model completion (M261-A002)"),
        SnippetCheck("M261-A002-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-A002-NDOC-03", "`M261-B001` is the next issue."),
    ),
    PART0_SPEC: (
        SnippetCheck("M261-A002-P0-01", "source-only frontend runs may now admit block literals"),
        SnippetCheck("M261-A002-P0-02", "(`M261-A002`)"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M261-A002-SPC-01", "## M261 block source model completion (A002)"),
        SnippetCheck("M261-A002-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M261-A002-SPC-03", "`objc_block_source_model_completion_surface`"),
        SnippetCheck("M261-A002-SPC-04", "`M261-B001` is the next issue."),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M261-A002-ARCH-01", "## M261 Block Source Model Completion (A002)"),
        SnippetCheck("M261-A002-ARCH-02", "source-only frontend runner may now admit block literals"),
        SnippetCheck("M261-A002-ARCH-03", "the next issue is `M261-B001`"),
    ),
    PARSER_CPP: (
        SnippetCheck("M261-A002-PARSER-01", "M261-A002 block-source-model-completion anchor"),
        SnippetCheck("M261-A002-PARSER-02", "block_parameter_signature_entries_lexicographic"),
    ),
    AST_HEADER: (
        SnippetCheck("M261-A002-AST-01", "kObjc3ExecutableBlockSourceModelCompletionContractId"),
        SnippetCheck("M261-A002-AST-02", "block_invoke_surface_entries_lexicographic"),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M261-A002-SEMA-PM-01", "M261-A002 block-source-model-completion anchor"),
        SnippetCheck("M261-A002-SEMA-PM-02", "allow_source_only_block_literals"),
    ),
    SEMA_CPP: (
        SnippetCheck("M261-A002-SEMA-01", "M261-A002 block-source-model-completion anchor"),
        SnippetCheck("M261-A002-SEMA-02", "if (context.allow_source_only_block_literals)"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M261-A002-LOWER-H-01", "struct Objc3BlockSourceModelCompletionContract"),
        SnippetCheck("M261-A002-LOWER-H-02", "std::string Objc3BlockSourceModelCompletionReplayKey("),
    ),
    LOWERING_CPP: (
        SnippetCheck("M261-A002-LOWER-CPP-01", "std::string Objc3ExecutableBlockSourceModelCompletionSummary()"),
        SnippetCheck("M261-A002-LOWER-CPP-02", "std::string Objc3BlockSourceModelCompletionReplayKey("),
    ),
    IR_CPP: (
        SnippetCheck("M261-A002-IR-01", "M261-A002 block-source-model-completion anchor"),
        SnippetCheck("M261-A002-IR-02", "; executable_block_source_model_completion = "),
    ),
    ARTIFACTS_CPP: (
        SnippetCheck("M261-A002-ART-01", "BuildBlockSourceModelCompletionContract("),
        SnippetCheck("M261-A002-ART-02", "objc_block_source_model_completion_surface"),
    ),
    PIPELINE_CPP: (
        SnippetCheck("M261-A002-PIPE-01", "M261-A002 block-source-model-completion anchor"),
        SnippetCheck("M261-A002-PIPE-02", "semantic_options.allow_source_only_block_literals"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M261-A002-PKG-01", '"check:objc3c:m261-a002-block-source-model-completion": "python scripts/check_m261_a002_block_literal_signature_capture_inventory_and_invoke_surface_source_modeling_core_feature_implementation.py"'),
        SnippetCheck("M261-A002-PKG-02", '"test:tooling:m261-a002-block-source-model-completion": "python -m pytest tests/tooling/test_check_m261_a002_block_literal_signature_capture_inventory_and_invoke_surface_source_modeling_core_feature_implementation.py -q"'),
        SnippetCheck("M261-A002-PKG-03", '"check:objc3c:m261-a002-lane-a-readiness": "python scripts/run_m261_a002_lane_a_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M261-A002-RUN-01", "scripts/build_objc3c_native_docs.py"),
        SnippetCheck("M261-A002-RUN-02", "build:objc3c-native"),
        SnippetCheck("M261-A002-RUN-03", "test_check_m261_a002_block_literal_signature_capture_inventory_and_invoke_surface_source_modeling_core_feature_implementation.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M261-A002-TEST-01", "def test_m261_a002_checker_emits_summary() -> None:"),
        SnippetCheck("M261-A002-TEST-02", CONTRACT_ID),
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


def block_source_model_surface(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    frontend = manifest.get("frontend", {})
    pipeline = frontend.get("pipeline", {})
    semantic_surface = pipeline.get("semantic_surface", {})
    surface = semantic_surface.get("objc_block_source_model_completion_surface")
    if not isinstance(surface, dict):
        raise TypeError(f"missing objc_block_source_model_completion_surface in {display_path(manifest_path)}")
    return surface


def run_dynamic_probes(failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    payload: dict[str, Any] = {}
    source_out_dir = PROBE_ROOT / "source-only-runner"
    native_out_dir = PROBE_ROOT / "native-fail-closed"
    source_out_dir.mkdir(parents=True, exist_ok=True)
    native_out_dir.mkdir(parents=True, exist_ok=True)

    source_summary = source_out_dir / "runner-summary.json"

    checks_total += require(RUNNER_EXE.exists(), display_path(RUNNER_EXE), "M261-A002-DYN-01", "frontend runner binary is missing", failures)
    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M261-A002-DYN-02", "native binary is missing", failures)
    checks_total += require(FIXTURE.exists(), display_path(FIXTURE), "M261-A002-DYN-03", "positive fixture is missing", failures)
    if failures:
        return checks_total, payload

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

    checks_total += require(source_completed.returncode == 0, display_path(source_out_dir), "M261-A002-DYN-04", f"source-only runner probe failed: {source_completed.stdout}{source_completed.stderr}", failures)
    checks_total += require(source_summary.exists(), display_path(source_summary), "M261-A002-DYN-05", "source-only runner summary is missing", failures)
    checks_total += require(source_manifest.exists(), display_path(source_manifest), "M261-A002-DYN-06", "source-only manifest is missing", failures)
    checks_total += require(source_diag.exists(), display_path(source_diag), "M261-A002-DYN-07", "source-only diagnostics are missing", failures)
    checks_total += require(not source_ir.exists(), display_path(source_ir), "M261-A002-DYN-08", "source-only probe must not emit IR", failures)
    checks_total += require(not source_obj.exists(), display_path(source_obj), "M261-A002-DYN-09", "source-only probe must not emit an object", failures)

    source_surface: dict[str, Any] = {}
    source_diagnostic_codes: list[str] = []
    if source_summary.exists() and source_manifest.exists() and source_diag.exists():
        source_surface = block_source_model_surface(source_manifest)
        source_summary_payload = load_json(source_summary)
        source_diag_payload = load_json(source_diag)
        diagnostics = source_diag_payload.get("diagnostics", [])
        if isinstance(diagnostics, list):
            source_diagnostic_codes = [
                str(diag.get("code", "")) for diag in diagnostics if isinstance(diag, dict)
            ]
        checks_total += require(source_summary_payload.get("success") is True, display_path(source_summary), "M261-A002-DYN-10", "source-only summary must report success", failures)
        checks_total += require(source_summary_payload.get("semantic_skipped") is False, display_path(source_summary), "M261-A002-DYN-11", "source-only summary must not skip sema", failures)
        checks_total += require(source_summary_payload.get("paths", {}).get("manifest"), display_path(source_summary), "M261-A002-DYN-12", "source-only summary must report manifest path", failures)
        checks_total += require(source_summary_payload.get("paths", {}).get("ir") == "", display_path(source_summary), "M261-A002-DYN-13", "source-only summary must keep IR path empty", failures)
        checks_total += require(source_summary_payload.get("paths", {}).get("object") == "", display_path(source_summary), "M261-A002-DYN-14", "source-only summary must keep object path empty", failures)
        checks_total += require(source_summary_payload.get("stages", {}).get("sema", {}).get("attempted") is True, display_path(source_summary), "M261-A002-DYN-15", "source-only summary must attempt sema", failures)
        checks_total += require(source_diagnostic_codes == [], display_path(source_diag), "M261-A002-DYN-16", f"source-only diagnostics must stay empty, observed {source_diagnostic_codes}", failures)
        checks_total += require(source_surface.get("block_literal_sites") == 1, display_path(source_manifest), "M261-A002-DYN-17", "unexpected block literal site count", failures)
        checks_total += require(source_surface.get("signature_entries_total") == 3, display_path(source_manifest), "M261-A002-DYN-18", "unexpected signature entry count", failures)
        checks_total += require(source_surface.get("explicit_typed_parameter_entries_total") == 2, display_path(source_manifest), "M261-A002-DYN-19", "unexpected explicit typed parameter count", failures)
        checks_total += require(source_surface.get("implicit_parameter_entries_total") == 1, display_path(source_manifest), "M261-A002-DYN-20", "unexpected implicit parameter count", failures)
        checks_total += require(source_surface.get("capture_inventory_entries_total") == 2, display_path(source_manifest), "M261-A002-DYN-21", "unexpected capture inventory count", failures)
        checks_total += require(source_surface.get("byvalue_readonly_capture_entries_total") == 2, display_path(source_manifest), "M261-A002-DYN-22", "unexpected by-value readonly capture count", failures)
        checks_total += require(source_surface.get("invoke_surface_entries_total") == 2, display_path(source_manifest), "M261-A002-DYN-23", "unexpected invoke surface count", failures)
        checks_total += require(source_surface.get("non_normalized_sites") == 0, display_path(source_manifest), "M261-A002-DYN-24", "non-normalized sites must stay zero", failures)
        checks_total += require(source_surface.get("contract_violation_sites") == 0, display_path(source_manifest), "M261-A002-DYN-25", "contract violation sites must stay zero", failures)
        checks_total += require(source_surface.get("deterministic_handoff") is True, display_path(source_manifest), "M261-A002-DYN-26", "deterministic handoff must stay true", failures)
        checks_total += require(source_surface.get("replay_key") == EXPECTED_REPLAY_KEY, display_path(source_manifest), "M261-A002-DYN-27", "source-model replay key drifted", failures)

    native_completed = run_process([
        str(NATIVE_EXE),
        str(FIXTURE),
        "--out-dir",
        str(native_out_dir),
        "--emit-prefix",
        "module",
    ])
    native_diag = native_out_dir / "module.diagnostics.json"
    native_manifest = native_out_dir / "module.manifest.json"
    native_ir = native_out_dir / "module.ll"
    native_obj = native_out_dir / "module.obj"
    checks_total += require(native_completed.returncode != 0, display_path(native_out_dir), "M261-A002-DYN-28", "native emit path must still fail closed", failures)
    checks_total += require(native_diag.exists(), display_path(native_diag), "M261-A002-DYN-29", "native fail-closed diagnostics are missing", failures)
    native_diagnostic_codes: list[str] = []
    if native_diag.exists():
        native_diag_payload = load_json(native_diag)
        diagnostics = native_diag_payload.get("diagnostics", [])
        if isinstance(diagnostics, list):
            native_diagnostic_codes = [
                str(diag.get("code", "")) for diag in diagnostics if isinstance(diag, dict)
            ]
        checks_total += require("O3S221" in native_diagnostic_codes, display_path(native_diag), "M261-A002-DYN-30", f"expected O3S221 in native fail-closed diagnostics, observed {native_diagnostic_codes}", failures)
    checks_total += require(not native_manifest.exists(), display_path(native_manifest), "M261-A002-DYN-31", "native fail-closed path must not emit a manifest", failures)
    checks_total += require(not native_ir.exists(), display_path(native_ir), "M261-A002-DYN-32", "native fail-closed path must not emit IR", failures)
    checks_total += require(not native_obj.exists(), display_path(native_obj), "M261-A002-DYN-33", "native fail-closed path must not emit an object", failures)

    payload["positive_source_only_case"] = {
        "out_dir": display_path(source_out_dir),
        "summary_path": display_path(source_summary),
        "manifest_path": display_path(source_manifest),
        "diagnostics_path": display_path(source_diag),
        "diagnostic_codes": source_diagnostic_codes,
        "surface": source_surface,
    }
    payload["negative_native_case"] = {
        "out_dir": display_path(native_out_dir),
        "diagnostics_path": display_path(native_diag),
        "diagnostic_codes": native_diagnostic_codes,
        "manifest_emitted": native_manifest.exists(),
        "ir_emitted": native_ir.exists(),
        "object_emitted": native_obj.exists(),
    }
    return checks_total, payload


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0

    for path, snippets in STATIC_SNIPPETS.items():
        static_total, static_failures = check_static_contract(path, snippets)
        checks_total += static_total
        failures.extend(static_failures)

    dynamic_payload: dict[str, Any]
    if args.skip_dynamic_probes:
        dynamic_payload = {"skipped": True}
    else:
        dynamic_total, dynamic_payload = run_dynamic_probes(failures)
        checks_total += dynamic_total

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "signature_model": SIGNATURE_MODEL,
        "capture_inventory_model": CAPTURE_INVENTORY_MODEL,
        "invoke_surface_model": INVOKE_MODEL,
        "evidence_model": EVIDENCE_MODEL,
        "fail_closed_model": FAIL_CLOSED_MODEL,
        "next_issue": NEXT_ISSUE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_case": dynamic_payload,
        "findings": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        print(f"[info] wrote summary to {display_path(args.summary_out)}", file=sys.stderr)
        return 1
    print(f"[ok] M261-A002 block source model completion validated ({checks_total} checks)")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

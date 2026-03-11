#!/usr/bin/env python3
"""Deterministic checker for M262-B002 ARC inference and lifetime semantics."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m262-b002-implicit-retain-release-inference-and-lifetime-extension-semantics-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-arc-inference-lifetime/m262-b002-v1"
SOURCE_MODEL = "explicit-arc-mode-now-infers-strong-owned-executable-object-signatures-for-the-supported-runnable-slice"
SEMANTIC_MODEL = "arc-enabled-unqualified-object-signatures-now-produce-canonical-retain-release-lifetime-accounting-while-nonarc-remains-zero-inference"
FAIL_CLOSED_MODEL = "non-arc-mode-keeps-unqualified-object-signatures-non-inferred-and-zero-retain-release-lifetime-accounting"
NON_GOAL_MODEL = "no-full-arc-cleanup-synthesis-no-weak-autorelease-return-property-synthesis-or-block-interaction-arc-semantics-yet"
NEXT_ISSUE = "M262-B003"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m262_implicit_retain_release_inference_and_lifetime_extension_semantics_core_feature_implementation_b002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m262" / "m262_b002_implicit_retain_release_inference_and_lifetime_extension_semantics_core_feature_implementation_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
ATTRIBUTE_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
SEMA_PASS_MANAGER_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SCAFFOLD_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_ownership_aware_lowering_behavior_scaffold.h"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m262_b002_lane_b_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m262_b002_implicit_retain_release_inference_and_lifetime_extension_semantics_core_feature_implementation.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_inference_lifetime_positive.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m262" / "b002-arc-inference-lifetime"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m262" / "M262-B002" / "arc_inference_lifetime_summary.json"
BOUNDARY_PREFIX = "; arc_inference_lifetime = "
NAMED_METADATA_LINE = "!objc3.objc_arc_inference_lifetime = !{!78}"
RETAIN_RELEASE_IR_PREFIX = "; frontend_objc_retain_release_operation_lowering_profile = "
BLOCK_ESCAPE_LANE_CONTRACT = "m168-block-storage-escape-lowering-v1"


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
        SnippetCheck("M262-B002-EXP-01", "# M262 Implicit Retain-Release Inference And Lifetime-Extension Semantics Core Feature Implementation Expectations (B002)"),
        SnippetCheck("M262-B002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M262-B002-EXP-03", "unqualified object parameters infer strong-owned retain/release behavior under `-fobjc-arc`"),
        SnippetCheck("M262-B002-EXP-04", "The contract must explicitly hand off to `M262-B003`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M262-B002-PKT-01", "# M262-B002 Implicit Retain-Release Inference And Lifetime-Extension Semantics Core Feature Implementation Packet"),
        SnippetCheck("M262-B002-PKT-02", "Issue: `#7197`"),
        SnippetCheck("M262-B002-PKT-03", "Packet: `M262-B002`"),
        SnippetCheck("M262-B002-PKT-04", "`M262-B003` is the explicit next handoff after this implementation lands."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M262-B002-SRC-01", "## M262 implicit retain-release inference and lifetime-extension semantics (M262-B002)"),
        SnippetCheck("M262-B002-SRC-02", "unqualified object parameters now infer strong-owned retain/release"),
        SnippetCheck("M262-B002-SRC-03", "`!objc3.objc_arc_inference_lifetime`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M262-B002-NDOC-01", "## M262 implicit retain-release inference and lifetime-extension semantics (M262-B002)"),
        SnippetCheck("M262-B002-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M262-B002-NDOC-03", "`M262-B003` is the next issue."),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M262-B002-SPC-01", "## M262 implicit retain-release inference and lifetime-extension semantics (B002)"),
        SnippetCheck("M262-B002-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M262-B002-SPC-03", f"`{SEMANTIC_MODEL}`"),
        SnippetCheck("M262-B002-SPC-04", "`M262-B003` is the next issue."),
    ),
    ATTRIBUTE_SPEC: (
        SnippetCheck("M262-B002-ATTR-01", "### B.2.6 ARC inference and lifetime-extension semantics (implementation note) {#b-2-6}"),
        SnippetCheck("M262-B002-ATTR-02", "under `-fobjc-arc`, unqualified object parameters and returns now infer a"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M262-B002-ARCH-01", "## M262 Implicit Retain-Release Inference And Lifetime-Extension Semantics (B002)"),
        SnippetCheck("M262-B002-ARCH-02", "non-ARC remains the truthful zero-inference baseline for the same source"),
        SnippetCheck("M262-B002-ARCH-03", "the next issue is `M262-B003`"),
    ),
    SEMA_CPP: (
        SnippetCheck("M262-B002-SEMA-01", "M262-B002 ARC inference/lifetime implementation anchor"),
        SnippetCheck("M262-B002-SEMA-02", "unqualified object parameters now synthesize a"),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M262-B002-PM-01", "M262-B002 ARC inference/lifetime implementation anchor"),
        SnippetCheck("M262-B002-PM-02", "zero-inference baseline"),
    ),
    SCAFFOLD_HEADER: (
        SnippetCheck("M262-B002-SCAF-01", "M262-B002 ARC inference/lifetime implementation anchor"),
        SnippetCheck("M262-B002-SCAF-02", "inferred strong-owned retain/release activity"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M262-B002-LHDR-01", "kObjc3ArcInferenceLifetimeContractId"),
        SnippetCheck("M262-B002-LHDR-02", "std::string Objc3ArcInferenceLifetimeSummary();"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M262-B002-LCPP-01", "std::string Objc3ArcInferenceLifetimeSummary()"),
        SnippetCheck("M262-B002-LCPP-02", ";semantic_model="),
        SnippetCheck("M262-B002-LCPP-03", ";next_issue=M262-B003"),
    ),
    IR_CPP: (
        SnippetCheck("M262-B002-IR-01", "M262-B002 ARC inference/lifetime implementation anchor"),
        SnippetCheck("M262-B002-IR-02", "; arc_inference_lifetime = "),
        SnippetCheck("M262-B002-IR-03", "objc_arc_inference_lifetime"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M262-B002-PKG-01", '"check:objc3c:m262-b002-implicit-retain-release-inference-and-lifetime-extension-semantics-contract": "python scripts/check_m262_b002_implicit_retain_release_inference_and_lifetime_extension_semantics_core_feature_implementation.py"'),
        SnippetCheck("M262-B002-PKG-02", '"test:tooling:m262-b002-implicit-retain-release-inference-and-lifetime-extension-semantics-contract": "python -m pytest tests/tooling/test_check_m262_b002_implicit_retain_release_inference_and_lifetime_extension_semantics_core_feature_implementation.py -q"'),
        SnippetCheck("M262-B002-PKG-03", '"check:objc3c:m262-b002-lane-b-readiness": "python scripts/run_m262_b002_lane_b_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M262-B002-RUN-01", "scripts/build_objc3c_native_docs.py"),
        SnippetCheck("M262-B002-RUN-02", "build:objc3c-native"),
        SnippetCheck("M262-B002-RUN-03", "test_check_m262_b002_implicit_retain_release_inference_and_lifetime_extension_semantics_core_feature_implementation.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M262-B002-TEST-01", "def test_m262_b002_checker_emits_summary() -> None:"),
        SnippetCheck("M262-B002-TEST-02", CONTRACT_ID),
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


def compile_fixture(fixture: Path, out_dir: Path, extra_args: Sequence[str] = ()) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [str(NATIVE_EXE), str(fixture), *extra_args, "--out-dir", str(out_dir), "--emit-prefix", "module"]
    return run_process(command)


def parse_replay_key(text: str) -> dict[str, str]:
    result: dict[str, str] = {}
    for part in text.split(";"):
        key, sep, value = part.partition("=")
        if sep:
            result[key.strip()] = value.strip()
    return result


def parse_ir_profile(text: str) -> dict[str, str]:
    result: dict[str, str] = {}
    for part in text.split(","):
        key, sep, value = part.partition("=")
        if sep:
            result[key.strip()] = value.strip()
    return result


def require_int(mapping: dict[str, str], key: str) -> int:
    return int(mapping.get(key, "0"))


def run_dynamic_probes(failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    payload: dict[str, Any] = {}

    arc_out_dir = PROBE_ROOT / "arc"
    nonarc_out_dir = PROBE_ROOT / "nonarc"

    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M262-B002-DYN-01", "native binary is missing", failures)
    checks_total += require(POSITIVE_FIXTURE.exists(), display_path(POSITIVE_FIXTURE), "M262-B002-DYN-02", "positive fixture is missing", failures)
    if failures:
        return checks_total, payload

    arc_completed = compile_fixture(POSITIVE_FIXTURE, arc_out_dir, ["-fobjc-arc"])
    nonarc_completed = compile_fixture(POSITIVE_FIXTURE, nonarc_out_dir)
    arc_manifest = arc_out_dir / "module.manifest.json"
    nonarc_manifest = nonarc_out_dir / "module.manifest.json"
    arc_diag = arc_out_dir / "module.diagnostics.json"
    nonarc_diag = nonarc_out_dir / "module.diagnostics.json"
    arc_ir = arc_out_dir / "module.ll"
    nonarc_ir = nonarc_out_dir / "module.ll"
    arc_obj = arc_out_dir / "module.obj"
    nonarc_obj = nonarc_out_dir / "module.obj"

    checks_total += require(arc_completed.returncode == 0, display_path(arc_out_dir), "M262-B002-DYN-03", f"ARC compile failed: {arc_completed.stdout}{arc_completed.stderr}", failures)
    checks_total += require(nonarc_completed.returncode == 0, display_path(nonarc_out_dir), "M262-B002-DYN-04", f"non-ARC compile failed: {nonarc_completed.stdout}{nonarc_completed.stderr}", failures)
    for check_id, artifact in (
        ("M262-B002-DYN-05", arc_manifest),
        ("M262-B002-DYN-06", nonarc_manifest),
        ("M262-B002-DYN-07", arc_diag),
        ("M262-B002-DYN-08", nonarc_diag),
        ("M262-B002-DYN-09", arc_ir),
        ("M262-B002-DYN-10", nonarc_ir),
        ("M262-B002-DYN-11", arc_obj),
        ("M262-B002-DYN-12", nonarc_obj),
    ):
        checks_total += require(artifact.exists(), display_path(artifact), check_id, "required probe artifact is missing", failures)

    if failures:
        return checks_total, payload

    arc_diag_payload = load_json(arc_diag)
    nonarc_diag_payload = load_json(nonarc_diag)
    arc_manifest_payload = load_json(arc_manifest)
    nonarc_manifest_payload = load_json(nonarc_manifest)
    arc_ir_text = arc_ir.read_text(encoding="utf-8")
    nonarc_ir_text = nonarc_ir.read_text(encoding="utf-8")

    checks_total += require(arc_diag_payload.get("diagnostics") == [], display_path(arc_diag), "M262-B002-DYN-13", "ARC diagnostics must stay clean", failures)
    checks_total += require(nonarc_diag_payload.get("diagnostics") == [], display_path(nonarc_diag), "M262-B002-DYN-14", "non-ARC diagnostics must stay clean", failures)
    checks_total += require(arc_manifest_payload.get("frontend", {}).get("arc_mode") == "enabled", display_path(arc_manifest), "M262-B002-DYN-15", f"expected frontend.arc_mode=enabled, observed {arc_manifest_payload.get('frontend', {}).get('arc_mode')!r}", failures)
    checks_total += require(nonarc_manifest_payload.get("frontend", {}).get("arc_mode") == "disabled", display_path(nonarc_manifest), "M262-B002-DYN-16", f"expected frontend.arc_mode=disabled, observed {nonarc_manifest_payload.get('frontend', {}).get('arc_mode')!r}", failures)
    checks_total += require(BOUNDARY_PREFIX + f"contract={CONTRACT_ID}" in arc_ir_text, display_path(arc_ir), "M262-B002-DYN-17", "ARC IR is missing the arc-inference-lifetime boundary line", failures)
    checks_total += require(BOUNDARY_PREFIX + f"contract={CONTRACT_ID}" in nonarc_ir_text, display_path(nonarc_ir), "M262-B002-DYN-18", "non-ARC IR is missing the arc-inference-lifetime boundary line", failures)
    checks_total += require(NAMED_METADATA_LINE in arc_ir_text, display_path(arc_ir), "M262-B002-DYN-19", "ARC IR is missing the arc-inference-lifetime named metadata", failures)
    checks_total += require(NAMED_METADATA_LINE in nonarc_ir_text, display_path(nonarc_ir), "M262-B002-DYN-20", "non-ARC IR is missing the arc-inference-lifetime named metadata", failures)
    checks_total += require(RETAIN_RELEASE_IR_PREFIX in arc_ir_text, display_path(arc_ir), "M262-B002-DYN-21", "ARC IR is missing retain/release lowering profile output", failures)
    checks_total += require(RETAIN_RELEASE_IR_PREFIX in nonarc_ir_text, display_path(nonarc_ir), "M262-B002-DYN-22", "non-ARC IR is missing retain/release lowering profile output", failures)

    arc_rr = parse_replay_key(arc_manifest_payload.get("lowering_retain_release_operation", {}).get("replay_key", ""))
    nonarc_rr = parse_replay_key(nonarc_manifest_payload.get("lowering_retain_release_operation", {}).get("replay_key", ""))
    arc_block = parse_replay_key(arc_manifest_payload.get("lowering_block_storage_escape", {}).get("replay_key", ""))
    nonarc_block = parse_replay_key(nonarc_manifest_payload.get("lowering_block_storage_escape", {}).get("replay_key", ""))

    checks_total += require(arc_manifest_payload.get("lowering_retain_release_operation", {}).get("deterministic_handoff") is True, display_path(arc_manifest), "M262-B002-DYN-23", "ARC retain/release handoff must remain deterministic", failures)
    checks_total += require(nonarc_manifest_payload.get("lowering_retain_release_operation", {}).get("deterministic_handoff") is True, display_path(nonarc_manifest), "M262-B002-DYN-24", "non-ARC retain/release handoff must remain deterministic", failures)
    checks_total += require(require_int(arc_rr, "ownership_qualified_sites") > 0, display_path(arc_manifest), "M262-B002-DYN-25", f"expected nonzero ARC ownership-qualified sites, observed {arc_rr}", failures)
    checks_total += require(require_int(arc_rr, "retain_insertion_sites") > 0, display_path(arc_manifest), "M262-B002-DYN-26", f"expected nonzero ARC retain insertion sites, observed {arc_rr}", failures)
    checks_total += require(require_int(arc_rr, "release_insertion_sites") > 0, display_path(arc_manifest), "M262-B002-DYN-27", f"expected nonzero ARC release insertion sites, observed {arc_rr}", failures)
    checks_total += require(require_int(arc_rr, "autorelease_insertion_sites") == 0, display_path(arc_manifest), "M262-B002-DYN-28", f"expected zero ARC autorelease insertion sites, observed {arc_rr}", failures)
    checks_total += require(require_int(arc_rr, "contract_violation_sites") == 0, display_path(arc_manifest), "M262-B002-DYN-29", f"expected zero ARC contract violation sites, observed {arc_rr}", failures)
    checks_total += require(require_int(nonarc_rr, "ownership_qualified_sites") == 0, display_path(nonarc_manifest), "M262-B002-DYN-30", f"expected zero non-ARC ownership-qualified sites, observed {nonarc_rr}", failures)
    checks_total += require(require_int(nonarc_rr, "retain_insertion_sites") == 0, display_path(nonarc_manifest), "M262-B002-DYN-31", f"expected zero non-ARC retain insertion sites, observed {nonarc_rr}", failures)
    checks_total += require(require_int(nonarc_rr, "release_insertion_sites") == 0, display_path(nonarc_manifest), "M262-B002-DYN-32", f"expected zero non-ARC release insertion sites, observed {nonarc_rr}", failures)
    checks_total += require(require_int(nonarc_rr, "autorelease_insertion_sites") == 0, display_path(nonarc_manifest), "M262-B002-DYN-33", f"expected zero non-ARC autorelease insertion sites, observed {nonarc_rr}", failures)
    checks_total += require(require_int(nonarc_rr, "contract_violation_sites") == 0, display_path(nonarc_manifest), "M262-B002-DYN-34", f"expected zero non-ARC contract violation sites, observed {nonarc_rr}", failures)
    checks_total += require(arc_manifest_payload.get("lowering_block_storage_escape", {}).get("lane_contract") == BLOCK_ESCAPE_LANE_CONTRACT, display_path(arc_manifest), "M262-B002-DYN-35", "ARC block-storage lane contract drifted", failures)
    checks_total += require(nonarc_manifest_payload.get("lowering_block_storage_escape", {}).get("lane_contract") == BLOCK_ESCAPE_LANE_CONTRACT, display_path(nonarc_manifest), "M262-B002-DYN-36", "non-ARC block-storage lane contract drifted", failures)
    checks_total += require(require_int(arc_block, "escape_analysis_enabled_sites") > 0 and require_int(arc_block, "escape_to_heap_sites") > 0, display_path(arc_manifest), "M262-B002-DYN-37", f"ARC block escape profile missing heap-promotion evidence: {arc_block}", failures)
    checks_total += require(require_int(nonarc_block, "escape_analysis_enabled_sites") > 0 and require_int(nonarc_block, "escape_to_heap_sites") > 0, display_path(nonarc_manifest), "M262-B002-DYN-38", f"non-ARC block escape profile missing heap-promotion evidence: {nonarc_block}", failures)

    arc_ir_profile = parse_ir_profile(next(line[len(RETAIN_RELEASE_IR_PREFIX):] for line in arc_ir_text.splitlines() if line.startswith(RETAIN_RELEASE_IR_PREFIX)))
    nonarc_ir_profile = parse_ir_profile(next(line[len(RETAIN_RELEASE_IR_PREFIX):] for line in nonarc_ir_text.splitlines() if line.startswith(RETAIN_RELEASE_IR_PREFIX)))
    checks_total += require(require_int(arc_ir_profile, "retain_insertion_sites") == require_int(arc_rr, "retain_insertion_sites"), display_path(arc_ir), "M262-B002-DYN-39", "ARC IR profile and manifest replay key disagree on retain sites", failures)
    checks_total += require(require_int(nonarc_ir_profile, "retain_insertion_sites") == require_int(nonarc_rr, "retain_insertion_sites"), display_path(nonarc_ir), "M262-B002-DYN-40", "non-ARC IR profile and manifest replay key disagree on retain sites", failures)

    payload["arc_case"] = {
        "out_dir": display_path(arc_out_dir),
        "frontend_arc_mode": arc_manifest_payload.get("frontend", {}).get("arc_mode"),
        "retain_release_replay": arc_rr,
        "block_storage_escape_replay": arc_block,
    }
    payload["nonarc_case"] = {
        "out_dir": display_path(nonarc_out_dir),
        "frontend_arc_mode": nonarc_manifest_payload.get("frontend", {}).get("arc_mode"),
        "retain_release_replay": nonarc_rr,
        "block_storage_escape_replay": nonarc_block,
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

    if args.skip_dynamic_probes:
        dynamic_payload: dict[str, Any] = {"skipped": True}
    else:
        dynamic_total, dynamic_payload = run_dynamic_probes(failures)
        checks_total += dynamic_total

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "source_model": SOURCE_MODEL,
        "semantic_model": SEMANTIC_MODEL,
        "non_goal_model": NON_GOAL_MODEL,
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
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        print(f"[info] wrote summary to {display_path(args.summary_out)}", file=sys.stderr)
        return 1
    print(f"[ok] M262-B002 arc inference/lifetime validated ({checks_total} checks)")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

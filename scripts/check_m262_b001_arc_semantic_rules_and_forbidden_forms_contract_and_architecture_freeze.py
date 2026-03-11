#!/usr/bin/env python3
"""Deterministic checker for M262-B001 ARC semantic rules and forbidden forms."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m262-b001-arc-semantic-rules-and-forbidden-forms-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-arc-semantic-rules/m262-b001-v1"
SOURCE_MODEL = "explicit-arc-mode-admits-only-explicit-ownership-surfaces-while-forbidden-property-forms-and-broad-inference-remain-fail-closed"
SEMANTIC_MODEL = "conflicting-property-ownership-forms-and-atomic-ownership-aware-storage-still-fail-closed-while-general-arc-inference-remains-deferred"
FAIL_CLOSED_MODEL = "forbidden-arc-property-forms-and-non-inferred-lifetime-semantics-terminate-deterministically"
NON_GOAL_MODEL = "no-implicit-retain-release-inference-no-lifetime-extension-no-method-family-based-arc-semantics-yet"
NEXT_ISSUE = "M262-B002"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m262_arc_semantic_rules_and_forbidden_forms_contract_and_architecture_freeze_b001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m262" / "m262_b001_arc_semantic_rules_and_forbidden_forms_contract_and_architecture_freeze_packet.md"
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
READINESS_RUNNER = ROOT / "scripts" / "run_m262_b001_lane_b_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m262_b001_arc_semantic_rules_and_forbidden_forms_contract_and_architecture_freeze.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_mode_handling_positive.objc3"
WEAK_MISMATCH_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m260_runtime_backed_storage_ownership_weak_mismatch_negative.objc3"
ATOMIC_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_property_atomic_ownership_negative.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m262" / "b001-arc-semantic-rules"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m262" / "M262-B001" / "arc_semantic_rules_summary.json"
BOUNDARY_PREFIX = "; arc_semantic_rules = "
NAMED_METADATA_LINE = "!objc3.objc_arc_semantic_rules = !{!77}"


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
        SnippetCheck("M262-B001-EXP-01", "# M262 ARC Semantic Rules And Forbidden Forms Contract And Architecture Freeze Expectations (B001)"),
        SnippetCheck("M262-B001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M262-B001-EXP-03", "conflicting `@property` ownership forms still failing deterministically"),
        SnippetCheck("M262-B001-EXP-04", "The contract must explicitly hand off to `M262-B002`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M262-B001-PKT-01", "# M262-B001 ARC Semantic Rules And Forbidden Forms Contract And Architecture Freeze Packet"),
        SnippetCheck("M262-B001-PKT-02", "Issue: `#7196`"),
        SnippetCheck("M262-B001-PKT-03", "Packet: `M262-B001`"),
        SnippetCheck("M262-B001-PKT-04", "`M262-B002` is the explicit next handoff after this freeze closes."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M262-B001-SRC-01", "## M262 ARC semantic rules and forbidden forms (M262-B001)"),
        SnippetCheck("M262-B001-SRC-02", "explicit ARC mode does not yet imply generalized ARC inference"),
        SnippetCheck("M262-B001-SRC-03", "`!objc3.objc_arc_semantic_rules`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M262-B001-NDOC-01", "## M262 ARC semantic rules and forbidden forms (M262-B001)"),
        SnippetCheck("M262-B001-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M262-B001-NDOC-03", "`M262-B002` is the next issue."),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M262-B001-SPC-01", "## M262 ARC semantic rules and forbidden forms (B001)"),
        SnippetCheck("M262-B001-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M262-B001-SPC-03", f"`{SEMANTIC_MODEL}`"),
        SnippetCheck("M262-B001-SPC-04", "`M262-B002` is the next issue."),
    ),
    ATTRIBUTE_SPEC: (
        SnippetCheck("M262-B001-ATTR-01", "### B.2.5 ARC semantic rules and forbidden forms (implementation note) {#b-2-5}"),
        SnippetCheck("M262-B001-ATTR-02", "explicit ARC mode does not yet imply generalized ARC inference"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M262-B001-ARCH-01", "## M262 ARC Semantic Rules And Forbidden Forms (B001)"),
        SnippetCheck("M262-B001-ARCH-02", "explicit ARC mode still leaves forbidden property forms fail-closed"),
        SnippetCheck("M262-B001-ARCH-03", "the next issue is `M262-B002`"),
    ),
    SEMA_CPP: (
        SnippetCheck("M262-B001-SEMA-01", "M262-B001 ARC semantic-rule freeze anchor"),
        SnippetCheck("M262-B001-SEMA-02", "property ownership conflicts, atomic ownership-aware storage, or"),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M262-B001-PM-01", "M262-B001 ARC semantic-rule freeze anchor"),
        SnippetCheck("M262-B001-PM-02", "broader inference remains"),
    ),
    SCAFFOLD_HEADER: (
        SnippetCheck("M262-B001-SCAF-01", "M262-B001 ARC semantic-rule freeze anchor"),
        SnippetCheck("M262-B001-SCAF-02", "legality/inference boundaries"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M262-B001-LHDR-01", "kObjc3ArcSemanticRulesContractId"),
        SnippetCheck("M262-B001-LHDR-02", "std::string Objc3ArcSemanticRulesSummary();"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M262-B001-LCPP-01", "std::string Objc3ArcSemanticRulesSummary()"),
        SnippetCheck("M262-B001-LCPP-02", ";semantic_model="),
        SnippetCheck("M262-B001-LCPP-03", ";next_issue=M262-B002"),
    ),
    IR_CPP: (
        SnippetCheck("M262-B001-IR-01", "M262-B001 ARC semantic-rule freeze anchor"),
        SnippetCheck("M262-B001-IR-02", "; arc_semantic_rules = "),
        SnippetCheck("M262-B001-IR-03", "objc_arc_semantic_rules"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M262-B001-PKG-01", '"check:objc3c:m262-b001-arc-semantic-rules-and-forbidden-forms-contract": "python scripts/check_m262_b001_arc_semantic_rules_and_forbidden_forms_contract_and_architecture_freeze.py"'),
        SnippetCheck("M262-B001-PKG-02", '"test:tooling:m262-b001-arc-semantic-rules-and-forbidden-forms-contract": "python -m pytest tests/tooling/test_check_m262_b001_arc_semantic_rules_and_forbidden_forms_contract_and_architecture_freeze.py -q"'),
        SnippetCheck("M262-B001-PKG-03", '"check:objc3c:m262-b001-lane-b-readiness": "python scripts/run_m262_b001_lane_b_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M262-B001-RUN-01", "scripts/build_objc3c_native_docs.py"),
        SnippetCheck("M262-B001-RUN-02", "ensure_objc3c_native_build.py"),
        SnippetCheck("M262-B001-RUN-03", "test_check_m262_b001_arc_semantic_rules_and_forbidden_forms_contract_and_architecture_freeze.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M262-B001-TEST-01", "def test_m262_b001_checker_emits_summary() -> None:"),
        SnippetCheck("M262-B001-TEST-02", CONTRACT_ID),
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


def run_dynamic_probes(failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    payload: dict[str, Any] = {}

    positive_out_dir = PROBE_ROOT / "arc-enabled-positive"
    weak_mismatch_out_dir = PROBE_ROOT / "weak-mismatch-negative"
    atomic_out_dir = PROBE_ROOT / "atomic-ownership-negative"

    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M262-B001-DYN-01", "native binary is missing", failures)
    checks_total += require(POSITIVE_FIXTURE.exists(), display_path(POSITIVE_FIXTURE), "M262-B001-DYN-02", "positive fixture is missing", failures)
    checks_total += require(WEAK_MISMATCH_FIXTURE.exists(), display_path(WEAK_MISMATCH_FIXTURE), "M262-B001-DYN-03", "weak mismatch fixture is missing", failures)
    checks_total += require(ATOMIC_FIXTURE.exists(), display_path(ATOMIC_FIXTURE), "M262-B001-DYN-04", "atomic ownership fixture is missing", failures)
    if failures:
        return checks_total, payload

    positive_completed = compile_fixture(POSITIVE_FIXTURE, positive_out_dir, ["-fobjc-arc"])
    positive_ir = positive_out_dir / "module.ll"
    positive_diag = positive_out_dir / "module.diagnostics.json"
    checks_total += require(positive_completed.returncode == 0, display_path(positive_out_dir), "M262-B001-DYN-05", f"positive ARC compile failed: {positive_completed.stdout}{positive_completed.stderr}", failures)
    checks_total += require(positive_ir.exists(), display_path(positive_ir), "M262-B001-DYN-06", "positive ARC IR is missing", failures)
    checks_total += require(positive_diag.exists(), display_path(positive_diag), "M262-B001-DYN-07", "positive ARC diagnostics are missing", failures)
    if positive_completed.returncode == 0 and positive_ir.exists() and positive_diag.exists():
        positive_diag_payload = load_json(positive_diag)
        positive_ir_text = positive_ir.read_text(encoding="utf-8")
        checks_total += require(positive_diag_payload.get("diagnostics") == [], display_path(positive_diag), "M262-B001-DYN-08", "positive ARC fixture must stay diagnostics-clean", failures)
        checks_total += require(BOUNDARY_PREFIX + f"contract={CONTRACT_ID}" in positive_ir_text, display_path(positive_ir), "M262-B001-DYN-09", "positive ARC IR is missing the arc-semantic-rules line", failures)
        checks_total += require(NAMED_METADATA_LINE in positive_ir_text, display_path(positive_ir), "M262-B001-DYN-10", "positive ARC IR is missing the arc-semantic-rules named metadata", failures)
        payload["positive_arc_case"] = {
            "out_dir": display_path(positive_out_dir),
            "boundary_present": BOUNDARY_PREFIX + f"contract={CONTRACT_ID}" in positive_ir_text,
            "named_metadata_present": NAMED_METADATA_LINE in positive_ir_text,
        }

    weak_completed = compile_fixture(WEAK_MISMATCH_FIXTURE, weak_mismatch_out_dir, ["-fobjc-arc"])
    weak_diag = weak_mismatch_out_dir / "module.diagnostics.json"
    checks_total += require(weak_completed.returncode != 0, display_path(weak_mismatch_out_dir), "M262-B001-DYN-11", "weak mismatch fixture must fail closed", failures)
    checks_total += require(weak_diag.exists(), display_path(weak_diag), "M262-B001-DYN-12", "weak mismatch diagnostics are missing", failures)
    if weak_diag.exists():
        weak_diag_payload = load_json(weak_diag)
        diagnostics = weak_diag_payload.get("diagnostics", [])
        weak_codes = [str(diag.get("code", "")) for diag in diagnostics if isinstance(diag, dict)]
        weak_messages = [str(diag.get("message", "")) for diag in diagnostics if isinstance(diag, dict)]
        checks_total += require("O3S206" in weak_codes, display_path(weak_diag), "M262-B001-DYN-13", f"expected O3S206 for weak mismatch, observed {weak_codes}", failures)
        checks_total += require(any("property ownership qualifier '__weak' conflicts with @property ownership modifier 'assign'" in message for message in weak_messages), display_path(weak_diag), "M262-B001-DYN-14", "expected deterministic weak/assign ownership mismatch diagnostic", failures)
        payload["weak_mismatch_case"] = {
            "out_dir": display_path(weak_mismatch_out_dir),
            "diagnostic_codes": weak_codes,
        }

    atomic_completed = compile_fixture(ATOMIC_FIXTURE, atomic_out_dir, ["-fobjc-arc"])
    atomic_diag = atomic_out_dir / "module.diagnostics.json"
    checks_total += require(atomic_completed.returncode != 0, display_path(atomic_out_dir), "M262-B001-DYN-15", "atomic ownership fixture must fail closed", failures)
    checks_total += require(atomic_diag.exists(), display_path(atomic_diag), "M262-B001-DYN-16", "atomic ownership diagnostics are missing", failures)
    if atomic_diag.exists():
        atomic_diag_payload = load_json(atomic_diag)
        diagnostics = atomic_diag_payload.get("diagnostics", [])
        atomic_codes = [str(diag.get("code", "")) for diag in diagnostics if isinstance(diag, dict)]
        atomic_messages = [str(diag.get("message", "")) for diag in diagnostics if isinstance(diag, dict)]
        checks_total += require("O3S206" in atomic_codes, display_path(atomic_diag), "M262-B001-DYN-17", f"expected O3S206 for atomic ownership failure, observed {atomic_codes}", failures)
        checks_total += require(any("atomic ownership-aware property 'value'" in message for message in atomic_messages), display_path(atomic_diag), "M262-B001-DYN-18", "expected deterministic atomic ownership diagnostic", failures)
        payload["atomic_ownership_case"] = {
            "out_dir": display_path(atomic_out_dir),
            "diagnostic_codes": atomic_codes,
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
    print(f"[ok] M262-B001 arc semantic rules validated ({checks_total} checks)")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

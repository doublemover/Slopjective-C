#!/usr/bin/env python3
"""Fail-closed checker for M257-E001 property/ivar execution gate."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m257-e001-property-ivar-execution-gate-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-executable-property-ivar-execution-gate/m257-e001-v1"
EVIDENCE_MODEL = "a002-b003-c003-d003-summary-chain"
EXECUTION_GATE_MODEL = (
    "runnable-property-ivar-evidence-consumes-source-sema-lowering-and-runtime-proofs"
)
FAILURE_MODEL = "fail-closed-on-property-ivar-execution-evidence-drift"
NEXT_CLOSEOUT_ISSUE = "M257-E002"

M257_A002_CONTRACT_ID = "objc3c-executable-property-ivar-source-model-completion/m257-a002-v1"
M257_B003_CONTRACT_ID = "objc3c-property-accessor-attribute-interactions/m257-b003-v1"
M257_C003_CONTRACT_ID = "objc3c-executable-synthesized-accessor-property-lowering/m257-c003-v1"
M257_D003_CONTRACT_ID = "objc3c-runtime-property-metadata-reflection/m257-d003-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m257_property_and_ivar_execution_gate_contract_and_architecture_freeze_e001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m257"
    / "m257_e001_property_and_ivar_execution_gate_contract_and_architecture_freeze_packet.md"
)
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
DEFAULT_SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m257_e001_lane_e_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_A002_SUMMARY = (
    ROOT / "tmp" / "reports" / "m257" / "M257-A002" / "property_ivar_source_model_completion_summary.json"
)
DEFAULT_B003_SUMMARY = (
    ROOT / "tmp" / "reports" / "m257" / "M257-B003" / "accessor_legality_attribute_interactions_summary.json"
)
DEFAULT_C003_SUMMARY = (
    ROOT / "tmp" / "reports" / "m257" / "M257-C003" / "synthesized_accessor_property_lowering_summary.json"
)
DEFAULT_D003_SUMMARY = (
    ROOT / "tmp" / "reports" / "m257" / "M257-D003" / "property_metadata_reflection_summary.json"
)
DEFAULT_SUMMARY_OUT = (
    ROOT / "tmp" / "reports" / "m257" / "M257-E001" / "property_ivar_execution_gate_summary.json"
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


EXPECTATIONS_SNIPPETS = (
    SnippetCheck(
        "M257-E001-DOC-EXP-01",
        "# M257 Property And Ivar Execution Gate Contract And Architecture Freeze Expectations (E001)",
    ),
    SnippetCheck("M257-E001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck(
        "M257-E001-DOC-EXP-03",
        "tmp/reports/m257/M257-D003/property_metadata_reflection_summary.json",
    ),
    SnippetCheck("M257-E001-DOC-EXP-04", "The gate must explicitly hand off to `M257-E002`."),
)
PACKET_SNIPPETS = (
    SnippetCheck(
        "M257-E001-DOC-PKT-01",
        "# M257-E001 Property And Ivar Execution Gate Contract And Architecture Freeze Packet",
    ),
    SnippetCheck("M257-E001-DOC-PKT-02", "Packet: `M257-E001`"),
    SnippetCheck("M257-E001-DOC-PKT-03", "- `M257-C003`"),
    SnippetCheck("M257-E001-DOC-PKT-04", "- `M257-D003`"),
    SnippetCheck("M257-E001-DOC-PKT-05", "`check:objc3c:m257-e001-lane-e-readiness`"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M257-E001-NDOC-01", "## Property and ivar execution gate (M257-E001)"),
    SnippetCheck("M257-E001-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M257-E001-NDOC-03", f"`{EVIDENCE_MODEL}`"),
    SnippetCheck(
        "M257-E001-NDOC-04",
        "tmp/reports/m257/M257-E001/property_ivar_execution_gate_summary.json",
    ),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M257-E001-SPC-01", "## M257 property/ivar execution gate (E001)"),
    SnippetCheck("M257-E001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M257-E001-SPC-03", f"`{FAILURE_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M257-E001-META-01", "## M257 property/ivar execution gate metadata anchors (E001)"),
    SnippetCheck("M257-E001-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck(
        "M257-E001-META-03",
        "tmp/reports/m257/M257-E001/property_ivar_execution_gate_summary.json",
    ),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M257-E001-ARCH-01", "## M257 property and ivar execution gate (E001)"),
    SnippetCheck(
        "M257-E001-ARCH-02",
        "`M257-E001` freezes the first lane-E proof gate for executable properties,",
    ),
    SnippetCheck(
        "M257-E001-ARCH-03",
        "`M257-E002` is the first issue allowed to broaden this freeze into a",
    ),
)
AST_SNIPPETS = (
    SnippetCheck("M257-E001-AST-01", "M257-E001 property-ivar-execution gate anchor"),
    SnippetCheck(
        "M257-E001-AST-02",
        "before broader runnable sample expansion is allowed.",
    ),
)
SEMA_SNIPPETS = (
    SnippetCheck("M257-E001-SEMA-01", "M257-E001 property-ivar-execution gate anchor"),
    SnippetCheck(
        "M257-E001-SEMA-02",
        "A002/B003/C003/D003 proof chain before runnable sample expansion",
    ),
)
IR_SNIPPETS = (
    SnippetCheck("M257-E001-IR-01", "M257-E001 property-ivar-execution gate anchor"),
    SnippetCheck(
        "M257-E001-IR-02",
        "property proof chain over this same emitted",
    ),
)
READINESS_RUNNER_SNIPPETS = (
    SnippetCheck("M257-E001-RUN-01", "check:objc3c:m257-a002-lane-a-readiness"),
    SnippetCheck(
        "M257-E001-RUN-02",
        "scripts/check_m257_b003_accessor_legality_and_ownership_or_atomicity_attribute_interactions_core_feature_expansion.py",
    ),
    SnippetCheck(
        "M257-E001-RUN-03",
        "scripts/check_m257_c003_synthesized_accessor_and_property_metadata_lowering_core_feature_implementation.py",
    ),
    SnippetCheck(
        "M257-E001-RUN-04",
        "scripts/check_m257_d003_property_metadata_registration_and_reflective_access_helpers_core_feature_expansion.py",
    ),
    SnippetCheck(
        "M257-E001-RUN-05",
        "tests/tooling/test_check_m257_e001_property_and_ivar_execution_gate_contract_and_architecture_freeze.py",
    ),
)
PACKAGE_SNIPPETS = (
    SnippetCheck(
        "M257-E001-PKG-01",
        '"check:objc3c:m257-e001-property-and-ivar-execution-gate": "python scripts/check_m257_e001_property_and_ivar_execution_gate_contract_and_architecture_freeze.py"',
    ),
    SnippetCheck(
        "M257-E001-PKG-02",
        '"test:tooling:m257-e001-property-and-ivar-execution-gate": "python -m pytest tests/tooling/test_check_m257_e001_property_and_ivar_execution_gate_contract_and_architecture_freeze.py -q"',
    ),
    SnippetCheck(
        "M257-E001-PKG-03",
        '"check:objc3c:m257-e001-lane-e-readiness": "python scripts/run_m257_e001_lane_e_readiness.py"',
    ),
)


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--ast-header", type=Path, default=DEFAULT_AST_HEADER)
    parser.add_argument("--sema-cpp", type=Path, default=DEFAULT_SEMA_CPP)
    parser.add_argument("--ir-cpp", type=Path, default=DEFAULT_IR_CPP)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--a002-summary", type=Path, default=DEFAULT_A002_SUMMARY)
    parser.add_argument("--b003-summary", type=Path, default=DEFAULT_B003_SUMMARY)
    parser.add_argument("--c003-summary", type=Path, default=DEFAULT_C003_SUMMARY)
    parser.add_argument("--d003-summary", type=Path, default=DEFAULT_D003_SUMMARY)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(
            Finding(display_path(path), "M257-E001-MISSING", f"required artifact is missing: {display_path(path)}")
        )
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def validate_a002(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    total = 0
    passed = 0
    total += 1
    passed += require(payload.get("ok") is True, artifact, "M257-E001-A002-01", "A002 summary must report ok=true", failures)
    total += 1
    passed += require(payload.get("contract_id") == M257_A002_CONTRACT_ID, artifact, "M257-E001-A002-02", "A002 contract id drifted", failures)
    total += 1
    passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M257-E001-A002-03", "A002 summary must report full check coverage", failures)
    total += 1
    passed += require(payload.get("layout_model") is not None, artifact, "M257-E001-A002-04", "A002 layout model must remain present", failures)
    total += 1
    passed += require(payload.get("attribute_model") is not None, artifact, "M257-E001-A002-05", "A002 attribute model must remain present", failures)
    distilled = {
        "ok": payload.get("ok"),
        "contract_id": payload.get("contract_id"),
        "layout_model": payload.get("layout_model"),
        "attribute_model": payload.get("attribute_model"),
    }
    return total, passed, distilled


def validate_b003(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    total = 0
    passed = 0
    total += 1
    passed += require(payload.get("contract_id") == M257_B003_CONTRACT_ID, artifact, "M257-E001-B003-01", "B003 contract id drifted", failures)
    total += 1
    passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M257-E001-B003-02", "B003 summary must report full check coverage", failures)
    total += 1
    passed += require(payload.get("findings") == [], artifact, "M257-E001-B003-03", "B003 findings must remain empty", failures)
    total += 1
    passed += require(payload.get("previous_contract_id") == "objc3c-property-default-ivar-binding-semantics/m257-b002-v1", artifact, "M257-E001-B003-04", "B003 previous contract handoff drifted", failures)
    distilled = {
        "contract_id": payload.get("contract_id"),
        "checks_passed": payload.get("checks_passed"),
        "checks_total": payload.get("checks_total"),
        "dynamic_probes_skipped": payload.get("dynamic_probes_skipped"),
    }
    return total, passed, distilled


def validate_c003(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    total = 0
    passed = 0
    total += 1
    passed += require(payload.get("ok") is True, artifact, "M257-E001-C003-01", "C003 summary must report ok=true", failures)
    total += 1
    passed += require(payload.get("contract_id") == M257_C003_CONTRACT_ID, artifact, "M257-E001-C003-02", "C003 contract id drifted", failures)
    total += 1
    passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M257-E001-C003-03", "C003 summary must report full check coverage", failures)
    total += 1
    passed += require(isinstance(payload.get("dynamic_case"), dict), artifact, "M257-E001-C003-04", "C003 dynamic case payload drifted", failures)
    distilled = {
        "ok": payload.get("ok"),
        "contract_id": payload.get("contract_id"),
        "dynamic_case": payload.get("dynamic_case"),
    }
    return total, passed, distilled


def validate_d003(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    total = 0
    passed = 0
    total += 1
    passed += require(payload.get("ok") is True, artifact, "M257-E001-D003-01", "D003 summary must report ok=true", failures)
    total += 1
    passed += require(payload.get("contract_id") == M257_D003_CONTRACT_ID, artifact, "M257-E001-D003-02", "D003 contract id drifted", failures)
    total += 1
    passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M257-E001-D003-03", "D003 summary must report full check coverage", failures)
    total += 1
    passed += require(payload.get("registration_model") is not None, artifact, "M257-E001-D003-04", "D003 registration model must remain present", failures)
    total += 1
    passed += require(payload.get("query_model") is not None, artifact, "M257-E001-D003-05", "D003 query model must remain present", failures)
    distilled = {
        "ok": payload.get("ok"),
        "contract_id": payload.get("contract_id"),
        "registration_model": payload.get("registration_model"),
        "query_model": payload.get("query_model"),
    }
    return total, passed, distilled


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.ast_header, AST_SNIPPETS),
        (args.sema_cpp, SEMA_SNIPPETS),
        (args.ir_cpp, IR_SNIPPETS),
        (args.readiness_runner, READINESS_RUNNER_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    a002_total, a002_passed, a002_distilled = validate_a002(args.a002_summary, failures)
    b003_total, b003_passed, b003_distilled = validate_b003(args.b003_summary, failures)
    c003_total, c003_passed, c003_distilled = validate_c003(args.c003_summary, failures)
    d003_total, d003_passed, d003_distilled = validate_d003(args.d003_summary, failures)
    checks_total += a002_total + b003_total + c003_total + d003_total
    checks_passed += a002_passed + b003_passed + c003_passed + d003_passed

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "evidence_model": EVIDENCE_MODEL,
        "execution_gate_model": EXECUTION_GATE_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_closeout_issue": NEXT_CLOSEOUT_ISSUE,
        "upstream_summaries": {
            "M257-A002": a002_distilled,
            "M257-B003": b003_distilled,
            "M257-C003": c003_distilled,
            "M257-D003": d003_distilled,
        },
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    json.dump(payload, sys.stdout, indent=2)
    sys.stdout.write("\n")
    return 0 if payload["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

#!/usr/bin/env python3
"""Fail-closed checker for M256-E001 class/protocol/category conformance gate."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m256-e001-class-protocol-category-conformance-gate-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-executable-class-protocol-category-conformance-gate/m256-e001-v1"
EVIDENCE_MODEL = "a003-b004-c003-d004-summary-chain"
EXECUTION_BOUNDARY_MODEL = (
    "runnable-class-protocol-category-evidence-consumes-source-sema-lowering-and-runtime-proofs"
)
FAILURE_MODEL = "fail-closed-on-class-protocol-category-conformance-evidence-drift"
NEXT_CLOSEOUT_ISSUE = "M256-E002"

M256_A003_CONTRACT_ID = "objc3c-executable-protocol-category-source-closure/m256-a003-v1"
M256_B004_CONTRACT_ID = "objc3c-inheritance-override-realization-legality/m256-b004-v1"
M256_C003_CONTRACT_ID = "objc3c-executable-realization-records/m256-c003-v1"
M256_D004_CONTRACT_ID = "objc3c-runtime-canonical-runnable-object-sample-support/m256-d004-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m256_class_protocol_and_category_conformance_gate_contract_and_architecture_freeze_e001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m256"
    / "m256_e001_class_protocol_and_category_conformance_gate_contract_and_architecture_freeze_packet.md"
)
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m256_e001_lane_e_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_A003_SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m256"
    / "M256-A003"
    / "protocol_category_source_surface_completion_for_executable_runtime_summary.json"
)
DEFAULT_B004_SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m256"
    / "M256-B004"
    / "inheritance_override_realization_legality_summary.json"
)
DEFAULT_C003_SUMMARY = ROOT / "tmp" / "reports" / "m256" / "M256-C003" / "realization_records_summary.json"
DEFAULT_D004_SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m256"
    / "M256-D004"
    / "canonical_runnable_object_sample_support_summary.json"
)
DEFAULT_SUMMARY_OUT = (
    ROOT
    / "tmp"
    / "reports"
    / "m256"
    / "M256-E001"
    / "class_protocol_category_conformance_gate_summary.json"
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
        "M256-E001-DOC-EXP-01",
        "# M256 Class, Protocol, And Category Conformance Gate Contract And Architecture Freeze Expectations (E001)",
    ),
    SnippetCheck("M256-E001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck(
        "M256-E001-DOC-EXP-03",
        "tmp/reports/m256/M256-D004/canonical_runnable_object_sample_support_summary.json",
    ),
    SnippetCheck("M256-E001-DOC-EXP-04", "The gate must explicitly hand off to `M256-E002`."),
)
PACKET_SNIPPETS = (
    SnippetCheck(
        "M256-E001-DOC-PKT-01",
        "# M256-E001 Class, Protocol, And Category Conformance Gate Contract And Architecture Freeze Packet",
    ),
    SnippetCheck("M256-E001-DOC-PKT-02", "Packet: `M256-E001`"),
    SnippetCheck("M256-E001-DOC-PKT-03", f"`{EVIDENCE_MODEL}`"),
    SnippetCheck("M256-E001-DOC-PKT-04", "`M256-E002` is the explicit next handoff"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M256-E001-NDOC-01", "## Class, protocol, and category conformance gate (M256-E001)"),
    SnippetCheck("M256-E001-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M256-E001-NDOC-03", f"`{EVIDENCE_MODEL}`"),
    SnippetCheck(
        "M256-E001-NDOC-04",
        "tmp/reports/m256/M256-E001/class_protocol_category_conformance_gate_summary.json",
    ),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M256-E001-SPC-01", "## M256 class/protocol/category conformance gate (E001)"),
    SnippetCheck("M256-E001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M256-E001-SPC-03", f"`{FAILURE_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M256-E001-META-01", "## M256 class/protocol/category conformance gate metadata anchors (E001)"),
    SnippetCheck("M256-E001-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck(
        "M256-E001-META-03",
        "tmp/reports/m256/M256-E001/class_protocol_category_conformance_gate_summary.json",
    ),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M256-E001-ARCH-01", "## M256 class/protocol/category conformance gate (E001)"),
    SnippetCheck(
        "M256-E001-ARCH-02",
        "`M256-E001` freezes the first lane-E proof gate for executable classes,",
    ),
    SnippetCheck(
        "M256-E001-ARCH-03",
        "`M256-E002` is the first issue allowed to broaden this freeze into a",
    ),
)
PARSER_SNIPPETS = (
    SnippetCheck("M256-E001-PARSE-01", "M256-E001 class-protocol-category conformance gate anchor"),
    SnippetCheck("M256-E001-PARSE-02", "the A003/B004/C003/D004 evidence chain and therefore still depends on this"),
)
SEMA_SNIPPETS = (
    SnippetCheck("M256-E001-SEMA-01", "M256-E001 class-protocol-category conformance gate anchor"),
    SnippetCheck("M256-E001-SEMA-02", "conformance, and category-merge decisions before execution-matrix"),
)
IR_SNIPPETS = (
    SnippetCheck("M256-E001-IR-01", "M256-E001 class-protocol-category conformance gate anchor"),
    SnippetCheck(
        "M256-E001-IR-02",
        "A003/B004/C003/D004 remain the canonical proof surface that lane-E",
    ),
)
READINESS_RUNNER_SNIPPETS = (
    SnippetCheck("M256-E001-RUN-01", "check:objc3c:m256-a003-lane-a-readiness"),
    SnippetCheck("M256-E001-RUN-02", "scripts/check_m256_b004_inheritance_override_and_realization_legality_core_feature_expansion.py"),
    SnippetCheck("M256-E001-RUN-03", "scripts/check_m256_c003_realization_records_for_class_protocol_and_category_artifacts_core_feature_expansion.py"),
    SnippetCheck("M256-E001-RUN-04", "scripts/check_m256_d004_canonical_runnable_class_and_object_sample_support_core_feature_expansion.py"),
    SnippetCheck(
        "M256-E001-RUN-05",
        "tests/tooling/test_check_m256_e001_class_protocol_and_category_conformance_gate_contract_and_architecture_freeze.py",
    ),
)
PACKAGE_SNIPPETS = (
    SnippetCheck(
        "M256-E001-PKG-01",
        '"check:objc3c:m256-e001-class-protocol-and-category-conformance-gate": "python scripts/check_m256_e001_class_protocol_and_category_conformance_gate_contract_and_architecture_freeze.py"',
    ),
    SnippetCheck(
        "M256-E001-PKG-02",
        '"test:tooling:m256-e001-class-protocol-and-category-conformance-gate": "python -m pytest tests/tooling/test_check_m256_e001_class_protocol_and_category_conformance_gate_contract_and_architecture_freeze.py -q"',
    ),
    SnippetCheck(
        "M256-E001-PKG-03",
        '"check:objc3c:m256-e001-lane-e-readiness": "python scripts/run_m256_e001_lane_e_readiness.py"',
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
    parser.add_argument("--parser-cpp", type=Path, default=DEFAULT_PARSER_CPP)
    parser.add_argument("--sema-cpp", type=Path, default=DEFAULT_SEMA_CPP)
    parser.add_argument("--ir-cpp", type=Path, default=DEFAULT_IR_CPP)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--a003-summary", type=Path, default=DEFAULT_A003_SUMMARY)
    parser.add_argument("--b004-summary", type=Path, default=DEFAULT_B004_SUMMARY)
    parser.add_argument("--c003-summary", type=Path, default=DEFAULT_C003_SUMMARY)
    parser.add_argument("--d004-summary", type=Path, default=DEFAULT_D004_SUMMARY)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


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
        failures.append(Finding(display_path(path), "M256-E001-MISSING", f"required artifact is missing: {display_path(path)}"))
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


def validate_a003(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M256-E001-A003-01", "A003 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("contract_id") == M256_A003_CONTRACT_ID, artifact, "M256-E001-A003-02", "A003 contract id drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M256-E001-A003-03", "A003 summary must report full check coverage", failures)

    dynamic_probe = payload.get("dynamic_probe", {})
    checks_total += 1
    checks_passed += require(isinstance(dynamic_probe, dict) and bool(dynamic_probe), artifact, "M256-E001-A003-04", "A003 must preserve the dynamic probe record", failures)
    checks_total += 1
    checks_passed += require(str(dynamic_probe.get("fixture", "")).endswith("m251_runtime_metadata_source_records_category_protocol_property.objc3"), artifact, "M256-E001-A003-05", "A003 fixture drifted", failures)
    checks_total += 1
    checks_passed += require(str(dynamic_probe.get("manifest_path", "")).endswith("module.manifest.json"), artifact, "M256-E001-A003-06", "A003 manifest path drifted", failures)
    checks_total += 1
    checks_passed += require(str(dynamic_probe.get("ir_path", "")).endswith("module.ll"), artifact, "M256-E001-A003-07", "A003 IR path drifted", failures)

    distilled = {
        "contract_id": payload.get("contract_id"),
        "fixture": dynamic_probe.get("fixture"),
        "manifest_path": dynamic_probe.get("manifest_path"),
        "ir_path": dynamic_probe.get("ir_path"),
    }
    return checks_total, checks_passed, distilled


def validate_b004(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M256-E001-B004-01", "B004 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("contract_id") == M256_B004_CONTRACT_ID, artifact, "M256-E001-B004-02", "B004 contract id drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M256-E001-B004-03", "B004 summary must report full check coverage", failures)
    checks_total += 1
    checks_passed += require(payload.get("previous_contract_id") == "objc3c-category-merge-conflict-semantics/m256-b003-v1", artifact, "M256-E001-B004-04", "B004 previous contract handoff drifted", failures)
    for key, check_id in (
        ("positive_case", "M256-E001-B004-05"),
        ("missing_superclass_case", "M256-E001-B004-06"),
        ("cycle_case", "M256-E001-B004-07"),
        ("bad_signature_case", "M256-E001-B004-08"),
        ("bad_kind_case", "M256-E001-B004-09"),
        ("unrealized_superclass_case", "M256-E001-B004-10"),
        ("bad_property_case", "M256-E001-B004-11"),
    ):
        checks_total += 1
        checks_passed += require(key in payload, artifact, check_id, f"B004 must preserve {key}", failures)

    distilled = {
        "contract_id": payload.get("contract_id"),
        "previous_contract_id": payload.get("previous_contract_id"),
        "dynamic_probes_executed": payload.get("dynamic_probes_executed"),
    }
    return checks_total, checks_passed, distilled


def validate_c003(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M256-E001-C003-01", "C003 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("contract_id") == M256_C003_CONTRACT_ID, artifact, "M256-E001-C003-02", "C003 contract id drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M256-E001-C003-03", "C003 summary must report full check coverage", failures)
    checks_total += 1
    checks_passed += require(isinstance(payload.get("dynamic_cases"), list), artifact, "M256-E001-C003-04", "C003 dynamic case list drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("failures") == [], artifact, "M256-E001-C003-05", "C003 failures must remain empty", failures)

    distilled = {
        "contract_id": payload.get("contract_id"),
        "dynamic_probes_executed": payload.get("dynamic_probes_executed"),
        "dynamic_case_count": len(payload.get("dynamic_cases", [])),
    }
    return checks_total, checks_passed, distilled


def validate_d004(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M256-E001-D004-01", "D004 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("contract_id") == M256_D004_CONTRACT_ID, artifact, "M256-E001-D004-02", "D004 contract id drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M256-E001-D004-03", "D004 summary must report full check coverage", failures)

    dynamic = payload.get("dynamic", {})
    checks_total += 1
    checks_passed += require(isinstance(dynamic, dict), artifact, "M256-E001-D004-04", "D004 dynamic payload must remain an object", failures)
    checks_total += 1
    checks_passed += require(dynamic.get("skipped") in {True, False, None}, artifact, "M256-E001-D004-05", "D004 skipped marker drifted", failures)

    distilled = {
        "contract_id": payload.get("contract_id"),
        "dynamic_probes_executed": payload.get("dynamic_probes_executed"),
        "dynamic_skipped": dynamic.get("skipped"),
    }
    return checks_total, checks_passed, distilled


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
        (args.parser_cpp, PARSER_SNIPPETS),
        (args.sema_cpp, SEMA_SNIPPETS),
        (args.ir_cpp, IR_SNIPPETS),
        (args.readiness_runner, READINESS_RUNNER_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        checks_passed += ensure_snippets(path, snippets, failures)
        checks_total += len(snippets)

    upstream_evidence: dict[str, Any] = {}
    for key, path, validator in (
        ("m256_a003", args.a003_summary, validate_a003),
        ("m256_b004", args.b004_summary, validate_b004),
        ("m256_c003", args.c003_summary, validate_c003),
        ("m256_d004", args.d004_summary, validate_d004),
    ):
        validator_total, validator_passed, distilled = validator(path, failures)
        checks_total += validator_total
        checks_passed += validator_passed
        upstream_evidence[key] = distilled

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "evidence_model": EVIDENCE_MODEL,
        "execution_boundary_model": EXECUTION_BOUNDARY_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_closeout_issue": NEXT_CLOSEOUT_ISSUE,
        "upstream_evidence": upstream_evidence,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.check_id} {finding.artifact}: {finding.detail}")
        print(f"[summary] {MODE} failed ({checks_passed}/{checks_total} checks passed)")
        print(f"[summary] wrote {display_path(args.summary_out)}")
        return 1

    print(f"[ok] {MODE} ({checks_passed}/{checks_total} checks passed)")
    print(f"[summary] wrote {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

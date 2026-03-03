#!/usr/bin/env python3
"""Fail-closed checker for M243-A011 diagnostic grammar hooks performance/quality guardrails."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = (
    "m243-a011-diagnostic-grammar-hooks-and-source-precision-"
    "performance-quality-guardrails-contract-v1"
)

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_diagnostic_grammar_hooks_and_source_precision_performance_quality_guardrails_a011_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_a011_diagnostic_grammar_hooks_and_source_precision_performance_quality_guardrails_packet.md",
    "a010_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_diagnostic_grammar_hooks_and_source_precision_a010_expectations.md",
    "a010_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_a010_diagnostic_grammar_hooks_and_source_precision_conformance_corpus_expansion_packet.md",
    "a010_checker": ROOT
    / "scripts"
    / "check_m243_a010_diagnostic_grammar_hooks_and_source_precision_conformance_corpus_expansion_contract.py",
    "a010_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_a010_diagnostic_grammar_hooks_and_source_precision_conformance_corpus_expansion_contract.py",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M243-A011-DOC-01",
            "# M243 Diagnostic Grammar Hooks and Source Precision Performance Quality Guardrails Expectations (A011)",
        ),
        (
            "M243-A011-DOC-02",
            "Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-performance-quality-guardrails/m243-a011-v1`",
        ),
        ("M243-A011-DOC-03", "Dependencies: `M243-A010`"),
        (
            "M243-A011-DOC-04",
            "spec/planning/compiler/m243/m243_a011_diagnostic_grammar_hooks_and_source_precision_performance_quality_guardrails_packet.md",
        ),
        (
            "M243-A011-DOC-05",
            "scripts/check_m243_a011_diagnostic_grammar_hooks_and_source_precision_performance_quality_guardrails_contract.py",
        ),
        (
            "M243-A011-DOC-06",
            "tests/tooling/test_check_m243_a011_diagnostic_grammar_hooks_and_source_precision_performance_quality_guardrails_contract.py",
        ),
        (
            "M243-A011-DOC-07",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        (
            "M243-A011-DOC-08",
            "check:objc3c:m243-a011-lane-a-readiness",
        ),
        (
            "M243-A011-DOC-09",
            "tmp/reports/m243/M243-A011/diagnostic_grammar_hooks_and_source_precision_performance_quality_guardrails_contract_summary.json",
        ),
    ),
    "packet_doc": (
        (
            "M243-A011-PKT-01",
            "# M243-A011 Diagnostic Grammar Hooks and Source Precision Performance Quality Guardrails Packet",
        ),
        ("M243-A011-PKT-02", "Packet: `M243-A011`"),
        ("M243-A011-PKT-03", "Dependencies: `M243-A010`"),
        (
            "M243-A011-PKT-04",
            "docs/contracts/m243_diagnostic_grammar_hooks_and_source_precision_performance_quality_guardrails_a011_expectations.md",
        ),
        (
            "M243-A011-PKT-05",
            "scripts/check_m243_a011_diagnostic_grammar_hooks_and_source_precision_performance_quality_guardrails_contract.py",
        ),
        (
            "M243-A011-PKT-06",
            "tests/tooling/test_check_m243_a011_diagnostic_grammar_hooks_and_source_precision_performance_quality_guardrails_contract.py",
        ),
        ("M243-A011-PKT-07", "`check:objc3c:m243-a011-lane-a-readiness`"),
        (
            "M243-A011-PKT-08",
            "python scripts/check_m243_a011_diagnostic_grammar_hooks_and_source_precision_performance_quality_guardrails_contract.py --emit-json",
        ),
    ),
    "a010_expectations_doc": (
        (
            "M243-A011-A010-DOC-01",
            "Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-conformance-corpus-expansion/m243-a010-v1`",
        ),
    ),
    "a010_packet_doc": (
        ("M243-A011-A010-PKT-01", "Packet: `M243-A010`"),
        ("M243-A011-A010-PKT-02", "Dependencies: `M243-A009`"),
    ),
    "a010_checker": (
        (
            "M243-A011-A010-CHK-01",
            "m243-a010-diagnostic-grammar-hooks-and-source-precision-",
        ),
        (
            "M243-A011-A010-CHK-02",
            '"conformance-corpus-expansion-contract-v1"',
        ),
    ),
    "a010_test": (
        (
            "M243-A011-A010-TST-01",
            "check_m243_a010_diagnostic_grammar_hooks_and_source_precision_conformance_corpus_expansion_contract.py",
        ),
    ),
    "architecture_doc": (
        (
            "M243-A011-ARCH-01",
            "M243 lane-A A011 performance and quality guardrails anchors parser diagnostic grammar-hook readiness chaining",
        ),
    ),
    "lowering_spec": (
        (
            "M243-A011-SPC-01",
            "diagnostic grammar hooks and source precision performance and quality guardrails shall preserve lane-A dependency anchors (`M243-A010`)",
        ),
    ),
    "metadata_spec": (
        (
            "M243-A011-META-01",
            "deterministic lane-A diagnostic grammar hooks/source precision performance and quality guardrails metadata anchors for `M243-A011` with explicit `M243-A010` dependency continuity",
        ),
    ),
    "package_json": (
        (
            "M243-A011-PKG-01",
            '"check:objc3c:m243-a011-diagnostic-grammar-hooks-and-source-precision-performance-quality-guardrails-contract": ',
        ),
        (
            "M243-A011-PKG-02",
            '"test:tooling:m243-a011-diagnostic-grammar-hooks-and-source-precision-performance-quality-guardrails-contract": ',
        ),
        (
            "M243-A011-PKG-03",
            '"check:objc3c:m243-a011-lane-a-readiness": "npm run check:objc3c:m243-a010-lane-a-readiness',
        ),
        (
            "M243-A011-PKG-04",
            "npm run check:objc3c:m243-a011-diagnostic-grammar-hooks-and-source-precision-performance-quality-guardrails-contract && npm run test:tooling:m243-a011-diagnostic-grammar-hooks-and-source-precision-performance-quality-guardrails-contract",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "package_json": (
        (
            "M243-A011-FORB-01",
            '"check:objc3c:m243-a011-lane-a-readiness": "npm run check:objc3c:m243-a009-lane-a-readiness',
        ),
    ),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path(
            "tmp/reports/m243/M243-A011/"
            "diagnostic_grammar_hooks_and_source_precision_performance_quality_guardrails_contract_summary.json"
        ),
    )
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit the summary JSON to stdout in addition to writing --summary-out.",
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        text = load_text(path, artifact=artifact)
        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                passed_checks += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))
        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                findings.append(Finding(artifact, check_id, f"forbidden snippet present: {snippet}"))
            else:
                passed_checks += 1

    ok = not findings
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": total_checks,
        "checks_passed": passed_checks,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in findings
        ],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if args.emit_json:
        print(canonical_json(summary), end="")

    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))


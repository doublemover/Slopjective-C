#!/usr/bin/env python3
"""Fail-closed checker for M243-A012 diagnostic grammar hooks integration closeout/sign-off."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = (
    "m243-a012-diagnostic-grammar-hooks-and-source-precision-"
    "integration-closeout-and-gate-signoff-contract-v1"
)

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_diagnostic_grammar_hooks_and_source_precision_integration_closeout_and_gate_signoff_a012_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_a012_diagnostic_grammar_hooks_and_source_precision_integration_closeout_and_gate_signoff_packet.md",
    "a011_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_diagnostic_grammar_hooks_and_source_precision_performance_quality_guardrails_a011_expectations.md",
    "a011_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_a011_diagnostic_grammar_hooks_and_source_precision_performance_quality_guardrails_packet.md",
    "a011_checker": ROOT
    / "scripts"
    / "check_m243_a011_diagnostic_grammar_hooks_and_source_precision_performance_quality_guardrails_contract.py",
    "a011_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_a011_diagnostic_grammar_hooks_and_source_precision_performance_quality_guardrails_contract.py",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M243-A012-DOC-01",
            "# M243 Diagnostic Grammar Hooks and Source Precision Integration Closeout and Gate Sign-off Expectations (A012)",
        ),
        (
            "M243-A012-DOC-02",
            "Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-integration-closeout-and-gate-signoff/m243-a012-v1`",
        ),
        ("M243-A012-DOC-03", "Dependencies: `M243-A011`"),
        (
            "M243-A012-DOC-04",
            "spec/planning/compiler/m243/m243_a012_diagnostic_grammar_hooks_and_source_precision_integration_closeout_and_gate_signoff_packet.md",
        ),
        (
            "M243-A012-DOC-05",
            "scripts/check_m243_a012_diagnostic_grammar_hooks_and_source_precision_integration_closeout_and_gate_signoff_contract.py",
        ),
        (
            "M243-A012-DOC-06",
            "tests/tooling/test_check_m243_a012_diagnostic_grammar_hooks_and_source_precision_integration_closeout_and_gate_signoff_contract.py",
        ),
        (
            "M243-A012-DOC-07",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        (
            "M243-A012-DOC-08",
            "check:objc3c:m243-a012-lane-a-readiness",
        ),
        (
            "M243-A012-DOC-09",
            "tmp/reports/m243/M243-A012/diagnostic_grammar_hooks_and_source_precision_integration_closeout_and_gate_signoff_contract_summary.json",
        ),
    ),
    "packet_doc": (
        (
            "M243-A012-PKT-01",
            "# M243-A012 Diagnostic Grammar Hooks and Source Precision Integration Closeout and Gate Sign-off Packet",
        ),
        ("M243-A012-PKT-02", "Packet: `M243-A012`"),
        ("M243-A012-PKT-03", "Dependencies: `M243-A011`"),
        (
            "M243-A012-PKT-04",
            "docs/contracts/m243_diagnostic_grammar_hooks_and_source_precision_integration_closeout_and_gate_signoff_a012_expectations.md",
        ),
        (
            "M243-A012-PKT-05",
            "scripts/check_m243_a012_diagnostic_grammar_hooks_and_source_precision_integration_closeout_and_gate_signoff_contract.py",
        ),
        (
            "M243-A012-PKT-06",
            "tests/tooling/test_check_m243_a012_diagnostic_grammar_hooks_and_source_precision_integration_closeout_and_gate_signoff_contract.py",
        ),
        ("M243-A012-PKT-07", "`check:objc3c:m243-a012-lane-a-readiness`"),
        (
            "M243-A012-PKT-08",
            "python scripts/check_m243_a012_diagnostic_grammar_hooks_and_source_precision_integration_closeout_and_gate_signoff_contract.py --emit-json",
        ),
    ),
    "a011_expectations_doc": (
        (
            "M243-A012-A011-DOC-01",
            "Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-performance-quality-guardrails/m243-a011-v1`",
        ),
    ),
    "a011_packet_doc": (
        ("M243-A012-A011-PKT-01", "Packet: `M243-A011`"),
        ("M243-A012-A011-PKT-02", "Dependencies: `M243-A010`"),
    ),
    "a011_checker": (
        (
            "M243-A012-A011-CHK-01",
            "m243-a011-diagnostic-grammar-hooks-and-source-precision-",
        ),
        (
            "M243-A012-A011-CHK-02",
            '"performance-quality-guardrails-contract-v1"',
        ),
    ),
    "a011_test": (
        (
            "M243-A012-A011-TST-01",
            "check_m243_a011_diagnostic_grammar_hooks_and_source_precision_performance_quality_guardrails_contract.py",
        ),
    ),
    "architecture_doc": (
        (
            "M243-A012-ARCH-01",
            "M243 lane-A A012 integration closeout and gate sign-off anchors parser diagnostic grammar-hook readiness chain sign-off governance",
        ),
    ),
    "lowering_spec": (
        (
            "M243-A012-SPC-01",
            "diagnostic grammar hooks and source precision integration closeout and gate sign-off shall preserve lane-A dependency anchors (`M243-A011`)",
        ),
    ),
    "metadata_spec": (
        (
            "M243-A012-META-01",
            "deterministic lane-A diagnostic grammar hooks/source precision integration closeout and gate sign-off metadata anchors for `M243-A012` with explicit `M243-A011` dependency continuity",
        ),
    ),
    "package_json": (
        (
            "M243-A012-PKG-01",
            '"check:objc3c:m243-a012-diagnostic-grammar-hooks-and-source-precision-integration-closeout-and-gate-signoff-contract": ',
        ),
        (
            "M243-A012-PKG-02",
            '"test:tooling:m243-a012-diagnostic-grammar-hooks-and-source-precision-integration-closeout-and-gate-signoff-contract": ',
        ),
        (
            "M243-A012-PKG-03",
            '"check:objc3c:m243-a012-lane-a-readiness": "npm run check:objc3c:m243-a011-lane-a-readiness',
        ),
        (
            "M243-A012-PKG-04",
            "npm run check:objc3c:m243-a012-diagnostic-grammar-hooks-and-source-precision-integration-closeout-and-gate-signoff-contract && npm run test:tooling:m243-a012-diagnostic-grammar-hooks-and-source-precision-integration-closeout-and-gate-signoff-contract",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "package_json": (
        (
            "M243-A012-FORB-01",
            '"check:objc3c:m243-a012-lane-a-readiness": "npm run check:objc3c:m243-a010-lane-a-readiness',
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
            "tmp/reports/m243/M243-A012/"
            "diagnostic_grammar_hooks_and_source_precision_integration_closeout_and_gate_signoff_contract_summary.json"
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
        raise FileNotFoundError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        total_checks += 1
        try:
            text = load_text(path, artifact=artifact)
            passed_checks += 1
        except FileNotFoundError as exc:
            findings.append(
                Finding(
                    artifact,
                    f"M243-A012-MISSING-{artifact.upper()}",
                    str(exc),
                )
            )
            continue

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

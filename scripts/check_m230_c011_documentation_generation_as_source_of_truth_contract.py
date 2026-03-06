#!/usr/bin/env python3
"""Fail-closed checker for M230-C011 Documentation generation as source-of-truth contract freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m230-c011-documentation-generation-as-source-of-truth-performance-and-quality-guardrails-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m230_documentation_generation_as_source_of_truth_performance_and_quality_guardrails_c011_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m230"
    / "m230_c011_documentation_generation_as_source_of_truth_performance_and_quality_guardrails_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m230/M230-C011/documentation_generation_as_source_of_truth_performance_and_quality_guardrails_summary.json"
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


EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M230-C011-DOC-EXP-01",
        "# M230 Documentation generation as source-of-truth Contract and Architecture Freeze Expectations (C010)",
    ),
    SnippetCheck(
        "M230-C011-DOC-EXP-02",
        "Contract ID: `objc3c-documentation-generation-as-source-of-truth/m230-c011-v1`",
    ),
    SnippetCheck("M230-C011-DOC-EXP-03", "Issue: `#5432`"),
    SnippetCheck("M230-C011-DOC-EXP-04", "Dependencies: `M230-C010`"),
    SnippetCheck(
        "M230-C011-DOC-EXP-05",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M230-C011-DOC-EXP-06",
        "`check:objc3c:m230-c011-lane-c-readiness`",
    ),
    SnippetCheck(
        "M230-C011-DOC-EXP-07",
        "`tmp/reports/m230/M230-C011/documentation_generation_as_source_of_truth_performance_and_quality_guardrails_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M230-C011-DOC-PKT-01",
        "# M230-C011 Documentation generation as source-of-truth Contract and Architecture Freeze Packet",
    ),
    SnippetCheck("M230-C011-DOC-PKT-02", "Packet: `M230-C011`"),
    SnippetCheck("M230-C011-DOC-PKT-03", "Issue: `#5432`"),
    SnippetCheck("M230-C011-DOC-PKT-04", "Dependencies: `M230-C010`"),
    SnippetCheck(
        "M230-C011-DOC-PKT-05",
        "check:objc3c:m230-c011-documentation-generation-as-source-of-truth-performance-and-quality-guardrails-contract",
    ),
    SnippetCheck(
        "M230-C011-DOC-PKT-06",
        "tmp/reports/m230/M230-C011/documentation_generation_as_source_of_truth_performance_and_quality_guardrails_summary.json",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M230-C011-ARCH-01",
        "M230 lane-C C010 Documentation generation as source-of-truth contract-freeze anchors",
    ),
    SnippetCheck(
        "M230-C011-ARCH-02",
        "m230_documentation_generation_as_source_of_truth_performance_and_quality_guardrails_c011_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M230-C011-SPC-01",
        "Documentation generation as source-of-truth contract-freeze governance shall preserve explicit",
    ),
    SnippetCheck(
        "M230-C011-SPC-02",
        "lane-C deterministic boundary anchors (`M230-C011`) and fail closed on contract-freeze evidence drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M230-C011-META-01",
        "deterministic lane-C Documentation generation as source-of-truth contract-freeze anchors for `M230-C011`",
    ),
    SnippetCheck(
        "M230-C011-META-02",
        "explicit lane-C contract-freeze metadata continuity so Documentation generation as source-of-truth drift fails closed",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M230-C011-PKG-01",
        '"check:objc3c:m230-c011-documentation-generation-as-source-of-truth-performance-and-quality-guardrails-contract": '
        '"python scripts/check_m230_c011_documentation_generation_as_source_of_truth_contract.py"',
    ),
    SnippetCheck(
        "M230-C011-PKG-02",
        '"test:tooling:m230-c011-documentation-generation-as-source-of-truth-performance-and-quality-guardrails-contract": '
        '"python -m pytest tests/tooling/test_check_m230_c011_documentation_generation_as_source_of_truth_contract.py -q"',
    ),
    SnippetCheck(
        "M230-C011-PKG-03",
        '"check:objc3c:m230-c011-lane-c-readiness": '
        '"npm run check:objc3c:m230-c010-lane-c-readiness && npm run check:objc3c:m230-c011-documentation-generation-as-source-of-truth-performance-and-quality-guardrails-contract '
        '&& npm run test:tooling:m230-c011-documentation-generation-as-source-of-truth-performance-and-quality-guardrails-contract"',
    ),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(list(argv))


def ensure_file(path: Path, artifact: str, check_id: str, findings: list[Finding]) -> str:
    if not path.is_file():
        findings.append(Finding(artifact=artifact, check_id=check_id, detail=f"missing file: {display_path(path)}"))
        return ""
    return path.read_text(encoding="utf-8")


def ensure_snippets(
    *,
    artifact: str,
    content: str,
    checks: Sequence[SnippetCheck],
    findings: list[Finding],
) -> int:
    passed = 0
    for check in checks:
        if check.snippet in content:
            passed += 1
        else:
            findings.append(Finding(artifact=artifact, check_id=check.check_id, detail=f"missing snippet: {check.snippet}"))
    return passed


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    artifact_inputs = (
        ("expectations_doc", "M230-C011-EXPECTATIONS", args.expectations_doc, EXPECTATIONS_SNIPPETS),
        ("packet_doc", "M230-C011-PACKET", args.packet_doc, PACKET_SNIPPETS),
        ("architecture_doc", "M230-C011-ARCHITECTURE", args.architecture_doc, ARCHITECTURE_SNIPPETS),
        ("lowering_spec", "M230-C011-LOWERING-SPEC", args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        ("metadata_spec", "M230-C011-METADATA-SPEC", args.metadata_spec, METADATA_SPEC_SNIPPETS),
        ("package_json", "M230-C011-PACKAGE", args.package_json, PACKAGE_SNIPPETS),
    )

    for _, artifact, path, snippets in artifact_inputs:
        checks_total += len(snippets) + 1
        content = ensure_file(path, artifact, f"{artifact}-FILE", findings)
        if content:
            checks_passed += 1
        checks_passed += ensure_snippets(
            artifact=artifact,
            content=content,
            checks=snippets,
            findings=findings,
        )

    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in findings
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if findings:
        print(
            f"[fail] {MODE}: {checks_passed}/{checks_total} checks passed; "
            f"{len(findings)} failure(s). Summary: {display_path(args.summary_out)}",
            file=sys.stderr,
        )
        for finding in findings:
            print(f" - ({finding.check_id}) [{finding.artifact}] {finding.detail}", file=sys.stderr)
        return 1

    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


def main() -> int:
    return run(sys.argv[1:])


if __name__ == "__main__":
    raise SystemExit(main())






















#!/usr/bin/env python3
"""Fail-closed checker for M249-E011 release gate/docs/runbooks performance and quality guardrails."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m249-e011-lane-e-release-gate-docs-runbooks-performance-and-quality-guardrails-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m249_lane_e_release_gate_docs_and_runbooks_performance_and_quality_guardrails_e011_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m249"
    / "m249_e011_lane_e_release_gate_docs_and_runbooks_performance_and_quality_guardrails_packet.md"
)
DEFAULT_E010_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m249_lane_e_release_gate_docs_and_runbooks_conformance_corpus_expansion_e010_expectations.md"
)
DEFAULT_E010_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m249"
    / "m249_e010_lane_e_release_gate_docs_and_runbooks_conformance_corpus_expansion_packet.md"
)
DEFAULT_E010_CHECKER = (
    ROOT / "scripts" / "check_m249_e010_lane_e_release_gate_docs_and_runbooks_conformance_corpus_expansion_contract.py"
)
DEFAULT_E010_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m249_e010_lane_e_release_gate_docs_and_runbooks_conformance_corpus_expansion_contract.py"
)
DEFAULT_E010_RUNNER_SCRIPT = ROOT / "scripts" / "run_m249_e010_lane_e_readiness.py"
DEFAULT_RUNNER_SCRIPT = ROOT / "scripts" / "run_m249_e011_lane_e_readiness.py"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m249/M249-E011/lane_e_release_gate_docs_runbooks_performance_and_quality_guardrails_summary.json"
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
        "M249-E011-DOC-EXP-01",
        "# M249 Lane E Release Gate, Docs, and Runbooks Performance and Quality Guardrails Expectations (E011)",
    ),
    SnippetCheck(
        "M249-E011-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-release-gate-docs-runbooks-performance-and-quality-guardrails/m249-e011-v1`",
    ),
    SnippetCheck("M249-E011-DOC-EXP-03", "- Issue: `#6958`"),
    SnippetCheck("M249-E011-DOC-EXP-04", "`M249-E010`"),
    SnippetCheck("M249-E011-DOC-EXP-05", "`M249-A004`"),
    SnippetCheck("M249-E011-DOC-EXP-06", "`M249-B005`"),
    SnippetCheck(
        "M249-E011-DOC-EXP-07",
        "Dependency token `M249-C006` is mandatory for lane-C edge-case expansion and robustness readiness chaining.",
    ),
    SnippetCheck("M249-E011-DOC-EXP-08", "`M249-D009`"),
    SnippetCheck(
        "M249-E011-DOC-EXP-09",
        "scripts/check_m249_e010_lane_e_release_gate_docs_and_runbooks_conformance_corpus_expansion_contract.py",
    ),
    SnippetCheck(
        "M249-E011-DOC-EXP-10",
        "`scripts/run_m249_e011_lane_e_readiness.py`",
    ),
    SnippetCheck(
        "M249-E011-DOC-EXP-11",
        "`scripts/check_m249_e011_lane_e_release_gate_docs_and_runbooks_performance_and_quality_guardrails_contract.py`",
    ),
    SnippetCheck(
        "M249-E011-DOC-EXP-12",
        "`tests/tooling/test_check_m249_e011_lane_e_release_gate_docs_and_runbooks_performance_and_quality_guardrails_contract.py`",
    ),
    SnippetCheck("M249-E011-DOC-EXP-13", "`compile:objc3c`"),
    SnippetCheck("M249-E011-DOC-EXP-14", "`proof:objc3c`"),
    SnippetCheck("M249-E011-DOC-EXP-15", "`test:objc3c:execution-replay-proof`"),
    SnippetCheck("M249-E011-DOC-EXP-16", "`test:objc3c:perf-budget`"),
    SnippetCheck(
        "M249-E011-DOC-EXP-17",
        "`tmp/reports/m249/M249-E011/lane_e_release_gate_docs_runbooks_performance_and_quality_guardrails_summary.json`",
    ),
)

E010_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E011-E010-DOC-01",
        "Contract ID: `objc3c-lane-e-release-gate-docs-runbooks-conformance-corpus-expansion/m249-e010-v1`",
    ),
    SnippetCheck(
        "M249-E011-E010-DOC-02",
        "Dependency token `M249-B010` is mandatory for lane-B conformance corpus expansion readiness chaining.",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E011-DOC-PKT-01",
        "# M249-E011 Lane-E Release Gate, Docs, and Runbooks Performance and Quality Guardrails Packet",
    ),
    SnippetCheck("M249-E011-DOC-PKT-02", "Packet: `M249-E011`"),
    SnippetCheck("M249-E011-DOC-PKT-03", "Issue: `#6958`"),
    SnippetCheck(
        "M249-E011-DOC-PKT-04",
        "Dependencies: `M249-E010`, `M249-A004`, `M249-B005`, `M249-C006`, `M249-D009`",
    ),
    SnippetCheck(
        "M249-E011-DOC-PKT-05",
        "docs/contracts/m249_lane_e_release_gate_docs_and_runbooks_performance_and_quality_guardrails_e011_expectations.md",
    ),
    SnippetCheck(
        "M249-E011-DOC-PKT-06",
        "scripts/check_m249_e011_lane_e_release_gate_docs_and_runbooks_performance_and_quality_guardrails_contract.py",
    ),
    SnippetCheck(
        "M249-E011-DOC-PKT-07",
        "tests/tooling/test_check_m249_e011_lane_e_release_gate_docs_and_runbooks_performance_and_quality_guardrails_contract.py",
    ),
    SnippetCheck(
        "M249-E011-DOC-PKT-08",
        "scripts/run_m249_e011_lane_e_readiness.py",
    ),
    SnippetCheck(
        "M249-E011-DOC-PKT-09",
        "scripts/check_m249_e010_lane_e_release_gate_docs_and_runbooks_conformance_corpus_expansion_contract.py",
    ),
    SnippetCheck(
        "M249-E011-DOC-PKT-10",
        "check:objc3c:m249-e010-lane-e-readiness",
    ),
    SnippetCheck(
        "M249-E011-DOC-PKT-11",
        "milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M249-E011-DOC-PKT-12",
        "tmp/reports/m249/M249-E011/lane_e_release_gate_docs_runbooks_performance_and_quality_guardrails_summary.json",
    ),
)

E010_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M249-E011-E010-PKT-01", "Packet: `M249-E010`"),
    SnippetCheck(
        "M249-E011-E010-PKT-02",
        "Dependencies: `M249-E009`, `M249-A009`, `M249-B010`, `M249-C010`, `M249-D010`",
    ),
)

RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E011-RUN-01",
        '"""Run M249-E011 lane-E readiness checks without deep npm nesting."""',
    ),
    SnippetCheck("M249-E011-RUN-02", "check:objc3c:m249-e010-lane-e-readiness"),
    SnippetCheck("M249-E011-RUN-03", "check:objc3c:m249-a004-lane-a-readiness"),
    SnippetCheck("M249-E011-RUN-04", "check:objc3c:m249-b005-lane-b-readiness"),
    SnippetCheck("M249-E011-RUN-05", "check:objc3c:m249-c006-lane-c-readiness"),
    SnippetCheck("M249-E011-RUN-06", "check:objc3c:m249-d009-lane-d-readiness"),
    SnippetCheck(
        "M249-E011-RUN-07",
        "scripts/check_m249_e011_lane_e_release_gate_docs_and_runbooks_performance_and_quality_guardrails_contract.py",
    ),
    SnippetCheck(
        "M249-E011-RUN-08",
        "tests/tooling/test_check_m249_e011_lane_e_release_gate_docs_and_runbooks_performance_and_quality_guardrails_contract.py",
    ),
    SnippetCheck("M249-E011-RUN-09", "[ok] M249-E011 lane-E readiness chain completed"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E011-ARCH-01",
        "M249 lane-E E003 release gate/docs/runbooks core feature implementation",
    ),
    SnippetCheck(
        "M249-E011-ARCH-02",
        "`M249-E002`, `M249-A003`, `M249-B003`,",
    ),
    SnippetCheck(
        "M249-E011-ARCH-03",
        "`M249-C003`, and `M249-D003`",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E011-SPC-01",
        "release gate/docs/runbooks core feature implementation wiring shall preserve",
    ),
    SnippetCheck(
        "M249-E011-SPC-02",
        "`M249-E002`, `M249-A003`, `M249-B003`,",
    ),
    SnippetCheck(
        "M249-E011-SPC-03",
        "`M249-C003`, and `M249-D003`",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E011-META-01",
        "deterministic lane-E release gate/docs/runbooks core feature implementation dependency anchors for",
    ),
    SnippetCheck(
        "M249-E011-META-02",
        "`M249-E002`, `M249-A003`, `M249-B003`, `M249-C003`, and `M249-D003`",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E011-PKG-01",
        '"check:objc3c:m249-e011-lane-e-release-gate-docs-runbooks-performance-and-quality-guardrails-contract": '
        '"python scripts/check_m249_e011_lane_e_release_gate_docs_and_runbooks_performance_and_quality_guardrails_contract.py"',
    ),
    SnippetCheck(
        "M249-E011-PKG-02",
        '"test:tooling:m249-e011-lane-e-release-gate-docs-runbooks-performance-and-quality-guardrails-contract": '
        '"python -m pytest tests/tooling/test_check_m249_e011_lane_e_release_gate_docs_and_runbooks_performance_and_quality_guardrails_contract.py -q"',
    ),
    SnippetCheck(
        "M249-E011-PKG-03",
        '"check:objc3c:m249-e011-lane-e-readiness": "python scripts/run_m249_e011_lane_e_readiness.py"',
    ),
    SnippetCheck("M249-E011-PKG-04", '"check:objc3c:m249-a004-lane-a-readiness": '),
    SnippetCheck("M249-E011-PKG-05", '"check:objc3c:m249-b005-lane-b-readiness": '),
    SnippetCheck("M249-E011-PKG-06", '"check:objc3c:m249-c006-lane-c-readiness": '),
    SnippetCheck("M249-E011-PKG-07", '"check:objc3c:m249-d009-lane-d-readiness": '),
    SnippetCheck("M249-E011-PKG-08", '"check:objc3c:m249-e010-lane-e-readiness": '),
    SnippetCheck("M249-E011-PKG-09", '"compile:objc3c": '),
    SnippetCheck("M249-E011-PKG-10", '"proof:objc3c": '),
    SnippetCheck("M249-E011-PKG-11", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M249-E011-PKG-12", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--e010-expectations-doc", type=Path, default=DEFAULT_E010_EXPECTATIONS_DOC)
    parser.add_argument("--e010-packet-doc", type=Path, default=DEFAULT_E010_PACKET_DOC)
    parser.add_argument("--e010-checker", type=Path, default=DEFAULT_E010_CHECKER)
    parser.add_argument("--e010-test", type=Path, default=DEFAULT_E010_TEST)
    parser.add_argument("--e010-runner-script", type=Path, default=DEFAULT_E010_RUNNER_SCRIPT)
    parser.add_argument("--runner-script", type=Path, default=DEFAULT_RUNNER_SCRIPT)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_doc_contract(
    *,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(
            Finding(
                display_path(path),
                exists_check_id,
                f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings
    if not path.is_file():
        findings.append(
            Finding(
                display_path(path),
                exists_check_id,
                f"required path is not a file: {display_path(path)}",
            )
        )
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(
                Finding(
                    display_path(path),
                    snippet.check_id,
                    f"missing required snippet: {snippet.snippet}",
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M249-E011-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M249-E011-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.e010_expectations_doc, "M249-E011-E010-DOC-EXISTS", E010_EXPECTATIONS_SNIPPETS),
        (args.e010_packet_doc, "M249-E011-E010-PKT-EXISTS", E010_PACKET_SNIPPETS),
        (args.runner_script, "M249-E011-RUN-EXISTS", RUNNER_SNIPPETS),
        (args.architecture_doc, "M249-E011-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M249-E011-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M249-E011-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M249-E011-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.e010_checker, "M249-E011-DEP-E010-ARG-01"),
        (args.e010_test, "M249-E011-DEP-E010-ARG-02"),
        (args.e010_runner_script, "M249-E011-DEP-E010-ARG-03"),
    ):
        checks_total += 1
        if not path.exists():
            failures.append(
                Finding(
                    display_path(path),
                    check_id,
                    f"required dependency path is missing: {display_path(path)}",
                )
            )
            continue
        if not path.is_file():
            failures.append(
                Finding(
                    display_path(path),
                    check_id,
                    f"required dependency path is not a file: {display_path(path)}",
                )
            )

    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail}
            for f in failures
        ],
    }

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

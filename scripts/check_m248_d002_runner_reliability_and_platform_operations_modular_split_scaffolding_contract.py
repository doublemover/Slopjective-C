#!/usr/bin/env python3
"""Fail-closed contract checker for the M248-D002 modular split/scaffolding packet."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-d002-runner-reliability-platform-operations-modular-split-scaffolding-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_runner_reliability_and_platform_operations_modular_split_scaffolding_d002_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_d002_runner_reliability_and_platform_operations_modular_split_scaffolding_packet.md"
)
DEFAULT_D001_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_runner_reliability_and_platform_operations_contract_freeze_d001_expectations.md"
)
DEFAULT_D001_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_d001_runner_reliability_and_platform_operations_contract_freeze_packet.md"
)
DEFAULT_D001_CHECKER_SCRIPT = (
    ROOT / "scripts" / "check_m248_d001_runner_reliability_and_platform_operations_contract.py"
)
DEFAULT_D001_TOOLING_TEST = (
    ROOT / "tests" / "tooling" / "test_check_m248_d001_runner_reliability_and_platform_operations_contract.py"
)
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m248/M248-D002/runner_reliability_and_platform_operations_modular_split_scaffolding_contract_summary.json"
)

ARTIFACT_ORDER: tuple[str, ...] = (
    "expectations_doc",
    "packet_doc",
    "d001_expectations_doc",
    "d001_packet_doc",
    "d001_checker_script",
    "d001_tooling_test",
)
ARTIFACT_RANK = {artifact: index for index, artifact in enumerate(ARTIFACT_ORDER)}


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
        "M248-D002-DOC-EXP-01",
        "# M248 Runner Reliability and Platform Operations Modular Split/Scaffolding Expectations (D002)",
    ),
    SnippetCheck(
        "M248-D002-DOC-EXP-02",
        "Contract ID: `objc3c-runner-reliability-platform-operations-modular-split-scaffolding/m248-d002-v1`",
    ),
    SnippetCheck("M248-D002-DOC-EXP-03", "Dependencies: `M248-D001`"),
    SnippetCheck(
        "M248-D002-DOC-EXP-04",
        "including code/spec anchors and milestone optimization improvements",
    ),
    SnippetCheck(
        "M248-D002-DOC-EXP-05",
        "spec/planning/compiler/m248/m248_d001_runner_reliability_and_platform_operations_contract_freeze_packet.md",
    ),
    SnippetCheck("M248-D002-DOC-EXP-06", "- `compile:objc3c`"),
    SnippetCheck("M248-D002-DOC-EXP-07", "- `test:objc3c:perf-budget`"),
    SnippetCheck(
        "M248-D002-DOC-EXP-08",
        "`tmp/reports/m248/M248-D002/runner_reliability_and_platform_operations_modular_split_scaffolding_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-D002-DOC-PKT-01",
        "# M248-D002 Runner Reliability and Platform Operations Modular Split/Scaffolding Packet",
    ),
    SnippetCheck("M248-D002-DOC-PKT-02", "Packet: `M248-D002`"),
    SnippetCheck("M248-D002-DOC-PKT-03", "Dependencies: `M248-D001`"),
    SnippetCheck(
        "M248-D002-DOC-PKT-04",
        "including code/spec anchors and milestone optimization",
    ),
    SnippetCheck(
        "M248-D002-DOC-PKT-05",
        "scripts/check_m248_d002_runner_reliability_and_platform_operations_modular_split_scaffolding_contract.py",
    ),
    SnippetCheck(
        "M248-D002-DOC-PKT-06",
        "spec/planning/compiler/m248/m248_d001_runner_reliability_and_platform_operations_contract_freeze_packet.md",
    ),
    SnippetCheck("M248-D002-DOC-PKT-07", "- `compile:objc3c`"),
    SnippetCheck("M248-D002-DOC-PKT-08", "- `test:objc3c:perf-budget`"),
)

D001_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-D002-D001-DOC-01",
        "Contract ID: `objc3c-runner-reliability-platform-operations-contract/m248-d001-v1`",
    ),
    SnippetCheck(
        "M248-D002-D001-DOC-02",
        "optimization improvements as mandatory scope inputs.",
    ),
)

D001_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-D002-D001-PKT-01", "Packet: `M248-D001`"),
    SnippetCheck("M248-D002-D001-PKT-02", "Dependencies: none"),
    SnippetCheck(
        "M248-D002-D001-PKT-03",
        "including code/spec anchors and milestone",
    ),
)

D001_CHECKER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-D002-D001-CHK-01",
        'MODE = "m248-d001-runner-reliability-platform-operations-contract-v1"',
    ),
    SnippetCheck(
        "M248-D002-D001-CHK-02",
        "tmp/reports/m248/M248-D001/runner_reliability_and_platform_operations_contract_summary.json",
    ),
)

D001_TOOLING_TEST_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-D002-D001-TST-01",
        "check_m248_d001_runner_reliability_and_platform_operations_contract",
    ),
    SnippetCheck("M248-D002-D001-TST-02", '"M248-D001-DOC-EXP-03"'),
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
    parser.add_argument("--d001-expectations-doc", type=Path, default=DEFAULT_D001_EXPECTATIONS_DOC)
    parser.add_argument("--d001-packet-doc", type=Path, default=DEFAULT_D001_PACKET_DOC)
    parser.add_argument("--d001-checker-script", type=Path, default=DEFAULT_D001_CHECKER_SCRIPT)
    parser.add_argument("--d001-tooling-test", type=Path, default=DEFAULT_D001_TOOLING_TEST)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_artifact(
    *,
    artifact: str,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    artifact_path = display_path(path)

    if not path.exists():
        findings.append(Finding(artifact, exists_check_id, f"required artifact missing: {artifact_path}"))
        return checks_total, findings
    if not path.is_file():
        findings.append(Finding(artifact, exists_check_id, f"required artifact is not a file: {artifact_path}"))
        return checks_total, findings

    try:
        text = path.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        findings.append(Finding(artifact, exists_check_id, f"artifact is not UTF-8 text: {artifact_path}"))
        return checks_total, findings
    except OSError as exc:
        findings.append(Finding(artifact, exists_check_id, f"unable to read artifact {artifact_path}: {exc}"))
        return checks_total, findings

    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(
                Finding(
                    artifact,
                    snippet.check_id,
                    f"missing required snippet in {artifact_path}: {snippet.snippet}",
                )
            )

    return checks_total, findings


def finding_sort_key(finding: Finding) -> tuple[int, str, str]:
    if finding.artifact not in ARTIFACT_RANK:
        raise ValueError(f"unknown artifact key in finding: {finding.artifact}")
    return (ARTIFACT_RANK[finding.artifact], finding.check_id, finding.detail)


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    checks_total = 0
    failures: list[Finding] = []

    for artifact, path, exists_check_id, snippets in (
        ("expectations_doc", args.expectations_doc, "M248-D002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        ("packet_doc", args.packet_doc, "M248-D002-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (
            "d001_expectations_doc",
            args.d001_expectations_doc,
            "M248-D002-D001-DOC-EXISTS",
            D001_EXPECTATIONS_SNIPPETS,
        ),
        ("d001_packet_doc", args.d001_packet_doc, "M248-D002-D001-PKT-EXISTS", D001_PACKET_SNIPPETS),
        ("d001_checker_script", args.d001_checker_script, "M248-D002-D001-CHK-EXISTS", D001_CHECKER_SNIPPETS),
        ("d001_tooling_test", args.d001_tooling_test, "M248-D002-D001-TST-EXISTS", D001_TOOLING_TEST_SNIPPETS),
    ):
        count, findings = check_artifact(
            artifact=artifact,
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    failures = sorted(failures, key=finding_sort_key)
    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in failures
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

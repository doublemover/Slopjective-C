#!/usr/bin/env python3
"""Fail-closed checker for M249-A005 feature packaging edge-case and compatibility completion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m249-a005-feature-packaging-surface-compatibility-edge-case-and-compatibility-completion-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m249_feature_packaging_surface_and_compatibility_contracts_edge_case_and_compatibility_completion_a005_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m249"
    / "m249_a005_feature_packaging_surface_and_compatibility_contracts_edge_case_and_compatibility_completion_packet.md"
)
DEFAULT_A004_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m249_feature_packaging_surface_and_compatibility_contracts_core_feature_expansion_a004_expectations.md"
)
DEFAULT_A004_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m249"
    / "m249_a004_feature_packaging_surface_and_compatibility_contracts_core_feature_expansion_packet.md"
)
DEFAULT_A004_CHECKER = (
    ROOT / "scripts" / "check_m249_a004_feature_packaging_surface_and_compatibility_contracts_core_feature_expansion_contract.py"
)
DEFAULT_A004_TEST = (
    ROOT / "tests" / "tooling" / "test_check_m249_a004_feature_packaging_surface_and_compatibility_contracts_core_feature_expansion_contract.py"
)
DEFAULT_RUNNER_SCRIPT = ROOT / "scripts" / "run_m249_a005_lane_a_readiness.py"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m249/M249-A005/"
    "feature_packaging_surface_and_compatibility_contracts_edge_case_and_compatibility_completion_summary.json"
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
        "M249-A005-DOC-EXP-01",
        "# M249 Feature Packaging Surface and Compatibility Contracts Edge-Case and Compatibility Completion Expectations (A005)",
    ),
    SnippetCheck(
        "M249-A005-DOC-EXP-02",
        "Contract ID: `objc3c-feature-packaging-surface-compatibility-contracts-edge-case-and-compatibility-completion/m249-a005-v1`",
    ),
    SnippetCheck("M249-A005-DOC-EXP-03", "Dependencies: `M249-A004`"),
    SnippetCheck(
        "M249-A005-DOC-EXP-04",
        "edge-case and compatibility completion dependency anchors remain explicit, deterministic, and traceable",
    ),
    SnippetCheck(
        "M249-A005-DOC-EXP-05",
        "docs/contracts/m249_feature_packaging_surface_and_compatibility_contracts_core_feature_expansion_a004_expectations.md",
    ),
    SnippetCheck(
        "M249-A005-DOC-EXP-06",
        "scripts/check_m249_a004_feature_packaging_surface_and_compatibility_contracts_core_feature_expansion_contract.py",
    ),
    SnippetCheck(
        "M249-A005-DOC-EXP-07",
        "`check:objc3c:m249-a005-feature-packaging-surface-compatibility-edge-case-and-compatibility-completion-contract`",
    ),
    SnippetCheck(
        "M249-A005-DOC-EXP-08",
        "`python scripts/run_m249_a005_lane_a_readiness.py`",
    ),
    SnippetCheck(
        "M249-A005-DOC-EXP-09",
        "`tmp/reports/m249/M249-A005/feature_packaging_surface_and_compatibility_contracts_edge_case_and_compatibility_completion_summary.json`",
    ),
    SnippetCheck(
        "M249-A005-DOC-EXP-10",
        "`check:objc3c:m249-a004-lane-a-readiness`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-A005-DOC-PKT-01",
        "# M249-A005 Feature Packaging Surface and Compatibility Contracts Edge-Case and Compatibility Completion Packet",
    ),
    SnippetCheck("M249-A005-DOC-PKT-02", "Packet: `M249-A005`"),
    SnippetCheck("M249-A005-DOC-PKT-03", "Dependencies: `M249-A004`"),
    SnippetCheck(
        "M249-A005-DOC-PKT-04",
        "edge-case and compatibility completion prerequisites",
    ),
    SnippetCheck(
        "M249-A005-DOC-PKT-05",
        "scripts/check_m249_a005_feature_packaging_surface_and_compatibility_contracts_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck(
        "M249-A005-DOC-PKT-06",
        "tests/tooling/test_check_m249_a005_feature_packaging_surface_and_compatibility_contracts_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck(
        "M249-A005-DOC-PKT-07",
        "scripts/run_m249_a005_lane_a_readiness.py",
    ),
    SnippetCheck("M249-A005-DOC-PKT-08", "`npm run check:objc3c:m249-a005-lane-a-readiness`"),
    SnippetCheck("M249-A005-DOC-PKT-09", "`check:objc3c:m249-a004-lane-a-readiness`"),
)

A004_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-A005-A004-DOC-01",
        "# M249 Feature Packaging Surface and Compatibility Contracts Core Feature Expansion Expectations (A004)",
    ),
    SnippetCheck(
        "M249-A005-A004-DOC-02",
        "Contract ID: `objc3c-feature-packaging-surface-compatibility-contracts-core-feature-expansion/m249-a004-v1`",
    ),
    SnippetCheck("M249-A005-A004-DOC-03", "Dependencies: `M249-A003`"),
)

A004_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M249-A005-A004-PKT-01", "Packet: `M249-A004`"),
    SnippetCheck("M249-A005-A004-PKT-02", "Dependencies: `M249-A003`"),
    SnippetCheck("M249-A005-A004-PKT-03", "scripts/run_m249_a004_lane_a_readiness.py"),
)

RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-A005-RUN-01",
        "check:objc3c:m249-a004-lane-a-readiness",
    ),
    SnippetCheck(
        "M249-A005-RUN-02",
        "scripts/check_m249_a005_feature_packaging_surface_and_compatibility_contracts_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck(
        "M249-A005-RUN-03",
        "tests/tooling/test_check_m249_a005_feature_packaging_surface_and_compatibility_contracts_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck(
        "M249-A005-RUN-04",
        "M249-A005 lane-A readiness chain completed",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-A005-ARCH-01",
        "M249 lane-A A003 feature packaging core feature implementation anchors",
    ),
    SnippetCheck(
        "M249-A005-ARCH-02",
        "docs/contracts/m249_feature_packaging_surface_and_compatibility_contracts_core_feature_implementation_a003_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-A005-SPC-01",
        "feature packaging core feature implementation governance shall preserve explicit",
    ),
    SnippetCheck(
        "M249-A005-SPC-02",
        "lane-A dependency anchors (`M249-A002`) and fail closed on core-feature evidence drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-A005-META-01",
        "deterministic lane-A feature packaging core feature metadata anchors for `M249-A003`",
    ),
    SnippetCheck(
        "M249-A005-META-02",
        "explicit `M249-A002` dependency continuity so core feature implementation drift fails closed",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-A005-PKG-01",
        '"check:objc3c:m249-a005-feature-packaging-surface-compatibility-edge-case-and-compatibility-completion-contract": '
        '"python scripts/check_m249_a005_feature_packaging_surface_and_compatibility_contracts_edge_case_and_compatibility_completion_contract.py"',
    ),
    SnippetCheck(
        "M249-A005-PKG-02",
        '"test:tooling:m249-a005-feature-packaging-surface-compatibility-edge-case-and-compatibility-completion-contract": '
        '"python -m pytest tests/tooling/test_check_m249_a005_feature_packaging_surface_and_compatibility_contracts_edge_case_and_compatibility_completion_contract.py -q"',
    ),
    SnippetCheck(
        "M249-A005-PKG-03",
        '"check:objc3c:m249-a005-lane-a-readiness": "python scripts/run_m249_a005_lane_a_readiness.py"',
    ),
    SnippetCheck("M249-A005-PKG-04", '"test:objc3c:parser-replay-proof": '),
    SnippetCheck("M249-A005-PKG-05", '"test:objc3c:parser-ast-extraction": '),
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
    parser.add_argument("--a004-expectations-doc", type=Path, default=DEFAULT_A004_EXPECTATIONS_DOC)
    parser.add_argument("--a004-packet-doc", type=Path, default=DEFAULT_A004_PACKET_DOC)
    parser.add_argument("--a004-checker", type=Path, default=DEFAULT_A004_CHECKER)
    parser.add_argument("--a004-test", type=Path, default=DEFAULT_A004_TEST)
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
        (args.expectations_doc, "M249-A005-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M249-A005-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.a004_expectations_doc, "M249-A005-A004-DOC-EXISTS", A004_EXPECTATIONS_SNIPPETS),
        (args.a004_packet_doc, "M249-A005-A004-PKT-EXISTS", A004_PACKET_SNIPPETS),
        (args.runner_script, "M249-A005-RUN-EXISTS", RUNNER_SNIPPETS),
        (args.architecture_doc, "M249-A005-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M249-A005-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M249-A005-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M249-A005-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.a004_checker, "M249-A005-DEP-A004-ARG-01"),
        (args.a004_test, "M249-A005-DEP-A004-ARG-02"),
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

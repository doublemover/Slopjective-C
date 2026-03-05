#!/usr/bin/env python3
"""Fail-closed checker for M234-D006 runtime property metadata integration edge-case expansion robustness."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m234-d006-runtime-property-metadata-integration-edge-case-expansion-and-robustness-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m234_runtime_property_metadata_integration_edge_case_expansion_and_robustness_d006_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m234"
    / "m234_d006_runtime_property_metadata_integration_edge_case_expansion_and_robustness_packet.md"
)
DEFAULT_D005_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m234_runtime_property_metadata_integration_edge_case_and_compatibility_completion_d005_expectations.md"
)
DEFAULT_D005_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m234"
    / "m234_d005_runtime_property_metadata_integration_edge_case_and_compatibility_completion_packet.md"
)
DEFAULT_D005_CHECKER = (
    ROOT
    / "scripts"
    / "check_m234_d005_runtime_property_metadata_integration_edge_case_and_compatibility_completion_contract.py"
)
DEFAULT_D005_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m234_d005_runtime_property_metadata_integration_edge_case_and_compatibility_completion_contract.py"
)
DEFAULT_D005_READINESS_RUNNER = ROOT / "scripts" / "run_m234_d005_lane_d_readiness.py"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m234_d006_lane_d_readiness.py"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m234/M234-D006/runtime_property_metadata_integration_edge_case_expansion_and_robustness_contract_summary.json"
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
        "M234-D006-DOC-EXP-01",
        "# M234 Runtime Property Metadata Integration Edge-Case Expansion and Robustness Expectations (D006)",
    ),
    SnippetCheck(
        "M234-D006-DOC-EXP-02",
        "Contract ID: `objc3c-runtime-property-metadata-integration-edge-case-expansion-and-robustness/m234-d006-v1`",
    ),
    SnippetCheck("M234-D006-DOC-EXP-03", "Dependencies: `M234-D005`"),
    SnippetCheck(
        "M234-D006-DOC-EXP-04",
        "scripts/check_m234_d005_runtime_property_metadata_integration_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck(
        "M234-D006-DOC-EXP-05",
        "scripts/run_m234_d006_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M234-D006-DOC-EXP-06",
        "code/spec anchors and milestone optimization",
    ),
    SnippetCheck(
        "M234-D006-DOC-EXP-07",
        "improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M234-D006-DOC-EXP-08",
        "`python scripts/run_m234_d005_lane_d_readiness.py`",
    ),
    SnippetCheck(
        "M234-D006-DOC-EXP-09",
        "`python scripts/run_m234_d006_lane_d_readiness.py`",
    ),
    SnippetCheck(
        "M234-D006-DOC-EXP-10",
        "`tmp/reports/m234/M234-D006/runtime_property_metadata_integration_edge_case_expansion_and_robustness_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-D006-DOC-PKT-01",
        "# M234-D006 Runtime Property Metadata Integration Edge-Case Expansion and Robustness Packet",
    ),
    SnippetCheck("M234-D006-DOC-PKT-02", "Packet: `M234-D006`"),
    SnippetCheck("M234-D006-DOC-PKT-03", "Dependencies: `M234-D005`"),
    SnippetCheck(
        "M234-D006-DOC-PKT-04",
        "scripts/check_m234_d006_runtime_property_metadata_integration_edge_case_expansion_and_robustness_contract.py",
    ),
    SnippetCheck(
        "M234-D006-DOC-PKT-05",
        "scripts/run_m234_d006_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M234-D006-DOC-PKT-06",
        "python scripts/run_m234_d005_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M234-D006-DOC-PKT-07",
        "python scripts/run_m234_d006_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M234-D006-DOC-PKT-08",
        "code/spec anchors and milestone",
    ),
    SnippetCheck(
        "M234-D006-DOC-PKT-09",
        "optimization improvements as mandatory scope inputs.",
    ),
)

D005_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-D006-D005-DOC-01",
        "Contract ID: `objc3c-runtime-property-metadata-integration-edge-case-and-compatibility-completion/m234-d005-v1`",
    ),
    SnippetCheck(
        "M234-D006-D005-DOC-02",
        "Dependencies: `M234-D004`",
    ),
)

D005_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M234-D006-D005-PKT-01", "Packet: `M234-D005`"),
    SnippetCheck("M234-D006-D005-PKT-02", "Dependencies: `M234-D004`"),
)

READINESS_RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M234-D006-RUN-01", '"""Run M234-D006 lane-D readiness checks without deep npm nesting."""'),
    SnippetCheck("M234-D006-RUN-02", "scripts/run_m234_d005_lane_d_readiness.py"),
    SnippetCheck(
        "M234-D006-RUN-03",
        "scripts/check_m234_d006_runtime_property_metadata_integration_edge_case_expansion_and_robustness_contract.py",
    ),
    SnippetCheck(
        "M234-D006-RUN-04",
        "tests/tooling/test_check_m234_d006_runtime_property_metadata_integration_edge_case_expansion_and_robustness_contract.py",
    ),
    SnippetCheck("M234-D006-RUN-05", "[ok] M234-D006 lane-D readiness chain completed"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-D006-ARCH-01",
        "M234 lane-D D004 runtime property metadata integration core feature expansion anchors",
    ),
    SnippetCheck(
        "M234-D006-ARCH-02",
        "docs/contracts/m234_runtime_property_metadata_integration_core_feature_expansion_d004_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-D006-SPC-01",
        "runtime property metadata integration core feature expansion shall",
    ),
    SnippetCheck(
        "M234-D006-SPC-02",
        "preserve explicit lane-D dependency anchors (`M234-D003`) and fail closed on",
    ),
    SnippetCheck(
        "M234-D006-SPC-03",
        "core-feature expansion evidence drift before edge-case compatibility completion readiness advances.",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-D006-META-01",
        "deterministic lane-D runtime property metadata integration core feature expansion metadata anchors for `M234-D004`",
    ),
    SnippetCheck(
        "M234-D006-META-02",
        "with explicit `M234-D003` dependency continuity so core feature expansion drift fails closed.",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-D006-PKG-01",
        '"check:objc3c:m234-d004-lane-d-readiness": "python scripts/run_m234_d004_lane_d_readiness.py"',
    ),
    SnippetCheck("M234-D006-PKG-02", '"compile:objc3c": '),
    SnippetCheck("M234-D006-PKG-03", '"proof:objc3c": '),
    SnippetCheck("M234-D006-PKG-04", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M234-D006-PKG-05", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--d005-expectations-doc", type=Path, default=DEFAULT_D005_EXPECTATIONS_DOC)
    parser.add_argument("--d005-packet-doc", type=Path, default=DEFAULT_D005_PACKET_DOC)
    parser.add_argument("--d005-checker", type=Path, default=DEFAULT_D005_CHECKER)
    parser.add_argument("--d005-test", type=Path, default=DEFAULT_D005_TEST)
    parser.add_argument("--d005-readiness-runner", type=Path, default=DEFAULT_D005_READINESS_RUNNER)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
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
        (args.expectations_doc, "M234-D006-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M234-D006-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.d005_expectations_doc, "M234-D006-D005-DOC-EXISTS", D005_EXPECTATIONS_SNIPPETS),
        (args.d005_packet_doc, "M234-D006-D005-PKT-EXISTS", D005_PACKET_SNIPPETS),
        (args.readiness_runner, "M234-D006-RUN-EXISTS", READINESS_RUNNER_SNIPPETS),
        (args.architecture_doc, "M234-D006-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M234-D006-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M234-D006-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M234-D006-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.d005_checker, "M234-D006-DEP-D005-ARG-01"),
        (args.d005_test, "M234-D006-DEP-D005-ARG-02"),
        (args.d005_readiness_runner, "M234-D006-DEP-D005-ARG-03"),
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


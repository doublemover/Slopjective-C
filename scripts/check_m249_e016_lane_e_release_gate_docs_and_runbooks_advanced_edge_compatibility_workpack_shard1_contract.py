#!/usr/bin/env python3
"""Fail-closed checker for M249-E016 lane-E release gate/docs/runbooks advanced edge compatibility workpack shard1."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m249-e016-lane-e-release-gate-docs-runbooks-advanced-edge-compatibility-workpack-shard1-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m249_lane_e_release_gate_docs_and_runbooks_advanced_edge_compatibility_workpack_shard1_e016_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m249"
    / "m249_e016_lane_e_release_gate_docs_and_runbooks_advanced_edge_compatibility_workpack_shard1_packet.md"
)
DEFAULT_E015_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m249_lane_e_release_gate_docs_and_runbooks_advanced_core_workpack_shard1_e015_expectations.md"
)
DEFAULT_E015_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m249"
    / "m249_e015_lane_e_release_gate_docs_and_runbooks_advanced_core_workpack_shard1_packet.md"
)
DEFAULT_E015_CHECKER = (
    ROOT
    / "scripts"
    / "check_m249_e015_lane_e_release_gate_docs_and_runbooks_advanced_core_workpack_shard1_contract.py"
)
DEFAULT_E015_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m249_e015_lane_e_release_gate_docs_and_runbooks_advanced_core_workpack_shard1_contract.py"
)
DEFAULT_E015_READINESS_RUNNER = ROOT / "scripts" / "run_m249_e015_lane_e_readiness.py"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m249_e016_lane_e_readiness.py"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m249/M249-E016/lane_e_release_gate_docs_and_runbooks_advanced_edge_compatibility_workpack_shard1_summary.json"
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
        "M249-E016-DOC-EXP-01",
        "# M249 Lane E Release Gate, Docs, and Runbooks Advanced Edge Compatibility Workpack (Shard 1) Expectations (E016)",
    ),
    SnippetCheck(
        "M249-E016-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-release-gate-docs-runbooks-advanced-edge-compatibility-workpack-shard1/m249-e016-v1`",
    ),
    SnippetCheck("M249-E016-DOC-EXP-03", "Dependencies: `M249-E015`"),
    SnippetCheck("M249-E016-DOC-EXP-04", "Issue: `#6963`"),
    SnippetCheck(
        "M249-E016-DOC-EXP-05",
        "scripts/check_m249_e015_lane_e_release_gate_docs_and_runbooks_advanced_core_workpack_shard1_contract.py",
    ),
    SnippetCheck("M249-E016-DOC-EXP-06", "scripts/run_m249_e016_lane_e_readiness.py"),
    SnippetCheck(
        "M249-E016-DOC-EXP-07",
        "traceable across dependency surfaces, including code/spec anchors and milestone",
    ),
    SnippetCheck(
        "M249-E016-DOC-EXP-08",
        "tmp/reports/m249/M249-E016/lane_e_release_gate_docs_and_runbooks_advanced_edge_compatibility_workpack_shard1_summary.json",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E016-DOC-PKT-01",
        "# M249-E016 Lane-E Release Gate, Docs, and Runbooks Advanced Edge Compatibility Workpack (Shard 1) Packet",
    ),
    SnippetCheck("M249-E016-DOC-PKT-02", "Packet: `M249-E016`"),
    SnippetCheck("M249-E016-DOC-PKT-03", "Issue: `#6963`"),
    SnippetCheck("M249-E016-DOC-PKT-04", "Dependencies: `M249-E015`"),
    SnippetCheck(
        "M249-E016-DOC-PKT-05",
        "scripts/check_m249_e015_lane_e_release_gate_docs_and_runbooks_advanced_core_workpack_shard1_contract.py",
    ),
    SnippetCheck(
        "M249-E016-DOC-PKT-06",
        "python scripts/run_m249_e015_lane_e_readiness.py",
    ),
    SnippetCheck(
        "M249-E016-DOC-PKT-07",
        "optimization improvements as mandatory scope inputs.",
    ),
)

E015_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E016-E015-DOC-01",
        "Contract ID: `objc3c-lane-e-release-gate-docs-runbooks-advanced-core-workpack-shard1/m249-e015-v1`",
    ),
    SnippetCheck("M249-E016-E015-DOC-02", "Dependencies: `M249-E014`"),
)

E015_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M249-E016-E015-PKT-01", "Packet: `M249-E015`"),
    SnippetCheck("M249-E016-E015-PKT-02", "Issue: `#6962`"),
    SnippetCheck("M249-E016-E015-PKT-03", "Dependencies"),
)

READINESS_RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E016-RUN-01",
        '"""Run M249-E016 lane-E readiness checks without deep npm nesting."""',
    ),
    SnippetCheck("M249-E016-RUN-02", "scripts/run_m249_e015_lane_e_readiness.py"),
    SnippetCheck(
        "M249-E016-RUN-03",
        "scripts/check_m249_e016_lane_e_release_gate_docs_and_runbooks_advanced_edge_compatibility_workpack_shard1_contract.py",
    ),
    SnippetCheck(
        "M249-E016-RUN-04",
        "tests/tooling/test_check_m249_e016_lane_e_release_gate_docs_and_runbooks_advanced_edge_compatibility_workpack_shard1_contract.py",
    ),
    SnippetCheck("M249-E016-RUN-05", "[ok] M249-E016 lane-E readiness chain completed"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E016-ARCH-01",
        "M249 lane-E E016 advanced edge compatibility workpack (shard 1) anchors release gate/docs/runbooks continuity",
    ),
    SnippetCheck(
        "M249-E016-ARCH-02",
        "docs/contracts/m249_lane_e_release_gate_docs_and_runbooks_advanced_edge_compatibility_workpack_shard1_e016_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E016-SPC-01",
        "release gate/docs/runbooks advanced edge compatibility workpack (shard 1) governance shall preserve",
    ),
    SnippetCheck(
        "M249-E016-SPC-02",
        "explicit lane-E dependency anchors (`M249-E016`, `M249-E015`) and fail closed on",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E016-META-01",
        "deterministic lane-E release gate/docs/runbooks advanced edge compatibility workpack (shard 1) metadata anchors for `M249-E016`",
    ),
    SnippetCheck(
        "M249-E016-META-02",
        "with explicit `M249-E015` dependency continuity and fail-closed advanced edge compatibility evidence continuity.",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E016-PKG-01",
        '"check:objc3c:m249-e016-lane-e-release-gate-docs-runbooks-advanced-edge-compatibility-workpack-shard1-contract"',
    ),
    SnippetCheck(
        "M249-E016-PKG-02",
        '"test:tooling:m249-e016-lane-e-release-gate-docs-runbooks-advanced-edge-compatibility-workpack-shard1-contract"',
    ),
    SnippetCheck(
        "M249-E016-PKG-03",
        '"check:objc3c:m249-e016-lane-e-readiness": "python scripts/run_m249_e016_lane_e_readiness.py"',
    ),
    SnippetCheck("M249-E016-PKG-05", '"test:objc3c:parser-replay-proof": '),
    SnippetCheck("M249-E016-PKG-06", '"test:objc3c:parser-ast-extraction": '),
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
    parser.add_argument("--e015-expectations-doc", type=Path, default=DEFAULT_E015_EXPECTATIONS_DOC)
    parser.add_argument("--e015-packet-doc", type=Path, default=DEFAULT_E015_PACKET_DOC)
    parser.add_argument("--e015-checker", type=Path, default=DEFAULT_E015_CHECKER)
    parser.add_argument("--e015-test", type=Path, default=DEFAULT_E015_TEST)
    parser.add_argument("--e015-readiness-runner", type=Path, default=DEFAULT_E015_READINESS_RUNNER)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--emit-json", action="store_true")
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
        (args.expectations_doc, "M249-E016-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M249-E016-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.e015_expectations_doc, "M249-E016-E015-DOC-EXISTS", E015_EXPECTATIONS_SNIPPETS),
        (args.e015_packet_doc, "M249-E016-E015-PKT-EXISTS", E015_PACKET_SNIPPETS),
        (args.readiness_runner, "M249-E016-RUN-EXISTS", READINESS_RUNNER_SNIPPETS),
        (args.architecture_doc, "M249-E016-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M249-E016-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M249-E016-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M249-E016-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.e015_checker, "M249-E016-DEP-E015-ARG-01"),
        (args.e015_test, "M249-E016-DEP-E015-ARG-02"),
        (args.e015_readiness_runner, "M249-E016-DEP-E015-ARG-03"),
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

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary_payload), encoding="utf-8")

    if args.emit_json:
        print(canonical_json(summary_payload), end="")

    if failures:
        for finding in failures:
            print(
                f"[{finding.check_id}] {finding.artifact}: {finding.detail}",
                file=sys.stderr,
            )
        return 1

    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

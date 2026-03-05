#!/usr/bin/env python3
"""Fail-closed checker for M233-D019 runtime metadata and lookup advanced integration workpack (shard 1)."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m233-d019-installer-runtime-operations-lookup-plumbing-advanced-integration-workpack-shard1-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m233_runtime_metadata_and_lookup_plumbing_advanced_integration_workpack_shard1_d019_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m233"
    / "m233_d019_runtime_metadata_and_lookup_plumbing_advanced_integration_workpack_shard1_packet.md"
)
DEFAULT_D018_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m233_runtime_metadata_and_lookup_plumbing_advanced_conformance_workpack_shard1_d018_expectations.md"
)
DEFAULT_D018_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m233"
    / "m233_d018_runtime_metadata_and_lookup_plumbing_advanced_conformance_workpack_shard1_packet.md"
)
DEFAULT_D018_CHECKER = (
    ROOT
    / "scripts"
    / "check_m233_d018_runtime_metadata_and_lookup_plumbing_advanced_conformance_workpack_shard1_contract.py"
)
DEFAULT_D018_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m233_d018_runtime_metadata_and_lookup_plumbing_advanced_conformance_workpack_shard1_contract.py"
)
DEFAULT_D018_READINESS_RUNNER = ROOT / "scripts" / "run_m233_d018_lane_d_readiness.py"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m233_d019_lane_d_readiness.py"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m233/M233-D019/runtime_metadata_and_lookup_plumbing_advanced_integration_workpack_shard1_contract_summary.json"
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
        "M233-D019-DOC-EXP-01",
        "# M233 Runtime Metadata and Lookup Plumbing Advanced Integration Workpack (Shard 1) Expectations (D019)",
    ),
    SnippetCheck(
        "M233-D019-DOC-EXP-02",
        "Contract ID: `objc3c-installer-runtime-operations-lookup-plumbing-advanced-integration-workpack-shard1/m233-d019-v1`",
    ),
    SnippetCheck("M233-D019-DOC-EXP-03", "Dependencies: `M233-D018`"),
    SnippetCheck("M233-D019-DOC-EXP-04", "Issue: `#6946`"),
    SnippetCheck(
        "M233-D019-DOC-EXP-05",
        "scripts/check_m233_d018_runtime_metadata_and_lookup_plumbing_advanced_conformance_workpack_shard1_contract.py",
    ),
    SnippetCheck("M233-D019-DOC-EXP-06", "scripts/run_m233_d019_lane_d_readiness.py"),
    SnippetCheck("M233-D019-DOC-EXP-07", "code/spec anchors and milestone optimization"),
    SnippetCheck(
        "M233-D019-DOC-EXP-08",
        "tmp/reports/m233/M233-D019/runtime_metadata_and_lookup_plumbing_advanced_integration_workpack_shard1_contract_summary.json",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M233-D019-DOC-PKT-01",
        "# M233-D019 Runtime Metadata and Lookup Plumbing Advanced Integration Workpack (Shard 1) Packet",
    ),
    SnippetCheck("M233-D019-DOC-PKT-02", "Packet: `M233-D019`"),
    SnippetCheck("M233-D019-DOC-PKT-03", "Issue: `#6946`"),
    SnippetCheck("M233-D019-DOC-PKT-04", "Dependencies: `M233-D018`"),
    SnippetCheck(
        "M233-D019-DOC-PKT-05",
        "scripts/check_m233_d018_runtime_metadata_and_lookup_plumbing_advanced_conformance_workpack_shard1_contract.py",
    ),
    SnippetCheck(
        "M233-D019-DOC-PKT-06",
        "python scripts/run_m233_d018_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M233-D019-DOC-PKT-07",
        "optimization improvements as mandatory scope inputs.",
    ),
)

D018_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M233-D019-D018-DOC-01",
        "Contract ID: `objc3c-installer-runtime-operations-lookup-plumbing-advanced-conformance-workpack-shard1/m233-d018-v1`",
    ),
    SnippetCheck("M233-D019-D018-DOC-02", "Dependencies: `M233-D017`"),
)

D018_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M233-D019-D018-PKT-01", "Packet: `M233-D018`"),
    SnippetCheck("M233-D019-D018-PKT-02", "Issue: `#6945`"),
    SnippetCheck("M233-D019-D018-PKT-03", "Dependencies: `M233-D017`"),
)

READINESS_RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M233-D019-RUN-01",
        '"""Run M233-D019 lane-D readiness checks without deep npm nesting."""',
    ),
    SnippetCheck("M233-D019-RUN-02", "scripts/run_m233_d018_lane_d_readiness.py"),
    SnippetCheck(
        "M233-D019-RUN-03",
        "scripts/check_m233_d019_runtime_metadata_and_lookup_plumbing_advanced_integration_workpack_shard1_contract.py",
    ),
    SnippetCheck(
        "M233-D019-RUN-04",
        "tests/tooling/test_check_m233_d019_runtime_metadata_and_lookup_plumbing_advanced_integration_workpack_shard1_contract.py",
    ),
    SnippetCheck("M233-D019-RUN-05", "[ok] M233-D019 lane-D readiness chain completed"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M233-D019-ARCH-01",
        "M233 lane-D D019 advanced integration workpack (shard 1) anchors runtime metadata and lookup plumbing continuity",
    ),
    SnippetCheck(
        "M233-D019-ARCH-02",
        "docs/contracts/m233_runtime_metadata_and_lookup_plumbing_advanced_integration_workpack_shard1_d019_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M233-D019-SPC-01",
        "runtime metadata and lookup plumbing advanced integration workpack (shard 1) governance shall preserve",
    ),
    SnippetCheck(
        "M233-D019-SPC-02",
        "explicit lane-D dependency anchors (`M233-D019`, `M233-D018`) and fail closed on",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M233-D019-META-01",
        "deterministic lane-D runtime metadata and lookup plumbing advanced integration workpack (shard 1) metadata anchors for `M233-D019`",
    ),
    SnippetCheck(
        "M233-D019-META-02",
        "with explicit `M233-D018` dependency continuity and fail-closed advanced integration evidence continuity.",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M233-D019-PKG-01",
        '"check:objc3c:m233-d019-installer-runtime-operations-lookup-plumbing-advanced-integration-workpack-shard1-contract"',
    ),
    SnippetCheck(
        "M233-D019-PKG-02",
        '"test:tooling:m233-d019-installer-runtime-operations-lookup-plumbing-advanced-integration-workpack-shard1-contract"',
    ),
    SnippetCheck(
        "M233-D019-PKG-03",
        '"check:objc3c:m233-d019-lane-d-readiness": "python scripts/run_m233_d019_lane_d_readiness.py"',
    ),
    SnippetCheck("M233-D019-PKG-04", '"compile:objc3c": '),
    SnippetCheck("M233-D019-PKG-05", '"proof:objc3c": '),
    SnippetCheck("M233-D019-PKG-06", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M233-D019-PKG-07", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--d018-expectations-doc", type=Path, default=DEFAULT_D018_EXPECTATIONS_DOC)
    parser.add_argument("--d018-packet-doc", type=Path, default=DEFAULT_D018_PACKET_DOC)
    parser.add_argument("--d018-checker", type=Path, default=DEFAULT_D018_CHECKER)
    parser.add_argument("--d018-test", type=Path, default=DEFAULT_D018_TEST)
    parser.add_argument("--d018-readiness-runner", type=Path, default=DEFAULT_D018_READINESS_RUNNER)
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
        (args.expectations_doc, "M233-D019-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M233-D019-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.d018_expectations_doc, "M233-D019-D018-DOC-EXISTS", D018_EXPECTATIONS_SNIPPETS),
        (args.d018_packet_doc, "M233-D019-D018-PKT-EXISTS", D018_PACKET_SNIPPETS),
        (args.readiness_runner, "M233-D019-RUN-EXISTS", READINESS_RUNNER_SNIPPETS),
        (args.architecture_doc, "M233-D019-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M233-D019-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M233-D019-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M233-D019-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.d018_checker, "M233-D019-DEP-D018-ARG-01"),
        (args.d018_test, "M233-D019-DEP-D018-ARG-02"),
        (args.d018_readiness_runner, "M233-D019-DEP-D018-ARG-03"),
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

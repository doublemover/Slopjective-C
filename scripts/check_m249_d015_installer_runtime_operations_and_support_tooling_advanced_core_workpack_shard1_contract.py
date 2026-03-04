#!/usr/bin/env python3
"""Fail-closed checker for M249-D015 installer/runtime advanced core workpack (shard 1)."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m249-d015-installer-runtime-operations-support-tooling-advanced-core-workpack-shard1-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m249_installer_runtime_operations_and_support_tooling_advanced_core_workpack_shard1_d015_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m249"
    / "m249_d015_installer_runtime_operations_and_support_tooling_advanced_core_workpack_shard1_packet.md"
)
DEFAULT_D014_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m249_installer_runtime_operations_and_support_tooling_release_candidate_and_replay_dry_run_d014_expectations.md"
)
DEFAULT_D014_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m249"
    / "m249_d014_installer_runtime_operations_and_support_tooling_release_candidate_and_replay_dry_run_packet.md"
)
DEFAULT_D014_CHECKER = (
    ROOT
    / "scripts"
    / "check_m249_d014_installer_runtime_operations_and_support_tooling_release_candidate_and_replay_dry_run_contract.py"
)
DEFAULT_D014_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m249_d014_installer_runtime_operations_and_support_tooling_release_candidate_and_replay_dry_run_contract.py"
)
DEFAULT_D014_READINESS_RUNNER = ROOT / "scripts" / "run_m249_d014_lane_d_readiness.py"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m249_d015_lane_d_readiness.py"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m249/M249-D015/installer_runtime_operations_and_support_tooling_advanced_core_workpack_shard1_contract_summary.json"
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
        "M249-D015-DOC-EXP-01",
        "# M249 Installer/Runtime Operations and Support Tooling Advanced Core Workpack (Shard 1) Expectations (D015)",
    ),
    SnippetCheck(
        "M249-D015-DOC-EXP-02",
        "Contract ID: `objc3c-installer-runtime-operations-support-tooling-advanced-core-workpack-shard1/m249-d015-v1`",
    ),
    SnippetCheck("M249-D015-DOC-EXP-03", "Dependencies: `M249-D014`"),
    SnippetCheck("M249-D015-DOC-EXP-04", "Issue: `#6942`"),
    SnippetCheck(
        "M249-D015-DOC-EXP-05",
        "scripts/check_m249_d014_installer_runtime_operations_and_support_tooling_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck("M249-D015-DOC-EXP-06", "scripts/run_m249_d015_lane_d_readiness.py"),
    SnippetCheck("M249-D015-DOC-EXP-07", "code/spec anchors and milestone optimization"),
    SnippetCheck(
        "M249-D015-DOC-EXP-08",
        "tmp/reports/m249/M249-D015/installer_runtime_operations_and_support_tooling_advanced_core_workpack_shard1_contract_summary.json",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-D015-DOC-PKT-01",
        "# M249-D015 Installer/Runtime Operations and Support Tooling Advanced Core Workpack (Shard 1) Packet",
    ),
    SnippetCheck("M249-D015-DOC-PKT-02", "Packet: `M249-D015`"),
    SnippetCheck("M249-D015-DOC-PKT-03", "Issue: `#6942`"),
    SnippetCheck("M249-D015-DOC-PKT-04", "Dependencies: `M249-D014`"),
    SnippetCheck(
        "M249-D015-DOC-PKT-05",
        "scripts/check_m249_d014_installer_runtime_operations_and_support_tooling_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck(
        "M249-D015-DOC-PKT-06",
        "python scripts/run_m249_d014_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M249-D015-DOC-PKT-07",
        "optimization improvements as mandatory scope inputs.",
    ),
)

D014_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-D015-D014-DOC-01",
        "Contract ID: `objc3c-installer-runtime-operations-support-tooling-release-candidate-replay-dry-run/m249-d014-v1`",
    ),
    SnippetCheck("M249-D015-D014-DOC-02", "Dependencies: `M249-D013`"),
)

D014_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M249-D015-D014-PKT-01", "Packet: `M249-D014`"),
    SnippetCheck("M249-D015-D014-PKT-02", "Issue: `#6941`"),
    SnippetCheck("M249-D015-D014-PKT-03", "Dependencies"),
)

READINESS_RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-D015-RUN-01",
        '"""Run M249-D015 lane-D readiness checks without deep npm nesting."""',
    ),
    SnippetCheck("M249-D015-RUN-02", "scripts/run_m249_d014_lane_d_readiness.py"),
    SnippetCheck(
        "M249-D015-RUN-03",
        "scripts/check_m249_d015_installer_runtime_operations_and_support_tooling_advanced_core_workpack_shard1_contract.py",
    ),
    SnippetCheck(
        "M249-D015-RUN-04",
        "tests/tooling/test_check_m249_d015_installer_runtime_operations_and_support_tooling_advanced_core_workpack_shard1_contract.py",
    ),
    SnippetCheck("M249-D015-RUN-05", "[ok] M249-D015 lane-D readiness chain completed"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-D015-ARCH-01",
        "M249 lane-D D015 advanced core workpack (shard 1) anchors installer/runtime operations and support tooling continuity",
    ),
    SnippetCheck(
        "M249-D015-ARCH-02",
        "docs/contracts/m249_installer_runtime_operations_and_support_tooling_advanced_core_workpack_shard1_d015_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-D015-SPC-01",
        "installer/runtime operations and support tooling advanced core workpack (shard 1) governance shall preserve",
    ),
    SnippetCheck(
        "M249-D015-SPC-02",
        "explicit lane-D dependency anchors (`M249-D015`, `M249-D014`) and fail closed on",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-D015-META-01",
        "deterministic lane-D installer/runtime operations and support tooling advanced core workpack (shard 1) metadata anchors for `M249-D015`",
    ),
    SnippetCheck(
        "M249-D015-META-02",
        "with explicit `M249-D014` dependency continuity and fail-closed advanced core evidence continuity.",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-D015-PKG-01",
        '"check:objc3c:m249-d015-installer-runtime-operations-support-tooling-advanced-core-workpack-shard1-contract"',
    ),
    SnippetCheck(
        "M249-D015-PKG-02",
        '"test:tooling:m249-d015-installer-runtime-operations-support-tooling-advanced-core-workpack-shard1-contract"',
    ),
    SnippetCheck(
        "M249-D015-PKG-03",
        '"check:objc3c:m249-d015-lane-d-readiness": "python scripts/run_m249_d015_lane_d_readiness.py"',
    ),
    SnippetCheck("M249-D015-PKG-04", '"compile:objc3c": '),
    SnippetCheck("M249-D015-PKG-05", '"proof:objc3c": '),
    SnippetCheck("M249-D015-PKG-06", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M249-D015-PKG-07", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--d014-expectations-doc", type=Path, default=DEFAULT_D014_EXPECTATIONS_DOC)
    parser.add_argument("--d014-packet-doc", type=Path, default=DEFAULT_D014_PACKET_DOC)
    parser.add_argument("--d014-checker", type=Path, default=DEFAULT_D014_CHECKER)
    parser.add_argument("--d014-test", type=Path, default=DEFAULT_D014_TEST)
    parser.add_argument("--d014-readiness-runner", type=Path, default=DEFAULT_D014_READINESS_RUNNER)
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
        (args.expectations_doc, "M249-D015-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M249-D015-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.d014_expectations_doc, "M249-D015-D014-DOC-EXISTS", D014_EXPECTATIONS_SNIPPETS),
        (args.d014_packet_doc, "M249-D015-D014-PKT-EXISTS", D014_PACKET_SNIPPETS),
        (args.readiness_runner, "M249-D015-RUN-EXISTS", READINESS_RUNNER_SNIPPETS),
        (args.architecture_doc, "M249-D015-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M249-D015-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M249-D015-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M249-D015-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.d014_checker, "M249-D015-DEP-D014-ARG-01"),
        (args.d014_test, "M249-D015-DEP-D014-ARG-02"),
        (args.d014_readiness_runner, "M249-D015-DEP-D014-ARG-03"),
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

#!/usr/bin/env python3
"""Fail-closed checker for M248-C015 replay harness/artifact advanced core workpack (shard 1)."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-c015-replay-harness-and-artifact-contracts-advanced-core-workpack-shard1-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_replay_harness_and_artifact_contracts_advanced_core_workpack_shard1_c015_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_c015_replay_harness_and_artifact_contracts_advanced_core_workpack_shard1_packet.md"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m248_c015_lane_c_readiness.py"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m248/M248-C015/replay_harness_and_artifact_contracts_advanced_core_workpack_shard1_contract_summary.json"
)


@dataclass(frozen=True)
class AssetCheck:
    check_id: str
    lane_task: str
    relative_path: Path


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


PREREQUISITE_ASSETS: tuple[AssetCheck, ...] = (
    AssetCheck(
        "M248-C015-C014-01",
        "M248-C014",
        Path(
            "docs/contracts/m248_replay_harness_and_artifact_contracts_release_candidate_and_replay_dry_run_c014_expectations.md"
        ),
    ),
    AssetCheck(
        "M248-C015-C014-02",
        "M248-C014",
        Path(
            "spec/planning/compiler/m248/m248_c014_replay_harness_and_artifact_contracts_release_candidate_and_replay_dry_run_packet.md"
        ),
    ),
    AssetCheck(
        "M248-C015-C014-03",
        "M248-C014",
        Path(
            "scripts/check_m248_c014_replay_harness_and_artifact_contracts_release_candidate_and_replay_dry_run_contract.py"
        ),
    ),
    AssetCheck(
        "M248-C015-C014-04",
        "M248-C014",
        Path(
            "tests/tooling/test_check_m248_c014_replay_harness_and_artifact_contracts_release_candidate_and_replay_dry_run_contract.py"
        ),
    ),
    AssetCheck(
        "M248-C015-C014-05",
        "M248-C014",
        Path("scripts/run_m248_c014_lane_c_readiness.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-C015-DOC-EXP-01",
        "Contract ID: `objc3c-replay-harness-and-artifact-contracts-advanced-core-workpack-shard1/m248-c015-v1`",
    ),
    SnippetCheck("M248-C015-DOC-EXP-02", "Dependencies: `M248-C014`"),
    SnippetCheck(
        "M248-C015-DOC-EXP-03",
        "Issue `#6831` defines canonical lane-C advanced core workpack (shard 1) scope.",
    ),
    SnippetCheck(
        "M248-C015-DOC-EXP-04",
        "Scope: lane-C replay harness/artifact advanced core workpack (shard 1) closure with fail-closed dependency chaining from C014.",
    ),
    SnippetCheck(
        "M248-C015-DOC-EXP-05",
        "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
    ),
    SnippetCheck(
        "M248-C015-DOC-EXP-06",
        "`check:objc3c:m248-c015-replay-harness-artifact-contracts-advanced-core-workpack-shard1-contract`",
    ),
    SnippetCheck(
        "M248-C015-DOC-EXP-07",
        "`test:tooling:m248-c015-replay-harness-artifact-contracts-advanced-core-workpack-shard1-contract`",
    ),
    SnippetCheck("M248-C015-DOC-EXP-08", "`check:objc3c:m248-c015-lane-c-readiness`"),
    SnippetCheck("M248-C015-DOC-EXP-09", "python scripts/run_m248_c014_lane_c_readiness.py"),
    SnippetCheck(
        "M248-C015-DOC-EXP-10",
        "python scripts/check_m248_c015_replay_harness_and_artifact_contracts_advanced_core_workpack_shard1_contract.py --emit-json",
    ),
    SnippetCheck(
        "M248-C015-DOC-EXP-11",
        "tmp/reports/m248/M248-C015/replay_harness_and_artifact_contracts_advanced_core_workpack_shard1_contract_summary.json",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-C015-DOC-PKT-01", "Packet: `M248-C015`"),
    SnippetCheck("M248-C015-DOC-PKT-02", "Issue: `#6831`"),
    SnippetCheck("M248-C015-DOC-PKT-03", "Dependencies: `M248-C014`"),
    SnippetCheck(
        "M248-C015-DOC-PKT-04",
        "`scripts/check_m248_c015_replay_harness_and_artifact_contracts_advanced_core_workpack_shard1_contract.py`",
    ),
    SnippetCheck(
        "M248-C015-DOC-PKT-05",
        "`tests/tooling/test_check_m248_c015_replay_harness_and_artifact_contracts_advanced_core_workpack_shard1_contract.py`",
    ),
    SnippetCheck("M248-C015-DOC-PKT-06", "`scripts/run_m248_c015_lane_c_readiness.py`"),
    SnippetCheck("M248-C015-DOC-PKT-07", "`python scripts/run_m248_c014_lane_c_readiness.py`"),
    SnippetCheck(
        "M248-C015-DOC-PKT-08",
        "`check:objc3c:m248-c015-replay-harness-artifact-contracts-advanced-core-workpack-shard1-contract`",
    ),
    SnippetCheck(
        "M248-C015-DOC-PKT-09",
        "`test:tooling:m248-c015-replay-harness-artifact-contracts-advanced-core-workpack-shard1-contract`",
    ),
    SnippetCheck("M248-C015-DOC-PKT-10", "`check:objc3c:m248-c015-lane-c-readiness`"),
    SnippetCheck(
        "M248-C015-DOC-PKT-11",
        "`tmp/reports/m248/M248-C015/replay_harness_and_artifact_contracts_advanced_core_workpack_shard1_contract_summary.json`",
    ),
)

PACKAGE_JSON_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-C015-PKG-01",
        '"check:objc3c:m248-c015-replay-harness-artifact-contracts-advanced-core-workpack-shard1-contract"',
    ),
    SnippetCheck(
        "M248-C015-PKG-02",
        '"test:tooling:m248-c015-replay-harness-artifact-contracts-advanced-core-workpack-shard1-contract"',
    ),
    SnippetCheck("M248-C015-PKG-03", '"check:objc3c:m248-c015-lane-c-readiness"'),
    SnippetCheck("M248-C015-PKG-04", "python scripts/run_m248_c015_lane_c_readiness.py"),
)

READINESS_RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-C015-RUN-01", "scripts/run_m248_c014_lane_c_readiness.py"),
    SnippetCheck(
        "M248-C015-RUN-02",
        "scripts/check_m248_c015_replay_harness_and_artifact_contracts_advanced_core_workpack_shard1_contract.py",
    ),
    SnippetCheck(
        "M248-C015-RUN-03",
        "tests/tooling/test_check_m248_c015_replay_harness_and_artifact_contracts_advanced_core_workpack_shard1_contract.py",
    ),
    SnippetCheck("M248-C015-RUN-04", "[ok] M248-C015 lane-C readiness chain completed"),
)

READINESS_RUNNER_FORBIDDEN_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-C015-RUN-FORB-01", "npm run "),
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
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--emit-json", action="store_true", help="Emit canonical summary JSON to stdout.")
    return parser.parse_args(argv)


def check_prerequisite_assets() -> tuple[int, list[Finding]]:
    checks_total = 0
    findings: list[Finding] = []
    for asset in PREREQUISITE_ASSETS:
        checks_total += 1
        absolute = ROOT / asset.relative_path
        if not absolute.exists():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"{asset.lane_task} prerequisite missing: {asset.relative_path.as_posix()}",
                )
            )
            continue
        if not absolute.is_file():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"{asset.lane_task} prerequisite path is not a file: {asset.relative_path.as_posix()}",
                )
            )
    return checks_total, findings


def check_text_contract(
    *,
    path: Path,
    exists_check_id: str,
    required: tuple[SnippetCheck, ...],
    forbidden: tuple[SnippetCheck, ...] = (),
) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists() or not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required text artifact is missing: {display_path(path)}",
            )
        )
        return checks_total, findings

    try:
        text = path.read_text(encoding="utf-8")
    except OSError as exc:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"unable to read required text artifact: {exc}",
            )
        )
        return checks_total, findings

    for snippet in required:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(
                Finding(
                    artifact=display_path(path),
                    check_id=snippet.check_id,
                    detail=f"missing required snippet: {snippet.snippet}",
                )
            )

    for snippet in forbidden:
        checks_total += 1
        if snippet.snippet in text:
            findings.append(
                Finding(
                    artifact=display_path(path),
                    check_id=snippet.check_id,
                    detail=f"forbidden snippet present: {snippet.snippet}",
                )
            )

    return checks_total, findings


def finding_sort_key(finding: Finding) -> tuple[str, str, str]:
    return (finding.artifact, finding.check_id, finding.detail)


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    dep_checks, dep_failures = check_prerequisite_assets()
    checks_total += dep_checks
    failures.extend(dep_failures)

    checks, findings = check_text_contract(
        path=args.expectations_doc,
        exists_check_id="M248-C015-DOC-EXP-EXISTS",
        required=EXPECTATIONS_SNIPPETS,
    )
    checks_total += checks
    failures.extend(findings)

    checks, findings = check_text_contract(
        path=args.packet_doc,
        exists_check_id="M248-C015-DOC-PKT-EXISTS",
        required=PACKET_SNIPPETS,
    )
    checks_total += checks
    failures.extend(findings)

    checks, findings = check_text_contract(
        path=args.package_json,
        exists_check_id="M248-C015-PKG-EXISTS",
        required=PACKAGE_JSON_SNIPPETS,
    )
    checks_total += checks
    failures.extend(findings)

    checks, findings = check_text_contract(
        path=args.readiness_runner,
        exists_check_id="M248-C015-RUN-EXISTS",
        required=READINESS_RUNNER_SNIPPETS,
        forbidden=READINESS_RUNNER_FORBIDDEN_SNIPPETS,
    )
    checks_total += checks
    failures.extend(findings)

    failures = sorted(failures, key=finding_sort_key)
    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [{"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail} for f in failures],
    }

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if args.emit_json:
        sys.stdout.write(canonical_json(summary_payload))

    if failures:
        if not args.emit_json:
            print(f"{MODE}: contract drift detected ({len(failures)} failed check(s)).", file=sys.stderr)
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1

    if not args.emit_json:
        print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

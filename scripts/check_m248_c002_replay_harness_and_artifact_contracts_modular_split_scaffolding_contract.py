#!/usr/bin/env python3
"""Fail-closed contract checker for the M248-C002 replay harness/artifact modular split scaffolding."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-c002-replay-harness-artifact-contracts-modular-split-scaffolding-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_replay_harness_and_artifact_contracts_modular_split_scaffolding_c002_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_c002_replay_harness_and_artifact_contracts_modular_split_scaffolding_packet.md"
)
DEFAULT_C001_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_replay_harness_and_artifact_contracts_contract_freeze_c001_expectations.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m248/M248-C002/"
    "replay_harness_and_artifact_contracts_modular_split_scaffolding_contract_summary.json"
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
        "M248-C002-DOC-EXP-01",
        "# M248 Replay Harness and Artifact Contracts Modular Split/Scaffolding Expectations (C002)",
    ),
    SnippetCheck(
        "M248-C002-DOC-EXP-02",
        "Contract ID: `objc3c-replay-harness-artifact-contracts-modular-split-scaffolding/m248-c002-v1`",
    ),
    SnippetCheck("M248-C002-DOC-EXP-03", "- Dependencies: `M248-C001`"),
    SnippetCheck(
        "M248-C002-DOC-EXP-04",
        "code/spec anchors and milestone optimization improvements as mandatory scope",
    ),
    SnippetCheck(
        "M248-C002-DOC-EXP-05",
        "`spec/planning/compiler/m248/m248_c001_replay_harness_and_artifact_contracts_contract_freeze_packet.md`",
    ),
    SnippetCheck(
        "M248-C002-DOC-EXP-06",
        "`python scripts/check_m248_c002_replay_harness_and_artifact_contracts_modular_split_scaffolding_contract.py`",
    ),
    SnippetCheck(
        "M248-C002-DOC-EXP-07",
        "`tmp/reports/m248/M248-C002/replay_harness_and_artifact_contracts_modular_split_scaffolding_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-C002-DOC-PKT-01",
        "# M248-C002 Replay Harness and Artifact Contracts Modular Split/Scaffolding Packet",
    ),
    SnippetCheck("M248-C002-DOC-PKT-02", "Packet: `M248-C002`"),
    SnippetCheck("M248-C002-DOC-PKT-03", "Dependencies: `M248-C001`"),
    SnippetCheck(
        "M248-C002-DOC-PKT-04",
        "m248_c001_replay_harness_and_artifact_contracts_contract_freeze_packet.md",
    ),
    SnippetCheck(
        "M248-C002-DOC-PKT-05",
        "`check:objc3c:m248-c002-lane-c-readiness`",
    ),
    SnippetCheck(
        "M248-C002-DOC-PKT-06",
        "`test:objc3c:execution-replay-proof`",
    ),
)

C001_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-C002-C001-01",
        "Contract ID: `objc3c-replay-harness-artifact-contracts-contract/m248-c001-v1`",
    ),
    SnippetCheck(
        "M248-C002-C001-02",
        "- Dependencies: none",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-C002-ARCH-01",
        "M248 lane-C C001 replay harness and artifact contract anchors",
    ),
    SnippetCheck(
        "M248-C002-ARCH-02",
        "docs/contracts/m248_replay_harness_and_artifact_contracts_contract_freeze_c001_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-C002-SPC-01",
        "replay harness/artifact governance shall preserve explicit lane-C",
    ),
    SnippetCheck(
        "M248-C002-SPC-02",
        "replay anchors and fail closed on replay evidence drift before execution and",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-C002-META-01",
        "deterministic lane-C replay metadata anchors for `M248-C001`",
    ),
    SnippetCheck(
        "M248-C002-META-02",
        "contract evidence and execution replay continuity so CI replay drift fails",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-C002-PKG-01",
        '"check:objc3c:m248-c002-replay-harness-artifact-contracts-modular-split-scaffolding-contract": '
        '"python scripts/check_m248_c002_replay_harness_and_artifact_contracts_modular_split_scaffolding_contract.py"',
    ),
    SnippetCheck(
        "M248-C002-PKG-02",
        '"test:tooling:m248-c002-replay-harness-artifact-contracts-modular-split-scaffolding-contract": '
        '"python -m pytest tests/tooling/test_check_m248_c002_replay_harness_and_artifact_contracts_modular_split_scaffolding_contract.py -q"',
    ),
    SnippetCheck(
        "M248-C002-PKG-03",
        '"check:objc3c:m248-c002-lane-c-readiness": '
        '"npm run check:objc3c:m248-c001-lane-c-readiness '
        '&& npm run check:objc3c:m248-c002-replay-harness-artifact-contracts-modular-split-scaffolding-contract '
        '&& npm run test:tooling:m248-c002-replay-harness-artifact-contracts-modular-split-scaffolding-contract"',
    ),
    SnippetCheck("M248-C002-PKG-04", '"test:objc3c:lowering-replay-proof": '),
    SnippetCheck("M248-C002-PKG-05", '"test:objc3c:execution-replay-proof": '),
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
    parser.add_argument("--c001-expectations-doc", type=Path, default=DEFAULT_C001_EXPECTATIONS_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_doc_contract(*, path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(Finding(display_path(path), exists_check_id, f"required document is missing: {display_path(path)}"))
        return checks_total, findings
    if not path.is_file():
        findings.append(Finding(display_path(path), exists_check_id, f"required path is not a file: {display_path(path)}"))
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M248-C002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M248-C002-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.c001_expectations_doc, "M248-C002-C001-EXISTS", C001_EXPECTATIONS_SNIPPETS),
        (args.architecture_doc, "M248-C002-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M248-C002-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M248-C002-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M248-C002-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

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


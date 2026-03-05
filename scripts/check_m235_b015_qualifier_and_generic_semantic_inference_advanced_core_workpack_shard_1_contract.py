#!/usr/bin/env python3
"""Fail-closed checker for M235-B015 qualifier/generic semantic inference advanced core workpack (shard 1)."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m235-b015-qualifier-and-generic-semantic-inference-advanced-core-workpack-shard-1-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m235_qualifier_and_generic_semantic_inference_advanced_core_workpack_shard_1_b015_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m235"
    / "m235_b015_qualifier_and_generic_semantic_inference_advanced_core_workpack_shard_1_packet.md"
)
DEFAULT_B014_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m235_qualifier_and_generic_semantic_inference_release_candidate_and_replay_dry_run_b014_expectations.md"
)
DEFAULT_B014_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m235"
    / "m235_b014_qualifier_and_generic_semantic_inference_release_candidate_and_replay_dry_run_packet.md"
)
DEFAULT_B014_CHECKER = (
    ROOT
    / "scripts"
    / "check_m235_b014_qualifier_and_generic_semantic_inference_release_candidate_and_replay_dry_run_contract.py"
)
DEFAULT_B014_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m235_b014_qualifier_and_generic_semantic_inference_release_candidate_and_replay_dry_run_contract.py"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m235/M235-B015/qualifier_and_generic_semantic_inference_advanced_core_workpack_shard_1_summary.json"
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
        "M235-B015-DOC-EXP-01",
        "# M235 Qualifier/Generic Semantic Inference Advanced Core Workpack (Shard 1) Expectations (B015)",
    ),
    SnippetCheck(
        "M235-B015-DOC-EXP-02",
        "Contract ID: `objc3c-qualifier-and-generic-semantic-inference-advanced-core-workpack-shard-1/m235-b015-v1`",
    ),
    SnippetCheck("M235-B015-DOC-EXP-03", "Dependencies: `M235-B014`"),
    SnippetCheck(
        "M235-B015-DOC-EXP-04",
        "Issue `#5795` defines canonical lane-B advanced core workpack (shard 1) scope.",
    ),
    SnippetCheck("M235-B015-DOC-EXP-05", "advanced_core_workpack_shard_1_consistent"),
    SnippetCheck("M235-B015-DOC-EXP-06", "advanced_core_workpack_shard_1_ready"),
    SnippetCheck("M235-B015-DOC-EXP-07", "advanced_core_workpack_shard_1_key_ready"),
    SnippetCheck("M235-B015-DOC-EXP-08", "advanced_core_workpack_shard_1_key"),
    SnippetCheck("M235-B015-DOC-EXP-09", "`M235-B014`"),
    SnippetCheck(
        "M235-B015-DOC-EXP-10",
        "`scripts/check_m235_b014_qualifier_and_generic_semantic_inference_release_candidate_and_replay_dry_run_contract.py`",
    ),
    SnippetCheck(
        "M235-B015-DOC-EXP-11",
        "`check:objc3c:m235-b015-qualifier-and-generic-semantic-inference-advanced-core-workpack-shard-1-contract`",
    ),
    SnippetCheck(
        "M235-B015-DOC-EXP-12",
        "`tmp/reports/m235/M235-B015/qualifier_and_generic_semantic_inference_advanced_core_workpack_shard_1_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-B015-DOC-PKT-01",
        "# M235-B015 Qualifier/Generic Semantic Inference Advanced Core Workpack (Shard 1) Packet",
    ),
    SnippetCheck("M235-B015-DOC-PKT-02", "Packet: `M235-B015`"),
    SnippetCheck("M235-B015-DOC-PKT-03", "Issue: `#5795`"),
    SnippetCheck("M235-B015-DOC-PKT-04", "Dependencies: `M235-B014`"),
    SnippetCheck("M235-B015-DOC-PKT-05", "Theme: `advanced core workpack (shard 1)`"),
    SnippetCheck(
        "M235-B015-DOC-PKT-06",
        "`scripts/check_m235_b015_qualifier_and_generic_semantic_inference_advanced_core_workpack_shard_1_contract.py`",
    ),
    SnippetCheck(
        "M235-B015-DOC-PKT-07",
        "`tests/tooling/test_check_m235_b015_qualifier_and_generic_semantic_inference_advanced_core_workpack_shard_1_contract.py`",
    ),
    SnippetCheck("M235-B015-DOC-PKT-08", "`npm run check:objc3c:m235-b015-lane-b-readiness`"),
)

B014_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-B015-B014-DOC-01",
        "# M235 Qualifier/Generic Semantic Inference Release-Candidate and Replay Dry-Run Expectations (B014)",
    ),
    SnippetCheck(
        "M235-B015-B014-DOC-02",
        "Contract ID: `objc3c-qualifier-and-generic-semantic-inference-release-candidate-and-replay-dry-run/m235-b014-v1`",
    ),
    SnippetCheck("M235-B015-B014-DOC-03", "Dependencies: `M235-B013`"),
)

B014_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M235-B015-B014-PKT-01", "Packet: `M235-B014`"),
    SnippetCheck("M235-B015-B014-PKT-02", "Issue: `#5794`"),
    SnippetCheck("M235-B015-B014-PKT-03", "Dependencies: `M235-B013`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-B015-ARCH-01",
        "M235 lane-B B015 qualifier/generic semantic inference advanced core workpack (shard 1) anchors",
    ),
    SnippetCheck(
        "M235-B015-ARCH-02",
        "m235_qualifier_and_generic_semantic_inference_advanced_core_workpack_shard_1_b015_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-B015-SPC-01",
        "qualifier/generic semantic inference advanced core workpack (shard 1) governance shall preserve explicit",
    ),
    SnippetCheck(
        "M235-B015-SPC-02",
        "lane-B dependency anchors (`M235-B014`) and fail closed on advanced core workpack (shard 1) evidence drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-B015-META-01",
        "deterministic lane-B qualifier/generic semantic inference advanced core workpack (shard 1) metadata anchors for `M235-B015`",
    ),
    SnippetCheck(
        "M235-B015-META-02",
        "explicit `M235-B014` dependency continuity so advanced core workpack (shard 1) drift fails closed",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-B015-PKG-01",
        '"check:objc3c:m235-b015-qualifier-and-generic-semantic-inference-advanced-core-workpack-shard-1-contract": '
        '"python scripts/check_m235_b015_qualifier_and_generic_semantic_inference_advanced_core_workpack_shard_1_contract.py"',
    ),
    SnippetCheck(
        "M235-B015-PKG-02",
        '"test:tooling:m235-b015-qualifier-and-generic-semantic-inference-advanced-core-workpack-shard-1-contract": '
        '"python -m pytest tests/tooling/test_check_m235_b015_qualifier_and_generic_semantic_inference_advanced_core_workpack_shard_1_contract.py -q"',
    ),
    SnippetCheck(
        "M235-B015-PKG-03",
        '"check:objc3c:m235-b015-lane-b-readiness": '
        '"npm run check:objc3c:m235-b014-lane-b-readiness '
        '&& npm run check:objc3c:m235-b015-qualifier-and-generic-semantic-inference-advanced-core-workpack-shard-1-contract '
        '&& npm run test:tooling:m235-b015-qualifier-and-generic-semantic-inference-advanced-core-workpack-shard-1-contract"',
    ),
    SnippetCheck("M235-B015-PKG-04", '"compile:objc3c": '),
    SnippetCheck("M235-B015-PKG-05", '"test:objc3c:perf-budget": '),
    SnippetCheck("M235-B015-PKG-06", '"test:objc3c:parser-replay-proof": '),
    SnippetCheck("M235-B015-PKG-07", '"test:objc3c:parser-ast-extraction": '),
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
    parser.add_argument("--b014-expectations-doc", type=Path, default=DEFAULT_B014_EXPECTATIONS_DOC)
    parser.add_argument("--b014-packet-doc", type=Path, default=DEFAULT_B014_PACKET_DOC)
    parser.add_argument("--b014-checker", type=Path, default=DEFAULT_B014_CHECKER)
    parser.add_argument("--b014-test", type=Path, default=DEFAULT_B014_TEST)
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
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings
    if not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required path is not a file: {display_path(path)}",
            )
        )
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(
                Finding(
                    artifact=display_path(path),
                    check_id=snippet.check_id,
                    detail=f"missing required snippet: {snippet.snippet}",
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M235-B015-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M235-B015-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.b014_expectations_doc, "M235-B015-B014-DOC-EXISTS", B014_EXPECTATIONS_SNIPPETS),
        (args.b014_packet_doc, "M235-B015-B014-PKT-EXISTS", B014_PACKET_SNIPPETS),
        (args.architecture_doc, "M235-B015-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M235-B015-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M235-B015-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M235-B015-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.b014_checker, "M235-B015-DEP-B014-ARG-01"),
        (args.b014_test, "M235-B015-DEP-B014-ARG-02"),
    ):
        checks_total += 1
        if not path.exists():
            failures.append(
                Finding(
                    artifact=display_path(path),
                    check_id=check_id,
                    detail=f"required dependency path is missing: {display_path(path)}",
                )
            )
            continue
        if not path.is_file():
            failures.append(
                Finding(
                    artifact=display_path(path),
                    check_id=check_id,
                    detail=f"required dependency path is not a file: {display_path(path)}",
                )
            )

    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
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



#!/usr/bin/env python3
"""Fail-closed checker for M245-D014 build/link/runtime reproducibility release-candidate and replay dry-run."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m245-d014-build-link-runtime-reproducibility-operations-release-candidate-and-replay-dry-run-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m245_build_link_runtime_reproducibility_operations_release_candidate_and_replay_dry_run_d014_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m245"
    / "m245_d014_build_link_runtime_reproducibility_operations_release_candidate_and_replay_dry_run_packet.md"
)
DEFAULT_D013_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m245_build_link_runtime_reproducibility_operations_docs_and_operator_runbook_synchronization_d013_expectations.md"
)
DEFAULT_D013_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m245"
    / "m245_d013_build_link_runtime_reproducibility_operations_docs_and_operator_runbook_synchronization_packet.md"
)
DEFAULT_D013_CHECKER = (
    ROOT
    / "scripts"
    / "check_m245_d013_build_link_runtime_reproducibility_operations_docs_and_operator_runbook_synchronization_contract.py"
)
DEFAULT_D013_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m245_d013_build_link_runtime_reproducibility_operations_docs_and_operator_runbook_synchronization_contract.py"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m245/M245-D014/build_link_runtime_reproducibility_operations_release_candidate_and_replay_dry_run_contract_summary.json"
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
        "M245-D014-DOC-EXP-01",
        "# M245 Build/Link/Runtime Reproducibility Operations Release-Candidate and Replay Dry-Run Expectations (D014)",
    ),
    SnippetCheck(
        "M245-D014-DOC-EXP-02",
        "Contract ID: `objc3c-build-link-runtime-reproducibility-operations-release-candidate-and-replay-dry-run/m245-d014-v1`",
    ),
    SnippetCheck("M245-D014-DOC-EXP-03", "Dependencies: `M245-D013`"),
    SnippetCheck(
        "M245-D014-DOC-EXP-04",
        "Issue `#6665` defines canonical lane-D release-candidate and replay dry-run scope.",
    ),
    SnippetCheck(
        "M245-D014-DOC-EXP-05",
        "dependency continuity and code/spec anchors as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M245-D014-DOC-EXP-06",
        "docs/contracts/m245_build_link_runtime_reproducibility_operations_docs_and_operator_runbook_synchronization_d013_expectations.md",
    ),
    SnippetCheck(
        "M245-D014-DOC-EXP-07",
        "scripts/check_m245_d013_build_link_runtime_reproducibility_operations_docs_and_operator_runbook_synchronization_contract.py",
    ),
    SnippetCheck(
        "M245-D014-DOC-EXP-08",
        "`python scripts/check_m245_d014_build_link_runtime_reproducibility_operations_release_candidate_and_replay_dry_run_contract.py --emit-json`",
    ),
    SnippetCheck(
        "M245-D014-DOC-EXP-09",
        "tests/tooling/test_check_m245_d014_build_link_runtime_reproducibility_operations_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck(
        "M245-D014-DOC-EXP-10",
        "`tmp/reports/m245/M245-D014/build_link_runtime_reproducibility_operations_release_candidate_and_replay_dry_run_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-D014-DOC-PKT-01",
        "# M245-D014 Build/Link/Runtime Reproducibility Operations Release-Candidate and Replay Dry-Run Packet",
    ),
    SnippetCheck("M245-D014-DOC-PKT-02", "Packet: `M245-D014`"),
    SnippetCheck("M245-D014-DOC-PKT-03", "Issue: `#6665`"),
    SnippetCheck("M245-D014-DOC-PKT-04", "Dependencies: `M245-D013`"),
    SnippetCheck("M245-D014-DOC-PKT-05", "Theme: `release-candidate and replay dry-run`"),
    SnippetCheck(
        "M245-D014-DOC-PKT-06",
        "dependency continuity and code/spec anchors as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M245-D014-DOC-PKT-07",
        "scripts/check_m245_d014_build_link_runtime_reproducibility_operations_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck(
        "M245-D014-DOC-PKT-08",
        "tests/tooling/test_check_m245_d014_build_link_runtime_reproducibility_operations_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck(
        "M245-D014-DOC-PKT-09",
        "spec/planning/compiler/m245/m245_d013_build_link_runtime_reproducibility_operations_docs_and_operator_runbook_synchronization_packet.md",
    ),
    SnippetCheck("M245-D014-DOC-PKT-10", "Freeze date: `2026-03-04`"),
    SnippetCheck(
        "M245-D014-DOC-PKT-11",
        "`python scripts/check_m245_d014_build_link_runtime_reproducibility_operations_release_candidate_and_replay_dry_run_contract.py --emit-json`",
    ),
)

D013_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-D014-D013-DOC-01",
        "# M245 Build/Link/Runtime Reproducibility Operations Docs and Operator Runbook Synchronization Expectations (D013)",
    ),
    SnippetCheck(
        "M245-D014-D013-DOC-02",
        "Contract ID: `objc3c-build-link-runtime-reproducibility-operations-docs-and-operator-runbook-synchronization/m245-d013-v1`",
    ),
    SnippetCheck("M245-D014-D013-DOC-03", "Dependencies: `M245-D012`"),
)

D013_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-D014-D013-PKT-01", "Packet: `M245-D013`"),
    SnippetCheck("M245-D014-D013-PKT-02", "Issue: `#6664`"),
    SnippetCheck("M245-D014-D013-PKT-03", "Dependencies: `M245-D012`"),
    SnippetCheck("M245-D014-D013-PKT-04", "Freeze date: `2026-03-04`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-D014-ARCH-01",
        "M245 lane-D D010 build/link/runtime reproducibility conformance corpus expansion",
    ),
    SnippetCheck(
        "M245-D014-ARCH-02",
        "docs/contracts/m245_build_link_runtime_reproducibility_operations_conformance_corpus_expansion_d010_expectations.md",
    ),
    SnippetCheck(
        "M245-D014-ARCH-03",
        "and fail-closed against `M245-D009` dependency drift.",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-D014-SPC-01",
        "build/link/runtime reproducibility conformance matrix implementation governance shall preserve explicit",
    ),
    SnippetCheck(
        "M245-D014-SPC-02",
        "lane-D dependency anchors (`M245-D008`) and fail closed on conformance matrix evidence drift",
    ),
    SnippetCheck(
        "M245-D014-SPC-03",
        "build/link/runtime reproducibility conformance corpus expansion governance shall preserve explicit",
    ),
    SnippetCheck(
        "M245-D014-SPC-04",
        "lane-D dependency anchors (`M245-D009`) and fail closed on conformance corpus evidence drift",
    ),
    SnippetCheck(
        "M245-D014-SPC-05",
        "before runtime reproducibility performance-and-quality-guardrails validation advances.",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-D014-META-01",
        "deterministic lane-D build/link/runtime reproducibility conformance matrix implementation metadata anchors for `M245-D009`",
    ),
    SnippetCheck(
        "M245-D014-META-02",
        "deterministic lane-D build/link/runtime reproducibility conformance corpus expansion metadata anchors for `M245-D010`",
    ),
    SnippetCheck(
        "M245-D014-META-03",
        "with explicit `M245-D009` dependency continuity so conformance corpus drift fails closed.",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-D014-PKG-01",
        '"check:objc3c:m245-d011-build-link-runtime-reproducibility-operations-performance-and-quality-guardrails-contract": '
        '"python scripts/check_m245_d011_build_link_runtime_reproducibility_operations_performance_and_quality_guardrails_contract.py"',
    ),
    SnippetCheck(
        "M245-D014-PKG-02",
        '"test:tooling:m245-d011-build-link-runtime-reproducibility-operations-performance-and-quality-guardrails-contract": '
        '"python -m pytest tests/tooling/test_check_m245_d011_build_link_runtime_reproducibility_operations_performance_and_quality_guardrails_contract.py -q"',
    ),
    SnippetCheck(
        "M245-D014-PKG-03",
        '"check:objc3c:m245-d011-lane-d-readiness": '
        '"npm run check:objc3c:m245-d010-lane-d-readiness '
        '&& npm run check:objc3c:m245-d011-build-link-runtime-reproducibility-operations-performance-and-quality-guardrails-contract '
        '&& npm run test:tooling:m245-d011-build-link-runtime-reproducibility-operations-performance-and-quality-guardrails-contract"',
    ),
    SnippetCheck("M245-D014-PKG-04", '"compile:objc3c": '),
    SnippetCheck("M245-D014-PKG-05", '"proof:objc3c": '),
    SnippetCheck("M245-D014-PKG-06", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M245-D014-PKG-07", '"test:objc3c:perf-budget": '),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def finding_sort_key(finding: Finding) -> tuple[str, str, str]:
    return (finding.check_id, finding.artifact, finding.detail)


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--d013-expectations-doc", type=Path, default=DEFAULT_D013_EXPECTATIONS_DOC)
    parser.add_argument("--d013-packet-doc", type=Path, default=DEFAULT_D013_PACKET_DOC)
    parser.add_argument("--d013-checker", type=Path, default=DEFAULT_D013_CHECKER)
    parser.add_argument("--d013-test", type=Path, default=DEFAULT_D013_TEST)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit canonical summary JSON to stdout.",
    )
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
        (args.expectations_doc, "M245-D014-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M245-D014-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.d013_expectations_doc, "M245-D014-D013-DOC-EXISTS", D013_EXPECTATIONS_SNIPPETS),
        (args.d013_packet_doc, "M245-D014-D013-PKT-EXISTS", D013_PACKET_SNIPPETS),
        (args.architecture_doc, "M245-D014-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M245-D014-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M245-D014-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M245-D014-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.d013_checker, "M245-D014-DEP-D013-ARG-01"),
        (args.d013_test, "M245-D014-DEP-D013-ARG-02"),
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

    failures = sorted(failures, key=finding_sort_key)
    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": failure.artifact, "check_id": failure.check_id, "detail": failure.detail}
            for failure in failures
        ],
    }

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if args.emit_json:
        print(canonical_json(summary_payload), end="")

    if failures:
        if not args.emit_json:
            for finding in failures:
                print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    if not args.emit_json:
        print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

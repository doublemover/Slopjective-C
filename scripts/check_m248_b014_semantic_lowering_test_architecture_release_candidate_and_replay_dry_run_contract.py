#!/usr/bin/env python3
"""Fail-closed contract checker for M248-B014 semantic/lowering release-candidate replay dry-run."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-b014-semantic-lowering-test-architecture-release-candidate-and-replay-dry-run-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_b014_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_b014_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_packet.md"
)
DEFAULT_B013_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_semantic_lowering_test_architecture_docs_and_operator_runbook_synchronization_b013_expectations.md"
)
DEFAULT_B013_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_b013_semantic_lowering_test_architecture_docs_and_operator_runbook_synchronization_packet.md"
)
DEFAULT_B013_CHECKER = (
    ROOT
    / "scripts"
    / "check_m248_b013_semantic_lowering_test_architecture_docs_and_operator_runbook_synchronization_contract.py"
)
DEFAULT_B013_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m248_b013_semantic_lowering_test_architecture_docs_and_operator_runbook_synchronization_contract.py"
)
DEFAULT_B013_READINESS_RUNNER = ROOT / "scripts" / "run_m248_b013_lane_b_readiness.py"
DEFAULT_B014_RUN_SCRIPT = (
    ROOT
    / "scripts"
    / "run_m248_b014_semantic_lowering_test_architecture_release_replay_dry_run.ps1"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m248/M248-B014/"
    "semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_contract_summary.json"
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
        "M248-B014-DOC-EXP-01",
        "# M248 Semantic/Lowering Test Architecture Release-Candidate and Replay Dry-Run Expectations (B014)",
    ),
    SnippetCheck(
        "M248-B014-DOC-EXP-02",
        "Contract ID: `objc3c-semantic-lowering-test-architecture-release-candidate-replay-dry-run/m248-b014-v1`",
    ),
    SnippetCheck("M248-B014-DOC-EXP-03", "- Dependencies: `M248-B013`"),
    SnippetCheck(
        "M248-B014-DOC-EXP-04",
        "Issue `#6814` defines canonical lane-B release-candidate and replay dry-run scope.",
    ),
    SnippetCheck(
        "M248-B014-DOC-EXP-05",
        "`scripts/check_m248_b013_semantic_lowering_test_architecture_docs_and_operator_runbook_synchronization_contract.py`",
    ),
    SnippetCheck(
        "M248-B014-DOC-EXP-06",
        "release-candidate-replay-dry-run-key continuity remain fail-closed across",
    ),
    SnippetCheck(
        "M248-B014-DOC-EXP-07",
        "`scripts/run_m248_b014_semantic_lowering_test_architecture_release_replay_dry_run.ps1`",
    ),
    SnippetCheck(
        "M248-B014-DOC-EXP-08",
        "`check:objc3c:m248-b014-semantic-lowering-test-architecture-release-candidate-and-replay-dry-run-contract`",
    ),
    SnippetCheck(
        "M248-B014-DOC-EXP-09",
        "`test:tooling:m248-b014-semantic-lowering-test-architecture-release-candidate-and-replay-dry-run-contract`",
    ),
    SnippetCheck(
        "M248-B014-DOC-EXP-10",
        "python scripts/check_m248_b014_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_contract.py --emit-json",
    ),
    SnippetCheck(
        "M248-B014-DOC-EXP-11",
        "`tmp/reports/m248/M248-B014/replay_dry_run_summary.json`",
    ),
    SnippetCheck(
        "M248-B014-DOC-EXP-12",
        "`tmp/reports/m248/M248-B014/semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-B014-DOC-PKT-01",
        "# M248-B014 Semantic/Lowering Test Architecture Release-Candidate and Replay Dry-Run Packet",
    ),
    SnippetCheck("M248-B014-DOC-PKT-02", "Packet: `M248-B014`"),
    SnippetCheck("M248-B014-DOC-PKT-03", "Issue: `#6814`"),
    SnippetCheck("M248-B014-DOC-PKT-04", "Dependencies: `M248-B013`"),
    SnippetCheck(
        "M248-B014-DOC-PKT-05",
        "`scripts/check_m248_b014_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_contract.py`",
    ),
    SnippetCheck(
        "M248-B014-DOC-PKT-06",
        "`tests/tooling/test_check_m248_b014_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_contract.py`",
    ),
    SnippetCheck(
        "M248-B014-DOC-PKT-07",
        "`scripts/run_m248_b014_semantic_lowering_test_architecture_release_replay_dry_run.ps1`",
    ),
    SnippetCheck(
        "M248-B014-DOC-PKT-08",
        "`check:objc3c:m248-b014-lane-b-readiness`",
    ),
    SnippetCheck(
        "M248-B014-DOC-PKT-09",
        "long_tail_grammar_gate_signoff_ready",
    ),
    SnippetCheck(
        "M248-B014-DOC-PKT-10",
        "improvements as mandatory scope inputs.",
    ),
)

B013_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-B014-B013-DOC-01",
        "Contract ID: `objc3c-semantic-lowering-test-architecture-docs-operator-runbook-synchronization/m248-b013-v1`",
    ),
    SnippetCheck("M248-B014-B013-DOC-02", "- Dependencies: `M248-B012`"),
    SnippetCheck(
        "M248-B014-B013-DOC-03",
        "scripts/check_m248_b013_semantic_lowering_test_architecture_docs_and_operator_runbook_synchronization_contract.py",
    ),
)

B013_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-B014-B013-PKT-01", "Packet: `M248-B013`"),
    SnippetCheck("M248-B014-B013-PKT-02", "Issue: `#6813`"),
    SnippetCheck("M248-B014-B013-PKT-03", "Dependencies: `M248-B012`"),
)

B014_RUN_SCRIPT_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-B014-RUN-01", '"module.manifest.json"'),
    SnippetCheck("M248-B014-RUN-02", '"module.diagnostics.json"'),
    SnippetCheck("M248-B014-RUN-03", '"module.ll"'),
    SnippetCheck("M248-B014-RUN-04", '"module.object-backend.txt"'),
    SnippetCheck("M248-B014-RUN-05", "readiness.ready_for_lowering"),
    SnippetCheck("M248-B014-RUN-06", "readiness.parse_artifact_replay_key_deterministic"),
    SnippetCheck("M248-B014-RUN-07", "readiness.toolchain_runtime_ga_operations_docs_runbook_sync_consistent"),
    SnippetCheck("M248-B014-RUN-08", "readiness.toolchain_runtime_ga_operations_docs_runbook_sync_ready"),
    SnippetCheck("M248-B014-RUN-09", "readiness.long_tail_grammar_integration_closeout_consistent"),
    SnippetCheck("M248-B014-RUN-10", "readiness.long_tail_grammar_gate_signoff_ready"),
    SnippetCheck("M248-B014-RUN-11", "long_tail_grammar_gate_signoff_ready=true"),
    SnippetCheck("M248-B014-RUN-12", "toolchain_runtime_ga_operations_docs_runbook_sync_key="),
    SnippetCheck("M248-B014-RUN-13", "readiness.long_tail_grammar_integration_closeout_key"),
    SnippetCheck(
        "M248-B014-RUN-14",
        "objc3c-semantic-lowering-test-architecture-release-candidate-replay-dry-run/m248-b014-v1",
    ),
    SnippetCheck("M248-B014-RUN-15", "replay_dry_run_summary.json"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-B014-ARCH-01",
        "M248 lane-B B001 semantic/lowering test architecture anchors explicit lane-B",
    ),
    SnippetCheck(
        "M248-B014-ARCH-02",
        "M248 lane-B B002 semantic/lowering modular split/scaffolding anchors explicit",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-B014-SPC-01",
        "semantic/lowering test architecture governance shall preserve explicit lane-B",
    ),
    SnippetCheck(
        "M248-B014-SPC-02",
        "dependency anchors (`M248-B001`) and fail closed on modular split evidence",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-B014-META-01",
        "deterministic lane-B semantic/lowering metadata anchors for `M248-B001`",
    ),
    SnippetCheck(
        "M248-B014-META-02",
        "`M248-B002` with explicit `M248-B001` dependency continuity so semantic",
    ),
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
    parser.add_argument("--b013-expectations-doc", type=Path, default=DEFAULT_B013_EXPECTATIONS_DOC)
    parser.add_argument("--b013-packet-doc", type=Path, default=DEFAULT_B013_PACKET_DOC)
    parser.add_argument("--b013-checker", type=Path, default=DEFAULT_B013_CHECKER)
    parser.add_argument("--b013-test", type=Path, default=DEFAULT_B013_TEST)
    parser.add_argument("--b013-readiness-runner", type=Path, default=DEFAULT_B013_READINESS_RUNNER)
    parser.add_argument("--b014-run-script", type=Path, default=DEFAULT_B014_RUN_SCRIPT)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit canonical summary JSON to stdout.",
    )
    return parser.parse_args(argv)


def check_doc_contract(
    *, path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]
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
        (args.expectations_doc, "M248-B014-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M248-B014-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.b013_expectations_doc, "M248-B014-B013-DOC-EXISTS", B013_EXPECTATIONS_SNIPPETS),
        (args.b013_packet_doc, "M248-B014-B013-PKT-EXISTS", B013_PACKET_SNIPPETS),
        (args.b014_run_script, "M248-B014-RUN-EXISTS", B014_RUN_SCRIPT_SNIPPETS),
        (args.architecture_doc, "M248-B014-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M248-B014-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M248-B014-META-EXISTS", METADATA_SPEC_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.b013_checker, "M248-B014-DEP-B013-ARG-01"),
        (args.b013_test, "M248-B014-DEP-B013-ARG-02"),
        (args.b013_readiness_runner, "M248-B014-DEP-B013-ARG-03"),
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

    failures = sorted(failures, key=lambda failure: (failure.artifact, failure.check_id, failure.detail))
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

#!/usr/bin/env python3
"""Fail-closed validator for M250-A001 frontend stability long-tail grammar closure freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-frontend-stability-long-tail-grammar-closure-freeze-contract-a001-v1"

ARTIFACTS: dict[str, Path] = {
    "parser_contract": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_contract.h",
    "parser_source": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp",
    "pipeline_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "readiness_surface": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_parse_lowering_readiness_surface.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_frontend_stability_long_tail_grammar_closure_a001_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "parser_contract": (
        ("M250-A001-PAR-01", "struct Objc3ParserContractSnapshot {"),
        ("M250-A001-PAR-02", "bool deterministic_handoff = true;"),
        ("M250-A001-PAR-03", "bool parser_recovery_replay_ready = true;"),
        ("M250-A001-PAR-04", "BuildObjc3ParserContractSnapshotFingerprint"),
        ("M250-A001-PAR-05", "snapshot.deterministic_handoff ? 1ull : 0ull"),
        ("M250-A001-PAR-06", "snapshot.parser_recovery_replay_ready ? 1ull : 0ull"),
        ("M250-A001-PAR-07", "inline Objc3ParserContractSnapshot BuildObjc3ParserContractSnapshot("),
        ("M250-A001-PAR-08", "snapshot.deterministic_handoff = true;"),
        ("M250-A001-PAR-09", "snapshot.parser_recovery_replay_ready = true;"),
    ),
    "parser_source": (
        ("M250-A001-SRC-01", "const bool deterministic_handoff ="),
        ("M250-A001-SRC-02", "result.contract_snapshot = BuildObjc3ParserContractSnapshot("),
    ),
    "pipeline_source": (
        ("M250-A001-PIPE-01", "result.parser_contract_snapshot = parse_result.contract_snapshot;"),
    ),
    "readiness_surface": (
        ("M250-A001-REA-01", "const Objc3ParserContractSnapshot &parser_snapshot ="),
        ("M250-A001-REA-02", "surface.parser_contract_deterministic = parser_snapshot.deterministic_handoff;"),
        ("M250-A001-REA-03", "surface.parser_recovery_replay_ready = parser_snapshot.parser_recovery_replay_ready;"),
        ("M250-A001-REA-04", "surface.parser_contract_snapshot_present &&"),
        ("M250-A001-REA-05", "surface.parser_contract_deterministic &&"),
        ("M250-A001-REA-06", "surface.parser_recovery_replay_ready &&"),
        ("M250-A001-REA-07", "surface.failure_reason = \"parser handoff is not deterministic\";"),
        ("M250-A001-REA-08", "surface.failure_reason = \"parser recovery handoff is not replay ready\";"),
    ),
    "architecture_doc": (
        ("M250-A001-ARCH-01", "M250 lane-A frontend stability freeze anchors long-tail grammar closure"),
        ("M250-A001-ARCH-02", "parse/objc3_parser_contract.h"),
        ("M250-A001-ARCH-03", "pipeline/objc3_parse_lowering_readiness_surface.h"),
    ),
    "contract_doc": (
        (
            "M250-A001-DOC-01",
            "Contract ID: `objc3c-frontend-stability-long-tail-grammar-closure-freeze/m250-a001-v1`",
        ),
        ("M250-A001-DOC-02", "Objc3ParserContractSnapshot"),
        ("M250-A001-DOC-03", "result.parser_contract_snapshot = parse_result.contract_snapshot"),
        ("M250-A001-DOC-04", "parser_recovery_replay_ready"),
        ("M250-A001-DOC-05", "tmp/reports/m250/M250-A001/frontend_stability_long_tail_grammar_closure_contract_summary.json"),
    ),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path("tmp/reports/m250/M250-A001/frontend_stability_long_tail_grammar_closure_contract_summary.json"),
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        text = load_text(path, artifact=artifact)
        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                passed_checks += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))

    ok = not findings
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": total_checks,
        "checks_passed": passed_checks,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in findings
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

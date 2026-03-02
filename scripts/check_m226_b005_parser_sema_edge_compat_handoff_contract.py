#!/usr/bin/env python3
"""Fail-closed validator for M226-B005 parser-sema edge compatibility handoff."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-parser-sema-edge-compat-handoff-contract-b005-v1"

ARTIFACTS: dict[str, Path] = {
    "sema_handoff": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_parser_sema_handoff_scaffold.h",
    "sema_contract": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h",
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parser_sema_edge_compat_handoff_b005_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "sema_handoff": (
        ("M226-B005-SEM-01", "BuildObjc3ParserContractTopLevelCountFromDeclBuckets"),
        ("M226-B005-SEM-02", "IsObjc3ParserContractCompatibilityEdgeCaseSnapshot"),
        ("M226-B005-SEM-03", "compatibility_mode != Objc3SemaCompatibilityMode::Legacy"),
        (
            "M226-B005-SEM-04",
            "normalized_snapshot.top_level_declaration_count == 0u &&\n      top_level_count != 0u",
        ),
        ("M226-B005-SEM-05", "normalized_snapshot.ast_shape_fingerprint ="),
        ("M226-B005-SEM-06", "normalized_snapshot.ast_top_level_layout_fingerprint ="),
        ("M226-B005-SEM-07", "scaffold.parser_contract_compatibility_edge_case_detected ="),
        ("M226-B005-SEM-08", "input.compatibility_mode == Objc3SemaCompatibilityMode::Legacy &&"),
        ("M226-B005-SEM-09", "NormalizeObjc3ParserContractSnapshotForCompatibilityEdgeCases("),
        ("M226-B005-SEM-10", "scaffold.parser_contract_snapshot_compatibility_normalized"),
    ),
    "sema_contract": (
        ("M226-B005-CON-01", "enum class Objc3SemaCompatibilityMode : std::uint8_t {"),
        ("M226-B005-CON-02", "Legacy = 1,"),
        ("M226-B005-CON-03", "Objc3SemaCompatibilityMode compatibility_mode = Objc3SemaCompatibilityMode::Canonical;"),
    ),
    "contract_doc": (
        (
            "M226-B005-DOC-01",
            "Contract ID: `objc3c-parser-sema-edge-compat-handoff-contract/m226-b005-v1`",
        ),
        ("M226-B005-DOC-02", "`Objc3SemaCompatibilityMode::Legacy`"),
        ("M226-B005-DOC-03", "`top_level_declaration_count`"),
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
        default=Path("tmp/reports/m226/M226-B005/parser_sema_edge_compat_handoff_summary.json"),
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
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
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

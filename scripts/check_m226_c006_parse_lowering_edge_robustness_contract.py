#!/usr/bin/env python3
"""Fail-closed validator for M226-C006 parse/lowering edge robustness."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-c006-parse-lowering-edge-robustness-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "readiness_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_parse_lowering_readiness_surface.h",
    "frontend_types_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parse_lowering_edge_robustness_c006_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "readiness_surface_header": (
        ("M226-C006-RDY-01", "inline bool IsObjc3LanguageVersionPragmaCoordinateOrderConsistent("),
        ("M226-C006-RDY-02", "const bool first_before_or_equal_last ="),
        ("M226-C006-RDY-03", "const bool single_directive_coordinates_consistent ="),
        ("M226-C006-RDY-04", "inline std::string BuildObjc3ParseArtifactEdgeRobustnessKey("),
        ("M226-C006-RDY-05", ";token_budget_consistent=\" + (parser_token_count_budget_consistent ? \"true\" : \"false\") +"),
        ("M226-C006-RDY-06", ";pragma_coordinate_order_consistent=\" +"),
        ("M226-C006-RDY-07", ";consistent=\" + (parse_artifact_edge_case_robustness_consistent ? \"true\" : \"false\");"),
        ("M226-C006-RDY-08", "surface.parser_token_count_budget_consistent ="),
        ("M226-C006-RDY-09", "surface.parser_token_count >= parser_snapshot_breakdown_count &&"),
        ("M226-C006-RDY-10", "surface.parser_token_count >= parser_snapshot.top_level_declaration_count &&"),
        ("M226-C006-RDY-11", "surface.parser_token_count >= ast_top_level_declaration_count;"),
        ("M226-C006-RDY-12", "surface.language_version_pragma_coordinate_order_consistent ="),
        ("M226-C006-RDY-13", "surface.parse_artifact_edge_case_robustness_consistent ="),
        ("M226-C006-RDY-14", "surface.parser_token_count_budget_consistent &&"),
        ("M226-C006-RDY-15", "surface.language_version_pragma_coordinate_order_consistent &&"),
        ("M226-C006-RDY-16", "!surface.parse_artifact_handoff_key.empty() &&"),
        ("M226-C006-RDY-17", "!surface.compatibility_handoff_key.empty() &&"),
        ("M226-C006-RDY-18", "!surface.parse_artifact_replay_key.empty();"),
        ("M226-C006-RDY-19", "surface.parse_artifact_edge_robustness_key = BuildObjc3ParseArtifactEdgeRobustnessKey("),
        ("M226-C006-RDY-20", "const bool parse_artifact_replay_key_ready ="),
        ("M226-C006-RDY-21", "parse_artifact_replay_key_ready &&"),
        ("M226-C006-RDY-22", "surface.parse_artifact_edge_case_robustness_consistent;"),
        ("M226-C006-RDY-23", "surface.failure_reason = \"parser token count budget is inconsistent\";"),
        ("M226-C006-RDY-24", "surface.failure_reason = \"language-version pragma coordinate order is inconsistent\";"),
        ("M226-C006-RDY-25", "surface.failure_reason = \"parse artifact edge-case robustness is inconsistent\";"),
    ),
    "frontend_types_header": (
        ("M226-C006-TYP-01", "bool parser_token_count_budget_consistent = false;"),
        ("M226-C006-TYP-02", "bool language_version_pragma_coordinate_order_consistent = false;"),
        ("M226-C006-TYP-03", "bool parse_artifact_edge_case_robustness_consistent = false;"),
        ("M226-C006-TYP-04", "std::string parse_artifact_edge_robustness_key;"),
    ),
    "artifacts_source": (
        ("M226-C006-ART-01", '\\"parser_token_count_budget_consistent\\": '),
        ("M226-C006-ART-02", '\\"language_version_pragma_coordinate_order_consistent\\": '),
        ("M226-C006-ART-03", '\\"parse_artifact_edge_case_robustness_consistent\\": '),
        ("M226-C006-ART-04", '\\"parse_artifact_edge_robustness_key\\":\\"'),
    ),
    "contract_doc": (
        ("M226-C006-DOC-01", "Contract ID: `objc3c-parse-lowering-edge-robustness-contract/m226-c006-v1`"),
        ("M226-C006-DOC-02", "parser_token_count_budget_consistent"),
        ("M226-C006-DOC-03", "language_version_pragma_coordinate_order_consistent"),
        ("M226-C006-DOC-04", "parse_artifact_edge_case_robustness_consistent"),
        ("M226-C006-DOC-05", "parse_artifact_edge_robustness_key"),
        ("M226-C006-DOC-06", "python scripts/check_m226_c006_parse_lowering_edge_robustness_contract.py"),
        ("M226-C006-DOC-07", "python -m pytest tests/tooling/test_check_m226_c006_parse_lowering_edge_robustness_contract.py -q"),
        ("M226-C006-DOC-08", "tmp/reports/m226/m226_c006_parse_lowering_edge_robustness_contract_summary.json"),
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
        default=Path("tmp/reports/m226/m226_c006_parse_lowering_edge_robustness_contract_summary.json"),
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists():
        raise ValueError(f"{artifact} file does not exist: {path.as_posix()}")
    if not path.is_file():
        raise ValueError(f"{artifact} path is not a file: {path.as_posix()}")
    try:
        return path.read_text(encoding="utf-8")
    except UnicodeDecodeError as exc:
        raise ValueError(f"{artifact} file is not valid UTF-8: {path.as_posix()}") from exc
    except OSError as exc:
        raise ValueError(f"unable to read {artifact} file {path.as_posix()}: {exc}") from exc


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    findings: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    try:
        for artifact, path in ARTIFACTS.items():
            text = load_text(path, artifact=artifact)
            for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
                checks_total += 1
                if snippet in text:
                    checks_passed += 1
                else:
                    findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))
    except ValueError as exc:
        print(
            "m226-c006-parse-lowering-edge-robustness-contract: "
            f"error: {exc}",
            file=sys.stderr,
        )
        return 2

    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
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

    if findings:
        print(
            "m226-c006-parse-lowering-edge-robustness-contract: contract drift detected "
            f"({len(findings)} finding(s)).",
            file=sys.stderr,
        )
        for finding in findings:
            print(f"- {finding.artifact}:{finding.check_id} {finding.detail}", file=sys.stderr)
        print(f"wrote summary: {args.summary_out.as_posix()}", file=sys.stderr)
        return 1

    print("m226-c006-parse-lowering-edge-robustness-contract: OK")
    print(f"- mode={MODE}")
    print(f"- checks_passed={checks_passed}")
    print("- fail_closed=true")
    print(f"- summary={args.summary_out.as_posix()}")
    return 0


def main() -> int:
    try:
        return run(sys.argv[1:])
    except Exception as exc:  # noqa: BLE001
        print(
            "m226-c006-parse-lowering-edge-robustness-contract: "
            f"error: {exc}",
            file=sys.stderr,
        )
        return 2


if __name__ == "__main__":
    raise SystemExit(main())

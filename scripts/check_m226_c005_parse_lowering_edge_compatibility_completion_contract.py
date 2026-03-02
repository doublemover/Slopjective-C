#!/usr/bin/env python3
"""Fail-closed validator for M226-C005 parse/lowering edge compatibility completion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-c005-parse-lowering-edge-compatibility-completion-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "readiness_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_parse_lowering_readiness_surface.h",
    "frontend_types_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m226_parse_lowering_edge_compatibility_completion_c005_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "readiness_surface_header": (
        ("M226-C005-RDY-01", "std::uint64_t parser_ast_top_level_layout_fingerprint,"),
        ("M226-C005-RDY-02", "std::uint64_t ast_top_level_layout_fingerprint,"),
        ("M226-C005-RDY-03", "const std::string &compatibility_handoff_key,"),
        ("M226-C005-RDY-04", "inline bool IsObjc3LanguageVersionPragmaContractConsistent("),
        ("M226-C005-RDY-05", "inline std::string BuildObjc3CompatibilityHandoffKey("),
        (
            "M226-C005-RDY-06",
            "surface.parser_ast_top_level_layout_fingerprint = parser_snapshot.ast_top_level_layout_fingerprint;",
        ),
        ("M226-C005-RDY-07", "surface.ast_top_level_layout_fingerprint ="),
        ("M226-C005-RDY-08", "surface.parse_artifact_layout_fingerprint_consistent ="),
        ("M226-C005-RDY-09", "const bool migration_hints_consistent ="),
        ("M226-C005-RDY-10", "const bool language_version_pragma_contract_consistent ="),
        ("M226-C005-RDY-11", "surface.compatibility_handoff_consistent ="),
        ("M226-C005-RDY-12", "surface.compatibility_handoff_key = BuildObjc3CompatibilityHandoffKey("),
        ("M226-C005-RDY-13", "surface.compatibility_handoff_consistent &&"),
        ("M226-C005-RDY-14", "surface.failure_reason = \"parse artifact layout fingerprint is inconsistent\";"),
        ("M226-C005-RDY-15", "surface.failure_reason = \"compatibility handoff is inconsistent\";"),
    ),
    "frontend_types_header": (
        ("M226-C005-TYP-01", "std::uint64_t parser_ast_top_level_layout_fingerprint = 0;"),
        ("M226-C005-TYP-02", "std::uint64_t ast_top_level_layout_fingerprint = 0;"),
        ("M226-C005-TYP-03", "bool parse_artifact_layout_fingerprint_consistent = false;"),
        ("M226-C005-TYP-04", "bool compatibility_handoff_consistent = false;"),
        ("M226-C005-TYP-05", "std::string compatibility_handoff_key;"),
    ),
    "artifacts_source": (
        ("M226-C005-ART-01", '\\"parse_artifact_layout_fingerprint_consistent\\": '),
        ("M226-C005-ART-02", '\\"compatibility_handoff_consistent\\": '),
        ("M226-C005-ART-03", '\\"parser_ast_top_level_layout_fingerprint\\": '),
        ("M226-C005-ART-04", '\\"ast_top_level_layout_fingerprint\\": '),
        ("M226-C005-ART-05", '\\"compatibility_handoff_key\\":\\"'),
    ),
    "contract_doc": (
        (
            "M226-C005-DOC-01",
            "Contract ID: `objc3c-parse-lowering-edge-compatibility-completion-contract/m226-c005-v1`",
        ),
        ("M226-C005-DOC-02", "parse_artifact_layout_fingerprint_consistent"),
        ("M226-C005-DOC-03", "compatibility_handoff_consistent"),
        ("M226-C005-DOC-04", "compatibility_handoff_key"),
        (
            "M226-C005-DOC-05",
            "python scripts/check_m226_c005_parse_lowering_edge_compatibility_completion_contract.py",
        ),
        (
            "M226-C005-DOC-06",
            "python -m pytest tests/tooling/test_check_m226_c005_parse_lowering_edge_compatibility_completion_contract.py -q",
        ),
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
        default=Path("tmp/reports/m226/m226_c005_parse_lowering_edge_compatibility_completion_contract_summary.json"),
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
            "m226-c005-parse-lowering-edge-compatibility-completion-contract: "
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
            "m226-c005-parse-lowering-edge-compatibility-completion-contract: contract drift detected "
            f"({len(findings)} finding(s)).",
            file=sys.stderr,
        )
        for finding in findings:
            print(f"- {finding.artifact}:{finding.check_id} {finding.detail}", file=sys.stderr)
        print(f"wrote summary: {args.summary_out.as_posix()}", file=sys.stderr)
        return 1

    print("m226-c005-parse-lowering-edge-compatibility-completion-contract: OK")
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
            "m226-c005-parse-lowering-edge-compatibility-completion-contract: "
            f"error: {exc}",
            file=sys.stderr,
        )
        return 2


if __name__ == "__main__":
    raise SystemExit(main())

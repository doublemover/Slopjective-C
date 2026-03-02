#!/usr/bin/env python3
"""Fail-closed validator for M226-A010 parser conformance corpus expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-parser-conformance-corpus-contract-a010-v1"
FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "m226_a010_parser_conformance_corpus"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parser_conformance_corpus_expansion_expectations.md",
    "manifest": FIXTURE_ROOT / "manifest.json",
}

REQUIRED_CASES: tuple[tuple[str, str, str | None], ...] = (
    ("A010-C001", "accept_void_pointer_param.objc3", None),
    ("A010-C002", "reject_i32_pointer_param.objc3", "O3P114"),
    ("A010-C003", "reject_bool_pointer_param.objc3", "O3P114"),
    ("A010-C004", "reject_trailing_comma_param.objc3", "O3P104"),
    ("A010-C005", "reject_post_comma_nondecl.objc3", "O3P100"),
)


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
        default=Path("tmp/reports/m226/M226-A010/parser_conformance_corpus_summary.json"),
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

    contract_doc = load_text(ARTIFACTS["contract_doc"], artifact="contract_doc")
    total_checks += 1
    if "Contract ID: `objc3c-parser-conformance-corpus-contract/m226-a010-v1`" in contract_doc:
        passed_checks += 1
    else:
        findings.append(
            Finding("contract_doc", "M226-A010-DOC-01", "missing expected contract id anchor")
        )

    manifest_path = ARTIFACTS["manifest"]
    manifest_text = load_text(manifest_path, artifact="manifest")
    try:
        manifest = json.loads(manifest_text)
    except json.JSONDecodeError as exc:
        findings.append(Finding("manifest", "M226-A010-MAN-01", f"invalid JSON: {exc}"))
        manifest = {}

    total_checks += 1
    if manifest.get("contract_id") == "objc3c-parser-conformance-corpus-contract/m226-a010-v1":
        passed_checks += 1
    else:
        findings.append(Finding("manifest", "M226-A010-MAN-02", "contract_id mismatch"))

    cases = {entry.get("id"): entry for entry in manifest.get("cases", []) if isinstance(entry, dict)}
    for case_id, path_name, diag_code in REQUIRED_CASES:
        total_checks += 1
        entry = cases.get(case_id)
        if entry is None:
            findings.append(Finding("manifest", f"M226-A010-CASE-{case_id}", f"missing case {case_id}"))
            continue
        if entry.get("path") != path_name or entry.get("diagnostic_code") != diag_code:
            findings.append(
                Finding(
                    "manifest",
                    f"M226-A010-CASE-{case_id}",
                    f"unexpected mapping for {case_id}: {entry}",
                )
            )
            continue
        case_path = FIXTURE_ROOT / path_name
        total_checks += 1
        if case_path.exists() and case_path.is_file() and case_path.read_text(encoding="utf-8").strip():
            passed_checks += 2
        else:
            findings.append(Finding("fixture", f"M226-A010-FIX-{case_id}", f"missing or empty {path_name}"))

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

#!/usr/bin/env python3
"""Fail-closed validator for M250-B010 semantic stability conformance corpus expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-semantic-stability-spec-delta-closure-conformance-corpus-expansion-contract-b010-v1"

ARTIFACTS: dict[str, Path] = {
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "core_surface": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_semantic_stability_core_feature_implementation_surface.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "b009_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_semantic_stability_spec_delta_closure_conformance_matrix_b009_expectations.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_semantic_stability_spec_delta_closure_conformance_corpus_expansion_b010_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_b010_semantic_stability_spec_delta_closure_conformance_corpus_expansion_packet.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "frontend_types": (
        ("M250-B010-TYP-01", "bool conformance_corpus_consistent = false;"),
        ("M250-B010-TYP-02", "bool conformance_corpus_ready = false;"),
        ("M250-B010-TYP-03", "std::string conformance_corpus_key;"),
    ),
    "core_surface": (
        ("M250-B010-SUR-01", "const bool conformance_corpus_consistent ="),
        ("M250-B010-SUR-02", "const bool conformance_corpus_ready ="),
        ("M250-B010-SUR-03", "surface.conformance_corpus_consistent ="),
        ("M250-B010-SUR-04", "surface.conformance_corpus_ready ="),
        ("M250-B010-SUR-05", "surface.conformance_corpus_key ="),
        ("M250-B010-SUR-06", "const bool conformance_corpus_expansion_ready ="),
        ("M250-B010-SUR-07", ";conformance-corpus-consistent="),
        ("M250-B010-SUR-08", ";conformance-corpus-ready="),
        ("M250-B010-SUR-09", ";conformance-corpus-key-ready="),
        ("M250-B010-SUR-10", ";conformance-corpus-expansion-ready="),
        ("M250-B010-SUR-11", "semantic stability conformance corpus is inconsistent"),
        ("M250-B010-SUR-12", "semantic stability conformance corpus is not ready"),
    ),
    "architecture_doc": (
        ("M250-B010-ARC-01", "M250 lane-B B010 conformance corpus expansion anchors explicit semantic"),
        ("M250-B010-ARC-02", "conformance_corpus_*"),
    ),
    "b009_contract_doc": (
        (
            "M250-B010-DEP-01",
            "Contract ID: `objc3c-semantic-stability-spec-delta-closure-conformance-matrix/m250-b009-v1`",
        ),
    ),
    "contract_doc": (
        (
            "M250-B010-DOC-01",
            "Contract ID: `objc3c-semantic-stability-spec-delta-closure-conformance-corpus-expansion/m250-b010-v1`",
        ),
        ("M250-B010-DOC-02", "conformance_corpus_consistent"),
        ("M250-B010-DOC-03", "conformance_corpus_ready"),
        (
            "M250-B010-DOC-04",
            "scripts/check_m250_b010_semantic_stability_spec_delta_closure_conformance_corpus_expansion_contract.py",
        ),
        (
            "M250-B010-DOC-05",
            "tests/tooling/test_check_m250_b010_semantic_stability_spec_delta_closure_conformance_corpus_expansion_contract.py",
        ),
        ("M250-B010-DOC-06", "npm run check:objc3c:m250-b010-lane-b-readiness"),
    ),
    "packet_doc": (
        ("M250-B010-PKT-01", "Packet: `M250-B010`"),
        ("M250-B010-PKT-02", "Dependencies: `M250-B009`"),
        (
            "M250-B010-PKT-03",
            "scripts/check_m250_b010_semantic_stability_spec_delta_closure_conformance_corpus_expansion_contract.py",
        ),
    ),
    "package_json": (
        (
            "M250-B010-CFG-01",
            '"check:objc3c:m250-b010-semantic-stability-spec-delta-closure-conformance-corpus-expansion-contract"',
        ),
        (
            "M250-B010-CFG-02",
            '"test:tooling:m250-b010-semantic-stability-spec-delta-closure-conformance-corpus-expansion-contract"',
        ),
        ("M250-B010-CFG-03", '"check:objc3c:m250-b010-lane-b-readiness"'),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface": (
        ("M250-B010-FORB-01", "const bool conformance_corpus_ready = true;"),
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
        default=Path(
            "tmp/reports/m250/M250-B010/semantic_stability_spec_delta_closure_conformance_corpus_expansion_contract_summary.json"
        ),
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
        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                findings.append(Finding(artifact, check_id, f"forbidden snippet present: {snippet}"))
            else:
                passed_checks += 1

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

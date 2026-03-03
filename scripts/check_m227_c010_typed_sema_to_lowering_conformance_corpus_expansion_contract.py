#!/usr/bin/env python3
"""Fail-closed validator for M227-C010 typed sema-to-lowering conformance corpus expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-typed-sema-to-lowering-conformance-corpus-expansion-c010-v1"

ARTIFACTS: dict[str, Path] = {
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "typed_surface": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_typed_sema_to_lowering_contract_surface.h",
    "parse_readiness": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_parse_lowering_readiness_surface.h",
    "c009_contract_doc": ROOT / "docs" / "contracts" / "m227_typed_sema_to_lowering_conformance_matrix_implementation_c009_expectations.md",
    "c009_checker": ROOT / "scripts" / "check_m227_c009_typed_sema_to_lowering_conformance_matrix_implementation_contract.py",
    "c009_tooling_test": ROOT / "tests" / "tooling" / "test_check_m227_c009_typed_sema_to_lowering_conformance_matrix_implementation_contract.py",
    "c009_packet_doc": ROOT / "spec" / "planning" / "compiler" / "m227" / "m227_c009_typed_sema_to_lowering_conformance_matrix_implementation_packet.md",
    "contract_doc": ROOT / "docs" / "contracts" / "m227_typed_sema_to_lowering_conformance_corpus_expansion_c010_expectations.md",
    "packet_doc": ROOT / "spec" / "planning" / "compiler" / "m227" / "m227_c010_typed_sema_to_lowering_conformance_corpus_expansion_packet.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "frontend_types": (
        ("M227-C010-TYP-01", "bool typed_conformance_corpus_consistent = false;"),
        ("M227-C010-TYP-02", "bool typed_conformance_corpus_ready = false;"),
        ("M227-C010-TYP-03", "std::string typed_conformance_corpus_key;"),
        ("M227-C010-TYP-04", "bool typed_sema_conformance_corpus_consistent = false;"),
        ("M227-C010-TYP-05", "bool typed_sema_conformance_corpus_ready = false;"),
        ("M227-C010-TYP-06", "std::string typed_sema_conformance_corpus_key;"),
    ),
    "typed_surface": (
        ("M227-C010-SUR-01", "BuildObjc3TypedSemaToLoweringConformanceCorpusKey("),
        ("M227-C010-SUR-02", "surface.typed_conformance_corpus_consistent ="),
        ("M227-C010-SUR-03", "surface.typed_conformance_corpus_ready ="),
        ("M227-C010-SUR-04", "surface.typed_conformance_corpus_key ="),
        ("M227-C010-SUR-05", "typed_conformance_corpus_key_ready"),
        ("M227-C010-SUR-06", "typed sema-to-lowering conformance corpus is inconsistent"),
        ("M227-C010-SUR-07", "typed sema-to-lowering conformance corpus is not ready"),
        ("M227-C010-SUR-08", "typed sema-to-lowering conformance corpus key is empty"),
    ),
    "parse_readiness": (
        ("M227-C010-REA-01", "surface.typed_sema_conformance_corpus_consistent ="),
        ("M227-C010-REA-02", "surface.typed_sema_conformance_corpus_key ="),
        ("M227-C010-REA-03", "surface.typed_sema_conformance_corpus_ready ="),
        ("M227-C010-REA-04", "const bool typed_conformance_corpus_alignment ="),
        ("M227-C010-REA-05", "surface.typed_sema_conformance_corpus_ready &&"),
        ("M227-C010-REA-06", "!surface.typed_sema_conformance_corpus_key.empty() &&"),
        ("M227-C010-REA-07", "typed sema-to-lowering conformance corpus drifted from parse/lowering readiness"),
    ),
    "c009_contract_doc": (("M227-C010-DEP-01", "m227-c009-v1"),),
    "c009_checker": (("M227-C010-DEP-02", 'MODE = "m227-typed-sema-to-lowering-conformance-matrix-implementation-c009-v1"'),),
    "c009_tooling_test": (("M227-C010-DEP-03", "def test_contract_passes_on_repository_sources"),),
    "c009_packet_doc": (("M227-C010-DEP-04", "Packet: `M227-C009`"),),
    "contract_doc": (
        ("M227-C010-DOC-01", "Contract ID: `objc3c-typed-sema-to-lowering-conformance-corpus-expansion/m227-c010-v1`"),
        ("M227-C010-DOC-02", "Execute issue `#5130`"),
        ("M227-C010-DOC-03", "Dependencies: `M227-C009`"),
    ),
    "packet_doc": (
        ("M227-C010-PKT-01", "Packet: `M227-C010`"),
        ("M227-C010-PKT-02", "Issue: `#5130`"),
        ("M227-C010-PKT-03", "Dependencies: `M227-C009`"),
    ),
    "package_json": (
        ("M227-C010-PKG-01", '"check:objc3c:m227-c010-typed-sema-to-lowering-conformance-corpus-expansion-contract"'),
        ("M227-C010-PKG-02", '"test:tooling:m227-c010-typed-sema-to-lowering-conformance-corpus-expansion-contract"'),
        ("M227-C010-PKG-03", '"check:objc3c:m227-c010-lane-c-readiness"'),
        ("M227-C010-PKG-04", "scripts/check_m227_c009_typed_sema_to_lowering_conformance_matrix_implementation_contract.py"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "typed_surface": (("M227-C010-FORB-01", "surface.typed_conformance_corpus_ready = true;"),),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path("tmp/reports/m227/M227-C010/typed_sema_to_lowering_conformance_corpus_expansion_contract_summary.json"),
    )
    return parser.parse_args(argv)


def load_text(path: Path, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        text = load_text(path, artifact)
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
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(
        json.dumps(
            {
                "mode": MODE,
                "ok": ok,
                "checks_total": total_checks,
                "checks_passed": passed_checks,
                "failures": [
                    {"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail} for f in findings
                ],
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

#!/usr/bin/env python3
"""Fail-closed validator for M228-B010 ownership-aware lowering conformance corpus expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-b010-ownership-aware-lowering-behavior-conformance-corpus-expansion-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "scaffold_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_ownership_aware_lowering_behavior_scaffold.h",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "b009_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_conformance_matrix_implementation_b009_expectations.md",
    "b009_checker": ROOT
    / "scripts"
    / "check_m228_b009_ownership_aware_lowering_behavior_conformance_matrix_implementation_contract.py",
    "b009_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_b009_ownership_aware_lowering_behavior_conformance_matrix_implementation_contract.py",
    "b009_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_b009_ownership_aware_lowering_behavior_conformance_matrix_implementation_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_conformance_corpus_expansion_b010_expectations.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_b010_ownership_aware_lowering_behavior_conformance_corpus_expansion_packet.md",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M228-B010-SCA-01", "bool conformance_corpus_consistent = false;"),
        ("M228-B010-SCA-02", "bool conformance_corpus_ready = false;"),
        ("M228-B010-SCA-03", "std::string conformance_corpus_key;"),
        (
            "M228-B010-SCA-04",
            "bool parse_lowering_conformance_corpus_consistent = false;",
        ),
        ("M228-B010-SCA-05", "std::size_t parse_lowering_conformance_corpus_case_count = 0;"),
        ("M228-B010-SCA-06", "std::string parse_lowering_conformance_corpus_key;"),
        (
            "M228-B010-SCA-07",
            "BuildObjc3OwnershipAwareLoweringBehaviorConformanceCorpusKey(",
        ),
        (
            "M228-B010-SCA-08",
            "bool parse_lowering_conformance_corpus_consistent,",
        ),
        (
            "M228-B010-SCA-09",
            "std::size_t parse_lowering_conformance_corpus_case_count,",
        ),
        (
            "M228-B010-SCA-10",
            "scaffold.parse_lowering_conformance_corpus_consistent =",
        ),
        ("M228-B010-SCA-11", "scaffold.parse_lowering_conformance_corpus_case_count ="),
        ("M228-B010-SCA-12", "scaffold.parse_lowering_conformance_corpus_key ="),
        ("M228-B010-SCA-13", "scaffold.conformance_corpus_consistent ="),
        ("M228-B010-SCA-14", "scaffold.conformance_corpus_key ="),
        ("M228-B010-SCA-15", "scaffold.conformance_corpus_ready ="),
        (
            "M228-B010-SCA-16",
            "inline bool IsObjc3OwnershipAwareLoweringBehaviorConformanceCorpusReady(",
        ),
        (
            "M228-B010-SCA-17",
            "ownership-aware lowering parse conformance corpus is inconsistent",
        ),
        (
            "M228-B010-SCA-18",
            "ownership-aware lowering conformance corpus is inconsistent",
        ),
        (
            "M228-B010-SCA-19",
            "ownership-aware lowering conformance corpus is not ready",
        ),
    ),
    "artifacts_source": (
        (
            "M228-B010-ART-01",
            "IsObjc3OwnershipAwareLoweringBehaviorConformanceCorpusReady(",
        ),
        ("M228-B010-ART-02", '"O3L320"'),
        (
            "M228-B010-ART-03",
            "ownership-aware lowering conformance corpus check failed",
        ),
        (
            "M228-B010-ART-04",
            ".parse_lowering_conformance_corpus_consistent",
        ),
        (
            "M228-B010-ART-05",
            ".parse_lowering_conformance_corpus_case_count",
        ),
        (
            "M228-B010-ART-06",
            ".parse_lowering_conformance_corpus_key",
        ),
    ),
    "b009_contract_doc": (
        (
            "M228-B010-DEP-01",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-conformance-matrix-implementation/m228-b009-v1`",
        ),
    ),
    "b009_checker": (
        (
            "M228-B010-DEP-02",
            'MODE = "m228-b009-ownership-aware-lowering-behavior-conformance-matrix-implementation-contract-v1"',
        ),
    ),
    "b009_tooling_test": (
        ("M228-B010-DEP-03", "def test_contract_passes_on_repository_sources"),
    ),
    "b009_packet_doc": (
        ("M228-B010-DEP-04", "Packet: `M228-B009`"),
    ),
    "contract_doc": (
        (
            "M228-B010-DOC-01",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-conformance-corpus-expansion/m228-b010-v1`",
        ),
        ("M228-B010-DOC-02", "Dependencies: `M228-B009`"),
        (
            "M228-B010-DOC-03",
            "BuildObjc3OwnershipAwareLoweringBehaviorConformanceCorpusKey",
        ),
        (
            "M228-B010-DOC-04",
            "IsObjc3OwnershipAwareLoweringBehaviorConformanceCorpusReady",
        ),
        ("M228-B010-DOC-05", "O3L320"),
        (
            "M228-B010-DOC-06",
            "Shared-file deltas required for full lane-B readiness",
        ),
    ),
    "planning_packet": (
        ("M228-B010-PKT-01", "Packet: `M228-B010`"),
        ("M228-B010-PKT-02", "Dependencies: `M228-B009`"),
        (
            "M228-B010-PKT-03",
            "python scripts/check_m228_b010_ownership_aware_lowering_behavior_conformance_corpus_expansion_contract.py",
        ),
    ),
    "package_json": (
        (
            "M228-B010-PKG-01",
            '"check:objc3c:m228-b010-ownership-aware-lowering-behavior-conformance-corpus-expansion-contract"',
        ),
        (
            "M228-B010-PKG-02",
            '"test:tooling:m228-b010-ownership-aware-lowering-behavior-conformance-corpus-expansion-contract"',
        ),
        ("M228-B010-PKG-03", '"check:objc3c:m228-b010-lane-b-readiness"'),
    ),
    "architecture_doc": (
        (
            "M228-B010-ARC-01",
            "M228 lane-B B010 conformance corpus expansion extends",
        ),
    ),
    "lowering_spec": (
        (
            "M228-B010-SPEC-01",
            "ownership-aware lowering conformance corpus expansion shall include",
        ),
    ),
    "metadata_spec": (
        ("M228-B010-META-01", "lane-B B010"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        (
            "M228-B010-FORB-01",
            "scaffold.conformance_corpus_ready = true;",
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
        default=Path(
            "tmp/reports/m228/M228-B010/ownership_aware_lowering_behavior_conformance_corpus_expansion_contract_summary.json"
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
    checks_total = 0
    checks_passed = 0

    for artifact, path in ARTIFACTS.items():
        try:
            text = load_text(path, artifact=artifact)
        except ValueError as exc:
            checks_total += 1
            findings.append(Finding(artifact, f"M228-B010-MISS-{artifact.upper()}", str(exc)))
            continue

        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            checks_total += 1
            if snippet in text:
                checks_passed += 1
            else:
                findings.append(Finding(artifact, check_id, f"missing snippet: {snippet}"))

        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
            checks_total += 1
            if snippet in text:
                findings.append(Finding(artifact, check_id, f"forbidden snippet present: {snippet}"))
            else:
                checks_passed += 1

    ok = not findings
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail}
            for f in findings
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

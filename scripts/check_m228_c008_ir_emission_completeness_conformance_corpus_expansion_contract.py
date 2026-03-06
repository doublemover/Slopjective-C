#!/usr/bin/env python3
"""Fail-closed validator for M228-C010 IR emission conformance corpus expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-c010-ir-emission-completeness-conformance-corpus-expansion-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "core_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_ir_emission_core_feature_implementation_surface.h",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "ir_header": ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h",
    "ir_source": ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
    "c009_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ir_emission_completeness_conformance_matrix_implementation_c009_expectations.md",
    "c009_checker": ROOT
    / "scripts"
    / "check_m228_c009_ir_emission_completeness_conformance_matrix_implementation_contract.py",
    "c009_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_c009_ir_emission_completeness_conformance_matrix_implementation_contract.py",
    "c009_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_c009_ir_emission_completeness_conformance_matrix_implementation_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ir_emission_completeness_conformance_corpus_expansion_c010_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_c010_ir_emission_completeness_conformance_corpus_expansion_packet.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-C010-SUR-01", "bool pass_graph_conformance_corpus_ready = false;"),
        ("M228-C010-SUR-02", "bool parse_artifact_conformance_corpus_consistent = false;"),
        ("M228-C010-SUR-03", "bool conformance_corpus_consistent = false;"),
        ("M228-C010-SUR-04", "bool conformance_corpus_key_transport_ready = false;"),
        ("M228-C010-SUR-05", "bool core_feature_conformance_corpus_ready = false;"),
        ("M228-C010-SUR-06", "std::string pass_graph_conformance_corpus_key;"),
        ("M228-C010-SUR-07", "std::string parse_artifact_conformance_corpus_key;"),
        ("M228-C010-SUR-08", "std::string conformance_corpus_key;"),
        ("M228-C010-SUR-09", "std::string conformance_corpus_failure_reason;"),
        ("M228-C010-SUR-10", "BuildObjc3IREmissionCoreFeatureConformanceCorpusKey("),
        ("M228-C010-SUR-11", "surface.pass_graph_conformance_corpus_ready ="),
        ("M228-C010-SUR-12", "surface.parse_artifact_conformance_corpus_consistent ="),
        ("M228-C010-SUR-13", "surface.conformance_corpus_consistent ="),
        ("M228-C010-SUR-14", "surface.conformance_corpus_key_transport_ready ="),
        ("M228-C010-SUR-15", "surface.core_feature_conformance_corpus_ready ="),
        ("M228-C010-SUR-16", "surface.conformance_corpus_key ="),
        (
            "M228-C010-SUR-17",
            "IR emission core feature parse artifact conformance corpus is inconsistent",
        ),
        ("M228-C010-SUR-18", "IR emission core feature conformance corpus is inconsistent"),
        (
            "M228-C010-SUR-19",
            "IR emission core feature conformance corpus key transport is not ready",
        ),
        ("M228-C010-SUR-20", "inline bool IsObjc3IREmissionCoreFeatureConformanceCorpusReady("),
        ("M228-C010-SUR-21", "!surface.conformance_corpus_key.empty())"),
    ),
    "artifacts_source": (
        ("M228-C010-ART-01", "IsObjc3IREmissionCoreFeatureConformanceCorpusReady("),
        ("M228-C010-ART-02", '"O3L330"'),
        (
            "M228-C010-ART-03",
            "IR emission core feature conformance corpus check failed",
        ),
        (
            "M228-C010-ART-04",
            "ir_frontend_metadata.ir_emission_core_feature_conformance_corpus_ready =",
        ),
        (
            "M228-C010-ART-05",
            "ir_frontend_metadata.ir_emission_core_feature_conformance_corpus_key =",
        ),
    ),
    "ir_header": (
        ("M228-C010-IRH-01", "bool ir_emission_core_feature_conformance_corpus_ready = false;"),
        ("M228-C010-IRH-02", "std::string ir_emission_core_feature_conformance_corpus_key;"),
    ),
    "ir_source": (
        ("M228-C010-IRS-01", 'out << "; ir_emission_core_feature_conformance_corpus = "'),
        (
            "M228-C010-IRS-02",
            'out << "; ir_emission_core_feature_conformance_corpus_ready = "',
        ),
    ),
    "architecture_doc": (
        (
            "M228-C010-ARC-01",
            "M228 lane-C C010 conformance corpus expansion anchors deterministic",
        ),
        ("M228-C010-ARC-02", "(`conformance_corpus_*`)"),
    ),
    "lowering_spec": (
        (
            "M228-C010-SPC-01",
            "IR-emission conformance corpus expansion shall remain deterministic",
        ),
    ),
    "metadata_spec": (
        (
            "M228-C010-META-01",
            "deterministic IR-emission conformance corpus expansion",
        ),
    ),
    "package_json": (
        (
            "M228-C010-CFG-01",
            '"check:objc3c:m228-c009-lane-c-readiness"',
        ),
        (
            "M228-C010-CFG-02",
            '"check:objc3c:m228-c010-ir-emission-completeness-conformance-corpus-expansion-contract"',
        ),
        (
            "M228-C010-CFG-03",
            '"test:tooling:m228-c010-ir-emission-completeness-conformance-corpus-expansion-contract"',
        ),
        ("M228-C010-CFG-04", '"check:objc3c:m228-c010-lane-c-readiness"'),
        (
            "M228-C010-CFG-05",
            '"check:objc3c:m228-c010-lane-c-readiness": "npm run check:objc3c:m228-c009-lane-c-readiness &&',
        ),
        (
            "M228-C010-CFG-06",
            "scripts/check_m228_c010_ir_emission_completeness_conformance_corpus_expansion_contract.py",
        ),
        (
            "M228-C010-CFG-07",
            "tests/tooling/test_check_m228_c010_ir_emission_completeness_conformance_corpus_expansion_contract.py -q",
        ),
    ),
    "c009_contract_doc": (
        (
            "M228-C010-DEP-01",
            "Contract ID: `objc3c-ir-emission-completeness-conformance-matrix-implementation/m228-c009-v1`",
        ),
    ),
    "c009_checker": (
        (
            "M228-C010-DEP-02",
            'MODE = "m228-c009-ir-emission-completeness-conformance-matrix-implementation-contract-v1"',
        ),
    ),
    "c009_tooling_test": (
        (
            "M228-C010-DEP-03",
            "check_m228_c009_ir_emission_completeness_conformance_matrix_implementation_contract",
        ),
    ),
    "c009_packet_doc": (
        ("M228-C010-DEP-04", "Packet: `M228-C009`"),
        ("M228-C010-DEP-05", "Dependencies: `M228-C008`"),
    ),
    "contract_doc": (
        (
            "M228-C010-DOC-01",
            "Contract ID: `objc3c-ir-emission-completeness-conformance-corpus-expansion/m228-c010-v1`",
        ),
        ("M228-C010-DOC-02", "Execute issue `#5226`"),
        ("M228-C010-DOC-03", "Dependencies: `M228-C009`"),
        ("M228-C010-DOC-04", "BuildObjc3IREmissionCoreFeatureConformanceCorpusKey"),
        ("M228-C010-DOC-05", "IsObjc3IREmissionCoreFeatureConformanceCorpusReady"),
        ("M228-C010-DOC-06", "O3L330"),
        (
            "M228-C010-DOC-07",
            "scripts/check_m228_c010_ir_emission_completeness_conformance_corpus_expansion_contract.py",
        ),
        (
            "M228-C010-DOC-08",
            "tests/tooling/test_check_m228_c010_ir_emission_completeness_conformance_corpus_expansion_contract.py",
        ),
        (
            "M228-C010-DOC-09",
            "spec/planning/compiler/m228/m228_c010_ir_emission_completeness_conformance_corpus_expansion_packet.md",
        ),
        (
            "M228-C010-DOC-10",
            "tmp/reports/m228/M228-C010/ir_emission_completeness_conformance_corpus_expansion_contract_summary.json",
        ),
        ("M228-C010-DOC-11", "Shared-file deltas required for full lane-C readiness"),
    ),
    "packet_doc": (
        (
            "M228-C010-PKT-01",
            "# M228-C010 IR Emission Completeness Conformance Corpus Expansion Packet",
        ),
        ("M228-C010-PKT-02", "Packet: `M228-C010`"),
        ("M228-C010-PKT-03", "Issue: `#5226`"),
        ("M228-C010-PKT-04", "Dependencies: `M228-C009`"),
        (
            "M228-C010-PKT-05",
            "docs/contracts/m228_ir_emission_completeness_conformance_corpus_expansion_c010_expectations.md",
        ),
        (
            "M228-C010-PKT-06",
            "scripts/check_m228_c010_ir_emission_completeness_conformance_corpus_expansion_contract.py",
        ),
        (
            "M228-C010-PKT-07",
            "tests/tooling/test_check_m228_c010_ir_emission_completeness_conformance_corpus_expansion_contract.py",
        ),
        (
            "M228-C010-PKT-08",
            "tmp/reports/m228/M228-C010/ir_emission_completeness_conformance_corpus_expansion_contract_summary.json",
        ),
        ("M228-C010-PKT-09", "conformance corpus expansion"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        (
            "M228-C010-FORB-01",
            "surface.core_feature_conformance_corpus_ready = true;",
        ),
    ),
    "package_json": (
        (
            "M228-C010-FORB-02",
            '"check:objc3c:m228-c010-lane-c-readiness": "npm run check:objc3c:m228-c010-ir-emission-completeness-conformance-corpus-expansion-contract"',
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
            "tmp/reports/m228/M228-C010/"
            "ir_emission_completeness_conformance_corpus_expansion_contract_summary.json"
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

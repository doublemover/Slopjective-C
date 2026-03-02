#!/usr/bin/env python3
"""Fail-closed validator for M228-C001 IR emission completeness freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-c001-ir-emission-completeness-freeze-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "ir_header": ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h",
    "ir_source": ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "lowering_contract_header": ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h",
    "lowering_contract_source": ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
    "contract_doc": ROOT / "docs" / "contracts" / "m228_ir_emission_completeness_contract_freeze_c001_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "ir_header": (
        ("M228-C001-IRH-01", "struct Objc3IRFrontendMetadata {"),
        ("M228-C001-IRH-02", "bool lowering_pass_graph_core_feature_ready = false;"),
        ("M228-C001-IRH-03", "std::string lowering_pass_graph_core_feature_key;"),
        ("M228-C001-IRH-04", "bool lowering_pass_graph_core_feature_expansion_ready = false;"),
        ("M228-C001-IRH-05", "std::string lowering_pass_graph_core_feature_expansion_key;"),
        ("M228-C001-IRH-06", "bool EmitObjc3IRText(const Objc3Program &program,"),
    ),
    "ir_source": (
        ("M228-C001-IRS-01", 'out << "; lowering_pass_graph_core_feature = "'),
        ("M228-C001-IRS-02", 'out << "; lowering_pass_graph_core_feature_ready = "'),
        ("M228-C001-IRS-03", 'out << "; lowering_pass_graph_core_feature_expansion = "'),
        ("M228-C001-IRS-04", 'out << "; lowering_pass_graph_core_feature_expansion_ready = "'),
        ("M228-C001-IRS-05", "bool EmitObjc3IRText(const Objc3Program &program,"),
    ),
    "artifacts_source": (
        ("M228-C001-ART-01", "IsObjc3LoweringPipelinePassGraphScaffoldReady("),
        ("M228-C001-ART-02", "IsObjc3LoweringPipelinePassGraphCoreFeatureSurfaceReady("),
        ("M228-C001-ART-03", "IsObjc3LoweringPipelinePassGraphCoreFeatureExpansionReady("),
        ("M228-C001-ART-04", '"O3L301"'),
        ("M228-C001-ART-05", '"O3L302"'),
        ("M228-C001-ART-06", '"O3L303"'),
        ("M228-C001-ART-07", "ir_frontend_metadata.lowering_pass_graph_core_feature_key ="),
        ("M228-C001-ART-08", "ir_frontend_metadata.lowering_pass_graph_core_feature_expansion_key ="),
        ("M228-C001-ART-09", "if (!EmitObjc3IRText(pipeline_result.program.ast, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)) {"),
    ),
    "lowering_contract_header": (
        ("M228-C001-LOWH-01", "bool TryBuildObjc3LoweringIRBoundary("),
        ("M228-C001-LOWH-02", "std::string Objc3LoweringIRBoundaryReplayKey("),
        ("M228-C001-LOWH-03", "std::string Objc3RuntimeDispatchDeclarationReplayKey("),
    ),
    "lowering_contract_source": (
        ("M228-C001-LOWS-01", "bool TryBuildObjc3LoweringIRBoundary("),
        ("M228-C001-LOWS-02", "std::string Objc3LoweringIRBoundaryReplayKey("),
        ("M228-C001-LOWS-03", "std::string Objc3RuntimeDispatchDeclarationReplayKey("),
        ("M228-C001-LOWS-04", "out << \"declare i32 @\" << boundary.runtime_dispatch_symbol << \"(i32, ptr\";"),
    ),
    "architecture_doc": (
        ("M228-C001-ARCH-01", "M228 lane-C C001 IR emission completeness freeze anchors direct IR metadata"),
        ("M228-C001-ARCH-02", "ir/objc3_ir_emitter.cpp"),
    ),
    "lowering_spec": (
        ("M228-C001-SPC-01", "direct IR emission completeness for ObjC lowering patterns shall remain"),
    ),
    "metadata_spec": (
        ("M228-C001-META-01", "direct IR-emission completeness metadata keys for pass-graph core/expansion"),
    ),
    "package_json": (
        ("M228-C001-CFG-01", '"check:objc3c:m228-c001-ir-emission-completeness-contract"'),
        ("M228-C001-CFG-02", '"test:tooling:m228-c001-ir-emission-completeness-contract"'),
        ("M228-C001-CFG-03", '"check:objc3c:m228-c001-lane-c-readiness"'),
    ),
    "contract_doc": (
        ("M228-C001-DOC-01", "Contract ID: `objc3c-ir-emission-completeness-freeze/m228-c001-v1`"),
        ("M228-C001-DOC-02", "O3L301`/`O3L302`/`O3L303"),
        ("M228-C001-DOC-03", "Objc3RuntimeDispatchDeclarationReplayKey"),
        ("M228-C001-DOC-04", "tmp/reports/m228/M228-C001/ir_emission_completeness_contract_summary.json"),
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
        default=Path("tmp/reports/m228/M228-C001/ir_emission_completeness_contract_summary.json"),
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

    summary = {
        "mode": MODE,
        "ok": not findings,
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

    if findings:
        for finding in findings:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

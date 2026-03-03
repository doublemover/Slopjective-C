#!/usr/bin/env python3
"""Fail-closed checker for M243-C006 lowering/runtime diagnostics edge-case expansion and robustness."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-c006-lowering-runtime-diagnostics-surfacing-edge-case-expansion-and-robustness-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_c006_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_c006_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_packet.md",
    "c005_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_completion_c005_expectations.md",
    "c005_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_c005_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_completion_packet.md",
    "c005_checker": ROOT
    / "scripts"
    / "check_m243_c005_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_completion_contract.py",
    "c005_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_c005_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_completion_contract.py",
    "surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface.h",
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "frontend_pipeline": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "frontend_artifacts": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M243-C006-DOC-01",
            "Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-edge-case-expansion-and-robustness/m243-c006-v1`",
        ),
        ("M243-C006-DOC-02", "Dependencies: `M243-C005`"),
        (
            "M243-C006-DOC-03",
            "Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface",
        ),
        (
            "M243-C006-DOC-04",
            "BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface(...)",
        ),
        (
            "M243-C006-DOC-05",
            "IsObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurfaceReady(...)",
        ),
        ("M243-C006-DOC-06", "O3L324"),
        ("M243-C006-DOC-07", "npm run check:objc3c:m243-c006-lane-c-readiness"),
        (
            "M243-C006-DOC-08",
            "scripts/check_m243_c006_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_contract.py",
        ),
        (
            "M243-C006-DOC-09",
            "tests/tooling/test_check_m243_c006_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_contract.py",
        ),
    ),
    "packet_doc": (
        ("M243-C006-PKT-01", "Packet: `M243-C006`"),
        ("M243-C006-PKT-02", "Dependencies: `M243-C005`"),
        (
            "M243-C006-PKT-03",
            "scripts/check_m243_c006_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_contract.py",
        ),
        (
            "M243-C006-PKT-04",
            "tests/tooling/test_check_m243_c006_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_contract.py",
        ),
        ("M243-C006-PKT-05", "check:objc3c:m243-c006-lane-c-readiness"),
        (
            "M243-C006-PKT-06",
            "tmp/reports/m243/M243-C006/lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_contract_summary.json",
        ),
    ),
    "c005_expectations_doc": (
        (
            "M243-C006-DEP-01",
            "Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-edge-case-compatibility-completion/m243-c005-v1`",
        ),
    ),
    "c005_packet_doc": (
        ("M243-C006-DEP-02", "Packet: `M243-C005`"),
        ("M243-C006-DEP-03", "Dependencies: `M243-C004`"),
    ),
    "c005_checker": (
        (
            "M243-C006-DEP-04",
            'MODE = "m243-c005-lowering-runtime-diagnostics-surfacing-edge-case-compatibility-completion-contract-v1"',
        ),
    ),
    "c005_tooling_test": (
        (
            "M243-C006-DEP-05",
            "check_m243_c005_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_completion_contract",
        ),
    ),
    "surface_header": (
        (
            "M243-C006-SUR-01",
            "BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseRobustnessKey(",
        ),
        (
            "M243-C006-SUR-02",
            "BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface(",
        ),
        ("M243-C006-SUR-03", "surface.edge_case_compatibility_consistent ="),
        ("M243-C006-SUR-04", "surface.parse_edge_case_expansion_consistent ="),
        ("M243-C006-SUR-05", "surface.parse_edge_case_robustness_ready ="),
        ("M243-C006-SUR-06", "surface.lowering_pipeline_edge_case_robustness_ready ="),
        ("M243-C006-SUR-07", "surface.edge_case_expansion_consistent ="),
        ("M243-C006-SUR-08", "surface.edge_case_robustness_key ="),
        ("M243-C006-SUR-09", "surface.edge_case_robustness_ready ="),
        (
            "M243-C006-SUR-10",
            "IsObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurfaceReady(",
        ),
        (
            "M243-C006-SUR-11",
            "lowering/runtime diagnostics edge-case robustness is not ready",
        ),
    ),
    "frontend_types": (
        (
            "M243-C006-TYP-01",
            "struct Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface {",
        ),
        ("M243-C006-TYP-02", "bool edge_case_expansion_consistent = false;"),
        ("M243-C006-TYP-03", "bool edge_case_robustness_ready = false;"),
        ("M243-C006-TYP-04", "std::string edge_case_robustness_key;"),
        (
            "M243-C006-TYP-05",
            "lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface;",
        ),
    ),
    "frontend_pipeline": (
        (
            "M243-C006-PIP-01",
            '#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface.h"',
        ),
        (
            "M243-C006-PIP-02",
            ".lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface =",
        ),
        (
            "M243-C006-PIP-03",
            "BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface(",
        ),
    ),
    "frontend_artifacts": (
        (
            "M243-C006-ART-01",
            '#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface.h"',
        ),
        (
            "M243-C006-ART-02",
            "IsObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurfaceReady(",
        ),
        ("M243-C006-ART-03", '"O3L324"'),
        (
            "M243-C006-ART-04",
            "lowering/runtime diagnostics surfacing edge-case expansion and robustness check failed: ",
        ),
    ),
    "architecture_doc": (
        (
            "M243-C006-ARC-01",
            "M243 lane-C C006 edge-case expansion and robustness anchors lowering/runtime diagnostics surfacing",
        ),
    ),
    "lowering_spec": (
        (
            "M243-C006-SPC-01",
            "lowering/runtime diagnostics surfacing edge-case expansion and robustness shall preserve",
        ),
    ),
    "metadata_spec": (
        (
            "M243-C006-META-01",
            "expansion and robustness metadata anchors for `M243-C006` with explicit",
        ),
    ),
    "package_json": (
        (
            "M243-C006-CFG-01",
            '"check:objc3c:m243-c006-lowering-runtime-diagnostics-surfacing-edge-case-expansion-and-robustness-contract"',
        ),
        (
            "M243-C006-CFG-02",
            '"test:tooling:m243-c006-lowering-runtime-diagnostics-surfacing-edge-case-expansion-and-robustness-contract"',
        ),
        ("M243-C006-CFG-03", '"check:objc3c:m243-c006-lane-c-readiness"'),
        (
            "M243-C006-CFG-04",
            "npm run check:objc3c:m243-c005-lane-c-readiness && npm run check:objc3c:m243-c006-lowering-runtime-diagnostics-surfacing-edge-case-expansion-and-robustness-contract && npm run test:tooling:m243-c006-lowering-runtime-diagnostics-surfacing-edge-case-expansion-and-robustness-contract",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "surface_header": (
        ("M243-C006-FORB-01", "surface.edge_case_robustness_ready = true;"),
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
            "tmp/reports/m243/M243-C006/lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_contract_summary.json"
        ),
    )
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit the summary payload to stdout as canonical JSON.",
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

    if args.emit_json:
        print(canonical_json(summary), end="")

    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

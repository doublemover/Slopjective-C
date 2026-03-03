#!/usr/bin/env python3
"""Fail-closed validator for M243-C002 lowering/runtime diagnostics surfacing modular split scaffolding."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-lowering-runtime-diagnostics-surfacing-modular-split-scaffolding-contract-c002-v1"

ARTIFACTS: dict[str, Path] = {
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "scaffold_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_lowering_runtime_diagnostics_surfacing_scaffold.h",
    "frontend_pipeline_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "frontend_artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "c001_contract_doc": ROOT / "docs" / "contracts" / "m243_lowering_runtime_diagnostics_surfacing_c001_expectations.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_lowering_runtime_diagnostics_surfacing_modular_split_scaffolding_c002_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_c002_lowering_runtime_diagnostics_surfacing_modular_split_scaffolding_packet.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "frontend_types": (
        ("M243-C002-TYP-01", "struct Objc3LoweringRuntimeDiagnosticsSurfacingScaffold {"),
        ("M243-C002-TYP-02", "bool parse_diagnostics_hardening_consistent = false;"),
        ("M243-C002-TYP-03", "bool diagnostics_replay_key_ready = false;"),
        (
            "M243-C002-TYP-04",
            "Objc3LoweringRuntimeDiagnosticsSurfacingScaffold",
        ),
        (
            "M243-C002-TYP-05",
            "lowering_runtime_diagnostics_surfacing_scaffold;",
        ),
    ),
    "scaffold_header": (
        (
            "M243-C002-SCA-01",
            "inline std::string BuildObjc3LoweringRuntimeDiagnosticsSurfacingScaffoldKey(",
        ),
        (
            "M243-C002-SCA-02",
            "BuildObjc3LoweringRuntimeDiagnosticsSurfacingScaffold(",
        ),
        ("M243-C002-SCA-03", "scaffold.parse_diagnostics_hardening_consistent ="),
        ("M243-C002-SCA-04", "scaffold.diagnostics_replay_key_ready ="),
        ("M243-C002-SCA-05", "scaffold.modular_split_ready ="),
        (
            "M243-C002-SCA-06",
            "inline bool IsObjc3LoweringRuntimeDiagnosticsSurfacingScaffoldReady(",
        ),
        (
            "M243-C002-SCA-07",
            'scaffold.failure_reason = "stage diagnostics bus accounting is inconsistent";',
        ),
        (
            "M243-C002-SCA-08",
            'scaffold.failure_reason = "lowering/runtime diagnostics surfacing scaffold not ready";',
        ),
    ),
    "frontend_pipeline_source": (
        (
            "M243-C002-PIP-01",
            '#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_scaffold.h"',
        ),
        (
            "M243-C002-PIP-02",
            "result.lowering_runtime_diagnostics_surfacing_scaffold =",
        ),
        (
            "M243-C002-PIP-03",
            "BuildObjc3LoweringRuntimeDiagnosticsSurfacingScaffold(result);",
        ),
    ),
    "frontend_artifacts_source": (
        (
            "M243-C002-ART-01",
            '#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_scaffold.h"',
        ),
        (
            "M243-C002-ART-02",
            "IsObjc3LoweringRuntimeDiagnosticsSurfacingScaffoldReady(",
        ),
        (
            "M243-C002-ART-03",
            "lowering/runtime diagnostics surfacing scaffold check failed: ",
        ),
        ("M243-C002-ART-04", '"O3L301"'),
    ),
    "architecture_doc": (
        (
            "M243-C002-ARC-01",
            "M243 lane-C C002 modular split scaffolding anchors lowering/runtime diagnostics",
        ),
        (
            "M243-C002-ARC-02",
            "pipeline/objc3_lowering_runtime_diagnostics_surfacing_scaffold.h",
        ),
    ),
    "lowering_spec": (
        (
            "M243-C002-SPC-01",
            "lowering/runtime diagnostics surfacing modular split scaffolding shall",
        ),
    ),
    "metadata_spec": (
        (
            "M243-C002-META-01",
            "deterministic lane-C lowering/runtime diagnostics surfacing modular split",
        ),
        (
            "M243-C002-META-02",
            "`M243-C002` with explicit `M243-C001` dependency",
        ),
    ),
    "c001_contract_doc": (
        (
            "M243-C002-C001-01",
            "Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-freeze/m243-c001-v1`",
        ),
    ),
    "contract_doc": (
        (
            "M243-C002-DOC-01",
            "Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-modular-split-scaffolding/m243-c002-v1`",
        ),
        ("M243-C002-DOC-02", "Objc3LoweringRuntimeDiagnosticsSurfacingScaffold"),
        ("M243-C002-DOC-03", "BuildObjc3LoweringRuntimeDiagnosticsSurfacingScaffold"),
        (
            "M243-C002-DOC-04",
            "python scripts/check_m243_c002_lowering_runtime_diagnostics_surfacing_modular_split_scaffolding_contract.py",
        ),
        (
            "M243-C002-DOC-05",
            "python -m pytest tests/tooling/test_check_m243_c002_lowering_runtime_diagnostics_surfacing_modular_split_scaffolding_contract.py -q",
        ),
        ("M243-C002-DOC-06", "npm run check:objc3c:m243-c002-lane-c-readiness"),
        (
            "M243-C002-DOC-07",
            "tmp/reports/m243/M243-C002/lowering_runtime_diagnostics_surfacing_modular_split_scaffolding_contract_summary.json",
        ),
    ),
    "packet_doc": (
        ("M243-C002-PKT-01", "Packet: `M243-C002`"),
        ("M243-C002-PKT-02", "Lane: `C`"),
        ("M243-C002-PKT-03", "Dependencies: `M243-C001`"),
        (
            "M243-C002-PKT-04",
            "scripts/check_m243_c002_lowering_runtime_diagnostics_surfacing_modular_split_scaffolding_contract.py",
        ),
        (
            "M243-C002-PKT-05",
            "tests/tooling/test_check_m243_c002_lowering_runtime_diagnostics_surfacing_modular_split_scaffolding_contract.py",
        ),
    ),
    "package_json": (
        (
            "M243-C002-CFG-01",
            '"check:objc3c:m243-c002-lowering-runtime-diagnostics-surfacing-modular-split-scaffolding-contract"',
        ),
        (
            "M243-C002-CFG-02",
            '"test:tooling:m243-c002-lowering-runtime-diagnostics-surfacing-modular-split-scaffolding-contract"',
        ),
        ("M243-C002-CFG-03", '"check:objc3c:m243-c002-lane-c-readiness"'),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M243-C002-FORB-01", "scaffold.modular_split_ready = true;"),
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
            "tmp/reports/m243/M243-C002/"
            "lowering_runtime_diagnostics_surfacing_modular_split_scaffolding_contract_summary.json"
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

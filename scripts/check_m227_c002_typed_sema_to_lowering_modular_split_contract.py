#!/usr/bin/env python3
"""Fail-closed checker for M227-C002 typed sema-to-lowering modular split scaffolding."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-c002-typed-sema-to-lowering-modular-split-contract-v1"

DEFAULT_TYPED_SURFACE_HEADER = (
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_typed_sema_to_lowering_contract_surface.h"
)
DEFAULT_FRONTEND_TYPES_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
DEFAULT_PARSE_READINESS_HEADER = (
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_parse_lowering_readiness_surface.h"
)
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m227_typed_sema_to_lowering_modular_split_c002_expectations.md"
DEFAULT_PACKET_DOC = (
    ROOT / "spec" / "planning" / "compiler" / "m227" / "m227_c002_typed_sema_to_lowering_modular_split_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m227/M227-C002/typed_sema_to_lowering_modular_split_contract_summary.json")


@dataclass(frozen=True)
class AssetCheck:
    check_id: str
    relative_path: Path


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


PREREQUISITE_ASSETS: tuple[AssetCheck, ...] = (
    AssetCheck(
        "M227-C002-DEP-C001-01",
        Path("docs/contracts/m227_typed_sema_to_lowering_contract_expectations.md"),
    ),
    AssetCheck(
        "M227-C002-DEP-C001-02",
        Path("spec/planning/compiler/m227/m227_c001_typed_sema_to_lowering_contract_and_architecture_freeze_packet.md"),
    ),
    AssetCheck(
        "M227-C002-DEP-C001-03",
        Path("scripts/check_m227_c001_typed_sema_to_lowering_contract.py"),
    ),
    AssetCheck(
        "M227-C002-DEP-C001-04",
        Path("tests/tooling/test_check_m227_c001_typed_sema_to_lowering_contract.py"),
    ),
)

TYPED_SURFACE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-C002-MOD-01",
        "inline std::string BuildObjc3TypedSemaToLoweringContractHandoffKey(",
    ),
    SnippetCheck(
        "M227-C002-MOD-02",
        "inline Objc3TypedSemaToLoweringContractSurface BuildObjc3TypedSemaToLoweringContractSurface(",
    ),
    SnippetCheck(
        "M227-C002-MOD-03",
        "TryBuildObjc3LoweringIRBoundary(options.lowering, lowering_boundary, lowering_error)",
    ),
    SnippetCheck("M227-C002-MOD-04", "surface.ready_for_lowering ="),
    SnippetCheck(
        "M227-C002-MOD-05",
        "inline bool IsObjc3TypedSemaToLoweringContractSurfaceReady(",
    ),
)

FRONTEND_TYPES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M227-C002-TYP-01", "struct Objc3TypedSemaToLoweringContractSurface {"),
    SnippetCheck(
        "M227-C002-TYP-02",
        "Objc3TypedSemaToLoweringContractSurface typed_sema_to_lowering_contract_surface;",
    ),
)

PIPELINE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M227-C002-PIP-01", '#include "pipeline/objc3_typed_sema_to_lowering_contract_surface.h"'),
    SnippetCheck(
        "M227-C002-PIP-02",
        "BuildObjc3TypedSemaToLoweringContractSurface(result, options);",
    ),
)

PARSE_READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M227-C002-PR-01", '#include "pipeline/objc3_typed_sema_to_lowering_contract_surface.h"'),
    SnippetCheck(
        "M227-C002-PR-02",
        "const Objc3TypedSemaToLoweringContractSurface typed_sema_to_lowering_contract_surface =",
    ),
    SnippetCheck(
        "M227-C002-PR-03",
        "BuildObjc3TypedSemaToLoweringContractSurface(pipeline_result, options);",
    ),
    SnippetCheck(
        "M227-C002-PR-04",
        "typed_sema_to_lowering_contract_surface.ready_for_lowering &&",
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-C002-DOC-EXP-01",
        "# M227 Typed Sema-to-Lowering Modular Split Scaffolding Expectations (C002)",
    ),
    SnippetCheck(
        "M227-C002-DOC-EXP-02",
        "Contract ID: `objc3c-typed-sema-to-lowering-modular-split-scaffold/m227-c002-v1`",
    ),
    SnippetCheck(
        "M227-C002-DOC-EXP-03",
        "Issue `#5122` defines canonical lane-C modular split scaffolding scope.",
    ),
    SnippetCheck("M227-C002-DOC-EXP-04", "Dependencies: `M227-C001`"),
    SnippetCheck(
        "M227-C002-DOC-EXP-05",
        "m227_c002_typed_sema_to_lowering_modular_split_packet.md",
    ),
    SnippetCheck(
        "M227-C002-DOC-EXP-06",
        "`check:objc3c:m227-c002-lane-c-readiness`",
    ),
    SnippetCheck(
        "M227-C002-DOC-EXP-07",
        "`tmp/reports/m227/M227-C002/typed_sema_to_lowering_modular_split_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-C002-DOC-PKT-01",
        "# M227-C002 Typed Sema-to-Lowering Modular Split Packet",
    ),
    SnippetCheck("M227-C002-DOC-PKT-02", "Packet: `M227-C002`"),
    SnippetCheck("M227-C002-DOC-PKT-03", "Issue: `#5122`"),
    SnippetCheck("M227-C002-DOC-PKT-04", "Freeze date: `2026-03-03`"),
    SnippetCheck("M227-C002-DOC-PKT-05", "Dependencies: `M227-C001`"),
    SnippetCheck(
        "M227-C002-DOC-PKT-06",
        "`check:objc3c:m227-c002-typed-sema-to-lowering-modular-split-contract`",
    ),
    SnippetCheck(
        "M227-C002-DOC-PKT-07",
        "`check:objc3c:m227-c002-lane-c-readiness`",
    ),
    SnippetCheck("M227-C002-DOC-PKT-08", "`compile:objc3c`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-C002-ARCH-01",
        "M227 lane-C C002 typed sema-to-lowering modular split scaffolding anchors",
    ),
    SnippetCheck(
        "M227-C002-ARCH-02",
        "docs/contracts/m227_typed_sema_to_lowering_modular_split_c002_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-C002-SPC-01",
        "typed sema-to-lowering modular split scaffolding governance shall preserve explicit",
    ),
    SnippetCheck(
        "M227-C002-SPC-02",
        "lane-C dependency anchors (`M227-C001`) and fail closed on typed sema/lowering modular split handoff drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-C002-META-01",
        "deterministic lane-C typed sema-to-lowering modular split metadata anchors for `M227-C002`",
    ),
    SnippetCheck(
        "M227-C002-META-02",
        "explicit `M227-C001` dependency continuity so modular split handoff drift fails closed",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-C002-PKG-01",
        '"check:objc3c:m227-c002-typed-sema-to-lowering-modular-split-contract": '
        '"python scripts/check_m227_c002_typed_sema_to_lowering_modular_split_contract.py"',
    ),
    SnippetCheck(
        "M227-C002-PKG-02",
        '"test:tooling:m227-c002-typed-sema-to-lowering-modular-split-contract": '
        '"python -m pytest tests/tooling/test_check_m227_c002_typed_sema_to_lowering_modular_split_contract.py -q"',
    ),
    SnippetCheck(
        "M227-C002-PKG-03",
        '"check:objc3c:m227-c002-lane-c-readiness": '
        '"npm run check:objc3c:m227-c001-lane-c-readiness '
        '&& npm run check:objc3c:m227-c002-typed-sema-to-lowering-modular-split-contract '
        '&& npm run test:tooling:m227-c002-typed-sema-to-lowering-modular-split-contract"',
    ),
    SnippetCheck("M227-C002-PKG-04", '"compile:objc3c": '),
    SnippetCheck("M227-C002-PKG-05", '"proof:objc3c": '),
    SnippetCheck("M227-C002-PKG-06", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M227-C002-PKG-07", '"test:objc3c:perf-budget": '),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--typed-surface-header", type=Path, default=DEFAULT_TYPED_SURFACE_HEADER)
    parser.add_argument("--frontend-types-header", type=Path, default=DEFAULT_FRONTEND_TYPES_HEADER)
    parser.add_argument("--pipeline-source", type=Path, default=DEFAULT_PIPELINE_SOURCE)
    parser.add_argument("--parse-readiness-header", type=Path, default=DEFAULT_PARSE_READINESS_HEADER)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_prerequisite_assets() -> tuple[int, list[Finding]]:
    checks_total = 0
    findings: list[Finding] = []
    for asset in PREREQUISITE_ASSETS:
        checks_total += 1
        absolute = ROOT / asset.relative_path
        if not absolute.exists():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"missing prerequisite asset: {asset.relative_path.as_posix()}",
                )
            )
            continue
        if not absolute.is_file():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"prerequisite path is not a file: {asset.relative_path.as_posix()}",
                )
            )
    return checks_total, findings


def check_path_for_snippets(
    *,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    artifact_name = display_path(path)

    if not path.exists():
        findings.append(Finding(artifact_name, exists_check_id, f"required file is missing: {artifact_name}"))
        return checks_total, findings
    if not path.is_file():
        findings.append(Finding(artifact_name, exists_check_id, f"required path is not a file: {artifact_name}"))
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(
                Finding(
                    artifact_name,
                    snippet.check_id,
                    f"missing required snippet: {snippet.snippet}",
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    dep_checks, dep_failures = check_prerequisite_assets()
    checks_total += dep_checks
    failures.extend(dep_failures)

    for path, exists_check_id, snippets in (
        (args.typed_surface_header, "M227-C002-MOD-EXISTS", TYPED_SURFACE_SNIPPETS),
        (args.frontend_types_header, "M227-C002-TYP-EXISTS", FRONTEND_TYPES_SNIPPETS),
        (args.pipeline_source, "M227-C002-PIP-EXISTS", PIPELINE_SNIPPETS),
        (args.parse_readiness_header, "M227-C002-PR-EXISTS", PARSE_READINESS_SNIPPETS),
        (args.expectations_doc, "M227-C002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M227-C002-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M227-C002-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M227-C002-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M227-C002-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M227-C002-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_path_for_snippets(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [{"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail} for f in failures],
    }

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


def main() -> int:
    try:
        return run(sys.argv[1:])
    except Exception as exc:  # noqa: BLE001
        print(f"m227-c002-typed-sema-to-lowering-modular-split-contract: error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())

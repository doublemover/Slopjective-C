#!/usr/bin/env python3
"""Fail-closed contract checker for M244-B001 interop semantic/type mediation freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m244-b001-interop-semantic-contracts-type-mediation-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m244_interop_semantic_contracts_and_type_mediation_contract_freeze_b001_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m244"
    / "m244_b001_interop_semantic_contracts_and_type_mediation_contract_and_architecture_freeze_packet.md",
    "sema_contract": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h",
    "typed_surface": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_typed_sema_to_lowering_contract_surface.h",
    "parse_readiness": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_parse_lowering_readiness_surface.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

EXISTENCE_CHECK_IDS: dict[str, str] = {
    "expectations_doc": "M244-B001-DOC-EXISTS",
    "packet_doc": "M244-B001-PKT-EXISTS",
    "sema_contract": "M244-B001-SEM-EXISTS",
    "typed_surface": "M244-B001-TYP-EXISTS",
    "parse_readiness": "M244-B001-PRS-EXISTS",
    "architecture_doc": "M244-B001-ARCH-EXISTS",
    "lowering_spec": "M244-B001-SPC-EXISTS",
    "metadata_spec": "M244-B001-META-EXISTS",
    "package_json": "M244-B001-PKG-EXISTS",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M244-B001-DOC-01",
            "# M244 Interop Semantic Contracts and Type Mediation Contract and Architecture Freeze Expectations (B001)",
        ),
        (
            "M244-B001-DOC-02",
            "Contract ID: `objc3c-interop-semantic-contracts-and-type-mediation-freeze/m244-b001-v1`",
        ),
        ("M244-B001-DOC-03", "Dependencies: none"),
        (
            "M244-B001-DOC-04",
            "Deterministic anchors, dependency tokens, and fail-closed behavior are mandatory scope inputs.",
        ),
        (
            "M244-B001-DOC-05",
            "scripts/check_m244_b001_interop_semantic_contracts_and_type_mediation_contract.py",
        ),
        (
            "M244-B001-DOC-06",
            "tests/tooling/test_check_m244_b001_interop_semantic_contracts_and_type_mediation_contract.py",
        ),
        ("M244-B001-DOC-07", "`check:objc3c:m244-b001-lane-b-readiness`"),
        (
            "M244-B001-DOC-08",
            "tmp/reports/m244/M244-B001/interop_semantic_contracts_and_type_mediation_contract_summary.json",
        ),
        ("M244-B001-DOC-09", "native/objc3c/src/sema/objc3_sema_contract.h"),
        (
            "M244-B001-DOC-10",
            "native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h",
        ),
    ),
    "packet_doc": (
        (
            "M244-B001-PKT-01",
            "# M244-B001 Interop Semantic Contracts and Type Mediation Contract and Architecture Freeze Packet",
        ),
        ("M244-B001-PKT-02", "Packet: `M244-B001`"),
        ("M244-B001-PKT-03", "Freeze date: `2026-03-03`"),
        ("M244-B001-PKT-04", "Dependencies: none"),
        (
            "M244-B001-PKT-05",
            "docs/contracts/m244_interop_semantic_contracts_and_type_mediation_contract_freeze_b001_expectations.md",
        ),
        (
            "M244-B001-PKT-06",
            "scripts/check_m244_b001_interop_semantic_contracts_and_type_mediation_contract.py",
        ),
        (
            "M244-B001-PKT-07",
            "tests/tooling/test_check_m244_b001_interop_semantic_contracts_and_type_mediation_contract.py",
        ),
        ("M244-B001-PKT-08", "native/objc3c/src/sema/objc3_sema_contract.h"),
        (
            "M244-B001-PKT-09",
            "native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h",
        ),
        (
            "M244-B001-PKT-10",
            "Deterministic anchors, dependency tokens, and fail-closed behavior remain mandatory scope controls.",
        ),
        (
            "M244-B001-PKT-11",
            "`python scripts/check_m244_b001_interop_semantic_contracts_and_type_mediation_contract.py --emit-json`",
        ),
    ),
    "sema_contract": (
        ("M244-B001-SEM-01", "inline bool IsObjc3CanonicalReferenceTypeForm(ValueType type) {"),
        ("M244-B001-SEM-02", "inline bool IsObjc3CanonicalBridgeTopReferenceTypeForm(ValueType type) {"),
        ("M244-B001-SEM-03", "struct Objc3SemanticIntegrationSurface {"),
        ("M244-B001-SEM-04", "Objc3TypeAnnotationSurfaceSummary type_annotation_surface_summary;"),
        (
            "M244-B001-SEM-05",
            "Objc3ProtocolQualifiedObjectTypeSummary protocol_qualified_object_type_summary;",
        ),
        (
            "M244-B001-SEM-06",
            "bool IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff);",
        ),
    ),
    "typed_surface": (
        (
            "M244-B001-TYP-01",
            "inline Objc3TypedSemaToLoweringContractSurface BuildObjc3TypedSemaToLoweringContractSurface(",
        ),
        (
            "M244-B001-TYP-02",
            "surface.semantic_integration_surface_built = pipeline_result.integration_surface.built;",
        ),
        ("M244-B001-TYP-03", "surface.semantic_type_metadata_handoff_deterministic ="),
        ("M244-B001-TYP-04", "surface.object_pointer_type_handoff_deterministic ="),
        ("M244-B001-TYP-05", "surface.typed_handoff_key_deterministic ="),
        ("M244-B001-TYP-06", 'surface.failure_reason = "typed handoff key is not deterministic";'),
        ("M244-B001-TYP-07", "surface.ready_for_lowering = surface.typed_core_feature_consistent;"),
    ),
    "parse_readiness": (
        ("M244-B001-PRS-01", "surface.semantic_integration_surface_built ="),
        ("M244-B001-PRS-02", "surface.semantic_type_metadata_deterministic ="),
        ("M244-B001-PRS-03", "surface.object_pointer_type_handoff_deterministic ="),
        ("M244-B001-PRS-04", "surface.typed_handoff_key_deterministic ="),
        ("M244-B001-PRS-05", "surface.compatibility_handoff_consistent ="),
        ("M244-B001-PRS-06", 'surface.failure_reason = "compatibility handoff is inconsistent";'),
        (
            "M244-B001-PRS-07",
            'surface.failure_reason = "object pointer/nullability handoff is not deterministic";',
        ),
        ("M244-B001-PRS-08", "surface.ready_for_lowering = diagnostics_clear &&"),
    ),
    "architecture_doc": (
        (
            "M244-B001-ARCH-01",
            "M244 lane-B B001 interop semantic contracts and type mediation anchors explicit",
        ),
        (
            "M244-B001-ARCH-02",
            "docs/contracts/m244_interop_semantic_contracts_and_type_mediation_contract_freeze_b001_expectations.md",
        ),
        (
            "M244-B001-ARCH-03",
            "spec/planning/compiler/m244/m244_b001_interop_semantic_contracts_and_type_mediation_contract_and_architecture_freeze_packet.md",
        ),
        (
            "M244-B001-ARCH-04",
            "interop semantic/type mediation anchors and fail-closed dependency-token continuity remain frozen",
        ),
    ),
    "lowering_spec": (
        (
            "M244-B001-SPC-01",
            "interop semantic contracts and type mediation governance shall preserve",
        ),
        (
            "M244-B001-SPC-02",
            "deterministic lane-B anchors, explicit dependency tokens (`none` for `M244-B001`)",
        ),
        (
            "M244-B001-SPC-03",
            "fail closed on semantic/type mediation drift before downstream interop",
        ),
    ),
    "metadata_spec": (
        (
            "M244-B001-META-01",
            "deterministic lane-B interop semantic/type mediation metadata anchors for `M244-B001`",
        ),
        (
            "M244-B001-META-02",
            "semantic integration + typed handoff determinism evidence, explicit dependency tokens (`none`)",
        ),
    ),
    "package_json": (
        (
            "M244-B001-PKG-01",
            '"check:objc3c:m244-b001-interop-semantic-contracts-type-mediation-contract": '
            '"python scripts/check_m244_b001_interop_semantic_contracts_and_type_mediation_contract.py"',
        ),
        (
            "M244-B001-PKG-02",
            '"test:tooling:m244-b001-interop-semantic-contracts-type-mediation-contract": '
            '"python -m pytest tests/tooling/test_check_m244_b001_interop_semantic_contracts_and_type_mediation_contract.py -q"',
        ),
        (
            "M244-B001-PKG-03",
            '"check:objc3c:m244-b001-lane-b-readiness": '
            '"npm run check:objc3c:m244-b001-interop-semantic-contracts-type-mediation-contract '
            '&& npm run test:tooling:m244-b001-interop-semantic-contracts-type-mediation-contract"',
        ),
        ("M244-B001-PKG-04", '"test:objc3c:sema-pass-manager-diagnostics-bus": '),
        ("M244-B001-PKG-05", '"test:objc3c:lowering-regression": '),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "package_json": (
        (
            "M244-B001-FORB-01",
            '"check:objc3c:m244-b001-lane-b-readiness": '
            '"npm run check:objc3c:m244-b001-interop-semantic-contracts-type-mediation-contract"',
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


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path(
            "tmp/reports/m244/M244-B001/interop_semantic_contracts_and_type_mediation_contract_summary.json"
        ),
    )
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit summary JSON to stdout in addition to writing --summary-out.",
    )
    return parser.parse_args(argv)


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for artifact, path in ARTIFACTS.items():
        checks_total += 1
        if not path.exists():
            findings.append(
                Finding(
                    display_path(path),
                    EXISTENCE_CHECK_IDS[artifact],
                    f"required file is missing: {display_path(path)}",
                )
            )
            continue
        if not path.is_file():
            findings.append(
                Finding(
                    display_path(path),
                    EXISTENCE_CHECK_IDS[artifact],
                    f"required path is not a file: {display_path(path)}",
                )
            )
            continue
        checks_passed += 1
        text = path.read_text(encoding="utf-8")

        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            checks_total += 1
            if snippet in text:
                checks_passed += 1
            else:
                findings.append(
                    Finding(
                        display_path(path),
                        check_id,
                        f"missing required snippet: {snippet}",
                    )
                )

        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
            checks_total += 1
            if snippet in text:
                findings.append(
                    Finding(
                        display_path(path),
                        check_id,
                        f"forbidden snippet present: {snippet}",
                    )
                )
            else:
                checks_passed += 1

    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail}
            for f in findings
        ],
    }

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary), encoding="utf-8")

    if args.emit_json:
        print(canonical_json(summary), end="")

    if findings:
        for finding in findings:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    if not args.emit_json:
        print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))


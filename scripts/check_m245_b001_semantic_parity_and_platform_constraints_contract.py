#!/usr/bin/env python3
"""Fail-closed contract checker for M245-B001 semantic parity/platform constraints freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m245-b001-semantic-parity-platform-constraints-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m245_semantic_parity_and_platform_constraints_contract_and_architecture_freeze_b001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m245"
    / "m245_b001_semantic_parity_and_platform_constraints_contract_and_architecture_freeze_packet.md"
)
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_TYPED_SURFACE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_typed_sema_to_lowering_contract_surface.h"
DEFAULT_PARSE_READINESS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_parse_lowering_readiness_surface.h"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m245/M245-B001/semantic_parity_and_platform_constraints_contract_summary.json"
)


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-B001-DOC-EXP-01",
        "# M245 Semantic Parity and Platform Constraints Contract and Architecture Freeze Expectations (B001)",
    ),
    SnippetCheck(
        "M245-B001-DOC-EXP-02",
        "Contract ID: `objc3c-semantic-parity-platform-constraints-freeze/m245-b001-v1`",
    ),
    SnippetCheck("M245-B001-DOC-EXP-03", "Dependencies: none"),
    SnippetCheck("M245-B001-DOC-EXP-04", "`native/objc3c/src/pipeline/objc3_frontend_types.h`"),
    SnippetCheck(
        "M245-B001-DOC-EXP-05",
        '"check:objc3c:m245-b001-semantic-parity-platform-constraints-contract": '
        '"python scripts/check_m245_b001_semantic_parity_and_platform_constraints_contract.py"',
    ),
    SnippetCheck(
        "M245-B001-DOC-EXP-06",
        '"test:tooling:m245-b001-semantic-parity-platform-constraints-contract": '
        '"python -m pytest tests/tooling/test_check_m245_b001_semantic_parity_and_platform_constraints_contract.py -q"',
    ),
    SnippetCheck(
        "M245-B001-DOC-EXP-07",
        '"check:objc3c:m245-b001-lane-b-readiness": '
        '"npm run check:objc3c:m245-b001-semantic-parity-platform-constraints-contract '
        '&& npm run test:tooling:m245-b001-semantic-parity-platform-constraints-contract"',
    ),
    SnippetCheck(
        "M245-B001-DOC-EXP-08",
        "`tmp/reports/m245/M245-B001/semantic_parity_and_platform_constraints_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-B001-DOC-PKT-01",
        "# M245-B001 Semantic Parity and Platform Constraints Contract and Architecture Freeze Packet",
    ),
    SnippetCheck("M245-B001-DOC-PKT-02", "Packet: `M245-B001`"),
    SnippetCheck("M245-B001-DOC-PKT-03", "Freeze date: `2026-03-03`"),
    SnippetCheck("M245-B001-DOC-PKT-04", "Dependencies: none"),
    SnippetCheck(
        "M245-B001-DOC-PKT-05",
        "`scripts/check_m245_b001_semantic_parity_and_platform_constraints_contract.py`",
    ),
    SnippetCheck(
        "M245-B001-DOC-PKT-06",
        "`tests/tooling/test_check_m245_b001_semantic_parity_and_platform_constraints_contract.py`",
    ),
    SnippetCheck(
        "M245-B001-DOC-PKT-07",
        '`"check:objc3c:m245-b001-semantic-parity-platform-constraints-contract": '
        '"python scripts/check_m245_b001_semantic_parity_and_platform_constraints_contract.py"`',
    ),
    SnippetCheck(
        "M245-B001-DOC-PKT-08",
        '`"check:objc3c:m245-b001-lane-b-readiness": '
        '"npm run check:objc3c:m245-b001-semantic-parity-platform-constraints-contract '
        '&& npm run test:tooling:m245-b001-semantic-parity-platform-constraints-contract"`',
    ),
)

FRONTEND_TYPES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-B001-FRT-01", "struct Objc3TypedSemaToLoweringContractSurface {"),
    SnippetCheck("M245-B001-FRT-02", "bool sema_parity_surface_ready = false;"),
    SnippetCheck("M245-B001-FRT-03", "bool sema_parity_surface_deterministic = false;"),
    SnippetCheck("M245-B001-FRT-04", "bool runtime_dispatch_contract_consistent = false;"),
    SnippetCheck("M245-B001-FRT-05", "bool compatibility_handoff_consistent = false;"),
    SnippetCheck("M245-B001-FRT-06", "bool long_tail_grammar_compatibility_handoff_ready = false;"),
)

TYPED_SURFACE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-B001-TYP-01", "surface.sema_parity_surface_deterministic ="),
    SnippetCheck("M245-B001-TYP-02", "surface.runtime_dispatch_contract_consistent ="),
    SnippetCheck(
        "M245-B001-TYP-03",
        "lowering_boundary.runtime_dispatch_arg_slots >= kObjc3RuntimeDispatchDefaultArgs &&",
    ),
    SnippetCheck(
        "M245-B001-TYP-04",
        "lowering_boundary.runtime_dispatch_arg_slots <= kObjc3RuntimeDispatchMaxArgs &&",
    ),
    SnippetCheck("M245-B001-TYP-05", 'surface.failure_reason = "semantic parity surface is not deterministic";'),
    SnippetCheck("M245-B001-TYP-06", 'surface.failure_reason = "runtime dispatch contract is inconsistent";'),
    SnippetCheck("M245-B001-TYP-07", "surface.ready_for_lowering = surface.typed_core_feature_consistent;"),
)

TYPED_SURFACE_FORBIDDEN_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-B001-TYP-FORB-01", "surface.runtime_dispatch_contract_consistent = true;"),
)

PARSE_READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-B001-PRS-01", "surface.compatibility_handoff_consistent ="),
    SnippetCheck(
        "M245-B001-PRS-02",
        "surface.compatibility_handoff_key = BuildObjc3CompatibilityHandoffKey(",
    ),
    SnippetCheck(
        "M245-B001-PRS-03",
        "surface.long_tail_grammar_compatibility_handoff_ready =",
    ),
    SnippetCheck("M245-B001-PRS-04", 'surface.failure_reason = "compatibility handoff is inconsistent";'),
    SnippetCheck(
        "M245-B001-PRS-05",
        'surface.failure_reason = "long-tail grammar compatibility handoff is not ready";',
    ),
    SnippetCheck(
        "M245-B001-PRS-06",
        'surface.failure_reason = "toolchain/runtime GA operations recovery/determinism hardening is inconsistent";',
    ),
    SnippetCheck("M245-B001-PRS-07", "surface.ready_for_lowering = diagnostics_clear &&"),
)

PARSE_READINESS_FORBIDDEN_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-B001-PRS-FORB-01", "surface.compatibility_handoff_consistent = true;"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-B001-ARC-01",
        "`pipeline/objc3_parse_lowering_readiness_surface.h` so compatibility handoff,",
    ),
    SnippetCheck(
        "M245-B001-ARC-02",
        "typed sema handoff and parse/lowering readiness surfaces stay split while",
    ),
    SnippetCheck(
        "M245-B001-ARC-03",
        "typed semantic case accounting and parse conformance accounting remain",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-B001-SPC-01",
        "semantic/lowering test architecture governance shall preserve explicit lane-B",
    ),
    SnippetCheck(
        "M245-B001-SPC-02",
        "semantic compatibility and migration checks governance shall preserve explicit",
    ),
    SnippetCheck(
        "M245-B001-SPC-03",
        "lane-B compatibility-mode and migration-assist handoff anchors and fail",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-B001-MET-01",
        "deterministic lane-B semantic/lowering metadata anchors for `M248-B001`",
    ),
    SnippetCheck(
        "M245-B001-MET-02",
        "deterministic lane-B semantic compatibility/migration metadata anchors for `M249-B001`",
    ),
    SnippetCheck(
        "M245-B001-MET-03",
        "with compile-route evidence and perf-budget continuity so platform",
    ),
    SnippetCheck("M245-B001-MET-04", "operation drift fails closed."),
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
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--typed-surface", type=Path, default=DEFAULT_TYPED_SURFACE)
    parser.add_argument("--parse-readiness", type=Path, default=DEFAULT_PARSE_READINESS)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_doc_contract(
    *,
    path: Path,
    exists_check_id: str,
    required_snippets: tuple[SnippetCheck, ...],
    forbidden_snippets: tuple[SnippetCheck, ...] = (),
) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(
            Finding(
                display_path(path),
                exists_check_id,
                f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings
    if not path.is_file():
        findings.append(
            Finding(
                display_path(path),
                exists_check_id,
                f"required path is not a file: {display_path(path)}",
            )
        )
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
    for snippet in required_snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(
                Finding(
                    display_path(path),
                    snippet.check_id,
                    f"missing required snippet: {snippet.snippet}",
                )
            )
    for snippet in forbidden_snippets:
        checks_total += 1
        if snippet.snippet in text:
            findings.append(
                Finding(
                    display_path(path),
                    snippet.check_id,
                    f"forbidden snippet present: {snippet.snippet}",
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, required_snippets, forbidden_snippets in (
        (args.expectations_doc, "M245-B001-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS, ()),
        (args.packet_doc, "M245-B001-DOC-PKT-EXISTS", PACKET_SNIPPETS, ()),
        (args.frontend_types, "M245-B001-FRT-EXISTS", FRONTEND_TYPES_SNIPPETS, ()),
        (
            args.typed_surface,
            "M245-B001-TYP-EXISTS",
            TYPED_SURFACE_SNIPPETS,
            TYPED_SURFACE_FORBIDDEN_SNIPPETS,
        ),
        (
            args.parse_readiness,
            "M245-B001-PRS-EXISTS",
            PARSE_READINESS_SNIPPETS,
            PARSE_READINESS_FORBIDDEN_SNIPPETS,
        ),
        (args.architecture_doc, "M245-B001-ARC-EXISTS", ARCHITECTURE_SNIPPETS, ()),
        (args.lowering_spec, "M245-B001-SPC-EXISTS", LOWERING_SPEC_SNIPPETS, ()),
        (args.metadata_spec, "M245-B001-MET-EXISTS", METADATA_SPEC_SNIPPETS, ()),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            required_snippets=required_snippets,
            forbidden_snippets=forbidden_snippets,
        )
        checks_total += count
        failures.extend(findings)

    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail}
            for f in failures
        ],
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


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

#!/usr/bin/env python3
"""Fail-closed checker for M245-B002 semantic parity/platform constraints modular split scaffolding."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m245-b002-semantic-parity-platform-constraints-modular-split-scaffolding-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m245_semantic_parity_and_platform_constraints_modular_split_scaffolding_b002_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m245"
    / "m245_b002_semantic_parity_and_platform_constraints_modular_split_scaffolding_packet.md"
)
DEFAULT_B001_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m245_semantic_parity_and_platform_constraints_contract_and_architecture_freeze_b001_expectations.md"
)
DEFAULT_B001_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m245"
    / "m245_b001_semantic_parity_and_platform_constraints_contract_and_architecture_freeze_packet.md"
)
DEFAULT_B001_CHECKER = ROOT / "scripts" / "check_m245_b001_semantic_parity_and_platform_constraints_contract.py"
DEFAULT_B001_TEST = (
    ROOT / "tests" / "tooling" / "test_check_m245_b001_semantic_parity_and_platform_constraints_contract.py"
)
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_TYPED_SURFACE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_typed_sema_to_lowering_contract_surface.h"
DEFAULT_PARSE_READINESS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_parse_lowering_readiness_surface.h"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m245/M245-B002/semantic_parity_and_platform_constraints_modular_split_scaffolding_contract_summary.json"
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
        "M245-B002-DOC-EXP-01",
        "# M245 Semantic Parity and Platform Constraints Modular Split/Scaffolding Expectations (B002)",
    ),
    SnippetCheck(
        "M245-B002-DOC-EXP-02",
        "Contract ID: `objc3c-semantic-parity-platform-constraints-modular-split-scaffolding/m245-b002-v1`",
    ),
    SnippetCheck("M245-B002-DOC-EXP-03", "Dependencies: `M245-B001`"),
    SnippetCheck(
        "M245-B002-DOC-EXP-04",
        "scripts/check_m245_b001_semantic_parity_and_platform_constraints_contract.py",
    ),
    SnippetCheck(
        "M245-B002-DOC-EXP-05",
        "code anchors and milestone",
    ),
    SnippetCheck(
        "M245-B002-DOC-EXP-06",
        "optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M245-B002-DOC-EXP-07",
        "`check:objc3c:m245-b002-lane-b-readiness`",
    ),
    SnippetCheck(
        "M245-B002-DOC-EXP-08",
        "`test:objc3c:sema-pass-manager-diagnostics-bus`",
    ),
    SnippetCheck(
        "M245-B002-DOC-EXP-09",
        "`test:objc3c:lowering-regression`",
    ),
    SnippetCheck(
        "M245-B002-DOC-EXP-10",
        "`tmp/reports/m245/M245-B002/semantic_parity_and_platform_constraints_modular_split_scaffolding_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-B002-DOC-PKT-01",
        "# M245-B002 Semantic Parity and Platform Constraints Modular Split/Scaffolding Packet",
    ),
    SnippetCheck("M245-B002-DOC-PKT-02", "Packet: `M245-B002`"),
    SnippetCheck("M245-B002-DOC-PKT-03", "Dependencies: `M245-B001`"),
    SnippetCheck(
        "M245-B002-DOC-PKT-04",
        "scripts/check_m245_b001_semantic_parity_and_platform_constraints_contract.py",
    ),
    SnippetCheck(
        "M245-B002-DOC-PKT-05",
        "`native/objc3c/src/pipeline/objc3_frontend_types.h`",
    ),
    SnippetCheck(
        "M245-B002-DOC-PKT-06",
        "`check:objc3c:m245-b002-lane-b-readiness`",
    ),
    SnippetCheck(
        "M245-B002-DOC-PKT-07",
        "`test:objc3c:lowering-regression`",
    ),
    SnippetCheck(
        "M245-B002-DOC-PKT-08",
        "typed sema handoff and",
    ),
    SnippetCheck(
        "M245-B002-DOC-PKT-09",
        "parse/lowering compatibility gating remain deterministic and fail-closed",
    ),
)

B001_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-B002-B001-DOC-01",
        "# M245 Semantic Parity and Platform Constraints Contract and Architecture Freeze Expectations (B001)",
    ),
    SnippetCheck(
        "M245-B002-B001-DOC-02",
        "Contract ID: `objc3c-semantic-parity-platform-constraints-freeze/m245-b001-v1`",
    ),
    SnippetCheck("M245-B002-B001-DOC-03", "Dependencies: none"),
    SnippetCheck(
        "M245-B002-B001-DOC-04",
        '"check:objc3c:m245-b001-lane-b-readiness":',
    ),
)

B001_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-B002-B001-PKT-01", "Packet: `M245-B001`"),
    SnippetCheck("M245-B002-B001-PKT-02", "Dependencies: none"),
    SnippetCheck(
        "M245-B002-B001-PKT-03",
        "`tmp/reports/m245/M245-B001/semantic_parity_and_platform_constraints_contract_summary.json`",
    ),
)

FRONTEND_TYPES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-B002-FRT-01", "struct Objc3TypedSemaToLoweringContractSurface {"),
    SnippetCheck("M245-B002-FRT-02", "bool sema_parity_surface_ready = false;"),
    SnippetCheck("M245-B002-FRT-03", "bool sema_parity_surface_deterministic = false;"),
    SnippetCheck("M245-B002-FRT-04", "bool runtime_dispatch_contract_consistent = false;"),
    SnippetCheck("M245-B002-FRT-05", "bool compatibility_handoff_consistent = false;"),
    SnippetCheck("M245-B002-FRT-06", "bool long_tail_grammar_compatibility_handoff_ready = false;"),
)

TYPED_SURFACE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-B002-TYP-01", "surface.sema_parity_surface_deterministic ="),
    SnippetCheck("M245-B002-TYP-02", "surface.runtime_dispatch_contract_consistent ="),
    SnippetCheck(
        "M245-B002-TYP-03",
        "lowering_boundary.runtime_dispatch_arg_slots >= kObjc3RuntimeDispatchDefaultArgs &&",
    ),
    SnippetCheck(
        "M245-B002-TYP-04",
        "lowering_boundary.runtime_dispatch_arg_slots <= kObjc3RuntimeDispatchMaxArgs &&",
    ),
    SnippetCheck("M245-B002-TYP-05", 'surface.failure_reason = "semantic parity surface is not deterministic";'),
    SnippetCheck("M245-B002-TYP-06", 'surface.failure_reason = "runtime dispatch contract is inconsistent";'),
    SnippetCheck("M245-B002-TYP-07", "surface.ready_for_lowering = surface.typed_core_feature_consistent;"),
)

TYPED_SURFACE_FORBIDDEN_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-B002-TYP-FORB-01", "surface.runtime_dispatch_contract_consistent = true;"),
)

PARSE_READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-B002-PRS-01", "surface.compatibility_handoff_consistent ="),
    SnippetCheck(
        "M245-B002-PRS-02",
        "surface.compatibility_handoff_key = BuildObjc3CompatibilityHandoffKey(",
    ),
    SnippetCheck(
        "M245-B002-PRS-03",
        "surface.long_tail_grammar_compatibility_handoff_ready =",
    ),
    SnippetCheck("M245-B002-PRS-04", 'surface.failure_reason = "compatibility handoff is inconsistent";'),
    SnippetCheck(
        "M245-B002-PRS-05",
        'surface.failure_reason = "long-tail grammar compatibility handoff is not ready";',
    ),
    SnippetCheck(
        "M245-B002-PRS-06",
        'surface.failure_reason = "toolchain/runtime GA operations recovery/determinism hardening is inconsistent";',
    ),
    SnippetCheck("M245-B002-PRS-07", "surface.ready_for_lowering = diagnostics_clear &&"),
)

PARSE_READINESS_FORBIDDEN_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-B002-PRS-FORB-01", "surface.compatibility_handoff_consistent = true;"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-B002-PKG-01",
        '"check:objc3c:m245-b002-semantic-parity-platform-constraints-modular-split-scaffolding-contract": '
        '"python scripts/check_m245_b002_semantic_parity_and_platform_constraints_modular_split_scaffolding_contract.py"',
    ),
    SnippetCheck(
        "M245-B002-PKG-02",
        '"test:tooling:m245-b002-semantic-parity-platform-constraints-modular-split-scaffolding-contract": '
        '"python -m pytest tests/tooling/test_check_m245_b002_semantic_parity_and_platform_constraints_modular_split_scaffolding_contract.py -q"',
    ),
    SnippetCheck(
        "M245-B002-PKG-03",
        '"check:objc3c:m245-b002-lane-b-readiness": '
        '"npm run check:objc3c:m245-b001-lane-b-readiness '
        '&& npm run check:objc3c:m245-b002-semantic-parity-platform-constraints-modular-split-scaffolding-contract '
        '&& npm run test:tooling:m245-b002-semantic-parity-platform-constraints-modular-split-scaffolding-contract"',
    ),
    SnippetCheck("M245-B002-PKG-04", '"test:objc3c:sema-pass-manager-diagnostics-bus": '),
    SnippetCheck("M245-B002-PKG-05", '"test:objc3c:lowering-regression": '),
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
    parser.add_argument("--b001-expectations-doc", type=Path, default=DEFAULT_B001_EXPECTATIONS_DOC)
    parser.add_argument("--b001-packet-doc", type=Path, default=DEFAULT_B001_PACKET_DOC)
    parser.add_argument("--b001-checker", type=Path, default=DEFAULT_B001_CHECKER)
    parser.add_argument("--b001-test", type=Path, default=DEFAULT_B001_TEST)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--typed-surface", type=Path, default=DEFAULT_TYPED_SURFACE)
    parser.add_argument("--parse-readiness", type=Path, default=DEFAULT_PARSE_READINESS)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
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
        (args.expectations_doc, "M245-B002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS, ()),
        (args.packet_doc, "M245-B002-DOC-PKT-EXISTS", PACKET_SNIPPETS, ()),
        (args.b001_expectations_doc, "M245-B002-B001-DOC-EXISTS", B001_EXPECTATIONS_SNIPPETS, ()),
        (args.b001_packet_doc, "M245-B002-B001-PKT-EXISTS", B001_PACKET_SNIPPETS, ()),
        (args.frontend_types, "M245-B002-FRT-EXISTS", FRONTEND_TYPES_SNIPPETS, ()),
        (
            args.typed_surface,
            "M245-B002-TYP-EXISTS",
            TYPED_SURFACE_SNIPPETS,
            TYPED_SURFACE_FORBIDDEN_SNIPPETS,
        ),
        (
            args.parse_readiness,
            "M245-B002-PRS-EXISTS",
            PARSE_READINESS_SNIPPETS,
            PARSE_READINESS_FORBIDDEN_SNIPPETS,
        ),
        (args.package_json, "M245-B002-PKG-EXISTS", PACKAGE_SNIPPETS, ()),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            required_snippets=required_snippets,
            forbidden_snippets=forbidden_snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.b001_checker, "M245-B002-DEP-B001-ARG-01"),
        (args.b001_test, "M245-B002-DEP-B001-ARG-02"),
    ):
        checks_total += 1
        if not path.exists():
            failures.append(
                Finding(
                    display_path(path),
                    check_id,
                    f"required dependency path is missing: {display_path(path)}",
                )
            )
            continue
        if not path.is_file():
            failures.append(
                Finding(
                    display_path(path),
                    check_id,
                    f"required dependency path is not a file: {display_path(path)}",
                )
            )

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

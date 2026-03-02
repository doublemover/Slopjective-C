#!/usr/bin/env python3
"""Fail-closed validator for M227-C002 typed sema-to-lowering modular split scaffold contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-c002-typed-sema-to-lowering-modular-split-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "typed_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_typed_sema_to_lowering_contract_surface.h",
    "frontend_types_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "pipeline_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "parse_readiness_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_parse_lowering_readiness_surface.h",
    "contract_doc": ROOT / "docs" / "contracts" / "m227_typed_sema_to_lowering_modular_split_c002_expectations.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "typed_surface_header": (
        (
            "M227-C002-MOD-01",
            "inline std::string BuildObjc3TypedSemaToLoweringContractHandoffKey(",
        ),
        (
            "M227-C002-MOD-02",
            "inline Objc3TypedSemaToLoweringContractSurface BuildObjc3TypedSemaToLoweringContractSurface(",
        ),
        (
            "M227-C002-MOD-03",
            "TryBuildObjc3LoweringIRBoundary(options.lowering, lowering_boundary, lowering_error)",
        ),
        ("M227-C002-MOD-04", "surface.ready_for_lowering ="),
        (
            "M227-C002-MOD-05",
            "inline bool IsObjc3TypedSemaToLoweringContractSurfaceReady(",
        ),
    ),
    "frontend_types_header": (
        ("M227-C002-TYP-01", "struct Objc3TypedSemaToLoweringContractSurface {"),
        (
            "M227-C002-TYP-02",
            "Objc3TypedSemaToLoweringContractSurface typed_sema_to_lowering_contract_surface;",
        ),
    ),
    "pipeline_source": (
        ("M227-C002-PIP-01", '#include "pipeline/objc3_typed_sema_to_lowering_contract_surface.h"'),
        (
            "M227-C002-PIP-02",
            "BuildObjc3TypedSemaToLoweringContractSurface(result, options);",
        ),
    ),
    "parse_readiness_header": (
        ("M227-C002-PR-01", '#include "pipeline/objc3_typed_sema_to_lowering_contract_surface.h"'),
        (
            "M227-C002-PR-02",
            "const Objc3TypedSemaToLoweringContractSurface typed_sema_to_lowering_contract_surface =",
        ),
        (
            "M227-C002-PR-03",
            "BuildObjc3TypedSemaToLoweringContractSurface(pipeline_result, options);",
        ),
        (
            "M227-C002-PR-04",
            "typed_sema_to_lowering_contract_surface.ready_for_lowering;",
        ),
    ),
    "contract_doc": (
        (
            "M227-C002-DOC-01",
            "Contract ID: `objc3c-typed-sema-to-lowering-modular-split-scaffold/m227-c002-v1`",
        ),
        ("M227-C002-DOC-02", "objc3_typed_sema_to_lowering_contract_surface.h"),
        (
            "M227-C002-DOC-03",
            "python scripts/check_m227_c002_typed_sema_to_lowering_modular_split_contract.py",
        ),
        (
            "M227-C002-DOC-04",
            "python -m pytest tests/tooling/test_check_m227_c002_typed_sema_to_lowering_modular_split_contract.py -q",
        ),
        (
            "M227-C002-DOC-05",
            "tmp/reports/m227/m227_c002_typed_sema_to_lowering_modular_split_contract_summary.json",
        ),
    ),
    "package_json": (
        (
            "M227-C002-PKG-01",
            '"check:objc3c:m227-c002-typed-sema-to-lowering-modular-split-contract": '
            '"python scripts/check_m227_c002_typed_sema_to_lowering_modular_split_contract.py"',
        ),
        (
            "M227-C002-PKG-02",
            '"test:tooling:m227-c002-typed-sema-to-lowering-modular-split-contract": '
            '"python -m pytest tests/tooling/test_check_m227_c002_typed_sema_to_lowering_modular_split_contract.py -q"',
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
        default=Path("tmp/reports/m227/m227_c002_typed_sema_to_lowering_modular_split_contract_summary.json"),
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists():
        raise ValueError(f"{artifact} file does not exist: {path.as_posix()}")
    if not path.is_file():
        raise ValueError(f"{artifact} path is not a file: {path.as_posix()}")
    try:
        return path.read_text(encoding="utf-8")
    except UnicodeDecodeError as exc:
        raise ValueError(f"{artifact} file is not valid UTF-8: {path.as_posix()}") from exc
    except OSError as exc:
        raise ValueError(f"unable to read {artifact}: {path.as_posix()}: {exc}") from exc


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    findings: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    try:
        for artifact, path in ARTIFACTS.items():
            text = load_text(path, artifact=artifact)
            for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
                checks_total += 1
                if snippet in text:
                    checks_passed += 1
                else:
                    findings.append(
                        Finding(
                            artifact=artifact,
                            check_id=check_id,
                            detail=f"expected snippet missing: {snippet}",
                        )
                    )
    except ValueError as exc:
        print(f"m227-c002-typed-sema-to-lowering-modular-split-contract: error: {exc}", file=sys.stderr)
        return 2

    findings = sorted(findings, key=lambda f: (f.artifact, f.check_id, f.detail))
    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
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

    if not findings:
        print("m227-c002-typed-sema-to-lowering-modular-split-contract: OK")
        print(f"- mode={MODE}")
        print(f"- checks_passed={checks_passed}")
        print("- fail_closed=true")
        print(f"- summary={args.summary_out.as_posix()}")
        return 0

    print(
        "m227-c002-typed-sema-to-lowering-modular-split-contract: contract drift detected "
        f"({len(findings)} finding(s)).",
        file=sys.stderr,
    )
    for finding in findings:
        print(f"- [{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    print(f"wrote summary: {args.summary_out.as_posix()}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

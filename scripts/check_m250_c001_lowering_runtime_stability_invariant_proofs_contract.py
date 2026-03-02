#!/usr/bin/env python3
"""Fail-closed validator for M250-C001 lowering/runtime stability and invariant proofs freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-lowering-runtime-stability-invariant-proofs-freeze-contract-c001-v1"

ARTIFACTS: dict[str, Path] = {
    "lowering_contract_header": ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h",
    "lowering_contract_source": ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp",
    "typed_handoff_surface": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_typed_sema_to_lowering_contract_surface.h",
    "parse_lowering_readiness_surface": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_parse_lowering_readiness_surface.h",
    "ir_emitter_source": ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp",
    "semantics_doc": ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md",
    "artifacts_doc": ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_lowering_runtime_stability_invariant_proofs_c001_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "lowering_contract_header": (
        ("M250-C001-LOWH-01", "inline constexpr std::size_t kObjc3RuntimeDispatchDefaultArgs = 4;"),
        ("M250-C001-LOWH-02", "inline constexpr std::size_t kObjc3RuntimeDispatchMaxArgs = 16;"),
        ("M250-C001-LOWH-03", 'inline constexpr const char *kObjc3RuntimeDispatchSymbol = "objc3_msgsend_i32";'),
        ("M250-C001-LOWH-04", 'inline constexpr const char *kObjc3SelectorGlobalOrdering = "lexicographic";'),
        ("M250-C001-LOWH-05", "struct Objc3LoweringIRBoundary {"),
        ("M250-C001-LOWH-06", "std::string selector_global_ordering = kObjc3SelectorGlobalOrdering;"),
        ("M250-C001-LOWH-07", "bool TryNormalizeObjc3LoweringContract(const Objc3LoweringContract &input,"),
        ("M250-C001-LOWH-08", "bool TryBuildObjc3LoweringIRBoundary(const Objc3LoweringContract &input,"),
        ("M250-C001-LOWH-09", "std::string Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary);"),
    ),
    "lowering_contract_source": (
        (
            "M250-C001-LOWS-01",
            "invalid lowering contract runtime_dispatch_symbol (expected [A-Za-z_.$][A-Za-z0-9_.$]*): ",
        ),
        ("M250-C001-LOWS-02", "if (!TryNormalizeObjc3LoweringContract(input, normalized, error)) {"),
        ("M250-C001-LOWS-03", "boundary.selector_global_ordering = kObjc3SelectorGlobalOrdering;"),
        ("M250-C001-LOWS-04", 'return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +'),
    ),
    "typed_handoff_surface": (
        ("M250-C001-SUR-01", "if (TryBuildObjc3LoweringIRBoundary(options.lowering, lowering_boundary, lowering_error)) {"),
        ("M250-C001-SUR-02", "surface.lowering_boundary_replay_key = Objc3LoweringIRBoundaryReplayKey(lowering_boundary);"),
        ("M250-C001-SUR-03", "surface.runtime_dispatch_contract_consistent ="),
        ("M250-C001-SUR-04", "lowering_boundary.runtime_dispatch_arg_slots >= kObjc3RuntimeDispatchDefaultArgs &&"),
        ("M250-C001-SUR-05", "lowering_boundary.runtime_dispatch_arg_slots <= kObjc3RuntimeDispatchMaxArgs &&"),
        ("M250-C001-SUR-06", "lowering_boundary.selector_global_ordering == kObjc3SelectorGlobalOrdering;"),
        ("M250-C001-SUR-07", "surface.typed_handoff_key_deterministic ="),
        ("M250-C001-SUR-08", 'surface.failure_reason = "runtime dispatch contract is inconsistent";'),
        ("M250-C001-SUR-09", 'surface.failure_reason = "typed handoff key is not deterministic";'),
    ),
    "parse_lowering_readiness_surface": (
        ("M250-C001-REA-01", "surface.typed_handoff_key_deterministic ="),
        ("M250-C001-REA-02", "surface.typed_sema_core_feature_consistent ="),
        ("M250-C001-REA-03", "surface.typed_sema_core_feature_key ="),
        ("M250-C001-REA-04", "const bool typed_core_feature_ready ="),
        ("M250-C001-REA-05", 'surface.failure_reason = "typed sema-to-lowering handoff key is not deterministic";'),
        ("M250-C001-REA-06", 'surface.failure_reason = "typed sema-to-lowering core feature expansion is inconsistent";'),
    ),
    "ir_emitter_source": (
        (
            "M250-C001-IR-01",
            "if (!TryBuildObjc3LoweringIRBoundary(lowering_contract, lowering_ir_boundary_, boundary_error_)) {",
        ),
        ("M250-C001-IR-02", 'out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\\n";'),
        ("M250-C001-IR-03", "if (expr->args.size() > lowering_ir_boundary_.runtime_dispatch_arg_slots) {"),
        ("M250-C001-IR-04", "runtime_dispatch_call_emitted_ = true;"),
        ("M250-C001-IR-05", "ctx.global_proofs_invalidated = true;"),
    ),
    "semantics_doc": (
        ("M250-C001-SEM-01", "Deterministic dispatch ABI marshalling invariants (fail-closed):"),
        (
            "M250-C001-SEM-02",
            "integration and handoff dispatch ABI marshalling summaries remain parity-equivalent before release gating passes.",
        ),
        ("M250-C001-SEM-03", "`result.parity_surface.deterministic_dispatch_abi_marshalling_handoff`"),
    ),
    "artifacts_doc": (
        ("M250-C001-ART-01", "## M206 lowering/runtime canonical optimization pipeline stage-1"),
        (
            "M250-C001-ART-02",
            "tmp/reports/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/canonical-optimization-source-anchors.txt",
        ),
        ("M250-C001-ART-03", "`runtime_dispatch_call_emitted_ = true;`"),
        ("M250-C001-ART-04", "`ctx.global_proofs_invalidated = true;`"),
        ("M250-C001-ART-05", "`Objc3LoweringIRBoundaryReplayKey(...)`"),
    ),
    "contract_doc": (
        (
            "M250-C001-DOC-01",
            "Contract ID: `objc3c-lowering-runtime-stability-invariant-proofs-freeze/m250-c001-v1`",
        ),
        ("M250-C001-DOC-02", "runtime_dispatch_call_emitted_ = true;"),
        ("M250-C001-DOC-03", "ctx.global_proofs_invalidated = true;"),
        ("M250-C001-DOC-04", "python scripts/check_m250_c001_lowering_runtime_stability_invariant_proofs_contract.py"),
        (
            "M250-C001-DOC-05",
            "python -m pytest tests/tooling/test_check_m250_c001_lowering_runtime_stability_invariant_proofs_contract.py -q",
        ),
        (
            "M250-C001-DOC-06",
            "tmp/reports/m250/M250-C001/lowering_runtime_stability_invariant_proofs_contract_summary.json",
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
        default=Path("tmp/reports/m250/M250-C001/lowering_runtime_stability_invariant_proofs_contract_summary.json"),
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

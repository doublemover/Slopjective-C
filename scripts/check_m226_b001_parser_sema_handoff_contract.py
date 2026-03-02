#!/usr/bin/env python3
"""Fail-closed architecture freeze checker for parser->sema handoff (M226-B001)."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-b001-parser-sema-handoff-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "parser_contract_header": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_contract.h",
    "parser_header": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.h",
    "ast_builder_contract_header": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_ast_builder_contract.h",
    "ast_builder_contract_source": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_ast_builder_contract.cpp",
    "sema_pass_manager_contract_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "sema"
    / "objc3_sema_pass_manager_contract.h",
    "sema_pass_manager_header": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.h",
    "sema_pass_manager_source": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp",
    "diagnostics_bus_header": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_diagnostics_bus.h",
    "pipeline_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "semantics_fragment": ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md",
    "m138_contract_doc": ROOT / "docs" / "contracts" / "parser_ast_contract_expectations.md",
    "m139_contract_doc": ROOT / "docs" / "contracts" / "sema_pass_manager_diagnostics_bus_expectations.md",
    "m226_contract_doc": ROOT / "docs" / "contracts" / "m226_parser_sema_handoff_expectations.md",
}

ARTIFACT_ORDER: tuple[str, ...] = tuple(ARTIFACTS.keys())
ARTIFACT_RANK = {name: index for index, name in enumerate(ARTIFACT_ORDER)}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "parser_contract_header": (
        ("M226-B001-PAR-01", "struct Objc3ParsedProgram {"),
        ("M226-B001-PAR-02", "Objc3Program ast;"),
        ("M226-B001-PAR-03", "inline Objc3Program &MutableObjc3ParsedProgramAst(Objc3ParsedProgram &program) {"),
        ("M226-B001-PAR-04", "inline const Objc3Program &Objc3ParsedProgramAst(const Objc3ParsedProgram &program) {"),
    ),
    "parser_header": (
        ("M226-B001-PAR-05", "struct Objc3ParseResult {"),
        ("M226-B001-PAR-06", "Objc3ParsedProgram program;"),
        ("M226-B001-PAR-07", "std::vector<std::string> diagnostics;"),
        ("M226-B001-PAR-08", "Objc3ParseResult ParseObjc3Program(const Objc3LexTokenStream &tokens);"),
    ),
    "ast_builder_contract_header": (
        ("M226-B001-PAR-09", "struct Objc3AstBuilderResult {"),
        ("M226-B001-PAR-10", "Objc3ParsedProgram program;"),
        ("M226-B001-PAR-11", "Objc3AstBuilderResult BuildObjc3AstFromTokens(const Objc3LexTokenStream &tokens);"),
    ),
    "ast_builder_contract_source": (
        ("M226-B001-PAR-12", "Objc3ParseResult parse_result = ParseObjc3Program(tokens);"),
        ("M226-B001-PAR-13", "builder_result.program = std::move(parse_result.program);"),
        ("M226-B001-PAR-14", "builder_result.diagnostics = std::move(parse_result.diagnostics);"),
    ),
    "sema_pass_manager_contract_header": (
        ("M226-B001-SEM-01", "inline constexpr std::array<Objc3SemaPassId, 3> kObjc3SemaPassOrder = {"),
        ("M226-B001-SEM-02", "Objc3SemaPassId::BuildIntegrationSurface,"),
        ("M226-B001-SEM-03", "Objc3SemaPassId::ValidateBodies,"),
        ("M226-B001-SEM-04", "Objc3SemaPassId::ValidatePureContract,"),
        ("M226-B001-SEM-05", "struct Objc3SemaPassManagerInput {"),
        ("M226-B001-SEM-06", "const Objc3ParsedProgram *program = nullptr;"),
        ("M226-B001-SEM-07", "Objc3SemaDiagnosticsBus diagnostics_bus;"),
        ("M226-B001-SEM-08", "inline bool IsMonotonicObjc3SemaDiagnosticsAfterPass("),
    ),
    "sema_pass_manager_header": (
        ("M226-B001-SEM-09", "Objc3SemaPassManagerResult RunObjc3SemaPassManager(const Objc3SemaPassManagerInput &input);"),
    ),
    "sema_pass_manager_source": (
        ("M226-B001-SEM-10", "if (input.program == nullptr) {"),
        ("M226-B001-SEM-11", "for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {"),
        ("M226-B001-SEM-12", "result.integration_surface = BuildSemanticIntegrationSurface(*input.program, pass_diagnostics);"),
        ("M226-B001-SEM-13", "ValidateSemanticBodies(*input.program, result.integration_surface, input.validation_options, pass_diagnostics);"),
        ("M226-B001-SEM-14", "ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);"),
        ("M226-B001-SEM-15", "input.diagnostics_bus.PublishBatch(pass_diagnostics);"),
        ("M226-B001-SEM-16", "result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();"),
        ("M226-B001-SEM-17", "IsMonotonicObjc3SemaDiagnosticsAfterPass(result.diagnostics_after_pass);"),
    ),
    "diagnostics_bus_header": (
        ("M226-B001-DIA-01", "struct Objc3FrontendDiagnosticsBus {"),
        ("M226-B001-DIA-02", "std::vector<std::string> lexer;"),
        ("M226-B001-DIA-03", "std::vector<std::string> parser;"),
        ("M226-B001-DIA-04", "std::vector<std::string> semantic;"),
        ("M226-B001-DIA-05", "inline void TransportObjc3DiagnosticsToParsedProgram("),
        ("M226-B001-DIA-06", "ast.diagnostics.insert(ast.diagnostics.end(), bus.lexer.begin(), bus.lexer.end());"),
        ("M226-B001-DIA-07", "ast.diagnostics.insert(ast.diagnostics.end(), bus.parser.begin(), bus.parser.end());"),
        ("M226-B001-DIA-08", "ast.diagnostics.insert(ast.diagnostics.end(), bus.semantic.begin(), bus.semantic.end());"),
    ),
    "pipeline_source": (
        ("M226-B001-PIP-01", '#include "parse/objc3_ast_builder_contract.h"'),
        ("M226-B001-PIP-02", '#include "sema/objc3_sema_pass_manager.h"'),
        ("M226-B001-PIP-03", "Objc3AstBuilderResult parse_result = BuildObjc3AstFromTokens(tokens);"),
        ("M226-B001-PIP-04", "result.program = std::move(parse_result.program);"),
        ("M226-B001-PIP-05", "result.stage_diagnostics.parser = std::move(parse_result.diagnostics);"),
        ("M226-B001-PIP-06", "sema_input.program = &result.program;"),
        ("M226-B001-PIP-07", "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;"),
        ("M226-B001-PIP-08", "Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);"),
        ("M226-B001-PIP-09", "result.sema_diagnostics_after_pass = sema_result.diagnostics_after_pass;"),
        ("M226-B001-PIP-10", "TransportObjc3DiagnosticsToParsedProgram(result.stage_diagnostics, result.program);"),
    ),
    "semantics_fragment": (
        ("M226-B001-SPEC-01", "## Sema pass manager + diagnostics bus contract (M139-E001)"),
        (
            "M226-B001-SPEC-02",
            "`RunObjc3SemaPassManager(...)` executes deterministic pass order (`BuildIntegrationSurface`, `ValidateBodies`, `ValidatePureContract`)",
        ),
        ("M226-B001-SPEC-03", "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic"),
        ("M226-B001-SPEC-04", "TransportObjc3DiagnosticsToParsedProgram(...)"),
    ),
    "m138_contract_doc": (
        ("M226-B001-REF-01", "Contract ID: `objc3c-parser-ast-contract/m138-v1`"),
    ),
    "m139_contract_doc": (
        ("M226-B001-REF-02", "Contract ID: `objc3c-sema-pass-manager-diagnostics-bus-contract/m139-v1`"),
    ),
    "m226_contract_doc": (
        ("M226-B001-DOC-01", "# Parser-to-Sema Handoff Architecture Freeze Expectations (M226-B001)"),
        ("M226-B001-DOC-02", "Contract ID: `objc3c-parser-sema-handoff-contract/m226-b001-v1`"),
        ("M226-B001-DOC-03", "`M226-B001-INV-01`"),
        ("M226-B001-DOC-04", "`M226-B001-INV-08`"),
        ("M226-B001-DOC-05", "`python scripts/check_m226_b001_parser_sema_handoff_contract.py`"),
        ("M226-B001-DOC-06", "`python -m pytest tests/tooling/test_check_m226_b001_parser_sema_handoff_contract.py -q`"),
        ("M226-B001-DOC-07", "`docs/contracts/parser_ast_contract_expectations.md` (`M138`)"),
        ("M226-B001-DOC-08", "`docs/contracts/sema_pass_manager_diagnostics_bus_expectations.md` (`M139`)"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "pipeline_source": (
        ("M226-B001-FORB-01", '#include "parse/objc3_parser.h"'),
        ("M226-B001-FORB-02", "ParseObjc3Program(tokens)"),
    ),
    "sema_pass_manager_header": (
        ("M226-B001-FORB-03", '#include "parse/objc3_parser_contract.h"'),
        ("M226-B001-FORB-04", '#include "ast/objc3_ast.h"'),
    ),
    "sema_pass_manager_source": (
        ("M226-B001-FORB-05", "BuildObjc3AstFromTokens("),
    ),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists():
        raise ValueError(f"{artifact} file does not exist: {display_path(path)}")
    if not path.is_file():
        raise ValueError(f"{artifact} path is not a file: {display_path(path)}")
    try:
        return path.read_text(encoding="utf-8")
    except UnicodeDecodeError as exc:
        raise ValueError(f"{artifact} file is not valid UTF-8: {display_path(path)}") from exc
    except OSError as exc:
        raise ValueError(f"unable to read {artifact} file {display_path(path)}: {exc}") from exc


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path("tmp/reports/m226_b001_parser_sema_handoff_contract_summary.json"),
    )
    return parser.parse_args(argv)


def finding_sort_key(finding: Finding) -> tuple[int, str, str]:
    if finding.artifact not in ARTIFACT_RANK:
        raise ValueError(f"unknown artifact in findings: {finding.artifact}")
    return (ARTIFACT_RANK[finding.artifact], finding.check_id, finding.detail)


def collect_required_findings(*, artifact: str, text: str) -> list[Finding]:
    findings: list[Finding] = []
    for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
        if snippet not in text:
            findings.append(
                Finding(
                    artifact=artifact,
                    check_id=check_id,
                    detail=f"expected snippet missing: {snippet}",
                )
            )
    return findings


def collect_forbidden_findings(*, artifact: str, text: str) -> list[Finding]:
    findings: list[Finding] = []
    for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
        if snippet in text:
            findings.append(
                Finding(
                    artifact=artifact,
                    check_id=check_id,
                    detail=f"forbidden snippet present: {snippet}",
                )
            )
    return findings


def total_checks() -> int:
    return sum(len(v) for v in REQUIRED_SNIPPETS.values()) + sum(len(v) for v in FORBIDDEN_SNIPPETS.values())


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    try:
        findings: list[Finding] = []
        for artifact, path in ARTIFACTS.items():
            text = load_text(path, artifact=artifact)
            findings.extend(collect_required_findings(artifact=artifact, text=text))
            findings.extend(collect_forbidden_findings(artifact=artifact, text=text))
        findings = sorted(findings, key=finding_sort_key)
    except ValueError as exc:
        print(f"m226-b001-parser-sema-handoff-contract: error: {exc}", file=sys.stderr)
        return 2

    checks = total_checks()
    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": checks,
        "checks_passed": checks - len(findings),
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
        print(
            "m226-b001-parser-sema-handoff-contract: contract drift detected "
            f"({len(findings)} finding(s)).",
            file=sys.stderr,
        )
        for finding in findings:
            print(f"- {finding.artifact}:{finding.check_id} {finding.detail}", file=sys.stderr)
        print(f"wrote summary: {display_path(args.summary_out)}", file=sys.stderr)
        return 1

    print("m226-b001-parser-sema-handoff-contract: OK")
    print(f"- mode={MODE}")
    print(f"- checks_passed={summary['checks_passed']}")
    print("- fail_closed=true")
    print(f"- summary={display_path(args.summary_out)}")
    return 0


def main() -> None:
    raise SystemExit(run(sys.argv[1:]))


if __name__ == "__main__":
    main()


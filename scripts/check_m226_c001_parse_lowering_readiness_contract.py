#!/usr/bin/env python3
"""Fail-closed readiness checker for parse/lowering contract surfaces (M226-C001)."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-c001-parse-lowering-readiness-contract-v1"

DEFAULT_PARSE_DIR = ROOT / "native" / "objc3c" / "src" / "parse"

EXPECTED_PARSE_FILES: tuple[str, ...] = (
    "objc3_ast_builder.cpp",
    "objc3_ast_builder.h",
    "objc3_ast_builder_contract.cpp",
    "objc3_ast_builder_contract.h",
    "objc3_diagnostics_bus.h",
    "objc3_parse_support.cpp",
    "objc3_parse_support.h",
    "objc3_parser.cpp",
    "objc3_parser.h",
    "objc3_parser_contract.h",
)

ARTIFACTS: dict[str, Path] = {
    "parse_parser_contract_header": DEFAULT_PARSE_DIR / "objc3_parser_contract.h",
    "parse_parser_header": DEFAULT_PARSE_DIR / "objc3_parser.h",
    "parse_parser_source": DEFAULT_PARSE_DIR / "objc3_parser.cpp",
    "parse_ast_builder_header": DEFAULT_PARSE_DIR / "objc3_ast_builder.h",
    "parse_ast_builder_source": DEFAULT_PARSE_DIR / "objc3_ast_builder.cpp",
    "parse_ast_builder_contract_header": DEFAULT_PARSE_DIR / "objc3_ast_builder_contract.h",
    "parse_ast_builder_contract_source": DEFAULT_PARSE_DIR / "objc3_ast_builder_contract.cpp",
    "parse_diagnostics_bus_header": DEFAULT_PARSE_DIR / "objc3_diagnostics_bus.h",
    "parse_support_header": DEFAULT_PARSE_DIR / "objc3_parse_support.h",
    "parse_support_source": DEFAULT_PARSE_DIR / "objc3_parse_support.cpp",
    "pipeline_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "frontend_artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "lowering_contract_source": ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp",
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parse_lowering_readiness_expectations.md",
}

ARTIFACT_ORDER: tuple[str, ...] = ("parse_directory", *ARTIFACTS.keys())
ARTIFACT_RANK = {name: index for index, name in enumerate(ARTIFACT_ORDER)}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "parse_parser_contract_header": (
        ("M226-C001-PARSE-01", "struct Objc3ParsedProgram {"),
        ("M226-C001-PARSE-02", "Objc3Program ast;"),
        ("M226-C001-PARSE-03", "struct Objc3ParserContractSnapshot {"),
        ("M226-C001-PARSE-04", "bool deterministic_handoff = true;"),
        ("M226-C001-PARSE-05", "bool parser_recovery_replay_ready = true;"),
        ("M226-C001-PARSE-06", "inline Objc3ParserContractSnapshot BuildObjc3ParserContractSnapshot("),
    ),
    "parse_parser_header": (
        ("M226-C001-PARSE-07", "struct Objc3ParseResult {"),
        ("M226-C001-PARSE-08", "Objc3ParsedProgram program;"),
        ("M226-C001-PARSE-09", "Objc3ParserContractSnapshot contract_snapshot;"),
        ("M226-C001-PARSE-10", "Objc3ParseResult ParseObjc3Program(const Objc3LexTokenStream &tokens);"),
    ),
    "parse_parser_source": (
        ("M226-C001-PARSE-11", '#include "parse/objc3_parse_support.h"'),
        ("M226-C001-PARSE-12", "using objc3c::parse::support::ParseIntegerLiteralValue;"),
        ("M226-C001-PARSE-13", "static std::string BuildMessageSendSelectorLoweringSymbol("),
        ("M226-C001-PARSE-14", "Objc3ParseResult ParseObjc3Program(const Objc3LexTokenStream &tokens) {"),
        (
            "M226-C001-PARSE-15",
            "result.contract_snapshot = BuildObjc3ParserContractSnapshot(result.program, result.diagnostics.size(), tokens.size());",
        ),
    ),
    "parse_ast_builder_header": (
        ("M226-C001-PARSE-16", "class Objc3AstBuilder {"),
        ("M226-C001-PARSE-17", "Objc3ParsedProgram BeginProgram() const;"),
        ("M226-C001-PARSE-18", "void AddFunctionDecl(Objc3ParsedProgram &program, Objc3ParsedFunctionDecl decl) const;"),
    ),
    "parse_ast_builder_source": (
        ("M226-C001-PARSE-19", "Objc3ParsedProgram Objc3AstBuilder::BeginProgram() const { return Objc3ParsedProgram{}; }"),
        ("M226-C001-PARSE-20", "MutableObjc3ParsedProgramAst(program).module_name = std::move(module_name);"),
        ("M226-C001-PARSE-21", "MutableObjc3ParsedProgramAst(program).functions.push_back(std::move(decl));"),
    ),
    "parse_ast_builder_contract_header": (
        ("M226-C001-PARSE-22", "struct Objc3AstBuilderResult {"),
        ("M226-C001-PARSE-23", "Objc3ParserContractSnapshot contract_snapshot;"),
        ("M226-C001-PARSE-24", "Objc3AstBuilderResult BuildObjc3AstFromTokens(const Objc3LexTokenStream &tokens);"),
    ),
    "parse_ast_builder_contract_source": (
        ("M226-C001-PARSE-25", "Objc3ParseResult parse_result = ParseObjc3Program(tokens);"),
        ("M226-C001-PARSE-26", "builder_result.program = std::move(parse_result.program);"),
        ("M226-C001-PARSE-27", "builder_result.contract_snapshot = parse_result.contract_snapshot;"),
    ),
    "parse_diagnostics_bus_header": (
        ("M226-C001-PARSE-28", "struct Objc3FrontendDiagnosticsBus {"),
        ("M226-C001-PARSE-29", "std::vector<std::string> lexer;"),
        ("M226-C001-PARSE-30", "std::vector<std::string> parser;"),
        ("M226-C001-PARSE-31", "std::vector<std::string> semantic;"),
        (
            "M226-C001-PARSE-32",
            "inline void TransportObjc3DiagnosticsToParsedProgram(const Objc3FrontendDiagnosticsBus &bus, Objc3ParsedProgram &program) {",
        ),
    ),
    "parse_support_header": (
        ("M226-C001-PARSE-33", "namespace objc3c::parse::support {"),
        ("M226-C001-PARSE-34", "bool ParseIntegerLiteralValue(const std::string &text, int &value);"),
        (
            "M226-C001-PARSE-35",
            "std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message);",
        ),
    ),
    "parse_support_source": (
        ("M226-C001-PARSE-36", "bool ParseIntegerLiteralValue(const std::string &text, int &value) {"),
        ("M226-C001-PARSE-37", "if (!NormalizeIntegerDigits(digit_text, base, normalized_digits)) {"),
        ("M226-C001-PARSE-38", "std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message) {"),
    ),
    "pipeline_source": (
        ("M226-C001-PIP-01", '#include "parse/objc3_ast_builder_contract.h"'),
        ("M226-C001-PIP-02", "Objc3AstBuilderResult parse_result = BuildObjc3AstFromTokens(tokens);"),
        ("M226-C001-PIP-03", "result.parser_contract_snapshot = parse_result.contract_snapshot;"),
        ("M226-C001-PIP-04", "Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);"),
        ("M226-C001-PIP-05", "BuildSymbolGraphScopeResolutionSummary(result.integration_surface,"),
        ("M226-C001-PIP-06", "TransportObjc3DiagnosticsToParsedProgram(result.stage_diagnostics, result.program);"),
    ),
    "frontend_artifacts_source": (
        (
            "M226-C001-ART-01",
            "Objc3FrontendArtifactBundle BuildObjc3FrontendArtifacts(const std::filesystem::path &input_path,",
        ),
        ("M226-C001-ART-02", "BuildMessageSendSelectorLoweringContract(program);"),
        ("M226-C001-ART-03", "BuildDispatchAbiMarshallingContract(program, options.lowering.max_message_send_args);"),
        ("M226-C001-ART-04", "BuildRuntimeShimHostLinkContract("),
        (
            "M226-C001-ART-05",
            "if (!EmitObjc3IRText(pipeline_result.program.ast, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)) {",
        ),
        ("M226-C001-ART-06", "ir_frontend_metadata.deterministic_message_send_selector_lowering_handoff ="),
        ("M226-C001-ART-07", "ir_frontend_metadata.deterministic_dispatch_abi_marshalling_handoff ="),
        ("M226-C001-ART-08", "ir_frontend_metadata.deterministic_runtime_shim_host_link_handoff ="),
    ),
    "lowering_contract_source": (
        ("M226-C001-LOW-01", "bool TryNormalizeObjc3LoweringContract(const Objc3LoweringContract &input,"),
        (
            "M226-C001-LOW-02",
            "error = \"invalid lowering contract max_message_send_args: \" + std::to_string(input.max_message_send_args) +",
        ),
        ("M226-C001-LOW-03", "bool TryBuildObjc3LoweringIRBoundary(const Objc3LoweringContract &input,"),
        ("M226-C001-LOW-04", "boundary.selector_global_ordering = kObjc3SelectorGlobalOrdering;"),
        ("M226-C001-LOW-05", "std::string Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary) {"),
        ("M226-C001-LOW-06", "bool IsValidObjc3MessageSendSelectorLoweringContract("),
        ("M226-C001-LOW-07", "std::string Objc3DispatchAbiMarshallingReplayKey("),
        ("M226-C001-LOW-08", "bool IsValidObjc3RuntimeShimHostLinkContract("),
        ("M226-C001-LOW-09", "std::string Objc3RuntimeShimHostLinkReplayKey("),
    ),
    "contract_doc": (
        ("M226-C001-DOC-01", "# Parse-Lowering Readiness Expectations (M226-C001)"),
        ("M226-C001-DOC-02", "Contract ID: `objc3c-parse-lowering-readiness-contract/m226-c001-v1`"),
        ("M226-C001-DOC-03", "`native/objc3c/src/parse/*`"),
        ("M226-C001-DOC-04", "`python scripts/check_m226_c001_parse_lowering_readiness_contract.py`"),
        ("M226-C001-DOC-05", "`python -m pytest tests/tooling/test_check_m226_c001_parse_lowering_readiness_contract.py -q`"),
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
    parser.add_argument("--parse-dir", type=Path, default=DEFAULT_PARSE_DIR)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path("tmp/reports/m226/m226_c001_parse_lowering_readiness_contract_summary.json"),
    )
    return parser.parse_args(argv)


def collect_parse_directory_findings(parse_dir: Path) -> list[Finding]:
    if not parse_dir.exists():
        raise ValueError(f"parse directory does not exist: {display_path(parse_dir)}")
    if not parse_dir.is_dir():
        raise ValueError(f"parse directory is not a directory: {display_path(parse_dir)}")

    actual_files = sorted(item.name for item in parse_dir.iterdir() if item.is_file())
    expected_files = sorted(EXPECTED_PARSE_FILES)
    if actual_files == expected_files:
        return []

    missing = sorted(set(expected_files) - set(actual_files))
    extra = sorted(set(actual_files) - set(expected_files))
    return [
        Finding(
            artifact="parse_directory",
            check_id="M226-C001-PARSE-00",
            detail=f"parse file set drifted (missing={missing}, extra={extra})",
        )
    ]


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


def total_checks() -> int:
    return 1 + sum(len(snippets) for snippets in REQUIRED_SNIPPETS.values())


def finding_sort_key(finding: Finding) -> tuple[int, str, str]:
    if finding.artifact not in ARTIFACT_RANK:
        raise ValueError(f"unknown artifact in findings: {finding.artifact}")
    return (ARTIFACT_RANK[finding.artifact], finding.check_id, finding.detail)


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    try:
        findings = collect_parse_directory_findings(args.parse_dir)
        for artifact, path in ARTIFACTS.items():
            text = load_text(path, artifact=artifact)
            findings.extend(collect_required_findings(artifact=artifact, text=text))
        findings = sorted(findings, key=finding_sort_key)
    except ValueError as exc:
        print(f"m226-c001-parse-lowering-readiness-contract: error: {exc}", file=sys.stderr)
        return 2

    checks_total = total_checks()
    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(findings),
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
            "m226-c001-parse-lowering-readiness-contract: contract drift detected "
            f"({len(findings)} finding(s)).",
            file=sys.stderr,
        )
        for finding in findings:
            print(f"- {finding.artifact}:{finding.check_id} {finding.detail}", file=sys.stderr)
        print(f"wrote summary: {display_path(args.summary_out)}", file=sys.stderr)
        return 1

    print("m226-c001-parse-lowering-readiness-contract: OK")
    print(f"- mode={MODE}")
    print(f"- checks_passed={summary['checks_passed']}")
    print("- fail_closed=true")
    print(f"- summary={display_path(args.summary_out)}")
    return 0


def main() -> int:
    try:
        return run(sys.argv[1:])
    except Exception as exc:  # noqa: BLE001
        print(f"m226-c001-parse-lowering-readiness-contract: error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())

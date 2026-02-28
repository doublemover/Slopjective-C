#!/usr/bin/env python3
"""Fail-closed contract validator for M138 parser extraction + AST scaffolding wiring."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m138-parser-ast-contract-v1"

DEFAULT_PARSER_HEADER = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.h"
DEFAULT_PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_AST_BUILDER_HEADER = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_ast_builder_contract.h"
DEFAULT_AST_BUILDER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_ast_builder_contract.cpp"
DEFAULT_AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
DEFAULT_PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
DEFAULT_CMAKE = ROOT / "native" / "objc3c" / "CMakeLists.txt"
DEFAULT_BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"
DEFAULT_GRAMMAR_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DEFAULT_ARTIFACTS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DEFAULT_TESTS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
DEFAULT_CONTRACT_DOC = ROOT / "docs" / "contracts" / "parser_ast_contract_expectations.md"
DEFAULT_PLANNING_LINT_WORKFLOW = ROOT / ".github" / "workflows" / "planning-lint.yml"
DEFAULT_TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"

PACKAGE_SCRIPT_NAME = "check:compiler-closeout:m138"
PACKAGE_TEST_SCRIPT_NAME = "test:objc3c:parser-ast-extraction"
PACKAGE_LANE_E_SCRIPT_NAME = "test:objc3c:lane-e"
PACKAGE_TASK_HYGIENE_SCRIPT_NAME = "check:task-hygiene"


@dataclass(frozen=True)
class SnippetRule:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class DriftFinding:
    artifact: str
    check_id: str
    detail: str


ARTIFACT_ORDER = (
    "parser_header",
    "parser_source",
    "ast_builder_header",
    "ast_builder_source",
    "ast_header",
    "pipeline_source",
    "cmake",
    "build_script",
    "grammar_fragment",
    "artifacts_fragment",
    "tests_fragment",
    "contract_doc",
    "planning_lint_workflow",
    "task_hygiene_workflow",
    "package_json",
)
ARTIFACT_RANK = {artifact: index for index, artifact in enumerate(ARTIFACT_ORDER)}

REQUIRED_SNIPPETS: dict[str, tuple[SnippetRule, ...]] = {
    "parser_header": (
        SnippetRule("M138-PRS-01", "struct Objc3ParseResult"),
        SnippetRule("M138-PRS-02", "Objc3ParsedProgram program;"),
        SnippetRule("M138-PRS-03", "Objc3ParseResult ParseObjc3Program("),
    ),
    "parser_source": (
        SnippetRule("M138-PRS-04", "class Objc3Parser"),
        SnippetRule("M138-PRS-05", "Objc3ParseResult ParseObjc3Program("),
    ),
    "ast_builder_header": (
        SnippetRule("M138-ASTB-01", "struct Objc3AstBuilderResult"),
        SnippetRule("M138-ASTB-02", "Objc3ParsedProgram program;"),
        SnippetRule("M138-ASTB-03", "BuildObjc3AstFromTokens("),
    ),
    "ast_builder_source": (
        SnippetRule("M138-ASTB-04", '#include "parse/objc3_parser.h"'),
        SnippetRule("M138-ASTB-05", "ParseObjc3Program(tokens)"),
    ),
    "ast_header": (
        SnippetRule("M138-AST-01", "struct Expr {"),
        SnippetRule("M138-AST-02", "struct Stmt {"),
        SnippetRule("M138-AST-03", "struct FunctionDecl {"),
        SnippetRule("M138-AST-04", "struct Objc3Program {"),
    ),
    "pipeline_source": (
        SnippetRule("M138-PIP-01", '#include "parse/objc3_ast_builder_contract.h"'),
        SnippetRule("M138-PIP-02", "Objc3AstBuilderResult parse_result = BuildObjc3AstFromTokens(tokens);"),
    ),
    "cmake": (
        SnippetRule("M138-CMK-01", "add_library(objc3c_parse STATIC"),
        SnippetRule("M138-CMK-02", "src/parse/objc3_ast_builder_contract.cpp"),
        SnippetRule("M138-CMK-03", "src/parse/objc3_parser.cpp"),
    ),
    "build_script": (
        SnippetRule("M138-BLD-01", '"native/objc3c/src/parse/objc3_ast_builder_contract.cpp"'),
        SnippetRule("M138-BLD-02", '"native/objc3c/src/parse/objc3_parser.cpp"'),
    ),
    "grammar_fragment": (
        SnippetRule("M138-DOCG-01", "## Parser subsystem + AST builder scaffolding contract (M138-E001)"),
    ),
    "artifacts_fragment": (
        SnippetRule("M138-DOCA-01", "## Parser/AST extraction validation artifacts (M138-E001)"),
    ),
    "tests_fragment": (
        SnippetRule("M138-DOCT-01", "npm run test:objc3c:parser-ast-extraction"),
        SnippetRule("M138-DOCT-02", "npm run check:compiler-closeout:m138"),
    ),
    "contract_doc": (
        SnippetRule("M138-DOC-01", "# Parser Subsystem and AST Scaffolding Contract Expectations (M138)"),
        SnippetRule("M138-DOC-02", "Contract ID: `objc3c-parser-ast-contract/m138-v1`"),
        SnippetRule("M138-DOC-03", "`python scripts/check_m138_parser_ast_contract.py`"),
        SnippetRule("M138-DOC-04", "`npm run check:compiler-closeout:m138`"),
    ),
    "planning_lint_workflow": (
        SnippetRule("M138-CI-PLN-01", "python scripts/check_m138_parser_ast_contract.py"),
    ),
    "task_hygiene_workflow": (
        SnippetRule("M138-CI-TH-01", "npm run check:compiler-closeout:m138"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[SnippetRule, ...]] = {
    "pipeline_source": (
        SnippetRule("M138-PIP-03", '#include "parse/objc3_parser.h"'),
        SnippetRule("M138-PIP-04", "class Objc3Parser {"),
    ),
}

PACKAGE_SCRIPT_REQUIRED_TOKENS: tuple[tuple[str, str], ...] = (
    ("M138-PKG-02", "python scripts/check_m138_parser_ast_contract.py"),
    ("M138-PKG-03", "npm run test:objc3c:parser-ast-extraction"),
    ("M138-PKG-04", '--glob "docs/contracts/parser_ast_contract_expectations.md"'),
)

PACKAGE_TEST_SCRIPT_REQUIRED_TOKENS: tuple[tuple[str, str], ...] = (
    ("M138-PKG-05", "tests/tooling/test_objc3c_parser_extraction.py"),
    ("M138-PKG-06", "tests/tooling/test_objc3c_parser_ast_builder_extraction.py"),
    ("M138-PKG-07", "python -m pytest"),
)

PACKAGE_LANE_E_REQUIRED_TOKENS: tuple[tuple[str, str], ...] = (
    ("M138-PKG-08", "test:objc3c:parser-ast-extraction"),
)

PACKAGE_TASK_HYGIENE_REQUIRED_TOKENS: tuple[tuple[str, str], ...] = (
    ("M138-PKG-09", "check:compiler-closeout:m138"),
)


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def resolve_input_path(path: Path) -> Path:
    if path.is_absolute():
        return path
    return ROOT / path


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


def load_json(path: Path, *, artifact: str) -> dict[str, object]:
    text = load_text(path, artifact=artifact)
    try:
        payload = json.loads(text)
    except json.JSONDecodeError as exc:
        raise ValueError(f"{artifact} parse error ({display_path(path)}): {exc}") from exc
    if not isinstance(payload, dict):
        raise ValueError(f"{artifact} root must be an object: {display_path(path)}")
    return payload


def finding_sort_key(finding: DriftFinding) -> tuple[int, str]:
    if finding.artifact not in ARTIFACT_RANK:
        raise ValueError(f"unknown artifact in drift finding: {finding.artifact}")
    return (ARTIFACT_RANK[finding.artifact], finding.check_id)


def collect_snippet_findings(*, artifact: str, content: str) -> list[DriftFinding]:
    findings: list[DriftFinding] = []
    for rule in REQUIRED_SNIPPETS[artifact]:
        if rule.snippet not in content:
            findings.append(
                DriftFinding(
                    artifact=artifact,
                    check_id=rule.check_id,
                    detail=f"expected snippet: {rule.snippet}",
                )
            )
    return findings


def collect_forbidden_findings(*, artifact: str, content: str) -> list[DriftFinding]:
    findings: list[DriftFinding] = []
    for rule in FORBIDDEN_SNIPPETS.get(artifact, ()):
        if rule.snippet in content:
            findings.append(
                DriftFinding(
                    artifact=artifact,
                    check_id=rule.check_id,
                    detail=f"forbidden snippet present: {rule.snippet}",
                )
            )
    return findings


def collect_package_findings(*, package_path: Path) -> list[DriftFinding]:
    payload = load_json(package_path, artifact="package_json")
    scripts = payload.get("scripts")
    findings: list[DriftFinding] = []

    if not isinstance(scripts, dict):
        findings.append(
            DriftFinding(
                artifact="package_json",
                check_id="M138-PKG-00",
                detail="package.json field 'scripts' must be an object.",
            )
        )
        return findings

    closeout_script = scripts.get(PACKAGE_SCRIPT_NAME)
    if not isinstance(closeout_script, str):
        findings.append(
            DriftFinding(
                artifact="package_json",
                check_id="M138-PKG-01",
                detail=f"missing scripts entry '{PACKAGE_SCRIPT_NAME}'.",
            )
        )
    else:
        for check_id, token in PACKAGE_SCRIPT_REQUIRED_TOKENS:
            if token not in closeout_script:
                findings.append(
                    DriftFinding(
                        artifact="package_json",
                        check_id=check_id,
                        detail=f"scripts['{PACKAGE_SCRIPT_NAME}'] missing token: {token}",
                    )
                )

    test_script = scripts.get(PACKAGE_TEST_SCRIPT_NAME)
    if not isinstance(test_script, str):
        findings.append(
            DriftFinding(
                artifact="package_json",
                check_id="M138-PKG-10",
                detail=f"missing scripts entry '{PACKAGE_TEST_SCRIPT_NAME}'.",
            )
        )
    else:
        for check_id, token in PACKAGE_TEST_SCRIPT_REQUIRED_TOKENS:
            if token not in test_script:
                findings.append(
                    DriftFinding(
                        artifact="package_json",
                        check_id=check_id,
                        detail=f"scripts['{PACKAGE_TEST_SCRIPT_NAME}'] missing token: {token}",
                    )
                )

    lane_e_script = scripts.get(PACKAGE_LANE_E_SCRIPT_NAME)
    if not isinstance(lane_e_script, str):
        findings.append(
            DriftFinding(
                artifact="package_json",
                check_id="M138-PKG-11",
                detail=f"missing scripts entry '{PACKAGE_LANE_E_SCRIPT_NAME}'.",
            )
        )
    else:
        for check_id, token in PACKAGE_LANE_E_REQUIRED_TOKENS:
            if token not in lane_e_script:
                findings.append(
                    DriftFinding(
                        artifact="package_json",
                        check_id=check_id,
                        detail=f"scripts['{PACKAGE_LANE_E_SCRIPT_NAME}'] missing token: {token}",
                    )
                )

    task_hygiene_script = scripts.get(PACKAGE_TASK_HYGIENE_SCRIPT_NAME)
    if not isinstance(task_hygiene_script, str):
        findings.append(
            DriftFinding(
                artifact="package_json",
                check_id="M138-PKG-12",
                detail=f"missing scripts entry '{PACKAGE_TASK_HYGIENE_SCRIPT_NAME}'.",
            )
        )
    else:
        for check_id, token in PACKAGE_TASK_HYGIENE_REQUIRED_TOKENS:
            if token not in task_hygiene_script:
                findings.append(
                    DriftFinding(
                        artifact="package_json",
                        check_id=check_id,
                        detail=f"scripts['{PACKAGE_TASK_HYGIENE_SCRIPT_NAME}'] missing token: {token}",
                    )
                )

    return findings


def validate_contract(
    *,
    parser_header: str,
    parser_source: str,
    ast_builder_header: str,
    ast_builder_source: str,
    ast_header: str,
    pipeline_source: str,
    cmake: str,
    build_script: str,
    grammar_fragment: str,
    artifacts_fragment: str,
    tests_fragment: str,
    contract_doc: str,
    planning_lint_workflow: str,
    task_hygiene_workflow: str,
    package_json_path: Path,
) -> list[DriftFinding]:
    findings: list[DriftFinding] = []
    surfaces = {
        "parser_header": parser_header,
        "parser_source": parser_source,
        "ast_builder_header": ast_builder_header,
        "ast_builder_source": ast_builder_source,
        "ast_header": ast_header,
        "pipeline_source": pipeline_source,
        "cmake": cmake,
        "build_script": build_script,
        "grammar_fragment": grammar_fragment,
        "artifacts_fragment": artifacts_fragment,
        "tests_fragment": tests_fragment,
        "contract_doc": contract_doc,
        "planning_lint_workflow": planning_lint_workflow,
        "task_hygiene_workflow": task_hygiene_workflow,
    }

    for artifact, content in surfaces.items():
        findings.extend(collect_snippet_findings(artifact=artifact, content=content))
        findings.extend(collect_forbidden_findings(artifact=artifact, content=content))

    findings.extend(collect_package_findings(package_path=package_json_path))
    return sorted(findings, key=finding_sort_key)


def render_drift_report(*, findings: list[DriftFinding], rerun_tokens: list[str]) -> str:
    ordered = sorted(findings, key=finding_sort_key)
    lines = [
        "m138-parser-ast-contract: contract drift detected "
        f"({len(ordered)} failed check(s)).",
        "drift findings:",
    ]
    for finding in ordered:
        lines.append(f"- {finding.artifact}:{finding.check_id}")
        lines.append(f"  {finding.detail}")
    lines.extend(
        [
            "remediation:",
            "1. Restore M138 parser/AST contract snippets and gate wiring.",
            "2. Re-run validator:",
            " ".join(rerun_tokens),
        ]
    )
    return "\n".join(lines)


def check_contract(
    *,
    parser_header_path: Path,
    parser_source_path: Path,
    ast_builder_header_path: Path,
    ast_builder_source_path: Path,
    ast_header_path: Path,
    pipeline_source_path: Path,
    cmake_path: Path,
    build_script_path: Path,
    grammar_fragment_path: Path,
    artifacts_fragment_path: Path,
    tests_fragment_path: Path,
    contract_doc_path: Path,
    planning_lint_workflow_path: Path,
    task_hygiene_workflow_path: Path,
    package_json_path: Path,
) -> int:
    findings = validate_contract(
        parser_header=load_text(parser_header_path, artifact="parser_header"),
        parser_source=load_text(parser_source_path, artifact="parser_source"),
        ast_builder_header=load_text(ast_builder_header_path, artifact="ast_builder_header"),
        ast_builder_source=load_text(ast_builder_source_path, artifact="ast_builder_source"),
        ast_header=load_text(ast_header_path, artifact="ast_header"),
        pipeline_source=load_text(pipeline_source_path, artifact="pipeline_source"),
        cmake=load_text(cmake_path, artifact="cmake"),
        build_script=load_text(build_script_path, artifact="build_script"),
        grammar_fragment=load_text(grammar_fragment_path, artifact="grammar_fragment"),
        artifacts_fragment=load_text(artifacts_fragment_path, artifact="artifacts_fragment"),
        tests_fragment=load_text(tests_fragment_path, artifact="tests_fragment"),
        contract_doc=load_text(contract_doc_path, artifact="contract_doc"),
        planning_lint_workflow=load_text(planning_lint_workflow_path, artifact="planning_lint_workflow"),
        task_hygiene_workflow=load_text(task_hygiene_workflow_path, artifact="task_hygiene_workflow"),
        package_json_path=package_json_path,
    )
    if findings:
        rerun_tokens = [
            "python",
            "scripts/check_m138_parser_ast_contract.py",
            "--contract-doc",
            display_path(contract_doc_path),
            "--package-json",
            display_path(package_json_path),
        ]
        print(render_drift_report(findings=findings, rerun_tokens=rerun_tokens), file=sys.stderr)
        return 1

    checks_passed = (
        sum(len(rules) for rules in REQUIRED_SNIPPETS.values())
        + sum(len(rules) for rules in FORBIDDEN_SNIPPETS.values())
        + 1
        + len(PACKAGE_SCRIPT_REQUIRED_TOKENS)
        + len(PACKAGE_TEST_SCRIPT_REQUIRED_TOKENS)
        + len(PACKAGE_LANE_E_REQUIRED_TOKENS)
        + len(PACKAGE_TASK_HYGIENE_REQUIRED_TOKENS)
    )
    print("m138-parser-ast-contract: OK")
    print(f"- mode={MODE}")
    print(f"- parser_header={display_path(parser_header_path)}")
    print(f"- parser_source={display_path(parser_source_path)}")
    print(f"- ast_builder_header={display_path(ast_builder_header_path)}")
    print(f"- ast_builder_source={display_path(ast_builder_source_path)}")
    print(f"- ast_header={display_path(ast_header_path)}")
    print(f"- pipeline_source={display_path(pipeline_source_path)}")
    print(f"- cmake={display_path(cmake_path)}")
    print(f"- build_script={display_path(build_script_path)}")
    print(f"- grammar_fragment={display_path(grammar_fragment_path)}")
    print(f"- artifacts_fragment={display_path(artifacts_fragment_path)}")
    print(f"- tests_fragment={display_path(tests_fragment_path)}")
    print(f"- contract_doc={display_path(contract_doc_path)}")
    print(f"- planning_lint_workflow={display_path(planning_lint_workflow_path)}")
    print(f"- task_hygiene_workflow={display_path(task_hygiene_workflow_path)}")
    print(f"- package_json={display_path(package_json_path)}")
    print(f"- checks_passed={checks_passed}")
    print("- fail_closed=true")
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_m138_parser_ast_contract.py",
        description="Fail-closed validator for M138 parser subsystem + AST scaffolding integration wiring.",
    )
    parser.add_argument("--parser-header", type=Path, default=DEFAULT_PARSER_HEADER)
    parser.add_argument("--parser-source", type=Path, default=DEFAULT_PARSER_SOURCE)
    parser.add_argument("--ast-builder-header", type=Path, default=DEFAULT_AST_BUILDER_HEADER)
    parser.add_argument("--ast-builder-source", type=Path, default=DEFAULT_AST_BUILDER_SOURCE)
    parser.add_argument("--ast-header", type=Path, default=DEFAULT_AST_HEADER)
    parser.add_argument("--pipeline-source", type=Path, default=DEFAULT_PIPELINE_SOURCE)
    parser.add_argument("--cmake", type=Path, default=DEFAULT_CMAKE)
    parser.add_argument("--build-script", type=Path, default=DEFAULT_BUILD_SCRIPT)
    parser.add_argument("--grammar-fragment", type=Path, default=DEFAULT_GRAMMAR_FRAGMENT)
    parser.add_argument("--artifacts-fragment", type=Path, default=DEFAULT_ARTIFACTS_FRAGMENT)
    parser.add_argument("--tests-fragment", type=Path, default=DEFAULT_TESTS_FRAGMENT)
    parser.add_argument("--contract-doc", type=Path, default=DEFAULT_CONTRACT_DOC)
    parser.add_argument("--planning-lint-workflow", type=Path, default=DEFAULT_PLANNING_LINT_WORKFLOW)
    parser.add_argument("--task-hygiene-workflow", type=Path, default=DEFAULT_TASK_HYGIENE_WORKFLOW)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        return check_contract(
            parser_header_path=resolve_input_path(args.parser_header),
            parser_source_path=resolve_input_path(args.parser_source),
            ast_builder_header_path=resolve_input_path(args.ast_builder_header),
            ast_builder_source_path=resolve_input_path(args.ast_builder_source),
            ast_header_path=resolve_input_path(args.ast_header),
            pipeline_source_path=resolve_input_path(args.pipeline_source),
            cmake_path=resolve_input_path(args.cmake),
            build_script_path=resolve_input_path(args.build_script),
            grammar_fragment_path=resolve_input_path(args.grammar_fragment),
            artifacts_fragment_path=resolve_input_path(args.artifacts_fragment),
            tests_fragment_path=resolve_input_path(args.tests_fragment),
            contract_doc_path=resolve_input_path(args.contract_doc),
            planning_lint_workflow_path=resolve_input_path(args.planning_lint_workflow),
            task_hygiene_workflow_path=resolve_input_path(args.task_hygiene_workflow),
            package_json_path=resolve_input_path(args.package_json),
        )
    except ValueError as exc:
        print(f"m138-parser-ast-contract: error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())

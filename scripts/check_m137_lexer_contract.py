#!/usr/bin/env python3
"""Fail-closed contract validator for M137 lexer extraction/token contract wiring."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m137-lexer-contract-v1"

DEFAULT_TOKEN_CONTRACT = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
DEFAULT_AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
DEFAULT_PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_CMAKE = ROOT / "native" / "objc3c" / "CMakeLists.txt"
DEFAULT_BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"
DEFAULT_CONTRACT_DOC = ROOT / "docs" / "contracts" / "lexer_token_contract_expectations.md"
DEFAULT_PLANNING_LINT_WORKFLOW = ROOT / ".github" / "workflows" / "planning-lint.yml"
DEFAULT_TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"

PACKAGE_SCRIPT_NAME = "check:compiler-closeout:m137"
PACKAGE_TEST_SCRIPT_NAME = "test:objc3c:lexer-parity"
PACKAGE_EXTRACTION_SCRIPT_NAME = "test:objc3c:lexer-extraction-token-contract"
PACKAGE_LANE_E_SCRIPT_NAME = "test:objc3c:lane-e"


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
    "token_contract_header",
    "ast_header",
    "parser_source",
    "cmake",
    "build_script",
    "contract_doc",
    "planning_lint_workflow",
    "task_hygiene_workflow",
    "package_json",
)
ARTIFACT_RANK = {artifact: index for index, artifact in enumerate(ARTIFACT_ORDER)}

REQUIRED_SNIPPETS: dict[str, tuple[SnippetRule, ...]] = {
    "token_contract_header": (
        SnippetRule("M137-TKN-01", "enum class Objc3SemaTokenKind"),
        SnippetRule("M137-TKN-02", "PointerDeclarator"),
        SnippetRule("M137-TKN-03", "NullabilitySuffix"),
        SnippetRule("M137-TKN-04", "struct Objc3SemaTokenMetadata"),
        SnippetRule("M137-TKN-05", "std::string text;"),
        SnippetRule("M137-TKN-06", "unsigned line = 1;"),
        SnippetRule("M137-TKN-07", "unsigned column = 1;"),
        SnippetRule("M137-TKN-08", "MakeObjc3SemaTokenMetadata"),
    ),
    "ast_header": (
        SnippetRule("M137-AST-01", '#include "token/objc3_token_contract.h"'),
        SnippetRule("M137-AST-02", "std::vector<Objc3SemaTokenMetadata> pointer_declarator_tokens;"),
        SnippetRule("M137-AST-03", "std::vector<Objc3SemaTokenMetadata> nullability_suffix_tokens;"),
    ),
    "parser_source": (
        SnippetRule("M137-PRS-01", "MakeObjc3SemaTokenMetadata("),
        SnippetRule("M137-PRS-02", "Objc3SemaTokenKind::PointerDeclarator"),
        SnippetRule("M137-PRS-03", "Objc3SemaTokenKind::NullabilitySuffix"),
    ),
    "cmake": (
        SnippetRule("M137-CMK-01", "add_library(objc3c_lex STATIC"),
        SnippetRule("M137-CMK-02", "src/lex/objc3_lexer.cpp"),
    ),
    "build_script": (
        SnippetRule("M137-BLD-01", '"native/objc3c/src/lex/objc3_lexer.cpp"'),
    ),
    "contract_doc": (
        SnippetRule("M137-DOC-01", "# Lexer Token Contract Expectations (M137)"),
        SnippetRule("M137-DOC-02", "Contract ID: `objc3c-lexer-token-contract/m137-v1`"),
        SnippetRule("M137-DOC-03", "`python scripts/check_m137_lexer_contract.py`"),
        SnippetRule("M137-DOC-04", "`npm run check:compiler-closeout:m137`"),
    ),
    "planning_lint_workflow": (
        SnippetRule("M137-CI-PLN-01", "python scripts/check_m137_lexer_contract.py"),
    ),
    "task_hygiene_workflow": (
        SnippetRule("M137-CI-TH-01", "npm run check:compiler-closeout:m137"),
    ),
}

PACKAGE_SCRIPT_REQUIRED_TOKENS: tuple[tuple[str, str], ...] = (
    ("M137-PKG-02", "python scripts/check_m137_lexer_contract.py"),
    ("M137-PKG-03", "npm run test:objc3c:lexer-extraction-token-contract"),
    ("M137-PKG-04", "npm run test:objc3c:lexer-parity"),
    ("M137-PKG-05", '--glob "docs/contracts/lexer_token_contract_expectations.md"'),
)

PACKAGE_TEST_SCRIPT_REQUIRED_TOKENS: tuple[tuple[str, str], ...] = (
    ("M137-PKG-06", "python -m pytest tests/tooling/test_objc3c_lexer_parity.py -q"),
)

PACKAGE_LANE_E_REQUIRED_TOKENS: tuple[tuple[str, str], ...] = (
    ("M137-PKG-07", "test:objc3c:driver-shell-split"),
    ("M137-PKG-08", "test:objc3c:lexer-extraction-token-contract"),
    ("M137-PKG-09", "test:objc3c:lexer-parity"),
)

PACKAGE_EXTRACTION_SCRIPT_REQUIRED_TOKENS: tuple[tuple[str, str], ...] = (
    ("M137-PKG-10", "powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_lexer_extraction_token_contract.ps1"),
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


def collect_package_findings(*, package_path: Path) -> list[DriftFinding]:
    payload = load_json(package_path, artifact="package_json")
    scripts = payload.get("scripts")
    findings: list[DriftFinding] = []

    if not isinstance(scripts, dict):
        findings.append(
            DriftFinding(
                artifact="package_json",
                check_id="M137-PKG-00",
                detail="package.json field 'scripts' must be an object.",
            )
        )
        return findings

    script_command = scripts.get(PACKAGE_SCRIPT_NAME)
    if not isinstance(script_command, str):
        findings.append(
            DriftFinding(
                artifact="package_json",
                check_id="M137-PKG-01",
                detail=f"missing scripts entry '{PACKAGE_SCRIPT_NAME}'.",
            )
        )
    else:
        for check_id, token in PACKAGE_SCRIPT_REQUIRED_TOKENS:
            if token not in script_command:
                findings.append(
                    DriftFinding(
                        artifact="package_json",
                        check_id=check_id,
                        detail=f"scripts['{PACKAGE_SCRIPT_NAME}'] missing token: {token}",
                    )
                )

    test_script_command = scripts.get(PACKAGE_TEST_SCRIPT_NAME)
    if not isinstance(test_script_command, str):
        findings.append(
            DriftFinding(
                artifact="package_json",
                check_id="M137-PKG-11",
                detail=f"missing scripts entry '{PACKAGE_TEST_SCRIPT_NAME}'.",
            )
        )
    else:
        for check_id, token in PACKAGE_TEST_SCRIPT_REQUIRED_TOKENS:
            if token not in test_script_command:
                findings.append(
                    DriftFinding(
                        artifact="package_json",
                        check_id=check_id,
                        detail=f"scripts['{PACKAGE_TEST_SCRIPT_NAME}'] missing token: {token}",
                    )
                )

    extraction_script_command = scripts.get(PACKAGE_EXTRACTION_SCRIPT_NAME)
    if not isinstance(extraction_script_command, str):
        findings.append(
            DriftFinding(
                artifact="package_json",
                check_id="M137-PKG-12",
                detail=f"missing scripts entry '{PACKAGE_EXTRACTION_SCRIPT_NAME}'.",
            )
        )
    else:
        for check_id, token in PACKAGE_EXTRACTION_SCRIPT_REQUIRED_TOKENS:
            if token not in extraction_script_command:
                findings.append(
                    DriftFinding(
                        artifact="package_json",
                        check_id=check_id,
                        detail=f"scripts['{PACKAGE_EXTRACTION_SCRIPT_NAME}'] missing token: {token}",
                    )
                )

    lane_e_script_command = scripts.get(PACKAGE_LANE_E_SCRIPT_NAME)
    if not isinstance(lane_e_script_command, str):
        findings.append(
            DriftFinding(
                artifact="package_json",
                check_id="M137-PKG-13",
                detail=f"missing scripts entry '{PACKAGE_LANE_E_SCRIPT_NAME}'.",
            )
        )
    else:
        for check_id, token in PACKAGE_LANE_E_REQUIRED_TOKENS:
            if token not in lane_e_script_command:
                findings.append(
                    DriftFinding(
                        artifact="package_json",
                        check_id=check_id,
                        detail=f"scripts['{PACKAGE_LANE_E_SCRIPT_NAME}'] missing token: {token}",
                    )
                )

    return findings


def validate_contract(
    *,
    token_contract_header: str,
    ast_header: str,
    parser_source: str,
    cmake: str,
    build_script: str,
    contract_doc: str,
    planning_lint_workflow: str,
    task_hygiene_workflow: str,
    package_json_path: Path,
) -> list[DriftFinding]:
    findings: list[DriftFinding] = []
    findings.extend(collect_snippet_findings(artifact="token_contract_header", content=token_contract_header))
    findings.extend(collect_snippet_findings(artifact="ast_header", content=ast_header))
    findings.extend(collect_snippet_findings(artifact="parser_source", content=parser_source))
    findings.extend(collect_snippet_findings(artifact="cmake", content=cmake))
    findings.extend(collect_snippet_findings(artifact="build_script", content=build_script))
    findings.extend(collect_snippet_findings(artifact="contract_doc", content=contract_doc))
    findings.extend(
        collect_snippet_findings(artifact="planning_lint_workflow", content=planning_lint_workflow)
    )
    findings.extend(
        collect_snippet_findings(artifact="task_hygiene_workflow", content=task_hygiene_workflow)
    )
    findings.extend(collect_package_findings(package_path=package_json_path))
    return sorted(findings, key=finding_sort_key)


def render_drift_report(*, findings: list[DriftFinding], rerun_tokens: list[str]) -> str:
    ordered = sorted(findings, key=finding_sort_key)
    lines = [
        "m137-lexer-contract: contract drift detected "
        f"({len(ordered)} failed check(s)).",
        "drift findings:",
    ]
    for finding in ordered:
        lines.append(f"- {finding.artifact}:{finding.check_id}")
        lines.append(f"  {finding.detail}")
    lines.extend(
        [
            "remediation:",
            "1. Restore M137 lexer contract snippets and gate wiring.",
            "2. Re-run validator:",
            " ".join(rerun_tokens),
        ]
    )
    return "\n".join(lines)


def check_contract(
    *,
    token_contract_path: Path,
    ast_header_path: Path,
    parser_source_path: Path,
    cmake_path: Path,
    build_script_path: Path,
    contract_doc_path: Path,
    planning_lint_workflow_path: Path,
    task_hygiene_workflow_path: Path,
    package_json_path: Path,
) -> int:
    findings = validate_contract(
        token_contract_header=load_text(token_contract_path, artifact="token_contract_header"),
        ast_header=load_text(ast_header_path, artifact="ast_header"),
        parser_source=load_text(parser_source_path, artifact="parser_source"),
        cmake=load_text(cmake_path, artifact="cmake"),
        build_script=load_text(build_script_path, artifact="build_script"),
        contract_doc=load_text(contract_doc_path, artifact="contract_doc"),
        planning_lint_workflow=load_text(planning_lint_workflow_path, artifact="planning_lint_workflow"),
        task_hygiene_workflow=load_text(task_hygiene_workflow_path, artifact="task_hygiene_workflow"),
        package_json_path=package_json_path,
    )
    if findings:
        rerun_tokens = [
            "python",
            "scripts/check_m137_lexer_contract.py",
            "--token-contract",
            display_path(token_contract_path),
            "--contract-doc",
            display_path(contract_doc_path),
            "--planning-lint-workflow",
            display_path(planning_lint_workflow_path),
            "--task-hygiene-workflow",
            display_path(task_hygiene_workflow_path),
            "--package-json",
            display_path(package_json_path),
        ]
        print(render_drift_report(findings=findings, rerun_tokens=rerun_tokens), file=sys.stderr)
        return 1

    checks_passed = (
        sum(len(rules) for rules in REQUIRED_SNIPPETS.values())
        + 1
        + len(PACKAGE_SCRIPT_REQUIRED_TOKENS)
        + len(PACKAGE_TEST_SCRIPT_REQUIRED_TOKENS)
        + len(PACKAGE_EXTRACTION_SCRIPT_REQUIRED_TOKENS)
        + len(PACKAGE_LANE_E_REQUIRED_TOKENS)
    )
    print("m137-lexer-contract: OK")
    print(f"- mode={MODE}")
    print(f"- token_contract={display_path(token_contract_path)}")
    print(f"- ast_header={display_path(ast_header_path)}")
    print(f"- parser_source={display_path(parser_source_path)}")
    print(f"- cmake={display_path(cmake_path)}")
    print(f"- build_script={display_path(build_script_path)}")
    print(f"- contract_doc={display_path(contract_doc_path)}")
    print(f"- planning_lint_workflow={display_path(planning_lint_workflow_path)}")
    print(f"- task_hygiene_workflow={display_path(task_hygiene_workflow_path)}")
    print(f"- package_json={display_path(package_json_path)}")
    print(f"- checks_passed={checks_passed}")
    print("- fail_closed=true")
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_m137_lexer_contract.py",
        description=(
            "Fail-closed validator for M137 lexer subsystem extraction token-contract docs/CI/build wiring."
        ),
    )
    parser.add_argument(
        "--token-contract",
        type=Path,
        default=DEFAULT_TOKEN_CONTRACT,
        help="Path to native/objc3c/src/token/objc3_token_contract.h.",
    )
    parser.add_argument(
        "--ast-header",
        type=Path,
        default=DEFAULT_AST_HEADER,
        help="Path to native/objc3c/src/ast/objc3_ast.h.",
    )
    parser.add_argument(
        "--parser-source",
        type=Path,
        default=DEFAULT_PARSER_SOURCE,
        help="Path to native/objc3c/src/parse/objc3_parser.cpp.",
    )
    parser.add_argument(
        "--cmake",
        type=Path,
        default=DEFAULT_CMAKE,
        help="Path to native/objc3c/CMakeLists.txt.",
    )
    parser.add_argument(
        "--build-script",
        type=Path,
        default=DEFAULT_BUILD_SCRIPT,
        help="Path to scripts/build_objc3c_native.ps1.",
    )
    parser.add_argument(
        "--contract-doc",
        type=Path,
        default=DEFAULT_CONTRACT_DOC,
        help="Path to docs/contracts/lexer_token_contract_expectations.md.",
    )
    parser.add_argument(
        "--planning-lint-workflow",
        type=Path,
        default=DEFAULT_PLANNING_LINT_WORKFLOW,
        help="Path to .github/workflows/planning-lint.yml.",
    )
    parser.add_argument(
        "--task-hygiene-workflow",
        type=Path,
        default=DEFAULT_TASK_HYGIENE_WORKFLOW,
        help="Path to .github/workflows/task-hygiene.yml.",
    )
    parser.add_argument(
        "--package-json",
        type=Path,
        default=DEFAULT_PACKAGE_JSON,
        help="Path to package.json.",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        return check_contract(
            token_contract_path=resolve_input_path(args.token_contract),
            ast_header_path=resolve_input_path(args.ast_header),
            parser_source_path=resolve_input_path(args.parser_source),
            cmake_path=resolve_input_path(args.cmake),
            build_script_path=resolve_input_path(args.build_script),
            contract_doc_path=resolve_input_path(args.contract_doc),
            planning_lint_workflow_path=resolve_input_path(args.planning_lint_workflow),
            task_hygiene_workflow_path=resolve_input_path(args.task_hygiene_workflow),
            package_json_path=resolve_input_path(args.package_json),
        )
    except ValueError as exc:
        print(f"m137-lexer-contract: error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())

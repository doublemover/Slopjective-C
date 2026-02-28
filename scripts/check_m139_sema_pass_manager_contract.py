#!/usr/bin/env python3
"""Fail-closed validator for M139 sema pass-manager + diagnostics-bus wiring."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m139-sema-pass-manager-contract-v1"

DEFAULT_SEMA_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
DEFAULT_SEMA_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_PURE_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_pure_contract.cpp"
DEFAULT_PIPELINE_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
DEFAULT_PIPELINE_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "frontend_pipeline_contract.h"
DEFAULT_CMAKE = ROOT / "native" / "objc3c" / "CMakeLists.txt"
DEFAULT_BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"
DEFAULT_SEMANTICS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
DEFAULT_DIAGNOSTICS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "40-diagnostics.md"
DEFAULT_ARTIFACTS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DEFAULT_TESTS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
DEFAULT_CONTRACT_DOC = ROOT / "docs" / "contracts" / "sema_pass_manager_diagnostics_bus_expectations.md"
DEFAULT_PLANNING_LINT_WORKFLOW = ROOT / ".github" / "workflows" / "planning-lint.yml"
DEFAULT_TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"

PACKAGE_SCRIPT_NAME = "check:compiler-closeout:m139"
PACKAGE_TEST_SCRIPT_NAME = "test:objc3c:sema-pass-manager-diagnostics-bus"
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
    "sema_header",
    "sema_source",
    "pure_contract_source",
    "pipeline_types",
    "pipeline_source",
    "pipeline_contract_header",
    "cmake",
    "build_script",
    "semantics_fragment",
    "diagnostics_fragment",
    "artifacts_fragment",
    "tests_fragment",
    "contract_doc",
    "planning_lint_workflow",
    "task_hygiene_workflow",
    "package_json",
)
ARTIFACT_RANK = {artifact: index for index, artifact in enumerate(ARTIFACT_ORDER)}

REQUIRED_SNIPPETS: dict[str, tuple[SnippetRule, ...]] = {
    "sema_header": (
        SnippetRule("M139-SEM-01", '#include "sema/objc3_sema_contract.h"'),
        SnippetRule("M139-SEM-02", "BuildSemanticIntegrationSurface(const Objc3ParsedProgram &program,"),
        SnippetRule(
            "M139-SEM-03",
            "ValidateSemanticBodies(const Objc3ParsedProgram &program, const Objc3SemanticIntegrationSurface &surface,",
        ),
        SnippetRule("M139-SEM-04", "ValidatePureContractSemanticDiagnostics(const Objc3ParsedProgram &program,"),
    ),
    "sema_source": (
        SnippetRule(
            "M139-SEM-05",
            "Objc3SemanticIntegrationSurface BuildSemanticIntegrationSurface(const Objc3ParsedProgram &program,",
        ),
        SnippetRule(
            "M139-SEM-06",
            "void ValidateSemanticBodies(const Objc3ParsedProgram &program, const Objc3SemanticIntegrationSurface &surface,",
        ),
    ),
    "pure_contract_source": (
        SnippetRule("M139-SEM-07", "void ValidatePureContractSemanticDiagnostics(const Objc3ParsedProgram &program,"),
    ),
    "pipeline_types": (
        SnippetRule("M139-PIP-01", '#include "parse/objc3_diagnostics_bus.h"'),
        SnippetRule("M139-PIP-02", '#include "sema/objc3_sema_contract.h"'),
        SnippetRule("M139-PIP-03", "Objc3FrontendDiagnosticsBus stage_diagnostics;"),
        SnippetRule("M139-PIP-04", "std::array<std::size_t, 3> sema_diagnostics_after_pass = {0, 0, 0};"),
        SnippetRule("M139-PIP-05", "Objc3SemanticIntegrationSurface integration_surface;"),
    ),
    "pipeline_source": (
        SnippetRule("M139-PIP-06", '#include "sema/objc3_sema_pass_manager.h"'),
        SnippetRule("M139-PIP-07", "Objc3SemaPassManagerInput sema_input;"),
        SnippetRule("M139-PIP-08", "Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);"),
        SnippetRule("M139-PIP-09", "result.sema_diagnostics_after_pass = sema_result.diagnostics_after_pass;"),
        SnippetRule("M139-PIP-10", "TransportObjc3DiagnosticsToParsedProgram(result.stage_diagnostics, result.program);"),
    ),
    "pipeline_contract_header": (
        SnippetRule("M139-CON-01", "struct DiagnosticsEnvelope {"),
        SnippetRule("M139-CON-02", "struct SemaStageOutput {"),
        SnippetRule("M139-CON-03", "StageId::Sema"),
        SnippetRule("M139-CON-04", "SemaStageOutput sema_output;"),
    ),
    "cmake": (
        SnippetRule("M139-CMK-01", "add_library(objc3c_sema STATIC"),
        SnippetRule("M139-CMK-02", "src/sema/objc3_sema_diagnostics_bus.cpp"),
        SnippetRule("M139-CMK-03", "src/sema/objc3_sema_pass_manager.cpp"),
        SnippetRule("M139-CMK-04", "src/sema/objc3_semantic_passes.cpp"),
        SnippetRule("M139-CMK-05", "src/sema/objc3_static_analysis.cpp"),
        SnippetRule("M139-CMK-06", "src/sema/objc3_pure_contract.cpp"),
    ),
    "build_script": (
        SnippetRule("M139-BLD-01", '"native/objc3c/src/sema/objc3_sema_diagnostics_bus.cpp"'),
        SnippetRule("M139-BLD-02", '"native/objc3c/src/sema/objc3_sema_pass_manager.cpp"'),
        SnippetRule("M139-BLD-03", '"native/objc3c/src/sema/objc3_semantic_passes.cpp"'),
        SnippetRule("M139-BLD-04", '"native/objc3c/src/sema/objc3_static_analysis.cpp"'),
        SnippetRule("M139-BLD-05", '"native/objc3c/src/sema/objc3_pure_contract.cpp"'),
    ),
    "semantics_fragment": (
        SnippetRule("M139-DOCS-01", "## Sema pass manager + diagnostics bus contract (M139-E001)"),
        SnippetRule("M139-DOCS-02", "native/objc3c/src/sema/objc3_sema_pass_manager.h"),
    ),
    "diagnostics_fragment": (
        SnippetRule("M139-DOCD-01", "## Sema diagnostics bus contract (M139-E001)"),
        SnippetRule("M139-DOCD-02", "Objc3FrontendDiagnosticsBus"),
    ),
    "artifacts_fragment": (
        SnippetRule("M139-DOCA-01", "## Sema pass-manager + diagnostics bus validation artifacts (M139-E001)"),
    ),
    "tests_fragment": (
        SnippetRule("M139-DOCT-01", "npm run test:objc3c:sema-pass-manager-diagnostics-bus"),
        SnippetRule("M139-DOCT-02", "npm run check:compiler-closeout:m139"),
    ),
    "contract_doc": (
        SnippetRule("M139-DOC-01", "# Sema Pass Manager and Diagnostics Bus Contract Expectations (M139)"),
        SnippetRule("M139-DOC-02", "Contract ID: `objc3c-sema-pass-manager-diagnostics-bus-contract/m139-v1`"),
        SnippetRule("M139-DOC-03", "`python scripts/check_m139_sema_pass_manager_contract.py`"),
        SnippetRule("M139-DOC-04", "`npm run check:compiler-closeout:m139`"),
    ),
    "planning_lint_workflow": (
        SnippetRule("M139-CI-PLN-01", "python scripts/check_m139_sema_pass_manager_contract.py"),
    ),
    "task_hygiene_workflow": (
        SnippetRule("M139-CI-TH-01", "npm run check:compiler-closeout:m139"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[SnippetRule, ...]] = {
    "sema_source": (
        SnippetRule("M139-FORB-01", "void ValidatePureContractSemanticDiagnostics("),
    ),
    "pipeline_types": (
        SnippetRule("M139-FORB-02", "struct FunctionInfo {"),
        SnippetRule("M139-FORB-03", "struct Objc3SemanticIntegrationSurface {"),
        SnippetRule("M139-FORB-04", "struct Objc3FrontendStageDiagnostics {"),
    ),
    "sema_header": (
        SnippetRule("M139-FORB-05", '#include "ast/objc3_ast.h"'),
    ),
    "pipeline_source": (
        SnippetRule("M139-FORB-06", '#include "sema/objc3_semantic_passes.h"'),
    ),
}

PACKAGE_SCRIPT_REQUIRED_TOKENS: tuple[tuple[str, str], ...] = (
    ("M139-PKG-02", "python scripts/check_m139_sema_pass_manager_contract.py"),
    ("M139-PKG-03", "npm run test:objc3c:sema-pass-manager-diagnostics-bus"),
    ("M139-PKG-04", '--glob "docs/contracts/sema_pass_manager_diagnostics_bus_expectations.md"'),
)

PACKAGE_TEST_SCRIPT_REQUIRED_TOKENS: tuple[tuple[str, str], ...] = (
    ("M139-PKG-05", "python -m pytest"),
    ("M139-PKG-06", "tests/tooling/test_objc3c_sema_extraction.py"),
    ("M139-PKG-07", "tests/tooling/test_objc3c_parser_contract_sema_integration.py"),
    ("M139-PKG-08", "tests/tooling/test_objc3c_pure_contract_extraction.py"),
    ("M139-PKG-09", "tests/tooling/test_objc3c_frontend_types_extraction.py"),
)

PACKAGE_LANE_E_REQUIRED_TOKENS: tuple[tuple[str, str], ...] = (
    ("M139-PKG-10", "test:objc3c:sema-pass-manager-diagnostics-bus"),
)

PACKAGE_TASK_HYGIENE_REQUIRED_TOKENS: tuple[tuple[str, str], ...] = (
    ("M139-PKG-11", "check:compiler-closeout:m139"),
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
                check_id="M139-PKG-00",
                detail="package.json field 'scripts' must be an object.",
            )
        )
        return findings

    closeout_script = scripts.get(PACKAGE_SCRIPT_NAME)
    if not isinstance(closeout_script, str):
        findings.append(
            DriftFinding(
                artifact="package_json",
                check_id="M139-PKG-01",
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
                check_id="M139-PKG-12",
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
                check_id="M139-PKG-13",
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
                check_id="M139-PKG-14",
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
    sema_header: str,
    sema_source: str,
    pure_contract_source: str,
    pipeline_types: str,
    pipeline_source: str,
    pipeline_contract_header: str,
    cmake: str,
    build_script: str,
    semantics_fragment: str,
    diagnostics_fragment: str,
    artifacts_fragment: str,
    tests_fragment: str,
    contract_doc: str,
    planning_lint_workflow: str,
    task_hygiene_workflow: str,
    package_json_path: Path,
) -> list[DriftFinding]:
    findings: list[DriftFinding] = []
    surfaces = {
        "sema_header": sema_header,
        "sema_source": sema_source,
        "pure_contract_source": pure_contract_source,
        "pipeline_types": pipeline_types,
        "pipeline_source": pipeline_source,
        "pipeline_contract_header": pipeline_contract_header,
        "cmake": cmake,
        "build_script": build_script,
        "semantics_fragment": semantics_fragment,
        "diagnostics_fragment": diagnostics_fragment,
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
        "m139-sema-pass-manager-contract: contract drift detected "
        f"({len(ordered)} failed check(s)).",
        "drift findings:",
    ]
    for finding in ordered:
        lines.append(f"- {finding.artifact}:{finding.check_id}")
        lines.append(f"  {finding.detail}")
    lines.extend(
        [
            "remediation:",
            "1. Restore M139 sema pass-manager + diagnostics-bus contract snippets and gate wiring.",
            "2. Re-run validator:",
            " ".join(rerun_tokens),
        ]
    )
    return "\n".join(lines)


def check_contract(
    *,
    sema_header_path: Path,
    sema_source_path: Path,
    pure_contract_source_path: Path,
    pipeline_types_path: Path,
    pipeline_source_path: Path,
    pipeline_contract_header_path: Path,
    cmake_path: Path,
    build_script_path: Path,
    semantics_fragment_path: Path,
    diagnostics_fragment_path: Path,
    artifacts_fragment_path: Path,
    tests_fragment_path: Path,
    contract_doc_path: Path,
    planning_lint_workflow_path: Path,
    task_hygiene_workflow_path: Path,
    package_json_path: Path,
) -> int:
    findings = validate_contract(
        sema_header=load_text(sema_header_path, artifact="sema_header"),
        sema_source=load_text(sema_source_path, artifact="sema_source"),
        pure_contract_source=load_text(pure_contract_source_path, artifact="pure_contract_source"),
        pipeline_types=load_text(pipeline_types_path, artifact="pipeline_types"),
        pipeline_source=load_text(pipeline_source_path, artifact="pipeline_source"),
        pipeline_contract_header=load_text(pipeline_contract_header_path, artifact="pipeline_contract_header"),
        cmake=load_text(cmake_path, artifact="cmake"),
        build_script=load_text(build_script_path, artifact="build_script"),
        semantics_fragment=load_text(semantics_fragment_path, artifact="semantics_fragment"),
        diagnostics_fragment=load_text(diagnostics_fragment_path, artifact="diagnostics_fragment"),
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
            "scripts/check_m139_sema_pass_manager_contract.py",
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
    print("m139-sema-pass-manager-contract: OK")
    print(f"- mode={MODE}")
    print(f"- sema_header={display_path(sema_header_path)}")
    print(f"- sema_source={display_path(sema_source_path)}")
    print(f"- pure_contract_source={display_path(pure_contract_source_path)}")
    print(f"- pipeline_types={display_path(pipeline_types_path)}")
    print(f"- pipeline_source={display_path(pipeline_source_path)}")
    print(f"- pipeline_contract_header={display_path(pipeline_contract_header_path)}")
    print(f"- cmake={display_path(cmake_path)}")
    print(f"- build_script={display_path(build_script_path)}")
    print(f"- semantics_fragment={display_path(semantics_fragment_path)}")
    print(f"- diagnostics_fragment={display_path(diagnostics_fragment_path)}")
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
        prog="check_m139_sema_pass_manager_contract.py",
        description="Fail-closed validator for M139 sema pass-manager extraction + diagnostics-bus integration wiring.",
    )
    parser.add_argument("--sema-header", type=Path, default=DEFAULT_SEMA_HEADER)
    parser.add_argument("--sema-source", type=Path, default=DEFAULT_SEMA_SOURCE)
    parser.add_argument("--pure-contract-source", type=Path, default=DEFAULT_PURE_CONTRACT_SOURCE)
    parser.add_argument("--pipeline-types", type=Path, default=DEFAULT_PIPELINE_TYPES)
    parser.add_argument("--pipeline-source", type=Path, default=DEFAULT_PIPELINE_SOURCE)
    parser.add_argument("--pipeline-contract-header", type=Path, default=DEFAULT_PIPELINE_CONTRACT_HEADER)
    parser.add_argument("--cmake", type=Path, default=DEFAULT_CMAKE)
    parser.add_argument("--build-script", type=Path, default=DEFAULT_BUILD_SCRIPT)
    parser.add_argument("--semantics-fragment", type=Path, default=DEFAULT_SEMANTICS_FRAGMENT)
    parser.add_argument("--diagnostics-fragment", type=Path, default=DEFAULT_DIAGNOSTICS_FRAGMENT)
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
            sema_header_path=resolve_input_path(args.sema_header),
            sema_source_path=resolve_input_path(args.sema_source),
            pure_contract_source_path=resolve_input_path(args.pure_contract_source),
            pipeline_types_path=resolve_input_path(args.pipeline_types),
            pipeline_source_path=resolve_input_path(args.pipeline_source),
            pipeline_contract_header_path=resolve_input_path(args.pipeline_contract_header),
            cmake_path=resolve_input_path(args.cmake),
            build_script_path=resolve_input_path(args.build_script),
            semantics_fragment_path=resolve_input_path(args.semantics_fragment),
            diagnostics_fragment_path=resolve_input_path(args.diagnostics_fragment),
            artifacts_fragment_path=resolve_input_path(args.artifacts_fragment),
            tests_fragment_path=resolve_input_path(args.tests_fragment),
            contract_doc_path=resolve_input_path(args.contract_doc),
            planning_lint_workflow_path=resolve_input_path(args.planning_lint_workflow),
            task_hygiene_workflow_path=resolve_input_path(args.task_hygiene_workflow),
            package_json_path=resolve_input_path(args.package_json),
        )
    except ValueError as exc:
        print(f"m139-sema-pass-manager-contract: error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())

#!/usr/bin/env python3
"""Fail-closed validator for M140 frontend-library boundary extraction wiring."""

from __future__ import annotations

import json
import sys
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MODE = "m140-frontend-library-boundary-contract-v1"

ARTIFACT_PATHS: dict[str, Path] = {
    "frontend_anchor": ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp",
    "frontend_api": ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "api.h",
    "cli_frontend_header": ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "objc3_cli_frontend.h",
    "cli_frontend_source": ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "objc3_cli_frontend.cpp",
    "sema_contract": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h",
    "sema_pass_manager_contract": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h",
    "sema_pass_manager_source": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp",
    "lowering_contract_header": ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h",
    "lowering_contract_source": ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp",
    "ir_emitter_source": ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp",
    "semantics_fragment": ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md",
    "artifacts_fragment": ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md",
    "tests_fragment": ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md",
    "contract_doc": ROOT / "docs" / "contracts" / "frontend_library_boundary_expectations.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "frontend_anchor": (
        ("M140-LIB-01", "CompileObjc3SourceWithPipeline(input_path, source_text, frontend_options)"),
        ("M140-LIB-02", "OBJC3C_FRONTEND_STATUS_DIAGNOSTICS"),
        ("M140-LIB-03", "BuildDiagnosticsJson("),
        ("M140-LIB-04", "RunIrCompile("),
    ),
    "frontend_api": (
        ("M140-LIB-05", "Pipeline-backed behavior:"),
        ("M140-LIB-06", "Runs lexer/parser/sema/lower/emit through the extracted frontend pipeline."),
    ),
    "cli_frontend_header": (
        ("M140-LIB-07", "struct Objc3FrontendCompileProduct"),
        ("M140-LIB-08", "Objc3FrontendCompileProduct CompileObjc3SourceWithPipeline("),
    ),
    "cli_frontend_source": (
        ("M140-LIB-09", "Objc3FrontendCompileProduct CompileObjc3SourceWithPipeline("),
        ("M140-LIB-10", "product.pipeline_result = RunObjc3FrontendPipeline(source, options);"),
    ),
    "sema_contract": (
        ("M140-SEM-01", "struct Objc3SemanticTypeMetadataHandoff {"),
        ("M140-SEM-02", "BuildSemanticTypeMetadataHandoff(const Objc3SemanticIntegrationSurface &surface)"),
    ),
    "sema_pass_manager_contract": (
        ("M140-SEM-03", "std::array<std::size_t, 3> diagnostics_emitted_by_pass = {0, 0, 0};"),
        ("M140-SEM-04", "Objc3SemanticTypeMetadataHandoff type_metadata_handoff;"),
    ),
    "sema_pass_manager_source": (
        ("M140-SEM-05", "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);"),
        ("M140-SEM-06", "result.deterministic_type_metadata_handoff ="),
    ),
    "lowering_contract_header": (
        ("M140-LOW-01", "struct Objc3LoweringIRBoundary {"),
        ("M140-LOW-02", "Objc3LoweringIRBoundaryReplayKey("),
    ),
    "lowering_contract_source": (
        ("M140-LOW-03", "TryBuildObjc3LoweringIRBoundary("),
        ("M140-LOW-04", "Objc3LoweringIRBoundaryReplayKey("),
    ),
    "ir_emitter_source": (
        ("M140-IR-01", "TryBuildObjc3LoweringIRBoundary(lowering_contract, lowering_ir_boundary_, boundary_error_)"),
        ("M140-IR-02", "ValidateMessageSendArityContract("),
        ("M140-IR-03", "; lowering_ir_boundary = "),
    ),
    "semantics_fragment": (
        ("M140-DOCS-01", "## Frontend library boundary contract (M140-E001)"),
        ("M140-DOCS-02", "native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp"),
    ),
    "artifacts_fragment": (
        ("M140-DOCA-01", "## Frontend library boundary validation artifacts (M140-E001)"),
        ("M140-DOCA-02", "npm run check:compiler-closeout:m140"),
    ),
    "tests_fragment": (
        ("M140-DOCT-01", "npm run test:objc3c:m140-boundary-contract"),
        ("M140-DOCT-02", "npm run check:compiler-closeout:m140"),
    ),
    "contract_doc": (
        ("M140-DOC-01", "# Frontend Library Boundary Contract Expectations (M140)"),
        ("M140-DOC-02", "Contract ID: `objc3c-frontend-library-boundary-contract/m140-v1`"),
        ("M140-DOC-03", "`python scripts/check_m140_frontend_library_boundary_contract.py`"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "frontend_anchor": (
        ("M140-FORB-01", "libobjc3c_frontend compile entrypoints are scaffolded only"),
    ),
    "frontend_api": (
        ("M140-FORB-02", "Current implementation status: scaffolded"),
    ),
}

REQUIRED_PACKAGE_SCRIPTS = {
    "check:compiler-closeout:m140": (
        "python scripts/check_m140_frontend_library_boundary_contract.py",
        "npm run test:objc3c:m140-boundary-contract",
        '--glob "docs/contracts/frontend_library_boundary_expectations.md"',
    ),
    "test:objc3c:m140-boundary-contract": (
        "tests/tooling/test_objc3c_frontend_library_entrypoint_extraction.py",
        "tests/tooling/test_objc3c_m140_boundary_contract.py",
        "tests/tooling/test_objc3c_sema_extraction.py",
        "tests/tooling/test_objc3c_sema_pass_manager_extraction.py",
        "tests/tooling/test_objc3c_lowering_contract.py",
        "tests/tooling/test_objc3c_ir_emitter_extraction.py",
    ),
    "check:task-hygiene": (
        "check:compiler-closeout:m140",
    ),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def _display(path: Path) -> str:
    try:
        return path.resolve().relative_to(ROOT).as_posix()
    except ValueError:
        return path.resolve().as_posix()


def _load_text(path: Path, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {_display(path)}")
    return path.read_text(encoding="utf-8")


def _collect_text_findings(artifact: str, text: str) -> list[Finding]:
    findings: list[Finding] = []
    for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
        if snippet not in text:
            findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))
    for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
        if snippet in text:
            findings.append(Finding(artifact, check_id, f"forbidden snippet present: {snippet}"))
    return findings


def _collect_package_findings(package_json_path: Path) -> list[Finding]:
    payload = json.loads(_load_text(package_json_path, "package_json"))
    scripts = payload.get("scripts")
    findings: list[Finding] = []
    if not isinstance(scripts, dict):
        return [Finding("package_json", "M140-PKG-00", "package.json scripts field must be an object")]

    for script_name, required_tokens in REQUIRED_PACKAGE_SCRIPTS.items():
        script_value = scripts.get(script_name)
        if not isinstance(script_value, str):
            findings.append(Finding("package_json", "M140-PKG-01", f"missing scripts['{script_name}']"))
            continue
        for index, token in enumerate(required_tokens, start=1):
            if token not in script_value:
                findings.append(
                    Finding(
                        "package_json",
                        f"M140-PKG-{script_name}-{index}",
                        f"scripts['{script_name}'] missing token: {token}",
                    )
                )
    return findings


def check() -> int:
    findings: list[Finding] = []

    for artifact, path in ARTIFACT_PATHS.items():
        if artifact == "package_json":
            continue
        text = _load_text(path, artifact)
        findings.extend(_collect_text_findings(artifact, text))

    findings.extend(_collect_package_findings(ARTIFACT_PATHS["package_json"]))
    findings.sort(key=lambda f: (f.artifact, f.check_id))

    if findings:
        print(
            f"m140-frontend-library-boundary-contract: contract drift detected ({len(findings)} finding(s)).",
            file=sys.stderr,
        )
        for finding in findings:
            print(f"- {finding.artifact}:{finding.check_id} {finding.detail}", file=sys.stderr)
        print("remediation:", file=sys.stderr)
        print("1. Restore M140 source/docs/package contract snippets.", file=sys.stderr)
        print("2. Re-run: python scripts/check_m140_frontend_library_boundary_contract.py", file=sys.stderr)
        return 1

    checks_passed = sum(len(v) for v in REQUIRED_SNIPPETS.values()) + sum(len(v) for v in FORBIDDEN_SNIPPETS.values())
    checks_passed += sum(len(v) for v in REQUIRED_PACKAGE_SCRIPTS.values())

    print("m140-frontend-library-boundary-contract: OK")
    print(f"- mode={MODE}")
    for artifact, path in ARTIFACT_PATHS.items():
        print(f"- {artifact}={_display(path)}")
    print(f"- checks_passed={checks_passed}")
    print("- fail_closed=true")
    return 0


def main() -> int:
    try:
        return check()
    except Exception as exc:  # noqa: BLE001
        print(f"m140-frontend-library-boundary-contract: error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())

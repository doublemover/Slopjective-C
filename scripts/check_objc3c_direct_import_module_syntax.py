#!/usr/bin/env python3
"""Validate direct @import syntax and package provenance contracts."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_package_manager.model import (  # noqa: E402
    DIRECT_IMPORT_PACKAGE_PROVENANCE,
    DIRECT_IMPORT_SYNTAX_SUPPORT,
    PACKAGE_MANAGER_TAMPER_CODE,
    build_lock_components,
    collect_package_module_graph_failures,
)
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file  # noqa: E402
from objc3c_tooling.paths import repo_rel  # noqa: E402

CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "package_ecosystem"
    / "direct_import_module_syntax_contract.json"
)
PACKAGE_MODEL_CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "package_ecosystem"
    / "package_manager_model_contract.json"
)
NEGATIVE_METADATA_CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "package_ecosystem"
    / "negative_package_metadata_contracts.json"
)
MODULE_INVENTORY_PATH = ROOT / "stdlib" / "module_inventory.json"
SHOWCASE_PORTFOLIO_PATH = ROOT / "showcase" / "portfolio.json"
SUMMARY_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "package-ecosystem"
    / "direct-import-module-syntax-summary.json"
)

SOURCE_TOKEN_REQUIREMENTS = (
    (
        "native/objc3c/src/token/objc3_token_kind_contract.h",
        ("KwAtImport",),
    ),
    (
        "native/objc3c/src/token/objc3_token_kind.cpp",
        ("case Objc3LexTokenKind::KwAtImport:",),
    ),
    (
        "native/objc3c/src/token/objc3_token_keyword_data.cpp",
        ('{"import", Objc3LexTokenKind::KwAtImport}',),
    ),
    (
        "native/objc3c/src/ast/objc3_ast_declarations.h",
        ("Objc3ModuleImportDecl", "std::vector<Objc3ModuleImportDecl> module_imports;"),
    ),
    (
        "native/objc3c/src/parse/objc3_ast_builder.h",
        ("AddModuleImport",),
    ),
    (
        "native/objc3c/src/parse/objc3_ast_builder.cpp",
        ("AddModuleImport",),
    ),
    (
        "native/objc3c/src/parse/objc3_parser_core_interop_profiles_top_level_dispatch.inc",
        ("ParseDirectModuleImport(program)",),
    ),
    (
        "native/objc3c/src/parse/objc3_parser_core_cstyle_parameters_module_global_parsing.inc",
        ('"O3MOD8219"',),
    ),
    (
        "native/objc3c/src/parse/objc3_parser_recovery_boundaries.cpp",
        ("KwAtImport",),
    ),
    (
        "scripts/objc3c_package_manager/model.py",
        (
            'DIRECT_IMPORT_SYNTAX_SUPPORT = "supported-fail-closed"',
            'DIRECT_IMPORT_PACKAGE_PROVENANCE = "locked-package"',
            '"missing_provenance_diagnostic": PACKAGE_MANAGER_TAMPER_CODE',
            "ambiguous direct @import module identity",
        ),
    ),
    (
        "scripts/check_objc3c_package_manager_model.py",
        ("DIRECT_IMPORT_SYNTAX_SUPPORT",),
    ),
    (
        "scripts/objc3c_package_manager/trust.py",
        ('PACKAGE_MANAGER_TAMPER_CODE = "O3PKG8055"',),
    ),
)


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _fixture_path(raw_path: object, failures: list[str]) -> Path:
    raw = str(raw_path)
    expect(raw != "", "empty direct @import fixture path", failures)
    expect(
        not raw.startswith(("tmp/", "temp/", "artifacts/")),
        f"direct @import fixture escaped source truth: {raw}",
        failures,
    )
    path = ROOT / raw
    expect(path.is_file(), f"missing direct @import fixture: {raw}", failures)
    return path


def _validate_contract(contract: dict[str, Any], failures: list[str]) -> None:
    expect(
        contract.get("contract_id")
        == "objc3c.package_ecosystem.direct_import_module_syntax.v1",
        "direct @import contract id drifted",
        failures,
    )
    expect(8219 in contract.get("issue_refs", []), "direct @import issue ref #8219 missing", failures)
    expect(
        set(contract.get("diagnostic_codes", [])) == {"O3MOD8219", PACKAGE_MANAGER_TAMPER_CODE},
        "direct @import diagnostic code set drifted",
        failures,
    )

    token_contract = contract.get("token_contract", {})
    parser_contract = contract.get("parser_contract", {})
    module_graph_contract = contract.get("module_graph_contract", {})
    expect(isinstance(token_contract, dict), "direct @import token contract missing", failures)
    expect(isinstance(parser_contract, dict), "direct @import parser contract missing", failures)
    expect(isinstance(module_graph_contract, dict), "direct @import module graph contract missing", failures)
    if isinstance(token_contract, dict):
        expect(token_contract.get("token_kind") == "KwAtImport", "direct @import token kind drifted", failures)
        expect(token_contract.get("spelling") == "@import", "direct @import spelling drifted", failures)
    if isinstance(parser_contract, dict):
        expect(parser_contract.get("ast_record") == "Objc3ModuleImportDecl", "direct @import AST record drifted", failures)
        expect(parser_contract.get("malformed_diagnostic") == "O3MOD8219", "direct @import parser diagnostic drifted", failures)
    if isinstance(module_graph_contract, dict):
        expect(
            module_graph_contract.get("direct_import_syntax") == DIRECT_IMPORT_SYNTAX_SUPPORT,
            "direct @import module graph support state drifted",
            failures,
        )
        expect(
            module_graph_contract.get("required_package_provenance")
            == DIRECT_IMPORT_PACKAGE_PROVENANCE,
            "direct @import package provenance drifted",
            failures,
        )
        expect(
            module_graph_contract.get("missing_provenance_diagnostic")
            == PACKAGE_MANAGER_TAMPER_CODE,
            "direct @import provenance diagnostic drifted",
            failures,
        )


def _validate_fixture_paths(contract: dict[str, Any], failures: list[str]) -> tuple[int, int]:
    positive_count = 0
    positive_import_count = 0
    for raw_path in contract.get("positive_fixtures", []):
        text = _read(_fixture_path(raw_path, failures))
        if "@import " in text:
            positive_import_count += 1
        positive_count += 1
    expect(positive_import_count > 0, "direct @import positive fixtures contain no direct import syntax", failures)

    negative_count = 0
    for case in contract.get("negative_fixtures", []):
        expect(isinstance(case, dict), "direct @import negative fixture record is not an object", failures)
        if not isinstance(case, dict):
            continue
        path = _fixture_path(case.get("path"), failures)
        text = _read(path)
        expected = str(case.get("expected_diagnostic", ""))
        expect(expected in {"O3MOD8219", PACKAGE_MANAGER_TAMPER_CODE}, f"direct @import negative diagnostic drifted for {repo_rel(path)}", failures)
        expect("@import" in text, f"direct @import negative fixture missing syntax: {repo_rel(path)}", failures)
        negative_count += 1
    return positive_count, negative_count


def _validate_source_tokens(failures: list[str]) -> None:
    for raw_path, tokens in SOURCE_TOKEN_REQUIREMENTS:
        path = ROOT / raw_path
        expect(path.is_file(), f"missing direct @import source: {raw_path}", failures)
        if not path.is_file():
            continue
        text = _read(path)
        for token in tokens:
            expect(token in text, f"direct @import source token missing from {raw_path}: {token}", failures)


def _validate_negative_metadata_contract(failures: list[str]) -> None:
    payload = load_json(NEGATIVE_METADATA_CONTRACT_PATH)
    cases = payload.get("cases", [])
    expect(isinstance(cases, list), "direct @import negative metadata cases missing", failures)
    if not isinstance(cases, list):
        return
    case_ids = {str(case.get("case_id")) for case in cases if isinstance(case, dict)}
    for required in (
        "direct-import-syntax-contract-drift",
        "missing-direct-import-package-provenance",
        "ambiguous-direct-import-module-identity",
    ):
        expect(required in case_ids, f"missing direct @import negative metadata case: {required}", failures)


def _validate_source_built_module_graphs(failures: list[str]) -> int:
    module_inventory = load_json(MODULE_INVENTORY_PATH)
    showcase_portfolio = load_json(SHOWCASE_PORTFOLIO_PATH)
    components = build_lock_components(
        root=ROOT,
        module_inventory=module_inventory,
        showcase_portfolio=showcase_portfolio,
    )
    packages = [package for package in components["packages"] if isinstance(package, dict)]
    dependencies = [
        dependency for dependency in components["dependencies"] if isinstance(dependency, dict)
    ]
    dependency_map: dict[str, list[dict[str, Any]]] = {}
    for dependency in dependencies:
        dependency_map.setdefault(str(dependency.get("from", "")), []).append(dependency)

    direct_import_count = 0
    for package in packages:
        package_failures = collect_package_module_graph_failures(
            package,
            dependencies=dependency_map.get(str(package.get("package_id", "")), []),
            root=ROOT,
        )
        failures.extend(package_failures)
        graph = package.get("module_graph", {})
        if isinstance(graph, dict):
            imports = graph.get("direct_imports", [])
            if isinstance(imports, list):
                direct_import_count += len(imports)
    expect(direct_import_count > 0, "source-built package graph has no direct @import records", failures)
    return direct_import_count


def main() -> int:
    failures: list[str] = []
    contract = load_json(CONTRACT_PATH)
    package_model_contract = load_json(PACKAGE_MODEL_CONTRACT_PATH)

    _validate_contract(contract, failures)
    positive_count, negative_count = _validate_fixture_paths(contract, failures)
    _validate_source_tokens(failures)
    _validate_negative_metadata_contract(failures)
    direct_import_count = _validate_source_built_module_graphs(failures)

    required_actions = set(package_model_contract.get("required_public_actions", []))
    expect(
        "validate-direct-import-module-syntax" in required_actions,
        "direct @import public replay action missing from package manager contract",
        failures,
    )

    payload = {
        "contract_id": "objc3c.package_ecosystem.direct_import_module_syntax.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "contract": repo_rel(CONTRACT_PATH),
        "issue_refs": contract.get("issue_refs", []),
        "diagnostic_codes": contract.get("diagnostic_codes", []),
        "direct_import_syntax": DIRECT_IMPORT_SYNTAX_SUPPORT,
        "package_provenance": DIRECT_IMPORT_PACKAGE_PROVENANCE,
        "positive_fixture_count": positive_count,
        "negative_fixture_count": negative_count,
        "source_built_direct_import_count": direct_import_count,
        "public_replay_action": "npm run objc3c -- validate-direct-import-module-syntax",
        "claim_boundary": contract.get("claim_boundary"),
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload, sort_keys=True)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    if failures:
        print("objc3c-direct-import-module-syntax: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("objc3c-direct-import-module-syntax: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

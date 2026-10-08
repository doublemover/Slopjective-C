from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from scripts.objc3c_workflow.action_catalog_package_lock import PACKAGE_LOCK_ACTION_SPECS
from scripts.objc3c_workflow.action_catalog_package_lock_contracts import PACKAGE_LOCK_PUBLIC_ACTIONS
from scripts.objc3c_workflow.actions.ecosystem_publication_owner_contracts import (
    ecosystem_publication_owner_contract,
)


ROOT = Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "tests" / "tooling" / "fixtures" / "package_ecosystem" / "direct_import_module_syntax_contract.json"
TOKEN_KIND_CONTRACT = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_kind_contract.h"
TOKEN_KIND_SOURCE = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_kind.cpp"
TOKEN_KEYWORD_DATA = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_keyword_data.cpp"
AST_DECLARATIONS = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast_declarations.h"
AST_BUILDER_HEADER = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_ast_builder.h"
AST_BUILDER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_ast_builder.cpp"
MODULE_PARSER = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_core_cstyle_parameters_module_global_parsing.inc"
TOP_LEVEL_DISPATCH = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_core_interop_profiles_top_level_dispatch.inc"
RECOVERY_BOUNDARIES = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_recovery_boundaries.cpp"
PACKAGE_MODEL = ROOT / "scripts" / "objc3c_package_manager" / "model.py"
PACKAGE_TRUST = ROOT / "scripts" / "objc3c_package_manager" / "trust.py"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def test_direct_import_token_parser_and_ast_contract_are_source_owned() -> None:
    contract = _read_json(CONTRACT)
    token_contract = contract["token_contract"]
    parser_contract = contract["parser_contract"]

    assert token_contract["token_kind"] in _read(TOKEN_KIND_CONTRACT)
    assert 'case Objc3LexTokenKind::KwAtImport:' in _read(TOKEN_KIND_SOURCE)
    assert '{"import", Objc3LexTokenKind::KwAtImport}' in _read(TOKEN_KEYWORD_DATA)
    assert parser_contract["ast_record"] in _read(AST_DECLARATIONS)
    assert "std::vector<Objc3ModuleImportDecl> module_imports;" in _read(AST_DECLARATIONS)
    assert "AddModuleImport" in _read(AST_BUILDER_HEADER)
    assert "AddModuleImport" in _read(AST_BUILDER_SOURCE)
    assert "ParseDirectModuleImport(program)" in _read(TOP_LEVEL_DISPATCH)
    assert '"O3MOD8219"' in _read(MODULE_PARSER)
    assert "KwAtImport" in _read(RECOVERY_BOUNDARIES)


def test_direct_import_fixtures_bind_syntax_to_package_provenance_contract() -> None:
    contract = _read_json(CONTRACT)
    positive_paths = [str(path) for path in contract["positive_fixtures"]]
    negative_paths = [str(case["path"]) for case in contract["negative_fixtures"]]

    for path in [*positive_paths, *negative_paths]:
        assert not path.startswith(("tmp/", "temp/"))
        assert (ROOT / path).is_file(), path

    consumer = _read(ROOT / "tests" / "tooling" / "fixtures" / "native" / "module_import_lookup_consumer.objc3")
    assert "@import moduleImportLookupProvider;" in consumer
    assert contract["module_graph_contract"]["direct_import_syntax"] in _read(PACKAGE_MODEL)
    assert contract["module_graph_contract"]["required_package_provenance"] in _read(PACKAGE_MODEL)
    assert '"missing_provenance_diagnostic": PACKAGE_MANAGER_TAMPER_CODE' in _read(PACKAGE_MODEL)
    assert (
        f'PACKAGE_MANAGER_TAMPER_CODE = "{contract["module_graph_contract"]["missing_provenance_diagnostic"]}"'
        in _read(PACKAGE_TRUST)
    )


def test_direct_import_public_action_is_registered_and_owned() -> None:
    assert "validate-direct-import-module-syntax" in PACKAGE_LOCK_ACTION_SPECS
    action = PACKAGE_LOCK_ACTION_SPECS["validate-direct-import-module-syntax"]
    assert action.backend == "python:scripts/check_objc3c_direct_import_module_syntax.py"
    assert any(
        public.action == "validate-direct-import-module-syntax"
        for public in PACKAGE_LOCK_PUBLIC_ACTIONS
    )

    contract = ecosystem_publication_owner_contract("validate-direct-import-module-syntax")
    assert contract.owner_role == "package-ecosystem-direct-import-owner"
    assert "direct_import_module_syntax_contract.json" in " ".join(contract.source_contracts)
    assert not contract.wrapper_only_allowed

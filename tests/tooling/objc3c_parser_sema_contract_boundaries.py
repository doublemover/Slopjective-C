from objc3c_parser_sema_integration_support import (
    PARSER_CONTRACT,
    PARSER_CORE_BODY,
    PARSER_CORE_PRELUDE,
    PASS_MANAGER_CONTRACT,
    SEMA_CONTRACT,
    SEMA_HEADER,
    SEMA_PASS_MANAGER,
    assert_contains_all,
    read_source,
)


def test_parser_contract_exports_parsed_program_aliases() -> None:
    contract = read_source(PARSER_CONTRACT)
    assert_contains_all(
        contract,
        (
            "struct Objc3ParsedProgram {",
            "Objc3Program ast;",
            "using Objc3ParsedGlobalDecl = GlobalDecl;",
            "using Objc3ParsedFunctionDecl = FunctionDecl;",
            "MutableObjc3ParsedProgramAst(",
            "Objc3ParsedProgramAst(",
        ),
    )


def test_parser_core_uses_ast_builder_scaffold() -> None:
    prelude = read_source(PARSER_CORE_PRELUDE)
    parser_core = read_source(PARSER_CORE_BODY)
    assert '#include "parse/objc3_ast_builder.h"' in prelude
    assert_contains_all(
        parser_core,
        (
            "Objc3AstBuilder ast_builder_;",
            "ast_builder_.BeginProgram()",
            "ast_builder_.AddGlobalDecl(program, std::move(*decl));",
            "ast_builder_.AddFunctionDecl(program, std::move(*fn));",
        ),
    )


def test_sema_header_consumes_parser_contract_outputs() -> None:
    sema = read_source(SEMA_HEADER)
    assert '#include "sema/objc3_sema_contract.h"' in sema
    assert '#include "parse/objc3_parser_contract.h"' not in sema
    assert '#include "ast/objc3_ast.h"' not in sema
    assert_contains_all(
        sema,
        (
            "BuildSemanticIntegrationSurface(const Objc3ParsedProgram &program,",
            "ValidatePureContractSemanticDiagnostics(const Objc3ParsedProgram &program,",
            "ValidateSemanticBodies(const Objc3ParsedProgram &program, const Objc3SemanticIntegrationSurface &surface,",
        ),
    )


def test_parser_to_sema_type_metadata_handoff_contract_is_explicit() -> None:
    sema_contract = read_source(SEMA_CONTRACT)
    sema_header = read_source(SEMA_HEADER)
    pass_manager_contract = read_source(PASS_MANAGER_CONTRACT)

    assert_contains_all(
        sema_contract,
        (
            "struct Objc3SemanticTypeMetadataHandoff {",
            "Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(",
            "bool IsDeterministicSemanticTypeMetadataHandoff(",
        ),
    )
    assert_contains_all(
        sema_header,
        (
            "BuildSemanticTypeMetadataHandoff(const Objc3SemanticIntegrationSurface &surface);",
            "IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff);",
        ),
    )
    assert_contains_all(
        pass_manager_contract,
        (
            "Objc3SemanticTypeMetadataHandoff type_metadata_handoff;",
            "bool deterministic_semantic_diagnostics = false;",
            "bool deterministic_type_metadata_handoff = false;",
            "std::array<std::size_t, 3> diagnostics_emitted_by_pass = {0, 0, 0};",
        ),
    )


def test_parser_to_sema_recovery_determinism_hardening_gate_is_explicit() -> None:
    sema_pass_manager = read_source(SEMA_PASS_MANAGER)
    pass_manager_contract = read_source(PASS_MANAGER_CONTRACT)

    assert_contains_all(
        sema_pass_manager,
        (
            "AreEquivalentBlockDeterminismPerfBaselineSites(",
            "const bool recovery_and_block_determinism_hardening_consistent =",
            "if (!recovery_and_block_determinism_hardening_consistent) {",
        ),
    )
    assert_contains_all(
        pass_manager_contract,
        (
            ".fail_closed_diagnostic_sites <=",
            ".diagnostic_emit_sites &&",
        ),
    )

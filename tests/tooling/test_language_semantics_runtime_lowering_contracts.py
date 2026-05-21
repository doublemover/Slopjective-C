from __future__ import annotations

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SEMA_CONTRACT_HEADER = (
    ROOT
    / "native"
    / "objc3c"
    / "src"
    / "sema"
    / "model"
    / "language_semantics_concrete_contracts.h"
)
LOWERING_HANDOFF_HEADER = (
    ROOT
    / "native"
    / "objc3c"
    / "src"
    / "lower"
    / "model"
    / "language_semantics_lowering_handoff.h"
)
LOWERED_MODULE_HEADER = (
    ROOT
    / "native"
    / "objc3c"
    / "src"
    / "lower"
    / "model"
    / "lowered_module_pipeline_surfaces.h"
)
COMPILE_PHASE_RESULT_HEADER = (
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "results" / "compile_phase_results.h"
)
PIPELINE_RESULT_HEADER = (
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "results" / "pipeline_result_model.h"
)
RUNTIME_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "execution"
    / "positive"
    / "language_semantics_public_runtime_contract.objc3"
)
RUNTIME_EXIT_CODE = RUNTIME_FIXTURE.with_suffix(".exitcode.txt")
GENERIC_METHOD_SUBSTITUTION_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "type_semantic_generic_method_substitution_positive.objc3"
)
GENERIC_FUNCTION_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "type_semantic_generic_function_positive.objc3"
)
AWAIT_OUTSIDE_ASYNC_NEGATIVE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "recovery"
    / "negative"
    / "negative_language_semantics_await_outside_async.objc3"
)
EXECUTOR_ON_SYNC_NEGATIVE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "recovery"
    / "negative"
    / "negative_language_semantics_executor_on_sync_function.objc3"
)
CONFLICTING_CAPTURE_NEGATIVE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "recovery"
    / "negative"
    / "negative_language_semantics_conflicting_capture_ownership.objc3"
)
PROTOCOL_ASSOCIATED_TYPE_NEGATIVE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "recovery"
    / "negative"
    / "negative_protocol_existential_associated_type_rejected.objc3"
)
PROTOCOL_DYNAMIC_DISPATCH_NEGATIVE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "recovery"
    / "negative"
    / "negative_protocol_existential_dynamic_dispatch_rejected.objc3"
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_language_semantics_concrete_contracts_bind_issue_fixtures_to_phases() -> None:
    header = _read(SEMA_CONTRACT_HEADER)

    for token in (
        "kObjc3LanguageSemanticsConcreteContractId",
        "Objc3LanguageSemanticsConcreteFixtureContract",
        "AllObjc3LanguageSemanticsConcreteFixtureContractsReady",
        "tests/tooling/fixtures/native/type_semantic_protocol_generic_positive.objc3",
        "tests/tooling/fixtures/native/type_semantic_generic_method_substitution_positive.objc3",
        "tests/tooling/fixtures/native/protocol_qualified_existential_value_flow.objc3",
        "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_protocol_qualified_unknown_message.objc3",
        "tests/tooling/fixtures/native/recovery/negative/negative_protocol_existential_associated_type_rejected.objc3",
        "tests/tooling/fixtures/native/recovery/negative/negative_protocol_existential_dynamic_dispatch_rejected.objc3",
        "tests/tooling/fixtures/native/execution/positive/escaping_owned_object_block_copy_dispose.objc3",
        "tests/tooling/fixtures/native/recovery/negative/negative_language_semantics_conflicting_capture_ownership.objc3",
        "tests/native/sema/concurrency/async_await_value_flow.objc3",
        "tests/tooling/fixtures/native/recovery/negative/negative_language_semantics_await_outside_async.objc3",
        "tests/tooling/fixtures/native/recovery/negative/negative_language_semantics_executor_on_sync_function.objc3",
        "tests/tooling/fixtures/native/execution/positive/language_semantics_public_runtime_contract.objc3",
        "O3S216",
        "O3P100",
        "O3S314",
        "O3S301",
        "O3S223",
        "O3S224",
    ):
        assert token in header

    for issue in ("8160", "8164", "8166", "8167"):
        assert issue in header


def test_language_semantics_lowering_handoff_is_exposed_to_phase_results() -> None:
    handoff = _read(LOWERING_HANDOFF_HEADER)
    lowered_module = _read(LOWERED_MODULE_HEADER)
    compile_phase = _read(COMPILE_PHASE_RESULT_HEADER)
    pipeline_result = _read(PIPELINE_RESULT_HEADER)

    for token in (
        "generic-specialization-metadata",
        "protocol-witness-conformance-metadata",
        "ownership-cleanup-transfer-metadata",
        "concurrency-executor-effect-metadata",
        "IsReadyObjc3LanguageSemanticsLoweringHandoffSurface",
        "AllObjc3LanguageSemanticsConcreteFixtureContractsReady",
    ):
        assert token in handoff

    assert "lower/model/language_semantics_lowering_handoff.h" in lowered_module
    assert "language_semantics_lowering_handoff_surface" in compile_phase
    assert "language_semantics_lowering_handoff_surface" in pipeline_result


def test_language_semantics_runtime_fixture_is_concrete_and_not_model_only() -> None:
    fixture = _read(RUNTIME_FIXTURE)

    for token in (
        "@interface SemanticVault<__covariant T : id<Persistable>>",
        "SemanticVault<SemanticBox *>",
        "SemanticVault<id<Persistable>>",
        "__weak id",
        "async fn runTask()",
        "__attribute__((objc_executor(main)))",
        "return consumeConcreteVault(nil) + ownedBlockScore(11, 11) + runTask();",
    ):
        assert token in fixture

    assert RUNTIME_EXIT_CODE.read_text(encoding="utf-8").strip() == "31"


def test_language_semantics_generic_method_fixture_exercises_receiver_substitution() -> None:
    fixture = _read(GENERIC_METHOD_SUBSTITUTION_FIXTURE)

    for token in (
        "@interface SemanticVault<__covariant T : id<Persistable>>",
        "- (T)peek;",
        "fn consumeGenericMethod(vault: SemanticVault<SemanticBox *> *?)",
        "let record = [vault? peek];",
        "let title = [record? title];",
    ):
        assert token in fixture


def test_language_semantics_generic_function_fixture_exercises_public_callable_generics() -> None:
    fixture = _read(GENERIC_FUNCTION_FIXTURE)

    for token in (
        "fn genericIdentity<T : id<Persistable>>(value: T) -> T",
        "return value;",
        "fn consumeGenericFunction(box: SemanticBox *?)",
        "let record = genericIdentity(box);",
        "let title = [record? title];",
    ):
        assert token in fixture


def test_language_semantics_negative_fixtures_publish_specific_diagnostics() -> None:
    expected = {
        AWAIT_OUTSIDE_ASYNC_NEGATIVE: ("O3S223", "return await fetchValue();"),
        EXECUTOR_ON_SYNC_NEGATIVE: (
            "O3S224",
            "fn runTask() -> i32 __attribute__((objc_executor(main)))",
        ),
        CONFLICTING_CAPTURE_NEGATIVE: (
            "O3S301",
            "let closure = ^[weak owner, unowned owner]",
        ),
        PROTOCOL_ASSOCIATED_TYPE_NEGATIVE: (
            "O3P100",
            "associatedtype Element;",
        ),
        PROTOCOL_DYNAMIC_DISPATCH_NEGATIVE: (
            "O3S314",
            '- (i32)invoke __attribute__((objc_dynamic));',
        ),
    }

    for path, (code, source_token) in expected.items():
        fixture = _read(path)
        assert f"// Expected diagnostic code(s): {code}." in fixture
        assert source_token in fixture

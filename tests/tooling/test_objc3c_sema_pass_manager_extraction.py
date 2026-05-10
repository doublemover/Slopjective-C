from objc3c_sema_pass_manager_behavior import (
    assert_build_surfaces_register_pass_manager_source,
    assert_pass_manager_contract_exposes_pass_order_and_diagnostics_bus,
    assert_pass_manager_module_orchestrates_semantic_passes,
    assert_pipeline_uses_pass_manager_and_diagnostics_bus,
)
from objc3c_sema_pass_manager_sources import (
    BUILD_SCRIPT,
    PASS_MANAGER_CONTRACT,
    PASS_MANAGER_HEADER,
    PASS_MANAGER_SOURCE,
    PIPELINE_SEMA_STAGE_RUNNER,
    SEMA_CMAKE_FILE,
    read_expanded_source,
)


def test_pass_manager_contract_exposes_pass_order_and_diagnostics_bus() -> None:
    assert_pass_manager_contract_exposes_pass_order_and_diagnostics_bus(
        read_expanded_source(PASS_MANAGER_CONTRACT)
    )


def test_pass_manager_module_exists_and_orchestrates_semantic_passes() -> None:
    assert_pass_manager_module_orchestrates_semantic_passes(
        read_expanded_source(PASS_MANAGER_HEADER),
        read_expanded_source(PASS_MANAGER_SOURCE),
    )


def test_pipeline_uses_pass_manager_and_diagnostics_bus() -> None:
    assert_pipeline_uses_pass_manager_and_diagnostics_bus(
        read_expanded_source(PIPELINE_SEMA_STAGE_RUNNER)
    )


def test_build_surfaces_register_pass_manager_source() -> None:
    assert_build_surfaces_register_pass_manager_source(
        read_expanded_source(SEMA_CMAKE_FILE),
        read_expanded_source(BUILD_SCRIPT),
    )

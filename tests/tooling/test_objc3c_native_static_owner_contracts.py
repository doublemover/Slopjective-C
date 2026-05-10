from __future__ import annotations

from objc3c_native_static_owner_contracts_behavior import (
    assert_ir_emitter_static_contract_uses_split_context_and_active_artifact_api,
    assert_native_include_shards_remain_private_to_owner_aggregators,
    assert_native_owner_modules_are_registered_in_cmake_without_include_shards,
    assert_runtime_public_result_abi_uses_split_status_and_payload_headers,
)


def test_native_owner_modules_are_registered_in_cmake_without_include_shards() -> None:
    assert_native_owner_modules_are_registered_in_cmake_without_include_shards()


def test_native_include_shards_remain_private_to_owner_aggregators() -> None:
    assert_native_include_shards_remain_private_to_owner_aggregators()


def test_ir_emitter_static_contract_uses_split_context_and_active_artifact_api() -> None:
    assert_ir_emitter_static_contract_uses_split_context_and_active_artifact_api()


def test_runtime_public_result_abi_uses_split_status_and_payload_headers() -> None:
    assert_runtime_public_result_abi_uses_split_status_and_payload_headers()

from __future__ import annotations

from native_package_action_owner_split_behavior import (
    assert_native_package_catalog_is_owner_facade,
    assert_native_package_handlers_are_owner_facade,
    assert_native_package_owner_handlers_do_not_route_through_native_build_wrapper,
    assert_native_package_public_order_and_owner_membership,
    assert_package_inventory_facade_publishes_category_owner_contracts,
    assert_runnable_toolchain_package_includes_compile_wrapper_dependencies,
    assert_runnable_toolchain_package_spec_publishes_platform_owner_contract,
    assert_runnable_toolchain_package_uses_strictmode_safe_staging_lookup,
)


def test_native_package_catalog_is_owner_facade() -> None:
    assert_native_package_catalog_is_owner_facade()


def test_native_package_handlers_are_owner_facade() -> None:
    assert_native_package_handlers_are_owner_facade()


def test_native_package_owner_handlers_do_not_route_through_native_build_wrapper() -> None:
    assert_native_package_owner_handlers_do_not_route_through_native_build_wrapper()


def test_runnable_toolchain_package_spec_publishes_platform_owner_contract() -> None:
    assert_runnable_toolchain_package_spec_publishes_platform_owner_contract()


def test_package_inventory_facade_publishes_category_owner_contracts() -> None:
    assert_package_inventory_facade_publishes_category_owner_contracts()


def test_runnable_toolchain_package_uses_strictmode_safe_staging_lookup() -> None:
    assert_runnable_toolchain_package_uses_strictmode_safe_staging_lookup()


def test_runnable_toolchain_package_includes_compile_wrapper_dependencies() -> None:
    assert_runnable_toolchain_package_includes_compile_wrapper_dependencies()


def test_native_package_public_order_and_owner_membership() -> None:
    assert_native_package_public_order_and_owner_membership()

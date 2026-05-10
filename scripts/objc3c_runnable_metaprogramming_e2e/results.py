"""Result parsing and assertions for runnable metaprogramming validation."""

from __future__ import annotations

from objc3c_tooling.probe_output import parse_key_value_output

from .assertions import expect
from .catalog import PRESERVED_HOST_CACHE_FIELDS
from .catalog import RUNTIME_PROBE_EXPECTATION
from .catalog import HostCacheExpectation


def assert_host_cache_expectation(
    payload: dict[str, object],
    expectation: HostCacheExpectation,
) -> None:
    for field_name, expected_value in expectation.expected_values.items():
        expect(
            payload.get(field_name) == expected_value,
            f"expected {expectation.context} to preserve {field_name}",
        )


def assert_preserved_host_cache_fields(
    first_payload: dict[str, object],
    second_payload: dict[str, object],
) -> None:
    for field_name in PRESERVED_HOST_CACHE_FIELDS:
        expect(
            second_payload.get(field_name) == first_payload.get(field_name),
            f"expected packaged metaprogramming second compile to preserve {field_name}",
        )


def assert_provider_module_name(provider_module_name: object) -> str:
    expect(
        isinstance(provider_module_name, str) and provider_module_name != "",
        "packaged metaprogramming provider did not publish a module name",
    )
    return provider_module_name


def assert_consumer_link_plan(
    link_plan: dict[str, object],
    *,
    provider_module_name: str,
) -> None:
    expect(
        link_plan.get("metaprogramming_host_cache_imported_module_count") == 1
        and link_plan.get("metaprogramming_host_cache_imported_module_names_lexicographic")
        == [provider_module_name]
        and link_plan.get("metaprogramming_host_cache_cross_module_preservation_ready")
        is True,
        "packaged metaprogramming consumer link plan drifted from the imported host-cache module set",
    )


def parse_runtime_probe_payload(result: object) -> dict[str, object]:
    return parse_key_value_output(result, "packaged metaprogramming runtime probe")


def assert_runtime_probe_payload(
    payload: dict[str, object],
    *,
    provider_host_cache: dict[str, object],
) -> None:
    expect(
        all(payload.get(field_name) == expected_value for field_name, expected_value in RUNTIME_PROBE_EXPECTATION.items()),
        "packaged metaprogramming runtime probe drifted from the live host-cache readiness boundary",
    )
    expect(
        payload.get("host_executable_relative_path")
        == provider_host_cache.get("host_executable_relative_path")
        and payload.get("cache_root_relative_path")
        == provider_host_cache.get("cache_root_relative_path"),
        "packaged metaprogramming runtime probe drifted from the packaged cache artifact paths",
    )


__all__ = [
    "assert_consumer_link_plan",
    "assert_host_cache_expectation",
    "assert_preserved_host_cache_fields",
    "assert_provider_module_name",
    "assert_runtime_probe_payload",
    "parse_runtime_probe_payload",
]

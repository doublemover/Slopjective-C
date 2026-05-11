"""Materialization assertions for live metaprogramming host-cache cases."""

from __future__ import annotations

from typing import NoReturn

from objc3c_runtime_acceptance.expectation_matching import expect


def expect_materializing_cache_miss_was_found() -> NoReturn:
    expect(
        False,
        "expected live metaprogramming host-cache implementation case to force a materializing cache miss before the cache-hit replay check",
    )
    raise AssertionError("unreachable")


__all__ = ["expect_materializing_cache_miss_was_found"]

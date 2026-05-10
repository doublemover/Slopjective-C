from __future__ import annotations

from collections.abc import Mapping
from typing import Any, Iterable


def assert_same_keys(actual: Mapping[str, Any], expected: Mapping[str, Any]) -> None:
    assert set(actual) == set(expected)


def assert_same_values(actual: Iterable[str], expected: Iterable[str]) -> None:
    assert set(actual) == set(expected)


def assert_no_overlap(left: Iterable[str], right: Iterable[str]) -> None:
    assert not set(left).intersection(set(right))


def assert_subset(subset: Iterable[str], superset: Iterable[str]) -> None:
    assert set(subset).issubset(set(superset))

from __future__ import annotations

from typing import Mapping


def assert_mapping_fields_match(
    actual: Mapping[str, object],
    expected: Mapping[str, object],
) -> None:
    for field_name, field_value in expected.items():
        assert actual[field_name] == field_value


def assert_false_fields(payload: Mapping[str, object], field_names: tuple[str, ...]) -> None:
    for field_name in field_names:
        assert payload[field_name] is False


def assert_non_empty_fields(payload: Mapping[str, object], field_names: tuple[str, ...]) -> None:
    for field_name in field_names:
        assert payload[field_name]

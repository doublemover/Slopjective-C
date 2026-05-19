from __future__ import annotations

from pathlib import Path
from typing import Any, Iterable


def assert_contains_all(text: str, tokens: Iterable[str]) -> None:
    for token in tokens:
        assert token in text


def assert_no_offenders(offenders: list[str]) -> None:
    assert offenders == []


def assert_path_exists(path: Path) -> None:
    assert path.is_file()


def assert_owner_split_shape(
    split: dict[str, Any],
    owner_labels: set[str],
) -> set[str]:
    assert split["surface_kind"]
    assert split["split_rule"]
    assert split["owners"]

    observed_owners = set()
    for owner_split in split["owners"]:
        assert owner_split["owner"] in owner_labels
        assert owner_split["selectors"]
        assert owner_split["disposition"]
        observed_owners.add(owner_split["owner"])
    return observed_owners


def assert_no_compatibility_runtime_dispatch(serialized: str) -> None:
    assert "objc3_msgsend_i32" not in serialized
    assert "compatibility_runtime_dispatch_symbol" not in serialized


def assert_live_runtime_dispatch(execution: dict[str, Any]) -> None:
    assert execution["runtime_dispatch_symbol"] == "objc3_runtime_dispatch_i32"
    assert execution["requires_live_runtime_dispatch"] is True

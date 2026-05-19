from __future__ import annotations

from typing import Any, Iterable


def assert_source_contains(source: str, tokens: Iterable[str]) -> None:
    for token in tokens:
        assert token in source


def assert_source_excludes(source: str, tokens: Iterable[str]) -> None:
    for token in tokens:
        assert token not in source


def assert_contract_identity(contract: dict[str, Any]) -> None:
    assert contract["contract_id"] == "objc3c.frontend.c_api.runner.contract.v1"
    assert contract["runner"] == "artifacts/bin/objc3c-frontend-c-api-runner.exe"
    assert (
        contract["source_glob"]
        == "native/objc3c/src/tools/objc3c_frontend_c_api_runner*"
    )
    assert contract["summary_mode"] == "objc3c-frontend-c-api-runner-v1"


def assert_contract_fields_are_in_source(
    source: str,
    fields: Iterable[str],
) -> None:
    for field in fields:
        assert f'\\"{field}\\"' in source or f'"{field}"' in source


def assert_stage_fields_are_in_source(
    source: str,
    stages: Iterable[str],
) -> None:
    for stage in stages:
        assert (
            f'WriteFrontendCApiRunnerStageSummaryJson(out, "{stage}", result.{stage}'
            in source
        )


def assert_escaped_json_fields_are_in_source(
    source: str,
    fields: Iterable[str],
) -> None:
    for field in fields:
        assert f'\\"{field}\\"' in source

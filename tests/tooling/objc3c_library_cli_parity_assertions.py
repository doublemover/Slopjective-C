from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_library_cli_parity_support import parity


def load_summary(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def assert_parity_pass_summary(payload: dict[str, Any]) -> None:
    assert payload["mode"] == "objc3c-library-cli-parity-v2"
    assert payload["ok"] is True
    assert payload["failures"] == []
    assert [item["dimension"] for item in payload["dimensions"]] == [
        "diagnostics",
        "manifest",
        "ir",
        "object",
    ]


def assert_failure_contains(payload: dict[str, Any], expected: str) -> None:
    assert payload["ok"] is False
    assert any(expected in failure for failure in payload["failures"])


def assert_sha256_proxy_summary(payload: dict[str, Any]) -> None:
    assert payload["ok"] is True
    assert payload["comparisons"][0]["source_kind"] == "sha256-proxy"


def assert_synthetic_fixture_authenticity_summary(payload: dict[str, Any]) -> None:
    assert payload["ok"] is True
    assert payload["artifact_authenticity"]["provenance_class"] == "synthetic_fixture"
    assert (
        payload["synthetic_fixture_contract"]["fixture_family_id"]
        == parity.SYNTHETIC_FIXTURE_FAMILY_ID
    )
    assert payload["authenticity_checks"]["failure_count"] == 0


def assert_source_mode_success_summary(payload: dict[str, Any]) -> None:
    assert payload["ok"] is True
    assert payload["artifacts"] == [
        "module.diagnostics.json",
        "module.ll",
        "module.manifest.json",
        "module.obj",
    ]
    assert payload["execution"]["source"].endswith("sample.objc3")
    assert payload["execution"]["work_key"] is not None
    assert len(payload["execution"]["work_key"]) == 16
    assert [item["role"] for item in payload["execution"]["commands"]] == [
        "cli",
        "c-api",
    ]


def assert_capability_backend_routing(
    payload: dict[str, Any],
    observed_commands: list[list[str]],
) -> None:
    assert payload["execution"]["routing"]["effective_ir_object_backend"] == "llvm-direct"
    assert len(observed_commands) == 2
    for command in observed_commands:
        backend_index = command.index("--objc3-ir-object-backend")
        assert command[backend_index + 1] == "llvm-direct"
        assert "--llc" in command


def assert_capability_fail_closed_summary(payload: dict[str, Any]) -> None:
    assert payload["ok"] is False
    assert payload["execution"]["commands"] == []
    assert any(
        "capability routing fail-closed: sema/type-system parity capability unavailable"
        in failure
        for failure in payload["failures"]
    )


def assert_work_keys_differ(payload_a: dict[str, Any], payload_b: dict[str, Any]) -> None:
    key_a = payload_a["execution"]["work_key"]
    key_b = payload_b["execution"]["work_key"]
    assert key_a != key_b
    assert len(key_a) == 16
    assert len(key_b) == 16


def assert_work_keys_match_and_are_hex(
    payload_a: dict[str, Any],
    payload_b: dict[str, Any],
) -> None:
    key_a = payload_a["execution"]["work_key"]
    key_b = payload_b["execution"]["work_key"]
    assert key_a == key_b
    assert len(key_a) == 16
    assert all(char in "0123456789abcdef" for char in key_a)


def assert_command_failure_summary(payload: dict[str, Any]) -> None:
    assert payload["ok"] is False
    failures = payload["failures"]
    assert len(failures) == 2
    assert "objc3c-native.exe" in failures[0]
    assert "objc3c-frontend-c-api-runner.exe" in failures[1]

    commands = payload["execution"]["commands"]
    assert [item["role"] for item in commands] == ["cli", "c-api"]
    assert [item["exit_code"] for item in commands] == [17, 23]
    assert [item["stdout"] for item in commands] == ["cli-stdout\n", "c-api-stdout\n"]
    assert [item["stderr"] for item in commands] == ["cli-stderr\n", "c-api-stderr\n"]

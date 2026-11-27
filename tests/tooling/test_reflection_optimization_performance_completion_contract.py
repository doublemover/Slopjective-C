from __future__ import annotations

import copy
import importlib.util
import json
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_reflection_optimization_performance_completion_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_reflection_optimization_performance_completion_contract",
    SCRIPT_PATH,
)
assert SPEC is not None and SPEC.loader is not None
checker = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = checker
SPEC.loader.exec_module(checker)


def _variant(tmp_path: Path, payload: dict[str, object]) -> Path:
    path = tmp_path / "completion_contract.json"
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    return path


def _contract_payload() -> dict[str, object]:
    return checker._load_json(checker.CONTRACT_PATH)


def test_completion_contract_rejects_unavailable_prototype_sources() -> None:
    result = checker.validate_completion_contract()

    assert not result.passed
    assert result.payload["issues"] == [8159, 8174, 8175]
    assert any(
        failure.startswith("public reflection header path missing: ")
        for failure in result.failures
    )


def test_completion_contract_cli_writes_failure_report(tmp_path: Path) -> None:
    report_path = tmp_path / "completion-contract.json"

    assert checker.main(["--summary-out", str(report_path)]) == 1

    payload = json.loads(report_path.read_text(encoding="utf-8"))
    assert payload["status"] == "FAIL"
    assert payload["contract_id"] == (
        "objc3c.reflection.optimization.performance.completion.validation.v1"
    )


def test_completion_contract_rejects_public_reflection_lifetime_drift(tmp_path: Path) -> None:
    payload = _contract_payload()
    variant = copy.deepcopy(payload)
    variant["public_reflection"]["expected_lifetime_model"] = "borrowed caller buffers"

    result = checker.validate_completion_contract(_variant(tmp_path, variant))

    assert not result.passed
    assert any("lifetime model drifted" in failure for failure in result.failures)


def test_completion_contract_rejects_reserved_optimization_success_claims(
    tmp_path: Path,
) -> None:
    payload = _contract_payload()
    variant = copy.deepcopy(payload)
    variant["semantic_optimization"]["reserved_pass_success_claims_allowed"] = True

    result = checker.validate_completion_contract(_variant(tmp_path, variant))

    assert not result.passed
    assert any(
        "permits reserved pass success claims" in failure
        for failure in result.failures
    )


def test_completion_contract_rejects_runtime_equivalence_action_drift(
    tmp_path: Path,
) -> None:
    payload = _contract_payload()
    variant = copy.deepcopy(payload)
    variant["semantic_optimization"]["required_runtime_equivalence_actions"].append(
        "test-runtime-equivalence-generated-only"
    )

    result = checker.validate_completion_contract(_variant(tmp_path, variant))

    assert not result.passed
    assert any(
        "runtime equivalence action missing" in failure
        for failure in result.failures
    )


def test_completion_contract_rejects_generated_report_authority(tmp_path: Path) -> None:
    payload = _contract_payload()
    variant = copy.deepcopy(payload)
    variant["performance_evidence"]["generated_report_authority_allowed"] = True

    result = checker.validate_completion_contract(_variant(tmp_path, variant))

    assert not result.passed
    assert any(
        "permits generated report authority" in failure for failure in result.failures
    )


def test_completion_contract_rejects_private_public_header_boundary(tmp_path: Path, monkeypatch) -> None:
    payload = _contract_payload()
    variant = copy.deepcopy(payload)
    variant["public_reflection"]["forbidden_public_header_tokens"].append(
        "objc3_runtime_reflection_state_snapshot"
    )

    read = checker._read

    def read_with_private_symbol(path, failures, owner):
        if owner == "public reflection header":
            return "struct objc3_runtime_reflection_state_snapshot;"
        return read(path, failures, owner)

    monkeypatch.setattr(checker, "_read", read_with_private_symbol)
    result = checker.validate_completion_contract(_variant(tmp_path, variant))

    assert not result.passed
    assert any(
        "public reflection header exposes forbidden token" in failure
        for failure in result.failures
    )

from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path
from types import ModuleType

import pytest


ROOT = Path(__file__).resolve().parents[2]
CHECKER_PATH = ROOT / "scripts" / "check_generic_protocol_completion_contract.py"
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "type_protocol"
    / "generic_protocol_completion_contract.json"
)


def _load_checker() -> ModuleType:
    spec = importlib.util.spec_from_file_location(
        "check_generic_protocol_completion_contract",
        CHECKER_PATH,
    )
    assert spec is not None
    assert spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def _write_mutated_contract(tmp_path: Path, mutator) -> Path:
    payload = json.loads(CONTRACT_PATH.read_text(encoding="utf-8"))
    mutator(payload)
    path = tmp_path / "generic_protocol_completion_contract.json"
    path.write_text(json.dumps(payload, indent=2, sort_keys=True), encoding="utf-8")
    return path


def test_generic_protocol_completion_contract_accepts_live_source_truth() -> None:
    checker = _load_checker()

    result = checker.validate_contract(CONTRACT_PATH)

    assert result.claim_count == 6
    assert result.source_anchor_count >= 12
    assert result.positive_evidence_count > result.claim_count
    assert result.negative_evidence_count > result.claim_count


def test_generic_protocol_completion_contract_fails_closed_without_negative_fixture(
    tmp_path: Path,
) -> None:
    checker = _load_checker()

    path = _write_mutated_contract(
        tmp_path,
        lambda payload: payload["claims"][0]["required_negative_evidence"].clear(),
    )

    with pytest.raises(checker.ContractError, match="required_negative_evidence"):
        checker.validate_contract(path)


def test_generic_protocol_completion_contract_requires_real_source_snippets(
    tmp_path: Path,
) -> None:
    checker = _load_checker()

    def mutate(payload: dict[str, object]) -> None:
        claims = payload["claims"]
        assert isinstance(claims, list)
        first_claim = claims[0]
        assert isinstance(first_claim, dict)
        anchors = first_claim["source_anchors"]
        assert isinstance(anchors, list)
        first_anchor = anchors[0]
        assert isinstance(first_anchor, dict)
        snippets = first_anchor["snippets"]
        assert isinstance(snippets, list)
        snippets[0] = "missing_generic_protocol_completion_anchor"

    path = _write_mutated_contract(tmp_path, mutate)

    with pytest.raises(checker.ContractError, match="missing source snippet"):
        checker.validate_contract(path)

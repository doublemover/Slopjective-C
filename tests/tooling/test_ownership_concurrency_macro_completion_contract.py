from __future__ import annotations

import json
import sys
from copy import deepcopy
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from check_ownership_concurrency_macro_completion_contract import (  # noqa: E402
    CONTRACT_PATH,
    EXPECTED_ISSUES,
    validate_ownership_concurrency_macro_completion_contract,
)


def _load_contract() -> dict:
    return json.loads(CONTRACT_PATH.read_text(encoding="utf-8"))


def _write_contract(path: Path, payload: dict) -> None:
    path.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def test_completion_contract_is_source_backed_and_fail_closed() -> None:
    result = validate_ownership_concurrency_macro_completion_contract()

    assert result.passed, result.failures
    assert result.payload["source_contract_id"] == (
        "objc3c.ownership-concurrency-macro.completion.contract.v1"
    )
    assert set(result.payload["issue_refs"]) == EXPECTED_ISSUES
    assert result.payload["source_group_counts"] == {
        "ownership_qualifiers_lifetimes_block_runtime_hooks": 6,
        "public_concurrency_task_actor_usability": 4,
        "macro_expansion_artifact_trust_sandbox_boundaries": 6,
    }
    assert all(result.payload["invariant_checks"].values())
    assert result.payload["unsupported_behavior_count"] == 4
    assert result.payload["fail_closed_fixture_case_count"] == 3


def test_completion_contract_rejects_missing_source_token(tmp_path: Path) -> None:
    contract = _load_contract()
    drifted = deepcopy(contract)
    drifted["source_surface_groups"][0]["source_records"][0]["required_tokens"].append(
        "missing-objc3-ownership-token"
    )
    tmp_contract = tmp_path / "contract.json"
    _write_contract(tmp_contract, drifted)

    result = validate_ownership_concurrency_macro_completion_contract(tmp_contract)

    assert not result.passed
    assert any("missing-objc3-ownership-token" in failure for failure in result.failures)


def test_completion_contract_rejects_tmp_source_truth(tmp_path: Path) -> None:
    contract = _load_contract()
    drifted = deepcopy(contract)
    drifted["source_surface_groups"][1]["source_records"][0]["path"] = (
        "tmp/generated-concurrency-summary.json"
    )
    tmp_contract = tmp_path / "contract.json"
    _write_contract(tmp_contract, drifted)

    result = validate_ownership_concurrency_macro_completion_contract(tmp_contract)

    assert not result.passed
    assert any(
        "forbidden source-truth path tmp/generated-concurrency-summary.json" in failure
        for failure in result.failures
    )


def test_completion_contract_rejects_unsupported_behavior_widening(
    tmp_path: Path,
) -> None:
    contract = _load_contract()
    drifted = deepcopy(contract)
    drifted["unsupported_behaviors"][2]["status"] = "supported"
    tmp_contract = tmp_path / "contract.json"
    _write_contract(tmp_contract, drifted)

    result = validate_ownership_concurrency_macro_completion_contract(tmp_contract)

    assert not result.passed
    assert any(
        "unsupported behavior status widened to supported" in failure
        for failure in result.failures
    )

from __future__ import annotations

import sys
from copy import deepcopy
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_macro_expansion_artifact_ownership import (
    CONTRACT_PATH,
    validate_macro_expansion_artifact_ownership,
)
from objc3c_shared.json_io import load_json_object, write_json_file


def test_macro_expansion_artifact_ownership_contract_passes() -> None:
    result = validate_macro_expansion_artifact_ownership()

    assert result.passed, result.failures
    assert result.payload["source_contract_id"] == (
        "objc3c.metaprogramming.macro.expansion.artifact.ownership.v1"
    )
    assert result.payload["issue_ref"] == 8168
    assert result.payload["generated_artifact_boundary"]["support_claim_authority"] is False
    assert result.payload["required_field_count"] >= 40
    assert result.payload["fail_closed_case_count"] == 4


def test_macro_expansion_artifact_ownership_rejects_support_claim_authority(
    tmp_path: Path,
) -> None:
    contract = deepcopy(load_json_object(CONTRACT_PATH))
    contract["generated_artifact_boundary"]["support_claim_authority"] = True
    tmp_contract = tmp_path / "contract.json"
    write_json_file(tmp_contract, contract, sort_keys=True)

    result = validate_macro_expansion_artifact_ownership(tmp_contract)

    assert not result.passed
    assert result.payload["status"] == "FAIL"
    assert any("support_claim_authority" in failure for failure in result.failures)


def test_macro_expansion_artifact_ownership_rejects_document_field_drift(
    tmp_path: Path,
) -> None:
    contract = deepcopy(load_json_object(CONTRACT_PATH))
    contract["host_cache_document"]["required_fields"].append(
        "missing_generated_artifact_owner_field"
    )
    tmp_contract = tmp_path / "contract.json"
    write_json_file(tmp_contract, contract, sort_keys=True)

    result = validate_macro_expansion_artifact_ownership(tmp_contract)

    assert not result.passed
    assert any(
        "missing_generated_artifact_owner_field" in failure
        for failure in result.failures
    )

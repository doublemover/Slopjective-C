from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "stdlib_concurrency"
    / "public_concurrency_usability_claims_contract.json"
)



NON_GOAL_FRAGMENTS = {
    "distributed",
    "swift",
    "fairness",
    "generic task abi",
    "os scheduler",
    "cross-process",
}


def _read_json(path: Path) -> dict[str, object]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload






def test_public_claim_names_do_not_publish_reserved_concurrency_non_goals() -> None:
    contract = _read_json(CONTRACT_PATH)
    all_reserved_text = " ".join(
        str(reserved).lower()
        for claim in contract["claims"]
        for reserved in claim["reserved_until_runtime_contract"]
    )

    for claim in contract["claims"]:
        assert isinstance(claim, dict)
        public_claim_text = " ".join(
            [
                str(claim["support_claim"]),
                str(claim["capability_id"]),
                str(claim["api_family"]),
                *[str(export_name) for export_name in claim["exports"]],
                *[str(runtime_symbol) for runtime_symbol in claim["runtime_abi"]],
                *[str(policy_key) for policy_key in claim["semantic_policy_keys"]],
            ]
        ).lower()
        assert not any(fragment in public_claim_text for fragment in NON_GOAL_FRAGMENTS)

    assert any(fragment in all_reserved_text for fragment in NON_GOAL_FRAGMENTS)

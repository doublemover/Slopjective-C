from __future__ import annotations

import importlib.util
import json
import sys
from copy import deepcopy
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

SCRIPT_PATH = ROOT / "scripts" / "build_long_horizon_operations_deprecation_policy_summary.py"
POLICY_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "long_horizon_operations"
    / "deprecation_support_policy.json"
)


def _load_checker() -> Any:
    spec = importlib.util.spec_from_file_location(
        "build_long_horizon_operations_deprecation_policy_summary",
        SCRIPT_PATH,
    )
    if spec is None or spec.loader is None:
        raise RuntimeError("unable to load deprecation policy summary script")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def _load_policy() -> dict[str, Any]:
    return json.loads(POLICY_PATH.read_text(encoding="utf-8"))


def _write_policy(tmp_path: Path, policy: dict[str, Any]) -> Path:
    root_tmp = ROOT / "tmp" / "tests" / "long-horizon-deprecation" / tmp_path.name
    root_tmp.mkdir(parents=True, exist_ok=True)
    path = root_tmp / "deprecation_support_policy.json"
    path.write_text(json.dumps(policy, indent=2) + "\n", encoding="utf-8")
    return path


def _redirect(checker: Any, tmp_path: Path, policy: dict[str, Any]) -> None:
    root_tmp = ROOT / "tmp" / "tests" / "long-horizon-deprecation" / tmp_path.name
    root_tmp.mkdir(parents=True, exist_ok=True)
    checker.POLICY_PATH = _write_policy(tmp_path, policy)
    checker.SUMMARY_PATH = root_tmp / "deprecation-support-policy-summary.json"


def _run_with_policy(tmp_path: Path, policy: dict[str, Any]) -> tuple[int, dict[str, Any]]:
    checker = _load_checker()
    _redirect(checker, tmp_path, policy)
    result = checker.main()
    summary = json.loads(checker.SUMMARY_PATH.read_text(encoding="utf-8"))
    return result, summary


def test_deprecation_policy_summary_enforces_transition_and_publication_contract(
    tmp_path: Path,
) -> None:
    result, summary = _run_with_policy(tmp_path, _load_policy())

    assert result == 0
    assert summary["status"] == "PASS"
    assert summary["false_claim_states"] == ["hard-deprecated", "removed"]
    assert summary["allowed_transition_count"] == 3
    assert summary["publication_field_requirement_count"] == 6
    assert summary["non_waivable_states"] == ["hard-deprecated", "removed"]


def test_deprecation_policy_summary_rejects_public_claim_on_hard_state(
    tmp_path: Path,
) -> None:
    policy = _load_policy()
    hard_state = next(
        entry for entry in policy["deprecation_states"] if entry["state"] == "hard-deprecated"
    )
    hard_state["public_claim_allowed"] = True

    result, summary = _run_with_policy(tmp_path, policy)

    assert result == 1
    assert summary["status"] == "FAIL"
    assert any("hard-deprecated must block public claims" in failure for failure in summary["failures"])


def test_deprecation_policy_summary_rejects_hard_transition_without_evidence(
    tmp_path: Path,
) -> None:
    policy = _load_policy()
    transition = next(
        entry
        for entry in policy["state_transition_policy"]["allowed_transitions"]
        if entry["to"] == "hard-deprecated"
    )
    transition["required_evidence"].remove("generated-revert-evidence")

    result, summary = _run_with_policy(tmp_path, policy)

    assert result == 1
    assert summary["status"] == "FAIL"
    assert any("misses hard-state evidence" in failure for failure in summary["failures"])


def test_deprecation_policy_summary_rejects_missing_publication_requirement(
    tmp_path: Path,
) -> None:
    policy = _load_policy()
    stripped = deepcopy(policy)
    stripped["publication_field_requirements"] = [
        entry
        for entry in stripped["publication_field_requirements"]
        if entry["field"] != "revert_evidence"
    ]

    result, summary = _run_with_policy(tmp_path, stripped)

    assert result == 1
    assert summary["status"] == "FAIL"
    assert any("revert_evidence" in failure for failure in summary["failures"])

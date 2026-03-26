from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
POLICY = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b003_checker_consolidation_and_migration_policy_core_feature_implementation_policy.json"


def test_policy_routes_align_with_expected_suite_order() -> None:
    payload = json.loads(POLICY.read_text(encoding="utf-8"))
    assert [entry["suite_id"] for entry in payload["consolidation_routes"]] == [
        "runtime_bootstrap_dispatch",
        "frontend_split_recovery",
        "module_parity_packaging",
        "native_fixture_corpus_and_runtime_probes",
    ]


def test_policy_freezes_shared_harness_defaults() -> None:
    payload = json.loads(POLICY.read_text(encoding="utf-8"))
    defaults = payload["migration_defaults"]
    assert defaults["new_readiness_must_delegate_to_shared_harness"] is True
    assert defaults["new_pytest_wrappers_must_wrap_shared_harness_or_retained_static_guard"] is True
    assert defaults["compatibility_window_closes_by_issue"] == "M313-E001"

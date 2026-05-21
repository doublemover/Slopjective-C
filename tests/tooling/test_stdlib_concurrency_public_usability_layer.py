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
MODULE_MANIFEST_PATH = ROOT / "stdlib" / "modules" / "objc3.concurrency" / "module.json"
MODULE_SOURCE_PATH = ROOT / "stdlib" / "modules" / "objc3.concurrency" / "module.objc3"
MODULE_SMOKE_PATH = ROOT / "stdlib" / "modules" / "objc3.concurrency" / "smoke.objc3"
RUNTIME_PROBE_PATH = ROOT / "tests" / "tooling" / "runtime" / "stdlib_concurrency_runtime_probe.cpp"
RUNTIME_CASE_PATH = (
    ROOT / "scripts" / "objc3c_runtime_acceptance" / "domains" / "stdlib_runtime_cases.py"
)


PUBLIC_ALIAS_ROUTES = {
    "objc3_task_spawn": [
        "return await objc3_concurrency_spawn_token(executor_tag);",
    ],
    "objc3_task_spawn_child": [
        "return await objc3_concurrency_child_spawn_token(executor_tag);",
    ],
    "objc3_task_spawn_detached": [
        "return await objc3_concurrency_detached_spawn_token(executor_tag);",
    ],
    "objc3_task_join": [
        "return await objc3_concurrency_join_status(result_code, executor_tag, 0);",
    ],
    "objc3_task_group_run_two": [
        "return await objc3_concurrency_task_group_scope_depth(executor_tag, 2);",
    ],
    "objc3_task_group_run_bounded": [
        "return await objc3_concurrency_task_group_scope_depth(executor_tag, child_tasks);",
    ],
    "objc3_task_is_cancelled": [
        "return await objc3_concurrency_cancellation_query(executor_tag);",
    ],
    "objc3_task_cancel_if_needed": [
        "return await objc3_concurrency_cancellation_checkpoint(executor_tag);",
        "return await objc3_concurrency_cancellation_query(executor_tag);",
    ],
    "objc3_executor_hop": [
        "return objc3_concurrency_executor_hop_token(value, executor_tag);",
    ],
    "objc3_actor_mailbox_send_and_drain": [
        "return objc3_concurrency_actor_mailbox_token(actor_handle, executor_tag, value);",
    ],
}

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


def test_public_claims_have_smoke_calls_and_runtime_probe_payloads() -> None:
    contract = _read_json(CONTRACT_PATH)
    module_manifest = _read_json(MODULE_MANIFEST_PATH)
    module_source = MODULE_SOURCE_PATH.read_text(encoding="utf-8")
    module_smoke = MODULE_SMOKE_PATH.read_text(encoding="utf-8")
    runtime_probe = RUNTIME_PROBE_PATH.read_text(encoding="utf-8")
    runtime_case = RUNTIME_CASE_PATH.read_text(encoding="utf-8")

    module_exports = set(module_manifest["exports"])
    module_runtime_abi = set(module_manifest["runtime_abi"])
    required_payloads = contract["runtime_probe_payload_requirements"]
    assert isinstance(required_payloads, dict)

    for field_name in required_payloads:
        assert field_name in runtime_probe
        assert f'payload.get("{field_name}")' in runtime_case

    for claim in contract["claims"]:
        assert isinstance(claim, dict)
        for export_name in claim["exports"]:
            assert export_name in module_exports
            assert f"{export_name}(" in module_source
            assert f"{export_name}(" in module_smoke

        for runtime_symbol in claim["runtime_abi"]:
            assert runtime_symbol in module_runtime_abi
            assert runtime_symbol in module_source
            assert runtime_symbol in runtime_probe

        for field_name in claim["runtime_probe_payload_fields"]:
            assert field_name in required_payloads

    for diagnostic in contract["diagnostic_evidence"]:
        assert isinstance(diagnostic, dict)
        fixture = ROOT / str(diagnostic["fixture"])
        assert fixture.is_file()
        fixture_text = fixture.read_text(encoding="utf-8")
        assert str(diagnostic["code"]) in fixture_text
        assert str(diagnostic["source_token"]) in fixture_text


def test_public_aliases_route_through_bounded_concurrency_tokens() -> None:
    module_source = MODULE_SOURCE_PATH.read_text(encoding="utf-8")
    module_manifest = _read_json(MODULE_MANIFEST_PATH)
    exported_signatures = module_manifest["abi_signatures"]
    assert isinstance(exported_signatures, dict)

    for alias_name, expected_routes in PUBLIC_ALIAS_ROUTES.items():
        assert alias_name in exported_signatures
        for expected_route in expected_routes:
            assert expected_route in module_source


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

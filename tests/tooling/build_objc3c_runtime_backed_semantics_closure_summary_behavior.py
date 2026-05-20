from __future__ import annotations

from typing import Any


def assert_runtime_backed_semantics_summary_behavior(
    summary: dict[str, Any],
) -> None:
    assert summary["counts"]["positive_fixture_count"] == 7
    assert summary["counts"]["negative_fixture_count"] == 10
    assert summary["counts"]["runtime_helper_symbol_count"] == 36
    assert summary["counts"]["required_ir_token_count"] == 17
    assert summary["counts"]["positive_fixture_ir_call_token_count"] == 42
    assert summary["counts"]["positive_fixture_import_surface_count"] == 5
    assert summary["checks"]["no_source_truth_under_tmp"] is True
    assert summary["checks"]["positive_fixture_ir_call_tokens"] is True
    assert summary["checks"]["positive_fixture_import_surfaces"] is True

    block_fixture = summary["positive_compile"]["block_arc_autorelease_return"]
    assert block_fixture["ir_call_tokens"][
        "call i32 @objc3_runtime_promote_block_i32"
    ] is True
    assert block_fixture["runtime_import_surface"][
        "objc_runtime_block_ownership_artifact_preservation.local_copy_helper_symbolized_sites >= 1"
    ] is True

    actor_fixture = summary["positive_compile"]["live_actor_mailbox_runtime"]
    assert actor_fixture["ir_call_tokens"][
        "call i32 @objc3_runtime_actor_mailbox_enqueue_i32"
    ] is True
    assert actor_fixture["runtime_import_surface"][
        "objc_concurrency_actor_mailbox_and_isolation_runtime_import_surface.actor_mailbox_runtime_ready"
    ] is True

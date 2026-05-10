from __future__ import annotations

from typing import Any


def render_text(payload: dict[str, Any]) -> str:
    lines = [
        f"contract_id: {payload['contract_id']}",
        f"recommended_class: {payload['recommended_class']}",
        f"split_required: {payload['split_required']}",
        f"rule_trace: {payload['rule_trace']}",
        f"recommended_shard_count: {payload['recommended_shard_count']}",
        f"target_ids_per_shard: {payload['target_ids_per_shard']}",
    ]
    return "\n".join(lines) + "\n"

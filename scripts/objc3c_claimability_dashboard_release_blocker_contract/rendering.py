from __future__ import annotations

from typing import Any


def render_markdown(summary: dict[str, Any]) -> str:
    projection = summary["dashboard_release_blocker_projection"]
    return (
        "# Claimability Dashboard Release-Blocker Contract Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Status: `{summary['status']}`\n"
        f"- Policy: `{summary['source_policy_path']}`\n"
        f"- Dashboard blocker: `{projection['blocker']}`\n"
        f"- Blocking public claim classes: `{', '.join(projection['blocking_public_claim_classes'])}`\n"
        f"- Blocking rollout classes: `{', '.join(projection['blocking_rollout_classes'])}`\n"
        f"- Source truth excludes tmp: `{summary['checks']['source_truth_excludes_tmp']}`\n"
    )

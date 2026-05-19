from __future__ import annotations

from typing import Any

from .constants import SNAPSHOT_KEYS
from .key_order import check_key_order


def validate_snapshot(
    snapshot: dict[str, Any],
    *,
    summary: dict[str, Any],
    contract_id: str,
    contract_version: str,
) -> list[str]:
    findings = check_key_order(snapshot, expected=SNAPSHOT_KEYS, label="snapshot")

    if snapshot.get("contract_id") != contract_id:
        findings.append(
            "snapshot.contract_id drift: "
            f"expected={contract_id!r} observed={snapshot.get('contract_id')!r}."
        )
    if snapshot.get("contract_version") != contract_version:
        findings.append(
            "snapshot.contract_version drift: "
            f"expected={contract_version!r} observed={snapshot.get('contract_version')!r}."
        )

    open_blockers = snapshot.get("open_blockers")
    if not isinstance(open_blockers, list):
        findings.append("snapshot.open_blockers must be a list.")
        open_blocker_count = None
    else:
        open_blocker_count = len(open_blockers)

    if snapshot.get("open_blocker_count") != open_blocker_count:
        findings.append(
            "snapshot.open_blocker_count drift: "
            f"expected={open_blocker_count!r} "
            f"observed={snapshot.get('open_blocker_count')!r}."
        )

    inputs = summary.get("inputs")
    if isinstance(inputs, dict):
        if snapshot.get("generated_at_utc") != inputs.get("generated_at_utc"):
            findings.append(
                "snapshot.generated_at_utc drift: "
                f"expected={inputs.get('generated_at_utc')!r} "
                f"observed={snapshot.get('generated_at_utc')!r}."
            )
        if snapshot.get("source") != inputs.get("source"):
            findings.append(
                "snapshot.source drift: "
                f"expected={inputs.get('source')!r} "
                f"observed={snapshot.get('source')!r}."
            )

    audit = summary.get("audit")
    if (
        isinstance(audit, dict)
        and audit.get("open_blocker_count") != snapshot.get("open_blocker_count")
    ):
        findings.append(
            "summary.audit.open_blocker_count drift: "
            f"expected={snapshot.get('open_blocker_count')!r} "
            f"observed={audit.get('open_blocker_count')!r}."
        )

    return findings

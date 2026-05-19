from __future__ import annotations

from datetime import UTC, datetime

CONTRACT_ID = "activation-seed-contract/v0.15"
TRIGGER_ORDER: tuple[tuple[str, str], ...] = (
    ("T1-ISSUES", "open issues > 0"),
    ("T2-MILESTONES", "open milestones > 0"),
    ("T3-ACTIONABLE-ROWS", "actionable catalog rows > 0"),
    ("T5-OPEN-BLOCKERS", "open blockers > 0"),
)
DEFAULT_ACTIONABLE_STATUSES: tuple[str, ...] = ("open", "open-blocked", "blocked")
OPEN_BLOCKERS_TRIGGER_ID = "T5-OPEN-BLOCKERS"

EXIT_GATE_CLOSED = 0
EXIT_GATE_OPEN = 1
EXIT_HARD_FAILURE = 2


def utc_now_string() -> str:
    return datetime.now(UTC).replace(microsecond=0).isoformat().replace("+00:00", "Z")

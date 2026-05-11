"""Security state and headline selection."""

from __future__ import annotations

from typing import Any


def boundary_status_from_summary(summary: dict[str, Any], *, warn_on_degraded: bool = False, trust_state: str | None = None) -> str:
    if summary.get("status") == "FAIL" or summary.get("ok") is False:
        return "FAIL"
    if warn_on_degraded and trust_state == "degraded":
        return "WARN"
    return "PASS"


def security_state_from_boundaries(trust_boundaries: list[dict[str, Any]]) -> str:
    statuses = {entry["status"] for entry in trust_boundaries}
    if "FAIL" in statuses:
        return "blocked"
    if "WARN" in statuses:
        return "degraded"
    return "ready"


def headline_for_state(security_state: str) -> str:
    if security_state == "ready":
        return "Objective-C 3 security posture is publishable on the current checked-in trust, package, and runtime evidence."
    if security_state == "degraded":
        return "Objective-C 3 security posture is publishable with caution while trust or response signals remain narrow."
    return "Objective-C 3 security posture is blocked until trust, response, or supply-chain regressions are resolved."

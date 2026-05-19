"""Diagnostic policy metadata for runtime acceptance reports."""

from __future__ import annotations

from objc3c_runtime_acceptance.probes import DEFAULT_PROBE_RETRIES
from objc3c_runtime_acceptance.probes import RETRYABLE_PROBE_EXIT_CODES


def build_probe_retry_policy() -> dict[str, object]:
    return {
        "contract_id": "objc3c.runtime.acceptance.probe.retry.policy.v1",
        "default_probe_retries": DEFAULT_PROBE_RETRIES,
        "retryable_exit_codes": sorted(RETRYABLE_PROBE_EXIT_CODES),
        "fail_closed": True,
    }


__all__ = ["build_probe_retry_policy"]

"""Security-hardening workflow target records and command helpers."""

from __future__ import annotations

import sys

from ..action_spec import ActionSpec
from .release_governance_security_names import (
    BUILD_SECURITY_POSTURE,
    CHECK_SECURITY_HARDENING_SCHEMA_SURFACE,
    CHECK_SECURITY_HARDENING_SURFACE,
    CHECK_SECURITY_RESPONSE_DRILL,
    CHECK_SECURITY_RUNTIME_HARDENING,
    PUBLISH_SECURITY_ADVISORIES,
    SECURITY_HARDENING_HARD_CUTOVER_GUARDRAILS,
    VALIDATE_SECURITY_HARDENING,
    VALIDATE_SECURITY_HARDENING_END_TO_END,
)
from .release_governance_security_owner_contracts import SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS
from .release_governance_security_paths import (
    SECURITY_HARDENING_END_TO_END_PY,
    SECURITY_HARDENING_POSTURE_PY,
    SECURITY_HARDENING_PUBLICATION_PY,
    SECURITY_HARDENING_RESPONSE_DRILL_PY,
    SECURITY_HARDENING_RUNTIME_HARDENING_PY,
    SECURITY_HARDENING_SOURCE_SURFACE_PY,
)
from .release_governance_security_target_models import SecurityHardeningTarget
from .schema_surfaces_paths import SECURITY_HARDENING_SCHEMA_SURFACE_PY

SECURITY_HARDENING_PUBLIC_TARGETS: dict[str, SecurityHardeningTarget] = {
    CHECK_SECURITY_HARDENING_SURFACE: SecurityHardeningTarget(
        CHECK_SECURITY_HARDENING_SURFACE,
        "validate the checked-in security-hardening source surface",
        "python:scripts/check_security_hardening_source_surface.py",
        (
            "security hardening only publishes from the checked-in trust boundary, "
            "policy, and workflow contracts"
        ),
        SECURITY_HARDENING_SOURCE_SURFACE_PY,
        "repo",
        SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS,
    ),
    CHECK_SECURITY_HARDENING_SCHEMA_SURFACE: SecurityHardeningTarget(
        CHECK_SECURITY_HARDENING_SCHEMA_SURFACE,
        "validate the checked-in security-hardening schema surface",
        "python:scripts/check_security_hardening_schema_surface.py",
        "security posture and advisory artifacts stay on checked-in schema contracts",
        SECURITY_HARDENING_SCHEMA_SURFACE_PY,
        "repo",
        SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS,
    ),
    BUILD_SECURITY_POSTURE: SecurityHardeningTarget(
        BUILD_SECURITY_POSTURE,
        "derive the machine-owned security posture from live hardening evidence",
        "python:scripts/build_objc3c_security_posture.py",
        (
            "security posture stays derived from the live trust boundary, release, "
            "and runtime evidence"
        ),
        SECURITY_HARDENING_POSTURE_PY,
        "repo",
        SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS,
    ),
    PUBLISH_SECURITY_ADVISORIES: SecurityHardeningTarget(
        PUBLISH_SECURITY_ADVISORIES,
        "publish the machine-owned security advisory artifacts",
        "python:scripts/publish_objc3c_security_advisories.py",
        (
            "security advisory publication stays traceable to the live posture and "
            "checked-in hardening policies"
        ),
        SECURITY_HARDENING_PUBLICATION_PY,
        "repo",
        SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS,
    ),
    VALIDATE_SECURITY_HARDENING: SecurityHardeningTarget(
        VALIDATE_SECURITY_HARDENING,
        "run the integrated security-hardening publication workflow",
        "runner-internal security-hardening child actions",
        (
            "security posture and advisory publication stay executable on the live "
            "release, trust, and hardening surfaces"
        ),
        None,
        "nightly",
        SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS,
    ),
    VALIDATE_SECURITY_HARDENING_END_TO_END: SecurityHardeningTarget(
        VALIDATE_SECURITY_HARDENING_END_TO_END,
        (
            "validate security-hardening entrypoints, command-surface sync, and "
            "publication artifacts end to end"
        ),
        "python:scripts/check_objc3c_security_hardening_end_to_end.py",
        (
            "security-hardening entrypoints and publication artifacts stay coherent "
            "with the live command and evidence surfaces"
        ),
        SECURITY_HARDENING_END_TO_END_PY,
        "full",
        SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS,
    ),
}

SECURITY_HARDENING_INTERNAL_TARGETS: dict[str, SecurityHardeningTarget] = {
    CHECK_SECURITY_RESPONSE_DRILL: SecurityHardeningTarget(
        CHECK_SECURITY_RESPONSE_DRILL,
        "validate response-drill evidence before publishing hardening claims",
        "python:scripts/check_security_hardening_response_drill.py",
        (
            "response drill status stays tied to release, distribution, platform, "
            "and posture evidence"
        ),
        SECURITY_HARDENING_RESPONSE_DRILL_PY,
        "nightly",
        ("response_drill_owner",),
    ),
    CHECK_SECURITY_RUNTIME_HARDENING: SecurityHardeningTarget(
        CHECK_SECURITY_RUNTIME_HARDENING,
        "validate runtime hardening evidence before publishing hardening claims",
        "python:scripts/check_security_hardening_runtime_hardening.py",
        (
            "runtime hardening claims stay gated by runtime acceptance and runnable "
            "release-candidate evidence"
        ),
        SECURITY_HARDENING_RUNTIME_HARDENING_PY,
        "nightly",
        ("runtime_hardening_owner",),
    ),
}

SECURITY_HARDENING_ACTION_SPECS: dict[str, ActionSpec] = {
    action_name: target.to_action_spec()
    for action_name, target in SECURITY_HARDENING_PUBLIC_TARGETS.items()
}

SECURITY_HARDENING_VALIDATION_CHILD_ACTIONS = (
    CHECK_SECURITY_RESPONSE_DRILL,
    CHECK_SECURITY_RUNTIME_HARDENING,
    CHECK_SECURITY_HARDENING_SURFACE,
    CHECK_SECURITY_HARDENING_SCHEMA_SURFACE,
    BUILD_SECURITY_POSTURE,
    PUBLISH_SECURITY_ADVISORIES,
)

SECURITY_HARDENING_ACTION_OWNER_CONTRACT_IDS: dict[str, tuple[str, ...]] = {
    action_name: target.owner_contract_ids
    for action_name, target in (
        SECURITY_HARDENING_PUBLIC_TARGETS | SECURITY_HARDENING_INTERNAL_TARGETS
    ).items()
}


def security_hardening_hard_cutover_guardrails() -> dict[str, object]:
    return dict(SECURITY_HARDENING_HARD_CUTOVER_GUARDRAILS)


def security_hardening_target(action_name: str) -> SecurityHardeningTarget:
    if action_name in SECURITY_HARDENING_PUBLIC_TARGETS:
        return SECURITY_HARDENING_PUBLIC_TARGETS[action_name]
    return SECURITY_HARDENING_INTERNAL_TARGETS[action_name]


def security_hardening_command(action_name: str) -> list[str]:
    target = security_hardening_target(action_name)
    if target.script is None:
        raise ValueError(f"{action_name} is a composite security-hardening action")
    return [sys.executable, str(target.script)]


__all__ = [
    "SECURITY_HARDENING_ACTION_OWNER_CONTRACT_IDS",
    "SECURITY_HARDENING_ACTION_SPECS",
    "SECURITY_HARDENING_INTERNAL_TARGETS",
    "SECURITY_HARDENING_PUBLIC_TARGETS",
    "SECURITY_HARDENING_VALIDATION_CHILD_ACTIONS",
    "security_hardening_command",
    "security_hardening_hard_cutover_guardrails",
    "security_hardening_target",
]

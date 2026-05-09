"""Security-hardening action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

SECURITY_HARDENING_ACTION_SPECS: dict[str, ActionSpec] = {
    "check-security-hardening-surface": ActionSpec("check-security-hardening-surface", "validate the checked-in security-hardening source surface", "python:scripts/check_security_hardening_source_surface.py", validation_tier="repo", guarantee_owner="security hardening only publishes from the checked-in trust boundary, policy, and workflow contracts"),
    "check-security-hardening-schema-surface": ActionSpec("check-security-hardening-schema-surface", "validate the checked-in security-hardening schema surface", "python:scripts/check_security_hardening_schema_surface.py", validation_tier="repo", guarantee_owner="security posture and advisory artifacts stay on checked-in schema contracts"),
    "build-security-posture": ActionSpec("build-security-posture", "derive the machine-owned security posture from live hardening evidence", "python:scripts/build_objc3c_security_posture.py", validation_tier="repo", guarantee_owner="security posture stays derived from the live trust boundary, release, and runtime evidence"),
    "publish-security-advisories": ActionSpec("publish-security-advisories", "publish the machine-owned security advisory artifacts", "python:scripts/publish_objc3c_security_advisories.py", validation_tier="repo", guarantee_owner="security advisory publication stays traceable to the live posture and checked-in hardening policies"),
    "validate-security-hardening": ActionSpec("validate-security-hardening", "run the integrated security-hardening publication workflow", "runner-internal security-hardening child actions", validation_tier="nightly", guarantee_owner="security posture and advisory publication stay executable on the live release, trust, and hardening surfaces"),
    "validate-security-hardening-end-to-end": ActionSpec("validate-security-hardening-end-to-end", "validate security-hardening entrypoints, command-surface sync, and publication artifacts end to end", "python:scripts/check_objc3c_security_hardening_end_to_end.py", validation_tier="full", guarantee_owner="security-hardening entrypoints and publication artifacts stay coherent with the live command and evidence surfaces"),
}

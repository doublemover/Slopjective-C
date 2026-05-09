"""Security-hardening handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import release_governance, schema_surfaces

REPORTING_SECURITY_HANDLERS: dict[str, ActionHandler] = {
    "check-security-hardening-surface": release_governance.action_check_security_hardening_surface,
    "check-security-hardening-schema-surface": schema_surfaces.action_check_security_hardening_schema_surface,
    "build-security-posture": release_governance.action_build_security_posture,
    "publish-security-advisories": release_governance.action_publish_security_advisories,
    "validate-security-hardening": release_governance.action_validate_security_hardening,
    "validate-security-hardening-end-to-end": release_governance.action_validate_security_hardening_end_to_end,
}

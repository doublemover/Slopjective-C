"""Security-hardening workflow paths."""

from __future__ import annotations

from ..environment import ROOT

SECURITY_HARDENING_SOURCE_SURFACE_PY = (
    ROOT / "scripts" / "check_security_hardening_source_surface.py"
)
SECURITY_HARDENING_RESPONSE_DRILL_PY = (
    ROOT / "scripts" / "check_security_hardening_response_drill.py"
)
SECURITY_HARDENING_RUNTIME_HARDENING_PY = (
    ROOT / "scripts" / "check_security_hardening_runtime_hardening.py"
)
SECURITY_HARDENING_SANITIZER_VALIDATION_PY = (
    ROOT / "scripts" / "check_security_sanitizer_validation.py"
)
SECURITY_HARDENING_SANITIZER_EXECUTION_EVIDENCE_PY = (
    ROOT / "scripts" / "check_security_sanitizer_execution_evidence.py"
)
SECURITY_HARDENING_LANGUAGE_RUNTIME_THREAT_MODEL_PY = (
    ROOT / "scripts" / "check_security_language_runtime_threat_model.py"
)
SECURITY_HARDENING_POSTURE_PY = (
    ROOT / "scripts" / "build_objc3c_security_posture.py"
)
SECURITY_HARDENING_PUBLICATION_PY = (
    ROOT / "scripts" / "publish_objc3c_security_advisories.py"
)
SECURITY_HARDENING_END_TO_END_PY = (
    ROOT / "scripts" / "check_objc3c_security_hardening_end_to_end.py"
)

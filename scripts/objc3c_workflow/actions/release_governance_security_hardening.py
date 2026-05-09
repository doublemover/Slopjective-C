"""Security hardening workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from ..composite_validation import run_composite_validation
from ..environment import ROOT
from .schema_surfaces import SECURITY_HARDENING_SCHEMA_SURFACE_PY

SECURITY_HARDENING_SOURCE_SURFACE_PY = (
    ROOT / "scripts" / "check_security_hardening_source_surface.py"
)
SECURITY_HARDENING_RESPONSE_DRILL_PY = (
    ROOT / "scripts" / "check_security_hardening_response_drill.py"
)
SECURITY_HARDENING_RUNTIME_HARDENING_PY = (
    ROOT / "scripts" / "check_security_hardening_runtime_hardening.py"
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


def action_check_security_hardening_surface(_: list[str]) -> int:
    return run([sys.executable, str(SECURITY_HARDENING_SOURCE_SURFACE_PY)])


def action_build_security_posture(_: list[str]) -> int:
    return run([sys.executable, str(SECURITY_HARDENING_POSTURE_PY)])


def action_publish_security_advisories(_: list[str]) -> int:
    return run([sys.executable, str(SECURITY_HARDENING_PUBLICATION_PY)])


def action_validate_security_hardening(_: list[str]) -> int:
    return run_composite_validation(
        "validate-security-hardening",
        [
            (
                "check-security-response-drill",
                [sys.executable, str(SECURITY_HARDENING_RESPONSE_DRILL_PY)],
            ),
            (
                "check-security-runtime-hardening",
                [sys.executable, str(SECURITY_HARDENING_RUNTIME_HARDENING_PY)],
            ),
            (
                "check-security-hardening-surface",
                [sys.executable, str(SECURITY_HARDENING_SOURCE_SURFACE_PY)],
            ),
            (
                "check-security-hardening-schema-surface",
                [sys.executable, str(SECURITY_HARDENING_SCHEMA_SURFACE_PY)],
            ),
            ("build-security-posture", [sys.executable, str(SECURITY_HARDENING_POSTURE_PY)]),
            (
                "publish-security-advisories",
                [sys.executable, str(SECURITY_HARDENING_PUBLICATION_PY)],
            ),
        ],
    )


def action_validate_security_hardening_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(SECURITY_HARDENING_END_TO_END_PY)])

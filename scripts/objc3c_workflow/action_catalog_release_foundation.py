"""Release foundation action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

RELEASE_FOUNDATION_PUBLIC_ACTIONS: tuple[str, ...] = (
    "check-release-foundation-surface",
    "check-release-foundation-schema-surface",
    "build-release-manifest",
    "publish-release-provenance",
    "validate-release-foundation",
)

RELEASE_FOUNDATION_VALIDATE_CHILD_ACTIONS: tuple[str, ...] = (
    "validate-performance-governance",
    "validate-runnable-release-candidate",
    "check-release-evidence",
    "check-release-foundation-surface",
    "check-release-foundation-schema-surface",
    "build-release-manifest",
    "publish-release-provenance",
)

RELEASE_FOUNDATION_ACTION_SPECS: dict[str, ActionSpec] = {
    "check-release-foundation-surface": ActionSpec("check-release-foundation-surface", "validate the checked-in release-foundation source surface", "python:scripts/check_release_foundation_source_surface.py", validation_tier="repo", guarantee_owner="release foundation only publishes from the checked-in release taxonomy, trust, payload, and provenance contracts"),
    "check-release-foundation-schema-surface": ActionSpec("check-release-foundation-schema-surface", "validate the checked-in release-foundation schema surface", "python:scripts/check_release_foundation_schema_surface.py", validation_tier="repo", guarantee_owner="release manifest, sbom, and attestation artifacts stay on checked-in schema contracts"),
    "build-release-manifest": ActionSpec("build-release-manifest", "derive the machine-owned release manifest from repeated runnable package assembly runs", "python:scripts/build_objc3c_release_manifest.py", validation_tier="repo", guarantee_owner="release payload selection and reproducibility proof stay tied to the live runnable package manifest and release-evidence boundary"),
    "publish-release-provenance": ActionSpec("publish-release-provenance", "publish the machine-owned release sbom and attestation artifacts", "python:scripts/publish_objc3c_release_provenance.py", validation_tier="repo", guarantee_owner="release provenance publication stays traceable to the live release manifest, package manifest, and release-evidence index"),
    "validate-release-foundation": ActionSpec("validate-release-foundation", "run the integrated release-foundation workflow", "runner-internal release-foundation child actions", validation_tier="nightly", guarantee_owner="release taxonomy, reproducible package assembly, and provenance publication stay executable on the live runnable package surface"),
}


__all__ = [
    "RELEASE_FOUNDATION_ACTION_SPECS",
    "RELEASE_FOUNDATION_PUBLIC_ACTIONS",
    "RELEASE_FOUNDATION_VALIDATE_CHILD_ACTIONS",
]

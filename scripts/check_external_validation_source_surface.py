#!/usr/bin/env python3
"""Validate the checked-in external validation source surface."""

from __future__ import annotations

from external_validation_source_surface.paths import (
    EXPECTED_ARTIFACT_ROOT,
    EXPECTED_REPORT_ROOT,
    EXPECTED_REQUIRED_PATHS,
    EXPECTED_ROOTS,
    ROOT,
    SOURCE_SURFACE,
    SUMMARY_PATH,
)
from external_validation_source_surface.runner import fail, run
from external_validation_source_surface.source_model import (
    EXPECTED_CONTRACT_IDS,
    EXPECTED_DISCLOSURE_MODES,
    EXPECTED_ESCALATION_TARGETS,
    EXPECTED_FAMILY_IDS,
    EXPECTED_INTAKE_FAMILIES,
    EXPECTED_INTAKE_SURFACES,
    EXPECTED_PUBLISHABLE_TRUST_STATES,
    EXPECTED_QUARANTINE_TRUST_STATES,
    EXPECTED_REQUIRED_PROVENANCE_FIELDS,
    EXPECTED_SOURCE_FAMILY_PATHS,
    EXPECTED_TRUST_STATES,
    SCHEMA_VERSION,
    SOURCE_SURFACE_CONTRACT_ID,
    SUMMARY_CONTRACT_ID,
)
from external_validation_source_surface.validation import (
    require_contract_id,
    require_exact_list,
    require_exact_path,
    require_path,
)


def main() -> int:
    return run(
        source_surface_path=SOURCE_SURFACE,
        summary_path=SUMMARY_PATH,
        expected_roots=EXPECTED_ROOTS,
    )


if __name__ == "__main__":
    raise SystemExit(main())

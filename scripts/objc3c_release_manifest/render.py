"""Release manifest artifact writers."""

from __future__ import annotations

from typing import Any

from objc3c_tooling.json_io import write_json_file

from .model import PackageAssembly, ReleaseValidation, build_release_manifest_summary
from .paths import MANIFEST_PATH, SOURCE_SURFACE, SUMMARY_PATH


def write_release_manifest_artifacts(
    *,
    payload: dict[str, Any],
    first: PackageAssembly,
    validation: ReleaseValidation,
) -> dict[str, Any]:
    MANIFEST_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(MANIFEST_PATH, payload)

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = build_release_manifest_summary(
        payload=payload,
        source_surface=SOURCE_SURFACE,
        manifest_path=MANIFEST_PATH,
        first=first,
        validation=validation,
    )
    write_json_file(SUMMARY_PATH, summary)
    return summary

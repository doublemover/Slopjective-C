#!/usr/bin/env python3
"""Stable entrypoint for the conformance evidence index generator."""

from __future__ import annotations

from conformance_evidence_index.builder import (
    build_artifact_records,
    build_index_payload,
    build_profiles_index,
    build_releases_index,
)
from conformance_evidence_index.cli import build_parser, main
from conformance_evidence_index.constants import (
    ARTIFACT_AUTHENTICITY_SCHEMA_ID,
    DEFAULT_GLOBS,
    DEFAULT_INPUT_ROOT,
    EVIDENCE_INDEX_ARTIFACT_FAMILY_ID,
    EVIDENCE_INDEX_REPORT_FAMILY_ID,
    EVIDENCE_INDEX_SURFACE_ID,
    GENERATOR_COMMAND,
    GENERATOR_PATH,
    GENERATOR_SCRIPT,
    INDEX_VERSION,
    ROOT,
    SCHEMA_ID,
    SUPPORT_CLAIM_RUNNABLE_EVIDENCE_CATALOG,
    UNKNOWN_PROFILE,
    UNKNOWN_RELEASE,
)
from conformance_evidence_index.manifest import (
    QUARTER_RELEASE_RE,
    SEMVER_RELEASE_RE,
    SUFFIX_RELEASE_RE,
    canonicalize_generated_at,
    coerce_str,
    infer_profile_release,
    is_release_token,
    split_profile_release,
    strip_file_stem_suffixes,
    trim_known_manifest_suffix,
)
from conformance_evidence_index.model import ArtifactRecord
from conformance_evidence_index.paths import (
    collect_artifact_paths,
    detect_media_type,
    file_sha256,
    load_json_object,
    normalize_pattern_list,
    normalize_repo_path,
    path_sort_key,
    resolve_repo_path,
)
from conformance_evidence_index.timestamps import (
    StrictGeneratedAtError,
    parse_rfc3339_utc,
    resolve_index_generated_at,
    source_date_epoch_to_utc,
)


if __name__ == "__main__":
    raise SystemExit(main())

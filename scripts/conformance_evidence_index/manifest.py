from __future__ import annotations

import re
from pathlib import Path
from typing import Any

from conformance_evidence_index.constants import UNKNOWN_PROFILE, UNKNOWN_RELEASE
from conformance_evidence_index.timestamps import (
    StrictGeneratedAtError,
    parse_rfc3339_utc,
)

QUARTER_RELEASE_RE = re.compile(r"^\d{4}Q[1-4]$")
SEMVER_RELEASE_RE = re.compile(r"^v?\d+\.\d+(?:\.\d+)?(?:[-+][A-Za-z0-9_.-]+)?$")
SUFFIX_RELEASE_RE = re.compile(
    r"^(?P<profile>.+)-(?P<release>\d{4}Q[1-4]|v?\d+\.\d+(?:\.\d+)?(?:[-+][A-Za-z0-9_.-]+)?)$"
)


def is_release_token(value: str | None) -> bool:
    if not value:
        return False
    return bool(QUARTER_RELEASE_RE.match(value) or SEMVER_RELEASE_RE.match(value))


def split_profile_release(token: str | None) -> tuple[str | None, str | None]:
    if not token:
        return None, None
    match = SUFFIX_RELEASE_RE.match(token)
    if not match:
        return None, None
    return match.group("profile"), match.group("release")


def trim_known_manifest_suffix(token: str | None) -> str | None:
    if not token:
        return None
    cleaned = token.strip()
    if not cleaned:
        return None
    for suffix in ("-artifact-manifest", "-manifest"):
        if cleaned.endswith(suffix) and len(cleaned) > len(suffix):
            return cleaned[: -len(suffix)]
    return None


def strip_file_stem_suffixes(stem: str) -> str:
    cleaned = stem
    for suffix in (".manifest", ".example", ".sample", ".index"):
        if cleaned.endswith(suffix):
            cleaned = cleaned[: -len(suffix)]
    return cleaned


def coerce_str(value: Any) -> str | None:
    if not isinstance(value, str):
        return None
    stripped = value.strip()
    return stripped or None


def canonicalize_generated_at(
    value: str | None,
    *,
    strict: bool,
    artifact_path: str,
) -> str | None:
    if not value:
        return None
    try:
        return parse_rfc3339_utc(value)
    except ValueError as exc:
        if strict:
            raise StrictGeneratedAtError(
                f"{artifact_path}: invalid generated_at {value!r}; expected RFC3339 date-time"
            ) from exc
        return value


def infer_profile_release(
    *,
    rel_path: str,
    payload: dict[str, Any] | None,
    release_retired_route: str | None,
    strict_generated_at: bool,
) -> tuple[str, str, str, str, str | None, str | None, str | None]:
    manifest_kind = "generic-json"
    schema_ref: str | None = None
    artifact_id: str | None = None
    source_generated_at: str | None = None
    issue_ref: str | None = None

    profile: str | None = None
    release: str | None = None
    candidate_tokens: list[str] = []

    if payload:
        explicit_profile = coerce_str(payload.get("profile_id"))
        manifest_kind = (
            coerce_str(payload.get("manifest_type"))
            or coerce_str(payload.get("bundle_type"))
            or coerce_str(payload.get("schema_id"))
            or coerce_str(payload.get("bundle_schema"))
            or "generic-json"
        )
        schema_ref = (
            coerce_str(payload.get("schema_id"))
            or coerce_str(payload.get("manifest_schema"))
            or coerce_str(payload.get("bundle_schema"))
        )
        artifact_id = (
            coerce_str(payload.get("artifact_id"))
            or coerce_str(payload.get("manifest_schema"))
            or coerce_str(payload.get("bundle_schema"))
            or coerce_str(payload.get("bundle_type"))
        )
        source_generated_at = canonicalize_generated_at(
            coerce_str(payload.get("generated_at")),
            strict=strict_generated_at,
            artifact_path=rel_path,
        )
        issue_ref = coerce_str(payload.get("issue_ref"))

        release = (
            coerce_str(payload.get("release_id"))
            or coerce_str(payload.get("release_stamp"))
            or coerce_str(payload.get("release"))
            or coerce_str(payload.get("release_train"))
        )
        spec_baseline = payload.get("spec_baseline")
        if release is None and isinstance(spec_baseline, dict):
            release = coerce_str(spec_baseline.get("release_train"))
        if release and not is_release_token(release):
            release = None

        profile_claim = payload.get("profile_claim")
        if isinstance(profile_claim, dict):
            claim_profile = coerce_str(profile_claim.get("profile"))
            if explicit_profile is None and claim_profile:
                explicit_profile = claim_profile
            if claim_profile:
                candidate_tokens.append(claim_profile)
        if explicit_profile:
            profile = explicit_profile

        for key in (
            "profile_id",
            "artifact_id",
            "manifest_schema",
            "manifest_type",
            "bundle_schema",
            "bundle_type",
            "schema_id",
        ):
            candidate = coerce_str(payload.get(key))
            if candidate:
                candidate_tokens.append(candidate)

    stem_token = strip_file_stem_suffixes(Path(rel_path).with_suffix("").name)
    if stem_token:
        candidate_tokens.append(stem_token)

    for token in candidate_tokens:
        normalized = token.split("/", 1)[0]
        profile_candidate, release_candidate = split_profile_release(normalized)
        if release is None and release_candidate:
            release = release_candidate

        if profile is None and profile_candidate:
            profile = profile_candidate
            continue

        if profile is None:
            suffix_trimmed = trim_known_manifest_suffix(normalized)
            if suffix_trimmed:
                profile = suffix_trimmed
                continue

    if artifact_id is None:
        artifact_id = stem_token or Path(rel_path).name

    if profile is None:
        profile = UNKNOWN_PROFILE
    else:
        profile_candidate, release_candidate = split_profile_release(profile)
        if profile_candidate and release_candidate:
            profile = profile_candidate
            if release is None or release == UNKNOWN_RELEASE:
                release = release_candidate
    if release is None:
        release = release_retired_route or UNKNOWN_RELEASE

    return (
        profile,
        release,
        artifact_id,
        manifest_kind,
        schema_ref,
        source_generated_at,
        issue_ref,
    )

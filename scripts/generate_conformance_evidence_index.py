#!/usr/bin/env python3
"""Generate a deterministic conformance evidence index JSON."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import sys
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_INPUT_ROOT = Path("reports/conformance")
DEFAULT_GLOBS = ("**/*.json",)
SCHEMA_ID = "objc3-conformance-evidence-index/v1"
INDEX_VERSION = 1
UNKNOWN_PROFILE = "unknown-profile"
UNKNOWN_RELEASE = "unknown-release"

QUARTER_RELEASE_RE = re.compile(r"^\d{4}Q[1-4]$")
SEMVER_RELEASE_RE = re.compile(r"^v?\d+\.\d+(?:\.\d+)?(?:[-+][A-Za-z0-9_.-]+)?$")
SUFFIX_RELEASE_RE = re.compile(
    r"^(?P<profile>.+)-(?P<release>\d{4}Q[1-4]|v?\d+\.\d+(?:\.\d+)?(?:[-+][A-Za-z0-9_.-]+)?)$"
)


class StrictGeneratedAtError(ValueError):
    """Raised when strict source generated_at validation fails."""


@dataclass(frozen=True)
class ArtifactRecord:
    artifact_path: str
    file_sha256: str
    size_bytes: int
    media_type: str
    profile_id: str
    release_id: str
    artifact_id: str
    manifest_kind: str
    schema_ref: str | None
    source_generated_at: str | None
    issue_ref: str | None

    def as_dict(self) -> dict[str, Any]:
        return {
            "artifact_path": self.artifact_path,
            "file_sha256": self.file_sha256,
            "size_bytes": self.size_bytes,
            "media_type": self.media_type,
            "profile_id": self.profile_id,
            "release_id": self.release_id,
            "artifact_id": self.artifact_id,
            "manifest_kind": self.manifest_kind,
            "schema_ref": self.schema_ref,
            "source_generated_at": self.source_generated_at,
            "issue_ref": self.issue_ref,
        }


def normalize_repo_path(path: Path) -> str:
    try:
        return path.resolve().relative_to(ROOT).as_posix()
    except ValueError:
        return path.resolve().as_posix()


def parse_rfc3339_utc(value: str) -> str:
    raw = value.strip()
    if not raw:
        raise ValueError("timestamp cannot be empty")

    normalized = raw
    if raw.endswith("Z"):
        normalized = raw[:-1] + "+00:00"

    try:
        parsed = datetime.fromisoformat(normalized)
    except ValueError as exc:
        raise ValueError(
            f"invalid timestamp {value!r}; expected RFC3339 date-time"
        ) from exc

    if parsed.tzinfo is None:
        raise ValueError(
            f"invalid timestamp {value!r}; timezone offset or Z suffix is required"
        )

    canonical = parsed.astimezone(timezone.utc).replace(microsecond=0)
    return canonical.isoformat().replace("+00:00", "Z")


def source_date_epoch_to_utc(value: str) -> str:
    try:
        epoch = int(value)
    except ValueError as exc:
        raise ValueError(
            f"SOURCE_DATE_EPOCH must be an integer; got {value!r}"
        ) from exc

    if epoch < 0:
        raise ValueError("SOURCE_DATE_EPOCH must be >= 0")

    parsed = datetime.fromtimestamp(epoch, tz=timezone.utc).replace(microsecond=0)
    return parsed.isoformat().replace("+00:00", "Z")


def resolve_index_generated_at(explicit_generated_at: str | None) -> str | None:
    if explicit_generated_at:
        return parse_rfc3339_utc(explicit_generated_at)

    source_date_epoch = os.getenv("SOURCE_DATE_EPOCH")
    if source_date_epoch:
        return source_date_epoch_to_utc(source_date_epoch)

    return None


def resolve_repo_path(raw_path: str) -> Path:
    candidate = Path(raw_path)
    if not candidate.is_absolute():
        candidate = ROOT / candidate
    return candidate.resolve()


def normalize_pattern_list(patterns: Sequence[str] | None, defaults: Sequence[str]) -> list[str]:
    if not patterns:
        return list(defaults)
    normalized = [pattern.strip() for pattern in patterns if pattern.strip()]
    return normalized or list(defaults)


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
    release_fallback: str | None,
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
        schema_ref = coerce_str(payload.get("schema_id")) or coerce_str(
            payload.get("manifest_schema")
        ) or coerce_str(payload.get("bundle_schema"))
        artifact_id = coerce_str(payload.get("artifact_id")) or coerce_str(
            payload.get("manifest_schema")
        ) or coerce_str(payload.get("bundle_schema")) or coerce_str(payload.get("bundle_type"))
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
    if release is None:
        release = release_fallback or UNKNOWN_RELEASE

    return (
        profile,
        release,
        artifact_id,
        manifest_kind,
        schema_ref,
        source_generated_at,
        issue_ref,
    )


def detect_media_type(path: Path) -> str:
    suffix = path.suffix.lower()
    if suffix == ".json":
        return "application/json"
    if suffix == ".md":
        return "text/markdown"
    if suffix == ".txt":
        return "text/plain"
    return "application/octet-stream"


def file_sha256(path: Path) -> str:
    hasher = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(131072), b""):
            hasher.update(chunk)
    return f"sha256:{hasher.hexdigest()}"


def load_json_object(path: Path) -> dict[str, Any] | None:
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except (UnicodeDecodeError, json.JSONDecodeError):
        return None

    if isinstance(payload, dict):
        return payload
    return None


def path_sort_key(path_string: str) -> str:
    return path_string.casefold()


def collect_artifact_paths(
    *,
    input_root: Path,
    globs: Sequence[str],
    excluded_paths: set[Path],
) -> list[Path]:
    paths: set[Path] = set()
    for pattern in globs:
        for candidate in input_root.glob(pattern):
            resolved = candidate.resolve()
            if not candidate.is_file():
                continue
            if resolved in excluded_paths:
                continue
            paths.add(resolved)
    return sorted(paths, key=lambda candidate: path_sort_key(normalize_repo_path(candidate)))


def build_artifact_records(
    *,
    artifact_paths: Sequence[Path],
    release_fallback: str | None,
    strict_generated_at: bool = False,
) -> list[ArtifactRecord]:
    records: list[ArtifactRecord] = []

    for path in artifact_paths:
        rel_path = normalize_repo_path(path)
        payload = load_json_object(path)
        (
            profile_id,
            release_id,
            artifact_id,
            manifest_kind,
            schema_ref,
            source_generated_at,
            issue_ref,
        ) = infer_profile_release(
            rel_path=rel_path,
            payload=payload,
            release_fallback=release_fallback,
            strict_generated_at=strict_generated_at,
        )

        records.append(
            ArtifactRecord(
                artifact_path=rel_path,
                file_sha256=file_sha256(path),
                size_bytes=path.stat().st_size,
                media_type=detect_media_type(path),
                profile_id=profile_id,
                release_id=release_id,
                artifact_id=artifact_id,
                manifest_kind=manifest_kind,
                schema_ref=schema_ref,
                source_generated_at=source_generated_at,
                issue_ref=issue_ref,
            )
        )

    return sorted(
        records,
        key=lambda record: (
            record.profile_id.casefold(),
            record.release_id.casefold(),
            record.artifact_path.casefold(),
        ),
    )


def build_profiles_index(records: Sequence[ArtifactRecord]) -> list[dict[str, Any]]:
    by_profile: dict[str, dict[str, list[str]]] = {}
    for record in records:
        release_map = by_profile.setdefault(record.profile_id, {})
        release_map.setdefault(record.release_id, []).append(record.artifact_path)

    profiles: list[dict[str, Any]] = []
    for profile_id in sorted(by_profile.keys(), key=str.casefold):
        release_map = by_profile[profile_id]
        releases: list[dict[str, Any]] = []
        artifact_total = 0
        for release_id in sorted(release_map.keys(), key=str.casefold):
            artifact_paths = sorted(release_map[release_id], key=str.casefold)
            artifact_total += len(artifact_paths)
            releases.append(
                {
                    "release_id": release_id,
                    "artifact_count": len(artifact_paths),
                    "artifact_paths": artifact_paths,
                }
            )
        profiles.append(
            {
                "profile_id": profile_id,
                "artifact_count": artifact_total,
                "releases": releases,
            }
        )
    return profiles


def build_releases_index(records: Sequence[ArtifactRecord]) -> list[dict[str, Any]]:
    by_release: dict[str, dict[str, list[str]]] = {}
    for record in records:
        profile_map = by_release.setdefault(record.release_id, {})
        profile_map.setdefault(record.profile_id, []).append(record.artifact_path)

    releases: list[dict[str, Any]] = []
    for release_id in sorted(by_release.keys(), key=str.casefold):
        profile_map = by_release[release_id]
        profiles: list[dict[str, Any]] = []
        artifact_total = 0
        for profile_id in sorted(profile_map.keys(), key=str.casefold):
            artifact_paths = sorted(profile_map[profile_id], key=str.casefold)
            artifact_total += len(artifact_paths)
            profiles.append(
                {
                    "profile_id": profile_id,
                    "artifact_count": len(artifact_paths),
                    "artifact_paths": artifact_paths,
                }
            )
        releases.append(
            {
                "release_id": release_id,
                "artifact_count": artifact_total,
                "profiles": profiles,
            }
        )
    return releases


def build_index_payload(
    *,
    records: Sequence[ArtifactRecord],
    input_root: Path,
    release_label: str | None,
    generated_at: str | None,
) -> dict[str, Any]:
    profiles = build_profiles_index(records)
    releases = build_releases_index(records)
    return {
        "schema_id": SCHEMA_ID,
        "index_version": INDEX_VERSION,
        "release_label": release_label,
        "generated_at": generated_at,
        "input_root": normalize_repo_path(input_root),
        "artifact_count": len(records),
        "profile_count": len(profiles),
        "release_count": len(releases),
        "artifacts": [record.as_dict() for record in records],
        "profiles": profiles,
        "releases": releases,
    }


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="generate_conformance_evidence_index.py",
        description=(
            "Scan conformance evidence artifacts and emit a deterministic "
            "profile/release index JSON."
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=(
            "Examples:\n"
            "  python scripts/generate_conformance_evidence_index.py \\\n"
            "    --output reports/conformance/evidence-index.v0.11.sample.json \\\n"
            "    --release-label v0.11 \\\n"
            "    --generated-at 2026-02-23T00:00:00Z\n\n"
            "  SOURCE_DATE_EPOCH=1767139200 python scripts/generate_conformance_evidence_index.py \\\n"
            "    --output reports/conformance/evidence-index.v0.11.json \\\n"
            "    --release-label v0.11"
        ),
    )
    parser.add_argument(
        "--input-root",
        default=str(DEFAULT_INPUT_ROOT),
        help=(
            "Artifact root directory to scan. Defaults to "
            "'reports/conformance' (relative to repository root)."
        ),
    )
    parser.add_argument(
        "--glob",
        action="append",
        default=None,
        help=(
            "Glob pattern(s) under --input-root to include. May be repeated. "
            "Defaults to '**/*.json'."
        ),
    )
    parser.add_argument(
        "--exclude",
        action="append",
        default=None,
        help=(
            "Repository-relative file path(s) to exclude from indexing. "
            "May be repeated."
        ),
    )
    parser.add_argument(
        "--output",
        default="-",
        help=(
            "Output path for index JSON, relative to repository root. "
            "Use '-' to write to stdout (default)."
        ),
    )
    parser.add_argument(
        "--release-label",
        default=None,
        help=(
            "Optional index-level release label (for example: v0.11). "
            "Also used as fallback release_id when an artifact has no release token."
        ),
    )
    parser.add_argument(
        "--generated-at",
        default=None,
        help=(
            "Optional RFC3339 timestamp (UTC) to embed in the index. "
            "If omitted, SOURCE_DATE_EPOCH is used when set."
        ),
    )
    parser.add_argument(
        "--strict-generated-at",
        action="store_true",
        help=(
            "Fail with exit code 2 when an artifact generated_at value is not "
            "valid RFC3339 with timezone."
        ),
    )
    parser.add_argument(
        "--allow-empty",
        action="store_true",
        help="Allow writing an empty index when no artifacts match.",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)

    input_root = resolve_repo_path(args.input_root)
    if not input_root.exists():
        parser.error(f"--input-root does not exist: {normalize_repo_path(input_root)}")
    if not input_root.is_dir():
        parser.error(f"--input-root is not a directory: {normalize_repo_path(input_root)}")

    globs = normalize_pattern_list(args.glob, DEFAULT_GLOBS)
    excluded_paths: set[Path] = set()
    for raw_exclusion in normalize_pattern_list(args.exclude, ()):
        excluded_paths.add(resolve_repo_path(raw_exclusion))

    output_path: Path | None = None
    if args.output != "-":
        output_path = resolve_repo_path(args.output)
        if output_path in excluded_paths:
            pass
        elif output_path.is_relative_to(input_root):
            excluded_paths.add(output_path)

    try:
        generated_at = resolve_index_generated_at(args.generated_at)
    except ValueError as exc:
        parser.error(str(exc))

    artifact_paths = collect_artifact_paths(
        input_root=input_root,
        globs=globs,
        excluded_paths=excluded_paths,
    )
    if not artifact_paths and not args.allow_empty:
        parser.error(
            "no artifacts matched; check --input-root/--glob or pass --allow-empty"
        )

    try:
        records = build_artifact_records(
            artifact_paths=artifact_paths,
            release_fallback=args.release_label,
            strict_generated_at=args.strict_generated_at,
        )
    except StrictGeneratedAtError as exc:
        parser.error(str(exc))
    payload = build_index_payload(
        records=records,
        input_root=input_root,
        release_label=args.release_label,
        generated_at=generated_at,
    )
    rendered = json.dumps(payload, indent=2, ensure_ascii=True) + "\n"

    if output_path is None:
        sys.stdout.write(rendered)
        return 0

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(rendered, encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

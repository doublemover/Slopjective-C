from __future__ import annotations

import hashlib
import re
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import display_path

ROOT = Path(__file__).resolve().parents[2]
TMP_ROOT = ROOT / "tmp"

DEFAULT_DIMENSION_MAP = {
    "diagnostics": "module.diagnostics.json",
    "manifest": "module.manifest.json",
    "ir": "module.ll",
    "object": "module.o",
}
DIMENSION_ORDER = ("diagnostics", "manifest", "ir", "object")
SHA256_PATTERN = re.compile(r"^[0-9a-f]{64}$")
ARTIFACT_NAME_PATTERN = re.compile(r"^[A-Za-z0-9][A-Za-z0-9._-]{0,127}$")
WORK_KEY_PATTERN = re.compile(r"^[A-Za-z0-9][A-Za-z0-9._-]{0,63}$")


@dataclass(frozen=True)
class ArtifactDigest:
    source_kind: str
    source_path: str
    sha256: str


def sha256_hex(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        while True:
            chunk = handle.read(1024 * 1024)
            if not chunk:
                break
            digest.update(chunk)
    return digest.hexdigest()


def ensure_directory(path: Path, *, label: str) -> None:
    if not path.exists():
        raise ValueError(f"{label} does not exist: {display_path(path)}")
    if not path.is_dir():
        raise ValueError(f"{label} must be a directory: {display_path(path)}")


def ensure_file(path: Path, *, label: str) -> None:
    if not path.exists():
        raise ValueError(f"{label} does not exist: {display_path(path)}")
    if not path.is_file():
        raise ValueError(f"{label} must be a file: {display_path(path)}")


def ensure_under_tmp(path: Path, *, label: str) -> None:
    try:
        resolved = path.resolve()
    except OSError:
        resolved = path
    try:
        resolved.relative_to(TMP_ROOT.resolve())
    except ValueError as exc:
        raise ValueError(
            f"{label} must be under {display_path(TMP_ROOT)}: {display_path(path)}"
        ) from exc


def sha256_text(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def default_dimension_map_for_emit_prefix(*, emit_prefix: str, object_artifact: str) -> dict[str, str]:
    return {
        "diagnostics": f"{emit_prefix}.diagnostics.json",
        "manifest": f"{emit_prefix}.manifest.json",
        "ir": f"{emit_prefix}.ll",
        "object": object_artifact,
    }


def normalize_artifact_name(raw: str, *, context: str) -> str:
    normalized = raw.strip()
    if not normalized:
        raise ValueError(f"{context} must be non-empty")
    if "/" in normalized or "\\" in normalized:
        raise ValueError(
            f"{context} must be a filename only (no path separators): {raw!r}"
        )
    if normalized in {".", ".."}:
        raise ValueError(f"{context} must not be '.' or '..': {raw!r}")
    if not ARTIFACT_NAME_PATTERN.fullmatch(normalized):
        raise ValueError(
            f"{context} contains unsupported characters: {raw!r}; "
            "expected [A-Za-z0-9._-] with leading alphanumeric"
        )
    return normalized


def normalize_work_key(raw: str) -> str:
    value = raw.strip()
    if not value:
        raise ValueError("--work-key must be non-empty when provided")
    if not WORK_KEY_PATTERN.fullmatch(value):
        raise ValueError(
            "--work-key must match [A-Za-z0-9][A-Za-z0-9._-]{0,63} "
            "(path separators and traversal are not allowed)"
        )
    return value


def parse_dimension_map(
    values: Sequence[str],
    *,
    default_mapping: dict[str, str],
) -> dict[str, str]:
    mapping = dict(default_mapping)
    for raw in values:
        if "=" not in raw:
            raise ValueError(
                f"--dimension-map entry must use DIMENSION=ARTIFACT format: {raw!r}"
            )
        dimension, artifact = raw.split("=", 1)
        dimension = dimension.strip()
        artifact = artifact.strip()
        if dimension not in DEFAULT_DIMENSION_MAP:
            supported = ", ".join(DIMENSION_ORDER)
            raise ValueError(
                f"unsupported dimension {dimension!r}; expected one of: {supported}"
            )
        if not artifact:
            raise ValueError(
                f"--dimension-map artifact path must be non-empty for {dimension!r}"
            )
        mapping[dimension] = normalize_artifact_name(
            artifact,
            context=f"--dimension-map artifact for {dimension!r}",
        )
    return mapping


def normalize_artifacts(values: Sequence[str]) -> list[str]:
    normalized = sorted(
        {
            normalize_artifact_name(value, context="--artifacts entry")
            for value in values
            if value and value.strip()
        }
    )
    if not normalized:
        raise ValueError("--artifacts must include at least one artifact filename")
    return normalized


def parse_proxy_digest(path: Path) -> str:
    text = path.read_text(encoding="utf-8").strip().lower()
    if not SHA256_PATTERN.fullmatch(text):
        raise ValueError(
            f"invalid sha256 proxy digest in {display_path(path)}; expected 64 lowercase hex chars"
        )
    return text


def resolve_artifact_digest(
    *,
    base_dir: Path,
    artifact_name: str,
) -> ArtifactDigest:
    artifact_path = base_dir / artifact_name
    if artifact_path.exists():
        if not artifact_path.is_file():
            raise ValueError(
                f"artifact path must be a file: {display_path(artifact_path)}"
            )
        return ArtifactDigest(
            source_kind="artifact-bytes",
            source_path=display_path(artifact_path),
            sha256=sha256_hex(artifact_path),
        )

    proxy_path = base_dir / f"{artifact_name}.sha256"
    if proxy_path.exists():
        if not proxy_path.is_file():
            raise ValueError(
                f"proxy digest path must be a file: {display_path(proxy_path)}"
            )
        return ArtifactDigest(
            source_kind="sha256-proxy",
            source_path=display_path(proxy_path),
            sha256=parse_proxy_digest(proxy_path),
        )

    raise ValueError(
        "missing artifact and proxy digest: "
        f"{display_path(artifact_path)} (or {display_path(proxy_path)})"
    )


def build_dimension_results(
    *,
    artifacts: Sequence[str],
    dimension_map: dict[str, str],
) -> list[dict[str, str]]:
    artifact_set = set(artifacts)
    results: list[dict[str, str]] = []
    for dimension in DIMENSION_ORDER:
        artifact = dimension_map[dimension]
        if artifact in artifact_set:
            status = "compared"
        else:
            status = "skipped"
        results.append(
            {
                "dimension": dimension,
                "artifact": artifact,
                "status": status,
            }
        )
    return results

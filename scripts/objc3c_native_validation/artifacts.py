"""Publish and validate explicit artifacts from completed native validation nodes."""

from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path
import re
import stat
import tempfile
from typing import Sequence


NODE_RESULT_CONTRACT_ID = "objc3c.native_validation.node_result.v1"
_SHA256 = re.compile(r"[0-9a-f]{64}\Z")


class NodeResultError(ValueError):
    """A node result cannot be reused because its record or artifacts are invalid."""


def _require_text(value: object, label: str) -> str:
    if not isinstance(value, str) or not value.strip():
        raise NodeResultError(f"{label} must be a nonempty string")
    return value


def _allowed_roots(roots: Sequence[Path]) -> tuple[Path, ...]:
    if not roots or isinstance(roots, (str, bytes)):
        raise NodeResultError("allowed_roots must contain at least one directory")
    resolved: list[Path] = []
    for root in roots:
        try:
            candidate = Path(root).resolve(strict=True)
            if not candidate.is_dir():
                raise NodeResultError(f"allowed root is not a directory: {root}")
        except (OSError, RuntimeError, TypeError, ValueError) as error:
            raise NodeResultError(f"invalid allowed root {root!r}: {error}") from error
        if candidate not in resolved:
            resolved.append(candidate)
    return tuple(resolved)


def _contained(path: Path, roots: tuple[Path, ...]) -> None:
    if not any(path.is_relative_to(root) for root in roots):
        raise NodeResultError(f"artifact is outside allowed roots: {path}")


def _file_stamp(info: os.stat_result) -> tuple[int, ...]:
    # On Windows, path stat adds executable permission bits from .exe/.com
    # suffixes while fstat does not. Compare the actual file type there; inode,
    # size and modification time still detect replacement or content changes.
    mode = stat.S_IFMT(info.st_mode) if os.name == "nt" else info.st_mode
    return (
        info.st_dev,
        info.st_ino,
        mode,
        info.st_size,
        info.st_mtime_ns,
    )


def _artifact_record(path: Path, roots: tuple[Path, ...]) -> dict:
    try:
        resolved = path.resolve(strict=True)
        _contained(resolved, roots)
        if not stat.S_ISREG(resolved.stat().st_mode):
            raise NodeResultError(f"artifact is not a regular file: {path}")
        with resolved.open("rb") as stream:
            before = os.fstat(stream.fileno())
            if not stat.S_ISREG(before.st_mode):
                raise NodeResultError(f"artifact is not a regular file: {path}")
            digest = hashlib.sha256()
            size = 0
            while chunk := stream.read(1024 * 1024):
                digest.update(chunk)
                size += len(chunk)
            after = os.fstat(stream.fileno())
            current_path = path.resolve(strict=True)
            _contained(current_path, roots)
            current = current_path.stat()
            if (
                current_path != resolved
                or _file_stamp(before) != _file_stamp(after)
                or _file_stamp(before) != _file_stamp(current)
                or size != before.st_size
            ):
                raise NodeResultError(f"artifact changed while being hashed: {path}")
    except NodeResultError:
        raise
    except (OSError, RuntimeError, TypeError, ValueError) as error:
        raise NodeResultError(f"cannot validate artifact {path}: {error}") from error
    return {"path": str(resolved), "size": size, "sha256": digest.hexdigest()}


def _unique_object(pairs: list[tuple[str, object]]) -> dict:
    result: dict = {}
    for key, value in pairs:
        if key in result:
            raise NodeResultError(f"duplicate JSON object key: {key}")
        result[key] = value
    return result


def _nonfinite_constant(value: str) -> None:
    raise NodeResultError(f"invalid nonfinite JSON value: {value}")


def _decode(contents: str) -> dict:
    try:
        result = json.loads(
            contents,
            object_pairs_hook=_unique_object,
            parse_constant=_nonfinite_constant,
        )
    except (ValueError, RecursionError) as error:
        raise NodeResultError(f"invalid node result JSON: {error}") from error
    if not isinstance(result, dict):
        raise NodeResultError("node result must be a JSON object")
    return result


def _validate_schema(record: dict) -> None:
    if record.get("contract_id") != NODE_RESULT_CONTRACT_ID:
        raise NodeResultError("node result contract_id is not supported")
    if record.get("status") != "PASS":
        raise NodeResultError("node result status must be PASS")
    for field in ("node_id", "node_identity", "build_identity", "run_id"):
        _require_text(record.get(field), field)
    if not isinstance(record.get("data"), dict):
        raise NodeResultError("node result data must be an object")
    inventory = record.get("artifacts")
    if not isinstance(inventory, dict) or not inventory:
        raise NodeResultError("node result artifacts must be a nonempty object")
    for role, artifact in inventory.items():
        _require_text(role, "artifact role")
        if not isinstance(artifact, dict):
            raise NodeResultError(f"artifact {role!r} must be an object")
        path = _require_text(artifact.get("path"), f"artifact {role!r} path")
        if not Path(path).is_absolute():
            raise NodeResultError(f"artifact {role!r} path must be absolute")
        size = artifact.get("size")
        if type(size) is not int or size < 0:
            raise NodeResultError(f"artifact {role!r} size must be a nonnegative integer")
        digest = artifact.get("sha256")
        if not isinstance(digest, str) or not _SHA256.fullmatch(digest):
            raise NodeResultError(f"artifact {role!r} sha256 must be 64 lowercase hex digits")


def write_node_result(
    path: Path,
    *,
    node_id: str,
    node_identity: str,
    build_identity: str,
    run_id: str,
    artifacts: dict[str, Path],
    data: dict,
    allowed_roots: Sequence[Path],
) -> dict:
    """Hash every explicit artifact and atomically publish a completed PASS record.

    Invalid input raises NodeResultError. Publication I/O failures raise OSError
    and preserve any prior result. Paths embedded in data are ordinary metadata.
    """
    roots = _allowed_roots(allowed_roots)
    if not isinstance(artifacts, dict) or not artifacts:
        raise NodeResultError("artifacts must be a nonempty object")
    target = Path(path)
    target = target.parent.resolve() / target.name
    inventory: dict = {}
    for role, artifact in artifacts.items():
        _require_text(role, "artifact role")
        try:
            artifact_path = Path(artifact)
        except TypeError as error:
            raise NodeResultError(f"invalid artifact path for {role!r}") from error
        entry = _artifact_record(artifact_path, roots)
        if Path(entry["path"]) == target or artifact_path.absolute() == target:
            raise NodeResultError("node result destination cannot also be an artifact")
        inventory[role] = entry
    record = {
        "contract_id": NODE_RESULT_CONTRACT_ID,
        "status": "PASS",
        "node_id": node_id,
        "node_identity": node_identity,
        "build_identity": build_identity,
        "run_id": run_id,
        "artifacts": inventory,
        "data": data,
    }
    _validate_schema(record)
    try:
        contents = json.dumps(record, indent=2, sort_keys=True, ensure_ascii=False, allow_nan=False)
        published = _decode(contents)
        encoded = (contents + "\n").encode("utf-8")
    except (TypeError, ValueError, RecursionError) as error:
        raise NodeResultError(f"node result data is not valid JSON: {error}") from error
    target.parent.mkdir(parents=True, exist_ok=True)
    staging: Path | None = None
    try:
        with tempfile.NamedTemporaryFile(
            mode="wb", dir=target.parent, prefix=f".{target.name}.", suffix=".tmp", delete=False
        ) as stream:
            staging = Path(stream.name)
            stream.write(encoded)
            stream.flush()
            os.fsync(stream.fileno())
        os.replace(staging, target)
    finally:
        if staging is not None:
            try:
                staging.unlink(missing_ok=True)
            except OSError:
                pass
    return published


def read_node_result(
    path: Path,
    *,
    node_id: str,
    node_identity: str | None,
    build_identity: str,
    allowed_roots: Sequence[Path],
) -> dict:
    """Read a reusable PASS record, validating all explicit files and identities."""
    _require_text(node_id, "expected node_id")
    _require_text(build_identity, "expected build_identity")
    if node_identity is not None:
        _require_text(node_identity, "expected node_identity")
    roots = _allowed_roots(allowed_roots)
    try:
        contents = Path(path).read_text(encoding="utf-8")
    except (OSError, UnicodeError, ValueError) as error:
        raise NodeResultError(f"cannot read node result {path}: {error}") from error
    record = _decode(contents)
    _validate_schema(record)
    for field, expected in (
        ("node_id", node_id),
        ("node_identity", node_identity),
        ("build_identity", build_identity),
    ):
        if expected is not None and record[field] != expected:
            raise NodeResultError(f"node result {field} does not match the requested identity")
    for role, artifact in record["artifacts"].items():
        actual = _artifact_record(Path(artifact["path"]), roots)
        if artifact["size"] != actual["size"]:
            raise NodeResultError(f"artifact {role!r} size does not match the node result")
        if artifact["sha256"] != actual["sha256"]:
            raise NodeResultError(f"artifact {role!r} sha256 does not match the node result")
    return record

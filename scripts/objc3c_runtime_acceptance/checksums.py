"""Checksum and replay-key helpers for runtime acceptance."""

from __future__ import annotations

import hashlib
import re
from contextlib import contextmanager
from contextvars import ContextVar
from pathlib import Path

_INPUT_HASHES = ContextVar("runtime_acceptance_input_hashes", default=None)


def _fresh_hash(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _stamp(path: Path) -> tuple:
    info = path.stat()
    return (path.resolve(), info.st_dev, info.st_ino, info.st_size,
            info.st_mtime_ns, info.st_ctime_ns, info.st_mode)


def file_sha256_hex(path: Path) -> str:
    inputs = _INPUT_HASHES.get()
    key = path.absolute()
    if inputs is None or key not in inputs["files"]:
        return _fresh_hash(path)
    entry = inputs["files"][key]
    current = _stamp(path)
    if current != entry["stamp"]:
        # A build wrapper may republish identical bytes. Rehash that case;
        # metadata is never sufficient to accept different compiler bytes.
        digest = _fresh_hash(path)
        inputs["full_reads"] += 1
        if digest != entry["sha256"] or current != _stamp(path):
            raise RuntimeError(f"runtime acceptance input changed: {path}")
        entry["stamp"] = current
    inputs["reused_reads"] += 1
    return entry["sha256"]


@contextmanager
def verified_input_hashes(paths):
    """Reuse only admitted compiler/runtime hashes, then rehash before PASS.

    Source files, generated artifacts and negative/reuse checks remain fresh.
    The context ends before any successful run report is published.
    """
    if _INPUT_HASHES.get() is not None:
        raise RuntimeError("nested runtime acceptance input snapshots are unsupported")
    inputs = {"files": {}, "full_reads": 0, "reused_reads": 0}
    for raw in paths:
        path = Path(raw).absolute()
        stamp = _stamp(path)
        digest = _fresh_hash(path)
        if stamp != _stamp(path):
            raise RuntimeError(f"runtime acceptance input changed while hashing: {path}")
        inputs["files"][path] = {"stamp": stamp, "sha256": digest}
        inputs["full_reads"] += 1
    token = _INPUT_HASHES.set(inputs)
    try:
        yield inputs
    finally:
        _INPUT_HASHES.reset(token)
        for path, entry in inputs["files"].items():
            before = _stamp(path)
            digest = _fresh_hash(path)
            inputs["full_reads"] += 1
            if digest != entry["sha256"] or before != _stamp(path):
                raise RuntimeError(f"runtime acceptance input changed before publication: {path}")


def optional_file_sha256_hex(path: Path | None) -> str:
    if path is None or not path.is_file():
        return ""
    return file_sha256_hex(path)


def sha256_text_hex(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def replay_key_counter(replay_key: str, counter_name: str) -> int:
    if replay_key == "" or counter_name == "":
        return 0
    match = re.search(re.escape(counter_name) + r"=([0-9]+)", replay_key)
    if match is None:
        return 0
    return int(match.group(1))

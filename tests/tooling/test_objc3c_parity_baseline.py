from __future__ import annotations

import hashlib
import json
from pathlib import Path

FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "native" / "parity_baseline"
SNAPSHOT_PATH = FIXTURE_ROOT / "baseline_snapshot.json"
EXPECTED_ROOT_KEYS = [
    "snapshot_version",
    "suite",
    "entries",
    "entry_count",
    "corpus_sha256",
]
EXPECTED_ENTRY_KEYS = ["path", "bytes", "line_count", "sha256"]


def canonical_json_text(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def compute_entry_digest_line(entry: dict[str, object]) -> str:
    return (
        f"{entry['path']}|{entry['sha256']}|{entry['bytes']}|{entry['line_count']}"
    )


def hash_text(value: str) -> str:
    return hashlib.sha256(value.encode("utf-8")).hexdigest()


def calculate_entry(relative_path: str) -> dict[str, object]:
    path = FIXTURE_ROOT / relative_path
    data = path.read_bytes()
    text = path.read_text(encoding="utf-8")
    return {
        "path": relative_path,
        "bytes": len(data),
        "line_count": text.count("\n"),
        "sha256": hashlib.sha256(data).hexdigest(),
    }


def load_snapshot() -> tuple[dict[str, object], str]:
    raw = SNAPSHOT_PATH.read_text(encoding="utf-8")
    payload = json.loads(raw)
    assert isinstance(payload, dict)
    return payload, raw


def test_baseline_snapshot_is_canonical_and_ordered() -> None:
    snapshot, raw = load_snapshot()

    assert list(snapshot.keys()) == EXPECTED_ROOT_KEYS
    assert raw == canonical_json_text(snapshot)
    assert snapshot["snapshot_version"] == 1
    assert snapshot["suite"] == "objc3c-parity-baseline"

    entries = snapshot["entries"]
    assert isinstance(entries, list)
    assert snapshot["entry_count"] == len(entries)
    assert len(entries) >= 2

    observed_paths: list[str] = []
    for entry in entries:
        assert isinstance(entry, dict)
        assert list(entry.keys()) == EXPECTED_ENTRY_KEYS
        observed_paths.append(str(entry["path"]))

    assert observed_paths == sorted(observed_paths)


def test_baseline_snapshot_locks_corpus_hashes_and_file_set() -> None:
    snapshot, _ = load_snapshot()
    entries = snapshot["entries"]
    assert isinstance(entries, list)

    snapshot_paths = [str(item["path"]) for item in entries if isinstance(item, dict)]
    discovered_paths = sorted(
        path.relative_to(FIXTURE_ROOT).as_posix()
        for path in FIXTURE_ROOT.rglob("*")
        if path.is_file() and path.suffix in {".objc3", ".m"}
    )
    assert discovered_paths == snapshot_paths

    recalculated_entries = [calculate_entry(relative_path) for relative_path in snapshot_paths]
    assert recalculated_entries == entries

    digest_lines = [compute_entry_digest_line(item) for item in recalculated_entries]
    corpus_payload = "\n".join(digest_lines) + "\n"
    assert snapshot["corpus_sha256"] == hash_text(corpus_payload)

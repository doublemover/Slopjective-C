from __future__ import annotations

import hashlib
import json
from pathlib import Path
from typing import Any


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def read_json(path: Path) -> Any:
    return json.loads(read(path))


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def artifact_hashes(out_dir: Path, names: list[str]) -> dict[str, str]:
    return {
        name: sha256(out_dir / name)
        for name in names
        if (out_dir / name).is_file()
    }

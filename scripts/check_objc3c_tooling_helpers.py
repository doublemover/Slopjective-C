#!/usr/bin/env python3
"""Fast checks for shared Python tooling helper semantics."""

from __future__ import annotations

import json
import tempfile
from pathlib import Path
from typing import Sequence
from objc3c_tooling.json_io import (
    canonical_json,
    load_json_array,
    load_json_object,
    load_optional_json_object,
    require_json_object,
    render_json,
    write_json_file,
    write_text_file,
)
from objc3c_tooling.paths import (
    ROOT,
    display_path,
    normalize_rel_path,
    repo_rel,
    resolve_repo_path,
    resolve_repo_path_inside,
)


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def main(argv: Sequence[str] | None = None) -> int:
    _ = argv
    expect(display_path(ROOT / "scripts") == "scripts", "display_path should prefer repo-relative output")
    expect(repo_rel(ROOT / "scripts") == "scripts", "repo_rel should render repo-relative output")
    expect(normalize_rel_path("a\\b/c") == "a/b/c", "normalize_rel_path should use forward slashes")
    expect(resolve_repo_path("scripts") == ROOT / "scripts", "resolve_repo_path should anchor relative paths")
    expect(
        resolve_repo_path_inside(ROOT / "scripts") == (ROOT / "scripts").resolve(),
        "resolve_repo_path_inside should accept repo-contained absolute paths",
    )

    with tempfile.TemporaryDirectory() as raw_tmp:
        tmp_dir = Path(raw_tmp)
        object_path = tmp_dir / "nested" / "object.json"
        array_path = tmp_dir / "array.json"
        text_path = tmp_dir / "text.txt"
        write_json_file(object_path, {"z": 1, "a": "\u00e9"}, sort_keys=True)
        write_json_file(array_path, [1, 2, 3])
        write_text_file(text_path, "hello\n")

        expect(
            object_path.read_text(encoding="utf-8") == '{\n  "a": "\\u00e9",\n  "z": 1\n}\n',
            "write_json_file should preserve JSON defaults, sort option, and trailing newline",
        )
        expect(load_json_object(object_path) == {"a": "\u00e9", "z": 1}, "load_json_object should load objects")
        expect(require_json_object(object_path) == {"a": "\u00e9", "z": 1}, "require_json_object should load objects")
        expect(
            load_optional_json_object(object_path) == {"a": "\u00e9", "z": 1},
            "load_optional_json_object should load present objects",
        )
        expect(
            load_optional_json_object(tmp_dir / "missing.json") is None,
            "load_optional_json_object should return None for missing paths",
        )
        expect(load_json_array(array_path) == [1, 2, 3], "load_json_array should load arrays")
        expect(text_path.read_text(encoding="utf-8") == "hello\n", "write_text_file should use UTF-8 text")
        expect(canonical_json({"b": 2, "a": 1}) == json.dumps({"b": 2, "a": 1}, indent=2) + "\n", "canonical_json default drifted")
        expect(render_json({"b": 2, "a": 1}, sort_keys=True) == '{\n  "a": 1,\n  "b": 2\n}\n', "render_json sort output drifted")

        try:
            require_json_object(tmp_dir / "missing.json")
        except RuntimeError as exc:
            expect("expected JSON artifact was not published" in str(exc), "require_json_object missing diagnostic drifted")
        else:
            raise RuntimeError("require_json_object should reject missing paths")

        try:
            load_json_object(array_path)
        except RuntimeError as exc:
            expect("expected JSON object" in str(exc), "load_json_object diagnostic drifted")
        else:
            raise RuntimeError("load_json_object should reject arrays")

        outside = tmp_dir / "outside.txt"
        try:
            repo_rel(outside)
        except ValueError as exc:
            expect("outside repository root" in str(exc), "repo_rel outside-root diagnostic drifted")
        else:
            raise RuntimeError("repo_rel should reject outside-root paths")

        try:
            resolve_repo_path_inside(outside)
        except ValueError as exc:
            expect("under repository root" in str(exc), "resolve_repo_path_inside diagnostic drifted")
        else:
            raise RuntimeError("resolve_repo_path_inside should reject outside-root paths")

    print("objc3c-tooling-helpers: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

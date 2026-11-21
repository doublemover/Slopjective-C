"""Linux admission binds the stable API and the separate sealed build stamp."""
from pathlib import Path

import pytest

from scripts.objc3c_native_validation.identity import NativeToolchainError, _Inputs
from scripts.objc3c_native_validation.identity_linux import _add_compiler_identity_headers


def headers(tmp_path: Path) -> tuple[Path, Path, dict]:
    root = tmp_path / "include/clang/Basic"
    root.mkdir(parents=True)
    foundation = root / "ObjC3Foundation.h"
    stamp = root / "ObjC3BuildIdentity.h"
    seal = {"upstream_commit": "a" * 40, "patch_digest": "b" * 64}
    foundation.write_text(seal["upstream_commit"])
    stamp.write_text(seal["patch_digest"] + "\n// generation 0\n")
    return foundation, stamp, seal


def test_separate_stamp_participates_in_snapshot(tmp_path: Path) -> None:
    foundation, stamp, seal = headers(tmp_path)
    before = _Inputs()
    _add_compiler_identity_headers(before, tmp_path, seal)
    stamp.write_text(stamp.read_text().replace("generation 0", "generation 1"))
    after = _Inputs()
    _add_compiler_identity_headers(after, tmp_path, seal)
    assert before.files[str(foundation.resolve())] == after.files[str(foundation.resolve())]
    assert before.files[str(stamp.resolve())] != after.files[str(stamp.resolve())]


def test_legacy_combined_header_cannot_replace_stamp(tmp_path: Path) -> None:
    foundation, stamp, seal = headers(tmp_path)
    foundation.write_text(seal["upstream_commit"] + seal["patch_digest"])
    stamp.unlink()
    with pytest.raises(NativeToolchainError, match="identity stamp"):
        _add_compiler_identity_headers(_Inputs(), tmp_path, seal)


def test_stale_stamp_is_rejected(tmp_path: Path) -> None:
    _, stamp, seal = headers(tmp_path)
    stamp.write_text("c" * 64)
    with pytest.raises(NativeToolchainError, match="identity stamp"):
        _add_compiler_identity_headers(_Inputs(), tmp_path, seal)

import json
import os
from pathlib import Path

import pytest

from scripts.objc3c_toolchain.package import MANIFEST, _digest, inventory, verify_sdk


def sealed(tmp_path):
    prefix = tmp_path / "sdk"
    (prefix / "bin").mkdir(parents=True)
    (prefix / "bin/clang").write_bytes(b"compiler")
    (prefix / "bin/clang").chmod(0o755)
    (prefix / "bin/clang++").symlink_to("clang")
    data = {"schema": "objc3.sdk.v1", "files": inventory(prefix)}
    data["seal_sha256"] = _digest(data)
    (prefix / MANIFEST).write_text(json.dumps(data))
    return prefix, data["seal_sha256"]


def test_relocated_sdk_verifies(tmp_path):
    prefix, seal = sealed(tmp_path)
    destination = tmp_path / "relocated"
    prefix.rename(destination)
    assert verify_sdk(destination, seal)["seal_sha256"] == seal


@pytest.mark.parametrize("damage", ["bytes", "mode", "symlink", "extra", "manifest", "expected"])
def test_sdk_seal_rejects_changes(tmp_path, damage):
    prefix, seal = sealed(tmp_path)
    compiler = prefix / "bin/clang"
    if damage == "bytes":
        stamp = compiler.stat()
        compiler.write_bytes(b"tampered")
        os.utime(compiler, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
    elif damage == "mode":
        compiler.chmod(0o644)
    elif damage == "symlink":
        (prefix / "bin/clang++").unlink()
        (prefix / "bin/clang++").symlink_to("../bin/clang")
    elif damage == "extra":
        (prefix / "bin/unrecorded").write_text("extra")
    elif damage == "manifest":
        data = json.loads((prefix / MANIFEST).read_text())
        data["files"] = {}
        (prefix / MANIFEST).write_text(json.dumps(data))
    else:
        seal = "0" * 64
    with pytest.raises(ValueError):
        verify_sdk(prefix, seal)


def test_seal_rejects_external_symbolic_link(tmp_path):
    prefix, _ = sealed(tmp_path)
    outside = tmp_path / "external"
    outside.write_text("external dependency")
    (prefix / "bin/external").symlink_to(outside)
    with pytest.raises(ValueError, match="escapes"):
        inventory(prefix)


def test_seal_rejects_absolute_internal_link_that_cannot_relocate(tmp_path):
    prefix, _ = sealed(tmp_path)
    (prefix / "bin/absolute").symlink_to(prefix / "bin/clang")
    with pytest.raises(ValueError, match="escapes"):
        inventory(prefix)

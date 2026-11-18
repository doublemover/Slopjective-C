#!/usr/bin/env python3
"""Exercise native diagnostic JSON and separately emitted compiler artifacts."""
import argparse
import hashlib
import json
import pathlib
import subprocess
import tempfile


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--driver", required=True)
    args = parser.parse_args()
    driver = str(pathlib.Path(args.driver).resolve(strict=True))
    with tempfile.TemporaryDirectory(prefix="objc3-diagnostic-cli-") as temp:
        root = pathlib.Path(temp)
        source = root / "source.c"
        original = "/* π */ int answer(void){return 42}".encode()
        source.write_bytes(original)
        broken = subprocess.run([driver, str(source), "--diagnostics-json", "--", "-std=c17"], capture_output=True, timeout=30)
        require(broken.returncode == 1, broken.stderr.decode(errors="replace"))
        data = json.loads(broken.stdout)
        require(data["contract_id"] == "objc3c.native-diagnostics.v1" and data["success"] is False, "invalid diagnostic record")
        edits = [edit for diagnostic in data["diagnostics"] for edit in diagnostic["edits"]]
        require(edits, "compiler dropped repair edits")
        updated = original
        for edit in sorted(edits, key=lambda e: e["begin_offset"], reverse=True):
            require(bytes.fromhex(edit["file_bytes_hex"]) == bytes(source), "incorrect physical filename")
            require(edit["source_sha256"] == hashlib.sha256(original).hexdigest(), "incorrect source guard")
            updated = updated[:edit["begin_offset"]] + bytes.fromhex(edit["replacement_bytes_hex"]) + updated[edit["end_offset"]:]
        source.write_bytes(updated)
        object_path = root / "answer.o"
        good = subprocess.run([driver, str(source), "--diagnostics-json", "--emit-object", str(object_path), "--", "-std=c17"], capture_output=True, timeout=30)
        require(good.returncode == 0 and json.loads(good.stdout)["success"] is True, good.stderr.decode(errors="replace"))
        require(object_path.stat().st_size > 0, "object output is empty")
        conflict = subprocess.run([driver, str(source), "--diagnostics-json", "--emit-object", "-"], capture_output=True, timeout=30)
        require(conflict.returncode == 2 and not conflict.stdout, "mixed artifact/JSON stdout was admitted")
        require(not (root / "-").exists(), "invalid output request created an artifact")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

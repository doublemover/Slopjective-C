"""Exercise native COFF normalization without weakening byte parity checks."""

from __future__ import annotations

import os
from pathlib import Path
import shutil
import struct
import subprocess

import pytest


ROOT = Path(__file__).resolve().parents[2]


@pytest.fixture(scope="module")
def normalizer(tmp_path_factory):
    directory = tmp_path_factory.mktemp("coff-normalizer")
    compiler = shutil.which("cl" if os.name == "nt" else "c++")
    if compiler is None:
        pytest.skip("requires a native C++ compiler (a VS developer shell on Windows)")
    executable = directory / ("normalize.exe" if os.name == "nt" else "normalize")
    sources = [ROOT / "tests/tooling/runtime/coff_debug_identity_probe.cpp",
               ROOT / "native/objc3c/src/io/objc3_object_format.cpp"]
    if os.name == "nt":
        command = [compiler, "/nologo", "/std:c++20", "/EHsc", "/MD",
                   "/I" + str(ROOT / "native/objc3c/src"),
                   *map(str, sources), "/Fe" + str(executable)]
    else:
        command = [compiler, "-std=c++20", "-I", str(ROOT / "native/objc3c/src"),
                   *map(str, sources), "-o", str(executable)]
    subprocess.run(command, cwd=directory, check=True, capture_output=True, text=True)
    return executable


def record(kind: int, payload: bytes) -> bytes:
    data = bytearray(struct.pack("<HH", 0, kind) + payload)
    data.extend(b"\0" * (-len(data) % 4))
    struct.pack_into("<H", data, 0, len(data) - 2)
    return bytes(data)


def coff(path: str) -> bytes:
    name = record(0x1101, struct.pack("<I", 0) + path.encode("utf-8") + b"\0")
    environment = record(0x113D, b"\0key\0unrelated.obj\0\0")
    symbols = name + environment
    debug = struct.pack("<III", 4, 0xF1, len(symbols)) + symbols
    raw = 100
    data_offset = raw + len(debug)
    relocation_offset = data_offset + 4
    symbol_offset = relocation_offset + 10
    header = struct.pack("<HHIIIHH", 0x8664, 2, 12345, symbol_offset, 3, 0, 0)
    debug_header = struct.pack("<8sIIIIIIHHI", b".debug$S", 0, 0, len(debug), raw,
                               0, 0, 0, 0, 0x42300040)
    data_header = struct.pack("<8sIIIIIIHHI", b".data", 0, 0, 4, data_offset,
                              relocation_offset, 0, 1, 0, 0xC0300040)
    debug_symbol = struct.pack("<8sIhHBB", b".debug$S", 0, 1, 0, 3, 1)
    auxiliary = struct.pack("<IHHIhB3s", len(debug), 0, 0, 0x12345678, 0, 0, b"\0" * 3)
    data_symbol = struct.pack("<8sIhHBB", b"value", 0, 2, 0, 2, 0)
    return (header + debug_header + data_header + debug + b"DATA" +
            struct.pack("<IIH", 0, 2, 3) + debug_symbol + auxiliary + data_symbol +
            struct.pack("<I", 4))


def normalize(normalizer: Path, path: Path) -> bytes:
    subprocess.run([str(normalizer), str(path)], check=True)
    return path.read_bytes()


def test_different_path_lengths_have_identical_valid_offsets_and_preserved_payloads(normalizer, tmp_path):
    outputs = []
    for index, name in enumerate(("a.obj", "C:/cli/module.obj", "C:/much/longer/library/output/module.obj")):
        path = tmp_path / f"case-{index}.obj"
        path.write_bytes(coff(name))
        output = normalize(normalizer, path)
        assert normalize(normalizer, path) == output
        outputs.append(output)
    assert outputs[0] == outputs[1] == outputs[2]
    output = outputs[0]
    assert struct.unpack_from("<I", output, 4)[0] == 0
    size, raw = struct.unpack_from("<II", output, 20 + 16)
    data, relocations = struct.unpack_from("<II", output, 60 + 20)
    assert data == raw + size and output[data:data + 4] == b"DATA"
    assert relocations == data + 4
    assert struct.unpack_from("<IIH", output, relocations) == (0, 2, 3)
    symbols = struct.unpack_from("<I", output, 8)[0]
    assert symbols == relocations + 10
    assert struct.unpack_from("<I", output, symbols + 18)[0] == size
    assert struct.unpack_from("<I", output, symbols + 26)[0] == 0
    assert b"objc3c-deterministic-object.obj\0" in output
    assert b"key\0unrelated.obj\0" in output
    assert output[-4:] == struct.pack("<I", 4)


@pytest.mark.parametrize("fault", ["record-length", "missing-nul", "relocations", "symbol-value", "auxiliary-count"])
def test_malformed_or_unowned_debug_layout_is_not_rewritten(normalizer, tmp_path, fault):
    original = bytearray(coff("C:/path/module.obj"))
    struct.pack_into("<I", original, 4, 0)
    symbols = struct.unpack_from("<I", original, 8)[0]
    if fault == "record-length":
        struct.pack_into("<H", original, 112, 0xFFFF)
    elif fault == "missing-nul":
        length = struct.unpack_from("<H", original, 112)[0] + 2
        original[120:112 + length] = b"x" * (length - 8)
    elif fault == "relocations":
        struct.pack_into("<H", original, 20 + 32, 1)
    elif fault == "symbol-value":
        struct.pack_into("<I", original, symbols + 8, 12)
    else:
        original[symbols + 17] = 255
    path = tmp_path / "invalid.obj"
    path.write_bytes(original)
    assert normalize(normalizer, path) == original

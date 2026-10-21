import hashlib
import os

import pytest

from scripts.objc3c_runtime_acceptance.checksums import file_sha256_hex, verified_input_hashes


def test_run_reuses_admitted_hashes_and_rehashes_before_publication(tmp_path):
    compiler = tmp_path / "compiler"
    compiler.write_bytes(b"compiler bytes")
    with verified_input_hashes([compiler]) as stats:
        for _ in range(12):
            assert file_sha256_hex(compiler) == hashlib.sha256(b"compiler bytes").hexdigest()
    assert stats["full_reads"] == 2
    assert stats["reused_reads"] == 12


@pytest.mark.parametrize("during_read", [True, False])
def test_same_size_same_mtime_mutation_rejected_before_pass(tmp_path, during_read):
    compiler = tmp_path / "compiler"
    compiler.write_bytes(b"original")
    stamp = compiler.stat()
    with pytest.raises(RuntimeError, match="input changed"):
        with verified_input_hashes([compiler]):
            compiler.write_bytes(b"tampered")
            os.utime(compiler, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
            if during_read:
                file_sha256_hex(compiler)
    assert file_sha256_hex(compiler) == hashlib.sha256(b"tampered").hexdigest()


def test_wrapper_can_republish_identical_bytes(tmp_path):
    compiler = tmp_path / "compiler"
    compiler.write_bytes(b"same")
    with verified_input_hashes([compiler]) as stats:
        replacement = tmp_path / "replacement"
        replacement.write_bytes(b"same")
        replacement.replace(compiler)
        assert file_sha256_hex(compiler) == hashlib.sha256(b"same").hexdigest()
    assert stats["full_reads"] == 3


def test_sources_and_artifacts_never_use_compiler_hash_cache(tmp_path):
    compiler, source = tmp_path / "compiler", tmp_path / "source"
    compiler.write_bytes(b"compiler")
    source.write_bytes(b"before")
    with verified_input_hashes([compiler]):
        first = file_sha256_hex(source)
        source.write_bytes(b"after!")
        assert file_sha256_hex(source) != first


def test_failed_run_clears_snapshot(tmp_path):
    compiler = tmp_path / "compiler"
    compiler.write_bytes(b"before")
    with pytest.raises(ValueError):
        with verified_input_hashes([compiler]):
            raise ValueError("fixture failed")
    compiler.write_bytes(b"after!")
    assert file_sha256_hex(compiler) == hashlib.sha256(b"after!").hexdigest()

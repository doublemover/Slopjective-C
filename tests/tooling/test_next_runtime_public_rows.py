from scripts import check_objc3c_next_runtime_public_rows as checker


def test_next_runtime_public_rows_cannot_claim_removed_prototype_sources(capsys) -> None:
    assert checker.main() == 1

    output = capsys.readouterr().out
    assert "objc3c-next-runtime-public-rows: FAIL" in output
    assert "runtime.public-api.reflection references missing or tmp-backed evidence path" in output
    assert "objc3_runtime_reflection.h" in output

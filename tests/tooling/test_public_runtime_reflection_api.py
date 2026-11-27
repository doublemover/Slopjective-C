from scripts import check_objc3c_public_runtime_reflection_api as checker


def test_retired_runtime_reflection_api_requires_its_source(monkeypatch) -> None:
    def unexpected_probe(failures):
        raise AssertionError(f"reflection probe executed without source: {failures}")

    monkeypatch.setattr(checker, "_execute_public_reflection_probe", unexpected_probe)

    report = checker.validate_public_runtime_reflection_api()

    assert report["status"] == "FAIL"
    assert any(
        "source unavailable" in failure and failure.endswith("objc3_runtime_reflection.h")
        for failure in report["failures"]
    )

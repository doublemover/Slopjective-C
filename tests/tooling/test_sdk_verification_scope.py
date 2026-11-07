"""Frozen qualification batches audit contents without rehashing each operation."""
from concurrent.futures import ThreadPoolExecutor
from contextvars import copy_context

import pytest

from scripts.objc3c_toolchain import binary_packages, sdk_example


@pytest.fixture
def sdk(tmp_path, monkeypatch):
    root = tmp_path / "sdk"
    root.mkdir()
    (root / "objc3-sdk.json").write_text('{"seal_sha256":"fixture"}')
    (root / "payload").write_text("original")
    audits = []

    def audit(path, env=None):
        audits.append(path)
        if (path / "payload").read_text() != "original":
            raise ValueError("SDK contents changed")

    monkeypatch.setattr(sdk_example, "_audit_sdk", audit)
    return root, audits


def test_standalone_operations_always_audit(sdk):
    root, audits = sdk
    binary_packages.sdk_identity(root)
    binary_packages.sdk_identity(root)
    assert audits == [root, root]


def test_nested_operations_share_only_their_qualification_scope(sdk):
    root, audits = sdk
    with sdk_example.sdk_verification(root):
        binary_packages.sdk_identity(root)
        with sdk_example.sdk_verification(root):
            binary_packages.sdk_identity(root)
        sdk_example.verify_installed_sdk(root)
        assert audits == [root]
    assert audits == [root, root]
    binary_packages.sdk_identity(root)
    assert audits == [root, root, root]


def test_content_changes_cannot_escape_the_final_audit(sdk):
    root, audits = sdk
    with pytest.raises(ValueError, match="SDK contents changed"):
        with sdk_example.sdk_verification(root):
            (root / "payload").write_text("changed")
            sdk_example.verify_installed_sdk(root)
    assert audits == [root, root]
    (root / "payload").write_text("original")
    sdk_example.verify_installed_sdk(root)
    assert audits == [root, root, root]


def test_manifest_changes_are_rejected_during_the_scope(sdk):
    root, audits = sdk
    with pytest.raises(ValueError, match="SDK manifest changed"):
        with sdk_example.sdk_verification(root):
            (root / "objc3-sdk.json").write_text('{"seal_sha256":"changed"}')
            sdk_example.verify_installed_sdk(root)
    assert audits == [root]


def test_failure_still_audits_and_resets_the_scope(sdk):
    root, audits = sdk
    with pytest.raises(RuntimeError, match="consumer failed"):
        with sdk_example.sdk_verification(root):
            raise RuntimeError("consumer failed")
    sdk_example.verify_installed_sdk(root)
    assert audits == [root, root, root]


def test_parallel_workers_do_not_inherit_another_workers_audit(sdk):
    root, audits = sdk
    with sdk_example.sdk_verification(root):
        with ThreadPoolExecutor(max_workers=1) as executor:
            executor.submit(sdk_example.verify_installed_sdk, root).result()
        assert audits == [root, root]
        sdk_example.verify_installed_sdk(root)
        assert audits == [root, root]
    assert audits == [root, root, root]


def test_explicit_qualification_workers_share_the_enclosing_audit(sdk):
    root, audits = sdk
    with sdk_example.sdk_verification(root):
        with ThreadPoolExecutor(max_workers=2) as executor:
            futures = [executor.submit(copy_context().run,
                                       sdk_example.verify_installed_sdk, root)
                       for _ in range(8)]
            for future in futures:
                future.result()
        assert audits == [root]
    assert audits == [root, root]

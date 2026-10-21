from __future__ import annotations

import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPTS = REPO_ROOT / "scripts"
sys.path.insert(0, str(SCRIPTS))
sys.path.insert(1, str(REPO_ROOT / "scripts"))
sys.path.insert(2, str(REPO_ROOT))

from objc3c_package_manager import producer_report  # noqa: E402
import check_objc3c_package_install_distribution_credibility as clean_install  # noqa: E402
from objc3c_package_manager import install_distribution as installation  # noqa: E402
import build_objc3c_package_mirror as producer_cli  # noqa: E402


def _producer_fixture(root: Path, *, node: str, build: str) -> Path:
    paths = (
        producer_report.LOCK, producer_report.MIRROR, producer_report.REGISTRY,
        producer_report.PUBLICATION, producer_report.RESTORE, producer_report.SUMMARY,
    )
    for raw in paths:
        path = root / raw
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text("{}\n", encoding="utf-8")
    cache = root / "tmp/artifacts/package-ecosystem/mirrors/cache/fixture.json"
    cache.parent.mkdir(parents=True, exist_ok=True)
    cache.write_text("{}\n", encoding="utf-8")
    source = root / "producer-input.json"
    source.write_text("{}\n", encoding="utf-8")
    (root / producer_report.LOCK).write_text(json.dumps({"packages": [], "digest_inputs": ["producer-input.json"]}), encoding="utf-8")
    (root / producer_report.SUMMARY).write_text(
        json.dumps({"status": "PASS", "cache_paths": ["tmp/artifacts/package-ecosystem/mirrors/cache/fixture.json"]}),
        encoding="utf-8",
    )
    path = root / "tmp/artifacts/cross-lane-e2e/runs/run-a/package-producer/producer-report.json"
    producer_report.write_producer_report(root, path, run_id="run-a", node_identity=node, build_identity=build)
    return path


class PackageCleanInstallReuseTests(unittest.TestCase):
    def test_invalid_producer_request_fails_before_materialization(self) -> None:
        with tempfile.TemporaryDirectory(dir=REPO_ROOT / "tmp") as temporary:
            report = Path(temporary) / "producer-report.json"
            arguments = ["--producer-report", str(report), "--run-id", "safe-run",
                         "--producer-node-identity", "producer-node", "--build-identity", "build-a"]
            with patch.object(producer_cli, "ensure_lock", side_effect=AssertionError("producer materialized")):
                with self.assertRaises(SystemExit):
                    producer_cli.main(arguments[:-2])
                report.write_text("existing evidence\n")
                with self.assertRaises(SystemExit):
                    producer_cli.main(arguments)
                self.assertEqual(report.read_text(), "existing evidence\n")

    def test_actual_materialization_owns_only_isolated_install_files(self) -> None:
        with tempfile.TemporaryDirectory(dir=REPO_ROOT / "tmp") as temporary:
            root = Path(temporary)
            output_root = root / "tmp/runs/isolated/package-consumer"
            source = root / "fixture-source.json"
            source.write_text("{}\n")
            manifest = root / "fixture-manifest.json"
            manifest.write_text('{"manifest_digest":"fixture-digest"}\n')
            input_paths = [root / name for name in ("mirror.json", "registry.json", "publication.json", "restore.json")]
            for path in input_paths:
                path.write_text("{}\n")
            marker = root / installation.INSTALL_ROOT_REL / "shared.txt"
            marker.parent.mkdir(parents=True)
            marker.write_text("must survive\n")
            lock = {"packages": [{"package_id": "fixture:one", "source": source.name,
                "source_digest": "sha256:" + "a" * 64, "package_manifest": {
                    "path": manifest.name, "digest": "sha256:" + "b" * 64}, "trust": {"signature": "fixture-signature"}}],
                "package_manager": {"language_version": "3.0", "abi_identity": "objc3-abi-2025Q4"},
                "resolution_plan": {"install_order": ["fixture:one"]}}
            with patch.object(installation, "repo_rel", lambda path: Path(path).relative_to(root).as_posix()), \
                 patch.object(installation, "publish_platform_install_receipt", side_effect=AssertionError("shared receipt was published")):
                result = installation.materialize_clean_distribution_install(root=root, contract={},
                    lock=lock, mirror={}, registry={}, publication={}, restore_receipt={},
                    mirror_path=input_paths[0], registry_path=input_paths[1],
                    publication_path=input_paths[2], restore_receipt_path=input_paths[3], output_root=output_root)
                receipt = json.loads((output_root / installation.INSTALL_RECEIPT_REL).read_text())
                self.assertEqual(installation.collect_registered_schema_failures(receipt,
                    installation.INSTALL_RECEIPT_SCHEMA_ID, "isolated install receipt", root=root, output_root=output_root), [])
                update = json.loads((output_root / installation.PACKAGE_UPDATE_RECEIPT_REL).read_text())
                self.assertEqual(installation.collect_package_operation_receipt_failures(root=root,
                    lock=lock, verification=result, receipt_path=output_root / installation.PACKAGE_UPDATE_RECEIPT_REL,
                    operation="update", expected_order=["fixture:one"], output_root=output_root), [])
                self.assertTrue((root / update["receipt_root"]).resolve().is_relative_to(output_root.resolve()))
                receipt["install_root"] = "tmp/elsewhere"
                self.assertTrue(installation.collect_registered_schema_failures(receipt,
                    installation.INSTALL_RECEIPT_SCHEMA_ID, "tampered isolated install receipt", root=root, output_root=output_root))
            self.assertEqual(result["manifest_count"], 1)
            self.assertTrue(marker.is_file())
            self.assertTrue((root / result["installed_packages"][0]["installed_manifest"]).is_file())
            self.assertTrue(all((root / raw).resolve().is_relative_to(output_root.resolve())
                                for raw in result["generated_paths"]))
            self.assertTrue((output_root / installation.INSTALL_RECEIPT_REL).is_file())

    def test_producer_rejects_changed_declared_input_and_partial_inventory(self) -> None:
        with tempfile.TemporaryDirectory(dir=REPO_ROOT / "tmp") as temporary:
            root = Path(temporary)
            report = _producer_fixture(root, node="node-a", build="build-a")
            payload = json.loads(report.read_text())
            payload["artifacts"].pop()
            unsigned = dict(payload)
            unsigned.pop("report_digest")
            payload["report_digest"] = producer_report._digest(unsigned)
            report.write_text(json.dumps(payload))
            with self.assertRaisesRegex(ValueError, "partial or mismatched"):
                producer_report.validate_producer_report(root, report)
            report = _producer_fixture(root, node="node-a", build="build-a")
            (root / "producer-input.json").write_text('{"changed":true}\n')
            with self.assertRaisesRegex(ValueError, "changed since production"):
                producer_report.validate_producer_report(root, report)

    def test_consumer_reuse_requires_unchanged_installed_files(self) -> None:
        with tempfile.TemporaryDirectory(dir=REPO_ROOT / "tmp") as temporary:
            root = Path(temporary)
            producer = _producer_fixture(root, node="producer-node", build="build-a")
            output_root = root / "tmp/runs/run-b/package-consumer"
            output_root.mkdir(parents=True)
            installed = output_root / "installed-manifest.json"
            installed.write_text('{"package":"fixture"}\n')
            verification = output_root / clean_install.INSTALL_VERIFICATION_REL
            verification.parent.mkdir(parents=True)
            verification.write_text(json.dumps({"generated_paths": [installed.relative_to(root).as_posix()]}))
            summary = output_root / "summary.json"
            summary.write_text(json.dumps({"status": "PASS", "from_nothing_probe": {
                "requested": True, "producer_report_digest": json.loads(producer.read_text())["report_digest"],
            }}))
            report = output_root / "consumer-report.json"
            producer_report.write_consumer_report(root, report, run_id="run-b",
                node_identity="consumer-node", build_identity="build-a",
                producer_report_path=producer, output_root=output_root, summary_path=summary)
            arguments = {"expected_node_identity": "consumer-node",
                         "expected_producer_identity": "producer-node", "expected_build_identity": "build-a"}
            self.assertEqual(producer_report.validate_consumer_report(root, report, **arguments)["status"], "PASS")
            installed.write_text('{"package":"tampered"}\n')
            with self.assertRaisesRegex(ValueError, "changed since installation"):
                producer_report.validate_consumer_report(root, report, **arguments)
            installed.unlink()
            with self.assertRaisesRegex(ValueError, "missing or escapes"):
                producer_report.validate_consumer_report(root, report, **arguments)

    def test_consumer_requires_both_expected_identities(self) -> None:
        with self.assertRaisesRegex(RuntimeError, "requires producer node and build"):
            clean_install.main(["--producer-report", "tmp/unused.json", "--output-root", "tmp/package-consumer"])

    def test_graph_clean_install_reuses_verified_producer_without_regeneration(self) -> None:
        with tempfile.TemporaryDirectory(dir=REPO_ROOT / "tmp") as temporary:
            root = Path(temporary) / "repo"
            root.mkdir()
            report = _producer_fixture(root, node="producer-node", build="sealed-build")
            output_root = root / "tmp/artifacts/cross-lane-e2e/runs/run-b/package-consumer"
            contract_path = root / "tests/tooling/fixtures/package_ecosystem/install_distribution_credibility_contract.json"
            contract_path.parent.mkdir(parents=True)
            contract_path.write_text(json.dumps({"required_public_actions": [clean_install.INSTALL_DISTRIBUTION_ACTION]}), encoding="utf-8")
            global_marker = root / clean_install.INSTALL_VALIDATION_ROOT_REL / "must-survive.txt"
            global_marker.parent.mkdir(parents=True)
            global_marker.write_text("shared producer/install output", encoding="utf-8")
            stale_isolated = output_root / clean_install.INSTALL_VALIDATION_ROOT_REL / "stale.txt"
            stale_isolated.parent.mkdir(parents=True)
            stale_isolated.write_text("old run-local install", encoding="utf-8")
            mocks = [
                patch.object(clean_install, "ROOT", root),
                patch.object(clean_install, "CONTRACT_PATH", contract_path),
                patch.object(clean_install, "LOCK_PATH", root / producer_report.LOCK),
                patch.object(clean_install, "MIRROR_PATH", root / producer_report.MIRROR),
                patch.object(clean_install, "REGISTRY_PATH", root / producer_report.REGISTRY),
                patch.object(clean_install, "PUBLICATION_PATH", root / producer_report.PUBLICATION),
                patch.object(clean_install, "RESTORE_RECEIPT_PATH", root / producer_report.RESTORE),
                patch.object(clean_install, "MIRROR_SUMMARY_PATH", root / producer_report.SUMMARY),
                patch.object(clean_install, "require_package_ecosystem_owner_policy", return_value={}),
                patch.object(clean_install, "require_package_ecosystem_blocker_metadata", return_value={}),
                patch.object(clean_install, "public_workflow_action_names", return_value=[clean_install.INSTALL_DISTRIBUTION_ACTION]),
                patch.object(clean_install, "run_package_mirror_generator", side_effect=AssertionError("producer was rerun")),
                patch.object(clean_install, "materialize_clean_distribution_install", return_value={
                    "manifest_count": 0, "dependency_count": 0, "network_policy": "no-network-during-validation",
                    "hosted_registry_support": "unsupported-fail-closed-if-claimed",
                    "offline_restore_support": "local-cache-digest-checked", "generated_paths": [],
                }),
                patch.object(clean_install, "collect_install_distribution_failures", return_value=[]),
            ]
            with mocks[0], mocks[1], mocks[2], mocks[3], mocks[4], mocks[5], mocks[6], mocks[7], \
                 mocks[8], mocks[9], mocks[10], mocks[11], mocks[12], mocks[13]:
                result = clean_install.main([
                    "--producer-report", str(report), "--output-root", str(output_root),
                    "--producer-node-identity", "producer-node", "--build-identity", "sealed-build",
                ])
            summary = json.loads((output_root / "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json").read_text())
            self.assertEqual(result, 0)
            self.assertEqual(summary["status"], "PASS")
            self.assertTrue(summary["from_nothing_probe"]["producer_report_digest"])
            self.assertTrue(summary["install_output_root"].endswith("package-consumer"))
            self.assertTrue(global_marker.is_file())
            self.assertFalse(stale_isolated.exists())

    def test_clean_consumer_rejects_stale_or_wrong_identity_producer(self) -> None:
        with tempfile.TemporaryDirectory(dir=REPO_ROOT / "tmp") as temporary:
            root = Path(temporary) / "repo"
            root.mkdir()
            report_path = _producer_fixture(root, node="node-a", build="build-a")
            with self.assertRaisesRegex(ValueError, "node identity"):
                producer_report.validate_producer_report(root, report_path, expected_node_identity="node-b")
            (root / producer_report.MIRROR).write_text("stale\n", encoding="utf-8")
            with self.assertRaisesRegex(ValueError, "changed since production"):
                producer_report.validate_producer_report(root, report_path)


if __name__ == "__main__":
    unittest.main()

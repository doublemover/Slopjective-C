from __future__ import annotations

import copy
import json
import os
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

from scripts.objc3c_cross_lane_execution import NODE_REPORT_CONTRACT_ID
from scripts.objc3c_native_validation.artifacts import write_node_result
from scripts.objc3c_native_validation.cli import inventory_identity, prepare_reuse, run_validation
from scripts.objc3c_native_validation.worker import apply_worker_environment, result_path


class NativeValidationCoordinatorTests(unittest.TestCase):
    def setUp(self):
        self.scratch = tempfile.TemporaryDirectory()
        self.addCleanup(self.scratch.cleanup)
        self.root = Path(self.scratch.name).resolve()
        self.plan = {"nodes": [
            {"node_id": "native-package:provider", "family_id": "native-package",
             "phase": "provider", "source": "provider.m", "identity_inputs": ["provider.m"], "depends_on": []},
            {"node_id": "native-package:consumer-eager", "family_id": "native-package",
             "phase": "consumer", "source": "consumer.m", "identity_inputs": ["consumer.m"],
             "depends_on": ["native-package:provider"]},
            {"node_id": "carriers:match:clang", "family_id": "carriers", "phase": "fixture",
             "source": "match.m", "identity_inputs": ["match.m"], "depends_on": []},
        ], "selected_node_ids": ["native-package:provider", "native-package:consumer-eager", "carriers:match:clang"],
            "selected_families": ["carriers", "native-package"]}
        self.identities = {node["node_id"]: "b" * 64 for node in self.plan["nodes"]}
        self.toolchain = {"build_identity": "a" * 64, "sdk_build_id": "sdk-test", "patch_digest": "c" * 64}
        for node in self.plan["nodes"]:
            (self.root / node["source"]).write_text("source", encoding="utf-8")

    def make_report(self):
        records = []
        for node in self.plan["nodes"]:
            node_id = node["node_id"]
            path = result_path(self.root / "old", node_id)
            path.parent.mkdir(parents=True)
            artifact = path.parent / "product.o"
            artifact.write_bytes(b"verified native product")
            write_node_result(path, node_id=node_id, node_identity=self.identities[node_id],
                              build_identity=self.toolchain["build_identity"], run_id="old",
                              artifacts={"object": artifact}, data={}, allowed_roots=[self.root])
            records.append({"node_id": node_id, "status": "completed", "result_path": str(path),
                            "node_identity": self.identities[node_id],
                            "build_identity": self.toolchain["build_identity"]})
        return {"contract_id": NODE_REPORT_CONTRACT_ID, "status": "PASS", "partial_report": False,
                "native_validation_schema": 1, "validation_complete": True,
                "selection_complete": True, "run_id": "old", "node_records": records,
                "node_inventory": [node["node_id"] for node in self.plan["nodes"]],
                "build_identity": self.toolchain["build_identity"], "inventory_identity": inventory_identity(self.plan)}

    def test_corrupt_provider_reruns_dependents_preserves_independent_reuse_and_old_report(self):
        report = self.make_report()
        prior = copy.deepcopy(report)
        provider = result_path(self.root / "old", "native-package:provider")
        (provider.parent / "product.o").write_bytes(b"corrupted")
        prepared, invalid = prepare_reuse(report, repo=self.root, plan=self.plan,
                                          identities=self.identities, toolchain=self.toolchain,
                                          inventory=inventory_identity(self.plan), run_id="new")
        self.assertEqual(set(invalid), {"native-package:provider", "native-package:consumer-eager"})
        self.assertEqual(report, prior)
        self.assertEqual(prepared["node_records"][2], prior["node_records"][2])
        self.assertNotEqual(prepared["node_records"][0]["node_identity"], self.identities["native-package:provider"])

    def test_incomplete_report_cannot_supply_reused_nodes(self):
        report = self.make_report()
        report["partial_report"] = True
        with self.assertRaisesRegex(ValueError, "partial"):
            prepare_reuse(report, repo=self.root, plan=self.plan, identities=self.identities,
                          toolchain=self.toolchain, inventory=inventory_identity(self.plan), run_id="new")

    def test_executor_pass_before_final_validation_cannot_supply_reuse(self):
        report = self.make_report()
        del report["validation_complete"]
        with self.assertRaisesRegex(ValueError, "no completed"):
            prepare_reuse(report, repo=self.root, plan=self.plan, identities=self.identities,
                          toolchain=self.toolchain, inventory=inventory_identity(self.plan), run_id="new")

    def test_wrong_result_run_is_rerun_with_dependents(self):
        report = self.make_report()
        path = result_path(self.root / "old", "native-package:provider")
        saved = json.loads(path.read_text())
        saved["run_id"] = "another-run"
        path.write_text(json.dumps(saved))
        _, invalid = prepare_reuse(report, repo=self.root, plan=self.plan,
                                   identities=self.identities, toolchain=self.toolchain,
                                   inventory=inventory_identity(self.plan), run_id="new")
        self.assertEqual(set(invalid), {"native-package:provider", "native-package:consumer-eager"})

    def test_nonfinite_timeout_is_rejected_before_tool_admission(self):
        with patch("scripts.objc3c_native_validation.cli.resolve_toolchain") as resolve:
            with self.assertRaisesRegex(ValueError, "finite"):
                run_validation(repo=self.root, plan=self.plan, output_root=self.root / "runs",
                               run_id="new", timeout_seconds=float("nan"))
            resolve.assert_not_called()

    def test_worker_removes_ambient_compiler_injection_and_uses_admitted_inputs(self):
        with patch.dict(os.environ, {"CL": "/DUNRECORDED", "CPATH": "unsealed-headers",
                                    "INCLUDE": "ambient", "LIBPATH": "ambient-libraries"}):
            apply_worker_environment({"INCLUDE": "admitted-headers"})
            self.assertNotIn("CL", os.environ)
            self.assertNotIn("CPATH", os.environ)
            self.assertNotIn("LIBPATH", os.environ)
            self.assertEqual(os.environ["INCLUDE"], "admitted-headers")

    def test_selection_changes_preserve_full_inventory_identity(self):
        selected = {**self.plan, "selected_node_ids": ["carriers:match:clang"], "selected_families": ["carriers"]}
        self.assertEqual(inventory_identity(self.plan), inventory_identity(selected))

    def test_changed_toolchain_overrules_successful_execution_and_preserves_failure_report(self):
        output = self.root / "runs"

        def execution(plan, **kwargs):
            run_dir = output / "new"
            run_dir.mkdir(parents=True)
            records = []
            for node in plan["nodes"]:
                node_id = node["node_id"]
                path = result_path(run_dir, node_id)
                path.parent.mkdir(parents=True)
                artifact = path.parent / "object.o"
                artifact.write_bytes(b"native object")
                write_node_result(path, node_id=node_id, node_identity=kwargs["node_identities"][node_id],
                                  build_identity=kwargs["build_identity"], run_id="new",
                                  artifacts={"object": artifact}, data={}, allowed_roots=[self.root])
                records.append({"node_id": node_id, "status": "completed"})
            return {"status": "PASS", "partial_report": False, "selection_complete": True,
                    "node_records": records}

        with patch("scripts.objc3c_native_validation.cli.resolve_toolchain", return_value=self.toolchain), \
             patch("scripts.objc3c_native_validation.cli.execute_node_plan", side_effect=execution), \
             patch("scripts.objc3c_native_validation.cli.verify_toolchain_snapshot", side_effect=ValueError("runtime DLL changed")):
            result = run_validation(repo=self.root, plan=self.plan, output_root=output,
                                    run_id="new", timeout_seconds=10)
        self.assertEqual(result["status"], "FAIL")
        self.assertTrue(result["partial_report"])
        self.assertFalse(result["selection_complete"])
        self.assertFalse(result["validation_complete"])
        self.assertTrue(all(row["status"] == "failed" for row in result["node_records"]))
        persisted = json.loads((output / "new" / "summary.json").read_text())
        self.assertIn("runtime DLL changed", persisted["validation_errors"][0])


if __name__ == "__main__":
    unittest.main()

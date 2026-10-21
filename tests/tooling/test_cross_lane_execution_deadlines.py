"""Bounded process cleanup and retained-output regressions without native tools."""

from __future__ import annotations

from pathlib import Path
import subprocess
import sys
import tempfile
import threading
import time
from types import SimpleNamespace
import unittest
from unittest import mock


sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from scripts import objc3c_cross_lane_execution as execution


class RetainedPipe:
    def __init__(self) -> None:
        self.release = threading.Event()
        self.closed = False

    def readline(self) -> str:
        self.release.wait(timeout=5)
        return ""

    def close(self) -> None:
        self.closed = True


class ExecutionDeadlineTests(unittest.TestCase):
    def setUp(self) -> None:
        self.directory = tempfile.TemporaryDirectory(prefix="node-deadline-")
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name)
        self.log = self.root / "node.log"

    def run_node(self, command: list[str], timeout: float) -> dict:
        return execution.run_node_command(
            {"node_id": "deadline-probe"}, command,
            cwd=self.root, log_path=self.log, timeout_seconds=timeout)

    def test_windows_taskkill_failure_or_timeout_kills_only_owned_leader(self) -> None:
        failures = (
            subprocess.CompletedProcess(["taskkill"], 1),
            subprocess.TimeoutExpired(["taskkill"], 2),
            OSError("taskkill unavailable"),
        )
        for failure in failures:
            with self.subTest(failure=type(failure).__name__):
                process = mock.Mock(pid=321, returncode=None)
                process.poll.return_value = None
                effect = failure if isinstance(failure, Exception) else None
                with mock.patch.object(execution, "os", SimpleNamespace(name="nt")), \
                     mock.patch.object(execution.subprocess, "run", side_effect=effect,
                                       return_value=failure) as taskkill:
                    execution._terminate_tree(process)
                command = taskkill.call_args.args[0]
                self.assertEqual(command, ["taskkill", "/PID", "321", "/T", "/F"])
                self.assertEqual(taskkill.call_args.kwargs["timeout"],
                                 execution._TERMINATE_TIMEOUT_SECONDS)
                process.kill.assert_called_once_with()
                process.wait.assert_called_once_with(timeout=execution._TERMINATE_TIMEOUT_SECONDS)

    def test_exited_windows_leader_does_not_taskkill_a_reusable_pid(self) -> None:
        process = mock.Mock(pid=321, returncode=0)
        process.poll.return_value = 0
        with mock.patch.object(execution, "os", SimpleNamespace(name="nt")), \
             mock.patch.object(execution.subprocess, "run") as taskkill:
            execution._terminate_tree(process)
        taskkill.assert_not_called()
        process.kill.assert_not_called()

    def test_failed_owned_leader_kill_wait_is_bounded(self) -> None:
        process = mock.Mock(pid=321, returncode=None)
        process.poll.return_value = None
        process.kill.side_effect = OSError("kill denied")
        process.wait.side_effect = subprocess.TimeoutExpired(["owned"], 2)
        with mock.patch.object(execution, "os", SimpleNamespace(name="nt")), \
             mock.patch.object(execution.subprocess, "run", side_effect=OSError("taskkill failed")):
            execution._terminate_tree(process)
        process.wait.assert_called_once_with(timeout=execution._TERMINATE_TIMEOUT_SECONDS)

    def test_pipe_retained_after_leader_exit_cannot_extend_node_deadline(self) -> None:
        pipe = RetainedPipe()
        process = mock.Mock(pid=321, stdout=pipe, returncode=0)
        process.poll.return_value = 0
        process.wait.return_value = 0
        started = time.monotonic()
        try:
            with mock.patch.object(execution.subprocess, "Popen", return_value=process), \
                 mock.patch.object(execution, "_terminate_tree") as cleanup, \
                 mock.patch.object(execution, "_OUTPUT_DRAIN_TIMEOUT_SECONDS", 0.05), \
                 mock.patch.object(execution, "_READER_JOIN_TIMEOUT_SECONDS", 0.02):
                result = self.run_node(["owned-probe"], 0.05)
            self.assertLess(time.monotonic() - started, 1)
            self.assertTrue(result["timed_out"])
            self.assertEqual(result["exit_code"], 124)
            self.assertIn("output drain deadline exceeded", self.log.read_text())
            cleanup.assert_called_once_with(process)
            process.wait.assert_called_once_with(timeout=execution._TERMINATE_TIMEOUT_SECONDS)
        finally:
            pipe.release.set()

    def test_unresponsive_leader_and_pipe_still_return_timeout(self) -> None:
        pipe = RetainedPipe()
        process = mock.Mock(pid=321, stdout=pipe, returncode=None)
        process.poll.return_value = None
        process.wait.side_effect = subprocess.TimeoutExpired(["owned-probe"], 2)
        try:
            with mock.patch.object(execution.subprocess, "Popen", return_value=process), \
                 mock.patch.object(execution, "_terminate_tree"), \
                 mock.patch.object(execution, "_OUTPUT_DRAIN_TIMEOUT_SECONDS", 0.05), \
                 mock.patch.object(execution, "_READER_JOIN_TIMEOUT_SECONDS", 0.02):
                result = self.run_node(["owned-probe"], 0.05)
            self.assertTrue(result["timed_out"])
            self.assertEqual(result["exit_code"], 124)
            self.assertIn("leader did not exit within cleanup deadline", self.log.read_text())
        finally:
            pipe.release.set()

    def test_nonfinite_deadline_cannot_spawn_a_process(self) -> None:
        with mock.patch.object(execution.subprocess, "Popen") as spawn:
            for timeout in (float("nan"), float("inf"), -float("inf")):
                with self.subTest(timeout=timeout), self.assertRaises(ValueError):
                    self.run_node(["owned-probe"], timeout)
            spawn.assert_not_called()

    def test_short_python_process_keeps_normal_output_and_status(self) -> None:
        result = self.run_node([sys.executable, "-c", "print('normal output', flush=True)"], 5)
        self.assertFalse(result["timed_out"])
        self.assertEqual(result["exit_code"], 0)
        self.assertIn("normal output", self.log.read_text())

    def test_short_python_sleep_is_terminated_at_deadline(self) -> None:
        started = time.monotonic()
        result = self.run_node([sys.executable, "-c", "import time; time.sleep(5)"], 0.1)
        self.assertLess(time.monotonic() - started, 4)
        self.assertTrue(result["timed_out"])
        self.assertEqual(result["exit_code"], 124)


if __name__ == "__main__":
    unittest.main()

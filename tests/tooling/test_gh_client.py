import importlib.util
import json
import subprocess
import sys
from pathlib import Path

MODULE_PATH = Path(__file__).resolve().parents[2] / "scripts" / "lib" / "gh_client.py"
SPEC = importlib.util.spec_from_file_location("gh_client", MODULE_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/lib/gh_client.py for tests.")
gh_client = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = gh_client
SPEC.loader.exec_module(gh_client)


def completed(
    returncode: int,
    *,
    stdout: bytes = b"",
    stderr: bytes = b"",
) -> subprocess.CompletedProcess[bytes]:
    return subprocess.CompletedProcess(
        args=["gh"],
        returncode=returncode,
        stdout=stdout,
        stderr=stderr,
    )


class SequenceRunner:
    def __init__(self, responses: list[subprocess.CompletedProcess[bytes]]) -> None:
        self._responses = list(responses)
        self.calls: list[tuple[list[str], Path]] = []

    def __call__(self, args: list[str], cwd: Path) -> subprocess.CompletedProcess[bytes]:
        self.calls.append((list(args), cwd))
        if not self._responses:
            raise AssertionError("runner called more times than expected")
        return self._responses.pop(0)


def test_run_json_decodes_utf8_with_replacement() -> None:
    runner = SequenceRunner(
        [
            completed(
                0,
                stdout=b'{"title":"caf\xff"}',
            )
        ]
    )
    client = gh_client.GhClient(
        root=Path("."),
        runner=runner,
        sleeper=lambda _: None,
    )

    payload = client.run_json(["api", "/zen"])

    assert payload == {"title": "caf\ufffd"}
    assert runner.calls[0][0] == ["gh", "api", "/zen"]


def test_issue_numbers_uses_paginated_api_and_filters_pull_requests() -> None:
    pages = [
        [
            {"number": 11},
            {"number": 12, "pull_request": {"url": "https://api.github.com/pulls/12"}},
        ],
        [{"number": 13}, {"number": "bad"}],
    ]
    runner = SequenceRunner(
        [
            completed(
                0,
                stdout=json.dumps(pages).encode("utf-8"),
            )
        ]
    )
    client = gh_client.GhClient(
        root=Path("."),
        runner=runner,
        sleeper=lambda _: None,
    )

    numbers = client.issue_numbers(state="open")

    assert numbers == {11, 13}
    assert runner.calls[0][0] == [
        "gh",
        "api",
        "repos/{owner}/{repo}/issues?state=open&per_page=100",
        "--paginate",
        "--slurp",
    ]


def test_run_json_retries_transient_failures_with_exponential_backoff() -> None:
    sleeps: list[float] = []
    runner = SequenceRunner(
        [
            completed(1, stderr=b"gh: Bad Gateway (HTTP 502)"),
            completed(1, stderr=b"gh: API rate limit exceeded (HTTP 429)"),
            completed(0, stdout=b'{"ok": true}'),
        ]
    )
    client = gh_client.GhClient(
        root=Path("."),
        max_retries=3,
        backoff_base_seconds=0.2,
        runner=runner,
        sleeper=sleeps.append,
    )

    payload = client.run_json(["api", "/rate_limit"])

    assert payload == {"ok": True}
    assert [round(delay, 3) for delay in sleeps] == [0.2, 0.4]
    assert len(runner.calls) == 3

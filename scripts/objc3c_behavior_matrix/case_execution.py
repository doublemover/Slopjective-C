"""Per-fixture behavior matrix execution."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.behavior_fixtures import BehaviorFixture
from objc3c_tooling.subprocesses import bounded_text

from .errors import BehaviorMatrixFailure
from .expectations import assert_tokens
from .native_execution import (
    canonical_link_text,
    compile_fixture,
    compile_output_text,
    link_fixture,
    run_executable,
)


def execute_fixture(fixture: BehaviorFixture, case_dir: Path) -> dict[str, object]:
    compile_result = compile_fixture(fixture, case_dir)
    compile_dir = case_dir / "compile"
    compile_text = compile_output_text(compile_result, compile_dir)
    expected_stage = fixture.expected_stage

    if expected_stage in {"parse", "compile"}:
        if fixture.fixture_kind in {"negative", "strict-error", "rejection"}:
            if compile_result.returncode == 0:
                raise BehaviorMatrixFailure(f"expected compile failure for {fixture.relative_source}")
            assert_tokens(fixture, compile_text)
        elif compile_result.returncode != 0:
            raise BehaviorMatrixFailure(
                f"compile failed for {fixture.relative_source}:\n{bounded_text(compile_text)}"
            )
        return {
            "fixture": fixture.relative_source,
            "metadata": fixture.relative_metadata,
            "owner_phase": fixture.owner_phase,
            "behavior_family": fixture.behavior_family,
            "fixture_kind": fixture.fixture_kind,
            "expected_stage": expected_stage,
            "compile": compile_result.to_dict(include_output=False),
            "status": "PASS",
        }

    if compile_result.returncode != 0:
        raise BehaviorMatrixFailure(
            f"compile failed before {expected_stage} check for {fixture.relative_source}:\n"
            f"{bounded_text(compile_text)}"
        )

    if expected_stage == "link":
        link_result = link_fixture(fixture, case_dir, compile_dir, include_runtime=False)
        link_text = canonical_link_text(fixture, link_result, compile_dir)
        if fixture.fixture_kind in {"negative", "strict-error", "rejection"}:
            if link_result.returncode == 0:
                raise BehaviorMatrixFailure(f"expected link failure for {fixture.relative_source}")
            assert_tokens(fixture, link_text)
        elif link_result.returncode != 0:
            raise BehaviorMatrixFailure(
                f"link failed for {fixture.relative_source}:\n{bounded_text(link_text)}"
            )
        return {
            "fixture": fixture.relative_source,
            "metadata": fixture.relative_metadata,
            "owner_phase": fixture.owner_phase,
            "behavior_family": fixture.behavior_family,
            "fixture_kind": fixture.fixture_kind,
            "expected_stage": expected_stage,
            "compile": compile_result.to_dict(include_output=False),
            "link": link_result.to_dict(include_output=False),
            "status": "PASS",
        }

    if expected_stage == "run":
        link_result = link_fixture(fixture, case_dir, compile_dir, include_runtime=True)
        link_text = "\n".join(part for part in (link_result.stdout, link_result.stderr) if part)
        if link_result.returncode != 0:
            raise BehaviorMatrixFailure(
                f"link failed before run for {fixture.relative_source}:\n{bounded_text(link_text)}"
            )
        run_result = run_executable(fixture, case_dir)
        run_text = "\n".join(part for part in (run_result.stdout, run_result.stderr) if part)
        if fixture.fixture_kind in {"negative", "strict-error", "rejection"}:
            if run_result.returncode == 0:
                raise BehaviorMatrixFailure(f"expected run failure for {fixture.relative_source}")
            assert_tokens(fixture, run_text)
        elif run_result.returncode != fixture.expected_exit_code:
            raise BehaviorMatrixFailure(
                f"unexpected run exit for {fixture.relative_source}: "
                f"expected={fixture.expected_exit_code} actual={run_result.returncode}\n"
                f"{bounded_text(run_text)}"
            )
        return {
            "fixture": fixture.relative_source,
            "metadata": fixture.relative_metadata,
            "owner_phase": fixture.owner_phase,
            "behavior_family": fixture.behavior_family,
            "fixture_kind": fixture.fixture_kind,
            "expected_stage": expected_stage,
            "compile": compile_result.to_dict(include_output=False),
            "link": link_result.to_dict(include_output=False),
            "run": run_result.to_dict(include_output=False),
            "expected_exit_code": fixture.expected_exit_code,
            "status": "PASS",
        }

    raise BehaviorMatrixFailure(f"unsupported expected stage {expected_stage} for {fixture.relative_source}")

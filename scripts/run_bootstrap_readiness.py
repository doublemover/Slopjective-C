#!/usr/bin/env python3
"""Run deterministic bootstrap readiness orchestration and persist evidence artifacts."""

from __future__ import annotations

import sys
from typing import Sequence

from objc3c_tooling.paths import display_path, resolve_repo_path

if __package__:
    from .bootstrap_readiness.checks import (
        check_markdown_consistency,
        determine_final_exit,
        parse_checker_payload,
    )
    from .bootstrap_readiness.cli import build_parser
    from .bootstrap_readiness.command_specs import (
        checker_specs,
        open_blockers_refresh_spec,
        spec_lint_spec,
    )
    from .bootstrap_readiness.commands import run_command
    from .bootstrap_readiness.constants import (
        CHECK_BOOTSTRAP_READINESS_SCRIPT_PATH,
        DEFAULT_CATALOG_JSON,
        DEFAULT_COMMAND_TIMEOUT_SECONDS,
        DEFAULT_OPEN_BLOCKERS_ROOT,
        DEFAULT_OUTPUT_DIR,
        DEFAULT_REFRESH_OPEN_BLOCKERS_RELATIVE_PATH,
        EXIT_BLOCKED,
        EXIT_BOOTSTRAPPABLE,
        EXIT_RUNNER_ERROR,
        EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH,
        ROOT,
        SPEC_LINT_SCRIPT_PATH,
    )
    from .bootstrap_readiness.evidence import persist_artifacts, write_text
    from .bootstrap_readiness.inputs import (
        default_open_blockers_output_path,
        resolve_inputs,
    )
    from .bootstrap_readiness.models import CommandResult, CommandSpec
    from .bootstrap_readiness.reports import (
        BOOTSTRAP_JSON_FILENAME,
        BOOTSTRAP_JSON_LOG_FILENAME,
        BOOTSTRAP_MD_FILENAME,
        BOOTSTRAP_MD_LOG_FILENAME,
        OPEN_BLOCKERS_REFRESH_LOG_FILENAME,
        REPORT_MD_FILENAME,
        SPEC_LINT_LOG_FILENAME,
        SUMMARY_JSON_FILENAME,
        build_summary_payload,
        render_command_log,
        render_markdown_report,
    )
else:
    from bootstrap_readiness.checks import (
        check_markdown_consistency,
        determine_final_exit,
        parse_checker_payload,
    )
    from bootstrap_readiness.cli import build_parser
    from bootstrap_readiness.command_specs import (
        checker_specs,
        open_blockers_refresh_spec,
        spec_lint_spec,
    )
    from bootstrap_readiness.commands import run_command
    from bootstrap_readiness.constants import (
        CHECK_BOOTSTRAP_READINESS_SCRIPT_PATH,
        DEFAULT_CATALOG_JSON,
        DEFAULT_COMMAND_TIMEOUT_SECONDS,
        DEFAULT_OPEN_BLOCKERS_ROOT,
        DEFAULT_OUTPUT_DIR,
        DEFAULT_REFRESH_OPEN_BLOCKERS_RELATIVE_PATH,
        EXIT_BLOCKED,
        EXIT_BOOTSTRAPPABLE,
        EXIT_RUNNER_ERROR,
        EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH,
        ROOT,
        SPEC_LINT_SCRIPT_PATH,
    )
    from bootstrap_readiness.evidence import persist_artifacts, write_text
    from bootstrap_readiness.inputs import (
        default_open_blockers_output_path,
        resolve_inputs,
    )
    from bootstrap_readiness.models import CommandResult, CommandSpec
    from bootstrap_readiness.reports import (
        BOOTSTRAP_JSON_FILENAME,
        BOOTSTRAP_JSON_LOG_FILENAME,
        BOOTSTRAP_MD_FILENAME,
        BOOTSTRAP_MD_LOG_FILENAME,
        OPEN_BLOCKERS_REFRESH_LOG_FILENAME,
        REPORT_MD_FILENAME,
        SPEC_LINT_LOG_FILENAME,
        SUMMARY_JSON_FILENAME,
        build_summary_payload,
        render_command_log,
        render_markdown_report,
    )


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    inputs = resolve_inputs(args)

    issues_path = inputs.issues_path
    milestones_path = inputs.milestones_path
    catalog_path = inputs.catalog_path
    output_dir = inputs.output_dir
    open_blockers_path = inputs.open_blockers_path
    refresh_open_blockers_root = inputs.refresh_open_blockers_root

    errors: list[str] = []
    refresh_open_blockers_result: CommandResult | None = None
    spec_lint_result: CommandResult | None = None

    if args.refresh_open_blockers:
        if open_blockers_path is None:
            open_blockers_path = default_open_blockers_output_path(output_dir)

        if args.open_blockers_generated_at_utc is None:
            errors.append(
                "--open-blockers-generated-at-utc is required when --refresh-open-blockers is set."
            )
        if args.open_blockers_source is None:
            errors.append(
                "--open-blockers-source is required when --refresh-open-blockers is set."
            )

        if (
            refresh_open_blockers_root is not None
            and args.open_blockers_generated_at_utc is not None
            and args.open_blockers_source is not None
        ):
            refresh_spec = open_blockers_refresh_spec(
                root=refresh_open_blockers_root,
                generated_at_utc=args.open_blockers_generated_at_utc,
                source=args.open_blockers_source,
            )
            refresh_open_blockers_result = run_command(refresh_spec)
            if refresh_open_blockers_result.exit_code != 0:
                errors.append(
                    "extract_open_blockers(snapshot-json) returned unexpected exit code "
                    f"{refresh_open_blockers_result.exit_code}."
                )
            else:
                assert open_blockers_path is not None
                try:
                    write_text(open_blockers_path, refresh_open_blockers_result.stdout)
                except OSError as exc:
                    errors.append(
                        "unable to persist refreshed open blockers snapshot to "
                        f"{display_path(open_blockers_path)}: {exc}."
                    )

    checker_json_spec, checker_markdown_spec = checker_specs(
        issues_path=issues_path,
        milestones_path=milestones_path,
        catalog_path=catalog_path,
        open_blockers_path=open_blockers_path,
    )

    checker_json_result = run_command(checker_json_spec)
    checker_markdown_result = run_command(checker_markdown_spec)

    checker_payload, checker_error = parse_checker_payload(checker_json_result)
    if checker_error is not None:
        errors.append(checker_error)

    if checker_payload is not None:
        markdown_error = check_markdown_consistency(
            checker_markdown_result,
            checker_payload=checker_payload,
        )
        if markdown_error is not None:
            errors.append(markdown_error)

    if args.run_spec_lint:
        spec_lint_result = run_command(spec_lint_spec(args.spec_globs))
        if spec_lint_result.exit_code != 0:
            errors.append(
                f"spec_lint returned unexpected exit code {spec_lint_result.exit_code}."
            )

    final_exit_code, final_status = determine_final_exit(
        checker_payload=checker_payload,
        errors=errors,
    )

    try:
        persist_artifacts(
            output_dir=output_dir,
            issues_path=issues_path,
            milestones_path=milestones_path,
            catalog_path=catalog_path,
            open_blockers_path=open_blockers_path,
            refresh_open_blockers_requested=bool(args.refresh_open_blockers),
            refresh_open_blockers_result=refresh_open_blockers_result,
            refresh_open_blockers_root=refresh_open_blockers_root,
            refresh_open_blockers_generated_at_utc=args.open_blockers_generated_at_utc,
            refresh_open_blockers_source=args.open_blockers_source,
            checker_json_result=checker_json_result,
            checker_markdown_result=checker_markdown_result,
            checker_payload=checker_payload,
            run_spec_lint_requested=bool(args.run_spec_lint),
            spec_globs=tuple(args.spec_globs),
            spec_lint_result=spec_lint_result,
            errors=tuple(errors),
            final_status=final_status,
            final_exit_code=final_exit_code,
        )
    except OSError as exc:
        print(f"error: unable to persist bootstrap readiness artifacts: {exc}", file=sys.stderr)
        return EXIT_RUNNER_ERROR

    print(
        "bootstrap-readiness: "
        f"status={final_status} "
        f"exit_code={final_exit_code} "
        f"summary={display_path(output_dir / SUMMARY_JSON_FILENAME)} "
        f"report={display_path(output_dir / REPORT_MD_FILENAME)}"
    )
    return final_exit_code


if __name__ == "__main__":
    raise SystemExit(main())

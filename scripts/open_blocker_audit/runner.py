"""Open-blocker audit orchestration flow."""

from __future__ import annotations

import argparse
import json
import sys
from typing import Callable

from objc3c_tooling.paths import display_path, resolve_repo_path

from .models import CommandResult, CommandSpec
from .normalization import (
    extract_blocker_count,
    normalize_extract_snapshot_stdout,
    normalize_snapshot_metadata,
)
from .rendering import (
    CHECKER_MODE,
    DEFAULT_SNAPSHOT_RELATIVE_PATH,
    EXIT_OK,
    EXIT_RUNNER_ERROR,
    RUNNER_CONTRACT_ID,
    RUNNER_CONTRACT_VERSION,
    build_contract_check_spec,
    build_output_paths,
    build_summary_payload,
    determine_final_exit,
    render_command_log,
    render_contract_check_transcript,
    render_markdown_report,
    validate_contract_check_output,
    write_text,
)
from .scanning import (
    build_extract_snapshot_spec,
    prepare_audit_scope,
    resolve_scope_markdown_paths,
)

CommandRunner = Callable[[CommandSpec], CommandResult]


def run_audit(args: argparse.Namespace, *, command_runner: CommandRunner) -> int:
    audit_root = resolve_repo_path(args.audit_root)
    output_dir = resolve_repo_path(args.output_dir)
    snapshot_json_path = (
        resolve_repo_path(args.snapshot_json)
        if args.snapshot_json is not None
        else output_dir / DEFAULT_SNAPSHOT_RELATIVE_PATH
    )

    scope, errors = prepare_audit_scope(
        audit_root=audit_root,
        raw_include_globs=args.include_globs,
        raw_exclude_paths=args.exclude_paths,
        include_default_excludes=not args.no_default_exclude,
    )
    metadata, metadata_errors = normalize_snapshot_metadata(
        raw_generated_at_utc=args.generated_at_utc,
        raw_source=args.source,
    )
    errors.extend(metadata_errors)

    if not errors:
        scope, markdown_errors = resolve_scope_markdown_paths(scope)
        errors.extend(markdown_errors)

    extract_result: CommandResult | None = None
    normalized_snapshot_payload: dict[str, object] | None = None

    if not errors:
        assert metadata.generated_at_utc is not None
        assert metadata.source is not None

        extract_spec = build_extract_snapshot_spec(
            effective_audit_root=scope.effective_audit_root,
            generated_at_utc=metadata.generated_at_utc,
            source=metadata.source,
            extractor_exclude_paths=scope.extractor_exclude_paths,
        )
        extract_result = command_runner(extract_spec)

        if extract_result.exit_code != 0:
            errors.append(
                "extract_open_blockers(snapshot-json) returned unexpected exit code "
                f"{extract_result.exit_code}."
            )
        else:
            normalized_snapshot_payload, normalize_errors = normalize_extract_snapshot_stdout(
                extract_result.stdout,
                generated_at_utc=metadata.generated_at_utc,
                source=metadata.source,
            )
            errors.extend(normalize_errors)

    blocker_count = extract_blocker_count(normalized_snapshot_payload)
    final_exit_code, final_status = determine_final_exit(
        errors=errors,
        blocker_count=blocker_count,
    )

    paths = build_output_paths(
        output_dir=output_dir,
        snapshot_json_path=snapshot_json_path,
    )

    try:
        output_dir.mkdir(parents=True, exist_ok=True)
        if extract_result is not None:
            write_text(
                paths.extract_log_path,
                render_command_log(
                    "extract_open_blockers snapshot-json command output",
                    extract_result,
                ),
            )

        if normalized_snapshot_payload is not None:
            write_text(
                snapshot_json_path,
                json.dumps(normalized_snapshot_payload, indent=2) + "\n",
            )

        summary = build_summary_payload(
            audit_root=audit_root,
            effective_audit_root=scope.effective_audit_root,
            include_globs=scope.include_globs,
            exclude_paths=scope.exclude_paths,
            extractor_exclude_paths=scope.extractor_exclude_paths,
            generated_at_utc=metadata.generated_at_utc,
            source=metadata.source,
            output_dir=output_dir,
            snapshot_json_path=snapshot_json_path,
            included_markdown_paths=scope.included_markdown_paths,
            excluded_markdown_paths=scope.excluded_markdown_paths,
            extract_result=extract_result,
            blocker_count=blocker_count,
            errors=tuple(errors),
            final_status=final_status,
            final_exit_code=final_exit_code,
        )
        summary_json = json.dumps(summary, indent=2) + "\n"
        report_markdown = render_markdown_report(summary)
        write_text(paths.summary_json_path, summary_json)
        write_text(paths.report_md_path, report_markdown)

        contract_check_spec = build_contract_check_spec(paths)
        contract_check_result = command_runner(contract_check_spec)

        write_text(
            paths.contract_check_transcript_path,
            render_contract_check_transcript(contract_check_result),
        )
        write_text(paths.contract_check_stderr_path, contract_check_result.stderr)

        contract_check_errors = validate_contract_check_output(
            contract_check_result,
            summary_json_path=paths.summary_json_path,
            snapshot_json_path=paths.snapshot_json_path,
            extract_log_path=paths.extract_log_path,
            checker_mode=CHECKER_MODE,
            runner_contract_id=RUNNER_CONTRACT_ID,
            runner_contract_version=RUNNER_CONTRACT_VERSION,
            exit_ok=EXIT_OK,
        )
        if contract_check_errors:
            errors.extend(contract_check_errors)
            final_exit_code, final_status = determine_final_exit(
                errors=errors,
                blocker_count=blocker_count,
            )
            summary = build_summary_payload(
                audit_root=audit_root,
                effective_audit_root=scope.effective_audit_root,
                include_globs=scope.include_globs,
                exclude_paths=scope.exclude_paths,
                extractor_exclude_paths=scope.extractor_exclude_paths,
                generated_at_utc=metadata.generated_at_utc,
                source=metadata.source,
                output_dir=output_dir,
                snapshot_json_path=snapshot_json_path,
                included_markdown_paths=scope.included_markdown_paths,
                excluded_markdown_paths=scope.excluded_markdown_paths,
                extract_result=extract_result,
                blocker_count=blocker_count,
                errors=tuple(errors),
                final_status=final_status,
                final_exit_code=final_exit_code,
            )
            summary_json = json.dumps(summary, indent=2) + "\n"
            report_markdown = render_markdown_report(summary)
            write_text(paths.summary_json_path, summary_json)
            write_text(paths.report_md_path, report_markdown)
    except OSError as exc:
        print(f"error: unable to persist open blocker audit artifacts: {exc}", file=sys.stderr)
        return EXIT_RUNNER_ERROR

    print(
        "open-blocker-audit: "
        f"status={final_status} "
        f"exit_code={final_exit_code} "
        f"snapshot={display_path(snapshot_json_path)} "
        f"summary={display_path(paths.summary_json_path)} "
        f"report={display_path(paths.report_md_path)} "
        f"contract_check_transcript={display_path(paths.contract_check_transcript_path)} "
        f"contract_check_stderr={display_path(paths.contract_check_stderr_path)}"
    )
    return final_exit_code


__all__ = ["CommandRunner", "run_audit"]

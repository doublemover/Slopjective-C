"""Execute the bounded native PR surface for the actual tested merge revision."""
from __future__ import annotations

import argparse
import json
import math
from pathlib import Path
import re

from scripts.objc3c_implementation_assurance import pr_pilot
from scripts.objc3c_toolchain.binary_packages import write_json
from scripts.objc3c_toolchain.core import planned_development_identity
from scripts.objc3c_toolchain.linux_host import native_linux_host, native_target
from scripts.objc3c_toolchain.package import verify_sdk
from .evidence import aggregate, artifact
from .plan import PACKAGE, from_checkout, git, prose
from .preview_download import WITNESS_OUTPUT
from .runner import run

FOUNDATION_FIELDS = {
    'upstream_commit': 'upstream_commit',
    'patch_digest': 'patch_digest',
    'build_config_digest': 'build_config_digest',
    'source_build_id': 'build_id',
}
SOURCE_SUITES = ('runtime', 'frontend')
SCHEMA = 'objc3c.native-pr.v1'
TOOLING_PATHS = ('.github/', 'scripts/objc3c_assurance/')
PREVIEW_INSTALL_PATHS = ('scripts/objc3c_assurance/preview_download.py',
                         'scripts/objc3c_toolchain/release_install.py',
                         '.github/actions/linux-preview-archive-cache/')
FRONTEND_WITNESSES = frozenset((
    'objc3_borrowed_pointers', 'objc3_c_inline_provider', 'objc3_carrier_service',
    'objc3_compile_session', 'objc3_diagnostic_cli', 'objc3_interface_codec',
))


def needs_install_witness(plan: dict) -> bool:
    paths = plan['changed_paths']
    return not paths or any(path.startswith(PACKAGE + PREVIEW_INSTALL_PATHS) for path in paths)


def frontend_selection(plan: dict) -> frozenset[str] | None:
    paths = plan['changed_paths']
    if paths and all(prose(path) or path.startswith(TOOLING_PATHS) or
                     (path.startswith('tests/tooling/test_') and path.endswith('.py') and path.count('/') == 2)
                     for path in paths):
        return FRONTEND_WITNESSES
    return None


def matching_foundation(repo: Path, plan: dict, sdk: Path | None) -> dict | None:
    if sdk is None or plan['foundation_policy'] == 'rebuild-required':
        return None
    installed = verify_sdk(sdk)
    if installed.get('target') != native_target():
        raise ValueError('PR foundation SDK targets a different machine')
    expected = planned_development_identity(repo)
    if any(installed.get(field) != expected[key] for field, key in FOUNDATION_FIELDS.items()):
        return None
    return {**{field: installed[field] for field in FOUNDATION_FIELDS},
            'seal_sha256': installed['seal_sha256'], 'target': installed['target']}


def matching_install_cost(cost: dict, release: dict) -> bool:
    return (cost.get('schema') == 'objc3c.sdk-install-cost.v1' and
            cost.get('seal_sha256') == release['seal_sha256'] and
            cost.get('source_build_id') == release['source_build_id'] and
            cost.get('archive_sha256') == release['sha256'] and cost.get('archive_bytes') == release['bytes'])


def execute(repo: Path, output: Path, *, head: str, base: str, tested: str,
            sdk: Path | None = None) -> dict:
    repo, output = repo.resolve(strict=True), output.resolve()
    if output.exists() or not output.is_relative_to(repo / 'out'):
        raise ValueError('native PR evidence requires fresh owned output')
    plan = from_checkout(repo, head=head, base=base, tested=tested)
    output.mkdir(parents=True)
    write_json(output / 'plan.json', plan)
    receipt = {'schema': SCHEMA, 'status': 'running',
               'revision': plan['revision'], 'plan_sha256': plan['sha256'],
               'classification': plan['classification']}
    try:
        if plan['classification'] == 'prose-only':
            receipt.update(status='PASS', foundation='not-required', suites=[],
                           implementation_pilot=pr_pilot.execute(repo, plan, output / 'implementation-pilot',
                               sdk=None, admitted=None, foundation_report=output / 'execution/foundation/foundation.json'))
            return receipt
        foundation = matching_foundation(repo, plan, sdk)
        suites = SOURCE_SUITES if foundation else ('foundation', *SOURCE_SUITES)
        frontend_names = frontend_selection(plan) if foundation else None
        receipt.update(foundation='verified-release' if foundation else 'current-source-build',
                       foundation_sdk=foundation, suites=list(suites),
                       frontend_ctests=sorted(frontend_names) if frontend_names is not None else None)
        result = run(repo, plan, output / 'execution', suite_ids=suites,
                     product_sdk=sdk if foundation else None, frontend_names=frontend_names)
        if result.get('revision') != plan['revision'] or result.get('plan_sha256') != plan['sha256']:
            raise ValueError('native PR execution reported a different tested plan')
        if result['status'] != 'PASS' or tuple(row['id'] for row in result['suites']) != suites or any(
                row['status'] != 'PASS' or not row['executions'] for row in result['suites']):
            raise ValueError('native PR execution omitted a passing selected suite')
        if foundation and matching_foundation(repo, plan, sdk) != foundation:
            raise ValueError('PR foundation SDK changed during native validation')
        receipt['implementation_pilot'] = pr_pilot.execute(repo, plan, output / 'implementation-pilot',
            sdk=sdk if foundation else None, admitted=foundation,
            foundation_report=output / 'execution/foundation/foundation.json')
        receipt['status'] = 'PASS'
        return receipt
    except BaseException as error:
        receipt.update(status='FAIL', error=f'{type(error).__name__}: {error}')
        raise
    finally:
        write_json(output / 'result.json', receipt)


def verify(repo: Path, evidence: Path, *, head: str, base: str, tested: str, execution: str) -> dict:
    """Independently check the exact tested merge and its published native receipts."""
    if execution != 'success':
        raise ValueError('native PR execution did not finish successfully: ' + execution)
    if evidence.is_symlink() or not evidence.is_dir():
        raise ValueError('native PR evidence directory is missing or linked')

    def read(name):
        path = evidence / name
        artifact(evidence, path)
        return json.loads(path.read_bytes())

    plan = from_checkout(repo, head=head, base=base, tested=tested)
    if read('selection.json') != plan or read('checked/plan.json') != plan:
        raise ValueError('native PR evidence describes a different tested selection')
    receipt = read('checked/result.json')
    expected = {'schema': SCHEMA, 'status': 'PASS', 'revision': plan['revision'],
                'plan_sha256': plan['sha256'], 'classification': plan['classification']}
    if any(receipt.get(key) != value for key, value in expected.items()):
        raise ValueError('native PR receipt is stale, failed or does not match the tested selection')
    pilot = receipt.get('implementation_pilot', {})
    foundation_report = evidence / 'checked/execution/foundation/foundation.json'
    if plan['classification'] == 'prose-only':
        if receipt.get('foundation') != 'not-required' or receipt.get('suites') != []:
            raise ValueError('prose-only PR reported an unexpected native selection')
        pr_pilot.verify(repo, plan, evidence / 'checked', pilot, admitted=None, foundation_report=foundation_report)
        return {**expected, 'suites': []}

    foundation = receipt.get('foundation')
    install_required = needs_install_witness(plan)
    if install_required or foundation == 'verified-release':
        manifest = json.loads(git(repo, 'show', f'{base}:docs/releases/preview-manifest.json'))
        released = manifest['archives'][native_linux_host()]
        installation = read('preview-install.json')
        cost = installation.get('cost', {})
        acquisition = installation.get('archive_acquisition', {})
        elapsed = acquisition.get('elapsed_seconds')
        if (installation.get('status') != 'PASS' or installation.get('activated') is not True or
                installation.get('seal_sha256') != released['seal_sha256'] or
                not matching_install_cost(cost, released) or acquisition.get('sha256') != released['sha256'] or
                acquisition.get('bytes') != released['bytes'] or acquisition.get('source') not in ('cache', 'download') or
                type(elapsed) is not float or not math.isfinite(elapsed) or elapsed < 0):
            raise ValueError('native PR installation does not match the base release archive')
        if install_required:
            witness = installation.get('witness', {})
            initial = witness.get('initial_install_cost', {})
            log_path = evidence / 'install-witness.log'
            log = artifact(evidence, log_path)
            if (cost.get('prefix_reused') is not True or witness.get('status') != 'PASS' or
                    not matching_install_cost(initial, released) or initial.get('prefix_reused') is not False or
                    witness.get('source') != 'native/main.objc3' or witness.get('log_sha256') != log['sha256'] or
                    log_path.read_bytes() != WITNESS_OUTPUT):
                raise ValueError('native PR is missing its matching installed relocation witness')

    planned = planned_development_identity(repo)
    if foundation == 'verified-release':
        admitted = {field: planned[key] for field, key in FOUNDATION_FIELDS.items()}
        admitted.update(seal_sha256=released['seal_sha256'], target=native_target())
        if plan['foundation_policy'] == 'rebuild-required' or receipt.get('foundation_sdk') != admitted:
            raise ValueError('native PR reused an ineligible foundation')
        suites = SOURCE_SUITES
    elif foundation == 'current-source-build':
        built = read('checked/execution/foundation/foundation.json')
        source = built.get('foundation_source', {})
        expected_source = {field: planned[key] for field, key in FOUNDATION_FIELDS.items()}
        effective = built.get('effective_configuration', {})
        tree = source.get('source_tree')
        if (receipt.get('foundation_sdk') is not None or built.get('source_identity') != planned['build_id'] or
                any(source.get(field) != value for field, value in expected_source.items()) or
                not isinstance(tree, str) or not re.fullmatch(r'[0-9a-f]{40}', tree) or
                effective.get('CLANG_OBJC3_PATCH_DIGEST') != planned['patch_digest'] or
                effective.get('CLANG_OBJC3_CONFIG_DIGEST') != planned['build_config_digest'] or
                built.get('cost_context', {}).get('build_config_digest') != planned['build_config_digest']):
            raise ValueError('native PR built a different foundation')
        suites = ('foundation', *SOURCE_SUITES)
    else:
        raise ValueError('native PR did not report its foundation source')
    if receipt.get('suites') != list(suites):
        raise ValueError('native PR receipt omitted a required suite')
    frontend_names = frontend_selection(plan) if foundation == 'verified-release' else None
    if receipt.get('frontend_ctests') != (sorted(frontend_names) if frontend_names is not None else None):
        raise ValueError('native PR receipt omitted the exact frontend CTest selection')
    pr_pilot.verify(repo, plan, evidence / 'checked', pilot, admitted=receipt.get('foundation_sdk'),
                    foundation_report=foundation_report)
    result = aggregate(plan, evidence / 'checked/execution', outcomes={'plan': 'success', 'native': execution},
                       tested=tested, suite_ids=suites, frontend_names=frontend_names)
    return {**result, 'implementation_pilot': pilot}


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('head', 'base', 'tested'):
        parser.add_argument('--' + name, required=True)
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument('--output', type=Path)
    mode.add_argument('--verify-evidence', type=Path)
    parser.add_argument('--sdk', type=Path)
    parser.add_argument('--execution')
    args = parser.parse_args()
    arguments = {'head': args.head, 'base': args.base, 'tested': args.tested}
    if args.verify_evidence is not None:
        if args.execution is None or args.sdk is not None:
            parser.error('evidence verification requires --execution and cannot take --sdk')
        result = verify(Path.cwd(), args.verify_evidence, execution=args.execution, **arguments)
    else:
        if args.execution is not None:
            parser.error('--execution is only for evidence verification')
        result = execute(Path.cwd(), args.output, sdk=args.sdk, **arguments)
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()

"""Execute the bounded native PR surface for the actual tested merge revision."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from scripts.objc3c_toolchain.binary_packages import write_json
from scripts.objc3c_toolchain.core import planned_development_identity
from scripts.objc3c_toolchain.linux_host import native_target
from scripts.objc3c_toolchain.package import verify_sdk
from .plan import from_checkout
from .runner import run

FOUNDATION_FIELDS = {
    'upstream_commit': 'upstream_commit',
    'patch_digest': 'patch_digest',
    'build_config_digest': 'build_config_digest',
    'source_build_id': 'build_id',
}
SOURCE_SUITES = ('runtime', 'frontend')


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


def execute(repo: Path, output: Path, *, head: str, base: str, tested: str,
            sdk: Path | None = None) -> dict:
    repo, output = repo.resolve(strict=True), output.resolve()
    if output.exists() or not output.is_relative_to(repo / 'out'):
        raise ValueError('native PR evidence requires fresh owned output')
    plan = from_checkout(repo, head=head, base=base, tested=tested)
    output.mkdir(parents=True)
    write_json(output / 'plan.json', plan)
    receipt = {'schema': 'objc3c.native-pr.v1', 'status': 'running',
               'revision': plan['revision'], 'plan_sha256': plan['sha256'],
               'classification': plan['classification']}
    try:
        if plan['classification'] == 'prose-only':
            receipt.update(status='PASS', foundation='not-required', suites=[])
            return receipt
        foundation = matching_foundation(repo, plan, sdk)
        suites = SOURCE_SUITES if foundation else ('foundation', *SOURCE_SUITES)
        receipt.update(foundation='verified-release' if foundation else 'current-source-build',
                       foundation_sdk=foundation, suites=list(suites))
        result = run(repo, plan, output / 'execution', suite_ids=suites,
                     product_sdk=sdk if foundation else None)
        if result.get('revision') != plan['revision'] or result.get('plan_sha256') != plan['sha256']:
            raise ValueError('native PR execution reported a different tested plan')
        if result['status'] != 'PASS' or tuple(row['id'] for row in result['suites']) != suites or any(
                row['status'] != 'PASS' or not row['executions'] for row in result['suites']):
            raise ValueError('native PR execution omitted a passing selected suite')
        if foundation and matching_foundation(repo, plan, sdk) != foundation:
            raise ValueError('PR foundation SDK changed during native validation')
        receipt['status'] = 'PASS'
        return receipt
    except BaseException as error:
        receipt.update(status='FAIL', error=f'{type(error).__name__}: {error}')
        raise
    finally:
        write_json(output / 'result.json', receipt)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('head', 'base', 'tested'):
        parser.add_argument('--' + name, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--sdk', type=Path)
    args = parser.parse_args()
    arguments = {'head': args.head, 'base': args.base, 'tested': args.tested}
    print(json.dumps(execute(Path.cwd(), args.output, sdk=args.sdk, **arguments), indent=2))


if __name__ == '__main__':
    main()

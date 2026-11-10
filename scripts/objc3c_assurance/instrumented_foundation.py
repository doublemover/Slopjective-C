"""Build one sanitizer compiler and restore it into independent consumers."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess

from scripts.objc3c_toolchain.binary_packages import installed_shared_library, write_json
from scripts.objc3c_toolchain.build import build_foundation
from scripts.objc3c_toolchain.core import build_plans, planned_development_identity
from scripts.objc3c_toolchain.instrumentation_profiles import select
from .plan import from_checkout, validate

PROFILE = 'compiler-asan-ubsan'
TRANSPORT_MARKER = 'objc3-instrumented-foundation.json'

CONSUMER_PRODUCTS = (
    'bin/clang',
    'bin/clang++',
    'bin/ld.lld',
    'bin/llvm-nm',
    'bin/llvm-ar',
    'lib/cmake/llvm/LLVMConfig.cmake',
    'lib/cmake/clang/ClangConfig.cmake',
    'lib/clang/23/include/stddef.h',
)


def digest(path: Path) -> str:
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def _require_checkout(repo: Path, plan: dict) -> None:
    if from_checkout(repo, **plan['revision'], periodic=plan['periodic']) != plan:
        raise ValueError('instrumented foundation plan differs from the exact checked-out revision')


def _portable_identity(repo: Path) -> dict:
    source = planned_development_identity(repo)
    profile = select(repo, PROFILE, 'linux-x86_64', source['build'], source,
                     {'tag': source['upstream_tag'], 'commit': source['upstream_commit']})
    stage = repo / 'out/toolchains/objc3-development/stage-linux-x86_64-compiler-asan-ubsan'
    return {'schema': 'objc3c.portable-foundation-identity.v1',
            'profile': PROFILE, 'source_build_id': profile['source_build_id'],
            'build_config_digest': profile['build_config_digest'],
            'upstream_commit': source['upstream_commit'],
            'patch_digest': source['patch_digest'],
            'paths': {'stage': str(stage)}, 'instrumentation': profile['recipe']}


def _consumer_products(stage: Path) -> list[Path]:
    """Return the exact stage surface consumed by the two downstream owners."""
    stage = stage.resolve(strict=True)
    paths, missing = [], []
    for relative in CONSUMER_PRODUCTS:
        path = stage / relative
        try:
            selected = path.resolve(strict=True)
            if not selected.is_relative_to(stage) or not selected.is_file():
                raise ValueError
            if relative.startswith('bin/') and not os.access(selected, os.X_OK):
                raise ValueError
            paths.append(selected)
        except (FileNotFoundError, OSError, RuntimeError, ValueError):
            missing.append(relative)
    for name in ('libLLVM.so', 'libclang-cpp.so'):
        try:
            paths.append(installed_shared_library(stage / 'lib', name))
        except (FileNotFoundError, OSError, RuntimeError, ValueError):
            missing.append('lib/'+name)
    if missing:
        raise ValueError('instrumented foundation omitted consumer prerequisites: '+', '.join(missing))
    return paths


def _identity(portable: dict, stage: Path) -> dict:
    paths = _consumer_products(stage)
    version = subprocess.check_output([stage / 'bin/clang', '--version'], text=True, timeout=30)
    if ('ObjC3-patches='+portable['patch_digest'] not in version or
            'ObjC3-config='+portable['build_config_digest'] not in version):
        raise ValueError('instrumented compiler version differs from its exact source/configuration')
    return {'source_build_id': portable['source_build_id'],
            'build_config_digest': portable['build_config_digest'],
            'profile': PROFILE, 'stage': str(stage),
            'files': {str(Path(path).relative_to(stage)): digest(Path(path)) for path in paths}}


def _retain_foundation_evidence(repo: Path, configured: dict | None, evidence: Path) -> None:
    build = (Path(configured['paths']['build']) if configured is not None else
             repo / 'out/toolchains/objc3-development/build-linux-x86_64-compiler-asan-ubsan')
    receipt = build / 'foundation-build-report.json'
    if not receipt.is_file():
        return
    shutil.copyfile(receipt, evidence / 'foundation.json')
    report = json.loads(receipt.read_text())
    for index, phase in enumerate(report.get('phases', [])):
        log = Path(phase.get('log', ''))
        if log.is_file():
            shutil.copyfile(log, evidence / ('phase-'+str(index)+'.log'))


def produce(repo: Path, plan: dict, output: Path, bundle: Path) -> dict:
    validate(plan);_require_checkout(repo, plan)
    output, bundle = output.resolve(), bundle.resolve()
    if (output.exists() or bundle.exists() or not output.is_relative_to(repo / 'out') or
            not bundle.is_relative_to(repo / 'out')):
        raise ValueError('instrumented foundation producer requires fresh owned outputs')
    output.mkdir(parents=True)
    evidence = output / 'foundation';evidence.mkdir()
    configured = None
    report = {'schema': 'objc3c.instrumented-foundation-bundle.v1', 'status': 'RUNNING',
              'revision': plan['revision'], 'plan_sha256': plan['sha256']}
    write_json(output / 'producer.json', report)
    try:
        foundation = build_foundation(repo, host='linux-x86_64', fetch=True, stage=True,
                                      instrumentation_profile=PROFILE, test_results=evidence)
        configured = build_plans(repo, host='linux-x86_64', instrumentation_profile=PROFILE)['configure']
        portable = _portable_identity(repo)
        stage = Path(configured['paths']['stage']).resolve(strict=True)
        if (configured['source_build_id'] != portable['source_build_id'] or
                configured['build_config_digest'] != portable['build_config_digest'] or
                str(stage) != portable['paths']['stage'] or
                configured['instrumentation'] != portable['instrumentation']):
            raise ValueError('instrumented producer plan differs from its portable identity')
        if foundation.get('status') != 'passed':
            raise ValueError('instrumented foundation producer did not pass')
        identity = _identity(portable, stage)
        relative = stage.relative_to(repo)
        bundle.parent.mkdir(parents=True, exist_ok=True)
        subprocess.run(['tar', '-I', 'zstd -T2 -3', '-cf', bundle, '-C', repo, str(relative)], check=True)
        report.update(status='PASS', identity=identity, foundation_identity=portable,
                      bundle={'sha256': digest(bundle), 'bytes': bundle.stat().st_size,
                              'root': str(relative)})
        write_json(output / 'producer.json', report)
        write_json(output / 'foundation-identity.json', portable)
        return report
    except BaseException as error:
        report.update(status='FAIL', error=f'{type(error).__name__}: {error}')
        write_json(output / 'producer.json', report)
        raise
    finally:
        _retain_foundation_evidence(repo, configured, evidence)


def restore(repo: Path, plan: dict, producer: Path, bundle: Path) -> dict:
    validate(plan);_require_checkout(repo, plan)
    report = json.loads(producer.resolve(strict=True).read_text())
    bundle = bundle.resolve(strict=True)
    portable = _portable_identity(repo)
    stage = Path(portable['paths']['stage'])
    expected = {'source_build_id': portable['source_build_id'],
                'build_config_digest': portable['build_config_digest'], 'profile': PROFILE}
    if (report.get('schema') != 'objc3c.instrumented-foundation-bundle.v1' or
            report.get('status') != 'PASS' or report.get('revision') != plan['revision'] or
            report.get('plan_sha256') != plan['sha256'] or
            report.get('foundation_identity') != portable or
            any(report.get('identity', {}).get(key) != value for key, value in expected.items()) or
            report.get('bundle', {}).get('sha256') != digest(bundle) or
            report['bundle'].get('bytes') != bundle.stat().st_size or stage.exists()):
        raise ValueError('instrumented foundation bundle is stale, substituted or already restored')
    root = report['bundle']['root'].rstrip('/')
    members = subprocess.check_output(['tar', '-I', 'zstd', '-tf', bundle], text=True).splitlines()
    if (not members or any(name.startswith(('/', '../')) or '/..' in name or
                           not (name.rstrip('/') == root or name.startswith(root+'/'))
                           for name in members)):
        raise ValueError('instrumented foundation bundle contains an escaped or unowned member')
    if root+'/'+TRANSPORT_MARKER in (name.rstrip('/') for name in members):
        raise ValueError('instrumented foundation bundle contains its unbound transport marker')
    subprocess.run(['tar', '-I', 'zstd', '-xf', bundle, '-C', repo], check=True)
    if _identity(portable, stage) != report['identity']:
        raise ValueError('restored instrumented foundation differs from its producer identity')
    marker = stage / TRANSPORT_MARKER
    write_json(marker, report)
    return report


def admit(repo: Path, receipt: Path, identity_file: Path | None = None) -> Path:
    report = json.loads(receipt.resolve(strict=True).read_text())
    portable = _portable_identity(repo)
    stage = Path(portable['paths']['stage']).resolve(strict=True)
    if identity_file is not None and json.loads(identity_file.resolve(strict=True).read_text()) != portable:
        raise ValueError('restored instrumented foundation portable identity was substituted')
    if (report.get('schema') != 'objc3c.instrumented-foundation-bundle.v1' or
            report.get('status') != 'PASS' or report.get('foundation_identity') != portable or
            _identity(portable, stage) != report.get('identity')):
        raise ValueError('restored instrumented foundation admission failed')
    return stage


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest='command', required=True)
    common = argparse.ArgumentParser(add_help=False)
    common.add_argument('--repo', type=Path, default=Path.cwd())
    common.add_argument('--plan', type=Path, required=True)
    producer = commands.add_parser('produce', parents=[common])
    producer.add_argument('--output', type=Path, required=True);producer.add_argument('--bundle', type=Path, required=True)
    restored = commands.add_parser('restore', parents=[common])
    restored.add_argument('--producer', type=Path, required=True);restored.add_argument('--bundle', type=Path, required=True)
    args = parser.parse_args(argv);repo = args.repo.resolve(strict=True);plan = json.loads(args.plan.read_text())
    result = (produce(repo, plan, args.output, args.bundle) if args.command == 'produce' else
              restore(repo, plan, args.producer, args.bundle))
    print(json.dumps({'status': result['status'], 'profile': PROFILE}, indent=2))


if __name__ == '__main__':
    main()

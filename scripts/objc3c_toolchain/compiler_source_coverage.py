"""Measure patched Clang/LLVM source coverage from the isolated coverage profile."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import tempfile
import time

from .binary_packages import digest, write_json
from .core import build_plans
from .source_coverage import binary_ids

PROFILE = 'compiler-source-coverage'
FLAGS = ('-fprofile-instr-generate', '-fcoverage-mapping', '-fprofile-update=atomic')
PATCH_FILE = re.compile(r'^\+\+\+ b/((?:clang|llvm)/\S+)$')
HUNK = re.compile(r'^@@ -\d+(?:,\d+)? \+(\d+)(?:,(\d+))? @@')


def require_binary_ids(observed: set[str], *, required: set[str], allowed: set[str], kind: str) -> None:
    """Require all executed mains while admitting only covered loaded modules."""
    if not required <= observed <= allowed:
        raise ValueError(f'compiler {kind} differ from the executed binary identities')


def patched_inventory(repo: Path, source: Path) -> tuple[dict, dict]:
    """Return immutable patched sources and new-line ranges from the patch stack."""
    files, ranges = {}, {}
    for patch in sorted((repo / 'toolchains/llvm/patches').glob('*.patch')):
        current = None
        for line in patch.read_text(errors='strict').splitlines():
            matched = PATCH_FILE.match(line)
            if matched:
                current = matched[1]
                candidate = source / current
                if (candidate.is_file() and candidate.suffix in ('.c', '.cc', '.cpp', '.h', '.inc') and
                        current.startswith(('clang/lib/', 'clang/include/'))):
                    files[current] = digest(candidate)
                    ranges.setdefault(current, [])
                else:
                    current = None
                continue
            hunk = HUNK.match(line)
            if current and hunk:
                start, count = int(hunk[1]), int(hunk[2] or 1)
                if count:
                    ranges[current].append([start, start + count - 1])
    if not files or not any(name.endswith('.cpp') for name in files):
        raise ValueError('patched compiler source inventory is empty')
    ranges = {name: merged_ranges(value) for name, value in ranges.items()}
    return dict(sorted(files.items())), dict(sorted(ranges.items()))


def merged_ranges(ranges):
    result = []
    for start, end in sorted(ranges):
        if result and start <= result[-1][1] + 1:
            result[-1][1] = max(result[-1][1], end)
        else:
            result.append([start, end])
    return result


def compile_inventory(build: Path, source: Path, eligible: dict) -> list[dict]:
    """Bind every patched implementation TU to an actual covered object command."""
    rows = json.loads((build / 'compile_commands.json').read_text())
    records = []
    implementations = {str((source / name).resolve()): name for name in eligible if name.endswith(('.c', '.cc', '.cpp'))}
    seen = set()
    for row in rows:
        unit = str(Path(row['file']).resolve())
        name = implementations.get(unit)
        if not name:
            continue
        argv = row.get('arguments') or shlex.split(row['command'])
        output = (Path(row['directory']) / (row.get('output') or argv[argv.index('-o') + 1])).resolve()
        if (not output.is_relative_to(build) or not output.is_file() or
                not set(FLAGS) <= set(argv) or
                any(arg.startswith(('-fno-profile-instr-generate', '-fno-coverage-mapping')) for arg in argv)):
            raise ValueError('patched compiler object lacks source coverage instrumentation: ' + name)
        records.append({'source': name, 'source_sha256': eligible[name], 'object': str(output),
                        'sha256': digest(output), 'argv': argv})
        seen.add(name)
    missing = sorted(set(implementations.values()) - seen)
    if missing:
        raise ValueError('patched compiler implementations disappeared from the compiled denominator: ' + ', '.join(missing))
    return records


def read_export(path: Path, *, source: Path, eligible: dict, changed: dict) -> dict:
    if path.stat().st_size > 256 * 1024**2:
        raise ValueError('patched compiler coverage export exceeded its bound')
    document = json.loads(path.read_text())
    if document.get('type') != 'llvm.coverage.json.export' or not document.get('data'):
        raise ValueError('missing patched compiler coverage export')
    result = {name: {'state': 'no-emitted-mapping', 'summary': None,
                     'changed_lines': changed[name]} for name in eligible}
    for data in document['data']:
        for row in data['files']:
            filename = Path(row['filename']).resolve()
            if not filename.is_relative_to(source):
                continue
            name = filename.relative_to(source).as_posix()
            if name not in result:
                continue
            if result[name]['summary'] is not None:
                raise ValueError('duplicate patched source in coverage export')
            summary = row['summary']
            for key in ('functions', 'regions', 'branches', 'lines'):
                value = summary[key]
                if (type(value['count']) is not int or type(value['covered']) is not int or
                        not 0 <= value['covered'] <= value['count']):
                    raise ValueError('invalid patched compiler coverage count')
            selected = []
            for segment in row.get('segments', []):
                if len(segment) < 5 or not segment[3]:
                    continue
                line, count = segment[0], segment[2]
                if any(start <= line <= end for start, end in changed[name]):
                    selected.append({'line': line, 'count': count, 'region_entry': bool(segment[4])})
            result[name] = {'state': 'executed' if summary['regions']['covered'] else 'zero-hit',
                'summary': summary, 'changed_lines': changed[name],
                'changed_region_entries': len([row for row in selected if row['region_entry']]),
                'covered_changed_region_entries': len([row for row in selected if row['region_entry'] and row['count'] > 0]),
                'uncovered_changed_lines': sorted({row['line'] for row in selected if row['count'] == 0})}
    return result


def compiler_raw_profiles(root: Path) -> list[Path]:
    """Admit bounded online-merged profiles from the large compiler graph."""
    paths = sorted(root.glob('*.profraw'))
    total = sum(path.stat().st_size for path in paths)
    if (not paths or len(paths) > 64 or total > 2 * 1024**3 or
            any(path.is_symlink() or not path.is_file() or not path.stat().st_size or
                path.stat().st_size > 512 * 1024**2 for path in paths) or
            any(path.suffix != '.profraw' for path in root.iterdir())):
        raise ValueError('missing, escaped, empty or excessive compiler raw coverage profiles')
    return paths


def qualify(repo: Path, output: Path, *, jobs=2) -> dict:
    repo, output = repo.resolve(strict=True), output.resolve()
    if output.exists() or not output.is_relative_to(repo / 'out') or jobs not in (1, 2, 3, 4):
        raise ValueError('compiler source coverage requires fresh owned output and bounded jobs')
    plan = build_plans(repo, host='linux-x86_64', instrumentation_profile=PROFILE)['configure']
    build, stage, source = (Path(plan['paths'][key]).resolve(strict=True) for key in ('build', 'stage', 'source'))
    foundation = json.loads((build / 'foundation-build-report.json').read_text())
    if (foundation.get('status') != 'passed' or foundation.get('source_identity') != plan['source_build_id'] or
            {row['phase'] for row in foundation.get('phases', [])} != {'configure', 'build', 'test', 'stage'} or
            any(row.get('exit_code') != 0 for row in foundation['phases'])):
        raise ValueError('compiler source coverage requires a complete matching profile build/test/stage')
    eligible, changed = patched_inventory(repo, source)
    output.mkdir(parents=True);(output / 'logs').mkdir()
    profile_root = Path(tempfile.mkdtemp(prefix='objc3-compiler-coverage-'))
    if profile_root.parent != Path('/tmp') or shutil.disk_usage(profile_root).free < 3 * 1024**3:
        shutil.rmtree(profile_root)
        raise ValueError('compiler source coverage needs 3 GiB of temporary profile storage')
    started = time.monotonic()
    tools = {name: stage / 'bin' / name for name in ('llvm-cov', 'llvm-profdata', 'llvm-readobj')}
    test_binaries = [build / relative for relative in plan['instrumentation']['test_binaries']]
    binaries = [stage / 'bin/clang', stage / 'lib/libclang-cpp.so', stage / 'lib/libLLVM.so',
                *test_binaries]
    report = {'schema': 'objc3c.compiler-source-coverage.v1', 'status': 'running',
        'profile': PROFILE, 'source_build_id': plan['source_build_id'],
        'build_config_digest': plan['build_config_digest'],
        'revision': subprocess.check_output(['git', '-C', repo, 'rev-parse', 'HEAD'], text=True).strip(),
        'eligible': eligible, 'eligible_sha256': __import__('hashlib').sha256(
            json.dumps(eligible, sort_keys=True, separators=(',', ':')).encode()).hexdigest(),
        'changed_ranges': changed, 'commands': [], 'suites': [], 'objects': [],
        'temporary_profile_root': str(profile_root),
        'profile_storage': 'Raw compiler profiles are isolated on /tmp and reclaimed after indexing.',
        'scope': 'Patched Clang whole-file plus patch-hunk views from the isolated compiler source-coverage profile. First-party façade/runtime, semantic coverage and fuzzer feedback remain separately labeled.',
        'exclusions': {'clang-tools-extra': 'Not linked into the current compiler/frontend qualification profile.',
                       'unpatched-upstream': 'Available in raw mappings but excluded from the patch-owned denominator.',
                       'generated-tablegen': 'Generated outputs are inputs to compiled owners, not patch-owned source files.'}}
    def save():
        report['seconds'] = round(time.monotonic() - started, 6)
        write_json(output / 'qualification.json', report)
    env = {key: value for key, value in os.environ.items() if key not in
           ('ASAN_OPTIONS', 'LSAN_OPTIONS', 'UBSAN_OPTIONS', 'TSAN_OPTIONS', 'LLVM_PROFILE_FILE')}
    env['LD_LIBRARY_PATH'] = str(stage / 'lib')
    env['PATH'] = str(stage / 'bin') + os.pathsep + env.get('PATH', '')
    env['LLVM_PROFILE_FILE'] = os.devnull
    env['PYTHONDONTWRITEBYTECODE'] = '1'
    def run(name, argv, *, profile=True, timeout=900, stdout=None):
        if any(row['name'] == name for row in report['commands']):
            raise ValueError('duplicate compiler coverage command')
        directory = profile_root / name
        if profile: directory.mkdir()
        case_env = {**env, **({'LLVM_PROFILE_FILE': str(directory / '%b-%8m.profraw')} if profile else {})}
        log = Path(stdout) if stdout else output / 'logs' / (name+'.log')
        stderr_log = output / 'logs' / (name+'.stderr.log')
        started_command = time.monotonic()
        with log.open('w') as stream, stderr_log.open('w') as errors:
            result = subprocess.run(list(map(str, argv)), cwd=repo, env=case_env,
                stdout=stream, stderr=errors if stdout else subprocess.STDOUT,
                timeout=timeout)
        row = {'name': name, 'argv': list(map(str, argv)), 'exit_code': result.returncode,
               'seconds': round(time.monotonic()-started_command, 6), 'log': str(log), 'sha256': digest(log),
               'stderr': str(stderr_log), 'stderr_sha256': digest(stderr_log)}
        report['commands'].append(row);save()
        if result.returncode:
            raise ValueError('compiler source coverage command failed: '+name)
        return log
    try:
        for name, tool in tools.items():
            if not tool.is_file() or '23.1.3' not in run(name+'-version', [tool, '--version'], profile=False).read_text():
                raise ValueError('coverage tool differs from the isolated compiler profile')
        binary_identity = {}
        for index, binary in enumerate(binaries):
            notes = run('binary-notes-'+str(index), [tools['llvm-readobj'], '--notes', binary], profile=False).read_text()
            values = re.findall(r'Build ID: ([0-9a-f]{40})', notes)
            if len(values) != 1: raise ValueError('covered compiler binary lacks one build identity')
            binary_identity[str(binary)] = values[0]
        expected_ids = set(binary_identity.values())
        expected_profile_ids = {binary_identity[str(binary)] for binary in [binaries[0], *test_binaries]}
        execution_binary_identity = dict(binary_identity)
        targets = [target for group in plan['instrumentation']['build_target_groups']
                   for target in group['targets']]
        execution_candidates = sorted({(build / 'bin' / target).resolve()
                                       for target in targets
                                       if (build / 'bin' / target).is_file()})
        for index, binary in enumerate(execution_candidates):
            if str(binary) in execution_binary_identity: continue
            notes = run('execution-binary-notes-'+str(index),
                        [tools['llvm-readobj'], '--notes', binary], profile=False).read_text()
            values = re.findall(r'Build ID: ([0-9a-f]{40})', notes)
            if len(values) != 1: raise ValueError('instrumented support binary lacks one build identity')
            execution_binary_identity[str(binary)] = values[0]
        allowed_profile_ids = set(execution_binary_identity.values())
        report['objects'] = compile_inventory(build, source, eligible)
        before = {row['object']: row['sha256'] for row in report['objects']}
        lit_json = output / 'lit-results.json'
        lit_log = run('objc3-lit', [build / 'bin/llvm-lit', '-sv', '--show-unsupported', '-j', str(min(jobs, 4)),
            '--output', lit_json, source / 'clang/test/ObjC3'], timeout=1800)
        lit = json.loads(lit_json.read_text())
        outcomes = {key: len([row for row in lit.get('tests', []) if row.get('code') == key])
                    for key in ('PASS', 'XFAIL', 'UNSUPPORTED')}
        if not lit.get('tests') or any(row.get('code') not in outcomes for row in lit['tests']) or not outcomes['PASS']:
            raise ValueError('covered ObjC3 lit suite is empty or failed')
        report['suites'].append({'name': 'ObjC3-lit', 'status': 'PASS', 'outcomes': outcomes,
                                 'log_sha256': digest(lit_log), 'result_sha256': digest(lit_json)})
        for binary in test_binaries:
            result = output / (binary.name+'.xml')
            run('unit-'+binary.name, [binary, '--gtest_output=xml:'+str(result)])
            if not result.is_file() or '<failure' in result.read_text():
                raise ValueError('covered compiler unit suite failed or omitted JUnit')
            report['suites'].append({'name': binary.name, 'status': 'PASS', 'result_sha256': digest(result)})
        profiles = []
        for directory in sorted(profile_root.iterdir()):
            profiles.extend(compiler_raw_profiles(directory))
        filename_ids = set()
        for profile in profiles:
            matched = re.fullmatch(r'([0-9a-f]{40})-.+\.profraw', profile.name)
            if not matched: raise ValueError('compiler raw profile omitted its executable build identity')
            filename_ids.add(matched[1])
        report['raw_profile_binary_ids'] = sorted(filename_ids);save()
        require_binary_ids(filename_ids, required=expected_profile_ids, allowed=allowed_profile_ids,
                           kind='raw profiles')
        merged = output / 'compiler.profdata'
        run('merge', [tools['llvm-profdata'], 'merge', '--sparse', *profiles, '-o', merged], profile=False)
        identifiers = run('profile-binary-ids', [tools['llvm-profdata'], 'show', '--binary-ids', merged], profile=False).read_text()
        observed_ids = binary_ids(identifiers)
        require_binary_ids(observed_ids, required=expected_profile_ids, allowed=allowed_profile_ids,
                           kind='merged profile')
        export = output / 'coverage-export.json'
        run('export', [tools['llvm-cov'], 'export', binaries[0],
            *('--object='+str(binary) for binary in binaries[1:]),
            '--check-binary-ids', '--instr-profile='+str(merged),
            '--skip-expansions', '--compilation-dir='+str(build),
            *(source / name for name in eligible)], profile=False, timeout=1800, stdout=export)
        report['source_coverage'] = read_export(export, source=source, eligible=eligible, changed=changed)
        if not any(row['state'] == 'executed' for row in report['source_coverage'].values()):
            raise ValueError('patched compiler coverage has no executed source')
        if any(digest(Path(path)) != value for path, value in before.items()):
            raise ValueError('compiler coverage object changed during execution')
        report['raw_profiles'] = {'count': len(profiles), 'bytes': sum(path.stat().st_size for path in profiles),
                                  'sha256': {path.relative_to(profile_root).as_posix(): digest(path)
                                             for path in profiles}}
        for raw in profiles: raw.unlink()
        report.update(status='PASS', profile_sha256=digest(merged), binary_ids=sorted(observed_ids),
                      covered_binary_ids=binary_identity,
                      executed_binary_ids=execution_binary_identity,
                      artifact_bytes=sum(path.stat().st_size for path in output.rglob('*') if path.is_file()))
    except BaseException as error:
        report.update(status='FAIL', error=str(error));raise
    finally:
        if profile_root.exists(): shutil.rmtree(profile_root)
        report['temporary_profile_root_reclaimed'] = not profile_root.exists()
        save()
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--jobs', type=int, choices=(1, 2, 3, 4), default=2)
    result = qualify(**vars(parser.parse_args()))
    print(json.dumps({'status': result['status'], 'eligible': len(result['eligible']),
                      'suites': len(result['suites'])}))


if __name__ == '__main__': main()

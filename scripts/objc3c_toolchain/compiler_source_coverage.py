"""Measure patched Clang/LLVM source coverage from the isolated coverage profile."""
from __future__ import annotations

from .linux_host import native_linux_host

import argparse
from collections import Counter
import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import tempfile
import time
from zipfile import ZipFile

from .binary_packages import digest, installed_shared_library, write_json
from .core import build_plans
from .source_coverage import binary_ids, coverage_object_sections, coverage_section_command, reclaim_indexed_export

PROFILE = 'compiler-source-coverage'
SCHEMA = 'objc3c.compiler-source-coverage.v1'
FLAGS = ('-fprofile-instr-generate', '-fcoverage-mapping', '-fprofile-update=atomic')
EXPORT_FLAGS = ('--check-binary-ids', '--skip-expansions', '--skip-functions')
COVERAGE_KINDS = ('functions', 'regions', 'branches', 'lines')
RECEIPT_LIMIT = 64 * 1024**2
PATCH_FILE = re.compile(r'^\+\+\+ b/((?:clang|llvm)/\S+)$')
HUNK = re.compile(r'^@@ -(\d+)(?:,(\d+))? \+(\d+)(?:,(\d+))? @@')


def selected_tools(repo: Path) -> tuple[dict, dict, dict]:
    """Reuse the completed coverage compiler for source-feedback experiments."""
    from .build import verify_effective_configuration
    from .llvm_tools import verify_installation
    from .sanitizer_policy import strict_environment

    plans = build_plans(repo, host=native_linux_host(), instrumentation_profile=PROFILE)
    plan = plans['configure']
    build, stage = (Path(plan['paths'][key]).resolve(strict=True) for key in ('build', 'stage'))
    receipt = json.loads((build / 'foundation-build-report.json').read_text())
    phases = receipt.get('phases', [])
    expected = {phase: 1 + len(plans[phase].get('follow_up_commands', []))
                for phase in ('configure', 'build', 'test', 'stage')}
    if (receipt.get('status') != 'passed' or receipt.get('source_identity') != plan['source_build_id'] or
            Counter(row.get('phase') for row in phases) != expected or
            any(row.get('exit_code') != 0 for row in phases)):
        raise ValueError('source feedback requires a complete matching coverage foundation')
    # Historical phase-plan IDs also include then-existing consumer directories
    # and runner resource observations. Source/configuration identity and the
    # effective CMake state bind this reuse; recomputing those historical IDs
    # after staging would reject a valid freshly completed build.
    verify_effective_configuration(plan)
    tools = {name: stage / 'bin' / name for name in
             ('clang', 'clang++', 'ld.lld', 'llvm-cov', 'llvm-profdata', 'llvm-readobj')}
    for name, installed in tools.items():
        verify_installation((build / 'bin' / name).resolve(strict=True), installed, build)
    for name in ('libLLVM.so', 'libclang-cpp.so'):
        verify_installation(build / 'lib' / name, installed_shared_library(stage / 'lib', name), build)
    bootstrap = Path(plan['host_environment']['bootstrap_root'])
    environment = strict_environment(tools['clang'], bootstrap / 'bin/llvm-symbolizer')
    environment['LLVM_PROFILE_FILE'] = os.devnull
    return plan, tools, environment


def require_binary_ids(observed: set[str], *, required: set[str], allowed: set[str], kind: str) -> None:
    """Require all executed mains while admitting only covered loaded modules."""
    if not required <= observed <= allowed:
        raise ValueError(f'compiler {kind} differ from the executed binary identities')


def unique_binary_paths(paths: list[Path], identities: dict[str, str]) -> list[Path]:
    """Select one coverage object for every admitted ELF build identity."""
    result, seen = [], set()
    for path in paths:
        identity = identities.get(str(path))
        if not identity: raise ValueError('coverage export binary lacks an admitted identity')
        if identity in seen: continue
        seen.add(identity);result.append(path)
    return result


def select_coverage_profiles(profile_ids: dict[Path, str], expected_ids: set[str],
                             *, suite: str) -> tuple[list[Path], list[Path]]:
    """Keep covered implementation modules separate from support-tool counters."""
    selected = [path for path, identity in profile_ids.items()
                if path.parent.name == suite and identity in expected_ids]
    selected_ids = {profile_ids[path] for path in selected}
    require_binary_ids(selected_ids, required=expected_ids, allowed=expected_ids,
                       kind='selected coverage profiles')
    return selected, [path for path in profile_ids if path not in selected]


def remap_patch_ranges(previous: list[list[int]], hunks: list[tuple[int, int, int, int]]) -> list[list[int]]:
    """Carry earlier patch ownership into the final stack's line coordinates."""
    result = []
    for start, end in previous:
        cursor, shift = start, 0
        for old_start, old_count, new_start, new_count in hunks:
            # A zero-count old hunk inserts after its line-number anchor.
            boundary = old_start if old_count else old_start + 1
            if cursor < boundary:
                before = min(end, boundary - 1)
                if cursor <= before:
                    result.append([cursor + shift, before + shift])
                cursor = max(cursor, before + 1)
            if cursor > end:
                break
            # Covered old hunk lines are superseded by this hunk's complete
            # new range, including its context. Deleted lines cease to exist.
            cursor = max(cursor, boundary + old_count)
            shift += new_count - old_count
        if cursor <= end:
            result.append([cursor + shift, end + shift])
    result.extend([start, start + count - 1] for _, _, start, count in hunks if count)
    return merged_ranges(result)


def patched_inventory(repo: Path, source: Path) -> tuple[dict, dict]:
    """Bind patched sources and hunk ranges in final, ordered-stack coordinates."""
    files, ranges = {}, {}
    for patch in sorted((repo / 'toolchains/llvm/patches').glob('*.patch')):
        current, hunks = None, []
        def finish():
            if current:
                ranges[current] = remap_patch_ranges(ranges.get(current, []), hunks)
        for line in patch.read_text(errors='strict').splitlines():
            if line.startswith('+++ '):
                finish()
                current, hunks = None, []
                matched = PATCH_FILE.match(line)
                if matched:
                    name = matched[1]
                    candidate = source / name
                    if (candidate.is_file() and candidate.suffix in ('.c', '.cc', '.cpp', '.h', '.inc') and
                            name.startswith(('clang/lib/', 'clang/include/'))):
                        current = name
                        files[current] = digest(candidate)
                continue
            hunk = HUNK.match(line)
            if current and hunk:
                hunks.append(tuple(int(value or 1) for value in hunk.groups()))
        finish()
    if not files or not any(name.endswith('.cpp') for name in files):
        raise ValueError('patched compiler source inventory is empty')
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
    seen, outputs = set(), set()
    for row in rows:
        unit = str(Path(row['file']).resolve())
        name = implementations.get(unit)
        if not name:
            continue
        argv = row.get('arguments') or shlex.split(row['command'])
        output = (Path(row['directory']) / (row.get('output') or argv[argv.index('-o') + 1])).resolve()
        if (output in outputs or not output.is_relative_to(build) or not output.is_file() or
                not set(FLAGS) <= set(argv) or
                any(arg.startswith(('-fno-profile-instr-generate', '-fno-coverage-mapping')) for arg in argv)):
            raise ValueError('patched compiler object lacks source coverage instrumentation: ' + name)
        records.append({'source': name, 'source_sha256': eligible[name], 'object': str(output),
                        'sha256': digest(output), 'argv': argv})
        seen.add(name)
        outputs.add(output)
    missing = sorted(set(implementations.values()) - seen)
    if missing:
        raise ValueError('patched compiler implementations disappeared from the compiled denominator: ' + ', '.join(missing))
    return records


def inspect_objects(records, read_sections) -> None:
    """Bind recorded compilation rows to the instrumentation in their actual ELF bytes."""
    for index, row in enumerate(records):
        try:
            row['coverage_sections'] = coverage_object_sections(read_sections(index, row['object']))
        except ValueError as error:
            raise ValueError(f"{row['source']}: {error}") from error


def changed_branches(branches: list, ranges: list[list[int]]) -> dict:
    """Keep individual true/false counts for decisions intersecting maintained hunks."""
    decisions = []
    for branch in branches:
        if (not isinstance(branch, list) or len(branch) != 9 or
                any(type(value) is not int or value < 0 for value in branch) or
                any(not value for value in branch[:4]) or tuple(branch[:2]) > tuple(branch[2:4])):
            raise ValueError('invalid patched compiler coverage branch')
        start, column, end, end_column, taken, not_taken, *_ = branch
        if any(start <= last and first <= end for first, last in ranges):
            decisions.append({'start': [start, column], 'end': [end, end_column],
                              'true_count': taken, 'false_count': not_taken})
    return {'arms': len(decisions) * 2,
            'covered_arms': sum((row['true_count'] > 0) + (row['false_count'] > 0) for row in decisions),
            'decisions': decisions}


def read_export(path: Path, *, source: Path, eligible: dict, changed: dict) -> dict:
    if path.stat().st_size > 256 * 1024**2:
        raise ValueError('patched compiler coverage export exceeded its bound')
    document = json.loads(path.read_text())
    if document.get('type') != 'llvm.coverage.json.export' or not document.get('data'):
        raise ValueError('missing patched compiler coverage export')
    result = {name: {'state': 'no-emitted-mapping', 'summary': None, 'changed_lines': changed[name],
                     'changed_branches': None} for name in eligible}
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
            for key in COVERAGE_KINDS:
                value = summary[key]
                if (type(value['count']) is not int or type(value['covered']) is not int or
                        not 0 <= value['covered'] <= value['count']):
                    raise ValueError('invalid patched compiler coverage count')
            selected = []
            for segment in row.get('segments', []):
                if len(segment) < 5 or not segment[3] or (len(segment) > 5 and segment[5]):
                    continue
                line, count = segment[0], segment[2]
                if any(start <= line <= end for start, end in changed[name]):
                    selected.append({'line': line, 'column': segment[1], 'count': count, 'region_entry': bool(segment[4])})
            entries = [row for row in selected if row['region_entry']]
            result[name] = {'state': 'executed' if summary['regions']['covered'] else 'zero-hit',
                'summary': summary, 'changed_lines': changed[name],
                'changed_branches': changed_branches(row.get('branches', []), changed[name]),
                'changed_region_entries': len(entries),
                'covered_changed_region_entries': sum(row['count'] > 0 for row in entries),
                'changed_region_mapping_sha256': hashlib.sha256(json.dumps(
                    [(row['line'], row['column']) for row in entries], separators=(',', ':')).encode()).hexdigest(),
                'uncovered_changed_lines': sorted({row['line'] for row in selected if row['count'] == 0})}
    return result


def coverage_baseline(repo: Path, path: Path) -> tuple[dict, dict]:
    """Read a bounded receipt or the exact artifact from a reviewed release."""
    path = path.resolve(strict=True)
    if not path.is_file() or path.stat().st_size > RECEIPT_LIMIT:
        raise ValueError('compiler coverage baseline must be a bounded regular file')
    signature = digest(path)
    if path.suffix != '.zip':
        if not path.is_relative_to(repo / 'out'):
            raise ValueError('local compiler coverage baseline must be stored under out')
        return json.loads(path.read_text()), {'sha256': signature}

    policy = json.loads((repo / 'toolchains/llvm/source-coverage-baselines.json').read_text())
    matches = [(host, row) for host, row in policy['hosts'].items() if row['archive_sha256'] == signature]
    if len(matches) != 1 or policy.get('schema') != 'objc3c.reviewed-source-coverage-baselines.v1':
        raise ValueError('coverage baseline ZIP differs from the reviewed release artifact')
    host, reviewed = matches[0]
    name = policy['compiler_receipt']
    with ZipFile(path) as archive:
        members = [entry for entry in archive.infolist() if entry.filename == name]
        if len(members) != 1 or members[0].file_size > RECEIPT_LIMIT:
            raise ValueError('reviewed coverage archive omitted its bounded compiler receipt')
        raw = archive.read(members[0])
    prior = json.loads(raw)
    if (hashlib.sha256(raw).hexdigest() != reviewed['receipt_sha256'] or
            prior.get('revision') != policy['revision'] or
            set(prior.get('covered_binary_ids', {}).values()) != {reviewed['shared_clang_build_id']} or
            prior.get('host', host) != host):
        raise ValueError('reviewed coverage archive contains a different release receipt')
    return {**prior, 'host': host}, {'sha256': signature, **reviewed}


def covered_arms(decisions: list[dict]) -> Counter:
    """Count covered decision arms at stable source coordinates."""
    return Counter(
        (tuple(row['start']), tuple(row['end']), arm)
        for row in decisions
        for arm in ('true', 'false')
        if row[arm + '_count'] > 0
    )


def arm_changes(delta: Counter) -> list[dict]:
    return [
        {'start': list(start), 'end': list(end), 'arm': arm, 'occurrences': count}
        for (start, end, arm), count in sorted(delta.items())
    ]


def compare_coverage(previous: dict, current: dict) -> dict:
    """Compare comparable source units and expose every changed denominator."""
    if (previous.get('schema') != SCHEMA or previous.get('status') != 'PASS' or
            previous.get('profile') != PROFILE or not previous.get('eligible') or
            set(previous['eligible']) != set(previous.get('source_coverage', {})) or
            not re.fullmatch(r'[0-9a-f]{40}', previous.get('revision', '')) or
            previous.get('coverage_owner') != 'shared-clang-cpp-patched-implementation' or
            (previous.get('host') and previous['host'] != current['host'])):
        raise ValueError('coverage baseline must be a complete patched-compiler report')
    before, after = previous['eligible'], current['eligible']
    result = {'baseline_revision': previous['revision'], 'current_revision': current['revision'],
              'baseline_host': previous.get('host'), 'current_host': current['host'],
              'added_files': sorted(after.keys() - before.keys()),
              'removed_files': sorted(before.keys() - after.keys()),
              'changed_files': {}, 'comparable_files': [], 'hit_deltas': {},
              'lost_branch_arms': {}, 'gained_branch_arms': {},
              'missing_branch_baseline': []}
    for name in sorted(before.keys() & after.keys()):
        old, new = previous['source_coverage'][name], current['source_coverage'][name]
        old_summary, new_summary = old['summary'], new['summary']
        if before[name] != after[name]:
            result['changed_files'][name] = 'source-bytes'
        elif old['changed_lines'] != new['changed_lines']:
            result['changed_files'][name] = 'maintained-hunks'
        elif (bool(old_summary) != bool(new_summary) or
              (old_summary and any(old_summary[key]['count'] != new_summary[key]['count']
                                   for key in COVERAGE_KINDS)) or
              old.get('changed_region_entries') != new.get('changed_region_entries') or
              (old.get('changed_region_mapping_sha256') and
               old['changed_region_mapping_sha256'] != new['changed_region_mapping_sha256'])):
            result['changed_files'][name] = 'emitted-mapping'
        else:
            if not old_summary:
                result['comparable_files'].append(name)
                continue
            old_branches, new_branches = old.get('changed_branches'), new.get('changed_branches')
            have_branches = old_branches is not None and new_branches is not None
            if have_branches:
                old_points = sorted((row['start'], row['end']) for row in old_branches['decisions'])
                new_points = sorted((row['start'], row['end']) for row in new_branches['decisions'])
                if old_points != new_points:
                    result['changed_files'][name] = 'emitted-mapping'
                    continue
            result['comparable_files'].append(name)
            delta = {key: new_summary[key]['covered'] - old_summary[key]['covered'] for key in COVERAGE_KINDS}
            delta['changed_region_entries'] = new['covered_changed_region_entries'] - old['covered_changed_region_entries']
            if have_branches:
                before_arms = covered_arms(old_branches['decisions'])
                after_arms = covered_arms(new_branches['decisions'])
                if lost := arm_changes(before_arms - after_arms):
                    result['lost_branch_arms'][name] = lost
                if gained := arm_changes(after_arms - before_arms):
                    result['gained_branch_arms'][name] = gained
                delta['changed_branch_arms'] = new_branches['covered_arms'] - old_branches['covered_arms']
            else:
                result['missing_branch_baseline'].append(name)
            if any(delta.values()):
                result['hit_deltas'][name] = delta
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


def lit_inventory(discovery: str, results: dict) -> dict:
    """Reconcile all discovered Objective-C 3 cases with recorded outcomes."""
    header, separator, listing = discovery.partition('-- Available Tests --\n')
    if header.strip() or not separator:
        raise ValueError('compiler coverage could not read the lit discovery inventory')
    lines = listing.splitlines()
    names = {line.removeprefix('  ') for line in lines if line.startswith('  ') and ' :: ' in line}
    tests = results.get('tests', [])
    if (not names or len(lines) != len(names) or len(tests) != len(names) or
            {row.get('name') for row in tests} != names):
        raise ValueError('compiler coverage lit execution differs from the full discovered inventory')
    outcomes = Counter(row.get('code') for row in tests)
    if (not outcomes['PASS'] or not outcomes.keys() <= {'PASS', 'XFAIL', 'UNSUPPORTED'} or
            any(row['code'] == 'UNSUPPORTED' and not row.get('output', '').strip() for row in tests)):
        raise ValueError('compiler coverage lit suite failed or omitted an unsupported-case reason')
    return {key: outcomes[key] for key in ('PASS', 'XFAIL', 'UNSUPPORTED')}


def qualify(repo: Path, output: Path, *, jobs=2, baseline: Path | None = None) -> dict:
    repo, output = repo.resolve(strict=True), output.resolve()
    if output.exists() or not output.is_relative_to(repo / 'out') or jobs not in (1, 2, 3, 4):
        raise ValueError('compiler source coverage requires fresh owned output and bounded jobs')
    if baseline is not None:
        baseline = baseline.resolve(strict=True)
        prior, baseline_identity = coverage_baseline(repo, baseline)
    host = native_linux_host()
    plan = build_plans(repo, host=host, instrumentation_profile=PROFILE)['configure']
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
    bootstrap = Path(plan['host_environment']['bootstrap_root'])
    tools['llvm-objdump'] = bootstrap / 'bin/llvm-objdump'
    test_binaries = [build / relative for relative in plan['instrumentation']['test_binaries']]
    binaries = [stage / 'bin/clang',
                installed_shared_library(stage / 'lib', 'libclang-cpp.so'),
                installed_shared_library(stage / 'lib', 'libLLVM.so'), *test_binaries]
    report = {'schema': SCHEMA, 'status': 'running',
        'profile': PROFILE, 'host': host, 'source_build_id': plan['source_build_id'],
        'build_config_digest': plan['build_config_digest'],
        'revision': subprocess.check_output(['git', '-C', repo, 'rev-parse', 'HEAD'], text=True).strip(),
        'eligible': eligible, 'eligible_sha256': hashlib.sha256(
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
           ('ASAN_OPTIONS', 'LSAN_OPTIONS', 'UBSAN_OPTIONS', 'TSAN_OPTIONS', 'LLVM_PROFILE_FILE') and
           not key.startswith(('LIT_', 'GTEST_'))}
    env['LD_LIBRARY_PATH'] = str(stage / 'lib')
    env['PATH'] = str(stage / 'bin') + os.pathsep + env.get('PATH', '')
    env['LLVM_PROFILE_FILE'] = os.devnull
    env['PYTHONDONTWRITEBYTECODE'] = '1'
    def run(name, argv, *, profile=True, stdout=None):
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
                stdout=stream, stderr=errors if stdout else subprocess.STDOUT)
        row = {'name': name, 'argv': list(map(str, argv)), 'exit_code': result.returncode,
               'seconds': round(time.monotonic()-started_command, 6), 'log': str(log), 'sha256': digest(log),
               'stderr': str(stderr_log), 'stderr_sha256': digest(stderr_log)}
        report['commands'].append(row);save()
        if result.returncode:
            raise ValueError('compiler source coverage command failed: '+name)
        return log
    try:
        for name, tool in tools.items():
            if not tool.is_file() or '23.1.5' not in run(name+'-version', [tool, '--version'], profile=False).read_text():
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
        # The patch-owned Clang implementation objects live in the shared
        # clang-cpp DSO. Combining its mappings with the driver, LLVM, or
        # statically linked unit binaries creates incompatible duplicate
        # inline/template function hashes rather than a valid union.
        compiler_binaries = [binaries[1]]
        compiler_ids = {binary_identity[str(binary)] for binary in compiler_binaries}
        export_binaries = unique_binary_paths(compiler_binaries, binary_identity)
        report['objects'] = compile_inventory(build, source, eligible)
        inspect_objects(report['objects'], lambda index, path: run('object-sections-'+str(index),
            coverage_section_command(tools['llvm-objdump'], path), profile=False).read_text())
        before = {row['object']: row['sha256'] for row in report['objects']}
        before.update({str(source / name): value for name, value in eligible.items()})
        before.update({str(path): digest(path) for path in [*binaries, *tools.values()]})
        lit_json = output / 'lit-results.json'
        lit_tool, lit_source = build / 'bin/llvm-lit', source / 'clang/test/ObjC3'
        discovery = run('objc3-lit-discovery', [lit_tool, '--show-tests', lit_source], profile=False,
                        stdout=output / 'lit-discovery.log')
        lit_log = run('objc3-lit', [lit_tool, '-sv', '--show-unsupported', '-j', str(min(jobs, 4)),
            '--output', lit_json, lit_source])
        outcomes = lit_inventory(discovery.read_text(), json.loads(lit_json.read_text()))
        report['suites'].append({'name': 'ObjC3-lit', 'status': 'PASS', 'outcomes': outcomes,
                                 'discovery_sha256': digest(discovery), 'log_sha256': digest(lit_log),
                                 'result_sha256': digest(lit_json)})
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
        profile_ids = {}
        for profile in profiles:
            matched = re.fullmatch(r'([0-9a-f]{40})-.+\.profraw', profile.name)
            if not matched: raise ValueError('compiler raw profile omitted its executable build identity')
            filename_ids.add(matched[1]);profile_ids[profile] = matched[1]
        report['raw_profile_binary_ids'] = sorted(filename_ids);save()
        require_binary_ids(filename_ids, required=expected_profile_ids, allowed=allowed_profile_ids,
                           kind='raw profiles')
        selected_profiles, excluded_profiles = select_coverage_profiles(
            profile_ids, compiler_ids, suite='objc3-lit')
        report['excluded_execution_profiles'] = {
            'count': len(excluded_profiles),
            'bytes': sum(profile.stat().st_size for profile in excluded_profiles),
            'binary_ids': sorted({profile_ids[profile] for profile in excluded_profiles}),
            'reason': 'The shared clang-cpp DSO owns the patch-compiled Clang implementation denominator. Driver, LLVM, support-tool, and statically linked unit profiles remain identity-checked execution evidence, but their duplicate inline/template mappings are not merged.'}
        merged = output / 'compiler.profdata'
        run('merge', [tools['llvm-profdata'], 'merge', '--sparse', *selected_profiles, '-o', merged], profile=False)
        identifiers = run('profile-binary-ids', [tools['llvm-profdata'], 'show', '--binary-ids', merged], profile=False).read_text()
        observed_ids = binary_ids(identifiers)
        require_binary_ids(observed_ids, required=compiler_ids, allowed=compiler_ids,
                           kind='merged profile')
        export = output / 'coverage-export.json'
        run('export', [tools['llvm-cov'], 'export', export_binaries[0],
            *('--object='+str(binary) for binary in export_binaries[1:]),
            *EXPORT_FLAGS, '--instr-profile='+str(merged), '--compilation-dir='+str(build),
            *(source / name for name in eligible)], profile=False, stdout=export)
        if (output / 'logs/export.stderr.log').read_text().strip():
            raise ValueError('compiler source coverage export emitted mismatch diagnostics')
        report['source_coverage'] = read_export(export, source=source, eligible=eligible, changed=changed)
        report['indexed_export'] = reclaim_indexed_export(export)
        if baseline is not None:
            if digest(baseline) != baseline_identity['sha256']:
                raise ValueError('compiler coverage baseline changed during execution')
            report['baseline_comparison'] = {**baseline_identity, **compare_coverage(prior, report)}
        if not any(row['state'] == 'executed' for row in report['source_coverage'].values()):
            raise ValueError('patched compiler coverage has no executed source')
        if any(digest(Path(path)) != value for path, value in before.items()):
            raise ValueError('compiler coverage source, object, binary or tool changed during execution')
        report['raw_profiles'] = {'count': len(profiles), 'bytes': sum(path.stat().st_size for path in profiles),
                                  'sha256': {path.relative_to(profile_root).as_posix(): digest(path)
                                             for path in profiles}}
        for raw in profiles: raw.unlink()
        report.update(status='PASS', profile_sha256=digest(merged), binary_ids=sorted(observed_ids),
                      covered_binary_ids={str(binary): binary_identity[str(binary)]
                                          for binary in compiler_binaries},
                      coverage_owner='shared-clang-cpp-patched-implementation',
                      executed_unit_binary_ids={str(binary): binary_identity[str(binary)]
                                                for binary in test_binaries},
                      executed_binary_ids=execution_binary_identity,
                      exported_binaries=[{'path': str(path),
                                          'build_id': execution_binary_identity[str(path)]}
                                         for path in export_binaries],
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
    parser.add_argument('--baseline', type=Path, help='optional reviewed release ZIP or qualified report under out/')
    parser.add_argument('--compare-current', type=Path, help='compare retained passing reports without executing LLVM')
    args = parser.parse_args()
    if args.compare_current:
        if not args.baseline or args.output.exists():
            parser.error('--compare-current needs --baseline and fresh output')
        repo = args.repo.resolve(strict=True)
        previous, baseline_identity = coverage_baseline(repo, args.baseline)
        current, current_identity = coverage_baseline(repo, args.compare_current)
        if current.get('status') != 'PASS' or current.get('schema') != SCHEMA or current.get('profile') != PROFILE:
            raise ValueError('current compiler coverage report is incomplete')
        result = {'schema': 'objc3c.compiler-source-coverage-comparison.v1', 'status': 'PASS',
                  'baseline': baseline_identity, 'current': current_identity, **compare_coverage(previous, current)}
        args.output.mkdir(parents=True)
        write_json(args.output / 'comparison.json', result)
        print(json.dumps({'status': result['status'], 'comparable_files': len(result['comparable_files'])}))
        return
    result = qualify(args.repo, args.output, baseline=args.baseline, jobs=args.jobs)
    print(json.dumps({'status': result['status'], 'eligible': len(result['eligible']),
                      'suites': len(result['suites'])}))


if __name__ == '__main__': main()

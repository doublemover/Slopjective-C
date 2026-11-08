"""Execute identified foundation plans and retain command timings and logs."""
from __future__ import annotations

from datetime import datetime, timezone
from contextlib import contextmanager
import json
import hashlib
import os
import platform
from pathlib import Path
import subprocess
import time

from .core import ToolchainError, build_plans, prepare, seal_development


def _digest_file(path: Path):
    if not path.is_file(): return None
    with path.open('rb') as stream: return hashlib.file_digest(stream, 'sha256').hexdigest()


def configuration_input_identity(plan):
    # The complete plan also inventories generated/staged consumers. Their
    # appearance after a successful cold build is an output, not invalidation.
    inputs = {key: value for key, value in plan.items() if key not in ('identity', 'sdk_consumers')}
    return hashlib.sha256(json.dumps(inputs, sort_keys=True, separators=(',', ':')).encode()).hexdigest()


def cost_context(plan, *, preparation_seconds, prepared):
    """Record cache state before configuration; cold directories are not cold OS caches."""
    directory = Path(plan['paths']['build'])
    prior_path = directory / 'foundation-build-report.json'
    try:
        prior = json.loads(prior_path.read_text()) if prior_path.is_file() else {}
    except (ValueError, OSError):
        prior = {}
    prior_configure = next((row.get('plan_identity') for row in prior.get('phases', [])
                            if row.get('phase') == 'configure'), None)
    input_identity = configuration_input_identity(plan)
    prior_input_identity = prior.get('cost_context', {}).get('configuration_input_identity')
    cached = {name: _digest_file(directory / name) for name in ('CMakeCache.txt', 'build.ninja', '.ninja_log')}
    initial_entries = len(list(directory.iterdir())) if directory.is_dir() else 0
    if not any(cached.values()) and initial_entries == 0:
        state = 'cold-build-directory'
    elif (cached['CMakeCache.txt'] and cached['build.ninja'] and prior.get('status') == 'passed' and
          prior.get('source_identity') == plan['source_build_id'] and
          prior_input_identity == input_identity):
        state = 'incremental-exact-inputs'
    elif prior.get('status') == 'passed' and (prior.get('source_identity') != plan['source_build_id'] or
            prior_input_identity is not None and prior_input_identity != input_identity):
        state = 'invalidation-different-inputs'
    else:
        state = 'existing-unqualified-tree'
    def read(path):
        candidate = Path(path)
        return candidate.read_text().strip() if candidate.is_file() else None
    cpu = read('/proc/cpuinfo') or ''
    return {'schema': 'objc3c.foundation-cost-context.v1',
        'configuration_input_identity': input_identity,
        'build_config_digest': plan['build_config_digest'],
        'instrumentation': plan.get('instrumentation'),
        'host_environment': plan['host_environment'],
        'host': {'system': '-'.join((platform.system(), platform.release(), platform.version())), 'machine': platform.machine(),
            'cpu_model': next((line.split(':', 1)[1].strip() for line in cpu.splitlines()
                               if line.startswith('model name')), None),
            'affinity': sorted(os.sched_getaffinity(0)) if hasattr(os, 'sched_getaffinity') else None,
            'cpu_quota': read('/sys/fs/cgroup/cpu.max'), 'memory_limit': read('/sys/fs/cgroup/memory.max')},
        'initial_state': state, 'initial_cache_files': cached,
        'initial_entry_count': initial_entries,
        'prior_report_sha256': _digest_file(prior_path),
        'prior_source_identity': prior.get('source_identity'), 'prior_status': prior.get('status'),
        'prior_build_config_digest': prior.get('cost_context', {}).get('build_config_digest'),
        'prior_configure_plan_identity': prior_configure,
        'prior_configuration_input_identity': prior_input_identity,
        'source_preparation': {'performed': prepared, 'seconds': preparation_seconds},
        'cache_scope': 'Build-tree state only. Shared compiler and filesystem caches are not cleared.'}


@contextmanager
def foundation_lock(repo: Path):
    path = repo / "out/toolchains/objc3-development/.foundation-build.lock"
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("a+b") as lock:
        lock.seek(0)
        if os.name == "nt":
            import msvcrt
            lock.write(b"0")
            lock.flush()
            lock.seek(0)
            acquire = lambda: msvcrt.locking(lock.fileno(), msvcrt.LK_NBLCK, 1)
            release = lambda: msvcrt.locking(lock.fileno(), msvcrt.LK_UNLCK, 1)
        else:
            import fcntl
            acquire = lambda: fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
            release = lambda: fcntl.flock(lock, fcntl.LOCK_UN)
        try:
            acquire()
        except OSError as error:
            raise ToolchainError(f"a foundation build already holds {path}; preserving its outputs") from error
        try:
            yield
        finally:
            lock.seek(0)
            release()


def verify_configuration_trace(plan: dict) -> dict:
    root = Path(plan['configuration_root']).resolve()
    admitted = {(root / name).resolve() for name in plan['configuration_inputs']}
    primary = root / 'toolchains/llvm/cmake/ObjC3LLVMCache.cmake'
    seen = set()
    for line in Path(plan['configuration_trace']).read_text().splitlines():
        event = json.loads(line)
        if 'cmd' not in event:
            continue
        owner = Path(event['file']).resolve()
        if owner not in admitted:
            raise ToolchainError(f'unadmitted foundation CMake source: {owner}')
        seen.add(owner)
        command, args = event['cmd'].lower(), event['args']
        dependency = (args[0] if command in ('include', 'configure_file') and args else
                      args[1] if command == 'file' and len(args) > 1 and
                      args[0].upper() in ('READ', 'STRINGS') else None)
        if dependency is not None:
            path = Path(dependency)
            if not path.is_absolute():
                path = owner.parent / path
            if path.resolve() not in admitted:
                raise ToolchainError(f'foundation CMake input is outside its configuration inventory: {dependency}')
    if primary not in seen:
        raise ToolchainError('foundation configuration trace did not execute the owned initial cache')
    return {'executed_inputs': sorted(str(path.relative_to(root)) for path in seen)}


def verify_effective_configuration(plan: dict) -> dict:
    if 'configuration_trace' in plan:
        verify_configuration_trace(plan)
    cache = Path(plan['paths']['build']) / 'CMakeCache.txt'
    values = {}
    for line in cache.read_text().splitlines():
        if line and not line.startswith(('#', '//')) and '=' in line and ':' in line.split('=', 1)[0]:
            name, value = line.split('=', 1)
            values[name.split(':', 1)[0]] = value
    effective = {}
    for name, expected in plan['effective_configuration'].items():
        if values.get(name) != expected:
            raise ToolchainError(f'foundation effective configuration mismatch: {name}: expected {expected}, got {values.get(name)}')
        effective[name] = values[name]
    return effective


def verify_build_targets(configure_plan: dict, build_plan: dict) -> dict:
    """Admit every build group before starting any expensive compilation."""
    required = []
    for command in (build_plan['command'], *build_plan.get('follow_up_commands', [])):
        start = command.index('--target') + 1
        end = command.index('--parallel')
        required.extend(command[start:end])
    required = list(dict.fromkeys(required))
    try:
        inventory = subprocess.check_output([
            configure_plan['host_environment']['ninja_path'], '-C',
            configure_plan['paths']['build'], '-t', 'targets', 'all'],
            text=True, stderr=subprocess.STDOUT, timeout=30)
    except (OSError, subprocess.SubprocessError) as error:
        raise ToolchainError(f'foundation target inventory failed: {error}') from error
    available = {line.rsplit(': ', 1)[0] for line in inventory.splitlines() if ': ' in line}
    missing = sorted(set(required) - available)
    if missing:
        raise ToolchainError('foundation configured build targets are missing: ' + ', '.join(missing))
    return {'required_targets': required, 'configured_target_count': len(available),
            'inventory_sha256': hashlib.sha256(inventory.encode()).hexdigest()}


def build_foundation(repo: Path, **options) -> dict:
    with foundation_lock(repo.resolve()):
        return _build_foundation(repo, **options)


def _build_foundation(repo: Path, *, host: str | None = None, fetch: bool = False,
                     stage: bool = False, instrumentation_profile: str | None = None,
                     test_results: Path | None = None) -> dict:
    repo = repo.resolve()
    source = repo / "out/toolchains/objc3-development/src"
    preparation_started = time.monotonic()
    prepared = not source.exists()
    if not source.exists():
        prepare(repo, source_dir=source, fetch=fetch)
        seal_development(repo, source_dir=source)
    plans = build_plans(repo, source_dir=source, host=host, instrumentation_profile=instrumentation_profile)
    report = {"status": "running", "source_identity": plans["configure"]["source_build_id"], "phases": [],
              "started_utc": datetime.now(timezone.utc).isoformat()}
    report['cost_context'] = cost_context(plans['configure'],
        preparation_seconds=time.monotonic()-preparation_started, prepared=prepared)
    started = time.monotonic()
    report_path = None
    history_path = None
    run_name = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%S.%fZ")
    try:
        for phase in ("configure", "build", "test", *(('stage',) if stage else ())):
            plan = plans[phase]
            if phase == "build":
                report["effective_configuration"] = verify_effective_configuration(plans["configure"])
                report['configured_build_targets'] = verify_build_targets(plans['configure'], plans['build'])
            directory = Path(plan["paths"]["build"])
            directory.mkdir(parents=True, exist_ok=True)
            report_path = directory / "foundation-build-report.json"
            run_directory = directory / "foundation-runs" / run_name
            run_directory.mkdir(parents=True, exist_ok=True)
            history_path = run_directory / "report.json"
            report["run_directory"] = str(run_directory)
            environment = os.environ.copy()
            if instrumentation_profile:
                toolchain = plan['host_environment']
                if plan['instrumentation']['kind'] in ('sanitizer', 'fuzzer'):
                    from .sanitizer_policy import strict_environment
                    bootstrap = Path(toolchain['clang_path']).resolve().parent.parent
                    environment = strict_environment(Path(toolchain['clang_path']), bootstrap / 'bin/llvm-symbolizer')
                elif plan['instrumentation']['kind'] == 'source-coverage':
                    # Build-time generators must not leave unowned profiles in
                    # the checkout. Qualification replaces this sink with one
                    # isolated %p/%m directory per executed suite.
                    environment.pop('LLVM_PROFILE_FILE', None)
                    environment['LLVM_PROFILE_FILE'] = os.devnull
                else:
                    raise ToolchainError('unknown compiler instrumentation environment policy')
                environment['PATH'] = os.pathsep.join(dict.fromkeys([
                    str(Path(toolchain['cmake_path']).parent),
                    str(Path(toolchain['ninja_path']).parent),
                    *environment['PATH'].split(os.pathsep)]))
            environment.update(plan.get("command_environment", {}))
            environment["PYTHONDONTWRITEBYTECODE"] = "1"
            environment["CCACHE_COMPILERCHECK"] = "content"
            for index, command in enumerate([plan["command"], *plan.get("follow_up_commands", [])]):
                command = list(command)
                result_file = None
                if phase == 'test' and test_results is not None:
                    test_results.mkdir(parents=True, exist_ok=True)
                    result_file = test_results / (f'foundation-{index}.json' if index == 0 else f'foundation-{index}.xml')
                    command += (['--output', str(result_file)] if index == 0 else
                                ['--gtest_output=xml:' + str(result_file)])
                log_path = run_directory / f"{phase}-{index}.log"
                phase_started = time.monotonic()
                print(f"foundation {phase} [{index + 1}]: {log_path}", flush=True)
                with log_path.open("w") as log:
                    result = subprocess.run(command, cwd=repo, env=environment,
                                            stdout=log, stderr=subprocess.STDOUT)
                report["phases"].append({"phase": phase, "plan_identity": plan["identity"],
                    "command": command, "exit_code": result.returncode, "log": str(log_path),
                    "log_sha256": _digest_file(log_path),
                    "elapsed_seconds": round(time.monotonic() - phase_started, 6)})
                if result_file is not None:
                    report['phases'][-1]['test_results'] = str(result_file)
                history_path.write_text(json.dumps(report, indent=2) + "\n")
                report_path.write_text(json.dumps(report, indent=2) + "\n")
                if result.returncode:
                    raise ToolchainError(f"foundation {phase} exited {result.returncode}; see {log_path}")
        refreshed = build_plans(repo, source_dir=source, host=host, instrumentation_profile=instrumentation_profile)["configure"]
        for key in ("source_build_id", "source_tree", "build_config_digest", "host_environment"):
            if refreshed[key] != plans["configure"][key]:
                raise ToolchainError(f"foundation {key} changed during the build")
        report["status"] = "passed"
    except (ToolchainError, OSError) as error:
        report.update(status="failed", error=str(error))
        raise
    finally:
        report.update(elapsed_seconds=round(time.monotonic() - started, 6),
                      finished_utc=datetime.now(timezone.utc).isoformat())
        if history_path is not None:
            history_path.write_text(json.dumps(report, indent=2) + "\n")
        if report_path is not None:
            report_path.write_text(json.dumps(report, indent=2) + "\n")
    return report

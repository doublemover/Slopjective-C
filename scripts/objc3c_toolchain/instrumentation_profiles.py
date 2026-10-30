"""Select isolated compiler-process instrumentation in the canonical planner."""
from __future__ import annotations

import hashlib
import json
import shlex
from pathlib import Path


def select(repo: Path, name: str, host: str, base_build: dict, sealed: dict, upstream: dict) -> dict:
    source = repo / 'toolchains/llvm/instrumentation-profiles.json'
    owner = repo / 'scripts/objc3c_toolchain/instrumentation_profiles.py'
    document = json.loads(source.read_text())
    if document.get('schema') != 'objc3c.compiler-instrumentation-profiles.v1' or name not in document['profiles']:
        raise ValueError('unknown compiler instrumentation profile: '+name)
    profile = document['profiles'][name]
    if profile['host'] != host or host != 'linux-x86_64' or profile['subject'] != 'compiler-implementation':
        raise ValueError('compiler instrumentation requires its declared native Linux host')
    if not all(base_build[key] for key in ('assertions', 'rtti', 'eh')):
        raise ValueError('compiler instrumentation must preserve assertions, RTTI and exceptions')
    cmake = profile['cmake']
    if (cmake.get('LLVM_USE_SANITIZER') != 'Address;Undefined' or
            shlex.split(cmake.get('LLVM_UBSAN_FLAGS', '')) != [
                '-fsanitize=undefined', '-fno-sanitize=function,vptr',
                '-fno-sanitize-recover=undefined'] or
            profile.get('facade_sanitizer') != 'address,undefined'):
        raise ValueError('compiler instrumentation requires the owned nonrecovering ASan/UBSan recipe')
    if any(not isinstance(key, str) or not isinstance(value, str) for key, value in cmake.items()):
        raise ValueError('instrumentation CMake values must be explicit strings')
    build = {**base_build, **{key: profile[key] for key in
        ('projects', 'configuration', 'build_target_groups', 'test_binaries', 'additional_test_suites')}}
    build['build_targets'] = [target for group in build['build_target_groups'] for target in group['targets']]
    if len(set(build['build_targets'])) != len(build['build_targets']):
        raise ValueError('instrumentation targets must not overlap their build groups')
    excluded = set(profile['excluded_distribution_components'])
    if not excluded.issubset(base_build['distribution_components']):
        raise ValueError('instrumentation distribution exclusion is not a canonical component')
    build['distribution_components'] = list(dict.fromkeys([
        *(target for target in base_build['distribution_components'] if target not in excluded),
        *profile['additional_distribution_components']]))
    inputs = {path.relative_to(repo).as_posix(): hashlib.sha256(path.read_bytes()).hexdigest()
              for path in (source, owner)}
    inputs = {**sealed['configuration_inputs'], **inputs}
    recipe = {'build': build, 'configuration_inputs': inputs, 'instrumentation': profile}
    identity = hashlib.sha256(json.dumps(recipe, sort_keys=True, separators=(',', ':')).encode()).hexdigest()
    build_id = f"{upstream['tag']}-{upstream['commit'][:12]}-{sealed['patch_digest'][:12]}-{identity[:8]}"
    return {'name': name, 'build': build, 'build_config_digest': identity,
            'source_build_id': build_id, 'configuration_inputs': inputs,
            'cmake': cmake, 'recipe': profile}

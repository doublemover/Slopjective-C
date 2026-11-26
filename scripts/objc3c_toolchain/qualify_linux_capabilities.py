"""Check the advertised feature queries of an installed Linux SDK."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import subprocess

from .generate_features import AUTHORITY, PROFILES, generated
from .sdk_example import digest, environment, sdk_verification, write_json

LINUX_TARGETS = ('x86_64-unknown-linux-gnu', 'aarch64-unknown-linux-gnu')
RUNTIME = 'gnustep-2.2'
LANGUAGES = (('objective-c', 'c17'), ('objective-c++', 'c++20'))


def consumer(view: dict, target: str, profile: str | None) -> tuple[str, list[dict]]:
    """Build assertions for every canonical spelling and alias in this mode."""
    checks = []

    def require(condition: str, message: str) -> None:
        checks.extend((f'#if !({condition})', f'#error {message}', '#endif'))

    if profile is None:
        require('!defined(__OBJC3__)', 'Objective-C 3 requires opt-in')
    else:
        for macro, expected in (
            ('__OBJC3__', 1),
            ('__OBJC_VERSION__', 3),
            ('__OBJC3_LANGUAGE_REVISION__', view['language_revision']),
            ('__OBJC3_EXTENSION_ABI_REVISION__', view['extension_abi_revision']),
            ('__OBJC3_PROFILE__', PROFILES.index(profile)),
        ):
            require(f'defined({macro}) && {macro} == {expected}', f'{macro} must equal {expected}')

    rows = []
    for feature in view['features']:
        selected = next((item for item in feature['qualified_targets']
                         if (item['triple'], item['runtime'], item['profile']) ==
                         (target, RUNTIME, profile)), None)
        enabled = int(selected is not None)
        spellings = [(feature['public_macro'], feature['feature_query'])]
        spellings.extend((f'__OBJC3_FEATURE_{alias}__', f'objc3_{alias.lower()}')
                         for alias in feature['aliases'])
        for macro, query in spellings:
            if profile is None:
                require(f'!defined({macro})', f'{macro} requires Objective-C 3')
            else:
                require(f'defined({macro}) && {macro} == {enabled}', f'{macro} must equal {enabled}')
            for builtin in ('__has_feature', '__has_extension'):
                require(f'{builtin}({query}) == {enabled}', f'{builtin}({query}) must equal {enabled}')
        rows.append({'feature': feature['name'], 'advertised': bool(enabled),
                     'witnesses': selected['evidence'] if selected else []})
    return '\n'.join([*checks, '']), rows


def qualify(sdk: Path, authority: Path, output: Path) -> dict:
    sdk, output = sdk.resolve(strict=True), output.resolve()
    if output.is_relative_to(sdk) or sdk.is_relative_to(output):
        raise ValueError('capability report must be disjoint from the sealed SDK')
    if output == authority.resolve():
        raise ValueError('capability report must be separate from the catalog')
    output.parent.mkdir(parents=True, exist_ok=True)
    output.unlink(missing_ok=True)
    _, rendered = generated(json.loads(authority.read_text(encoding='utf-8')))
    view = json.loads(rendered)
    commands = []
    with sdk_verification(sdk):
        manifest = json.loads((sdk / 'objc3-sdk.json').read_text())
        target = manifest['target']
        if target not in LINUX_TARGETS:
            raise ValueError(f'unsupported Linux SDK capability target: {target}')
        compiler = sdk / 'bin/clang'
        compiler_sha = digest(compiler)
        env = environment(sdk)
        for language, standard in LANGUAGES:
            for profile in (*PROFILES, None):
                program, rows = consumer(view, target, profile)
                cmd = [str(compiler), '--no-default-config', '--target=' + target,
                       '-x', language, '-std=' + standard, '-fobjc-runtime=' + RUNTIME,
                       '-fsyntax-only', '-Werror', '-']
                if profile is not None:
                    cmd[1:1] = ['-fobjc-version=3', '-fobjc3-profile=' + profile]
                result = subprocess.run(cmd, cwd=output.parent, env=env, input=program,
                                        capture_output=True, text=True)
                if result.returncode:
                    raise ValueError(f'{target} {language} {profile or "default"} feature queries failed:\n'
                                     f'{result.stderr}{result.stdout}')
                commands.append({'language': language, 'profile': profile, 'argv': cmd,
                                 'features': rows})
    report = {'schema': 'objc3c.linux-installed-capability-queries.v1', 'status': 'PASS',
              'target': target, 'runtime': RUNTIME, 'sdk_seal': manifest['seal_sha256'],
              'source_build_id': manifest['source_build_id'], 'compiler_sha256': compiler_sha,
              'authority_sha256': view['authority_sha256'], 'commands': commands}
    write_json(output, report)
    return report


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk-root', type=Path, required=True)
    parser.add_argument('--authority', type=Path, default=AUTHORITY)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    try:
        report = qualify(args.sdk_root, args.authority, args.output)
    except subprocess.CalledProcessError as error:
        diagnostic = (error.stderr or b'').decode(errors='replace')
        parser.exit(1, f'SDK verification failed: {error}\n{diagnostic}')
    except (OSError, ValueError) as error:
        parser.exit(1, f'{error}\n')
    print(f'{report["target"]} installed capability queries: PASS ({len(report["commands"])} modes)')


if __name__ == '__main__':
    main()

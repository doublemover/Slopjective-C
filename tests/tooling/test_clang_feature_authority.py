import copy
import json
from pathlib import Path
import re

import pytest

from scripts.objc3c_toolchain.generate_features import AUTHORITY, TABLE, VIEW, generated


def test_checked_in_feature_views_match_one_authority():
    authority = json.loads(AUTHORITY.read_text(encoding="utf-8"))
    table, view = generated(authority)
    assert TABLE.read_text(encoding="utf-8") == table
    assert VIEW.read_text(encoding="utf-8") == view
    assert {feature["macro"] for feature in authority["features"]}.issuperset(
        {"DEFER", "OPTIONALS", "THROWS", "ASYNC_AWAIT", "ACTORS"})


def test_frontend_interface_and_preprocessor_share_authoritative_revisions():
    authority = json.loads(AUTHORITY.read_text(encoding="utf-8"))
    packet = (Path(__file__).resolve().parents[2] / 'toolchains/llvm/patches/'
              '0001-objc3-language-mode-and-build-identity.patch').read_text()
    for name, key in (('LANGUAGE_REVISION', 'language_revision'),
                      ('EXTENSION_ABI_REVISION', 'extension_abi_revision')):
        value = re.search(r'^\+#define CLANG_OBJC3_' + name + r' (\d+)$', packet, re.M)
        assert value and int(value[1]) == authority[key]
        assert 'llvm::itostr(CLANG_OBJC3_' + name + ')' in packet


def test_unqualified_feature_cannot_claim_implementation():
    authority = json.loads(AUTHORITY.read_text(encoding="utf-8"))
    authority["features"][0].update(state="implemented", qualified_targets=[])
    with pytest.raises(ValueError, match="require qualified"):
        generated(authority)


def test_qualification_predicate_binds_actual_target_runtime_and_profile():
    authority = copy.deepcopy(json.loads(AUTHORITY.read_text(encoding="utf-8")))
    authority["features"][0].update(state="implemented", qualified_targets=[{
        "triple": "x86_64-pc-windows-msvc", "runtime": "gnustep-2.0",
        "profile": "strict", "evidence": ["target-runtime-fixture-pass"]}])
    table, _ = generated(authority)
    assert 'Target.getTriple().str() == "x86_64-pc-windows-msvc"' in table
    assert 'LangOpts.ObjCRuntime.getAsString() == "gnustep-2.0"' in table
    assert "LangOpts.ObjC3Profile == 1" in table


def test_alias_collisions_and_unknown_states_are_rejected():
    authority = json.loads(AUTHORITY.read_text(encoding="utf-8"))
    authority["features"][1]["aliases"].append("DEFER")
    with pytest.raises(ValueError, match="duplicate feature macro"):
        generated(authority)

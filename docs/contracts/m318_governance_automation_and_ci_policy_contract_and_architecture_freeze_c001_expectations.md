# M318-C001 Expectations

Contract ID: `objc3c-governance-automation-contract/m318-c001-v1`

`M318-C001` freezes the shared governance-automation contract for `M318`.

The contract must define:
- one shared governance guard runner;
- one CI workflow path that invokes it;
- one stable stage order and summary/report layout;
- one alarm vocabulary for budget and exception regressions.

The contract must consume the already-landed `M318-B001`, `M318-B002`, and `M318-B003`
policy surfaces rather than re-encoding a second governance vocabulary.

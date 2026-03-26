# M318-B001 Expectations

Contract ID: `objc3c-governance-anti-noise-budget-policy/m318-b001-v1`

`M318-B001` freezes the long-lived governance and budget-enforcement policy that
future work must follow after the cleanup program.

The policy must make three things explicit:
- what can grow without an exception;
- what is prohibited without an exception;
- which issue or later `M318` surface owns enforcement for each budget family.

The frozen policy must prohibit, without an approved exception record:
- public command growth beyond the `M314` budget;
- new milestone-local checker/readiness/pytest wrapper families beyond the `M313`
  budget;
- new milestone-coded product identifiers in product code;
- new synthetic `.ll` fixtures outside the fenced parity root;
- new proof-looking replay artifacts without provenance and regeneration metadata.

The policy may allow normal growth without exception only where the cleanup program
explicitly carved out safe surfaces, such as acceptance-suite inputs, governance docs,
and `tmp/` planning artifacts.

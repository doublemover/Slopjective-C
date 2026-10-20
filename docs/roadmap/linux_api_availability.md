# Linux public API availability

Public semantic interfaces preserve Clang's native `deprecated`, `unavailable`
and `availability` attributes. This applies to supported functions, variables,
typedefs, enums and enumerators, data records and fields, and Objective-C
interfaces, protocols, categories, methods and properties. Export selection runs
before public attribute validation, so private implementation annotations remain
outside the public contract.

```c
int current_api(void);
int old_api(void)
    __attribute__((deprecated("use current_api", "current_api")));
int removed_api(void) __attribute__((unavailable("removed API")));
```

The compiler reads attribute fields from Clang declarations. Messages,
replacement text, platform names, normalized versions, strictness, unavailability
and environments participate in declaration identities. These identities bind
the native semantic fingerprint; changing availability invalidates dependent
compilations even when the callable ABI stays the same. Equivalent supported
attribute spellings retain the same native identity.

Canonical printing preserves the complete native contract, including escaped
attribute strings and Objective-C property and container annotations. The
generated interface must parse again with the original language, target and
ownership mode and reproduce every declaration identity. Attributes that cannot
round-trip are rejected before publication.

Interfaces carrying this metadata require `objc3.api-availability.v1`. The
capability and declaration metadata must agree. Schema 2 and extension ABI 13
remain unchanged; the new compiler patch identity and SDK seal require rebuilding
objects, PCH files, interfaces and packages with the current SDK. Source packages
remain v4 and immutable packages remain v3, with required explicit exports.

Native imports, qualified lookup and transitive re-exports retain availability
diagnostics. Consumers can promote deprecation warnings using ordinary Clang
diagnostic controls. Universal unavailability is a native error. Platform version
conditions follow Clang's target rules; retaining another platform's metadata
does not introduce a Linux OS version check or a runtime availability test.

ARC/MRC C ABI projections preserve availability alongside the producer's native
return-ownership contract. A projection cannot remove an annotation from its
canonical producer proof.

The installed qualification covers Objective-C17 ARC/MRC, Objective-C++20/23
ARC/MRC and C++20/23 at O0/O2. Sixteen cases exercise cold and reused source
graphs, private edits, public metadata edits, relocated source-free packages,
native deprecation/unavailability errors and forged interface metadata. Failure
checks preserve the prior consumer object and remove stale success records.
[Privileged SPI](linux_module_spi.md) has a separate implemented contract.
Module-owned linker/runtime names remain a future milestone.

The [acceptance receipt](../support/linux_api_availability_acceptance.json) binds
the installed qualification, sanitizer runs and fresh Debian/Ubuntu archives to
the implementation and exact SDK seal. The
[latency report](../support/linux_api_availability_latency.json) checks the
recorded command budgets on the same CPU model and quota.

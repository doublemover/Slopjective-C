# Native bit-fields and inherited fields in typed key paths

The keypath 2.2.0 source extension admits named native bit-fields and unambiguous
inherited C++ fields in the existing trivially copyable record traversal. The
metadata layout remains v2. Paths preserve the record component's source type
and the actual field declaration's owner identity.

C++ member lookup observes hiding, inherited using declarations, ambiguous base
subobjects and native access control, including protected-object rules. Normal
base conversion selects the declaring subobject. Access failures emit the
native diagnostic; a quiet access probe cannot discard a literal without an
error. References, nontrivial records, unions and ownership-erasing storage
remain excluded.

Bit-field getters use native signedness and value conversion. A field setter
reads one containing property value, updates the selected field using native
assignment, and writes the property back once. Native bit width truncation,
sign extension and sibling preservation therefore match ordinary field
assignment. No field address or flattened byte offset replaces member access.

Maintained witnesses exercise C17 and C++ field values, signed/unsigned
bit-fields, multiple inherited base subobjects, access rejection, name hiding,
public using declarations and exact getter/setter counts. Native printing,
PCH and AST import retain literal spelling and typed accessors. The source-free
package fixture adds a public bit-field record and a provider-owned path with
consumer-side metadata equality.

The [focused receipt](../support/linux_keypath_record_fields_progress.json)
records four native regressions, native AST import and two source/PCH runtime
configurations (12 commands). Broader compiler/frontend, source/PCH,
sanitizer and package validation follows the combined source batch. The accepted
SDK remains [2.29.0](../support/linux_source_batch_2_29_acceptance.json), carrying
keypath 2.1.0. Nullable writable traversal and nonnull/scalar/aggregate nullable
results, dependent/effectful/isolated paths and full profile conformance remain
open.

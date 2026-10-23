#ifndef TYPED_KEYPATH_RUNTIME_PCH
#include "runtime.h"
#endif
#include <assert.h>
#include <string.h>

static int path_evaluations, root_evaluations, value_evaluations;
static int operand_sequence;
static WritableKeyPath<RuntimePathNode *, int> once_path(void) {
  ++path_evaluations;
  operand_sequence = operand_sequence * 10 + 1;
  return runtime_nested_path();
}
static RuntimePathNode *once_root(RuntimePathNode *root) {
  ++root_evaluations;
  operand_sequence = operand_sequence * 10 + 2;
  return root;
}
static int once_value(void) {
  ++value_evaluations;
  operand_sequence = operand_sequence * 10 + 3;
  return 42;
}
static KeyPath<RuntimePathNode *, int> once_readonly_return(void) {
  return once_path();
}
static int consume_readonly(KeyPath<RuntimePathNode *, int> path,
                            RuntimePathNode * _Nonnull root) {
  return KeyPathGet(path, root);
}

int main(void) {
  @autoreleasepool {
    RuntimePathNode *root = [RuntimePathNode newWithCount:1];
    RuntimePathNode *child = [RuntimePathNode newWithCount:17];
    root.child = child;
    WritableKeyPath<RuntimePathNode *, int> path = runtime_nested_path();
    const objc3_keypath_descriptor_v2 *metadata = KeyPathMetadata(path);
    assert(path_child_reads == 0 && path_count_reads == 0);
    assert(objc3_keypath_validate_v2(metadata) == OBJC3_KEYPATH_OK);
    assert(metadata->component_count == 2 &&
           !strcmp(metadata->path, "child.count"));
    const objc3_keypath_component_v2 *part = 0;
    assert(objc3_keypath_component_v2_at(metadata, 0, &part) == 0);
    assert(!strcmp(part->getter_selector, "readChild"));
    assert(part->flags & OBJC3_KEYPATH_COMPONENT_STRONG);
    assert(part->flags & OBJC3_KEYPATH_COMPONENT_NONNULL);
    assert(objc3_keypath_component_v2_at(metadata, 1, &part) == 0);
    assert(!strcmp(part->setter_selector, "writeCount:"));

    assert(KeyPathGet(once_path(), once_root(root)) == 17);
    assert(path_evaluations == 1 && root_evaluations == 1);
    assert(operand_sequence == 12);
    operand_sequence = 0;
    assert(path_child_reads == 1 && path_count_reads == 1);
    KeyPathSet(once_path(), once_root(root), once_value());
    assert(path_evaluations == 2 && root_evaluations == 2 &&
           value_evaluations == 1);
    assert(operand_sequence == 123);
    assert(path_child_reads == 2 && path_count_sets == 1);
    assert(KeyPathGet(path, root) == 42);
    assert(KeyPathGet(runtime_identity_path(), child) == 142);

    // A provider-owned record and a consumer literal share structural metadata
    // identity while their descriptor/global helper addresses may differ.
    int equal = 0;
    assert(objc3_keypath_equal_v2(metadata,
        KeyPathMetadata(@keypath(RuntimePathNode, child.count)), &equal) == 0);
    assert(equal);
    KeyPath<RuntimePathNode *, int> read = KeyPathAsReadOnly(path);
    assert(KeyPathGet(read, root) == 42);
    assert(KeyPathMetadata(read) == metadata);
    int evaluations_before_narrowing = path_evaluations;
    read = once_path();
    assert(path_evaluations == evaluations_before_narrowing + 1);
    assert(KeyPathGet(once_readonly_return(), root) == 42);
    assert(path_evaluations == evaluations_before_narrowing + 2);
    assert(consume_readonly(once_path(), root) == 42);
    assert(path_evaluations == evaluations_before_narrowing + 3);

    // The selected path is a value snapshot before later operands execute.
    // Changing the source record while evaluating the root cannot redirect it.
    WritableKeyPath<RuntimePathNode *, int> selected = runtime_nested_path();
    assert(KeyPathGet(selected,
        (selected = @keypath(RuntimePathNode, count), root)) == 42);
    selected = runtime_nested_path();
    KeyPathSet(selected, (selected = @keypath(RuntimePathNode, count), root), 43);
    assert(root.count == 1 && child.count == 43);
    KeyPathSet(runtime_nested_path(), root, 42);

    PathPair pair = {21, 22};
    KeyPathSet(runtime_pair_path(), root, pair);
    PathPair actual = KeyPathGet(runtime_pair_path(), root);
    assert(actual.first == 21 && actual.second == 22);
    path_pair_reads = path_pair_sets = 0;
    WritableKeyPath<RuntimePathNode *, long> first =
        @keypath(RuntimePathNode, pair.first);
    KeyPathSet(first, root, 41L);
    assert(path_pair_reads == 1 && path_pair_sets == 1);
    actual = KeyPathGet(runtime_pair_path(), root);
    assert(actual.first == 41 && actual.second == 22);
    assert(KeyPathGet(first, root) == 41);
    const objc3_keypath_descriptor_v2 *field_metadata = KeyPathMetadata(first);
    assert(!(field_metadata->flags & OBJC3_KEYPATH_KVC_COMPATIBLE));
    assert(objc3_keypath_component_v2_at(field_metadata, 1, &part) == 0);
    assert(part->flags & OBJC3_KEYPATH_COMPONENT_RECORD_FIELD);
    assert(!*part->getter_selector && !*part->setter_selector);
    path_throw_pair_getter = 1;
    int sets_before_failure = path_pair_sets;
    @try {
      KeyPathSet(first, root, 99L);
      assert(0);
    } @catch (RuntimePathNode *failure) {
      assert(failure == root);
    }
    path_throw_pair_getter = 0;
    assert(path_pair_sets == sets_before_failure);
    path_throw_pair_setter = 1;
    @try {
      KeyPathSet(first, root, 99L);
      assert(0);
    } @catch (RuntimePathNode *failure) {
      assert(failure == root);
    }
    path_throw_pair_setter = 0;
    actual = root.pair;
    assert(actual.first == 41L && actual.second == 22L);

    // Property copying remains the native setter's job. The key-path record
    // stores no captured object and does not bypass copy/strong semantics.
    KeyPathSet(@keypath(RuntimePathNode, copied), root, child);
    assert(path_copies == 1);
    RuntimePathNode *copied = KeyPathGet(@keypath(RuntimePathNode, copied), root);
    assert(copied != child && copied.count == child.count);
#if __has_feature(objc_arc)
    RuntimePathNode *ephemeral = [RuntimePathNode newWithCount:7];
    __weak RuntimePathNode *weak = ephemeral;
    WritableKeyPath<RuntimePathNode *, int> no_capture = [ephemeral selfPath];
    ephemeral = nil;
    assert(weak == nil);
    assert(KeyPathGet(no_capture, child) == 42);
    @autoreleasepool {
      RuntimePathNode *observed = [RuntimePathNode newWithCount:9];
      KeyPathSet(@keypath(RuntimePathNode, observer), root, observed);
      assert(KeyPathGet(@keypath(RuntimePathNode, observer), root) == observed);
      observed = nil;
    }
    assert(KeyPathGet(@keypath(RuntimePathNode, observer), root) == nil);
    const objc3_keypath_component_v2 *weak_part = 0;
    assert(objc3_keypath_component_v2_at(
        KeyPathMetadata(@keypath(RuntimePathNode, observer)), 0, &weak_part) == 0);
    assert((weak_part->flags & (OBJC3_KEYPATH_COMPONENT_WEAK |
                                OBJC3_KEYPATH_COMPONENT_NULLABLE)) ==
           (OBJC3_KEYPATH_COMPONENT_WEAK | OBJC3_KEYPATH_COMPONENT_NULLABLE));
#else
    objc_release(child);
    objc_release(root);
#endif
  }
  assert(path_destroyed >= 3);
}

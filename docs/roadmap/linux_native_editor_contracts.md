# Native Linux editor contracts

The frontloaded source batch extends clangd over the new Objective-C 3.0 AST
contracts. These changes and their focused source-tool fixtures are authored;
no clangd build or test execution has been performed for this batch.

Explicit Block capture names are references to their original variables,
including unused items. Reference traversal emits each written name once and
skips generated creation/heap-copy recipes. Selection maps a capture token to
its existing declaration without allocating synthetic AST nodes or changing the
compiler AST. Rename can start at the capture name. Ownership modifiers receive
semantic modifier tokens and hovers with the resolved field type. A bare item
named `weak`, `strong`, `move` or `unowned` remains an ordinary variable reference.

`withLifetime` and `keepAlive` expose their operand once and retain the body as
source, while hiding implicit binding storage. Their keyword tokens and hovers
explain precise-owner duration and deferred-action ordering. Resource operation
hovers preserve public `take`/`reset`/`discard` names as written; reserved builtin
users retain their written spelling.

Declaration hovers expose borrowed-pointer and returned-owner contracts,
effective actor/nonisolated/executor metadata, typed key-path writability and
physical async frame-versus-task identity. These descriptions read native AST
metadata rather than inferring semantics from identifier spelling.

Key-path applications use the native PseudoObjectExpr public syntax and written
arguments. Editor selection and references skip private getter/setter fields and
synthetic callee declarations, retain each operand once and classify the public
operation token. Hovers present KeyPathGet/Set/Metadata/AsReadOnly and public
Root/Value types. Literal hovers recover the declared path without presenting
private implementation globals.

Pending focused witnesses cover capture-start rename, multiple unused capture
items, contextual modifier names, precise lifetime references/tokens, borrowed
returns, exact named executor identity, public resource names and key-path
operation hover/reference/token behavior. Completion, cross-file indexing and
additional literal component rename behavior still require their own complete
native source relationships; this slice does not claim those boundaries closed.

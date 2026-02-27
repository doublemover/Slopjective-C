# Compiler Dispatch Plan

## Milestone

- Number: **86**
- Title: **M01 Compiler Charter and Core/Strict Scope Freeze**
- Open issues in milestone: **6**

## Parallelization

- Parallel lanes: `A`, `B`, `C`, `D`, `E`
- Regroup dependency issues: `#1213`
- Note: INT regroup is lane-gated and should execute only after parallel lanes complete.

## Next Tasks

### Lane `A` (1 open issues)
- `#1172` `M01-A001`: [Compiler][M01][Lane A][M01-A001] Define command/grammar contract slice (Contract Definition)

### Lane `B` (1 open issues)
- `#1181` `M01-B001`: [Compiler][M01][Lane B][M01-B001] Define semantic rule matrix slice (Contract Definition)

### Lane `C` (1 open issues)
- `#1189` `M01-C001`: [Compiler][M01][Lane C][M01-C001] Define lowering/ABI contract slice (Contract Definition)

### Lane `D` (1 open issues)
- `#1197` `M01-D001`: [Compiler][M01][Lane D][M01-D001] Define diagnostics/conformance contract slice (Contract Definition)

### Lane `E` (1 open issues)
- `#1205` `M01-E001`: [Compiler][M01][Lane E][M01-E001] Define tooling/CI contract slice (Contract Definition)

#pragma once

struct Expr;
struct FunctionContext;
struct Objc3IRBlockLoweringContext;

void EmitObjc3IRBlockDescriptor(
    const Expr &expr, const Objc3IRBlockLoweringContext &context);

void EmitObjc3IRBlockCopyHelper(
    const Expr &expr, const Objc3IRBlockLoweringContext &context);

void EmitObjc3IRBlockDisposeHelper(
    const Expr &expr, const FunctionContext &ctx,
    const Objc3IRBlockLoweringContext &context);

void EmitObjc3IRBlockInvokeThunk(
    const Expr &expr, const Objc3IRBlockLoweringContext &context);

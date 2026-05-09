#pragma once

#include <string>

#include "ast/objc3_ast_core.h"

bool Objc3ExprIsBlockLiteral(const Expr &expr);
bool Objc3BlockRequiresByrefStorage(const Expr &expr);
bool Objc3BlockRequiresRuntimeCopyDispose(const Expr &expr);
bool Objc3BlockRequiresRuntimePromotion(const Expr &expr);
bool Objc3BlockCanUseStackInvokeLowering(const Expr &expr);
std::string Objc3BlockLoweringReplayKey(const Expr &expr);

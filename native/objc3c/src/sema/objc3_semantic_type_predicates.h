#pragma once

#include "sema/objc3_semantic_type_helpers.h"

bool IsUnknownSemanticType(const SemanticTypeInfo &info);
bool IsCallableSemanticType(const SemanticTypeInfo &info);
bool IsScalarSemanticType(const SemanticTypeInfo &info);
bool IsScalarBoolCompatibleType(const SemanticTypeInfo &info);
bool IsScalarI32CompatibleType(const SemanticTypeInfo &info);
bool AreScalarI32AliasCompatible(const SemanticTypeInfo &lhs,
                                 const SemanticTypeInfo &rhs);

bool IsCanonicalObjc3TypeFormScaffoldReady();
bool IsObjCReferenceValueType(ValueType type);
bool IsObjCReferenceSemanticType(const SemanticTypeInfo &info);
bool IsNullableObjCReferenceSemanticType(const SemanticTypeInfo &info);
bool IsNonnullDestinationObjCReferenceSemanticType(
    const SemanticTypeInfo &info);
bool IsVoidSemanticType(const SemanticTypeInfo &info);
SemanticTypeInfo MakeNonnullRefinedSemanticType(const SemanticTypeInfo &info);
bool IsOwnedObjCReferenceSemanticType(const SemanticTypeInfo &info);
bool IsWeakObjCReferenceSemanticType(const SemanticTypeInfo &info);
bool IsUnownedObjCReferenceSemanticType(const SemanticTypeInfo &info);
bool IsMessageCompatibleType(const SemanticTypeInfo &info);

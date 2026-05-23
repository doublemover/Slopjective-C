static SemanticTypeInfo MakeSemanticTypeFromObjc3CollectionIdentity(
    const std::string &identity) {
  if (identity == "i32") {
    return MakeScalarSemanticType(ValueType::I32);
  }
  if (identity == "bool") {
    return MakeScalarSemanticType(ValueType::Bool);
  }
  if (identity == "TextHandle") {
    return MakeScalarSemanticType(ValueType::TextHandle);
  }
  if (identity == "id" || identity == "object") {
    return MakeScalarSemanticType(ValueType::ObjCId);
  }
  return MakeScalarSemanticType(ValueType::Unknown);
}

static bool IsObjc3CollectionLiteralRuntimeI32Type(
    const SemanticTypeInfo &type) {
  return IsScalarSemanticType(type) && type.type == ValueType::I32;
}

static bool IsObjc3CollectionValueCompatibleWithIdentity(
    const std::string &identity,
    const SemanticTypeInfo &type) {
  if (identity == "i32") {
    return IsObjc3CollectionLiteralRuntimeI32Type(type);
  }
  if (identity == "bool") {
    return IsScalarSemanticType(type) && type.type == ValueType::Bool;
  }
  if (identity == "TextHandle") {
    return IsScalarSemanticType(type) && type.type == ValueType::TextHandle;
  }
  if (identity == "id" || identity == "object") {
    return IsObjCReferenceSemanticType(type);
  }
  return false;
}

static const char *Objc3MutableCollectionTypeNameForLiteral(
    Expr::CollectionLiteralKind kind) {
  switch (kind) {
    case Expr::CollectionLiteralKind::Array:
      return "MutableArray";
    case Expr::CollectionLiteralKind::Map:
      return "MutableMap";
    case Expr::CollectionLiteralKind::Set:
      return "MutableSet";
    case Expr::CollectionLiteralKind::None:
      return "";
  }
  return "";
}

static bool IsObjc3CollectionMapKind(Objc3GenericCollectionKind kind) {
  return kind == Objc3GenericCollectionKind::Map ||
         kind == Objc3GenericCollectionKind::MutableMap;
}

static bool IsObjc3CollectionArrayLikeKind(Objc3GenericCollectionKind kind) {
  return kind == Objc3GenericCollectionKind::Array ||
         kind == Objc3GenericCollectionKind::MutableArray ||
         kind == Objc3GenericCollectionKind::Slice;
}

static bool IsObjc3CollectionSetKind(Objc3GenericCollectionKind kind) {
  return kind == Objc3GenericCollectionKind::Set ||
         kind == Objc3GenericCollectionKind::MutableSet;
}

static bool IsObjc3MutableCollectionKind(Objc3GenericCollectionKind kind) {
  return kind == Objc3GenericCollectionKind::MutableArray ||
         kind == Objc3GenericCollectionKind::MutableMap ||
         kind == Objc3GenericCollectionKind::MutableSet;
}

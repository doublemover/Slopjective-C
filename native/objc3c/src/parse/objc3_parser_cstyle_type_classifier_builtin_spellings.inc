bool ClassifyObjc3CStyleBuiltinTypeSpelling(
    const std::string &spelling,
    Objc3CStyleTypeTokenClassification &classification) {
  classification = Objc3CStyleTypeTokenClassification{};
  if (spelling == "int" || spelling == "NSInteger" || spelling == "NSUInteger") {
    classification.recognized = true;
    classification.type = ValueType::I32;
    return true;
  }
  if (spelling == "bool" || spelling == "BOOL") {
    classification.recognized = true;
    classification.type = ValueType::Bool;
    return true;
  }
  if (spelling == "void") {
    classification.recognized = true;
    classification.type = ValueType::Void;
    return true;
  }
  if (spelling == "id") {
    classification.recognized = true;
    classification.type = ValueType::ObjCId;
    classification.id_spelling = true;
    return true;
  }
  if (spelling == "Class") {
    classification.recognized = true;
    classification.type = ValueType::ObjCClass;
    classification.class_spelling = true;
    return true;
  }
  if (spelling == "SEL") {
    classification.recognized = true;
    classification.type = ValueType::ObjCSel;
    classification.sel_spelling = true;
    return true;
  }
  if (spelling == "Protocol") {
    classification.recognized = true;
    classification.type = ValueType::ObjCProtocol;
    return true;
  }
  if (spelling == "instancetype") {
    classification.recognized = true;
    classification.type = ValueType::ObjCInstancetype;
    classification.instancetype_spelling = true;
    return true;
  }
  return false;
}

#pragma once

enum class ValueType {
  Unknown,
  I32,
  Bool,
  Void,
  Function,
  ObjCId,
  ObjCClass,
  ObjCSel,
  ObjCProtocol,
  ObjCInstancetype,
  ObjCObjectPtr,
  TextHandle
};

#pragma once

namespace objc3c::support {

bool IsObjCRuntimeTypeSurface(bool id_spelling,
                              bool class_spelling,
                              bool sel_spelling,
                              bool instancetype_spelling,
                              bool object_pointer_type_spelling);

}  // namespace objc3c::support

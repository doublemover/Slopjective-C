#include "ir/objc3_ir_deterministic_publication.h"

#include <sstream>

std::string Objc3IRBoolLiteral(bool value) {
  return value ? "true" : "false";
}

std::string Objc3IRUnsignedLiteral(std::size_t value) {
  return std::to_string(value);
}

std::string Objc3IRSymbolOrNull(bool present, const std::string &symbol) {
  return present ? symbol : "null";
}

Objc3IRPublicationField Objc3IRField(const std::string &key,
                                     const std::string &value) {
  return Objc3IRPublicationField{key, value};
}

Objc3IRPublicationField Objc3IRField(const std::string &key,
                                     std::size_t value) {
  return Objc3IRPublicationField{key, Objc3IRUnsignedLiteral(value)};
}

Objc3IRPublicationField Objc3IRFlag(const std::string &key, bool value) {
  return Objc3IRPublicationField{key, Objc3IRBoolLiteral(value)};
}

std::string BuildObjc3IRCommentSurface(
    const std::string &label,
    const std::vector<Objc3IRPublicationField> &fields) {
  std::ostringstream out;
  out << "; " << label << " = ";
  for (std::size_t i = 0; i < fields.size(); ++i) {
    if (i != 0) {
      out << ";";
    }
    out << fields[i].key << "=" << fields[i].value;
  }
  out << "\n";
  return out.str();
}

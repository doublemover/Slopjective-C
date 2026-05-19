#pragma once

#include <cstddef>
#include <string>
#include <vector>

struct Objc3IRPublicationField {
  std::string key;
  std::string value;
};

std::string Objc3IRBoolLiteral(bool value);
std::string Objc3IRUnsignedLiteral(std::size_t value);
std::string Objc3IRSymbolOrNull(bool present, const std::string &symbol);
Objc3IRPublicationField Objc3IRField(const std::string &key,
                                     const std::string &value);
Objc3IRPublicationField Objc3IRField(const std::string &key,
                                     std::size_t value);
Objc3IRPublicationField Objc3IRFlag(const std::string &key, bool value);
std::string BuildObjc3IRCommentSurface(
    const std::string &label,
    const std::vector<Objc3IRPublicationField> &fields);

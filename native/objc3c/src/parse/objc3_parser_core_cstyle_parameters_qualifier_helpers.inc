  bool IsBorrowedQualifierSpelling(const std::string &text) const {
    return objc3c::parse::IsObjc3BorrowedQualifierSpelling(text);
  }

  std::string BuildReturnsBorrowedProfile(bool declared,
                                          std::size_t owner_index,
                                          bool return_borrowed_pointer_qualified) {
    return objc3c::parse::BuildObjc3ReturnsBorrowedProfile(
        declared, owner_index, return_borrowed_pointer_qualified);
  }

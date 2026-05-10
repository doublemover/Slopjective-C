    if (!parameter_type_spelling_matched) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P108",
          "expected parameter type 'i32', 'bool', 'BOOL', 'NSInteger', 'NSUInteger', or "
          "'id', 'Class', 'SEL', 'Protocol', 'instancetype', object pointer spelling, "
          "ownership qualifiers '__strong/__weak/__autoreleasing/__unsafe_unretained', "
          "or vector forms "
          "'i32x2/i32x4/i32x8/i32x16' and 'boolx2/boolx4/boolx8/boolx16'"));
      return false;
    }

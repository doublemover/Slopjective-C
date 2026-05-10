void CollectBlockLiteralMutatedIdentifiersFromStatement(
    const Stmt *stmt,
    std::vector<std::string> &mutated_identifiers);

#include "parse/objc3_parser_block_literal_mutated_identifiers_mutation_predicates.inc"

#include "parse/objc3_parser_block_literal_mutated_identifiers_for_clause_traversal.inc"

#include "parse/objc3_parser_block_literal_mutated_identifiers_statement_body_traversal.inc"

#include "parse/objc3_parser_block_literal_mutated_identifiers_statement_traversal.inc"

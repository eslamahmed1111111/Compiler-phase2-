#ifndef CFG_PARSER_H
#define CFG_PARSER_H

#include <bits/stdc++.h>
#include "Symbol.h"
#include "Rule.h"

using namespace std;


class Rule;
class Symbol;


class CFG_Parser
{
    public:
        CFG_Parser(string file_path);

        Rule * find_rule_with_lhs(const string &lhs);
        bool find_symbol_with_name(const string &name, string type);
        vector<Rule*> rules;
        vector<Symbol*> terminals;
        vector<Symbol*> non_terminals;
        vector<Rule*> get_rules();
        vector<Symbol*> get_terminals();
        vector<Symbol*> get_non_terminals();
        vector<string> get_string_rules();
        bool is_LL_grammar();


    protected:

    private:
        vector<string> string_rules; // contains all rules METHOD_BODY = STATEMENT_LIST, STATEMENT_LIST = STATEMENT | STATEMENT_LIST STATEMENT, STATEMENT = DECLARATION | IF | WHILE | ASSIGNMENT
        int find_rule_index(string s);
        void parse_rules_string(string rules);
        void add_new_rule(string rule_string);
        void clean_rule_string(string &rule_string);
};

#endif // CFG_PARSER_H

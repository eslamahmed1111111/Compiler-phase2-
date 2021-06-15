#ifndef SYMBOL_H
#define SYMBOL_H

#include <bits/stdc++.h>
#include "Rule.h"

using namespace std;

class Rule;

class Symbol
{
    public:
        Symbol();
        Symbol(string symbol_string); // Terminal
        Symbol(Rule* grammar_rule); // Nonterminal

        enum Type {
            Terminal,
            NonTerminal,
            Synch
        };

        Rule* get_symbol_rule();
        Type get_symbol_type();
        string get_symbol_string();



    protected:

    private:
        Rule* grammar_rule;
        string symbol_string;
        Type type;
};

#endif // SYMBOL_H

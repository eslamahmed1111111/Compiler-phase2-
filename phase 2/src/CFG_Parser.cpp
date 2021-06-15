#include "CFG_Parser.h"

using namespace std;
// each rule consists of one or more productions each production consist of one or more symbols each symbol is (terminal or non terminal)
CFG_Parser::CFG_Parser(string file_path)
{
    // Read the file as one string and send it to parse_rules_string to parse it.
    ifstream cfg_ifs(file_path);
    stringstream cfg_buffer;
    cfg_buffer << cfg_ifs.rdbuf();

    parse_rules_string(cfg_buffer.str());
    terminals.push_back(new Symbol("$"));
    /*
    rule name SIMPLE_EXPRESSION
    rule productions
    production 0-TERM,1 -
    production 1-SIGN,1 - TERM,1 -
    production 2-SIMPLE_EXPRESSION,1 - addop,0 - TERM,1 - */

    for(Rule* rule : this->rules){
        cout << "rule name "<< rule->get_rule_name() << "\n rule productions \n";
        vector<vector<Symbol*> > prod = rule->get_productions();
        for(int i = 0; i < prod.size() ; i++){
                cout << "production " << i << "-" ;
            for(Symbol* symbol : prod[i]){
                cout << symbol->get_symbol_string() << "," << symbol->get_symbol_type() << " - ";
            }
            cout << endl;
        }
        cout << endl;
    }

  //Terminals: id -> terminal,; -> terminal,int -> terminal,float -> terminal,if -> terminal,( -> terminal,) -> terminal,{ -> terminal,} -> terminal,else -> terminal
   //Non-terminals: METHOD_BODY -> Non-terminal,STATEMENT_LIST -> Non-terminal,STATEMENT -> Non-terminal,DECLARATION -> Non-terminal,IF -> Non-terminal

/*
    for(Symbol* symbol : this->get_terminals()){
        cout << symbol->get_symbol_string() << " -> terminal" << endl;
    }
     for(Symbol* symbol : this->get_non_terminals()){
        cout << symbol->get_symbol_string() << " -> Non-terminal" << endl;
    }
*/
}


// Parse the string by the '#' character as it precedes every grammar rule.
// And send it to the add_new_rule to parse it and make new Symbol.
void CFG_Parser::parse_rules_string(string rules) {
    // Production rule can be expanded over many lines where the '#' may be inside the rule.
    vector<string> rules_tokens = split(rules, '#', true);

    /*
    for(int  i = 0 ; i < rules_tokens.size(); i++){
        cout << i << " " << rules_tokens[i] << endl;
    }
    */
    for(string rule : rules_tokens) {
        add_new_rule(rule);
    }

    // eliminate left recursion and left factor
}

void CFG_Parser::add_new_rule(string rule_string) {
    //cout << "Before - > "<< rule_string;
    clean_rule_string(rule_string);
    //cout << "After - > " << rule_string << endl;

    if (rule_string == "") return;

    // adjust Lambda format
    replace_string(rule_string, "'\\L'", "\\L");
    replace_string(rule_string, "\\L", "'\\L'");

    //cout << "lamda - > " << rule_string << endl;
    // push rule string to vector
    string_rules.push_back(rule_string);

    // parse rule_string to Rule
    Rule* rule ;
    Rule::RuleStringStruct rst = rule->split_rule_string(rule_string); // get the lhs and rhs as (METHOD_BODY, STATEMENT_LIST) & (STATEMENT_LIST, STATEMENT | STATEMENT_LIST STATEMENT)

    if (Rule* rule_found = find_rule_with_lhs(rst.lhs)) { // added before
        rule_found->parse_productions_from_string(rst.rhs);
    }
    else { // Not added before.
        Rule* new_rule = new Rule(rst.lhs, this);
        new_rule->parse_productions_from_string(rst.rhs);
    }
}

void CFG_Parser::clean_rule_string(string &rule_string) {
    replace_all(rule_string, "\f\n\r\t\v", ' ');
    // Clear duplicate consecutive spaces. Replace('\s+', '\s').
    remove_duplicate_consecutive(rule_string, ' ');
    // Trimming right and left extra spaces.
    remove_right_left_spaces(rule_string);
}


Rule* CFG_Parser::find_rule_with_lhs(const string &lhs) {
    for (Rule* rule : rules) {
        if(rule->get_rule_name() == lhs) return rule; // exist before.
    }
    return NULL; // not exist.
}
bool CFG_Parser::find_symbol_with_name(const string &name, string type) {
    if(type == "terminal")
    {
        for (Symbol* s : terminals) {
            if(s->get_symbol_string() == name ) return true; // exist before.
        }
    }
    else
    {
        for (Symbol* s : non_terminals) {
            if(s->get_symbol_string() == name) return true; // exist before.
        }
    }

    return false; // not exist.
}
// check left recursion and left factor
bool CFG_Parser::is_LL_grammar()
{
    bool left_rec = false, left_fac = false, left_fac_r = false, left_rec_r = false;
    for(Rule *rule : rules)
    {
        left_rec_r = left_fac_r = false;
        for(int i=0; i< (int)rule->get_productions().size(); i++)
        {
            vector<Symbol*> production = rule->get_productions()[i];
            // Check Left Recursion
            if(production[0]->get_symbol_type() == Symbol::Type::NonTerminal && production[0]->get_symbol_string() == rule->get_rule_name())
            {
                if (! left_rec && ! left_fac){
                    cout << "Not LL(1) Grammar;" <<endl;
                    left_rec = true;
                }

                if(! left_rec_r){
                   cout << "Left Recursion in:  "<< rule->get_rule_name()<<endl;
                    left_rec_r = true;
                }
            }
            else if (production[0]->get_symbol_type() == Symbol::Type::NonTerminal){
                vector<vector<Symbol*> > prods = rules[find_rule_index(production[0]->get_symbol_string())]->get_productions();
                for(int k=0; k<prods.size(); k++){
                    if(prods[k][0]->get_symbol_type() == Symbol::Type::NonTerminal && prods[k][0]->get_symbol_string() == rule->get_rule_name())
                    {
                        if (! left_rec && ! left_fac){
                            cout << "Not LL(1) Grammar;" <<endl;
                            left_rec = true;
                        }

                        if(! left_rec_r){
                           cout << "Left Recursion in:  "<< rule->get_rule_name()<<endl;
                            left_rec_r = true;
                        }
                    }
                }
            }
            // Check Left Factor
            Symbol * current_first_symbol = production[0];
            for(int k=i+1; i< (int)rule->get_productions().size() - 1 && k < (int)rule->get_productions().size(); k++) {
                Symbol * next_first_symbol = rule->get_productions()[k][0];
                if(next_first_symbol->get_symbol_type() == current_first_symbol->get_symbol_type()
                   && next_first_symbol->get_symbol_string() == current_first_symbol->get_symbol_string()) {
                    if (! left_rec && ! left_fac){
                        cout << "Not LL(1) Grammar;" <<endl;
                        left_fac = true;
                    }

                    if(! left_fac_r){
                       cout << "Left Factor in:  "<< rule->get_rule_name()<<endl;
                        left_fac_r = true;
                    }
                }
                else if(next_first_symbol->get_symbol_string() != rule->get_rule_name()){
                    vector<vector<Symbol*> > prods = rules[find_rule_index(next_first_symbol->get_symbol_string())]->get_productions();
                    for(int k=0; k<prods.size(); k++){
                        if(prods[k][0]->get_symbol_type() == current_first_symbol->get_symbol_type()
                           && prods[k][0]->get_symbol_string() == current_first_symbol->get_symbol_string()) {
                            if (! left_rec && ! left_fac){
                                cout << "Not LL(1) Grammar;" <<endl;
                                left_fac = true;
                            }

                            if(! left_fac_r){
                               cout << "Left Factor in:  "<< rule->get_rule_name()<<endl;
                                left_fac_r = true;
                            }
                        }
                    }

                }
            }
        }
    }
    return left_rec || left_fac;
}

int CFG_Parser::find_rule_index(string s)
{
    for(int i=0; i<rules.size(); i++){
        if(rules[i]->get_rule_name() == s) return i;
    }
    return 0;
}

vector<Rule*> CFG_Parser::get_rules()
{
    return rules;
}

vector<string> CFG_Parser::get_string_rules()
{
    return string_rules;
}
vector<Symbol*> CFG_Parser::get_terminals()
{
    return terminals;
}

vector<Symbol*> CFG_Parser::get_non_terminals()
{
    return non_terminals;
}

#include "Lexical_Analyzer.h"
#include <stdio.h>
Lexical_Analyzer::Lexical_Analyzer(string filename) // take input file
{
    // Grammar parsing and construct NFA
    GrammerParser* gr = new GrammerParser("regular_expressions.txt");
    Graph * nfa_graph = gr->getFullNFA(); // get the generated graph of the regular expressions joined together
    vector<string> input = gr->get_weights(); // a-z A-z < < = > ! 0-9 () {} ; . / * + -
    vector<string> tokens = gr->get_expressions(); // id boolean int float num relop assign if else while ; , ( ) { } addop mulop
    /*
    cout << " ---- " <<tokens.size() << endl;
    for(int i = 0; i < tokens.size();i++){
        cout << tokens[i] << " " << endl;
    }
    cout << endl;
     cout << " ---- " <<input.size() << endl;
    for(int i = 0; i < tokens.size();i++){
        cout << input[i] << " " << endl;
    }
    cout << endl;
    */



    // NFA to DFA
    NFA_To_DFA dfa(nfa_graph, input, tokens);
    vector < vector < DFA_State > > dfa_table = dfa.get_DFA_table(); // contains each DFA_state with its nfa_states with the accepting_token
    vector < DFA_State > dfa_states = dfa.get_DFA_states();

/*
    cout << " - " << dfa_states.size() << endl;
    for(int i = 0; i < dfa_states.size(); i++){
            vector<int> temp = dfa_states[i].get_nfa_states_nums();
        for(int j = 0; j < temp.size() ; j++){
            cout <<  temp[j] << " " ;
        }
        cout << endl;
    }
*/
    unordered_map<string, int> acc_tokens_map;
    for(int i=tokens.size()-1; i>=0; i--)
    {
        acc_tokens_map[tokens[i]] = i;
    }


    // Minimization
    Minimization *minimizer = new Minimization(acc_tokens_map);
    vector < vector < DFA_State > > min_dfa = minimizer->minimize_DFA(dfa_table, dfa_states);
    vector < DFA_State > min_states = minimizer->get_minimum_states();

    map<char, int> input_map_table;
    for(int i=0; i<input.size(); i++)
    {
        input_map_table[input[i][0]] = i;
    }

/*  for(auto it = input_map_table.cbegin(); it != input_map_table.cend(); ++it)
    {
        std::cout << it->first << " " << it->second << "\n";
    }
*/

    Tokens *token_manager = new Tokens(min_dfa, input_map_table, min_states, filename);
    string token = token_manager->getToken();
    while(token != ""){
        token = token_manager->getToken();
    }
}
Lexical_Analyzer::~Lexical_Analyzer()
{

}

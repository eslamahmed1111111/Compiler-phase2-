#include "NFA_To_DFA.h"
#include "DFA_State.h"
#include "State.h"
#include <queue>

NFA_To_DFA::NFA_To_DFA() {}
// input = a-z A-z < = > ! 0-9 () {} ; . / * + - tokens = id boolean int float num relop assign if else while ; , ( ) { } addop mulop
NFA_To_DFA::NFA_To_DFA(Graph * NFA, vector<string> input, vector<string> tokens)
{
    /*
    State* s = NFA->get_start_state();
    queue<State*> q;
    q.push(s);

    while(q.size() != 0){
        cout << q.front()->get_state_num() << endl;
        vector<Edge>* childrens;
        childrens = q.front()->get_children();
        q.pop();
        for(int i = 0;i<childrens->size();i++){
            q.push((*childrens)[i].getTo());
            cout << (*childrens)[i].get_weight() << "->" << (*childrens)[i].getTo()->get_state_num() << endl;
        }
    }
*/

    for(int i=tokens.size()-1; i>=0; i--)
    {
        acc_tokens_map[tokens[i]] = i;
        //cout << tokens[i] << " " << i << endl;
    }
    unordered_map<string,int> input_map_table;//weights
    for(int i=0; i<input.size(); i++)
    {
        input_map_table[input[i]] = i;
        //cout << input[i] << " " << i << endl;
    }
    DFA_BFS(NFA->get_start_state(), input_map_table); // generate transition table of NFA to make it more easily for conversion to DFA
    //print_DFA_trans_table();
    do_subset_construction(input_map_table.size());
}

/*
 * BFS on the nfa graph to produce the transition table of the nfa
 * input: start state of the graph
 * input_map_table to map the input characters and their order in the transition table
 */
void NFA_To_DFA::DFA_BFS(State *start_state, unordered_map<string,int> input_map_table)
{
    vector<int> e_clos_row;
    vector<int> nfa_trans_row;

    // counter to add the new state number to the state_map_table as key and the counter as a value
    int state_count = 0;
    queue <State*> q;

    // map of key = state_num , value = index of the state in transition table
    unordered_map<int,int> state_map_table;

    State* temp_state;
    // Children of old graph
    vector<Edge> *children;

    temp_state = start_state;
    // Start_state is accepting and accepted token
    //cout << temp_state->is_accepting_state() << endl;
    //cout << temp_state->get_accepted_token() << endl;
    accepted_states.push_back(temp_state->is_accepting_state());
    accepted_tokens.push_back(temp_state->get_accepted_token());

    //Push start State
    q.push(temp_state);
    state_map_table.insert(pair<int,int>(temp_state->get_state_num(), state_count++));

    while(q.size()!=0)
    {
        temp_state = q.front(); // old State
        q.pop();
        // state children
        children = temp_state->get_children();
        State * state;
        // iterate over the children to fill the transition table and epsilon table
        for(int i=0; i< children->size(); i++)
        {
            pair<State*, State*> p = (*children)[i].get_start_end_states(); // children #i
            state = p.second;
            // Visited State => add to new graph only
            if( state_map_table.find(state->get_state_num()) != state_map_table.end() )
            {
                // if epsilon add it to epsilon closure
                if( (*children)[i].get_weight() == "")
                {
                    e_clos_row.push_back(state_map_table[state->get_state_num()]);
                    //cout << "1" << endl;
                    //cout << state_map_table[state->get_state_num()] << endl;
                }
                // not epsilon, add it trans_table
                else
                {
                    //cout << "2" << endl;
                    // add state index to transition table
                    nfa_trans_row.push_back(state_map_table[state->get_state_num()]);
                    // add input for the previous state to the table
                    nfa_trans_row.push_back(input_map_table[(*children)[i].get_weight()]);
                }
                continue;
            }
            //cout << "3" << endl;
            //Not visited

            // New_state is_accepting and accepted token for each state you know if it accepting and what its acceptance
            accepted_states.push_back(state->is_accepting_state());
            accepted_tokens.push_back(state->get_accepted_token());

            //Push next State
            q.push(state);
            state_map_table.insert(pair<int,int>(state->get_state_num(), state_count++));
            // if epsilon add it to epsilon closure
            if( (*children)[i].get_weight() == "")
            {
                e_clos_row.push_back(state_map_table[state->get_state_num()]);
            }
            // not epsilon, add it trans_table
            else
            {
                // add state index to transition table
                nfa_trans_row.push_back(state_map_table[state->get_state_num()]);
                // add input for the previous state to the table
                nfa_trans_row.push_back(input_map_table[(*children)[i].get_weight()]);
            }
        }
        // nfa_trans_row contains index of state and weight
        if (nfa_trans_row.size()<2)
        {
            //cout << "here" << nfa_trans_row.size() << endl;
            nfa_trans_row.push_back(-1);
            nfa_trans_row.push_back(-1);
        }
        // Add row to table and clear the row vector
        /*
        for(int  i = 0; i < e_clos_row.size(); i++){
            cout << e_clos_row[i] << " " ;
        }
        cout << endl;
        for(int  i = 0; i < nfa_trans_row.size(); i++){
            cout << nfa_trans_row[i] << " " ;
        }
        cout << endl;
        */
        /* its simply an adjecency matrix contains states that
          state  nfa_trans_table         ep_clos_table
            0    1051                    234

            here state 0 can reach state 1 with weight 0('b') and also state 5 with weight 1('i') and can reach states 2,3,4 with epsilon

            each state saved its acceptance
            states              0        1
            accepted_states     0        1
            accepted_tokens    ' '      'if'
        */
        nfa_trans_table.push_back(nfa_trans_row); // contains at state a all states that a can reach with weight .. like 1 0
        ep_clos_table.push_back(e_clos_row);// contains all states that a reach with epsilon
        e_clos_row.clear();
        nfa_trans_row.clear();
    }
}
void NFA_To_DFA::print_DFA_trans_table()
{
    cout<< "*******************************************"<<endl;
    cout<< "Transition Table :" << endl;
    for(int i=0 ; i<nfa_trans_table.size() ; i++)
    {
        vector<int> v = nfa_trans_table[i];
        if(v.size()>2)
        {
            cout<< "Row " << i << endl;
            cout<< endl;
            for(int j=0 ; j < v.size() ; j++)
                cout<< " " <<v[j];
            cout<< " |  Is acceptance? = "  << accepted_states[i] << " | Token Type = " << accepted_tokens[i] << "| Epsilon Closure ";
            vector<int> e = ep_clos_table[i];
            for(int j = 0 ; j < e.size(); j++)
                cout<< " " << e[j];
            cout<< endl;
        }
    }
    cout<< "End **********************************"<<endl;
}
void NFA_To_DFA::do_subset_construction (int numberOfInputs) // numberOfInputs = 77 all unique weights in regular expression file
{
    int nfa_size = nfa_trans_table.size (); // number of states in nfa transition table

    if (nfa_size == -1)
        return;
    /*
    for(int i = 0; i < 10;i++){
            cout << "state " << i <<  "->";
        for(int j = 0 ; j < nfa_trans_table[i].size();j+=2){
            cout << nfa_trans_table[i][j] << " w= " << nfa_trans_table[i][j+1] << " , ";
        }
        cout << endl;
    }
    for(int i = 0; i < 10;i++){
            cout << "state "<< i << "-> " ;
        for(int j = 0 ; j < ep_clos_table[i].size();j++){
            cout << ep_clos_table[i][j] << " ";
        }
        cout << endl;
    }
    */
    // indexes to iterate over NFA and DFA states
    int dfa_index = 0, nfa_index =0 ;
    // counter to make the new state with this counter as a state number
    int state_count = 0;
    string aaccepted_token = "";

    DFA_State start_state = DFA_State (nfa_size); // initialy it can contains all nfs states false so there is an boolean array each index represent if it in this DFA_STATE if true
    start_state.set_state_num(state_count);
    // Add start State to DFA_State
    start_state.add_nfa_state(0, accepted_states[0]);
    if (accepted_states[0])
        aaccepted_token = accepted_tokens[0];
    // Add Epsilon Closure of first State to DFA_State
    while(nfa_index<start_state.get_nfa_states_nums().size()) // let state 0 -e-> 1,2,3,4,5,6
    {
        int state_number = start_state.get_nfa_states_nums()[nfa_index];
        for(int i =0 ; i<ep_clos_table[state_number].size() ; i++)
        {
            int state = ep_clos_table[state_number][i]; // 1
            start_state.add_nfa_state(state, accepted_states[state]);
            if (accepted_states[state] && (aaccepted_token =="" || acc_tokens_map[aaccepted_token] < acc_tokens_map[accepted_tokens[state]]) )
                aaccepted_token =accepted_tokens[state];

        }
        nfa_index++;
    }
    nfa_index = 0;
    state_count++;
    // Add start State to DFA_states Vector
    start_state.set_accepting(start_state.is_accepting_state(), aaccepted_token);
    DFA_states.push_back(start_state);
    accepted_tokens.push_back(aaccepted_token);
    // Start while loop to add all new states

    while (dfa_index < DFA_states.size())
    {
        vector <DFA_State> DFA_table_row ;
        DFA_State currentState = DFA_states[dfa_index];
        vector <int> cur_state_nums = currentState.get_nfa_states_nums();
        for (int i=0 ; i<numberOfInputs; i++) // check under all weights value which states to reach
        {
            DFA_State to = DFA_State (nfa_size);
            to.set_state_num(state_count);
            aaccepted_token = "";
            for (int j=0 ; j<cur_state_nums.size(); j++)
            {
                int tempState =  cur_state_nums[j];
                for(int k=0; k<nfa_trans_table[tempState].size(); k+=2)
                {

                    int trans_state_num = nfa_trans_table[tempState][k];

                    if (trans_state_num != -1 && nfa_trans_table[tempState][k+1] == i)
                    {

                        to.add_nfa_state(trans_state_num, accepted_states[trans_state_num]);
                        if (accepted_states[trans_state_num] && (aaccepted_token =="" || acc_tokens_map[aaccepted_token] < acc_tokens_map[accepted_tokens[trans_state_num]]) )
                        {
                            aaccepted_token = accepted_tokens[trans_state_num];
                        }
                    }
                }
            }
            // repeat again and find all reachable states with epsilon weight
            while(nfa_index < to.get_nfa_states_nums().size())
            {
                int to_state_num = to.get_nfa_states_nums()[nfa_index];
                for(int i=0 ; i<ep_clos_table[to_state_num].size() ; i++)
                {
                    int state = ep_clos_table[to_state_num][i];
                    to.add_nfa_state(state, accepted_states[state]);
                    if (accepted_states[state] && (aaccepted_token =="" || acc_tokens_map[aaccepted_token] < acc_tokens_map[accepted_tokens[state]]))
                        aaccepted_token = accepted_tokens[state];
                }
                nfa_index++;
            }
            nfa_index = 0;
            if (!to.is_empty_state())
            {

                int found = get_dfa_state_index(to);
                if (found == -1)
                {
                    to.set_accepting(to.is_accepting_state(), aaccepted_token);
                    DFA_states.push_back(to);
                    accepted_tokens.push_back(aaccepted_token);
                    state_count ++ ;
                }
                else
                    to = DFA_states[found];
            }
            else
            {
                to.set_state_num(-1);
            }

            DFA_table_row.push_back (to);
        }
        dfa_index ++;
        DFA_table.push_back(DFA_table_row);

    }

}
int NFA_To_DFA::get_dfa_state_index (DFA_State state)
{
    vector <bool> acc = state.get_nfa_states_accept();
    for (int i = 0 ; i<DFA_states.size(); i++)
    {
        vector <bool> acc2 = DFA_states[i].get_nfa_states_accept();
        if (acc == acc2)
            return i;
    }
    return -1;
}


vector < vector < DFA_State > > NFA_To_DFA::get_DFA_table ()
{
    return DFA_table;
}

vector < DFA_State> NFA_To_DFA::get_DFA_states ()
{
    return DFA_states;
}

vector<string> NFA_To_DFA::get_accepted_tokens()
{
    return new_accepted_tokens;
}


NFA_To_DFA::~NFA_To_DFA()
{
    //Destructor
}

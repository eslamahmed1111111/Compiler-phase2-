#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include "GrammerParser.h"
#include "NFA_TO_DFA.h"
#include "Minimization.h"
#include "DFA_State.h"
#include "Tokens.h"
#include "State.h"
#include "Edge.h"
#include "Graph.h"
#include "BFS.h"

#include <bits/stdc++.h>

using namespace std;

class Lexical_Analyzer
{
    public:
        Lexical_Analyzer(string file_path);
        virtual ~Lexical_Analyzer();

    protected:

    private:
};

#endif // LEXICAL_ANALYZER_H

#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>

using namespace std;

typedef char Symbol;
typedef int State;
typedef unordered_map<Symbol, unordered_set<State>> TransitionMap;
typedef unordered_map<State, TransitionMap> NFATransitions;
typedef unordered_set<State> DFAState;
struct unordered_set_state_hash
{
    std::size_t operator()(const DFAState &state) const
    {
        std::size_t seed = 0;
        for (const auto &s : state)
        {
            seed ^= std::hash<State>{}(s) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};
typedef unordered_map<DFAState, TransitionMap, unordered_set_state_hash> DFATransitions;

unordered_set<State> epsilonClosure(const NFATransitions &nfa, const State &state)
{
    unordered_set<State> closure;
    vector<State> stack;
    stack.push_back(state);
    closure.insert(state);

    while (!stack.empty())
    {
        State current = stack.back();
        stack.pop_back();

        if (nfa.find(current) != nfa.end())
        {
            const TransitionMap &transitions = nfa.at(current);
            if (transitions.find('\'') != transitions.end())
            {
                const unordered_set<State> &epsilonTransitions = transitions.at('\'');
                for (const auto &nextState : epsilonTransitions)
                {
                    if (closure.find(nextState) == closure.end())
                    {
                        closure.insert(nextState);
                        stack.push_back(nextState);
                    }
                }
            }
        }
    }

    return closure;
}
DFATransitions convertNFAtoDFA(const NFATransitions &nfa, const unordered_set<Symbol> &alphabet, const State &startState, const unordered_set<State> &finalStates)
{
    DFATransitions dfa;
    queue<DFAState> stateQueue;
    DFAState initialState = epsilonClosure(nfa, startState);
    dfa[initialState] = TransitionMap(); // Initialize the initial state with an empty TransitionMap
    stateQueue.push(initialState);

    while (!stateQueue.empty())
    {
        DFAState currentState = stateQueue.front();
        stateQueue.pop();

        for (const auto &symbol : alphabet)
        {
            DFAState nextState;
            for (const auto &state : currentState)
            {
                if (nfa.find(state) != nfa.end())
                {
                    const TransitionMap &transitions = nfa.at(state);
                    if (transitions.find(symbol) != transitions.end())
                    {
                        const unordered_set<State> &transitionedStates = transitions.at(symbol);
                        for (const auto &transitionedState : transitionedStates)
                        {
                            unordered_set<State> closure = epsilonClosure(nfa, transitionedState);
                            nextState.insert(closure.begin(), closure.end());
                        }
                    }
                }
            }

            if (dfa[currentState].find(symbol) == dfa[currentState].end())
            {
                dfa[currentState][symbol] = nextState;
                if (dfa.find(nextState) == dfa.end())
                {
                    dfa[nextState] = TransitionMap();
                    stateQueue.push(nextState);
                }
            }
        }
    }

    return dfa;
}

int main()
{
    int numStates;
    cout << "Enter the number of states: ";
    cin >> numStates;

    unordered_set<State> finalStates;
    cout << "Enter the accepting states (separated by comma): ";
    string finalStatesInput;
    cin >> finalStatesInput;

    size_t pos = 0;
    while ((pos = finalStatesInput.find(',')) != string::npos)
    {
        finalStates.insert(stoi(finalStatesInput.substr(0, pos)));
        finalStatesInput.erase(0, pos + 1);
    }
    finalStates.insert(stoi(finalStatesInput));

    NFATransitions nfa;
    unordered_set<Symbol> alphabet = {'a', 'b'};

    cout << "Enter the transitions in the format 'from_state:transition_alphabet:to_state'. Type 'end' to finish." << endl;
    string transition;
    /* while (true)
    {
        cout << "> ";
        cin >> transition;
        if (transition == "end")
        {
            break;
        }

        size_t colonPos1 = transition.find(':');
        size_t colonPos2 = transition.find(':', colonPos1 + 1);

        if (colonPos1 == string::npos || colonPos2 == string::npos)
        {
            cout << "Invalid transition format. Please try again." << endl;
            continue;
        }

        State fromState = stoi(transition.substr(0, colonPos1));
        string symbolStr = transition.substr(colonPos1 + 1, colonPos2 - colonPos1 - 1);
        State toState = stoi(transition.substr(colonPos2 + 1));

        for (char symbol : symbolStr)
        {
            nfa[fromState][symbol].insert(toState);
        }
    } */

    while (true) {
    cin >> transition;
    if (transition == "end") {
        break;
    }

    size_t colonPos1 = transition.find(':');
    size_t colonPos2 = transition.find(':', colonPos1 + 1);

    if (colonPos1 == string::npos || colonPos2 == string::npos) {
        cout << "Invalid transition format. Please try again." << endl;
        continue;
    }

    State fromState = stoi(transition.substr(0, colonPos1));
    string symbolStr = transition.substr(colonPos1 + 1, colonPos2 - colonPos1 - 1);

    size_t commaPos = colonPos2;
    while ((commaPos = transition.find(',', commaPos + 1)) != string::npos) {
        State toState = stoi(transition.substr(colonPos2 + 1, commaPos - colonPos2 - 1));
        for (char symbol : symbolStr) {
            nfa[fromState][symbol].insert(toState);
        }
        colonPos2 = commaPos;
    }

    State toState = stoi(transition.substr(colonPos2 + 1));
    for (char symbol : symbolStr) {
        nfa[fromState][symbol].insert(toState);
    }
}


    State startState = 0;

    DFATransitions dfa = convertNFAtoDFA(nfa, alphabet, startState, finalStates);
    unordered_set<int> startingStates;
    //cout << "Starting state: ";
    DFAState initialState = epsilonClosure(nfa, startState);
    for (const auto &state : initialState)
    {
        startingStates.insert(state);
    }
    if (startingStates.empty())
    {
        cout << "0";
    }
    else if (startingStates.size() == 1 && startingStates.count(0) == 1)
    {
        std::cout << "1";
    }
    else if (startingStates.size() == 1 && startingStates.count(1) == 1)
    {
        std::cout << "2";
    }
    else if (startingStates.size() == 1 && startingStates.count(2) == 1)
    {
        cout << "3";
    }
    else if (startingStates.size() == 2 && startingStates.count(0) == 1 && startingStates.count(1) == 1)
    {
        cout << "4";
    }
    else if (startingStates.size() == 2 && startingStates.count(0) == 1 && startingStates.count(2) == 1)
    {
        cout << "5";
    }
    else if (startingStates.size() == 2 && startingStates.count(1) == 1 && startingStates.count(2) == 1)
    {
        cout << "6";
    }
    else if (startingStates.size() == 3 && startingStates.count(0) == 1 && startingStates.count(1) == 1 && startingStates.count(2) == 1)
    {
        cout << "7";
    }
    cout << endl;
    //cout << "Final states: ";
    unordered_set<int> final_states;
    for (const auto &stateTransitions : dfa)
    {
        const DFAState &currentState = stateTransitions.first;
        bool isFinalState = false;
        for (const auto &state : currentState)
        {
            if (finalStates.find(state) != finalStates.end())
            {
                isFinalState = true;
                break;
            }
        }
        if (isFinalState)
        {
            for (const auto &state : currentState)
            {
                final_states.insert(state);
            }
            if (final_states.empty())
            {
                cout << "0";
            }
            else if (final_states.size() == 1 && final_states.count(0) == 1)
            {
                std::cout << "1";
            }
            else if (final_states.size() == 1 && final_states.count(1) == 1)
            {
                std::cout << "2";
            }
            else if (final_states.size() == 1 && final_states.count(2) == 1)
            {
                cout << "3";
            }
            else if (final_states.size() == 2 && final_states.count(0) == 1 && final_states.count(1) == 1)
            {
                cout << "4";
            }
            else if (final_states.size() == 2 && final_states.count(0) == 1 && final_states.count(2) == 1)
            {
                cout << "5";
            }
            else if (final_states.size() == 2 && final_states.count(1) == 1 && final_states.count(2) == 1)
            {
                cout << "6";
            }
            else if (final_states.size() == 3 && final_states.count(0) == 1 && final_states.count(1) == 1 && final_states.count(2) == 1)
            {
                cout << "7";
            }
        }
    }
    cout << endl;
    //cout << "DFA transitions:" << endl;
    int current_state = 0;

    for (const auto &stateTransitions : dfa)
    {

        if (stateTransitions.first.empty())
        {

            current_state = 0;
        }
        else if (stateTransitions.first.size() == 1 && stateTransitions.first.count(0) == 1)
        {

            current_state = 1;
        }
        else if (stateTransitions.first.size() == 1 && stateTransitions.first.count(1) == 1)
        {

            current_state = 2;
        }
        else if (stateTransitions.first.size() == 1 && stateTransitions.first.count(2) == 1)
        {
            current_state = 3;
        }
        else if (stateTransitions.first.size() == 2 && stateTransitions.first.count(0) == 1 && stateTransitions.first.count(1) == 1)
        {
            current_state = 4;
        }
        else if (stateTransitions.first.size() == 2 && stateTransitions.first.count(0) == 1 && stateTransitions.first.count(2) == 1)
        {
            current_state = 5;
        }
        else if (stateTransitions.first.size() == 2 && stateTransitions.first.count(1) == 1 && stateTransitions.first.count(2) == 1)
        {
            current_state = 6;
        }
        else if (stateTransitions.first.size() == 3 && stateTransitions.first.count(0) == 1 && stateTransitions.first.count(1) == 1 && stateTransitions.first.count(2) == 1)
        {
            current_state = 7;
        }

        for (const auto &transition : stateTransitions.second)
        {
            cout << current_state << ":" << transition.first << ":";

            if (transition.second.empty())
            {
                cout << "0";
            }
            if (transition.second.size() == 1 && transition.second.count(0) == 1)
            {
                cout << "1";
            }
            else if (transition.second.size() == 1 && transition.second.count(1) == 1)
            {
                cout << "2";
            }
            else if (transition.second.size() == 1 && transition.second.count(2) == 1)
            {
                cout << "3";
            }
            else if (transition.second.size() == 2 && transition.second.count(0) == 1 && transition.second.count(1) == 1)
            {
                cout << "4";
            }
            else if (transition.second.size() == 2 && transition.second.count(0) == 1 && transition.second.count(2) == 1)
            {
                cout << "5";
            }
            else if (transition.second.size() == 2 && transition.second.count(1) == 1 && transition.second.count(2) == 1)
            {
                cout << "6";
            }

            else if (transition.second.size() == 3 && transition.second.count(0) == 1 && transition.second.count(1) == 1 && transition.second.count(2) == 1)
            {
                cout << "7";
            }
            cout << endl;
        }
    }

    return 0;
}
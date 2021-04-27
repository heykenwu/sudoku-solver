// Test code that combines all your implementation from Questions 1 to 3 in Assignment 2
// compile and run with:
//          g++ -std=c++11 COMP8042A2Test.cpp -o COMP8042A2Test && ./COMP8042A2Test
#include<iostream>
#include<vector>
#include "Graph.h"
#include "CuckooHashing.h"
#include "Sudoku.h"

using namespace std;

int main() {
    /*
    Example Run Results:
    Results of topological sort: 0 1 2 3
    Contents of Cuckoo hash table:
    Table 0:	| F	| D	| G	| E	| C
    Table 1:	| A	| 	| 	| B	|
                  Contents of Cuckoo hash table:
    Table 0:	| 53	| 	| 	| 36	| 67	| 50	| 20	| 75	| 	| 39	| 100
    Table 1:	| 	| 	| 	| 	| 	| 	| 	| 	| 105	| 	| 3
    -------------------------
    SOLVED!
    -------------------------
    8  1  2  7  5  3  6  4  9
    9  4  3  6  8  2  1  7  5
    6  7  5  4  9  1  2  8  3
    1  5  4  2  3  7  8  9  6
    3  6  9  8  4  5  7  2  1
    2  8  7  1  6  9  5  3  4
    5  2  1  9  7  4  3  6  8
    4  3  8  5  2  6  9  1  7
    7  9  6  3  1  8  4  5  2

    Process finished with exit code 0
    */
    int adjacencyMatrix[4][4] = {0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0};
    Graph graph;
    graph.CreateGraph(adjacencyMatrix);
    cout << "Results of topological sort: ";
    cout << graph.TopologicalSort() << endl;

    CuckooHashTable t1(5), t2(11);
    t1.Add("A");
    t1.Add("B");
    t1.Add("C");
    t1.Add("D");
    t1.Add("E");
    t1.Add("F");
    t1.Add("G");
    cout << t1.Print();
    int keys_1[] = { 20,50,53,75,100,67,105,3,36,39 };
    for (auto k:keys_1)
        cout << t2.Add(std::to_string(k));
    cout << t2.Print();

    int sudoku[9][9] = {
            {8, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 3, 6, 0, 0, 0, 0, 0},
            {0, 7, 0, 0, 9, 0, 2, 0, 0},
            {0, 5, 0, 0, 0, 7, 0, 0, 0},
            {0, 0, 0, 0, 4, 5, 7, 0, 0},
            {0, 0, 0, 1, 0, 0, 0, 3, 0},
            {0, 0, 1, 0, 0, 0, 0, 6, 8},
            {0, 0, 8, 5, 0, 0, 0, 1, 0},
            {0, 9, 0, 0, 0, 0, 4, 0, 0}};

    SudokuSolver solver(sudoku);
    if (solver.Solve()) {
        cout<<"-------------------------"<<endl;
        cout<<"         SOLVED!         "<<endl;
        cout<<"-------------------------"<<endl;
        cout << solver.Print();
    }
    else
        cout << "Solution Does Not Exist!" << endl;
    return 0;
}

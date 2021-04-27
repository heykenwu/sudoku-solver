#ifndef SUDOKO_H
#define SUDOKO_H
#include <stack>
#include <iostream>
/*
Hong Kit Wu, A00968591
Reference: https://www.ocf.berkeley.edu/~jchu/publicportal/sudoku/sudoku.paper.html
The algorithm design technique is the Dancing Links algorithm which is a backtracking 
algorithm designed for solving the exact cover problem. And Sudoku is one of the case that has
this problem.

Dancing Links solve the Exact Cover problem by putting it into a toroidal doubly-linked list (A matrix).
This matrix can only track the cells for each row that correspond to meeting a constraint.
For the rest of the cells that are not relevant will be ignored (will not be linked). 
The concept image is shown on the reference website under the section "The Dancing Links Algorithm concerning Exact Cover".
In the matrix, the column will be the constrains of the sudoku and the rows will be the entries of the cells.
(i.e. row 1 col 1 value 1 will be considered as one entry). 

In Sudoku, we will have 4 constrains for every cell:
1. A single value in each cell.
2. Values 1 through 9 must exist only once in each row.
3. Values 1 through 9 must exist only once in each column.
4. Values 1 through 9 must exist only once in each square.

Therefore, in the toroidal doubly-linked list, 
for columns (constrains), we will have totally 324 constrains (9 rows x 9 columns x 4 constrains).
For rows (entries), we will have 729 entries (9 rows x 9 columns x 9 (values 1 to 9)).
The dancing links algorithm has two main functions:
cover(): to remove a column from the matrix as well as remove all rows in the column from other columns they are in.
uncover(): for backtracking, recover the removed column and its related bottom nodes.
Another part that is important to solve the 0's is to choose the column with 
the least number of nodes to decrease the branching of the algorithm. It is a main part of the solve_the_zeros().
Every possible node will be passed to the stack in the solve_the_zeros(). If the node lead to a failed result,
it will pop out from the stack and then do a uncover for that column. 
When all the constrains are satisfy (Root pointer points to itself), it means that a compelete solution
will be stored in the stack. 
And then what we do is to pop the number from the stack and add each number into the puzzle 2D array.
If there is still a column left and no nodes under it at the end, it means there is no solution for this puzzle. 
*/
using namespace std;
//Node for the toroidal doubly-linked list. Each node will have top bottom left right Node pointer that points to its neigbhors.
class Node {
    public:
        Node* top;
        Node* bottom;
        Node* right;
        Node* left;
        Node* col_header;

        //column number
        int col;
        //row number
        int row;
        //value
        int val;

        Node(){
            top = NULL;
            bottom = NULL;
            right = NULL;
            left = NULL;
            col_header = NULL;
            col = 0;
            row = 0;
            val = 0;
        };
        Node(int r, int c, int value): row(r), col(c), val(value) {
            top = NULL;
            bottom = NULL;
            right = NULL;
            left = NULL;
            col_header = NULL;
        };
};

class SudokuSolver {
    private:
        int puzzle[9][9]{};
        Node* Root;
        bool finish;
        stack<Node> solution;
    public:
        explicit SudokuSolver(int (&p)[9][9]){
            //set the value of finish to be false (unsolved)
            finish = false;
            Root = new Node();
            //Bind the top bottom left right to the Root itself.
            Root->top = Root;
            Root->bottom = Root;
            Root->left = Root;
            Root->right = Root;
            for (int i = 0; i < 9; ++i)
                for (int j = 0; j < 9; ++j)
                    puzzle[i][j] = p[i][j];
        }
        ~SudokuSolver(){
            //delete the nodes column by column
            Node* tmp;
            for (Node* col_head = Root->right; col_head != Root; col_head = tmp) {
                for (Node* row = col_head->bottom; row != col_head; row = tmp) {
                    tmp = row->bottom;
                    delete row;
                }
                tmp = col_head->right;
                delete col_head;
            }
            delete Root;
        }
        void init() {
            /*
                in this function we are going to construct the maximum matrix using by the dancing links algorithm
                row will be the entries and column will be the constrains of the 9x9 sudoku
                col(constrains) = 9*9 + 9*9 + 9*9 + 9*9 = 324
                entries = 9*9*9 = 729
                maxtrix size = 9;
                box size = 3 (9x9 sudoku contains 9 3x3 box); 
            */
            int row_max = 729;
            int col_max = 324;
            int m_size = 9;
            int box_size = 3;

            int offset_of_row = 81;

            Node* max_matrix[row_max][col_max];
            //initial the matrix (set all elements to null)
            for(int i = 0; i < row_max; i++) {
                for( int j = 0; j < col_max; j++) {
                    max_matrix[i][j] = NULL;
                }
            }

            //link the same cell in the 9x9 sudoku with different Nodes together (left and right) 
            for(int i = 0; i < m_size; i++) {
                for (int j = 0; j < m_size; j++) {
                    for (int x = 0; x < m_size; x++) {
                        //row offset for each entry (row, col, value) 
                        int row = (i * offset_of_row+ j * m_size+ x);
                        //column position corresponding the group of constrains (cell, row, column or box)
                        int cell_position = i*m_size + j;
                        int row_position = 9*9 + i*m_size+x;
                        int col_position = 9*9*2 + j*m_size+x;
                        int box_postion = 9*9*3 + ((i/box_size+j/box_size*box_size)*m_size+x);
                        //cell Node
                        max_matrix[row][cell_position] = new Node(i,j,x);
                        //row Node
                        max_matrix[row][row_position] = new Node(i,j,x);
                        //link cell Node and row Node together
                        max_matrix[row][cell_position]->right = max_matrix[row][row_position];
                        max_matrix[row][row_position]->left = max_matrix[row][cell_position];
                        //col Node
                        max_matrix[row][col_position] = new Node(i,j,x); 
                        //link row Node and col Node together
                        max_matrix[row][row_position]->right = max_matrix[row][col_position];
                        max_matrix[row][col_position]->left = max_matrix[row][row_position];
                        //box Node
                        max_matrix[row][box_postion] = new Node(i,j,x);
                        //link col Node and box Node together
                        max_matrix[row][col_position]->right = max_matrix[row][box_postion];
                        max_matrix[row][box_postion]->left = max_matrix[row][col_position];
                        //link box Node and cell Node together (circular: line the end Node to the start Node)
                        max_matrix[row][box_postion]->right = max_matrix[row][cell_position];
                        max_matrix[row][cell_position]->left = max_matrix[row][box_postion];
                    }
                }
            }
        //link the Nodes by constrains (link all nodes within the same column)
            Node* new_column_header;
            Node* previous_node;
            for(int i = 0; i < col_max; i++) {
                //create a new constrain (a column)
                new_column_header = new Node();
                new_column_header->top = new_column_header;
                new_column_header->bottom = new_column_header;
                new_column_header->left = new_column_header;
                new_column_header->right = new_column_header;
                new_column_header->col_header = new_column_header;
                previous_node = new_column_header;
                //links nodes within the same col
                for(int j = 0; j < row_max; j++) {
                    if(max_matrix[j][i] != NULL) {
                        previous_node->bottom = max_matrix[j][i];
                        max_matrix[j][i]->top = previous_node;
                        //circular linking end Node to start Node
                        max_matrix[j][i]->bottom = new_column_header;
                        new_column_header->top = max_matrix[j][i];
                        max_matrix[j][i]->col_header=new_column_header;
                        //save the current node used to link with the next node
                        previous_node = max_matrix[j][i];
                    }
                }
                /*
                    loop the constrain column and then add the new constrain header to the end of the headers
                    as well as connecting them together (like a double link list [col header]<->[col header]). 
                    (circular: the last constrain header node will point to the first constrain header node)
                */
                Node* current = Root;
                while(current->right != Root) {
                    current = current->right;
                }
                current->right->left = new_column_header;
                new_column_header->right = current->right;
                current->right = new_column_header;
                new_column_header->left = current;
            }
        }
        /*
            In this function we find the known clue in the matrix and then add it to the stack. 
        */
        void insert_known_cell() {
            int puzzle_size = 9;
            Node* the_node;
            for(int i = 0; i < puzzle_size; i++) {
                for(int j = 0; j < puzzle_size; j++) {
                    if(puzzle[i][j] != 0) {
                        the_node = find_node(i,j,puzzle[i][j] - 1);
                        if(the_node !=NULL) {
                            Node* col_to_cover = the_node->col_header;
                            Node* the_row_start = the_node;
                            cover(col_to_cover);
                            for(Node* row_tmp = the_row_start->right; row_tmp != the_row_start; row_tmp = row_tmp->right) {
                                cover(row_tmp);
                            }
                            solution.push(*the_node);
                        }
                    }
                }
            }
        }
        /*
            Recursive function: to find the column and satisfy them solve the zeros
            loop: 
            Step 1: find next available column
            Step 2: Cover the column
            Step 3: Cover the related entry under the column
            Step 4: enter to the recusive function again and again until it return false or true
                    If false encounter, it will do a uncover and loop through the another path until a true returned or go through all paths.
                    If true encounter, it will also do a uncover and the for loop will be done and return true. 
        */
        bool solve_the_zeros(){
            //constrains are satisfy
            if(Root->right == Root) {
                return true;
            }
            //num of Col
            int NumberNodeOfConstrain;
            // choose the column with the least number of nodes to decrease the branching of the algorithm
            Node* col_header = Root->right;
            int min_nodes = -1;
            
            for(Node* next_constrain = col_header; next_constrain != Root; next_constrain = next_constrain->right) {
                int current_count = 0;
                for(Node* next_bot = next_constrain->bottom; next_bot != next_constrain; next_bot = next_bot->bottom) {
                    current_count++;
                }
                if((min_nodes == -1) || (current_count < min_nodes)) {
                    col_header = next_constrain;
                    min_nodes = current_count;
                }
            }
            NumberNodeOfConstrain = min_nodes;

            if(NumberNodeOfConstrain < 1) {
                return false;
            }
            //cover the current choosen column header
            cover(col_header);
            for(Node* bot_node = col_header->bottom; (bot_node != col_header) && (!finish); bot_node = bot_node->bottom) {
                //push the node to stack
                solution.push(*bot_node);
                //cover the constrain and the related entry
                for(Node* right_node = bot_node->right; right_node != bot_node; right_node = right_node->right){
                    cover(right_node->col_header);
                }
                // choose another constrain with a node recursively until a true or false return
                finish = solve_the_zeros();
                //if the path is not right, pop the previous pushed node from the stack
                if(!finish) {
                    solution.pop();
                }
                //uncover the previous covered constrain and the related node
                for(Node* right_node = bot_node->right; right_node != bot_node; right_node = right_node->right){
                    uncover(right_node->col_header);
                }
            }
            //uncover the choosen column header
            uncover(col_header);
            return finish;
        }

        /* 
            cover the current column header and the nodes related to the nodes under the column header (unlink)
        */
        void cover(Node* col_) {
            // to cover the header by link the col header on its right to the right Node of the colheader on its left. 
            Node* col_head = col_->col_header;
            col_head->right->left = col_head->left;
            col_head->left->right = col_head->right;
            Node* bot_node = col_head->bottom;
            //loop through the bottom nodes under the column header
            while (bot_node != col_head) {
                Node* right_node = bot_node->right;
                //cover the entry except the node under this column header (cover the row)
                while(right_node != bot_node){
                    right_node->top->bottom = right_node->bottom;
                    right_node->bottom->top = right_node->top;  
                    right_node = right_node->right;
                }
                bot_node = bot_node->bottom;
            }
        }
        /*
        uncover the current column header and the nodes related to the nodes under the column header (link back)
        */
        void uncover(Node* col_) {
            Node* col_head = col_->col_header;
            Node* bot_node = col_head->bottom;
            col_head->left->right = col_head;
            col_head->right->left = col_head;
            while (bot_node != col_head){
                Node* right_node = bot_node->right;
                while (right_node != bot_node) {
                    right_node->top->bottom = right_node;
                    right_node->bottom->top = right_node;
                    right_node = right_node->right;
                }
                bot_node = bot_node->bottom;
            }
        }
        /*
        find node with the specific row col and value
        */
        Node* find_node(int r, int c, int v){
            for (Node* colNode = Root->right; colNode != Root; colNode = colNode->right) {
                for(Node* botNode = colNode->bottom; botNode != colNode; botNode = botNode->bottom){
                    if((botNode->row == r) && (botNode->col == c) && (botNode->val == v)) { 
                        return botNode;
                    }
                }
            }
            return NULL;
        }
        /*
        Solve function (return true or false):
            Step one: set the bool finish to be false (unsolved).
            Step two: init the toroidal doubly-linked list (A matrix).
            Step three: add the clues (known numbers) to the stack.
            Step four: solve the unknown number (0s)
            Step five: check the boolean. If true, a solution is found and we will add the result from the stack to the 2D array puzzle.
                       If false, no solution for the puzzle and we will just clear the stack.
        */
        bool Solve() {
            finish = false;
            init();
            insert_known_cell();
            finish = solve_the_zeros();
            if(finish) {
                while(!solution.empty()) {
                    Node answer = solution.top();
                    puzzle[answer.row][answer.col] = answer.val+1;
                    solution.pop();
                }
            } else {
                while(!solution.empty()) {
                    solution.pop();
                }
            }
            return finish;
        }
        
        string Print() {
            std::ostringstream os;
            for(auto & i : puzzle) {
                for (int j : i)
                    os << j << "  ";
                os << endl;
            }
            return os.str();
        }
};
#endif //SUDOKO_H

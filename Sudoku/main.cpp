//Project 2
//Lyzz Ma (hm1936)
//May 12, 2020

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

vector<vector<char>> horizontal; //store inequalities between horizontally-adjacent cells
vector<vector<char>> vertical; //store inequalities between vertically-adjacent cells

//get all the coordinates of all neighbors of State[x][y]
vector<vector<int>> getRelation(int x, int y);

//Store the information of each cell
struct Tile{
    //Constructor
    Tile(int data, int i, int j):value(data){ //coordinate:i, j
        //assign domain to default empty cell
        if(value == 0){
            for(int i =1; i<6; i++){
                domain.push_back(i);
            }
            legal_numbers = 5;
        }
        relations = getRelation(i, j);
    }
    
    Tile(vector<int> domain, int i, int j): domain(domain){
        value = 0;
        relations = getRelation(i, j);
    }
    
    //Parameters
    int value;
    vector<int> domain; //legal values left
    int legal_numbers;
    vector<vector<int>> relations; //neighbors
    int unassigned_neighbors;
    
    //to check whether the tile is empty
    bool isEmpty(){
        return value == 0;
    }
};

//Domain_reduction by forward checking
bool reduce_domain(vector<vector<Tile*>>& state, int x, int y);

//calculate and assign Numbers of Unassigned Neighbors in a state
void getUnassignedNeighbors(const vector<vector<Tile*>>& state);

//store the current state of the course
struct Node{
    //constructor
    Node(){}
    
    Node(const vector<vector<int>>& numbers, Node* parent): parent(parent){
        for(int i = 0; i< 5; i++){
            vector<Tile*> row;
            for(int j = 0; j < 5; j++){
                row.push_back(new Tile(numbers[i][j], i, j));
            }
            state.push_back(row);
        }
    }
    
    Node(const vector<vector<Tile*>>& state, Node* parent): state(state), parent(parent){}
    
    //Parameter
    vector<vector<Tile*>> state; // to store all the cells the current state according to their position
    Node* parent;
    vector<int> Changed_pos; //store the position of the tile that will be decided according to the current state
    int curr_changed_pos_value; //store the value that the tile at changed_pos will be
    
    //domain reduction for all the tiles in the state, return whether there detects confliction
    bool optimize(){
        if(parent==nullptr){
            //reduce domain of all tiles
            for(int i = 0; i < 5; i++){
                for(int j = 0; j < 5; j++){
                    if(!reduce_domain(state, i, j)){
                        return false; //there is confliction so that some tile may not has value
                    }
                }
            }
            
            //Organize numbers of unassigned neighbors for all the tiles;
            getUnassignedNeighbors(state);
            return true; //no confliction
        }
        //Forward checking
        else{
            for(int i = 0; i< 5; i++){
                if(i != parent->Changed_pos[0]){
                    if(!reduce_domain(state, i, parent->Changed_pos[1])){
                        return false; //there is confliction so that some tile may not has value
                    }
                }
                if(i != parent->Changed_pos[1]){
                    if(!reduce_domain(state, parent->Changed_pos[0],i)){
                        return false; //there is confliction so that some tile may not has value
                    }
                }
                
            
            }
            
            //Organize numbers of unassigned neighbors for all the tiles;
            getUnassignedNeighbors(state);
            return true; //no confliction
        }
        
        return true;
    }
    
    //to check whether the state is finished;
    bool finished(){
        //check whether every tile is not empty
        for(size_t i = 0; i <5; i++){
            for(size_t j = 0; j <5; j++){
                if(state[i][j]->isEmpty()) return false;
            }
        }
        return true;
    }
};

//select the most constrained tile from the current unassigned variables
vector<int> SelectUnassignedVariable(Node* curr);

//if the current state failed / if there is confliction
Node* curr_failed(Node* curr);

//copy and return a current state
vector<vector<Tile*>> copy_state(const vector<vector<Tile*>>& rhs);

//Backtracking Algorithm
Node* Backtracking_Algorithm(Node* curr);

int main() {
    
    //read files
    string name;
    
    cout << "Input the name of the text file: ";
    cin >> name; //get the name of the input file
    
    ifstream file_inp(name);
    if(!file_inp){
        cout << "cannot open file" << endl;
        exit(1);
    }
    
    //read numbers in cells
    vector<vector<int>> initial_state;
    
    for(size_t i = 0; i< 5; i++){
        vector<int> row;
        for(size_t j = 0; j < 5; j++){
            int curr;
            file_inp >> curr;
            row.push_back(curr);
            
        }
        initial_state.push_back(row);
        
    }
    //read inequalities between horizontally-adjacent cells
    for(size_t i = 0; i< 5; i++){
        vector<char> row;
        for(size_t j = 0; j < 4; j++){
            char curr;
            file_inp >> curr;
            row.push_back(curr);
            
        }
        horizontal.push_back(row);
    }
    
    //read inequalities between vertically-adjacent cells
    for(size_t i = 0; i < 4; i++){
        vector<char> row;
        for(size_t j = 0; j < 5; j++){
            char curr;
            file_inp >> curr;
            row.push_back(curr);
            
        }
        vertical.push_back(row);
    }
    
    Node* root = new Node(initial_state, nullptr);
    Node* result;
    
    //Backtracking Algorithm
    result = Backtracking_Algorithm(root);
    
    //display
    for(size_t i = 0; i< 5; i++){
        for(size_t j = 0; j< 5; j++){
            cout << result->state[i][j]->value <<' ';
        }
        cout << endl;
    }
}

//Backtracking algorithm: return a Node* that includes a finished state
Node* Backtracking_Algorithm(Node* curr){
    if(curr->finished()) return curr; //if the state is finished, return the result
    
    //if there is no conflicted domain under the current state
    if(curr->optimize()){
        //move on and decide the next tile
        
        vector<int> next = SelectUnassignedVariable(curr); //choose the position of the next tile to be decided
        curr->Changed_pos = next; //assign the position of next tile to the parameter of the Node*
        
        //decide the value of the next tile: the smallest element in its domain
        int value = curr->state[next[0]][next[1]]->domain[0];
        curr->curr_changed_pos_value = value; //assign the value to the parameter of the Node*
        
        //create a new Tile* to store the updated tile
        Tile* changed_tile = new Tile(value, next[0], next[1]);
        vector<vector<Tile*>> new_state=copy_state(curr->state);
        
        //update the current node to a Node* with the updated state
        curr = new Node(new_state, curr);
        curr->state[next[0]][next[1]] = changed_tile;//update the position with the new Tile
        
    }
    
    //if there is no conflicted domain under the current state
    else{
        //move back up to the parent Node and change the value of the changable Tile
        curr = curr_failed(curr); //get the Node*  returned to
        vector<int> next = curr->Changed_pos; //obtain the position of tile to be changed
        int value = curr->curr_changed_pos_value; //obtain the value the tile is gonna changed to
        
        //update the state with the updated tile
        Tile* changed_tile = new Tile(value, next[0], next[1]);
        curr = new Node(copy_state(curr->state), curr);
        curr->state[next[0]][next[1]] = changed_tile;
    }
    
    //recursive to solve the empty cells left
    return Backtracking_Algorithm(curr);
}

//get all the coordinates of all neighbors of State[x][y]
vector<vector<int>> getRelation(int x, int y){
    vector<vector<int>> result;
    
    // add all the other cell on the same column
    for(int i = 0; i <5; i++){
        if(i != x){
            vector<int> pair;
            pair.push_back(i);
            pair.push_back(y);
            result.push_back(pair);
        }
    }
    
    // add all the other cell on the same row
    for(int j = 0; j <5; j++){
        if(j != x){
            vector<int> pair;
            pair.push_back(x);
            pair.push_back(j);
            result.push_back(pair);
        }
    }
    return result;
}

//copy and return a current state: save all the Tile* in a new matrix
vector<vector<Tile*>> copy_state(const vector<vector<Tile*>>& rhs){
    vector<vector<Tile*>> new_state;
    for(size_t i = 0; i < 5; i++){
        vector<Tile*> row;
        for(size_t j = 0; j < 5; j++){
            row.push_back(rhs[i][j]);
        }
        new_state.push_back(row);
    }
    return new_state;
}

//Domain_reduction by forward checking
    //if there is confliction(no elements in domain) return false; else true
bool reduce_domain(vector<vector<Tile*>>& state, int x, int y){
    if(state[x][y]->value !=0) return true;
    else{
        vector<int> renewed_domain;
        renewed_domain = state[x][y]->domain;
        
        //according to its horizontal line
        for(int j = 0; j <5; j++){
            if(j != y){
                if(!state[x][j]->isEmpty()){
                    for(size_t n = 0; n < renewed_domain.size(); n++){
                        if(renewed_domain[n] == state[x][j]->value){
                            renewed_domain[n] = 0;
                        }
                    }
                }
            }
        }
        
        //according to its vertical line
        for(int i = 0; i <5; i++){
            if(i != x){
                if(!state[i][y]->isEmpty()){
                    for(size_t n = 0; n < renewed_domain.size(); n++){
                        if(renewed_domain[n] == state[i][y]->value){
                            renewed_domain[n] = 0;
                        }
                    }
                }
            }
        }
        
        //according to inequalities between horizontally-adjacent cells
        //check the relationship with its left tile
        if(y!=0){
            if(!state[x][y-1]->isEmpty() && horizontal[x][y-1] != '0'){
                for(size_t n = 0; n <renewed_domain.size(); n++){
                    if(horizontal[x][y-1] == '>'){
                        if(renewed_domain[n] >= state[x][y-1]->value){
                            renewed_domain[n] = 0;
                        }
                    }
                    else if(horizontal[x][y-1] == '<'){
                        if(renewed_domain[n] <= state[x][y-1]->value){
                            renewed_domain[n] = 0;
                        }
                    }
                }
            }
        }
        
        //check the relationship with its right tile
        if(y!=state[x].size()-1){
            if(!state[x][y+1]->isEmpty() && horizontal[x][y] != '0'){
                for(size_t n = 0; n < renewed_domain.size(); n++){
                    if(horizontal[x][y] == '>'){
                        if(renewed_domain[n] <= state[x][y+1]->value){
                            renewed_domain[n] = 0;
                        }
                    }
                    else if(horizontal[x][y] == '<'){
                        if(renewed_domain[n] >= state[x][y+1]->value){
                            renewed_domain[n] = 0;
                        }
                    }
                }
            }
        }
        
        //according to inequalities between vertically-adjacent cells
        //check the relationship with its up tile
        if(x!=0){
            if(!state[x-1][y]->isEmpty()&& vertical[x-1][y] != '0'){
                for(size_t n = 0; n < renewed_domain.size(); n++){
                    if(vertical[x-1][y] == 'v'){
                        if(renewed_domain[n] >= state[x-1][y]->value){
                            renewed_domain[n] = 0;
                        }
                    }
                    else if(vertical[x-1][y] == '^'){
                        if(renewed_domain[n] <= state[x-1][y]->value){
                            renewed_domain[n] = 0;
                        }
                    }
                }
            }
        }
        //check the relationship with its up tile
        if(x!=state.size()-1){
            if(!state[x+1][y]->isEmpty()&& vertical[x][y] != '0'){
                for(size_t n = 0; n < renewed_domain.size(); n++){
                    if(vertical[x][y] == 'v'){
                        if(renewed_domain[n] <= state[x+1][y]->value){
                            renewed_domain[n] = 0;
                        }
                    }
                    else if(vertical[x][y] == '^'){
                        if(renewed_domain[n] >= state[x+1][y]->value){
                            renewed_domain[n] = 0;
                        }
                    }
                }
            }
        }
        
        //calculate and assign the number of legal values
        int count = 0;
        vector<int> new_domain;
        for(size_t i = 0; i < renewed_domain.size(); i++ ){
            if(renewed_domain[i]!=0){
                new_domain.push_back(renewed_domain[i]);
                count++;
            }
        }
        if(count == 0) return false; // there causes a confliction on the current Tile
        else{
            Tile* new_Tile= new Tile(new_domain,x ,y);
            state[x][y] = new_Tile; //update the tile
            state[x][y]->legal_numbers = count;
        }
        return true; //no confliction on the current tile
    }
}

//calculate and assign Numbers of Unassigned Neighbors in a state
void getUnassignedNeighbors(const vector<vector<Tile*>>& state){
    
    for(int i =0; i <5; i++){
        for(int j =0; j <5; j++){
            int count = 0;
            for(vector<int> pair: state[i][j]->relations){
                if(state[pair[0]][pair[1]] == 0){//check if empty
                    count++;
                }
            }
            state[i][j]->unassigned_neighbors = count;
        }
    }
}

//select the most constrained tile from the current unassigned variables
vector<int> SelectUnassignedVariable(Node* curr){
    vector<vector<int>> possible_tiles; //store all tie possible tiles
    
    //most constrained: fewer legal values
    int min_legal_values = 6;
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            if(curr->state[i][j]->isEmpty()){
                //if more constrained variable found
                if(curr->state[i][j]->legal_numbers < min_legal_values){
                    min_legal_values = curr->state[i][j]->legal_numbers;
                    possible_tiles.clear();
                    vector<int> new_pair;
                    new_pair.push_back(i);
                    new_pair.push_back(j);
                    possible_tiles.push_back(new_pair);
                }
                //if tie: add the tile to the possible tile list
                else if(curr->state[i][j]->legal_numbers == min_legal_values){
                    vector<int> new_pair;
                    new_pair.push_back(i);
                    new_pair.push_back(j);
                    possible_tiles.push_back(new_pair);
                }
            }
        }
    }
    
    //if tie
    if(possible_tiles.size()>1){
        //most constrained: most unassigned neighbors / degree heuristic
        int max_unassigned_neighbors = -1;
        vector<int> result;
        for(vector<int> pair: possible_tiles){
            if(curr->state[pair[0]][pair[1]]->unassigned_neighbors > max_unassigned_neighbors){
                max_unassigned_neighbors = curr->state[pair[0]][pair[1]]->unassigned_neighbors;
                result = pair;
            }
        }
        return result;
    }
    //if not tie
    return possible_tiles[0];
}

//if the current state has conflicted tiles
Node* curr_failed(Node* curr){
    //return back to the parent
    Node* res = curr->parent;
    vector<int> pos = res->Changed_pos;
    
    //if there is value left in domain
    if(res->state[pos[0]][pos[1]]->domain.size()>1){
        //change the value of the Tile that needs to be decided according to the current state
        for(size_t i = 0; i < res->state[pos[0]][pos[1]]->domain.size()-1; i++){
            res->state[pos[0]][pos[1]]->domain[i] = res->state[pos[0]][pos[1]]->domain[i+1];
        }
        res->state[pos[0]][pos[1]]->domain.pop_back(); //drop the invalid value: the first value by default
        //assign the current last element in the domain as the new value
        res->curr_changed_pos_value = res->state[pos[0]][pos[1]]->domain[0]; //take values in increasing order->take smallest
        return res;
    }
    
    //no value available
    else{
        //the parent fails->keep moving backward
        return curr_failed(res);
    }
}

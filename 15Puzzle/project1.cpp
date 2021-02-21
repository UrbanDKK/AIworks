//Project 1
//Lyzz Ma (hm1936)
//April 6, 2020

#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

vector<vector<int>> goal_state;

//calculate the Manhattan distance of a tile
int Manhattan_distance(int target, const vector<vector<int>>& curr_state, const vector<vector<int>>& goal_state){
    int curr_x = 0;
    int curr_y = 0;
    int goal_x = 0;
    int goal_y = 0;
    
    //cout << target <<'a' <<endl;
    //find the position of the target tile in current state
    while(curr_state[curr_x][curr_y]!=target){
        curr_y++;
        if(curr_y == 4){
            curr_x++;
            curr_y = 0;
        }
       
    }
    
    //find the position of the target tile in goal state
    while(goal_state[goal_x][goal_y]!=target){
        goal_y++;
        if(goal_y == 4){
            goal_x++;
            goal_y = 0;
        }
    }
    
    return abs(goal_x - curr_x)+ abs(goal_y - curr_y); //horizon transform + vertical transform
}

//calculate the sum of Manhattan distance of a state
int heuristic(const vector<vector<int>>& curr_state, const vector<vector<int>>& goal_state){
    int sum = 0;
    for(int i = 1; i <= 15; i++){
        sum += Manhattan_distance(i, curr_state, goal_state);
    }
    
    return sum;
}

//find the position of the blank tile
vector<int> find_blank(const vector<vector<int>>& curr_state){
    int curr_x = 0;
    int curr_y = 0;

    //find the position of the target tile in current state
    while(curr_state[curr_x][curr_y]!=0){
        curr_y++;
        if(curr_y == 4){
            curr_x++;
            curr_y = 0;
        }
    }
    
    vector<int> blank_pos;
    blank_pos.push_back(curr_x);
    blank_pos.push_back(curr_y);
    return blank_pos;
}


struct Node {
    
    Node(){};
   
    Node(const vector<vector<int>>& state, Node* parent, char action): state(state), parent(parent), action(action){
        
        if(parent){//if it is not the root
            level = parent->level+1;
            parent->children.push_back(this);
            gn = parent->gn + 1;
        }
        else{//if it is the root
            gn = 0;
            level = 0;
            
        }
        
        blank = find_blank(state);
        
        hn = heuristic(state, goal_state);
        fn = hn+gn;
        
       
    };
    
    vector<vector<int>> state;
    Node* parent;
    vector<Node*> children;
    int level;
    vector<int> blank; //position of the blank tile
    int fn;
    int hn;
    int gn;
    char action;
    
};

vector<Node*> all_Nodes; //stored all of the generated nodes
vector<Node*> frontier; //stored the nodes that have not been expanded yet

//check whether two states are the same
bool check_same(const vector<vector<int>>& state1, const vector<vector<int>>& state2){
    for(size_t i = 0; i < 4; i++){
        for(size_t j = 0; j < 4; j++){
            if(state1[i][j] != state2[i][j]){
                return false;
            }
        }
    }
    return true;
}

//check whether the state is repeated
bool check_repeated(Node* curr){
    for(size_t i = 0 ; i< all_Nodes.size(); i++){
        if(check_same(curr->state, all_Nodes[i]->state)){
            //if current state has higher fn
            if(curr->fn >= all_Nodes[i]->fn){
                curr->parent->children.pop_back();
                return true;
            }
            
            //if current state has lower fn
            else{
                all_Nodes[i] = curr; //update the previous one with the current one
                for(size_t j = 0 ; j< frontier.size(); j++){
                    //update the previous one with the current one if it is in frontier
                    if(frontier[j] != nullptr && check_same(curr->state, frontier[j]->state)){
                        frontier[j] = curr;
                        return true;
                    }
                }
                return true;
            }
            
        }
    }
    
    
    return false;
}


//move the blank tile left
void left(Node* parent){
    
    vector<vector<int>> curr_state;
    for(size_t i = 0 ; i < 4; i++){
        vector<int> row;
        for(size_t j = 0; j < 4; j++){
            row.push_back(parent->state[i][j]);
        }
        curr_state.push_back(row);
    }
    
    //swap the value of the left tile with blank tile
    curr_state[parent->blank[0]][parent->blank[1]] = curr_state[parent->blank[0]][parent->blank[1]-1];
    curr_state[parent->blank[0]][parent->blank[1]-1] = 0;
    
    Node* new_Node = new Node(curr_state, parent, 'L');
    
    if(check_repeated(new_Node)){
        return;
    }
    
    //if not repeated
    all_Nodes.push_back(new_Node);
    frontier.push_back(new_Node);
    
}

//move the blank tile right
void right(Node* parent){
    
    vector<vector<int>> curr_state;
    for(size_t i = 0 ; i < 4; i++){
        vector<int> row;
        for(size_t j = 0; j < 4; j++){
            row.push_back(parent->state[i][j]);
        }
        curr_state.push_back(row);
    }
    
    //swap the value of the right tile with blank tile
    curr_state[parent->blank[0]][parent->blank[1]] = curr_state[parent->blank[0]][parent->blank[1]+1];
    curr_state[parent->blank[0]][parent->blank[1]+1] = 0;
    
    
    Node* new_Node = new Node(curr_state, parent, 'R');
    
    if(check_repeated(new_Node)){
        return;
    }
    
    //if not repeated
    all_Nodes.push_back(new_Node);
    frontier.push_back(new_Node);
    
}

//move the blank tile up
void up(Node* parent){
    
    vector<vector<int>> curr_state;
    for(size_t i = 0 ; i < 4; i++){
        vector<int> row;
        for(size_t j = 0; j < 4; j++){
            row.push_back(parent->state[i][j]);
        }
        curr_state.push_back(row);
    }
    
    //swap the value of the up tile with blank tile
    curr_state[parent->blank[0]][parent->blank[1]] = curr_state[parent->blank[0]-1][parent->blank[1]];
    curr_state[parent->blank[0]-1][parent->blank[1]] = 0;
    
    Node* new_Node = new Node(curr_state, parent, 'U');
    if(check_repeated(new_Node)){
        return;
    }
    
    //if not repeated
    all_Nodes.push_back(new_Node);
    frontier.push_back(new_Node);
}

//move the blank tile down
void down(Node* parent){
    
    vector<vector<int>> curr_state;
    for(size_t i = 0 ; i < 4; i++){
        vector<int> row;
        for(size_t j = 0; j < 4; j++){
            row.push_back(parent->state[i][j]);
        }
        curr_state.push_back(row);
    }
    
    //swap the value of the down tile with blank tile
    curr_state[parent->blank[0]][parent->blank[1]] = curr_state[parent->blank[0]+1][parent->blank[1]];
    curr_state[parent->blank[0]+1][parent->blank[1]] = 0;
    
    
    Node* new_Node = new Node(curr_state, parent, 'D');
    if(check_repeated(new_Node)){ //check whether the state is repeated
        return;
    }
    
    //if not repeated
    all_Nodes.push_back(new_Node);
    frontier.push_back(new_Node);
}

//create all possible children of a given
void create_children(Node* curr){
    
    if(curr->blank[1]!=0){
        left(curr);
    }
    if(curr->blank[1]!=3){
        right(curr);
    }
    if(curr->blank[0]!=0){
        up(curr);
    }
    if(curr->blank[0]!=3){
        down(curr);
    }
    
    
    
    //remove the expanded node from frontier
    for(int i = 0 ; i < frontier.size(); i++){
        if(frontier[i] == curr){
            frontier[i] = nullptr;
            return;
        }
    }
}

//find the node with lowest fn in frontier
Node* lowest_fn(){
    Node* lowest = nullptr;
    for(Node* node : frontier){
        if(node != nullptr){
            if(lowest == nullptr){
                lowest = node;
            }
            else if(node->fn < lowest->fn){
                lowest = node;
            }
        }
        
    }
    
    return lowest;
}

int main() {
    
    //read the file to obtain initial node and goal node
    string name;
    
    
    cout << "Input the name of the text file: ";
    cin >> name;
    
    
    vector<vector<int>> initial_state;
    
    
    ifstream file_inp(name);
    if(!file_inp){
        cout << "cannot open file" << endl;
        exit(1);
    }
    
    
    for(size_t i = 0; i< 4; i++){
        vector<int> row;
        for(size_t j = 0; j < 4; j++){
            int curr;
            file_inp >> curr;
            row.push_back(curr);
            
        }
        initial_state.push_back(row);
        
    }
    
    
    for(size_t i = 0; i< 4; i++){
        vector<int> row;
        for(size_t j = 0; j < 4; j++){
            int curr;
            file_inp >> curr;
            row.push_back(curr);
            
        }
        goal_state.push_back(row);
    }
    
    file_inp.close();
    
    Node root(initial_state, nullptr, ' ');
    
    all_Nodes.push_back(&root);
    frontier.push_back(&root);
    
    Node* next = &root;
    
    bool found = false;
    while(!found){
        next = lowest_fn(); //expand the node with lowest f(n)
        if(check_same(goal_state, next->state)){ //if the current state is the goal state
            found =  true;
        }
        else{ //if not, expand
            create_children(next);
        }
        
    }
    
    vector<Node*> solution;
    solution.push_back(next);
    while(solution[solution.size()-1] != &root){
        solution.push_back(solution[solution.size()-1]->parent);
    }
    //display
    
    //initial state
    for(size_t i = 0; i < 4; i++){
        for(size_t j = 0; j < 4; j++){
            cout << initial_state[i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
    
    //goal state
    for(size_t i = 0; i < 4; i++){
        for(size_t j = 0; j < 4; j++){
            cout << goal_state[i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
    
    //depth evel of the shallowest goal node
    cout << next->level <<endl;
    
    //numbers of nodes generated
    cout << all_Nodes.size() <<endl;
    
    //display actions
    for(int i = solution.size()-2; i>=0; i--){
        cout << solution[i]->action << ' ';
    }
    
    cout << endl;
    
    //display f(n)
    for(int i = solution.size()-1; i>=0; i--){
        cout << solution[i]->fn << ' ';
    }
    cout << endl;
    
}

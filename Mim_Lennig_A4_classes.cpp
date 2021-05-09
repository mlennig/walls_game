//
//  Mim_Lennig_A4_classes.cpp
//  MimLennigA4
//
//  Created by Miriam Lennig on 2016-11-19.
//  Copyright © 2016 Miriam Lennig. All rights reserved.
//

#include "Mim_Lennig_A4_classes.hpp"

using namespace std;
extern const int N;

// XY class represents a point (a, b) ordered pair on a lattice
XY::XY(){
    x = 0;
    y = 0;
}

// Constructs an (a, b) point on a lattice
XY::XY(int a, int b){
    x = a;
    y = b;
}

// Copy constructor
XY::XY(XY& point){
    x = point.x;
    y = point.y;
}

// Generates a new point by adding a to itself
XY& XY::add(Compass a){
    int dx;
    int dy;
    
    if (a == north){
        dx = -1;
        dy = 0;
    }
    else if (a == east){
        dx = 0;
        dy = 1;
    }
    else if (a == south){
        dx = 1;
        dy = 0;
    }

    else if (a == west){
        dx = 0;
        dy = -1;
    }
    
    XY* newPos = new XY(x + dx, y + dy);
 
    return *newPos;
}

int XY::getx(){
    return x;
}

int XY::gety(){
    return y;
}

void XY::setx(int a){
    x = a;
}

void XY::sety(int a){
    y = a;
}

// Class State represents a game state; these are the nodes of the game tree
// Construct the start state of the game
State::State(){
    score = 999;
    pos[0] = XY(N / 2 , 0);
    pos[1] = XY(N / 2, N - 1);
    
    heading[0] = west;
    heading[1] = east;
    
    player2move = 0;
    depthInTree = 0;
    
    // Initialize the board to all empty
    for (int k = 0; k < N; k++){
        for (int j = 0; j < N; j++)
            board[k][j] = 99;           // 99 represents an empty square
    }
    
    for (int k = 0; k < 2; k++)
        board[pos[k].getx()][pos[k].gety()] = k;
    
    makeChildren();
}

State::State(int player){
    score = 999;
    pos[0] = XY(N / 2 , 0);
    pos[1] = XY(N / 2, N - 1);
    
    heading[0] = west;
    heading[1] = east;
    
    player2move = player;
    depthInTree = 0;
    
    // Initialize the board to all empty
    for (int k = 0; k < N; k++){
        for (int j = 0; j < N; j++)
            board[k][j] = 99;           // 99 represents an empty square
    }
    
    for (int k = 0; k < 2; k++)
        board[pos[k].getx()][pos[k].gety()] = k;
    
    makeChildren();
}

// Constructs next game state when you make move m from state parent
State::State(State* p, Move m){
    score = 999;
    parent = p;
    move = m;
    player2move = (parent->player2move + 1) % 2;    // Switch player
    depthInTree = parent->depthInTree += 1;     // Increment depth
    
    // Copy the parent's board state
    for (int k = 0; k < N; k++)
        for (int j = 0; j < N; j++)
            board[k][j] = parent->board[k][j];
    
    // Copy the parent's position & direction
    for (int k = 0; k < 2; k++){
        pos[k] = parent->pos[k];
        heading[k] = parent->heading[k];
        nNoBrickMoves[k] = parent->nNoBrickMoves[k];
        isStalled[k] = parent->isStalled[k];
    }
    
    if(!isStalled[player2move]){
        // This is the beginning of the move
        if (m == Left){
            // Turn left
            nNoBrickMoves[player2move] += 1;
            if (heading[player2move] == north){
                heading[player2move] = west;
            }
            else if (heading[player2move] == east){
                heading[player2move] = north;
            }
            else if (heading[player2move] == south){
                heading[player2move] = east;
            }
            else if (heading[player2move] == west){
                heading[player2move] = south;
            }
        }
        else if (m == Right){
            // Turn right
            nNoBrickMoves[player2move] += 1;
            if (heading[player2move] == north){
                heading[player2move] = east;
            }
            else if (heading[player2move] == east){
                heading[player2move] = south;
            }
            else if (heading[player2move] == south){
                heading[player2move] = west;
            }
            else if (heading[player2move] == west){
                heading[player2move] = north;
            }
        }
        else if (m == Forward){
            // Try forward
            if (canMoveForward()){
                if(board[pos[player2move].getx()][pos[player2move].gety()] == player2move)
                    // Already laid a brick here
                    nNoBrickMoves[player2move] += 1;
                else {
                    board[pos[player2move].getx()][pos[player2move].gety()] = player2move;      // Lay a brick
                    nNoBrickMoves[player2move] = 0;     // Able to lay a brick on this move
                }
                pos[player2move] = pos[player2move].add(heading[player2move]);  // Forward movement
            }
            else
                nNoBrickMoves[player2move] += 1;        // Unable to move forward AND did not lay a brick
        }
        else
            cout << "*******ERROR ILLEGAL MOVE**********\n";
        // This is the end of the move
        
        // Is this player stalled?
        isStalled[player2move] = nNoBrickMoves[player2move] >= 9;
    }

    cout << "player2move: " << player2move << " depthInTree: " << depthInTree << " heading: " << heading[player2move] << " move: " << m << " nNoBrickMoves = " << nNoBrickMoves[player2move] << " isEndState(): " << isEndState() << " Score = " << score << endl;
    print();
    cout << endl;
    
    
    // Make children if not at an end state
    // Otherwise, calculate terminal score
    makeChildren();
}

State::~State(){
    delete leftChild;
    delete rightChild;
    delete forwardChild;
}

// Can we move forward?
bool State::canMoveForward(){
    XY tryforward = pos[player2move].add(heading[player2move]);
    int opponent = (player2move + 1) % 2;
    
    int x = tryforward.getx();
    int y = tryforward.gety();

    if ((x < 0) || (y < 0) || (x >= N) || (y >= N))
        return false;
    if (board[x][y] == opponent)
        return false;
    if ((pos[opponent].getx() == x) && (pos[opponent].gety() == y))
        return false;
    return true;
}

// Are we in an end state?
bool State::isEndState(){
    if ((isStalled[0] && isStalled[1]) || boardIsFull())
        return true;
    if ((nBricksLaid(0) >= 5) || (nBricksLaid(1) >= 5))
        return true;
    
    return false;
}

// Is the board full?
bool State::boardIsFull(){
    for (int k = 0; k < N; k++)
        for (int j = 0; j < N; j++)
           if ((board[k][j] == 99) && (((pos[0].getx() != k) || (pos[0].gety() != j)) && ((pos[1].getx() != k) || (pos[1].gety() != j))))
               return false;
    
    return true;
}

// Computes score in an end state
int State::computeTerminalScore(){
    int count0 = 0;
    int count1 = 0;
    for (int k = 0; k < N; k++)
        for (int j = 0; j < N; j++)
            if(board[k][j] == 0)
                count0 += 1;
            else if(board[k][j] == 1)
                count1 += 1;
    if (count0 > count1)
        // Player 0 won
        return 2;
    else if (count1 > count0)
        // Player 1 won
        return -2;
    else
        return 0;
}


// Compute intermediate state score
int State::minimax(){
    if (score == 999){
        // Score has not already been computed
        if (player2move == 0)
            // Starting player's turn
            score = max((max(leftChild->minimax(), rightChild->minimax())), forwardChild->minimax());
        else
            // Opponent's turn
            score = min((min(leftChild->minimax(), rightChild->minimax())), forwardChild->minimax());
    }
    return score;
}

void State::makeChildren(){
    // Are we in an end state?
    if (isEndState()){
        score = computeTerminalScore();
    }
    else{
        // We are not at an end state
        // Generate all possible moves as this state's children in the game tree
        if ((parent->move == Left) || (parent->move == Right)){
            forwardChild = new State (this, Forward);
            leftChild = new State(this, Left);
            rightChild = new State(this, Right);
        }
        else {
            leftChild = new State(this, Left);
            rightChild = new State(this, Right);
            forwardChild = new State(this, Forward);
        }
        
        // Compute the score of this state
       minimax();
    }
}

void State::print(){
    for(int row = 0; row < N; row++){
        for(int col = 0; col < N; col++){
            char c;
            
            if(board[row][col] == 0)
                c = 'o';
            else if (board[row][col] == 1)
                c = 'x';
            else
                c = '.';
            
            if((pos[0].getx() == row) && (pos[0].gety() == col))
                c = '0';
            if((pos[1].getx() == row) && (pos[1].gety() == col))
                c = '1';
            
            cout << " " << c;
        }
        cout << endl;
    }
    
}

int State::nBricksLaid(int player){
    int count = 0;
    for (int k = 0; k < N; k++)
        for (int j = 0; j < N; j++)
            if(board[k][j] == player)
                count += 1;
    return count;
}













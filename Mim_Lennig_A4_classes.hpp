//
//  Mim_Lennig_A4_classes.hpp
//  MimLennigA4
//
//  Created by Miriam Lennig on 2016-11-19.
//  Copyright © 2016 Miriam Lennig. All rights reserved.
//

#ifndef Mim_Lennig_A4_classes_hpp
#define Mim_Lennig_A4_classes_hpp

#include <iostream>
#include <cmath>

static const int N = 3;
enum Move {Left, Right, Forward};
enum Compass {north, south, east, west};

// XY class represents a point (a, b) ordered pair on a lattice
class XY {
    int x;
    int y;
public:
    int getx();
    int gety();
    void setx(int);
    void sety(int);
    XY& add(Compass);
    XY(int a, int b);
    XY();
    XY(XY&);
};

// Class State represents a game state; these are the nodes of the game tree
class State{
    XY pos[2];
    Compass heading[2];
    
    State* leftChild;
    State* rightChild;
    State* forwardChild;
    
    int board[N][N];
    int player2move;
    int nNoBrickMoves[2] = {0, 0};
    int score = 999;
    bool isStalled[2] = {false, false};
    int depthInTree = 0;
    Compass computeHeading();
    int minimax();
    void makeChildren();
    Move move;
    State* parent;
    
public:
    State();
    State(State*, Move);
    State(int);
    ~State();
    bool canMoveForward();
    bool isEndState();
    bool boardIsFull();
    int computeTerminalScore();
    void print();
    int nBricksLaid(int);
};







#endif /* Mim_Lennig_A4_classes_hpp */

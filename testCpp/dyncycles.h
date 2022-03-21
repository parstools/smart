//
// Created by andrzej on 3/17/22.
//
#pragma once
#include <vector>

struct Object {
    int use_count;
    int weak_count;
    int out_count;
    int link_number;
    Object() {
        use_count = 1;
        out_count = 0;
        link_number = 0;
    }
    virtual ~Object(){}
};

struct ShadowOne {
    int Index;
    int numCycles = 0;
    std::vector<std::pair<int,int>> ranges;
    void AddLastRange(int nodeNumber);
    bool IsInRange(int nodeNumber, int defaultLast);
};

void lshared_release_single(Object** pp);
void lshared_assign_single(Object** pp, Object* p);

struct Shadow {
    int link_counter = 1;
    void NewGraph(int startNumber);
    std::vector<ShadowOne*> graphs;//może lepsza linkedList
    std::vector<Object*> toRelease;
    bool IsCycle(int from, int to);
    void JoinGraphs(int from, int to);
    int Localize(int nodeNumber);
    ~Shadow(){
        for (Object* p: toRelease)
            lshared_release_single((Object**)&p);
        for (ShadowOne* p: graphs) delete p;
    }
};

void link_assign(Object* src, Shadow *shadow, Object** pp, Object* p);
void var_assign(Shadow *shadow, Object** pp, Object* p);
void try_release(Shadow *shadow, Object* p);
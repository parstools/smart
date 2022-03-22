#include <iostream>
#include "dyncycles.h"

using namespace std;

struct Elem: public Object {
    Elem* next;
    Elem* next2;
    string name;
    Elem(string name):Object() {
        this->name = name;
    }
    ~Elem() override {
        cout << "delete " << name << endl;
        lshared_release_single((Object**)&next);
        lshared_release_single((Object**)&next2);
    }
};

void CycleInMiddle() {
    Shadow shadow;
    Elem* A = new Elem("A");
    Elem* B = new Elem("B");
    Elem* C = new Elem("C");
    Elem* D = new Elem("D");
    Elem* E = new Elem("E");
    //A->next = B;
    link_assign(A,&shadow,(Object**)&A->next, B);
    //B->next = C;
    link_assign(B,&shadow,(Object**)&B->next, C);
    //C->next = D;
    link_assign(C,&shadow,(Object**)&C->next, D);
    //C->next2 = E;
    link_assign(C,&shadow,(Object**)&C->next2, E);
    //D->next = B; //cycle
    link_assign(D,&shadow,(Object**)&D->next, B);

    try_release(&shadow, E);
    try_release(&shadow, D);
    try_release(&shadow, C);
    try_release(&shadow, B);
    try_release(&shadow, A);
}


void JoinTwo() {
    Elem* A = new Elem("A");
    Elem* B = new Elem("B");
    Elem* C = new Elem("C");
    Elem* D = new Elem("D");
    Elem* E = new Elem("E");
    Elem* F = new Elem("F");

    Shadow shadow;
    link_assign(A,&shadow,(Object**)&A->next, B);
    link_assign(B,&shadow,(Object**)&B->next, F);

    link_assign(C,&shadow,(Object**)&C->next, D);
    link_assign(C,&shadow,(Object**)&C->next2, E);

    link_assign(D,&shadow,(Object**)&D->next, B);

    try_release(&shadow, F);
    try_release(&shadow, E);
    try_release(&shadow, D);
    try_release(&shadow, C);
    try_release(&shadow, B);
    try_release(&shadow, A);
}

void JoinTwo2() {
    Elem* A = new Elem("A");
    Elem* B = new Elem("B");
    Elem* C = new Elem("C");
    Elem* D = new Elem("D");
    Elem* E = new Elem("E");
    Elem* F = new Elem("F");

    Shadow shadow;

    link_assign(A,&shadow,(Object**)&A->next, B);
    link_assign(B,&shadow,(Object**)&B->next, F);

    link_assign(C,&shadow,(Object**)&C->next, D);
    link_assign(C,&shadow,(Object**)&C->next2, E);

    link_assign(B,&shadow,(Object**)&B->next2, D);

    try_release(&shadow, F);
    try_release(&shadow, E);
    try_release(&shadow, D);
    try_release(&shadow, C);
    try_release(&shadow, B);
    try_release(&shadow, A);
}

void JoinTwo3() {
    Elem* A = new Elem("A");
    Elem* B = new Elem("B");
    Elem* C = new Elem("C");
    Elem* D = new Elem("D");
    Elem* E = new Elem("E");
    Elem* F = new Elem("F");

    Shadow shadow;

    link_assign(A,&shadow,(Object**)&A->next, B);
    link_assign(B,&shadow,(Object**)&B->next, F);

    link_assign(C,&shadow,(Object**)&C->next, D);
    link_assign(C,&shadow,(Object**)&C->next2, E);

    link_assign(B,&shadow,(Object**)&B->next2, C);

    try_release(&shadow, F);
    try_release(&shadow, E);
    try_release(&shadow, D);
    try_release(&shadow, C);
    try_release(&shadow, B);
    try_release(&shadow, A);
}


//first building first graph, next second graph
//we divide first to half, first half of first is joined to second graph
void SplitAndJoin() {
    Elem* A = new Elem("A");
    Elem* B = new Elem("B");
    Elem* C = new Elem("C");
    Elem* D = new Elem("D");
    Elem* E = new Elem("E");
    Elem* F = new Elem("F");

    Shadow shadow;

    link_assign(A,&shadow,(Object**)&A->next, B);
    link_assign(B,&shadow,(Object**)&B->next, C);
    link_assign(C,&shadow,(Object**)&C->next, D);

    link_assign(E,&shadow,(Object**)&E->next, F);

    //split nand join
    link_assign(B,&shadow,(Object**)&B->next, nullptr);//z gtym czy bez gtego nie powinno być różnicy
    link_assign(B,&shadow,(Object**)&B->next, E);

    try_release(&shadow, F);
    try_release(&shadow, E);
    try_release(&shadow, D);
    try_release(&shadow, C);
    try_release(&shadow, B);
    try_release(&shadow, A);
}

//tutaj trudne, nie ma nawet pojedynczego roota (ale lista rootow)
void GroupToOne() {
    Elem* A = new Elem("A");
    Elem* B = new Elem("B");
    Elem* C = new Elem("C");
    Elem* D = new Elem("D");
    Elem* E = new Elem("E");
    Elem* F = new Elem("F");
    Shadow shadow;

    link_assign(A,&shadow,(Object**)&A->next, D);
    link_assign(B,&shadow,(Object**)&B->next, D);
    link_assign(C,&shadow,(Object**)&C->next, D);

    //cycle
    link_assign(D,&shadow,(Object**)&D->next, E);
    link_assign(E,&shadow,(Object**)&E->next, F);
    link_assign(F,&shadow,(Object**)&F->next, D);

    try_release(&shadow, F);
    try_release(&shadow, E);
    try_release(&shadow, D);
    try_release(&shadow, C);
    try_release(&shadow, B);
    try_release(&shadow, A);
}

void ApartmentLike() {
    Shadow shadow;
    Elem* A = new Elem("Apartment");
    Elem* B = new Elem("Person");
    link_assign(A,&shadow,(Object**)&A->next, B);
    link_assign(B,&shadow,(Object**)&B->next, A);
    //here A can't be at beginning releasedd when B is not released
    //bacause A is root
    cout << "przed user null" <<endl;
    var_assign(&shadow, (Object**)&A, nullptr);
    cout << "po user null" <<endl;
    try_release(&shadow,  B);
    cout << "przed nie, po zwalnianie" <<endl;
    try_release(&shadow,  A);
    //try_release(&shadow, tmp);
}

struct MstEdge {
    int from;
    int to;
    bool shared;
};

const MstEdge mst_edges[] = {{0, 1, true}, {0, 2, true}, {2, 4, true},
                             {4, 0, false}, {4, 1, false}, {1, 3, true}, {3, 0, false},
                             {4, 5, true}, {4, 6, true}, {6, 7, true}, {6, 2, false},
                             {5, 6, false}, {6, 8, true}, {8, 9, true}, {8, 7, false}, {9, 5, false}};


struct Vertex: public Object {
    int id;
    Vertex(int id) {
        this->id = id;
    }
    Vertex* p0 = nullptr;
    Vertex* p1 = nullptr;
    Vertex* p2 = nullptr;
    Vertex* p3 = nullptr;
    ~Vertex() override {
        //cout << "delete " << id << endl;
        lshared_release_single((Object**)&p0);
        lshared_release_single((Object**)&p1);
        lshared_release_single((Object**)&p2);
        lshared_release_single((Object**)&p3);
    }
};

static void buildMst() {
    Vertex* v0 = new Vertex(0);
    Vertex* v1 = new Vertex(1);
    Vertex* v2 = new Vertex(2);
    Vertex* v3 = new Vertex(3);
    Vertex* v4 = new Vertex(4);
    Vertex* v5 = new Vertex(5);
    Vertex* v6 = new Vertex(6);
    Vertex* v7 = new Vertex(7);
    Vertex* v8 = new Vertex(8);
    Vertex* v9 = new Vertex(9);
    Shadow shadow;
    //link_assign(E,&shadow,(Object**)&E->next, F);
    //cout << "będzie shared dla " << v1->ref_count << " czyli "<<0<<"->" <<1 << endl;
    link_assign(v0 , &shadow, (Object**)&v0->p0, v1);
    //cout << "będzie shared dla " << v2->ref_count << " czyli "<<0<<"->" <<2 << endl;
    link_assign(v0 , &shadow, (Object**)&v0->p1, v2);
    //cout << "będzie shared dla " << v4->ref_count << " czyli "<<2<<"->" <<4 << endl;
    link_assign(v2 , &shadow, (Object**)&v2->p0, v4);
    //cout << "będzie weak dla " << v0->ref_count << " czyli "<<4<<"->" <<0 << endl;
    link_assign(v4 , &shadow, (Object**)&v4->p0, v0);
    //cout << "będzie weak dla " << v1->ref_count << " czyli "<<4<<"->" <<1 << endl;
    link_assign(v4 , &shadow, (Object**)&v4->p1, v1);
    //cout << "będzie shared dla " << v3->ref_count << " czyli "<<1<<"->" <<3 << endl;
    link_assign(v1 , &shadow, (Object**)&v1->p0, v3);
    //cout << "będzie weak dla " << v0->ref_count << " czyli "<<3<<"->" <<0 << endl;
    link_assign(v3 , &shadow, (Object**)&v3->p0, v0);
    //cout << "będzie shared dla " << v5->ref_count << " czyli "<<4<<"->" <<5 << endl;
    link_assign(v4 , &shadow, (Object**)&v4->p2, v5);
    //cout << "będzie shared dla " << v6->ref_count << " czyli "<<4<<"->" <<6 << endl;
    link_assign(v4 , &shadow, (Object**)&v4->p3, v6);
    //cout << "będzie shared dla " << v7->ref_count << " czyli "<<6<<"->" <<7 << endl;
    link_assign(v6 , &shadow, (Object**)&v6->p0, v7);
    //cout << "będzie weak dla " << v2->ref_count << " czyli "<<6<<"->" <<2 << endl;
    link_assign(v6 , &shadow, (Object**)&v6->p1, v2);
    //cout << "będzie weak dla " << v6->ref_count << " czyli "<<5<<"->" <<6 << endl;
    link_assign(v5 , &shadow, (Object**)&v5->p0, v6);
    //cout << "będzie shared dla " << v8->ref_count << " czyli "<<6<<"->" <<8 << endl;
    link_assign(v6 , &shadow, (Object**)&v6->p2, v8);
    //cout << "będzie shared dla " << v9->ref_count << " czyli "<<8<<"->" <<9 << endl;
    link_assign(v8 , &shadow, (Object**)&v8->p0, v9);
    //cout << "będzie weak dla " << v7->ref_count << " czyli "<<8<<"->" <<7 << endl;
    link_assign(v8 , &shadow, (Object**)&v8->p1, v7);
    //cout << "będzie weak dla " << v5->ref_count << " czyli "<<9<<"->" <<5 << endl;
    link_assign(v9 , &shadow, (Object**)&v9->p0, v5);
    try_release(&shadow, v9);
    try_release(&shadow, v8);
    try_release(&shadow, v7);
    try_release(&shadow, v6);
    try_release(&shadow, v5);
    try_release(&shadow, v4);
    try_release(&shadow, v3);
    try_release(&shadow, v2);
    try_release(&shadow, v1);
   // cout << "before this should not delete, after delete all 10" << endl;
    try_release(&shadow, v0);
}

void DoubleOneDirection() {
    for (int i=0; i<2; i++) {
        Shadow shadow;
        Elem *A = new Elem("A");
        Elem *B = new Elem("B");
        link_assign(A, &shadow, (Object **) &A->next, B);
        link_assign(A, &shadow, (Object **) &A->next2, B);
        if (i==0) {
            try_release(&shadow, B);
            try_release(&shadow, A);
        }
        else {
            try_release(&shadow, A);
            try_release(&shadow, B);
        }
    }
}


int main() {
    DoubleOneDirection();
    return 1;
    ApartmentLike();
    cout << "=========================CycleInMiddle\n";
    CycleInMiddle();
    cout << "=========================JoinTwo\n";
    JoinTwo();
    cout << "=========================JoinTwo2\n";
    JoinTwo2();
    cout << "=========================JoinTwo3\n";
    JoinTwo3();
    cout << "=========================SplitAndJoin\n";
    SplitAndJoin();
    cout << "=========================GroupToOne\n";
    GroupToOne();
    return 0;
}

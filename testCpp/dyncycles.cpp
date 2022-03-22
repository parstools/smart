//
// Created by andrzej on 3/17/22.
//
#include "dyncycles.h"
#include <cassert>
#include <iostream>

void lshared_init_elem(Object** pp, Object* p) {
    p->ref_count = 1;
    *pp = p;
}

void lshared_add_ref_single(Object* p) {
    ++p->ref_count;
}

void lshared_release_single(Object** pp) {
    Object *p = *pp;
    if (!p) return;
    if (--p->ref_count == 0)
    {
        *pp = nullptr;
        delete p;
    }
}

void lshared_release_user(Shadow *shadow, Object** pp) {
    Object *p = *pp;
    if (!p) return;
    if (p->weak_count>0) {
        p->weak_count--;
        shadow->toRelease.push_back(p);
    }
    else
    if (--p->ref_count == 0)
    {
        *pp = nullptr;
        delete p;
    }
}

void lshared_assign_single(Object** pp, Object* p) {
    assert(pp);
    Object*__tmp = p;
    if (__tmp != *pp) {
        if (__tmp != nullptr)
            lshared_add_ref_single(__tmp);
        lshared_release_single(pp);
        *pp = __tmp;
    }
}

void link_assign(Object* src, Shadow *shadow, Object** pp, Object* p) {
    assert(pp);
    Object*__tmp = p;
    if (__tmp != *pp)//something changes
    {
        //if (src!=p) //not points to self
        {
            if (src->ref_count < 2 && src->link_number == 0) {
                if (__tmp->ref_count < 2) {
                    //std::cout << "begin new graph from " << shadow->link_counter << std::endl;
                    shadow->NewGraph(shadow->link_counter);
                }
                else
                    std::cout << "adding new root " << shadow->link_counter << std::endl;
            }
            if (src->link_number==0) {
                src->link_number = shadow->link_counter;
                shadow->link_counter++;
            } //else already links by other field
        }
        if (__tmp != nullptr) {
            bool isCycle = false;
            if (__tmp->link_number>0) {
                if (shadow->IsCycle(src->link_number, __tmp->link_number)) {
                    if (__tmp->link_number<src->link_number) {
                        //std::cout << "cykl " << src->link_number
                          //        << "->" << __tmp->link_number << std::endl;
                        isCycle = true;
                    }
                    else if (__tmp->link_number==src->link_number) {
                        std::cout << "self cycle " << src->link_number
                                  << "->" << __tmp->link_number << std::endl;
                        isCycle = true;
                    }
                    else
                        std::cout << "nie ma cyklu , do przodu " << src->link_number
                                  << "->" << __tmp->link_number << std::endl;
                }else {
                    std::cout << "laczenie grafów " << src->link_number
                              << "->" << __tmp->link_number << std::endl;
                    shadow->JoinGraphs(src->link_number, __tmp->link_number);
                }
            }
            if (!isCycle)
                lshared_add_ref_single(__tmp);
            else
                ++p->weak_count;
            src->out_count++;
        }
        if (*pp)
            src->out_count--;
        lshared_release_single(pp);
        *pp = __tmp;
    }
}

void var_assign(Shadow *shadow, Object** pp, Object* p) {
    assert(pp);
    Object*__tmp = p;
    if (__tmp != *pp)//something changes
    {
        if (__tmp != nullptr) {
               lshared_add_ref_single(__tmp);
        }
        lshared_release_user(shadow, pp);
        *pp = __tmp;
    }
}

void try_release(Shadow *shadow, Object* p) {
    if(!p)
        return;
    if (p->weak_count>0)
        shadow->toRelease.push_back(p);
    else
        lshared_release_single((Object **) &p);
}

void lweak_assign(Object** pp, Object* p) {
    assert(pp);
    *pp = p;
}

void Shadow::NewGraph(int startNumber) {
    ShadowOne *shadowOne = new ShadowOne;
    if (!graphs.empty())
        graphs.back()->AddLastRange(startNumber-1);
    shadowOne->ranges.emplace_back(startNumber,0);
    shadowOne->Index = graphs.size();
    graphs.push_back(shadowOne);
}

bool Shadow::IsCycle(int from, int to) {
    return Localize(from)==Localize(to);
}

int Shadow::Localize(int nodeNumber) {
    for (ShadowOne* one: graphs) {
        if (one->IsInRange(nodeNumber, link_counter))
            return one->Index;
    }
    assert(false);
    return 0;
}

void Shadow::JoinGraphs(int from, int to) {

}

bool ShadowOne::IsInRange(int nodeNumber, int defaultLast) {
    for (auto p:ranges) {
        if (!p.second)
            p.second = defaultLast;
        if (nodeNumber>=p.first && nodeNumber<=p.second)
            return true;
    }
    return false;
}

void ShadowOne::AddLastRange(int nodeNumber) {
    assert(!ranges.empty());
    assert(ranges.back().second==0);
    ranges.back().second = nodeNumber;
}

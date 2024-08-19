#ifndef ALL_VECS_H
#define ALL_VECS_H

#include "my_vec.h"
#include <string>

struct vecNode
{
    my_vec* vec;
    std::string name;
    vecNode* next;
}* head;

class vecList
{
    vecNode* _head;
    my_vec* _vec;
public:
    vecList() : _head(NULL) {}
    ~vecList();
    void addNode(const my_vec* vec, std::string name);
    void deleteNodeFromBeginning();
    void printVecIfInList(std::string name);
};

std::ostream& operator<<(std::ostream& os, my_vec* vec);

#endif
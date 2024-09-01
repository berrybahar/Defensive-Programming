#include "all_vecs.h"

vecList::~vecList()
{
    while(_head != NULL)
        deleteNodeFromBeginning();
}

void vecList::addNode(const my_vec* vec, std::string name)
{
    vecNode* newNode = new vecNode();
    newNode->name = name;
    newNode->vec = new my_vec(*vec);
    newNode->next = _head;
    _head = newNode;
}

void vecList::deleteNodeFromBeginning()
{
    if(!_head)
    {
        std::cout<< "List is empty." << std::endl;
        return;
    }

    vecNode* temp = _head;
    _head = _head->next;
    delete temp;
}

void vecList::printVecIfInList(std::string name)
{
    vecNode* temp = _head;
    while(temp != NULL)
    {
        if(temp->name == name)
        {
            my_vec* vec = temp->vec;
            std::cout<<vec<<std::endl;
            return;
        }
        temp = temp->next;
    }
    std::cout<<name<<" not in the list!"<<std::endl;
}

std::ostream& operator<<(std::ostream& os, my_vec* vec)
{
    os<<'('<<(*vec).getX()<<" ,"<<(*vec).getY()<<" ,"<<(*vec).getZ()<<')';
    return os;
}

int main()
{
    std::string names[10] = { "David", "Dana", "Moshe", "Vered", "Mohammed", "Yasmin", "Ahmed", "Lucy", "Naftali", "Ayelet" };
    vecList list;
    for(int i = 0; i < sizeof(names)/sizeof(names[0]); i++)
        list.addNode(new my_vec(i + 1,i + 1,i + 1), names[i]);
    
    list.printVecIfInList("Vered");
    
    return 0;
}

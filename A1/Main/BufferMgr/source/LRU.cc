#ifndef A1_LRU_C
#define A1_LRU_C
#include "LRU.h"
/*this is LRU implementaition file, used the double linked node for the page update */
class Node;

LRU::LRU(int fullSize) {
    this->curSizeNum = 0;
    this->fullSizeCapcity = fullSize;
}

void LRU::insert(Node *node) {
    curSizeNum++;
    // before u insert the new page node, you need to judge at first to avoid the overflow
    if (curSizeNum<=fullSizeCapcity){
        if(curSizeNum == 1) {
            head = node;
            tail = node;
        } else {
            // this in insert at head
            head->setPre(node);
            node->setNext(head);
            head = node;
        }
    }
    else{
        std::cout << "This is already full, do not add any node\n";
        std::cout << "You need to use the LRU algorithm\n";
        return;
    }
}

void LRU::update(Node *node) {
    Node* tmpHead = new Node(nullptr);
    Node* tmpTail = new Node(nullptr);
    head->setPre(tmpHead);
    tail->setNext(tmpTail);
    tmpHead->setNext(head);
    tmpTail->setPre(tail);

    Node* preNode = node->getPre();
    Node* nextNode = node->getNext();

    nextNode->setPre(preNode);
    preNode->setNext(nextNode);

    node->setPre(tmpHead);
    node->setNext(tmpHead->getNext());

    tmpHead->setNext(node);
    tmpHead->getNext()->setPre(node);

    head = tmpHead->getNext(); //node
    tail = tmpTail->getPre();
    delete tmpHead;
    delete tmpTail;
    tail->setNext(nullptr);
    head->setPre(nullptr);
}

Node* LRU::evict() {
    Node* tempHead = new Node(nullptr);
    Node* tempTail = new Node(nullptr);
    head->setPre(tempHead);
    tail->setNext(tempTail);
    tempHead->setNext(head);
    tempTail->setPre(tail);

    Node* node = head;
    while(node->getPage()->isIsPin()) {
        node = node->getNext();
    }

    Node* preNode = node->getPre();
    Node* nextNode = node->getNext();

    nextNode->setPre(preNode);
    preNode->setNext(nextNode);

    node->setNext(nullptr);
    node->setPre(nullptr);

    if (tempHead->getNext() == tempTail){
        head = nullptr;
        tail = nullptr;
    }else{
        head = tempHead->getNext();
        tail = tempTail->getPre();
        tail->setNext(nullptr);
        head->setPre(nullptr);
    }

    delete tempHead;
    delete tempTail;

    curSizeNum--;
    return node;
}

bool LRU::isEmpty() {
    return head == nullptr;
}



Page *Node::getPage() const {
    return page;
}

Node *Node::getPre() const {
    return pre;
}

Node *Node::getNext() const {
    return next;
}


void Node::setPre(Node *pre) {
    this->pre = pre;
}

void Node::setNext(Node *next) {
    this->next = next;
}

Node::Node(Page* page){
    this->page = page;
    this->pre = nullptr;
    this->next = nullptr;
};



#endif
#include <iostream>
#include <climits>
#include "InternalNode.h"

using namespace std;

InternalNode::InternalNode(int ISize, int LSize,
  InternalNode *p, BTreeNode *left, BTreeNode *right) :
  BTreeNode(LSize, p, left, right), internalSize(ISize)
{
  keys = new int[internalSize]; // keys[i] is the minimum of children[i]
  children = new BTreeNode* [ISize];
} // InternalNode::InternalNode()


BTreeNode* InternalNode::addPtr(BTreeNode *ptr, int pos)
{ // called when original was full, pos is where the node belongs.
  if(pos == internalSize)
    return ptr;

  BTreeNode *last = children[count - 1];

  for(int i = count - 2; i >= pos; i--)
  {
    children[i + 1] = children[i];
    keys[i + 1] = keys[i];
  } // shift things to right to make room for ptr, i can be -1!

  children[pos] = ptr;  // i will end up being the position that it is inserted
  keys[pos] = ptr->getMinimum();
  ptr->setParent(this);
  return last;
} // InternalNode:: addPtr()


void InternalNode::addToLeft(BTreeNode *last)
{
  ((InternalNode*)leftSibling)->insert(children[0]);

  for(int i = 0; i < count - 1; i++)
  {
    children[i] = children[i + 1];
    keys[i] = keys[i + 1];
  }

  children[count - 1] = last;
  keys[count - 1] = last->getMinimum();
  last->setParent(this);
  if(parent)
    parent->resetMinimum(this);
} // InternalNode::ToLeft()


void InternalNode::addToRight(BTreeNode *ptr, BTreeNode *last)
{
  ((InternalNode*) rightSibling)->insert(last);
  if(ptr == children[0] && parent)
    parent->resetMinimum(this);
} // InternalNode::addToRight()



void InternalNode::addToThis(BTreeNode *ptr, int pos)
{  // pos is where the ptr should go, guaranteed count < internalSize at start
  int i;

  for(i = count - 1; i >= pos; i--)
  {
      children[i + 1] = children[i];
      keys[i + 1] = keys[i];
  } // shift to the right to make room at pos

  children[pos] = ptr;
  keys[pos] = ptr->getMinimum();
  count++;
  ptr->setParent(this);

  if(pos == 0 && parent)
    parent->resetMinimum(this);
} // InternalNode::addToThis()



int InternalNode::getMaximum() const
{
  if(count > 0) // should always be the case
    return children[count - 1]->getMaximum();
  else
    return INT_MAX;
}  // getMaximum();


int InternalNode::getMinimum()const
{
  if(count > 0)   // should always be the case
    return children[0]->getMinimum();
  else
    return 0;
} // InternalNode::getMinimum()


InternalNode* InternalNode::insert(int value)
{  // count must always be >= 2 for an internal node
  int pos; // will be where value belongs

  for(pos = count - 1; pos > 0 && keys[pos] > value; pos--);

  BTreeNode *last, *ptr = children[pos]->insert(value);
  if(!ptr)  // child had room.
    return NULL;

  if(count < internalSize)
  {
    addToThis(ptr, pos + 1);
    return NULL;
  } // if room for value

  last = addPtr(ptr, pos + 1);

  if(leftSibling && leftSibling->getCount() < internalSize)
  {
    addToLeft(last);
    return NULL;
  }
  else // left sibling full or non-existent
    if(rightSibling && rightSibling->getCount() < internalSize)
    {
      addToRight(ptr, last);
      return NULL;
    }
    else // both siblings full or non-existent
      return split(last);
} // InternalNode::insert()


void InternalNode::insert(BTreeNode *oldRoot, BTreeNode *node2)
{ // Node must be the root, and node1
  children[0] = oldRoot;
  children[1] = node2;
  keys[0] = oldRoot->getMinimum();
  keys[1] = node2->getMinimum();
  count = 2;
  children[0]->setLeftSibling(NULL);
  children[0]->setRightSibling(children[1]);
  children[1]->setLeftSibling(children[0]);
  children[1]->setRightSibling(NULL);
  oldRoot->setParent(this);
  node2->setParent(this);
} // InternalNode::insert()


void InternalNode::insert(BTreeNode *newNode)
{ // called by sibling so either at beginning or end
  int pos;

  if(newNode->getMinimum() <= keys[0]) // from left sibling
    pos = 0;
  else // from right sibling
    pos = count;

  addToThis(newNode, pos);
} // InternalNode::insert(BTreeNode *newNode)


void InternalNode::print(Queue <BTreeNode*> &queue)
{
  int i;

  cout << "Internal: ";
  for (i = 0; i < count; i++)
    cout << keys[i] << ' ';
  cout << endl;

  for(i = 0; i < count; i++)
    queue.enqueue(children[i]);

} // InternalNode::print()


BTreeNode* InternalNode::remove(int value)
{  // to be written by students
    int pos;
    bool leafbelow = false;
    
    //Go from last key to first, check if any will work
    for(pos = count - 1; pos > -1 ; pos--)
    {
        //if a key is smaller than the value we need to delete, we go into that node
        if (keys[pos] <= value)
        {
            //we go to that children in pos and call remove, might go to leaf node or internal
            if (children[pos]->remove(value) == NULL)
            {
                //if it returns null, we know two leaves have merged
                leafbelow = true;
                for (int mergepos = pos; mergepos < count-1; mergepos++)
                {
                    
                    //if the right most leaf was deleted then just set that position to null
                    if (mergepos == count -1)
                    {
                        children[mergepos] = NULL;
                        break;
                        
                    }
                    children[mergepos] = children[mergepos+1];
                    
                    resetMinimum(children[mergepos]);
                    //cout << keys[mergepos];
                    
                }
                //decrement the count for the internal node
                count--;
            }
            
            break;
        }
        
    }
    
    
    
    //if there is only one node left with one child, return to Btree te pointer to child
    if (count == 1 && (getRightSibling() == NULL) && (getLeftSibling() == NULL) && (parent == NULL) )
    {
        children[0]->setParent(NULL);
        BTreeNode *returnednode = children[0];
        delete this;
        return returnednode;
    }
    
    //if (count == 1 )
    //{
        //children[0]->setParent(NULL);
        //return children[0];
    //}
    
    
    //Here we check if an internal node has less children then the minimum required
    if ((count < getminsize() || count == 1)&& parent != NULL  && leafbelow == true)
    {
        //Check left sib
        if (getLeftSibling() != NULL)
        {
            //see if we can borrow from left
            if (getLeftSibling()->getCount() -1 >= getminsize())
            {
                //cout << "Gotta get with left";
                borrowLeft();
            }
            //else merge with left
            else
            {
                mergeLeft();
                return NULL;
            }

            
        }
        
        //Check right sib
        else if (getRightSibling() != NULL)
        {
            
            if (getRightSibling()->getCount() -1 >= getminsize())
            {
                borrowRight();
            }
            else
            {
                //WRITE THE MERGE CODE DONT MESS WITH ANYTING ELSE NOW
                mergeRight();
                return NULL;
                //cout << "Gotta merge with right";
                
                
            }

            
            
        }
        
        
        
    }
    
    
    

  return this;
} // InternalNode::remove(int value)



void InternalNode::resetMinimum(const BTreeNode* child)
{
  for(int i = 0; i < count; i++)
    if(children[i] == child)
    {
      keys[i] = children[i]->getMinimum();
      if(i == 0 && parent)
        parent->resetMinimum(this);
      break;
    }
} // InternalNode::resetMinimum()


InternalNode* InternalNode::split(BTreeNode *last)
{
  InternalNode *newptr = new InternalNode(internalSize, leafSize,
    parent, this, rightSibling);


  if(rightSibling)
    rightSibling->setLeftSibling(newptr);

  rightSibling = newptr;

  for(int i = (internalSize + 1) / 2; i < internalSize; i++)
  {
    newptr->children[newptr->count] = children[i];
    newptr->keys[newptr->count++] = keys[i];
    children[i]->setParent(newptr);
  }

  newptr->children[newptr->count] = last;
  newptr->keys[newptr->count++] = last->getMinimum();
  last->setParent(newptr);
  count = (internalSize + 1) / 2;
  return newptr;
} // split()



int InternalNode::getminsize()
{
    return ((internalSize + 1) / 2);
}

void InternalNode::borrowLeft()
{
    /**/
    
    //HEre we het the first leaf from the right sib
    //children[count] = ((InternalNode*)getLeftSibling())->children[getLeftSibling()->getCount()-1];
    
    addToThis(((InternalNode*)getLeftSibling())->children[getLeftSibling()->getCount()-1],0);
    
    
    //after getting that leaf, change its parent pointer to this internal node
    children[count]->setParent(this);
    
    
    //increment the count of this internal node
    //count++;
    
    //reset min of the this internal node and its parent
    //resetMinimum(children[count-1]);
    //
    
    
    //Decrment the size of the right sib
    ((InternalNode*)getLeftSibling())->count--;
    
    //delete ((InternalNode*)getLeftSibling())->children[getLeftSibling()->getCount()];
    
    
    
  
    
    /**/

}

void InternalNode::mergeLeft()
{
    //GIve all the children to the right sibling
    for (int i =0; i < count; i++)
    {
        //children[i]->setParent(((InternalNode*)getRightSibling()));
        addToLeft(children[i]);
    }
    
    //If left sib is no null
    if (getRightSibling() != NULL)
    {
        //set the right sibling's left sibling as this siblings left sibling
        getLeftSibling()->setRightSibling(getRightSibling());
        //set this right sibling of this sibling's left sibling as the right sibling of this sibling
        getRightSibling()->setLeftSibling(getLeftSibling());
    }
    else
    {
        //if no left sib exists, set the right sib's left sib as NULL
        getLeftSibling()->setRightSibling(NULL);
    }
    
    delete this;

    
    
    
}

void InternalNode::borrowRight()
{
    /**/
    
    //HEre we het the first leaf from the right sib
    //children[count] = ((InternalNode*)rightSibling)->children[0];
    
    addToThis(((InternalNode*)rightSibling)->children[0], count);
    
    //after getting that leaf, change its parent pointer to this internal node
    //children[count]->setParent(this);

    
    //increment the count of this internal node
    //count++;
    
    //reset min of the this internal node and its parent
    resetMinimum(children[count-1]);
    //
    
    //move everything over to the left in the right sibling
    for (int i = 0; i < rightSibling->getCount() -1 ; i++)
    {
        ((InternalNode*)rightSibling)->children[i] = ((InternalNode*)rightSibling)->children[i+1];
        
        
        ((InternalNode*)rightSibling)->resetMinimum(((InternalNode*)rightSibling)->children[i]);
        
    }
    
    //Decrment the size of the right sib
    ((InternalNode*)rightSibling)->count--;
    
    
    
    if (((InternalNode*)rightSibling)->parent != NULL)
    {
        ((InternalNode*)rightSibling)->parent->resetMinimum(rightSibling);
    }
    
    
    /**/

}

void InternalNode::mergeRight()
{
    //GIve all the children to the right sibling
    for (int i =0; i < count; i++)
    {
        //children[i]->setParent(((InternalNode*)getRightSibling()));
        addToRight(children[i], children[i]);
    }
    
    //If left sib is no null
    if (getLeftSibling() != NULL)
    {
        //set the right sibling's left sibling as this siblings left sibling
        getRightSibling()->setLeftSibling(getLeftSibling());
        //set this right sibling of this sibling's left sibling as the right sibling of this sibling
        getLeftSibling()->setRightSibling(getRightSibling());
    }
    else
    {
        //if no left sib exists, set the right sib's left sib as NULL
        getRightSibling()->setLeftSibling(NULL);
    }

    delete this;    
}


InternalNode::~InternalNode()
{
    
    delete [] keys;
    delete [] children;
}




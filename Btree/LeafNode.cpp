#include <iostream>
#include <climits>
#include "LeafNode.h"
#include "InternalNode.h"
#include "QueueAr.h"

using namespace std;


LeafNode::LeafNode(int LSize, InternalNode *p,
  BTreeNode *left, BTreeNode *right) : BTreeNode(LSize, p, left, right)
{
  values = new int[LSize];
}  // LeafNode()

LeafNode::~LeafNode()
{
    delete values;
}

void LeafNode::addToLeft(int value, int last)
{
  leftSibling->insert(values[0]);

  for(int i = 0; i < count - 1; i++)
    values[i] = values[i + 1];

  values[count - 1] = last;
  if(parent)
    parent->resetMinimum(this);
} // LeafNode::ToLeft()

void LeafNode::addToRight(int value, int last)
{
  rightSibling->insert(last);

  if(value == values[0] && parent)
    parent->resetMinimum(this);
} // LeafNode::addToRight()

void LeafNode::addToThis(int value)
{
  int i;

  for(i = count - 1; i >= 0 && values[i] > value; i--)
      values[i + 1] = values[i];

  values[i + 1] = value;
  count++;

  if(value == values[0] && parent)
    parent->resetMinimum(this);
} // LeafNode::addToThis()


void LeafNode::addValue(int value, int &last)
{
  int i;

  if(value > values[count - 1])
    last = value;
  else
  {
    last = values[count - 1];

    for(i = count - 2; i >= 0 && values[i] > value; i--)
      values[i + 1] = values[i];
    // i may end up at -1
    values[i + 1] = value;
  }
} // LeafNode:: addValue()


int LeafNode::getMaximum()const
{
  if(count > 0)  // should always be the case
    return values[count - 1];
  else
    return INT_MAX;
} // getMaximum()


int LeafNode::getMinimum()const
{
  if(count > 0)  // should always be the case
    return values[0];
  else
    return 0;

} // LeafNode::getMinimum()


LeafNode* LeafNode::insert(int value)
{
  int last;

  if(count < leafSize)
  {
    addToThis(value);
    return NULL;
  } // if room for value

  addValue(value, last);

  if(leftSibling && leftSibling->getCount() < leafSize)
  {
    addToLeft(value, last);
    return NULL;
  }
  else // left sibling full or non-existent
    if(rightSibling && rightSibling->getCount() < leafSize)
    {
      addToRight(value, last);
      return NULL;
    }
    else // both siblings full or non-existent
      return split(value, last);
}  // LeafNode::insert()

void LeafNode::print(Queue <BTreeNode*> &queue)
{
  cout << "Leaf: ";
  for (int i = 0; i < count; i++)
    cout << values[i] << ' ';
  cout << endl;
} // LeafNode::print()


LeafNode* LeafNode::remove(int value)
{   // To be written by students
    
    
    ///START:: No Borrow
    int i;
    for(i = 0; i< count ; i++)
    {
        if (value == values[i])
        {
            for (int d = i; d < count-1; d++)
            {
                values[d] = values[d+1];
            }
           
            count--;
            if (parent != NULL)
            {
                parent->resetMinimum(this);
            }
            
            break;
        }
    }
    
    //END:No BORROW
    
    
    //Check if the leaf has become smaller than minimum size
    if (count < (leafSize + 1) / 2)
    {
        //check if left sib exists
        if (getLeftSibling() != NULL)
        {
            //if it exists chck if it can give up an int
            if (getLeftSibling()->getCount() -1 >= (leafSize + 1) / 2)
            {
                for (int d = 0; d < count; d++)
                {
                    values[d+1] = values[d];
                }
                values[0] = getLeftSibling()->getMaximum();
                count++;
                getLeftSibling()->remove(values[0]);
                
                parent->resetMinimum(this);
                
            }
            else
            {
                //if it cant give up a value, merge
                for (int insertpos = 0; insertpos < count; insertpos++)
                {
                    getLeftSibling()->insert(values[insertpos]);
                }
                
                getLeftSibling()->setRightSibling(NULL);
                parent->resetMinimum(this);
                this->setParent(NULL);
                delete this;
                return NULL;
                
            }
            

            
        }
        else if (getRightSibling() != NULL)
        {
            if (getRightSibling()->getCount() -1 >= (leafSize + 1) / 2)
            {
                values[count] = getRightSibling()->getMinimum();
                count++;
                getRightSibling()->remove(values[count]);
                
            }
            else
            {
                
                //MERGE WITH RIGHT
                //WItht the insert code of right
                
            }
        }
        
    }
    
  return this;  // filler for stub
}  // LeafNode::remove()


int LeafNode::getminsize()
{
    return ((leafSize + 1) / 2);
}


LeafNode* LeafNode::split(int value, int last)
{
  LeafNode *ptr = new LeafNode(leafSize, parent, this, rightSibling);


  if(rightSibling)
    rightSibling->setLeftSibling(ptr);

  rightSibling = ptr;

  for(int i = (leafSize + 1) / 2; i < leafSize; i++)
    ptr->values[ptr->count++] = values[i];

  ptr->values[ptr->count++] = last;
  count = (leafSize + 1) / 2;

  if(value == values[0] && parent)
    parent->resetMinimum(this);
  return ptr;
} // LeafNode::split()


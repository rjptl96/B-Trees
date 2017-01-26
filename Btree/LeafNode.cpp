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
            
            break;
        }
    }
    //END:No BORROW
    
    //Check if the leaf has become smaller than minimum size
    if (count < (leafSize + 1) / 2)
    {
        if (leftSibling != NULL)
        {
            int getfromleft;
            count++;
            getfromleft = leftSibling->getfromleft();
            if (getfromleft == 1)
            {
                cout << "YAY";
            }
            else if(getfromleft == 0)
            {
                cout << "Gotta Merge";
            }
            
        }
        else if (rightSibling != NULL)
        {
            int getfromright;
            getfromright = rightSibling->getfromright(count);
            
            if (getfromright == 1)
            {
                cout << "YAY";
            }
            else if(getfromright == 0)
            {
                cout << "Gotta Merge";
            }
            
            
            
        }
        else
        {
        
        //This thang doesnt have any siblings
        }
    }
    
  return NULL;  // filler for stub
}  // LeafNode::remove()



int* LeafNode::getkeysorvalues()
{
    return values;
}




bool LeafNode::getfromright(int &end)
{
    if ( count -1 >= getminsize() )
    {
        int returnedvalue = values[0];
        for (int d = 0; d < count-1; d++)
        {
            values[d] = values[d+1];
        }
        
        count--;
        //reset the parents key for min
        parent->resetMinimum(this);
        leftSibling->getkeysorvalues()[end] = returnedvalue;
        end++;
        
        return 1;
    }
    else
    {
        return 0;
    }

    
}
bool LeafNode::getfromleft()
{
    if ( count -1 >= getminsize() )
    {
        int returnedvalue = values[count-1];
        //rightSibling
        
        count--;
        rightSibling->getkeysorvalues()[1] = rightSibling->getkeysorvalues()[0];
        rightSibling->getkeysorvalues()[0] = returnedvalue;
        
    
        return 1;
    }
    else
    {
        return 0;
    }

    
}



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

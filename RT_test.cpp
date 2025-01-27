//
// Test.cpp
//
// This is a direct port of the C version of the RTree test program.
//

#include <iostream>
#include "RTree.h"

using namespace std;

typedef XMLNode ValueType;
//typedef int ValueType;

typedef int KINDTYPE;

list<ValueType> node_list;

struct Rect
{
  Rect()  {}

  Rect(int a_minX, int a_minY, int a_maxX, int a_maxY)
  {
    min[0] = a_minX;
    min[1] = a_minY;

    max[0] = a_maxX;
    max[1] = a_maxY;
  }


  int min[2];
  int max[2];
};

// struct Rect rects[] =
// {
//   Rect(0, 0, 2, 2), // xmin, ymin, xmax, ymax (for 2 dimensional RTree)
//   Rect(5, 5, 7, 7),
//   Rect(8, 5, 9, 6),
// //   Rect(7, 1, 9, 2),
// // };

// int nrects = sizeof(rects) / sizeof(rects[0]);

struct Rect rects[1000];

XMLNode xmln;
//This is a operation of target rect

bool MySearchCallback(ValueType id, void* arg, KINDTYPE kind)
{
  // cout << "Hit data rect " << id <<" kind = "<<kind<<"\n";
  cout << "Hit data rect m_childLen" << id.m_childLen <<" kind = "<<kind<<"\n";
 // node_list.insert(node_list.end(),id);
  node_list.push_back(id);
  return true; // keep going
}


int main()
{
  typedef RTree<ValueType, int, 2, float> MyTree;
  MyTree tree;

  int num_of_rects,i, nhits;
  int x0,y0,wi,hi;
  KINDTYPE k;
  cin>>num_of_rects;
  //input rects
  for(i=0;i<num_of_rects;i++){
    cin>>x0>>y0>>wi>>hi>>k;
    rects[i] = Rect(x0, y0, x0+wi, y0+hi);
    xmln.m_childLen = i;
    ///插入函数
    /// \param 最小点坐标,最大点坐标,id,种类 
    tree.Insert(rects[i].min, rects[i].max, xmln, k); // Note, all values including zero are fine in this version
  }
  //int nrects = sizeof(rects) / sizeof(rects[0]);
  int nrects = num_of_rects;
  cout << "nrects = " << nrects << "\n";

  node_list.clear();

  cin>>x0>>y0>>wi>>hi>>k;
  Rect search_rect(x0, y0,x0+wi, y0+hi); // search will find above rects that this one overlaps
  //tree.Remove(rects[6].min,rects[6].max,6);   //remove the leafnode from R-tree
  // list<ValueType>::iterator i;
  // for (i = node_list.begin(); i!= node_list.end(); ++i)
  //          cout <<i->m_childLen<< " ";
 
  ///查询函数
  /// \param 最小点坐标,最大点坐标,返回函数指针,联系,种类
  nhits = tree.Search(search_rect.min, search_rect.max, MySearchCallback, NULL,k);

  cout << "Search resulted in " << nhits << " hits\n";
   if(!node_list.empty()){
      ValueType temp = node_list.front();
      cout<<"back = "<<temp.m_childLen<<endl;
  }
  ///删除所有的条目
  tree.RemoveAll();   //Remove all of node and the tree
  // Iterator test
  // 遍历输出所有条目(执行里RemoveAll 所以为空)
  // int itIndex = 0;
  // MyTree::Iterator it;
  // for( tree.GetFirst(it);
  //      !tree.IsNull(it);
  //      tree.GetNext(it) )
  // {
  //   int value = tree.GetAt(it);

  //   int boundsMin[2] = {0,0};
  //   int boundsMax[2] = {0,0};
  //   it.GetBounds(boundsMin, boundsMax);
  //   cout << "it[" << itIndex++ << "] " << value << " = (" << boundsMin[0] << "," << boundsMin[1] << "," << boundsMax[0] << "," << boundsMax[1] << ")\n";
  // }

  // // Iterator test, alternate syntax
  // itIndex = 0;
  // tree.GetFirst(it);
  // while( !it.IsNull() )
  // {
  //   int value = *it;
  //   ++it;
  //   cout << "it[" << itIndex++ << "] " << value << "\n";
  // }

  return 0;

}


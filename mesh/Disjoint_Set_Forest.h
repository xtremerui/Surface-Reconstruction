/*
*
*  Disjoint_Set_Forest.h -- an implementation for disjoint set data structure
*
*  Created by Ge Chunyuan on 04/09/2009.
*
*  version: 0.1
*/
#pragma once
#ifndef _DISJOINT_SET_H_
#define _DISJOINT_SET_H_
#include <vector>
template <typename T> class DisjointSet
{
public:
	DisjointSet();
	~DisjointSet();
	void    makeSet		( const std::vector<T*> s );
	bool	findSet		( const T* s, T* parent);
	void	Union		( const T* s1, const T* s2 );
protected:
	struct Node
	{
		int		rank;
		T*		data;
		Node*	parent; 
	};
	int m_nElementCnt; 
	int m_nSetCnt; 
	std::vector<Node*> m_Nodes; 
};
template< class T> DisjointSet<T>::DisjointSet()
{
	m_nElementCnt = 0;	
	m_nSetCnt = 0;		
}
template< class T> DisjointSet<T>::~DisjointSet()
{
	for (int i=0;i<m_nElementCnt;i++)
		delete m_Nodes[i];
}
template< class T> void DisjointSet<T>::makeSet( const std::vector<T*> s )
{
	m_nElementCnt += (int)s.size();
	m_nSetCnt += (int)s.size();
	typename std::vector<T*>::const_iterator it = s.begin();
	for (;it != s.end(); ++ it)
	{
		Node* pNode = new Node;
		pNode->data = *it;

		pNode->parent = NULL;
		pNode->rank = 0;
		m_Nodes.push_back(pNode);
	}
}
template< class T> bool DisjointSet<T>::findSet( const T* s, T* parent)
{
	
	Node* curNode = NULL;
	bool find =false;	
	for (int i=0;i<(int)m_Nodes.size();i++)
	{
		curNode = m_Nodes[i];
		if (curNode->data == s)
		{
			find = true;
			break;
		}
	}
	
	if (!find) return false;
	
	// find the root 
	Node* pRoot = curNode;
	while (pRoot->parent != NULL)
	{
		pRoot = pRoot->parent;
	}
	
	// update all curNode's parent to root
	while (curNode != pRoot)
	{
		Node* pNext = curNode->parent;
		curNode->parent = pRoot;
		curNode = pNext;
	}
	//printf("find %d ",pRoot->data->index);fflush(stdout);
	*parent = *(pRoot->data);
	//printf("done ");fflush(stdout);
	return true;
}
template< class T> void	DisjointSet<T>::Union( const T* s1, const T* s2 )
{
	Node* pNode1 = NULL;
	Node* pNode2 = NULL;
	//printf("s1 %d \n", s1->index);fflush(stdout);
	//printf("s2 %d \n", s2->index);fflush(stdout);
	int find = 0;
	for (int i=0;i<(int)m_Nodes.size();++i)
	{
		if (m_Nodes[i]->data->index == s1->index || m_Nodes[i]->data->index == s2->index )
		{
			find ++;
			//printf("mnode %d \n", m_Nodes[i]->data->index);fflush(stdout);
			if (m_Nodes[i]->data->index == s1->index)
				pNode1 = m_Nodes[i];
			else
				pNode2 = m_Nodes[i];
		}
	}
	// not found 
	//printf("aa %d",find);fflush(stdout);
	if ( find != 2) return ;
	//printf("p1rank: %d", pNode1->rank);fflush(stdout);
	//printf("p2rank: %d", pNode2->rank);fflush(stdout);
	if (pNode1->rank > pNode2->rank)
		pNode2->parent = pNode1;
	else if (pNode1->rank < pNode2->rank)
		pNode1->parent = pNode2;
	else
	{
		pNode2->parent = pNode1;
		++ pNode1->rank;
	}
	--m_nSetCnt;
	//printf("ap1Par: %d", pNode1->parent->data->index);fflush(stdout);
		//printf("ap2Par: %d", pNode2->parent->data->index);fflush(stdout);
}
#endif //_DISJOINT_SET_H_

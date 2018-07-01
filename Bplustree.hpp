//
//  Bplustree.hpp
//  LASTSQL
//
//  Created by 缪晴朗 on 18/6/30.
//  Copyright © 2018年 缪晴朗. All rights reserved.
//

/*
 实现一棵B+树（bptnp M)
 
 叶子节点是bptnp类型 包含element e, long offset
 
 路标由element生成
 
 
 查找element 返回一个offset
 
 */
//#include "BPtree.hpp"
//#include "BufferManger.h"
 
#ifndef Bplustree_hpp
#define Bplustree_hpp

#include <stdio.h>
#include<iostream>
#include<time.h>
#include <stdlib.h>
using namespace std;


#define level 3	//B+树的阶,必须为大于3奇数

typedef	 int KEYTYPE;
typedef	 unsigned long	FILEP;


//B+树节点的数据结构
typedef struct
{
    KEYTYPE	key[level] ;		//关键字域
    FILEP	Pointer[level+1] ;	//指针域
    long  offset;            //该关键字在文件中对应的偏移量
    int		nkey ;				//关键字数
    bool	isleaf ;				//是否为叶节点 叶节点:true 否则为false
    
}BPlusNode;


//插入关键字的数据结构
typedef struct
{
    KEYTYPE	key;
    FILEP	Raddress;		  //该关键字对应记录的地址
    long offset;
}TRecord;


//保存查询结果的数据结构
typedef struct
{
    bool	exist;
    FILEP	Baddress;	//保存包含该记录的B+树节点地址
    FILEP	Raddress;	//该关键字的所指向的记录地址
    long offset_result;
    
}SearchResult;



class BPlusTree
{
    FILEP   ROOT;		//树根在文件内的偏移地址
    FILE	*Bfile;		//B+树文件的指针
    //  FILE	*Rfile;		//记录文件的指针
    
public:
    
    FILEP	GetBPlusNode() const;//在磁盘上分配一块B+树节点空间
    void	ReadBPlusNode(const FILEP ,BPlusNode& ) const;//读取address地址上的一块B+树节点
    void	WriteBPlusNode(const FILEP ,const BPlusNode& );//将一个B+树节点写入address地址
    
    void	Build_BPlus_Tree();//建立一棵空B+树
    
    
    void	Insert_BPlus_Tree(TRecord& );//向B+树插入关键字
    void	insert_bplus_tree(FILEP ,TRecord& );
    
    void	Split_BPlus_Node(BPlusNode& ,BPlusNode& ,const int );
    
    void	Search_BPlus_Tree(TRecord& ,SearchResult& ) const;
    
    void	Delete_BPlus_Tree(TRecord& );
    void	delete_BPlus_tree(FILEP ,TRecord& );
    
    void  EnumLeafKey();
    
    
    BPlusTree();
    ~BPlusTree();
};

#endif /* Bplustree_hpp */

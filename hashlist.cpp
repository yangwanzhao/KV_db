#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <functional>

#include "hashlist.h"
 
using namespace std;
//pHash_Header hash_list[HASH_NUM_MAX];
 
 
/*initial a hashlist*/
pHash_List init_hash_list(void)
{
    u32 i;
    pHash_List plist;
    //pHash_Header phead;
    
    plist = (Hash_List *)malloc(sizeof(Hash_List)); 
    
    for( i = 0;i < HASH_NUM_MAX;i++ )
    {
        plist->list[i] = (Hash_Header *)malloc(sizeof(Hash_Header)); 
        plist->list[i]->next = NULL;
    }     
    
    return plist;
}
 
#if 0
/*initial a hash header*/
pHash_Header init_hash_header(void)
{
    Hash_Header *phead;
    
    phead = (Hash_Header *)malloc(sizeof(Hash_Header));
    phead->next = NULL;
    
    return phead;
}
 
/*initial a hash node header*/ 
pNode init_node_header(void)
{
    Node *phead;
    
    phead = (Node *)malloc(sizeof(Node));
    phead->next = NULL;
    
    return phead;      
} 
 
/*initial all node header*/
void init_all_node_header(void)
{
    u32 i;
    Hash_Header *plist;
    
    for( i = 0;i < HASH_NUM_MAX;i++)
    {
       hash_list[i] = init_hash_header(); 
    }
}
#endif
/*insert a node by id*/
pNode insert_node_to_hash(pHash_List plist,string key,string value)
{
    Node *ptail,*pre,*p;
    u32 id;
    
    std::hash<std::string> h;
    size_t n = h(key);
    id = n % HASH_NUM_MAX;

    ptail = (Node *)malloc(sizeof(Node));
    ptail->next = NULL;
    ptail->value = value;
    ptail->id   = id;
    ptail->key   = key;

    if( NULL == plist->list[id]->next )
    {
        plist->list[id]->next = ptail;
        cout << "链表插入点id=" << ptail->id << endl;; 
        
        return ptail;
    }
    pre = plist->list[id]->next;
    while( pre )
    {
        p = pre;
        pre = pre->next;   
    }
    p->next = ptail;
    cout << "链表插入点id=" << ptail->id << endl; 
    
    return ptail;
}
 
/*delete a node by id*/
int delete_node_to_hash(pHash_List plist,string key)
{
   Node *psea;
   u32 id;
    
   std::hash<std::string> h;
   size_t n = h(key);
   id = n % HASH_NUM_MAX;
   
   psea = plist->list[id]->next; 
   if( NULL == psea )
   {
       cout << "搜索的链表内容为空" << endl; 
       return -1; 
   }
 #if 1  
   if(key == psea->key )
   {
       plist->list[id]->next = psea->next;  
       free(psea);
       //printf("该链表唯一元素被删除\n"); 
       cout << "正确删除数据" << endl; 
       return 0; 
   }
 #endif  
   if( NULL == psea->next )
   {
       cout << "搜索的id不存在" << endl;; 
       return -2;    
   } 
   
   while( key != psea->next->key )
   {
       psea = psea->next;
       if( NULL == psea->next )
       {
           cout << "搜索的id不存在" << endl;; 
           return -2;    
       }       
   } 
   psea->next = psea->next->next;
   free(psea->next);
   
   cout << "正确删除数据" << endl; 
   return 0;  
}
 
/*print the whole hash table*/
void print_hash(pHash_List plist)
{
    u32 i;
    pNode p; 
    
    cout << "打印整个哈希表信息:" << endl; 
    
    for( i = 0;i < HASH_NUM_MAX;i++)
    {
        p = plist->list[i]->next;
        if(NULL != p)
        {
            cout << "第" << i << "排: " << endl;       
        }
  
        while( NULL != p )
        {
            cout << "id=" << p->id << " key=" << p->key << " value=" << p->value << endl;
            
            p = p->next;
        }        
    }
} 
 
/*free the whole hash table*/
void free_all_hash(pHash_List plist)
{
    u32 i;
    pNode p,pn; 
    
    cout << "\nfree the whole hashtable" << endl; 
    
    for( i = 0;i < HASH_NUM_MAX;i++)
    {
        p = plist->list[i]->next;;
        pn = p;
        if(NULL == p)
        {
            continue;
        }
        cout << "释放第" << i << "排: " << endl;    
        while( NULL != pn )
        {
            p = pn;
            pn = p->next;
            
            cout << "id=" << p->id << ", value=" << p->value << endl;  
            free(p);              
            //p = p->next;
        }      
        free(plist->list[i]);  
    }
    
} 


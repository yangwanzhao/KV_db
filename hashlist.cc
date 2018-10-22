#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <functional>
#include <regex>

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
string insert_node_to_hash(pHash_List plist, string data)
{
  Node *ptail,*pre,*p;
  u32 id, position, len_key, len_value;
  string key, value, response;

  // (u32) len_key
  position = data.find("\n");
  len_key = stoi(data.substr(0, position));
  data = data.substr(position+1);
  // (string) key
  position = data.find("\n");
  key = data.substr(0, position);
  data = data.substr(position+1);
  // (u32) len_value
  position = data.find("\n");
  len_value = stoi(data.substr(0, position));
  // (string) value 
  value = data.substr(position+1);

  std::hash<std::string> h;
  size_t n = h(key);
  id = n % HASH_NUM_MAX;

  ptail = (Node *)malloc(sizeof(Node));
  ptail->next = NULL;
  ptail->value = value;
  ptail->id   = id;
  ptail->key   = key;
  ptail->len_key = len_key;
  ptail->len_value = len_value;


  if( NULL == plist->list[id]->next )
  {
    plist->list[id]->next = ptail;
    response = "OK";
    return response;
  }
  pre = plist->list[id]->next;
  while( pre )
  {
    p = pre;
    pre = pre->next;   
  }
  p->next = ptail;
  response = "OK";
  return response;
}

/*delete a node by id*/
string delete_node_to_hash(pHash_List plist,string data)
{
  Node *psea;
  u32 id, position, len_key;
  string key, response;

  // (u32) len_key
  position = data.find("\n");
  len_key = stoi(data.substr(0, position));
  // (string) key
  key = data.substr(position+1);
  
  std::hash<std::string> h;
  size_t n = h(key);
  id = n % HASH_NUM_MAX;

  psea = plist->list[id]->next; 
  if( NULL == psea )
  {
   response = "ERROR";
   return response;
 } 
 if(key == psea->key )
 {
   plist->list[id]->next = psea->next;  
   free(psea);
   response = "OK";
   return response;
 }
 if( NULL == psea->next )
 {
   response = "ERROR";
   return response; 
 } 

 while( key != psea->next->key )
 {
   psea = psea->next;
   if( NULL == psea->next )
   {
     response = "ERROR";
     return response;
   }       
 } 
 psea->next = psea->next->next;
 free(psea->next);

 response = "OK";
 return response;
}


string get_node_to_hash(pHash_List plist, string data)
{
  Node *psea;
  u32 id, position, len_key;
  string key, response;

  // (u32) len_key
  position = data.find("\n");
  len_key = stoi(data.substr(0, position));
  // (string) key
  key = data.substr(position+1);

  std::hash<std::string> h;
  size_t n = h(key);
  id = n % HASH_NUM_MAX;

  psea = plist->list[id]->next; 
  if( NULL == psea )
  {
    response = "ERROR";
    return response;
  }
  
  if(key == psea->key )
  {
    response = "OK\nRESULT-LEN:" + to_string(psea->len_value) + "\nRESULT:" + psea->value + "\n";
    return response; 
  } 
  if( NULL == psea->next )
  {
    response = "ERROR";
    return response;
  }

  while( key != psea->next->key )
  {
    psea = psea->next;
    if( NULL == psea->next )
    {
     response = "ERROR";
     return response;
   }   
 }

 response = "OK\nRESULT-LEN:" + to_string(psea->len_value) + "\nRESULT:" + psea->value + "\n";
 return response;  
}

string regexp_node_to_hash(pHash_List plist, string data)
{
  u32 i, num_results, position, len_regexp;
  string sub_str, regexp, response;
  pNode p; 

  // (u32) len_key
  position = data.find("\n");
  len_regexp = stoi(data.substr(0, position));
  // (string) key
  regexp = data.substr(position+1);


  num_results = 0;
  response = "";
  for( i = 0;i < HASH_NUM_MAX;i++)
  {
    p = plist->list[i]->next;

    while( NULL != p )
    {
      if (regex_match(p->key, regex(regexp)))
      {
        num_results++;
        response = response + to_string(p->len_key) + "\n" + p->key + "\n" + to_string(p->len_value) + "\n" + p->value+ "\n";
      }
      p = p->next;
    }        
  }

  response = to_string(num_results) + "\n" + response;
  return response;

  // for (int i = 0; i < num_results; ++i)
  // {
  //   for (int j = 0; j < 4; ++j)
  //   {
  //     position = results.find("\n");
  //     if (position != results.npos)
  //     {
  //       sub_str = results.substr(0, position);
  //       results = results.substr(position+1);
  //     }

  //     switch (j) {
  //       case 0:
  //       cout << "RESULT-KEY-LEN:" << sub_str << endl;
  //       break;
  //       case 1:
  //       cout << "RESULT-KEY:" << sub_str << endl;
  //       break;
  //       case 2:
  //       cout << "RESULT-VAL-LEN:" << sub_str << endl;
  //       break;
  //       case 3:
  //       cout << "RESULT-VAL:" << sub_str << endl;
  //       break;
  //       default:
  //       break;
  //     }
  //   }
  // }

}
/*print the whole hash table*/
void print_hash(pHash_List plist)
{
  u32 i;
  pNode p; 

  cout << "Print the hash table:" << endl; 

  for( i = 0;i < HASH_NUM_MAX;i++)
  {
    p = plist->list[i]->next;

    while( NULL != p )
    {
      cout << "id=" << p->id << " len_key=" << p->len_key << " key=" << p->key << " len_value=" << p->len_value << " value=" << p->value << endl;
      p = p->next;
    }        
  }
  cout << endl;
} 

/*free the whole hash table*/
void free_all_hash(pHash_List plist)
{
  u32 i;
  pNode p,pn; 

  cout << "Free the whole hashtable" << endl; 

  for( i = 0;i < HASH_NUM_MAX;i++)
  {
    p = plist->list[i]->next;;
    pn = p;
    if(NULL == p)
    {
      continue;
    }   
    while( NULL != pn )
    {
      p = pn;
      pn = p->next;

      cout << "id=" << p->id << " key=" << p->key << " value=" << p->value << endl;  
      free(p);              
            //p = p->next;
    }      
    free(plist->list[i]);  
  }

} 


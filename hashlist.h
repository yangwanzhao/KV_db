#ifndef _H_HASHLIST_
#define _H_HASHLIST_
 
#define HASH_NUM_MAX 65536
 
#define u32 unsigned int
 
//typedef struct _Node * pNode;
//typedef struct _Hash_Header * pHash_Header;
 
typedef struct _Node
{
	std::string key;
    u32 id;
    std::string value;
    struct _Node *next;        
}Node,*pNode;
 
typedef struct _Hash_Header
{
    struct _Node *next;    
}Hash_Header,*pHash_Header;
 
typedef struct _Hash_List
{
    struct  _Hash_Header* list[HASH_NUM_MAX];           
}Hash_List,*pHash_List;
 
pHash_List init_hash_list(void);
//pHash_Header init_hash_header(void);
//pNode init_node_header(void);
//void init_all_node_header(void);
pNode insert_node_to_hash(pHash_List plist,std::string key,std::string value);
int delete_node_to_hash(pHash_List plist,std::string key);
void print_hash(pHash_List plist);
void free_all_hash(pHash_List plist); 
 
 
#endif


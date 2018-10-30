#ifndef _H_HASHLIST_
#define _H_HASHLIST_
 
#define HASH_NUM_MAX 65536
 
#define u32 unsigned int
 
//typedef struct _Node * pNode;
//typedef struct _Hash_Header * pHash_Header;
 
typedef struct _Node
{
	std::string key;
    u32 id,len_key,len_value;
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
std::string insert_node_to_hash(pHash_List plist,std::string data);
std::string delete_node_to_hash(pHash_List plist,std::string data);
std::string reduce_node_to_hash(pHash_List plist,std::string data);
void print_hash(pHash_List plist);
std::string get_node_to_hash(pHash_List plist, std::string data);
void free_all_hash(pHash_List plist); 
std::string regexp_node_to_hash(pHash_List plist, std::string data);

void init_hash(pHash_List plist); 
 

#endif


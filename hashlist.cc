#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <functional>
#include <v8.h>
#include <regex>
#include <boost/filesystem.hpp>
#include <libgen.h>
#include <unistd.h>
#include <boost/asio.hpp>

#include "common.h"
#include "print.h"
#include "hashlist.h"

using namespace std;
using namespace v8;


// v8 part
static char fil_share[128]; // for shared key between c++ and js - filter
static char com_share[4096];
static char com_res_share[128];
// vector<string> com_share;   // for shared value vector between c++ and js - combine

static Handle<Value> FilGetter(Local<String> name, const AccessorInfo& info) {
  return String::New((char*)&fil_share,strlen((char*)&fil_share)); 
} 

static void FilSetter(Local<String> name, Local<Value> value, const AccessorInfo& info) {
  Local<String> str = value->ToString(); 
  str->WriteAscii((char*)&fil_share); 
}

static Handle<Value> ComGetter(Local<String> name, const AccessorInfo& info) {
  return String::New((char*)&com_share,strlen((char*)&com_share)); 
} 

static void ComSetter(Local<String> name, Local<Value> value, const AccessorInfo& info) {
  Local<String> str = value->ToString(); 
  str->WriteAscii((char*)&com_share); 
}

static Handle<Value> ComResGetter(Local<String> name, const AccessorInfo& info) {
  return String::New((char*)&com_res_share,strlen((char*)&com_res_share)); 
} 

static void ComResSetter(Local<String> name, Local<Value> value, const AccessorInfo& info) {
  Local<String> str = value->ToString(); 
  str->WriteAscii((char*)&com_res_share); 
}

const std::string load_file(const std::string &name) {
// use boost to load a file from an input stream: 
  std::ifstream file{name};
  const auto file_size = boost::filesystem::file_size(name); 
  std::string data(file_size, ' ');
  file.read(data.data(), file_size);
  return data;
}

// v8 part


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
    if (pre->key == ptail->key)
    {
      response = "ERROR";
      return response;
    }
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
    response = "OK\nRESULT-LEN:" + to_string(psea->len_value) + "\nRESULT:" + psea->value;
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

 response = "OK\nRESULT-LEN:" + to_string(psea->len_value) + "\nRESULT:" + psea->value;
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
        response = response + "\nRESULT-KEY-LEN:" + to_string(p->len_key) + "\nRESULT-KEY:" + p->key + 
        "\nRESULT-VAL-LEN:" + to_string(p->len_value) + "\nRESULT-VAL:" + p->value;
      }
      p = p->next;
    }        
  }
  if (num_results > 0)
  {
    response = "OK\nNUM-RESULTS:" + to_string(num_results) + response;
  }
  else
  {
    response = "ERROR";
  }

  return response;

}

// ----------------------- JUST FOR TEST -------------
void init_hash(pHash_List plist)
{
  string data;
  data = "3\nwww\n2\nqq";
  insert_node_to_hash(plist, data);
  data = "4\nwaaa\n5\nqaaaq";
  insert_node_to_hash(plist, data);
  data = "5\nqavqw\n7\nqasdfsq";
  insert_node_to_hash(plist, data);
  data = "4\nqaqw\n2\nqa";
  insert_node_to_hash(plist, data);
  data = "6\nqaa2qw\n4\nq22a";
  insert_node_to_hash(plist, data);
}
// ----------------------- JUST FOR TEST -------------


string reduce_node_to_hash(pHash_List plist, string data)
{
  // decode filter and combine
  u32 position, filter_len, combine_len, num_value=0;
  string filter, combine, key_shared, value_shared, Readonly, response;
  pNode p; 

  // data: filter_text.length() + "\n" + filter_text + "\n" + combine_text.length() + "\n" + combine_text
  position = data.find("\n");
  filter_len = stoi(data.substr(0, position));
  // data: filter_text + "\n" + combine_text.length() + "\n" + combine_text
  data = data.substr(position+1);
  filter = data.substr(0, filter_len);
  // data: combine_text.length() + "\n" + combine_text
  data = data.substr(filter_len+2); 
  position = data.find("\n");
  combine_len = stoi(data.substr(0, position));
  // data: combine_text
  combine = data.substr(position+1);
  Readonly = "Readonly_Server.js";
  combine = load_file(Readonly.c_str()) + combine;



  // cout << "filter:\n" << filter << endl;
  // cout << "\ncombine:\n" << combine << endl;


  Locker locker;
  // RAII v8 scope manager, to ensure that all handles get cleaned up 
  HandleScope handle_scope;
  // a definition of the global context. Anything in this goes into every
  // context built from it
  Handle<ObjectTemplate> globalTemplate = ObjectTemplate::New();

  //public the name variable to script 
  globalTemplate->SetAccessor(String::New("shared_key"), FilGetter, FilSetter);
  globalTemplate->SetAccessor(String::New("shared_val"), ComGetter, ComSetter);
  globalTemplate->SetAccessor(String::New("shared_val_res"), ComResGetter, ComResSetter);

  // add print_message to the global template
  globalTemplate->Set(v8::String::New("print"), 
    FunctionTemplate::New(print_message)); 

  // create a context in which to run scripts
  Handle<Context> context = Context::New(NULL, globalTemplate); 


  // filter each key value pair
  for(int i = 0;i < HASH_NUM_MAX;i++)
  {
    p = plist->list[i]->next;

    while( NULL != p )
    {
      key_shared = p->key;   // what if key == true
      strcpy(fil_share, key_shared.c_str());
      run_script(filter, context);
      key_shared = fil_share;
      if (key_shared == "true")    // think a better way to return true
      {
        num_value++;
        value_shared = value_shared + to_string(p->value.length()) + "\n" + p->value + "\n";
      }
      p = p->next;
    }        
  }
  value_shared = to_string(num_value) + "\n" + value_shared;
  strcpy(com_share, value_shared.c_str());
  run_script(combine, context);

  response = com_res_share;
  response = "OK\nRESULT-LEN:" + to_string(response.length()) + "\nRESULT:" + response;

  return response;
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


/**
 * CSE 411
 * Fall 2018
 * Programming Assignment #3
 *
 * The purpose of this assignment is to gain more experience with C++ support
 * for embedded scripting, networking, and regular expressions.  We will do so
 * by creating a server that implements a key/value store.
 *
 * Key/value stores play a significant role in modern distributed systems.  They
 * allow a program to easily store data to a high-availability server, and to
 * easily retrieve that data.  A key/value store may be persistent, or it may be
 * an in-memory cache.  Key/value stores are usually replicated and distributed,
 * but we won't worry about those features for this assignment.  Examples of 
 * key/value stores include Redis, MongoDB, Memcached, DynamoDB, Firebase, 
 * CouchBase, Voldemort, and many more.  Since key/value stores do not track 
 * many-one relationships among elements, they are sometimes called "NoSQL"
 * databases.
 *
 * At a bare minimum, a key/value store must provide the same interface that a 
 * C++ map would offer:
 *   - get(key) 
 *       Returns the value associated with a key, if present, or null otherwise
 *   - del(key)
 *       Removes key and its associated value from the store, and returns true.
 *       If the key wasn't in the store, returns false.
 *   - put(key, value) 
 *       Adds a mapping from key to value, if no prior mapping for key exists.  
 *       If a mapping existed, then it is modified, so that the mapping from key
 *       will be to the provided value.
 *
 * Note that even implementing this interface is not trivial, because the 
 * key/value store must be highly concurrent.  That is, it should use as many 
 * threads as are available, and should allow operations on different data 
 * elements to proceed simultaneously as much as possible.
 * 
 * To the above interface, we will add the following methods:
 *   - where(regexp)
 *       Returns all of the key/value pairs in the data set for which the key
 *       matches the regular expression provided by regexp.
 *   - reduce(JavaScript filter, JavaScript combine)
 *       Runs the provided JavaScript code and returns the result.  See below 
 *       for more details.
 * 
 * As you may have guessed, in order to be as general as possible, key/value 
 * stores typically treat the key as a string, and the value as a byte array.  
 * If there is any structure to the values, the key/value store does not know 
 * about it.  So, for example, a reduce() filter may know that the value can be 
 * parsed as a JSON object, but the key/value store itself would not know that 
 * the value was anything other than a byte array.
 * ·
 * To implement the key/value store data structure, you cannot simply use the 
 * std::unordered_map, because it does not support concurrency.  You will need 
 * to create your own data structure.  The easiest approach is to create a large
 * array (at least 2^16 elements), where each array element holds a linked list.  
 * You can use the built-in C++ hash function to hash a key to one of these 
 * lists, and you can achieve concurrency by locking at the granularity of 
 * lists.  Note that for where() and reduce(), this means your result may not be
 * "consistent", because your code will only lock one list at a time.  Once you 
 * have this implemented, testing get(), del(), and put() should be easy.
 * 
 * To implement where(), your code should use the C++ regex library.  That is, 
 * you should go through one list at a time, and for each key, use the regex to 
 * determine if it matches, and if so, add *a copy of* the key/value pair to the
 * result that you will return.  (Note: copying is essential for concurrency 
 * and consistency reasons)
 * 
 * To implement reduce(), you will need to move state back and forth between C++
 * and JavaScript.  The high-level behavior is that the key/value store should 
 * lock each bucket, run filter() on each key/value pair in the bucket, and 
 * whenever filter() returns a non-null value, the result should be added to a
 * vector.  Then the key/value store should execute the provided combine() 
 * function on the vector to produce a single result that gets returned to the 
 * caller.
 * 
 * Finally, it's important for us to specify the protocol to be used for sending 
 * requests to the k/v store over the network, and for receiving results from 
 * the k/v store.  In general, our format will be similar to the HTTP protocol: 
 * we will make use of newlines and lines of text for metadata, and 
 * variable-length content for binary data.  Note: it is acceptable to forbid 
 * newlines within keys.  However, a value may contain arbitrary binary data, to
 * include \0 and \n characters.
 * 
 * put:
 *   - first line: PUT\n
 *   - second line: KEY-LEN: number\n
 *   - third line: the key, ending with \n
 *   - fourth line: VAL-LEN: number\n
 *   - remaining lines: the value (VAL-LEN bytes)
 * 
 * get:
 *   - first line: GET\n
 *   - second line: KEY-LEN: number\n
 *   - third line: the key, ending with \n
 * 
 * del:
 *   - first line: DEL\n
 *   - second line: KEY-LEN: number\n
 *   - third line: the key, ending with \n
 * 
 * where:
 *   - first line: WHERE\n
 *   - second line: REGEX-LEN: number\n
 *   - third line: the regex, ending with \n
 * 
 * reduce:
 *   - first line: REDUCE\n
 *   - second line: FILTER-LEN: number\n
 *   - third line: the filter code, ending with \n
 *   - fourth line: COMBINE-LEN: number\n
 *   - fifth line: the combine code, ending with \n
 * 
 * responses to GET and REDUCE will be of the following form:
 *   - first line: either OK\n or ERROR\n
 *   - second line: RESULT-LEN: number\n
 *   - remaining lines: the result value (RESULT-LEN bytes)
 * 
 * responses to PUT and DEL will be of the following form
 *   - first line: either OK\n or ERROR\n
 * 
 * responses to WHERE will be of the following form:
 *   - first line: either OK\n or ERROR\n
 *   - second line: NUM-RESULTS: number\n
 *   - next line: RESULT-KEY-LEN: number\n
 *   - next line: RESULT-KEY-LEN bytes of key, followed by \n
 *   - next line: RESULT-VAL-LEN: number\n
 *   - next line: RESULT-VAL-LEN bytes of result, followed by \n
 *   - repeat previous four lines as many times as needed for a total of 
 *     NUM-RESULTS results
 * 
 * Note: when ERROR is returned, there should be no further data transmitted in 
 * the response.
 * 
 * You should always check for properly-formed input.  In particular, an 
 * extension of this project might add additional "headers" to a request, so you
 * should not ignore leading text or assume messages will be in the correct 
 * form.
 *
 * Your program should use advanced features of C++ whenever appropriate.  It
 * should compile with g++ version 7.  You should organize your code into files
 * in a way that is appropriate.  You should comment your code.
 * 
 * In addition to the kvserver executable, you should make one (or more) clients
 * that demonstrate the abilities of your kvserver.  Be sure to update the 
 * Makefile accordingly.
 *
 * Turn-in will be via bitbucket.
 */

#include <iostream>
#include <libgen.h>
#include <unistd.h>

#include "hashlist.h"

/** Print some helpful usage information */
void usage(const char *progname) {
  using std::cout;
  cout << "KeyValue Server\n";
  cout << "  Usage: " << progname << " [options]\n";
  cout << "    -p <int> : Port on which to listen (default 41100)\n";
  cout << "    -h       : print this message\n";
}

int main(int argc, char *argv[]) {
  // Config vars that we get via getopt
  int port = 41100;       // random seed
  bool show_help = false; // show usage?

  // Parse the command line options:
  int o;
  while ((o = getopt(argc, argv, "p:h")) != -1) {
    switch (o) {
    case 'h':
      show_help = true;
      break;
    case 'p':
      port = atoi(optarg);
      break;
    default:
      show_help = true;
      break;
    }
  }

  // Print help and exit
  if (show_help) {
    usage(basename(argv[0]));
    exit(0);
  }

  // Print the configuration... this makes results of scripted experiments
  // much easier to parse
  //std::cout << "p = " << port << std::endl;

  // Time for you to start writing code :)

  pNode p;
  pHash_List plist;
 
  plist = init_hash_list();
  insert_node_to_hash(plist,"first","138");
  insert_node_to_hash(plist,"second","1334");
  insert_node_to_hash(plist,"third","137");
    
  print_hash(plist);
    
  delete_node_to_hash(plist,"sdf");
  delete_node_to_hash(plist,"second");
  //delete_node_to_hash(plist,301);
    
  print_hash(plist);
    
  free_all_hash(plist);




}
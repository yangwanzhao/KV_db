#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <queue>
#include <string.h>
#include <chrono>
#include <thread>
#include "hashlist.h"

using namespace boost::asio::ip;
using namespace std;

#define BUF_SIZE 4096

pHash_List plist = init_hash_list();
// plist = init_hash_list();

class Session : public std::enable_shared_from_this<Session> {
public:
  Session(tcp::socket socket) : socket_(std::move(socket)) {}

  void Start() {
    DoRead();
  }
  void DoRead() {
    auto self(shared_from_this());
    socket_.async_read_some(
      boost::asio::buffer(buffer_),
      [this, self](boost::system::error_code ec, std::size_t length) {
        if (!ec) {
          recieve_data(length);
        }
      });

  }

  void DoWrite(std::size_t length) {
    auto self(shared_from_this());
    boost::asio::async_write(
      socket_,
      boost::asio::buffer(buffer_, length),
      [this, self](boost::system::error_code ec, std::size_t length) {
        if (!ec) {
          DoRead();
        }
      });
  }

// function for thread 1 : recieve data from client and add to queue
  void recieve_data(size_t length){
    string command, response, data;
    u32 position;
    stringstream stream;
    for (int i = 0; i < length; ++i)
    {
      stream << buffer_[i];
    }
    data = stream.str();
    // ********** de-protocol ***********
    position = data.find("\n");
    if (position <= data.length())
    {
      command = data.substr(0, position);
      data = data.substr(position+1);
      if (command == "PUT")
      {
        response = insert_node_to_hash(plist, data);
      }
      else if (command == "GET")
      {
        response = get_node_to_hash(plist, data);
      }
      else if (command == "DEL")
      {
        response = delete_node_to_hash(plist, data);
      }
      else if (command == "WHERE")
      {
        response = regexp_node_to_hash(plist, data);
      }

      // ----------------------- JUST FOR TEST -------------
      else if (command == "SHOW")
      {
        print_hash(plist);
        response = "SHOWN IN SERVER";
      }
      else if (command == "INIT")
      {
        init_hash(plist);
        response = "DONE"; 
      }
      // ----------------------- JUST FOR TEST -------------

      else if (command == "REDUCE")
      {
        response = reduce_node_to_hash(plist, data);
      }
      data = response;  
    }
    data = to_string(data.length()) + "\n" + data;
    for (int i = 0; i < data.length(); ++i)
    {
      buffer_[i] = data[i];
    }
    length = data.length();

    DoWrite(length);
  }
  
  

private:
  tcp::socket socket_;
  // vector<unsigned char> buffer_;
  unsigned char buffer_[BUF_SIZE];
};


class Server {
public:
  Server(boost::asio::io_service& ioc, std::uint16_t port)
  : acceptor_(ioc, tcp::endpoint(tcp::v4(), port)) {
    DoAccept();
  }

private:
  void DoAccept() {
  	std::cout << "Waiting for a client to connect..." << std::endl;
    acceptor_.async_accept(
      [this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
         cout << "Connected to " << socket.remote_endpoint().address().to_string() << endl;
         std::make_shared<Session>(std::move(socket))->Start();

       }
       DoAccept();
     }
     );
  }

private:
  tcp::acceptor acceptor_;
};

int main(int argc, char* argv[]) 
{
  using namespace std;
  using std::chrono::duration;
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
	//Get port as size_t from argv
  size_t port = 41100;
	// Parse the command line options:
  int o;
  while ((o = getopt(argc, argv, "p:")) != -1) {
    switch (o) {
      case 'p':
      port = atoi(optarg);
      break;
      default:
      break;
    }
  }
  
  boost::asio::io_service ioc;
  Server server(ioc, port);
  ioc.run();

  return 0;
}
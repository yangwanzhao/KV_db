#include <iostream>
#include <libgen.h>
#include <unistd.h>
#include <boost/asio.hpp>

using namespace std;

#define BUF_SIZE 4096
/**
 * Connect to a server so that we can have bidirectional communication on the 
 * socket (represented by a file descriptor) that this function returns
 *
 * @param hostname The name of the server (ip or DNS) to connect to 
 * @param port the server's port that we should use
 */

boost::asio::ip::tcp::socket connect_to_server(std::string hostname,
 std::string port) {
  using namespace boost::asio;
  io_service io_service;
  ip::tcp::resolver resolver(io_service);
  ip::tcp::resolver::query query(hostname, port);
  ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
  ip::tcp::socket socket(io_service);
  connect(socket, endpoint_iterator);
  return socket;
}

/**
 * Receive text from stdin, send it to the server, and then print whatever the
 * server sends back.
 * 
 * @param socket The socket file descriptor to use for the echo operation
 */
void echo_client(boost::asio::ip::tcp::socket &socket){
  using namespace std;
  using namespace boost::asio;

  // track connection duration, bytes transmitted
  size_t xmitBytes = 0;
  struct timeval start_time, end_time;
  gettimeofday(&start_time, nullptr);

  while(true){
    // Get the data from stdin, This assumes that we haven't redirected stdin
    // to a socket
    cout<<"Client: ";
    string data;
    getline(cin, data);
    if (cin.eof()){
      break;
    } 
    string key,value,str_length_key,str_length_value;
    int length_key,length_value;

    // ******* PUT *******
    if (data == "PUT")
    {
      cout << "KEY-LEN:";
      getline(cin, str_length_key);
      length_key = stoi(str_length_key);
      cout << "KEY:";
      getline(cin, key);
      if(key.length() != length_key)
      {
        cout << "Length error\n" << endl;
        continue;
      }
      cout << "VAL-LEN:";
      getline(cin, str_length_value);
      length_value = stoi(str_length_value);
      cout << "VAL:";
      getline(cin, value);
      if(value.length() != length_value)
      {
        cout << "Length error\n" << endl;
        continue;
      }

      // protocol
      // PUT\nlength_key\nkey\nlength_value\nvalue
      data = "PUT\n" + str_length_key + "\n" + key + "\n" + str_length_value + "\n" + value;
    }

    // ******* GET *******
    else if (data == "GET")
    {
      cout << "KEY-LEN:";
      getline(cin, str_length_key);
      length_key = stoi(str_length_key);
      cout << "KEY:";
      getline(cin, key);
      if(key.length() != length_key)
      {
        cout << "Length error\n" << endl;
        continue;
      }

      // protocol
      // GET\nlength_key\nkey
      data = "GET\n" + str_length_key + "\n" + key;
    }

    // ******* DEL *******
    else if (data == "DEL")
    {
      cout << "KEY-LEN:";
      getline(cin, str_length_key);
      length_key = stoi(str_length_key);
      cout << "KEY:";
      getline(cin, key);
      if(key.length() != length_key)
      {
        cout << "Length error\n" << endl;
        continue;
      }
      // protocol
      // DEL\nlength_key\nkey
      data = "DEL\n" + str_length_key + "\n" + key;
    }

    // ******* SHOW *******
    else if (data == "SHOW")
    {
      data = "SHOW\n";
    }

    // ******* WHERE *******
    else if (data == "WHERE")
    {
      cout << "REGEX-LEN:";
      getline(cin, str_length_key);
      length_key = stoi(str_length_key);
      cout << "REGEX:";
      getline(cin, key);
      if(key.length() != length_key)
      {
        cout << "Length error\n" << endl;
        continue;
      }
      // protocol
      // WHERE\nlength_key\nkey
      data = "WHERE\n" + str_length_key + "\n" + key;
    }

    // ******* REDUCE *******
    else if (data == "REDUCE")
    {
      /* code */
    }

    // ******* EXIT *******
    else if (data == "EXIT")
    {
      break;
    }



    //ready to recieve data from server
    size_t recd = 0, position;
    unsigned char buf[BUF_SIZE];
    // vector<unsigned char> buf;
    size_t len;
    stringstream stream;
    string str_buf;
    boost::system::error_code ignored_error;
    write(socket, buffer(data), ignored_error);
    xmitBytes += data.length();
    
    cout << "Server:" << endl;
    str_buf = "";
    while (true)
    {
      len = socket.read_some(boost::asio::buffer(buf), ignored_error);

      for (int i = 0; i < len; ++i)
      {
        stream << buf[i];
      }

      str_buf = stream.str();
      position = str_buf.find("\n");
      if ( position < str_buf.length() )
      {
        recd = stoi(str_buf.substr(0, position));
        break;
      }
    }

    stream.str("");

    while(str_buf.length()<recd+1+to_string(recd).length())
    {
      len = socket.read_some(boost::asio::buffer(buf), ignored_error);
      for (int i = 0; i < len; ++i)
      {
        stream << buf[i];
      }
      str_buf = str_buf + stream.str();
      stream.str("");
    }

    position = str_buf.find("\n");
    str_buf = str_buf.substr(position+1);

    cout << str_buf << endl;
  }
  gettimeofday(&end_time, nullptr);
  cout << endl
  << "Transmitted " << xmitBytes << " bytes in "
  << (end_time.tv_sec - start_time.tv_sec) << " seconds" << endl;

}


/** Print some helpful usage information */
void usage(const char *progname) {
  using std::cout;
  cout << "  Usage: " << progname << " [options]\n";
  cout << "    -p <int> : Port on which to listen (default 41100)\n";
  cout << "    -h       : print this message\n";
}

int main(int argc, char *argv[]) {
  // Config vars that we get via getopt
  string port = "41100";       // random seed
  bool show_help = false; // show usage?
  string server_name = "localhost";
  // Parse the command line options:
  int o;
  while ((o = getopt(argc, argv, "p:h")) != -1) {
    switch (o) {
      case 'h':
      show_help = true;
      break;
      case 'p':
      port = string(optarg);
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

  // Time for you to start writing code :)
  
  //client
  try {
    // Set up the client socket
    auto socket = connect_to_server(server_name, port);
    cout << "...Connected" << endl;
    cout << endl;
    //Start the 'echo' interaction
    echo_client(socket);

    // clean up when done
    socket.close();
  }catch (std::exception &e){
    std::cerr << e.what() << std::endl;
  }
  return 0;


}



#include <string>
#include <iostream>
#include <ostream>
#include <fstream>
#include <regex>
#include "tclap/CmdLine.h"
#include "curl_easy.h"
#include "curl_exception.h"
#include "curl_sender.h"
#include "curl_receiver.h"

using namespace std;
using namespace TCLAP;
using namespace curl;

string enc(const string &plaintext) {
  return plaintext; // TODO
}

string build_post(const string &host, const string &request) {
  ostringstream body;
  body << "{\"request\": \"" << request << "\"}";

  ostringstream out;
  out <<
    "POST / HTTP/1.1\r\n"
    "Host: " << host << "\r\n"
    "Content-Length: " << body.str().length() << "\r\n"
    "Content-Type: application/json\r\n"
    "Cache-Control: no-cache\r\n\r\n"
      << body.str();

  return out.str();
}

string basename(const string &fname) {
  size_t pos = fname.find_last_of('.');
  return fname.substr(0, pos);
}

string url_encode(const string &raw) {
  ostringstream escaped;
  escaped.fill('0');
  escaped << hex;

  for (string::const_iterator i = raw.begin(), n = raw.end(); i != n; ++i) {
    string::value_type c = (*i);

    // Keep alphanumeric and other accepted characters intact
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      escaped << c;
      continue;
    }

    // Any other characters are percent-encoded
    escaped << uppercase;
    escaped << '%' << setw(2) << int((unsigned char) c);
    escaped << nouppercase;
  }

  return escaped.str();
}

int main(int argc, const char * const argv[]) {
  string verb, target, contents;
  try {
    CmdLine cmd("Securely post/delete blog article to/from the blog system.");
    ValueArg<string> postArg("p", "post", "Post a file to blog system",
                             true, "", "file");
    ValueArg<string> deleteArg("d", "delete", "Delete a blog from blog system",
                               true, "", "article-title");
    cmd.xorAdd(postArg, deleteArg);
    cmd.parse(argc, argv);

    if (postArg.isSet()) {
      verb = "post";
      target = postArg.getValue();
      try {
        ifstream in(target);
        if (!in.is_open()) {
          cerr << "error: can not open file " << target << endl;
          return 1;
        }
        contents = string(istreambuf_iterator<char>(in),
                          istreambuf_iterator<char>());
        contents = url_encode(contents);
        target = basename(target);
      } catch (ifstream::failure e) {
        cerr << "error: " << e.what() << endl;
        return 1;
      }
    } else if (deleteArg.isSet()) {
      verb = "delete";
      target = deleteArg.getValue();
    } else {
      verb = "nonsense";
    }
  } catch (ArgException &e) {
    cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
  }

  cout << verb << " " << target << " ..." << endl;

  string host = "localhost:3000";
  ostringstream request;
  request << verb << "+" << target << "+" << time(nullptr);
  if (!contents.empty()) { request << "+" << contents;  }

  curl_easy easy;
  try {
    easy.add<CURLOPT_URL>(host.c_str());
    easy.add<CURLOPT_CONNECT_ONLY>(true);
    easy.perform();

    curl_sender<string> sender(easy);
    sender.send(build_post(host, enc(request.str())));
  } catch (curl_easy_exception error) {
    curlcpp_traceback errors = error.get_traceback();
    error.print_traceback();
  }

  curl_receiver<char, 1024> receiver;
  for (;;) {
    receiver.receive(easy);
    int count = receiver.get_received_bytes();
    if (count) break;
  }
  array<char,1024> buffer = receiver.get_buffer();
  int size = 1;
  for (auto p = buffer.begin(); *p != 0; ++p) size++;
  string response(buffer.begin(), buffer.begin() + size);

  regex bodyPattern { "\r\n\r\n(.*)" };
  smatch match;
  if (regex_search(response, match, bodyPattern)) {
    cout << match[1] << endl; // TODO parse response
  }
}

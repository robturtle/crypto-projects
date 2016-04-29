#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "tclap/CmdLine.h"
#include "curlpp/cURLpp.hpp"
#include "curlpp/Easy.hpp"
#include "curlpp/Options.hpp"
using namespace std;
using namespace TCLAP;
using namespace cURLpp;
using namespace cURLpp::Options;

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


static string url { "localhost:3000" };

int main(int argc, const char * const argv[]) {
  string verb, target, contents;
  bool debug;
  try {
    CmdLine cmd("Securely post/delete blog articles to/from the blog system");
    ValueArg<string> postArg("p", "post", "Post a file to blog system",
                             true, "", "file");
    ValueArg<string> deleteArg("d", "delete", "Delete a blog from blog system",
                               true, "", "article-title");
    SwitchArg debugArg("", "debug", "Print HTTP link debug info", false);
    cmd.xorAdd(postArg, deleteArg);
    cmd.add(debugArg);
    cmd.parse(argc, argv);
    debug = debugArg.getValue();

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

  ostringstream cmd;
  cmd << verb << "+" << target << "+" << time(nullptr);
  if (!contents.empty()) {
    cmd << "+" << contents;
  } // TODO else forge fake contents to make delete command indistinguishable

  string body {
    (ostringstream{} << "{\"request\": \"" << cmd.str() << "\"}").str()
  };

  ostringstream response;
  try {
    Cleanup cleanup;
    Easy request;

    list<string> headers {
      "Content-Type: application/json"
    , "Cache-Control: no-cache"
    , (ostringstream{} << "Content-Length: " << body.length()).str()
    };

    request.setOpt<Url>(url);
    request.setOpt<HttpHeader>(headers);
    request.setOpt<Post>(true);
    request.setOpt<PostFields>(body);
    if (debug) {
      request.setOpt<Verbose>(true);
    }
    request.setOpt<WriteStream>(&response);
    request.perform();
    cout << response.str() << endl;
  } catch (RuntimeError &e) {
    cout << e.what() << endl;
  } catch (LogicError &e) {
    cout << e.what() << endl;
  }

  return 0;
}

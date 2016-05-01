#include <vector>
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

/********************************************************************************
 * Utils
 ********************************************************************************/
namespace utils {
  vector<string> basenameAndExt(const string &fname) {
    size_t pos = fname.find_last_of('.');
    return vector<string> {
      fname.substr(0, pos),
      fname.substr(pos + 1, fname.length())
    };
  }

  string urlEncode(const string &raw) {
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

} /* ns utils */

namespace {
  string url { "https://localhost:3000" };
  string verb, target, contents, extension;
  string username, password;
  bool isPrivate = false;
  bool debug = false;
}

void parseArgs(int argc, const char * const argv[]) {
  try {
    CmdLine cmd("Securely post/delete blog articles to/from the blog system");
    ValueArg<string> postArg("p", "post", "Post a file to blog system",
                             true, "", "file");
    ValueArg<string> deleteArg("d", "delete", "Delete a blog from blog system",
                               true, "", "article-title");
    SwitchArg privateArg("", "private", "Make the post only be seen by yourself", false);
    SwitchArg debugArg("", "debug", "Print HTTP link debug info", false);
    SwitchArg localArg("", "localhost", "Test on localhost", false);
    cmd.xorAdd(postArg, deleteArg);
    cmd.add(privateArg);
    cmd.add(debugArg);
    cmd.add(localArg);
    cmd.parse(argc, argv);
    debug = debugArg.getValue();
    if (localArg.getValue()) {
      url = "https://localhost:3000";
    } else {
      url = "https://westudy.online";
    }

    if (postArg.isSet()) {
      verb = "post";
      target = postArg.getValue();
      try {
        ifstream in(target);
        if (!in.is_open()) {
          cerr << "error: can not open file " << target << endl;
          exit(1);
        }
        contents = string(istreambuf_iterator<char>(in),
                          istreambuf_iterator<char>());
        vector<string> splits = utils::basenameAndExt(target);
        target = splits[0];
        extension = splits[1];
      } catch (ifstream::failure e) {
        cerr << "error: " << e.what() << endl;
        exit(1);
      }
    } else if (deleteArg.isSet()) {
      verb = "delete";
      target = deleteArg.getValue();
      // TODO contents = forge some trash contents
    } else {
      verb = "nonsense";
    }

  } catch (ArgException &e) {
    cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
  }
}

void readUserInfo() {
  // TODO
  username = "Yang Liu";
  password = "5f84AADA";
}

string constructRequest() {
  string d { "-" };
  string req = (ostringstream{}
             << verb
          <<d<< target
          <<d<< time(nullptr)
          <<d<< username
          <<d<< isPrivate
          <<d<< extension
          <<d<< contents).str();
  return "request=" + utils::urlEncode(req);
}

void sendRequest(const string &body) {
  ostringstream response;
  try {
    Cleanup cleanup;
    Easy request;

    list<string> headers {
      "Content-Type: application/x-www-form-urlencoded"
        , "Cache-Control: no-cache"
        , (ostringstream{} << "Content-Length: " << body.length()).str()
        };

    request.setOpt<Url>(url);
    request.setOpt<HttpHeader>(headers);
    request.setOpt<Post>(true);
    request.setOpt<PostFields>(body);
    request.setOpt<Verbose>(debug);
    request.setOpt<WriteStream>(&response);
    request.perform();
    cout << response.str() << endl;

  } catch (RuntimeError &e) {
    cout << e.what() << endl;

  } catch (LogicError &e) {
    cout << e.what() << endl;
  }
}

void authUser() {
  // TODO if privkey found, use it
  // TODO otherwise:
  readUserInfo();
  // TODO send auth request to server
}

int main(int argc, const char * const argv[]) {
  parseArgs(argc, argv);
  authUser();
  string body = constructRequest();
  sendRequest(body);
}

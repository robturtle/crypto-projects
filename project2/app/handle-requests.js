/* jslint esversion: 6 */
const Blog = require('../models/blog');
const User = require('../models/user');
const Key = require('../models/key');
const exec = require('child_process').exec;

module.exports = function(req, res) {
  var username;
  if (req.body.request) {
    const ciphertext = decodeURIComponent(req.body.request);
    username = decodeURIComponent(req.body.username);
    dec(ciphertext, username, res, request => {
      // TODO check timestamp
      console.log(`received request: ${request.verb}`);
      handleRequest(request, res);
    });
  }

  else if (req.body.password) {
    const password = decodeURIComponent(req.body.password);
    username = decodeURIComponent(req.body.username);
    User.findOne({ 'username': username }, (err, user) => {
      if (err) throw err;
      if (!user) return res.status(400).send('Authentication failed.');
      if (user.validPassword(password)) {
        Key.findOneAndUpdate({
          'username': username
        }, {
          'username': username,
          'key': decodeURIComponent(req.body.key),
          'created_at': new Date()
        }, { upsert: true }, (err, doc) => {
          if (err) throw err;
          return res.send('Authenticated');
        });
      } else {
        return res.status(400).send('Authentication faied.');
      }
    });
  }

};

function dec(ciphertext, username, res, callback) {
  Key.findOne({ 'username': username }, (err, doc) => {
    if (err) throw err;
    if (!doc) return res.status(400).send('Unauthorized request');
    const key = doc.key;
    exec(`echo "${ciphertext}\n${key}" | ./bin/dec`,
         (err, stdout, stderr) => {
           const plaintext = stdout;
           request = parseRequest(plaintext);
           callback(request);
         });
    doc.remove();
  });
}

function parseRequest(plain) {
  const delim = ';';
  params = plain.split(delim);
  var request = {
    verb: params[0],
    target: params[1],
    timestamp: params[2],
    username: params[3],
    public: params[4],
    extension: params[5],
    contents: params.slice(6).join(delim)
  };
  return request;
}

function handleRequest(request, res) {
  const verb = request.verb;
  if (verb === 'post') {
    if (!request.contents.length) {
      return res.status(400).send('error: empty blog contents');
    } else {
      request.rendered = render(request.contents, request.extension);
      if (!request.rendered) {
        return res.status(400).send('error: unsupported file format');
      }
    }
    Blog.findOneAndUpdate({
      'title': request.target,
      'author': request.username
    }, {
      'title': request.target,
      'author': request.username,
      'contents': request.contents,
      'public': request.public,
      'extension': request.extension,
      'shownTitle': toSpaceSeparatedString(request.target),
      'renderedContents': request.rendered
    }, { upsert: true }, (err, doc) => {
      if (err) throw err;
      return res.send('OK');
    });
  }

  else if (verb === 'delete') {
    Blog.find({
      'title': request.target,
      'author': request.username
    }, (err, docs) => {
      if (err) throw err;
      if (!docs.length) {
        return res.send('failed: post not found');
      } else {
        var post = docs[0];
        post.remove((err) => {
          if (err) throw err;
          return res.send('OK');
        });
      }
    });
  }

  else {
    res.status(400).send('error: bad request');
  }
}

function render(contents, extension) {
  if (extension === 'html') {
    return contents;
  } else if (extension === 'md') {
    // TODO
  }
}

function toSpaceSeparatedString(instr) {
  instr = instr.replace(/^[^a-zA-Z]*/, '');
  var outstr = instr.replace(/[-_]?([A-Za-z]+)/g, function(str) {
    str = str.replace(/[-_]/, '');
    return ' ' + str.toLowerCase();
  });
  return outstr.charAt(1).toUpperCase() + outstr.slice(2);
}

/* jslint esversion: 6 */
const Blog = require('../models/blog');

module.exports = function(req, res) {
  const postField = req.body.request;
  if (!postField) res.send('error: empty request');

  const ciphertext = decodeURIComponent(postField);
  const request = parseRequest(ciphertext);

  // TODO check timestamp

  console.log(`received request: ${request.verb}`);
  handleRequest(request, res);

};

function dec(ciphertext) {
  // TODO dec
  return ciphertext;
}

function parseRequest(ciphertext) {
  const delim = '-';
  const plain = dec(ciphertext);
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

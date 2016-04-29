/* jslint esversion: 6 */
String.prototype.replaceAll = function(search, replacement) {
  var target = this;
  return target.replace(new RegExp(search, 'g'), replacement);
};

/********************************************************************************
 * Basic settings
 ********************************************************************************/
const mongoose = require('mongoose');
const bodyParser = require('body-parser');
const express = require('express');
const http = require('http');
const https = require('https');
const forceSSL = require('express-force-ssl');

const app = express();
app.use(bodyParser.urlencoded({ extended: false }));
app.set('forceSSLOptions', {
  enable301Redirects: true,
  trustXFPHeader: false,
  httpsPort: 443,
  sslRequiredMessage: 'SSL Required.'
});
app.use(forceSSL);
const httpsOptions = require('./config/certificates');
http.createServer(app).listen(8080);
console.log(`http listening on 8080`);
https.createServer(httpsOptions, app).listen(3000);
console.log(`SSL listening on 3000`);

const Blog = mongoose.model('Blog', {
  title: { type: String, unique: true },
  contents: String
});
// TODO move back to local DB
mongoose.connect('mongodb://crypto:AU2J5MekN9ze@ds019101.mlab.com:19101/playground');
var db = mongoose.connection;
db.on('error', console.error.bind(console, 'connection error:'));
db.once('open', () => console.log('connected to mongodb'));

/********************************************************************************
 * Server route settings
 ********************************************************************************/
app.get('/', (request, response) => {
  Blog.find((err, blogs) => {
    if (err) return console.error(err);
    response.send(buildHomepage(blogs));
  });
});

app.post('/', (request, response) => {
  var ciphertext = request.body.request;
  if (!ciphertext) return response.send('failed: empty request');
  handleCiphertext(ciphertext, response);
});

app.get('/signup', (request, response) => {
  response.sendFile(`${__dirname}/signup.html`);
});

app.post('/signup', (request, response) => {
  response.send('OK'); // TODO redirect to index
});

/********************************************************************************
 * Helper functions
 ********************************************************************************/
function buildHomepage(blogs) {
  // TODO use template system
  var content = '';
  content += "<h1>Posts</h1>";
  content += '<ul id="blogs">';
  blogs.forEach(blog => {
    content += `<li><strong>${blog.title}</strong><div>${blog.contents}</div></li>`;
  });
  content += '</ul>';
  return content;
}

// request format: verb+target+timestamp[+contents]
//                 note that contents are url encoded
// TODO remove url encode => do base64 on all request string instead
// examples:
//  post+blog title+1461638293+This is blog contents
//  delete+blog title+1461638749
function handleCiphertext(ciphertext, response) {
  // TODO send to decryptor, and receive output
  // TODO check integrity, if failed, response.send('integrity check failed')
  // TODO check timestamp, if too far from now, response.send('stall timestamp')
  // TODO decrypt, if illegal content, response.send('bad request')
  //               else do jobs according to request and response.send('success')
  var plaintext = ciphertext;

  // request accepted, execute normal functions
  params = plaintext.split('-');
  verb      = params[0];
  target    = params[1];
  timestamp = params[2];
  console.log('received data:', verb, target, timestamp);
  if (verb === "post") {
    var contents = params.slice(3).join('+');
    contents = decodeURIComponent(contents).replaceAll("\n", "\n<br/>");
    if (!contents.length) {
      return response.status(400).send("post failed: contents is empty");
    }
    Blog.findOneAndUpdate({'title': target},
                          {'title': target, 'contents': contents},
                          {upsert: true},
                          (err, doc) => {
                            if (err) {
                              console.log(err);
                              return response.status(500).send(err);
                            } else {
                              console.log(`saving blog ${target} (${contents.slice(0,10)}...)`);
                              return response.send('OK');
                            }
                          }
                         );
  } else if (verb == "delete") {
    Blog.find({ 'title': target }, (err, docs) => {
      if (err) return response.status(500).send(err);
      if (!docs.length) {
        return response.send("delete failed: post not found");
      } else {
        docs[0].remove((err) => {
          if (err) return response.status(500).send(err);
          else return response.send('OK');
        });
      }
    });
  } else {
    response.send('bad request');
  }
}

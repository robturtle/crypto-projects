/* jslint esversion: 6 */
/********************************************************************************
 * Basic settings
 ********************************************************************************/
const mongoose = require('mongoose');
const bodyParser = require('body-parser');
const express = require('express');

const app = express();
app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json());

const Blog = mongoose.model('Blog', { title: String, contents: String });
mongoose.connect('mongodb://crypto:AU2J5MekN9ze@ds019101.mlab.com:19101/playground');
var db = mongoose.connection;
db.on('error', console.error.bind(console, 'connection error:'));
db.once('open', () => console.log('connected to mongodb'));

const port = 3000; // TODO change to 80 when production
app.listen(port, () => console.log(`listening on ${port}`));

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
  console.log(`received request: ${ciphertext}`);
  handleCiphertext(ciphertext, response);
});

/********************************************************************************
 * Helper functions
 ********************************************************************************/
function buildHomepage(blogs) {
  var content = '';
  content += '<ul>';
  blogs.forEach(blog => {
    content += `<li><strong>${blog.title}</strong><div>${blog.contents}</div></li>`;
  });
  content += '</ul>';
  return content;
}

function handleCiphertext(ciphertext, response) {
  // TODO send to decryptor, and receive output
  // TODO check integrity, if failed, response.send('integrity check failed')
  // TODO check timestamp, if too far from now, response.send('stall timestamp')
  // TODO decrypt, if illegal content, response.send('bad request')
  //               else do jobs according to request and response.send('success')
  response.send('post response');
}

/* jslint esversion: 6 */
const mongoose = require('mongoose');

const Blog = mongoose.model('Blog', { title: String, contents: String });

mongoose.connect('mongodb://localhost/blog');
var db = mongoose.connection;
db.on('error', console.error.bind(console, 'connection error:'));
db.once('open', function() { console.log('connected to mongodb'); });

const blogs = [
  {
    title: 'Hello, blog',
    contents: 'Good, better, best. Never let it rest.'
  },
  {
    title: 'What a wonderful day',
    contents: 'You can\'t cross the sea merely by standing and starting at the water.'
  }
];

blogs.forEach(function(obj) {
  new Blog(obj).save(function(err, blog) {
    console.log('saving blog ' + blog.title);
  });
});

function checkToQuit() {
  Blog.count(function(err, count) {
    if (count >= blogs.length) {
      console.log('finished.');
      db.close();
    } else {
      setTimeout(checkToQuit, 1000);
    }
  });
}
checkToQuit();

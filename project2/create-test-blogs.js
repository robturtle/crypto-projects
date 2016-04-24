/* jslint esversion: 6 */
const mongoose = require('mongoose');

const Blog = mongoose.model('Blog', { title: String, contents: String });

mongoose.connect('mongodb://crypto:AU2J5MekN9ze@ds019101.mlab.com:19101/playground');
var db = mongoose.connection;
db.on('error', console.error.bind(console, 'connection error:'));
db.once('open', function() { console.log('connected to mongodb'); });

Blog.count((err, count) => {
  if (count > 0) {
    console.log('test data already existed, abort.');
    db.close();

  } else {

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

    blogs.forEach(obj => {
      new Blog(obj).save(function(err, blog) {
        if (err) {
          db.close();
          return console.error(err);
        }
        console.log('saving blog ' + blog.title);
      });
    });

    const checkToQuit = () => {
      Blog.count(function(err, count) {
        if (count >= blogs.length) {
          console.log('finished.');
          db.close();
        } else {
          setTimeout(checkToQuit, 1000);
        }
      });
    };
    checkToQuit();

  }
});

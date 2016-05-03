/* jslint esversion: 6 */
const Blog = require('../models/blog');
const handleRequests = require('./handle-requests');
const genDigest = require('./genDigest');

module.exports = function(app, passport) {

  app.post('/', handleRequests);

  app.get('/', (req, res) => {
    Blog.find((err, blogs) => {
      genDigest(blogs);
      if (err) throw err;
      res.render('pages/index.ejs', { blogs: blogs });
    });
  });

  app.get('/login', (req, res) => res.render('pages/login.ejs', {
    message: req.flash('loginMessage')
  }));

  app.post('/login', passport.authenticate('local-login', {
    successRedirect: '/profile',
    failureRedirect: '/login',
    failureFlash: true
  }));

  app.get('/signup', (req, res) => res.render('pages/signup.ejs', {
    message: req.flash('signupMessage')
  }));

  app.post('/signup', passport.authenticate('local-signup', {
    successRedirect: '/profile',
    failureRedirect: '/signup',
    failureFlash: true
  }));

  app.get('/logout', (req, res) => {
    req.logout();
    res.redirect('/');
  });

  app.get('/profile', isLoggedIn, (req, res) => res.render('pages/profile.ejs'));
  app.get('/about', (req, res) => res.render('pages/about.ejs'));

  function isLoggedIn(req, res, next) {
    if (req.isAuthenticated()) { return next(); }
    res.redirect('/');
  }

};

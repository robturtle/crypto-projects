/* jslint esversion: 6 */
module.exports = function(app, passport) {

  app.get('/', (req, res) => res.render('pages/index.ejs'));
  app.get('/login', (req, res) => res.render('pages/login.ejs', {
    message: req.flash('loginMessage')
  }));
  app.get('/signup', (req, res) => res.render('pages/signup.ejs', {
    message: req.flash('signupMessage')
  }));
  app.get('/profile', isLoggedIn, (req, res) => res.render('pages/profile.ejs', {
    user: req.user
  }));

  function isLoggedIn(req, res, next) {
    if (req.isAuthenticated()) { return next(); }
    res.redirect('/');
  }

};

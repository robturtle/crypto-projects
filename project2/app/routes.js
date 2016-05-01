/* jslint esversion: 6 */
module.exports = function(app, passport) {

  app.get('/', (req, res) => res.render('pages/index.ejs'));

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

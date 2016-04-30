/* jslint esversion: 6 */
const LocalStrategy = require('passport-local').Strategy;
const User = require('../models/user');
const configLocal = {
  usernameField: 'username',
  passwordField: 'password',
  passReqToCallback: true
};

module.exports = function(passport) {
  passport.serializeUser((user, done) => done(null, user.id));
  passport.deserializeUser((id, done) => {
    User.findById(id, (err, user) => done(err, user));
  });

  passport.use('local-signup',
               new LocalStrategy(configLocal, (req, username, password, done) => {
                 process.nextTick(() => {
                   User.findOne({ 'username': username }, (err, user) => {
                     if (err) return done(err);
                     if (user) {
                       return done(null, false, req.flash('signupMessage', 'Username already existed.'));
                     } else {
                       var newUser = new User();
                       newUser.username = username;
                       newUser.password = newUser.generateHash(password);
                       newUser.save((err) => {
                         if (err) throw err;
                         return done(null, newUser);
                       });
                     }
                   });
                 });
               }));

  passport.use('local-login',
               new LocalStrategy(configLocal, (req, username, password, done) => {
                 User.findOne({ 'username': username }, (err, user) => {
                   if (err) return done(err);
                   if (!user) return done(null, false, req.flash('loginMessage', 'User does not exists.'));
                   if (!user.validPassword(password)) {
                     return done(null, false, req.flash('loginMessage', 'Wrong password'));
                   }
                   return done(null, user);
                 });
               }));
};

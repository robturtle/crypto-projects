/* jslint esversion: 6 */
const mongoose = require('mongoose');
const bcrypt = require('bcrypt-nodejs');

const UserSchema = mongoose.Schema({
  username: { type: String, index: true, unique: true },
  password: String,
  facebook: {
    id: String,
    token: String,
    email: String,
    name: String
  },
  twitter: {
    id: String,
    token: String,
    displayName: String,
    username: String
  },
  google: {
    id: String,
    token: String,
    email: String,
    name: String
  }
});

UserSchema.methods.generateHash = password =>
  bcrypt.hashSync(password, bcrypt.genSaltSync(8), null);

UserSchema.methods.validPassword = function(password) {
  return bcrypt.compareSync(password, this.password);
};

module.exports = mongoose.model('User', UserSchema);

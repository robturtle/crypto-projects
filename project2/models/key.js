/* jslint esversion: 6 */
const mongoose = require('mongoose');

const KeySchema = mongoose.Schema({
  username: { type: String, unique: true },
  key: String,
  created_at: { type: Date, expires: 5*60 }
});

module.exports = mongoose.model('Key', KeySchema);

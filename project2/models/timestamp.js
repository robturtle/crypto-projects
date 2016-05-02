/* jslint esversion: 6 */
const mongoose = require('mongoose');

const TimestampSchema = mongoose.Schema({
  username: String,
  timestamp: { type: Date, expires: 5 * 60 }
});

module.exports = mongoose.model('Timestamp', TimestampSchema);

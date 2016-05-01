/* jslint esversion: 6 */
const mongoose = require('mongoose');

const BlogSchema = mongoose.Schema({
  title: { type: String, index: true, unique: true },
  author: String,
  contents: String,
  public: Boolean,
  extension: String,
  renderedContents: String
});

module.exports = mongoose.model('Blog', BlogSchema);

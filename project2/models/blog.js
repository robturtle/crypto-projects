/* jslint esversion: 6 */
const mongoose = require('mongoose');

const BlogSchema = mongoose.Schema({
  title: String,
  author: String,
  contents: String,
  public: Boolean,
  extension: String,
  shownTitle: String,
  renderedContents: String
});

module.exports = mongoose.model('Blog', BlogSchema);

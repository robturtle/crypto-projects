/* jslint esversion: 6 */

const morePattern = /<!--\s*more\s*-->[\s\S]*/;

module.exports = function(blogs) {
  blogs.forEach(function(blog) {
    if (blog.renderedContents.match(morePattern)) {
      blog.more = true;
      blog.digest = blog.renderedContents.replace(morePattern, '');
    }
  });
};

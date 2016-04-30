/* jslint esversion: 6 */

// web server
const express = require('express');
const app = express();
const http = require('http');
const https = require('https');
const httpPort = process.env.HTTP_PORT || 8080;
const httpsPort = process.env.HTTPS_PORT || 3000;
const configHTTPS = require('./config/certificates');

http
  .createServer(app)
  .listen(httpPort, () => console.log(`HTTP listening on ${httpPort}...`));
https
  .createServer(configHTTPS, app)
  .listen(httpsPort, () => console.log(`HTTPS listening on ${httpsPort}...`));

// server utils
const forceSSL = require('express-force-ssl');
app.set('forceSSLOptions', {
  enable301Redirects: true,
  trustXFPHeader: false,
  httpsPort: httpsPort,
  sslRequiredMessage: 'SSL Required.'
});
app.use(forceSSL);

const cookieParser = require('cookie-parser');
app.use(cookieParser());
const bodyParser = require('body-parser');
app.use(bodyParser.urlencoded({ extended: false }));

const session = require('express-session');
app.use(session({
  secret: require('./config/secret').secret,
  resave: false,
  saveUninitialized: false
}));

// page utils
app.use(express.static(`${__dirname}/public`));
const flash = require('connect-flash');
app.use(flash());
app.set('view engine', 'ejs');
const expressLayouts = require('express-ejs-layouts');
app.set('layout', 'default');
app.use(expressLayouts);

// database
const mongoose = require('mongoose');
const configDB = require('./config/db');
mongoose.connect(configDB.url, () => console.log('database connected...'));

// logging
const morgan = require('morgan');
app.use(morgan('dev'));

// extended feature
//const passport = require('passport'); // other account system
//app.use(passport.initialize());
//app.use(passport.session());

// routes ================================================================================
require('./app/routes.js')(app/*, passport*/);

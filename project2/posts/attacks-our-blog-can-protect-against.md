## Eavesdroppers & MitM attacks

> content-needed: communication-protected-by-HTTPS


<!-- more -->

> content-needed: it-is-confidentiality-integrity-guaranteed


![](https://dl.dropboxusercontent.com/s/y0c3fcd5gwfvwun/Screenshot%202016-05-02%2023.00.07.png)

> content-needed: screenshot of captured packets

![](https://dl.dropboxusercontent.com/s/56ye2oql9hx94wl/Screenshot%202016-05-03%2012.54.03.png?dl=0)

## Replay attacks

> content-needed: 2 phase policy


```js
function checkTimestamp(timestamp, username, res, next) {
  const now = Date.now() / 1000;
  if (now - timestamp > 5 * 60 || now < timestamp) {
    return res.status(400).send('expired timestamp');
  } else {
    Timestamp.findOne({ 'username': username, 'timestamp': timestamp }, (err, doc) => {
      if (err) throw err;
      if (doc) {
        return res.status(400).send('reject replay attack.');
      } else {
        new Timestamp({ 'username': username, 'timestamp': new Date(timestamp * 1000) }).save();
        next();
      }
    });
  }
}
```

```js
const TimestampSchema = mongoose.Schema({
  username: String,
  timestamp: { type: Date, expires: 5 * 60 }
});
```

> content-needed: reject expired timestamp


![](https://dl.dropboxusercontent.com/s/ik6n9hozf80zpkn/Screenshot%202016-05-02%2023.56.56.png?dl=0)


> content-needed: reject identical timestamp


![](https://dl.dropboxusercontent.com/s/xfnrqg0t6pn173v/reject-identical-timestamp.png?dl=0)

## User authentication
### Password

> content-needed: user-authed-by-password


> content-needed: password-protected-by-hashing


![](https://dl.dropboxusercontent.com/s/js088uy7njx15wc/Screenshot%202016-05-02%2023.15.06.png)


> content-needed: client-link-process-description


![](https://dl.dropboxusercontent.com/s/b3gnymheopmlsg6/client-communications.png)


### RSA (To be implemented)

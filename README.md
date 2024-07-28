This folder contains the code that demonstrates our covert channel. Instructions for usage are below

1. Makefile

To create all binaries run:

bash
```
make all
```

2. Commands to send packets over channel

The server sender talks to the client receiver and vice versa. Both senders have the same input commands. To connect the server sender and client receiver run the following.

In first terminal

bash
```
sudo ./client_rec
```

In second terminal

bash
```
sudo ./server_send whatisup 11
```

whatisup is the message and 11 is the prime set number that the encoding and decoding will use for the first four ASCII characters. The prime set number can be anywhere between 1 and 16, inclusive. Messages can contain all ASCII characters. To use spaces wrap the message in quotation marks (e.g. "what is up").

To connect from client to server, simply switch the names in both commands.

3. What is our code doing?

Both senders obfuscate data in a field that is typically a SHA1 hash. We use a variable amount of bytes of the hash to fill with our data, the rest are random characters. Then the data is pulled from the hash on the other side and decoded. We encode and decode the data using a combination of ASCII values and the Chinese Remainder Theorem. Then packets are sent over TCP, where the sequence number mod 16 relays some data.
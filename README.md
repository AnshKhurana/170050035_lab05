# 170050035_lab05
### | CS 252 | Computer Networks | lab05 | Socket Programming |

Implementation of a simplified version of PoP3 email protocol between a client and a server.

Essentially the email server has access to a password file and a directory structure where messages(emails) of the different users are stored under individual user folders. Each client contacts the server, shares its login credentials which are verified by the server. After successful client login, a client can determine the number of messages it has stored at server and then retrieve a few of these messages. In our simplified version, messages simply correspond to files of different types: txt, pdf, jpg etc

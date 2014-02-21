#!/usr/bin/python
# ring, ring - simple python cgi
# when pinged log a doorbell buzz to file (or do something else!)

import logging,cgitb
cgitb.enable()

logfile="/var/log/doorbell.log"
logging.basicConfig(format='%(asctime)s %(levelname)-7s %(message)s', datefmt='%Y-%m-%d %H:%M:%S', level=logging.INFO, filename=logfile)
logging.info("Doorbell Buzzed")

# print web content
print "Content-type: text/html"
print
print "<html><body><p>Ok</p></body></html>"

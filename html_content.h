char* get_html_template() { return 
"<html>"
"<head></head>"
"<body>"
"<h1>Hey! Your IP address is ^ip^           .</h1>"
"<p>This tool is mainly meant to be used from the command line.</p>"
"<p>To try it out, you can do <b>my_ip=$(curl gimmeip.org) && echo $my_ip</b>.</p>"
"</body>"
"</html>"
;}

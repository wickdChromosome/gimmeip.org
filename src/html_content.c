char* get_html_template() { return "\
<html>\
<head>	\
<meta charset='utf-8'>\
<meta name='viewport' content='width=device-width, initial-scale=1.0'>\
<meta http-equiv='X-UA-Compatible' content='IE=edge'>\
\
<title>gimmeip.org</title>\
\
<!-- Ajax -->\
<script src='https://code.jquery.com/jquery-3.4.1.min.js' integrity='sha256-CSXorXvZcTkaix6Yvo6HppcZGetbYMGWSFlBw8HfCJo=' crossorigin='anonymous'> </script>\
\
<!-- Fonts -->\
<link href='http://fonts.googleapis.com/icon?family=Material+Icons' rel='stylesheet'>\
<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/materialize/0.97.8/css/materialize.min.css'>\
<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css'>\
\
<!-- JQuery -->\
<script type='text/javascript' src='https://code.jquery.com/jquery-2.1.1.min.js'></script>    \
\
<!-- Materialize -->\
<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/materialize/0.97.8/css/materialize.min.css'>\
<script src='https://cdnjs.cloudflare.com/ajax/libs/materialize/0.97.8/js/materialize.min.js'></script>\
\
<!-- Typed.js -->\
<script src='https://cdn.jsdelivr.net/npm/typed.js@2.0.11'></script>\
\
<!-- Animate.css -->\
<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/animate.css/3.7.2/animate.min.css'>	\
\
<!-- highlight.js -->\
<script src='https://cdnjs.cloudflare.com/ajax/libs/highlight.js/10.2.0/highlight.min.js'></script>\
<script>hljs.initHighlightingOnLoad();</script>\
\
<!-- Global site tag (gtag.js) - Google Analytics -->\
<script async src='https://www.googletagmanager.com/gtag/js?id=UA-113278907-3'></script>\
<script>\
  window.dataLayer = window.dataLayer || [];\
  function gtag(){dataLayer.push(arguments);}\
  gtag('js', new Date());\
\
  gtag('config', 'UA-113278907-3');\
</script>\
\
\
</head>\
<body style='margin-top:100px;'>\
\
<!-- Welcome text -->\
<span id='welcome_text' align=center style='margin:auto;'></span>\
\
<div style='text-align:center; margin-top:25px;'>\
<div hidden id='post_typing_div' class='animated fadeIn'>\
\
<p>To get your public IP on the terminal(or in scripts), you can do the following in bash:</p> \
\
<pre><code class='bash'>my_ip=$(curl gimmeip.org) && echo $my_ip</code></pre>\
\
<a style='' href='https://github.com/wickdChromosome/gimmeip.org'>Github Page</a>\
\
</div>\
</div>\
\
</body>\
\
<script type='text/javascript'>\
	\
	var typed = new Typed('#welcome_text',{\
		strings: ['<h4>Here is your public IP address: ^ip^           </h4>'],\
		stringsElement: '#typed-strings',\
		showCursor: false,\
		typeSpeed: 10,\
		onComplete: (self) => {\
\
			$('#post_typing_div').show();\
\
		},\
	});\
\
\
</script>\
\
</html>\
";}

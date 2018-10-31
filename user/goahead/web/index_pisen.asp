<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<title><%write(com_utt);%></title>
<script language="JavaScript" type="text/javascript">
var lang = "zhcn";
<%getSysLang();%>
function initLanguage()
{
	var cook = "en";
	var lang_en = "<% getLangBuilt("en"); %>";
	var lang_zhtw = "<% getLangBuilt("zhtw"); %>";
	var lang_zhcn = "<% getLangBuilt("zhcn"); %>";
	/*
	var lang_en =  lang_zhcn =1;
	var lang_zhtw =0;
	*/
	if (document.cookie.length > 0) {
		var s = document.cookie.indexOf("language=");
		var e = document.cookie.indexOf(";", s);
		if (s != -1) {
			if (e == -1)
				cook = document.cookie.substring(s+9);
			else
				cook = document.cookie.substring(s+9, e);
		}
	}

	if (lang == "en") {
		document.cookie="language=en; path=/";
		if (cook != lang)
			window.location.reload();
		if (lang_en != "1") {
			if (lang_zhtw == "1") {
				document.cookie="language=zhtw; path=/";
				window.location.reload();
			}
			else if (lang_zhcn == "1") {
				document.cookie="language=zhcn; path=/";
				window.location.reload();
			}
		}
	}
	else if (lang == "zhtw") {
		document.cookie="language=zhtw; path=/";
		if (cook != lang)
			window.location.reload();
		if (lang_zhtw != "1") {
			if (lang_en == "1") {
				document.cookie="language=en; path=/";
				window.location.reload();
			}
			else if (lang_zhcn == "1") {
				document.cookie="language=zhcn; path=/";
				window.location.reload();
			}
		}
	}
	else if (lang == "zhcn") {
		document.cookie="language=zhcn; path=/";
		if (cook != lang)
			window.location.reload();
		if (lang_zhcn != "1") {
			if (lang_en == "1") {
				document.cookie="language=en; path=/";
				window.location.reload();
			}
			else if (lang_zhtw == "1") {
				document.cookie="language=zhtw; path=/";
				window.location.reload();
			}
		}
	}
}

function onInit()
{
	initLanguage();
	window.location.href="home.asp";
	
}

</script>
</head>
<body onload=onInit()></body>
</html>

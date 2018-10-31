 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("langChange"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<%getSysLang();%>

function initValue() {
	var lang_element = document.getElementById("langSelection");
	var lang_en = "<% getLangBuilt("en"); %>";
	var lang_zhtw = "<% getLangBuilt("zhtw"); %>";
	var lang_zhcn = "<% getLangBuilt("zhcn"); %>";

	if (lang_en == "1")
		lang_element.options[lang_element.length] = new Option('<% write(lc_englist); %>', 'en');
	if (lang_zhtw == "1")
		lang_element.options[lang_element.length] = new Option('<% write(lc_chinesetrad); %>', 'zhtw');
	if (lang_zhcn == "1")
		lang_element.options[lang_element.length] = new Option('<% write(lc_chinesesim); %>', 'zhcn');

	if (document.cookie.length > 0) {
		var s = document.cookie.indexOf("language=");
		var e = document.cookie.indexOf(";", s);
		var i;

		if (s != -1) {
			if (e == -1)
				lang = document.cookie.substring(s+9);
			else
				lang = document.cookie.substring(s+9, e);
		}
		for (i=0; i<lang_element.options.length; i++) {
			if (lang == lang_element.options[i].value) {
				lang_element.options.selectedIndex = i;
				break;
			}
		}
	}
}

function setLanguage()
{
	document.cookie="language="+document.Lang.langSelection.value+"; path=/";
	parent.location.reload();
	return true;
}
//-->
</script>
</head>
<body onLoad="initValue()" background="<% write(com_bodyMain); %>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
 <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<% write(LangChange); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">//showerror(errorstr);</script>
  <tr valign="top">
    <td>
 <form name="Lang" method="post" action="/goform/setSysLang">
 <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7">
	<table border="0" width="600" align='center'>
 <tr><td colspan="3" height="30">&nbsp;</td></tr>
 <tr><td height=30 align=right id=langselect width=39%><% write(LangChange); %></td><td height="30" width=3%>&nbsp;</td>
 <td width="58%" height="30">
<select name="langSelection" id="langSelection">
</select>&nbsp;&nbsp;&nbsp;<input type="submit" id="button1" value="<% write(com_save); %>" onClick="setLanguage()" class="input-button">&nbsp;&nbsp;&nbsp;<input type="button" value="<% write(com_help); %>" class="input-button" onClick="window.open('<% write(com_helppage); %>#langChange')">
</td></tr>
 <tr><td colspan="3" height="20">&nbsp;</td></tr>
</table></td></tr></table>
</form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>

	      

<html> 
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("Notice"); %>
<% langTran("common"); %>
<title><% write(Announce); %></title>
<script language="javascript">
<% getSysLang(); %>
<% aspNoticeReceive(); %>
</script>
<style type=text/css>
body{background:#6BB23A;font-size:13px}
#m{background:#FFF;position:absolute;width:650px;height:320px;margin:75px 60px 75px 40px}
#m #c{padding:20px;}
#m #s{position:absolute;color:#e60000;bottom:10px;right:10px;}
</style>
</head>
<body>
<form id="preview" name="preview" method="post" action="">
<div id="m">
<h3 align=center><% write(Form("noticeTitle"));%></h3>
<% write(Form("hidBdy"));%>
<div>
<br><br>
<script language="javascript">
var skiptimes = "<% write(Form("skiptime"));%>";
var skipurls = "<% write(Form("skipurl"));%>";
if (skiptimes != "")
{
    document.write(skiptimes + '<% write(skip); %><a href = "http://' + skipurls + '">' + skipurls + '</a>');
}
</script>
</div></div>
</form></body></html>


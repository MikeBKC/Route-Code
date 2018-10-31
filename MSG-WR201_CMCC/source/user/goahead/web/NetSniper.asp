<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<title><%write(com_utt);%></title>
<script language="JavaScript" type="text/javascript">
<%aspNetSniperConfig();%>
function onInit()
{
	  if(enable == "on")
			window.location.href="NetSniperList.asp";
		else
			window.location.href="NetSniperConf.asp";
}

</script>
</head>
<body onload=onInit()></body>
</html>


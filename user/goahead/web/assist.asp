<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<link rel=stylesheet type="text/css" href="css/style.css">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
</head>
<body>
<input type="hidden" value="" name="dev_name" id="dev_name">
<input type="hidden" value="" name="login_name" id="login_name">
<script type="text/javascript" src="js/common.js"></script>
<script language="JavaScript" type="text/javascript">
    var cprefix = 'bw_r';
	//交互
	<%
		aspOutAssitVar();
	%>
	//end交互
	if(deviceName=='')
	{
		deviceName="ADMIN";
	} 
	if(loginName=='')
	{
		loginName="admin";
	}
	document.getElementById('dev_name').value=deviceName;
	document.getElementById('login_name').value=loginName;
	cookie.set(cprefix + 'devType', deviceType, 14);//记录芯片类型，方便其他页面使用
</script>
</body>
</html>

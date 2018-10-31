<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<title>赚钱wifi</title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 

<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
</head>
<script type="text/javascript">
<%aspoutPhone();%>
function init()
{
    var host = window.location.host;
    if(isBind)
	salewifi.location.href="http://biz.greenwifi.com.cn/login.jsp?<%getSaleWifiSn();%>&lanIp="+host+"&phone="+phone;
    else
	salewifi.location.href="http://biz.greenwifi.com.cn/register.jsp?<%getSaleWifiSn();%>&lanIp="+host;
}
$(function(){
    init();
});

</script>
<style type="text/css">
ul li {  float:left; margin-left:2px; margin-top:10px; height:29px; padding-top:3px;  padding-left:10px; padding-right:10px; color:#fff; width:120px;cursor:pointer;}
#tabs{width:650px;}
#secondli{background:#fff;color:#0099D2;}
#firstli{background:#0099D2;color:#fff;}
</style>

<body id="salewifi_register" scroll="no">
<div id="tabs" >
    <div style=" height:42px;background-color:#0099D2;" id="tabstitle">
	<ul>
		<li style="margin-left:15px; " id="firstli" onclick="window.parent.open_windows(null,'makeMoney',null)" >配置</li>
		<li id="secondli" onclick="window.parent.open_windows(null,'income',null)" >收益</li>
	    </ul>
    </div>

<div style="width:600px; margin:0 auto; text-align:center;">


	<form action="" method="post" name="income">
    
	<td>
	<table border="0" cellpadding="0" cellspacing="0" id="runstatusInfo" style="background:#5e5e5e;" class="group" width="600px">
	<tr bgcolor="c0ddff">
	      <td colspan="2"><iframe src="" name="salewifi" scrolling="no" align="MIDDLE" width="600" height="250" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
	    </tr>
        </table>
    </td>
    </form>
<br>
<br>
</div>
<br>
</div>
<br>
</body>
</html>

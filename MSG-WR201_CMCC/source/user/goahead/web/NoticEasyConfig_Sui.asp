<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<% langTran("EasyTask"); %>
<title><% aspGetTitle(); %></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js" ></script>
<script type="text/javascript" src="script/jquery.corner.js" ></script>
</head>

<style type="text/css">
.container{ padding-left:30px; padding-bottom:10px;}
.EasyConfhello{ text-indent:2em; padding-top:10px; padding-bottom:10px;}
.EasyConfinfo1{ text-indent:2em;  padding-bottom:25px;}
#enterConfig{ padding-left:230px;}
.tips{ font-size:15px; padding-top:35px; color:#6a6a6a;}
.tips1{font-size:15px;color:#6a6a6a;}
</style>
<script type="text/javascript">
var enabled = 0;
var easyTaskOn = 0;
var ConnStatus = 0;
var connStatusSW ="";
<% aspoutEasyTaskNotice(); %>

function save()
{
    if (document.getElementById("easyTaskNoticeEn").checked == true)
    {
	NoticEasyManaConfig.enable.value = "off";
    }
    else
    {
	NoticEasyManaConfig.enable.value = "on";
    }
    NoticEasyManaConfig.action="/goform/formEasyTaskNoticeEn_Sui";
    NoticEasyManaConfig.submit();
}
function checkConnectStatus(){
    switch(ConnStatus)
    {
	case 0:
	    connStatusSW ="<%write(eas_disconnect);%>";
	    break;
	case 6:
	    connStatusSW ="<%write(eas_userpasserror);%>";
	    break;
	default:
	    connStatusSW ="<%write(eas_connecterror);%>";
    }
    document.getElementById("status").innerHTML = connStatusSW;
}

function pageRedirect(){
    NoticEasyManaConfig.action="/home.asp";
    NoticEasyManaConfig.submit();
}
		
function init(){
	checkConnectStatus();
	//document.getElementById("contentdiv").style.height="220px";
	$("#easyConfig").corner("15px");
	document.getElementById("easyTaskNoticeEn").checked = (enabled ? false:true);
}    
</script>
<body id="easyconfig" onLoad="init();">
<form action="" method="post" name="NoticEasyManaConfig">
<div class="easywrapper" id="easyConfig">
    <div class="container">
        	<div class="EasyConftit"><%write(eas_info1);%></div>
            <div class="EasyConfhello"><%write(eas_info2);%></div>
            <div class="EasyConfinfo1"><%write(eas_info4);%><span id="status" style="color:#FF0000;"></span><%write(eas_info5);%></div>
            <div><input type="checkbox" name="easyTaskNoticeEn" id="easyTaskNoticeEn" onClick="save();" value=""/><%write(eas_notips);%><input type="hidden" name="enable" id="enable"/><span id="enterConfig"><a href="#" title="" onClick="pageRedirect();"><%write(eas_config);%></a></span></div>
            <div class="tips"><%write(eas_notice);%></div>
            <div  class="tips1"><%write(eas_Errwarn);%></div>
        </div>
</div>
</form>
</body>
</html>

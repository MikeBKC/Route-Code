<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<%langTran("common");%>
<%langTran("WebAuth_message");%>
<title>Attention</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<style type="text/css">                                             
<!--                                                                
#content {                                                          
       position: relative;                                         
       height: 100%;                                               
       width: 100%;                                                
       margin:0px;        
       padding:0px;       
}                          
#content div{              
       height: 100%;
       width: 100%; 
       position: absolute;
       z-index: 9;        
       left: 0px;         
       top: 0px;          
}                          
#content img {             
       margin: 0px;       
       position: absolute;
       padding:0px;       
}                          
-->
</style>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
<% aspOutputWebAuthMessage(); %>
<%aspOutputTips();%>
/*
function pic()
{	if(activePic == 1){
    if(en_picture == 1 || en_picture == 3){
	document.getElementById("picdir").style.backgroundImage="url("+picturedir+")";
    }else if(en_picture == 0 || en_picture == 2){
	document.getElementById("picdir").style.backgroundImage="url('"+pictureUrl+"')";  
	}
	}
}
*/
function logoff()
{
	var tbl = webAuthMessage;
	tbl.action="http://"+ip+"/goform/formWebAuthUserSubmit";
//	alert("<%write(msg_logout);%>");
	tbl.submit();
}
function modifypasswd()
{
    modifyweb = "http://" + ip + "/waUsers.asp";
    window.open(modifyweb, '_blank', "");
}
var flag=0;
function ShowCountDown() 
{	
	if(flag == 0)
	{
	leftsecond=eval(leftTime);
	flag = 1;
	}else{
	leftsecond--;
	}
var day=Math.floor(leftsecond/(3600*24));	
var hour=Math.floor((leftsecond-day*3600*24)/3600); 
var minute=Math.floor((leftsecond-hour*3600-day*3600*24)/60); 
var second=Math.floor(leftsecond-day*3600*24-hour*3600-minute*60); 
var cc = document.getElementById("LeftTime1"); 
cc.innerHTML = "<%write(msg_leftTime);%>"+day+"<%write(com_day);%>"+hour+"<%write(com_hour);%>"+minute+"<%write(com_minute);%>"+second+"<%write(com_second);%>"; 
}
if(eval(leftTime) > 0){
setInterval('ShowCountDown()',1000);
}
function init()
{
	if (billing == "1")
	{
	document.getElementById("StopDatas1").style.display ="";
	document.getElementById("StopDatas2").style.display ="none";
	sysDate=new Date(UTCtoLocal(eval(stopData),0) * 1000);
	var aa = document.getElementById("StopDatas1"); 
	aa.innerHTML ="<%write(msg_effectData);%>"+sysDate.getFullYear()+"-"+eval(sysDate.getMonth()+1)+"-"+sysDate.getDate();
		if(leftTime > 0){
	document.getElementById("LeftTime1").style.display ="";
	document.getElementById("LeftTime2").style.display ="none";
		}else{
	document.getElementById("LeftTime1").style.display ="none";
	document.getElementById("LeftTime2").style.display ="";
	var bb = document.getElementById("LeftTime2"); 
	bb.innerHTML ="<%write(msg_leftTime);%><%write(msg_unlimited);%>";
		}
	}else{
	document.getElementById("StopDatas1").style.display ="none";
	document.getElementById("StopDatas2").style.display ="";
	var sd2 = document.getElementById("StopDatas2"); 
	sd2.innerHTML ="<%write(msg_effectData);%><%write(msg_forEver);%>";
	
	}
	
	if(en_picture == 2 || en_picture == 3){
	tipstitles = "";
	tipsinfos = "";
	}

}
</script>
</head>
<body onload="init();" id="content">
<script language="javascript">
if(activePic == 1){
    if((en_picture == 1 || en_picture == 3)&&(picturedir != ""))
    {
	document.write('<img src="'+picturedir+'" width="100%" height="100%" />');
    }else if((en_picture == 0 || en_picture == 2)&&(pictureUrl != "")){
	document.write('<img src="'+pictureUrl+'" width="100%" height="100%" />');
	}else{
	document.write('<a></a>');
	}
}else{
	document.write('<a></a>');
}
</script>
<div>
<form name="webAuthMessage" id="webAuthMessage" method="get" action="">
<input type="hidden" name="userCommand" id="userCommand" value="logoff">
</form>
<table width="100%" align="center"><tr><td>
<table width="380px" align="center" style="border:solid 1px #CCCCCC">
<tr><td height="20">&nbsp;</td></tr>
<tr><td align="center"><p><strong><font  size="+2"><script type='text/javascript'>document.write(tipstitles);</script></font></strong></p></td></tr>
<tr><td align="left"><strong><font  size="+0"><script type='text/javascript'>document.write(tipsinfos);</script></font></strong></td></tr>
<tr><td align="left" id="StopDatas1" style="display"></td></tr>
<tr><td align="left" id="StopDatas2" style="display"></td></tr>
<tr><td align="left" id="LeftTime1" style="display"></td></tr>
<tr><td align="left" id="LeftTime2" style="display"></td></tr>
<tr><td align="left"><strong><font  size="-1">----------------------------------------------</font></strong></td></tr>
<tr><td align="left"><p><strong><font  size="+2"><%write(com_zhuyi);%></font></strong></p></td></tr>
<tr><td align="left"><strong><font  size="-1"><%write(msg_helpOne);%></font></strong></td></tr>
<tr><td align="left"><strong><font  size="-1"><%write(msg_helpTwo);%></font></strong></td></tr>
<tr><td align="left"><strong><font  size="-1"><%write(msg_helpThree);%><script type='text/javascript'>document.write('<a href="http://'+ip+'/WebAuth_message.asp">'+"http://"+ip+"/WebAuth_message.asp"+'</a>');</script><%write(msg_helpFour);%></font></strong></td></tr>
<tr><td height="20">&nbsp;</td></tr>
<tr><td align="right"><input type="button" value="<%write(msg_modifyPasswd);%>" class="input-button" onClick="modifypasswd()">
	<input type="button" value="<%write(msg_disConnect);%>" class="input-btton" onClick="logoff()">
	</td></tr>
<tr><td height="20">&nbsp;</td></tr>
</table>
</td></tr></table>
</div>
</body>
</html>

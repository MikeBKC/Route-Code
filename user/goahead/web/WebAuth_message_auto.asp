<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("WebAuth_message");%>
<title>Attention</title>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<!-- css3-mediaqueries.js for IE8 or older -->
<!--[if lt IE 9]>
	<script src="js/css3-mediaqueries.js"></script>
<![endif]-->
</head>
<style type="text/css">
html,body{ width:100%; height:100%; margin:0; padding:0;/* background-color: #ecf8f8;*/ color:#656565;font-family: Microsoft Yahei, Verdana, Arial, Sans-Serif;}
.header{ width:100%; height:38%; z-index:-999; }
.main{ margin:-3% auto; text-align: center; width:450px; /*border:1px solid #CF3; */z-index:150;background-color: #ecf4f5;}
.tabs{ width:100%; z-index:170;background-color:#ffffff;width:450px; }
.header_tr{ height:50px;}
input[type='text'],input[type='password']{ width:130px; height:15px;}
.button{ padding-bottom:3px; padding-top:4px;text-align: center;font-weight: bold;color: #656565;background-color:#daecee;border: none;border-radius: 5px; width:100px; }
	
@media screen and (max-width: 600px){
.main,.tabs{ width:300px;}

}
</style>
<script type="text/javascript">
var needjump = 0;
<% aspOutputWebAuthMessage(); %>
<%aspOutputTips();%>
//var currentType="0";//当前登录方式0：账号密码登录 1：手机（短信认证）2：微信认证
</script>
<body onLoad="init();" style=" z-index:-1;background-color: #f7f7f7;">

<div class="header">
	<!--<img src="images/5.jpg" width="100%" height="38%" style="position:absolute; z-index:-9999;"/>-->
    <script type="text/javascript">
	//activePic=1;en_picture =0;picturedir="images/5.jpg";pictureUrl="";
    if(activePic == 1){
		if((en_picture == 1 || en_picture == 3)&&(picturedir != ""))
		{
		document.write('<img src="'+picturedir+'" width="100%" width="100%" height="38%" style="position:absolute; z-index:-9999;" />');
		}else if((en_picture == 0 || en_picture == 2)&&(pictureUrl != "")){
		document.write('<img src="'+pictureUrl+'" width="100%" width="100%" height="38%" style="position:absolute; z-index:-9999;" />');
		}else{
		document.write('<a width="100%" height="38%" style="position:absolute; z-index:-9999;">&nbsp;</a>');
		}
	}
    
    </script>
</div>
<div class="main" style="z-index:-9999;">
<form name="webAuthMessage" id="webAuthMessage" method="get" action="">
<input type="hidden" name="userCommand" id="userCommand" value="logoff">

	<table cellpadding="0" cellspacing="0" border="0" class="tabs" style="border: 1px solid;">
    	<tr style="background-color:#ecf4f9" class="header_tr">
        	<td>&nbsp;</td>
        	<td align="center">
            </td>
            <td>&nbsp;</td>
        </tr>
        <tr height="10px">
        	<td colspan="3" id="errormsg" align="center"></td>
        </tr>
         <tr>
        	<td align="right" valign="middle"></td>
            <td align="center" valign="top">
            	<table cellpadding="0" cellspacing="0" border="0" style="display:;" id="tabs_1">
                	<tr height="30px">
                    	<td rowspan="4" align="center" valign="middle"  style="padding-top:40px;" ><img src="images/success.png" width="60px" />&nbsp;</td>
                        <td align="right"></td>
                        <td></td>
                    </tr>
                    <tr height="30px">
                    	
                    	<td colspan="2" align="left">认证成功。</td>
                        
                    </tr>
                    <tr height="30px" id="effTime_tr">
                    	
                    	<td colspan="2" align="left" id="time_td1"></td>
                    </tr>
                    <tr height="30px">
                    	
                    	<td colspan="2" align="left" id="time_td"></td>
                    </tr>
                    <tr height="30px">
                    	<td colspan="3" align="center"></td>
                    </tr>
                </table>
               
            </td>
            <td>&nbsp;</td>
        </tr>
        <tr height="25px">
        	<td colspan="3"><input type="button" value="<%write(msg_modifyPasswd);%>" id="modifypwdbtn" onClick="modifypasswd();" class="button"/>&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(msg_disConnect);%>" onClick="logoff()" class="button"/></td>
        </tr>
        <tr height="25px" style="background-color:#ecf4f9">
        	<td colspan="3">&nbsp;</td>
        </tr>
    </table>
    </form>
</div>
<script type="text/javascript">
function logoff(){
	var tbl = webAuthMessage;
	tbl.action="http://"+ip+"/goform/formWebAuthUserSubmit";
//	alert("<%write(msg_logout);%>");
	tbl.submit();
}
function modifypasswd(){
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
	var cc = document.getElementById("time_td"); 
	cc.innerHTML = "<%write(msg_leftTime);%>"+day+"<%write(com_day);%>"+hour+"<%write(com_hour);%>"+minute+"<%write(com_minute);%>"+second+"<%write(com_second);%>"; 
}
if(eval(leftTime) > 0){
setInterval('ShowCountDown()',1000);
}
function UrlRedirect()
{
    clearInterval(timename); 
    window.location.href = "http://" + jumpUrl;
}
if ((needjump == 1) && (jumpUrl != ""))
{
    timename=setInterval('UrlRedirect()',7000);
}
function UTCtoLocal(time, timezone) {
    date = new Date();
    return time + timezone + date.getTimezoneOffset() * 60;
}
function init(){
	if(currentType == "1" || currentType == "2"){
		document.getElementById("modifypwdbtn").style.display="none";	
	}
	var aa = document.getElementById("time_td");
	var bb = document.getElementById("time_td1");
	//billing = 1;
	//leftTime =24;
	if (billing == "1"){
		var sysDate=new Date(UTCtoLocal(eval(stopData),0) * 1000);
		 
		bb.innerHTML ="<%write(msg_effectData);%>"+sysDate.getFullYear()+"-"+eval(sysDate.getMonth()+1)+"-"+sysDate.getDate();
		if(leftTime <= 0){
			aa.innerHTML ="<%write(msg_leftTime);%><%write(msg_unlimited);%>";	
		}
	}else{
		// document.getElementById("effTime_tr").style.display="none";
		 aa.innerHTML ="<%write(msg_effectData);%><%write(msg_forEver);%>";
	}	
}
</script>
</body>
</html>

<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("apMailConfig");%>
<%langTran("commonWords");%>
<% langTran("JS"); %>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/config.js" type="text/javascript"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" ContentType="text/xml">

var errorstr = "";

<%aspoutErrorMsg();%>
<%aspOutPutApMailConfigInfo();%>
<!--
function init(){
	var f=ApMail_Config;

	f.enable.checked = (enables == "on") ? true : false;
	f.senderEmail.value = senderEmails;
	f.senderEmailPasswd.value = senderEmailPasswds;
	f.receiverEmail.value = receiverEmails;
	f.smtpServer.value = smtpServers;
	f.emailTitle.value = emailTitles;
	f.sendSpaceTime.value = space_time;

}
function re_checkEmail(cntrl, tips)
{	
    //验证邮箱的正则表达式
    var reg = /^\w+((-\w+)|(\.\w+))*\@[A-Za-z0-9]+((\.|-)[A-Za-z0-9]+)*\.[A-Za-z]+$/;
    var retips = "";
    var value = trimstr(cntrl.value);
    var tips = "<%write(email);%>";

    if (value == "") {
	retips = tips + "<% write(JSAlert33); %>";
    } else if(!reg.test(value)){
	retips = tips + "<% write(format); %>" + "<% write(JSAlert35); %>";
    }

	return retips;
}

function re_checkServer(cntrl,tips)
{
    var retips = "";
    var value = trimstr(cntrl.value);
    var tips = "<%write(smtpServer);%>";

    if (value == "") {
	retips = tips + "<% write(JSAlert33); %>";
    } else if(!/^([\w-]+\.)+((com)|(net)|(org)|(gov\.cn)|(info)|(cc)|(com\.cn)|(net\.cn)|(org\.cn)|(name)|(biz)|(tv)|(cn)|(mobi)|(name)|(sh)|(ac)|(io)|(tw)|(com\.tw)|(hk)|(com\.hk)|(ws)|(travel)|(us)|(tm)|(la)|(me\.uk)|(org\.uk)|(ltd\.uk)|(plc\.uk)|(in)|(eu)|(it)|(jp))$/.test(value)) {
	retips = tips + "<% write(format); %>" + "<% write(JSAlert35); %>";
    }
	return retips;

}
function save(){
    var f=ApMail_Config;

    if (checkData()) {
	f.action = "/goform/formApMail";
	f.submit();
    }
}
function checkData(){
    var f = ApMail_Config;
    var senderEmailObj = f.senderEmail;
    var senderEmailPasswdOjb = f.senderEmailPasswd;
    var receiverEmailOjb = f.receiverEmail;
    var smtpServerObj = f.smtpServer;
    var tips = "";
    var enableObj = f.enable;
    var emailTilteObj = f.emailTitle;

    enableObj.value = enableObj.checked == true ? "on" : "off";
	if(!checkEnglishName(f.emailTitle,30,"<%write(emailTitle);%>",0)) return false;
	if(!checkSmtpServer(f.smtpServer,60,"<%write(smtpServer);%>",0)) return false;
	if(!checkEmail(f.senderEmail,64,"<%write(senderEmail);%>",0)) return false;
	if(!checkPassword2(f.senderEmailPasswd,30,"<%write(senderEmailPasswd);%>",0,1)) return false;
	if(!checkEmail(f.receiverEmail,64,"<%write(receiverEmail);%>",0)) return false;
	if(!checkNumber(f.sendSpaceTime,1,65535,"<%write(sendSpaceTime);%>",0)) return false;

    return true;
}
function Reset(){
	ApMail_Config.reset();
	init();
}
//-->
</script>
</head>
<body onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<%write(mailNotice);%>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"	 name="ApMail_Config" method="post">
<p>&nbsp;</p>
<script language="javascript">
var arrayThreeMenu=new Array(
	"80%",
	3,
	new Array("<% write(APL); %>",'apLog.asp',1,0),
	new Array("<% write(LogManage); %>",'apLogManage.asp',1,0),
	new Array("<% write(mailNotice);%>",'apMailConfig.asp',1,1)
	);
showThreeMenu();
</script>
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
<td align="center"  class="three_td" valign="top">
  <table border="0" width="600" align='center'>
  		<tr><td colspan="4" height="20"></td></tr>
	    <tr>
		 <td width="35%" align="right" height="25" id=""><%write(com_startUse);%></td>
		 <td width="2%"></td>
		 <td width="60%">
		 	<input name="enable" id="enable" type="checkbox"/>
		</td>
		<td width="3%" ></td>
	   </tr>

	    <tr>
		 <td align="right" height="25" id=""><%write(emailTitle);%></td>
		 <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
		 <td>
		 	<input type="text" name="emailTitle" id="emailTitle" maxlength="30" size="18" onchange="showfiledtip(re_checkEnglishName(this,'<%write(emailTitle);%>'),yutd6)" style="width:160px"/>
		</td>
		<td id="yutd6"></td>
	   </tr>	   	   	   		

	    <tr>
		 <td align="right" height="25" id=""><%write(smtpServer);%></td>
		 <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
		 <td>
		 	<input type="text" name="smtpServer" id="smtpServer" maxlength="60" size="18"  onchange="showfiledtip(re_checkServer(this,'<%write(smtpServer);%>'),yutd5)" style="width:160px" />&nbsp;（<%write(format);%>：smtp.xx.com）
		</td>
		<td  id="yutd5"></td>
	   </tr>	   	   	   		

		<tr>
	     <td align="right" height="25" id=""><%write(senderEmail);%></td>
		 <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
		 <td>

		 	<input type="text" name="senderEmail" id="senderEmail" maxlength="64" size="18"  onchange="showfiledtip(re_checkEmail(this,'<%write(senderEmail);%>'),yutd2)" style="width:160px" />&nbsp;（<%write(Tips);%>）
		</td>
		<td  id="yutd2"></td>
	   </tr>
	    <tr>
		 <td align="right" height="25" id=""><%write(senderEmailPasswd);%></td>
		 <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
		 <td>
		 	<input type="password" name="senderEmailPasswd" id="senderEmailPasswd" maxlength="30" size="18" onchange="showfiledtip(re_checkPassword1(this,30,0),yutd3)" style="width:160px"/>
		</td>
		<td  id="yutd3"></td>
	   </tr>
	    <tr>
		 <td align="right" height="25" id=""><%write(receiverEmail);%></td>
		 <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
		 <td>
		 	<input type="text" name="receiverEmail" id="receiverEmail" maxlength="64" size="18"  onchange="showfiledtip(re_checkEmail(this,'<%write(senderEmail);%>'),yutd4)" style="width:160px"/>
		</td>
		<td  id="yutd4"></td>
	   </tr>

		<tr>
	     <td align="right" height="25" id=""><%write(sendSpaceTime);%></td>
		 <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
		 <td>
		 	<input type="text" name="sendSpaceTime" id="sendSpaceTime" onKeyPress="figureOnly()"  maxlength="5" size="18" onChange="showfiledtip(re_checkNumber(this,1,65535,'<% write(sendSpaceTime); %>'),yutd1)" style="width:160px"/>&nbsp;<%write(com_minute);%>
		</td>
		<td id="yutd1"></td>
	   </tr>

	  <tr><td align="center" colspan="4" height="20">&nbsp;<%write(ApMailConfigNote);%></td></tr>

	  <tr><td colspan="4" height="20">&nbsp;</td></tr>
	  <tr><td colspan="4">
  <table width="35%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td	  height="30" align="left" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>" onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_help);%>" onClick="window.open('<%write(com_helppage);%>#apLogManage')"></td>
  </tr>
</table>
  </td></tr>
  </table></td>
  </tr>
</table>
</form></td>
  </tr><script language="javascript">init();</script>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>

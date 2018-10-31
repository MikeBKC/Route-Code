
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("SysLogInfo"); %>
<% langTran("left"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<%getSysLang();%>

var errorstr="";

var strSecondMenuFocus="<% write(SysLogInfo); %>";
 
function makeRequest(url, content) {
    http_request = false;

    if (window.XMLHttpRequest) { // Mozilla, Safari,...
        http_request = new XMLHttpRequest();
        if (http_request.overrideMimeType) {
            http_request.overrideMimeType('text/xml');
        }
    } else if (window.ActiveXObject) { // IE
        try {
            http_request = new ActiveXObject("Msxml2.XMLHTTP");
        } catch (e) {
            try {
            http_request = new ActiveXObject("Microsoft.XMLHTTP");
            } catch (e) {}
        }
    }

    if (!http_request) {
        alert('Giving up :( Cannot create an XMLHTTP instance');
        return false;
    }
    http_request.onreadystatechange = alertContents;
    http_request.open('POST', url, true);
    http_request.send(content);
}

function alertContents() {
    if (http_request.readyState == 4) {
        if (http_request.status == 200) {
			uploadLogField(http_request.responseText);
        } else {
            alert('There was a problem with the request.');
        }
    }
}

function uploadLogField(str)
{
	if(str == "-1"){
		document.getElementById("syslog").value =
		"Not support.\n(Busybox->\n  System Logging Utilitie ->\n    syslogd\n    Circular Buffer\n    logread"
	}else
	{ 
	    str = str.replace(/kernel: /g ,'');
	    str = str.replace(/syslog: /g ,'');
	    document.getElementById("syslog").value = str;
	}
}

function updateLog()
{
	makeRequest("/goform/SysLogMess", "n/a", false);
}
function fillData(index,i)
{
	var tbl = User_Config;
	tbl.Action[1].checked = true;
	tbl.UserID.value = ids[i];
	tbl.textarea.value = IPs[i]+" "+Macs[i]+" "+Remark[i];
}
function radioClick()
{
	var tbl = User_Config;
	if (tbl.Action[0].checked)
	{
		tbl.hidId.value = "";
	}
	if (tbl.Action[1].checked)
	{
		tbl.hidId.value = tbl.fillId.value;
	}
}
 

function readArp() {
}


function save()
{
}

function init()
{
	updateLog();
}
function selectIP()
{
}

function delLogInformation()
{
    if(confirm("<% write(SureDelMsg); %>"))
    {
	NetAction_Config.action = "/goform/DelHistorySysLog";
	NetAction_Config.submit();
	
    }
}

//-->
</script>
</head>
<body onbeforeunload="logOut()" background="<% write(com_bodyMain); %>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td> 
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<% write(leftMenud77); %>");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"90%",
						2,
						new Array("<% write(SysLogInfo); %>",'SysLogInfo.asp',1,1),
						new Array("<% write(LogEdit); %>",'SysLogInfo_edit.asp',1,0)
		);
		showThreeMenu();
	</script>
  <table width="90%" border="0" cellspacing="0" cellpadding="0" align="center">

<tr>
     <td align="center" class="three_td" valign="top"><table width="600" border="0" cellspacing="0" cellpadding="0">

<form action="/goform/" method="post" name="NetAction_Config" id="NetAction_Config"> 
 
<tr>
    <td align="center" >
  <table border="0" width="600" align='center' >
		<tr height="50" valign="bottom">
		<td width="10%" >&nbsp;</td>
		<td width="77%">
		<table width="100%" align="center" >
		</table>		</td>
		<td width="13%">&nbsp;</td>
		</tr>
		<tr>
		<td>&nbsp;</td>
		<td align="center"><textarea id="syslog" name="syslog" style="font-size:15px; font-family:'新宋体'; width:692px; height:350px" readonly></textarea></td>
		<td>&nbsp;</td>
		</tr>		
		<tr>
		<td width="10%" height="40">&nbsp;</td>
		<td width="90%">
		<table width="100%"  style="width:700px;" align="center">
		<tr>
		

		<td width="158"  ><input type="hidden" name="PageId"/>
			<input type="hidden" name="data" />
			<input type="hidden" name="UserID" /></td>
			<td width="604" align="right"><input type="button" value="<% write(DelLog); %>" onClick="delLogInformation()" >&nbsp;</td>
		<td><input name="button" type="button" onClick="location.reload()" value="<% write(com_fresh); %>"/></td>
		</tr>
		</table>		</td>
		</tr>
	</table>
	    <br /><br />
 </td></tr>
</form> </table>
  </td></tr>
  <script language="javascript">init();</script>
</table> </td>
<!--////end-->

  </tr>
</table></td><td width="15"></td></tr></table>
</body>
</html>

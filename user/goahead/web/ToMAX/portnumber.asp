<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("WANportnumber");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script src="js/config.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">
var errorstr="";
var rightstr="";
<% outputTheErrorMsg(); %>
<% outputTheRightMsg(); %>
<%getWanIfCount();%>
 
var ports = "4";

function checkData() {
	if(portsno.PortNumber.value <= "4" && portsno.PortNumber.value >="1")
	{
		if(confirm("<%write(wanP_confirm);%>")){
			return true;
		}
	}else{
			alert("<%write(wanP_numerror);%>");
			return false;
	}
}
function save(){
	var f=portsno;
	
	if (checkData()) {
		f.action = "/goform/WanIfCountConfig";
		portsno.submit();
	} 
}  

function initTranslation()
{
	var e;
 
}

function init()
{
	//portsno.portnumbers.value = ports
}

</script>
</head>
<body onLoad="initTranslation()" onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<%write(wanP_config);%>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
   <script language="javascript">showright(rightstr);</script>
  <tr valign="top">
    <td><form action="/goform/WanIfCountConfig"  name="portsno" method="post" enctype="multipart/form-data">
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7">
	<table border="0" width="800" align='center'>
 <tr><td colspan="3" height="30">&nbsp;</td></tr>
  <tr>
    <td width="46%" align="right" id="portno"><%write(wanP_num);%></td>
  	<td width="1%"></td>
	<td width="52%"><select name="PortNumber">
	<script language="javascript">
    if ((platform == "nv520W")|| (platform == "U1000N") || (platform == "nv842W")|| (platform == "nv518W")|| (platform == "nv518W-VPN")|| (platform == "nv512W")) {/*去掉无线接口*/
	    maxwanIfCount = maxwanIfCount - 1;
	    wanIfCount = wanIfCount - 1;
	}else if((platform =="nv518W-Plus")||(platform =="nv518W-Lite"))/*去掉3G和无线接口*/
	{
	    maxwanIfCount = maxwanIfCount - 2 ;
	    wanIfCount = wanIfCount - 2;
	}

	for(var i=1;i<=maxwanIfCount;i++)
	{
		if(i==wanIfCount)
		{
			document.write('<option value="'+i+'" selected="selected">'+i+'</option>');	
		}else{
	    	document.write('<option value="'+i+'" >'+i+'</option>');
		}
	}
	</script>
	</select>
	</td>
	<td width="1%"  id="yutd1"></td>
  </tr>
   
	   <tr><td colspan="4">
  <table width="27%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="80%"  height="30" align="center" valign="bottom"><input type="button" class="input-button" value="<%write(com_save);%>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_help);%>" onClick="window.open('<%write(com_helppage);%>#WanPortNum')" id="utthelp"></td>
  </tr>
</table>

  </td></tr>
  <tr><td colspan="3" height="10">&nbsp;</td></tr>
 
</table></td></tr></table>
</form></td>
  </tr>
</table></td>
  <td width="15"></td><script>init();</script>
  </tr>
</table>
</body>
</html>

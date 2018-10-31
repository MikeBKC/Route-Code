<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("apLogManage"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">

<%getSysLang();%>
<%aspOutputApLogManageConfig();%>

function save()
{
	var f = LogManage_Config;

	f.action ="/goform/apLogManageConfig";
	f.submit();
}
function init()
{
	var f = LogManage_Config;
 
	f.apLinkLog.checked=(ApLinkLogEnable.toLowerCase() == "1"?true:false);
	f.apUserLog.checked=(ApUserLogEnable.toLowerCase() == "1"?true:false);
	f.apUserNum.checked=(ApUserNumEnable.toLowerCase() == "1"?true:false);

	setEnable();
}
/*全选按钮的主动操作对其他按钮的影响*/
function setAllEnable()
{
    var f = LogManage_Config;
    if(f.AllEnable.checked)
    {
	f.apLinkLog.checked = true;
	f.apUserLog.checked = true;
	f.apUserNum.checked = true;
    }
    else
    {
	f.apLinkLog.checked = false;
	f.apUserLog.checked = false;
	f.apUserNum.checked = false;
    }
}
/*单个选择对全选控件的影响*/
function setEnable()
{
    var f = LogManage_Config;
    if(
	    (f.apLinkLog.checked == false)
	    || (f.apUserLog.checked == false)
		|| (f.apUserNum.checked == true)
      )
    {
	f.AllEnable.checked = false;
    }
    if(
	    (f.apLinkLog.checked == true)
	    && (f.apUserLog.checked == true)
		&& (f.apUserNum.checked == true)
      )
    {
	f.AllEnable.checked = true;
    }
}

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
  <td><script language="javascript">showSecondTitle(_("<% write(LogManage); %>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td>		
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"80%",
						3,
						new Array("<% write(APL); %>",'apLog.asp',1,0),
						new Array("<% write(LogManage); %>",'apLogManage.asp',1,1),
						new Array("<% write(mailNotice); %>",'apMailConfig.asp',1,0)
		);
		showThreeMenu();
	</script>
	
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
    <td align="center"  class="three_td" valign="top">
	<table border="0" width="600" align='center'>
	 <form action=""  name="LogManage_Config" method="post" >
 <tr><td colspan="3" height="30">&nbsp;</td></tr>

 <tr><td colspan="4" align="center"><table width="600" border="0" cellspacing="0" cellpadding="0" align="center">
 
    <tr>
	 <td width="120" height="22" align="right"><input type="checkbox" name="AllEnable" onClick="setAllEnable()" /></td>
		<td width="10" height="22">&nbsp;</td>
		<td width="454" height="22" colspan="2"><input type="hidden" name="hidAll" id="hidAll" size="10" value="1">     <strong><% write(SelectA); %></strong></td>
		</tr>
    <tr>
	<td width="120" align="right" ><input name="apLinkLog" type="checkbox" onClick="setEnable()"></td>
  	<td width="10"></td>
	<td width="454"><% write(APLinkLog); %></td>
    </tr>
    <tr>
	<td width="120" align="right" ><input name="apUserLog" type="checkbox" onClick="setEnable()"></td>
  	<td width="10"></td>
	<td width="454"><% write(APUserLog); %></td>
    </tr>
    <tr>
    <tr>
	<td width="120" align="right" ><input name="apUserNum" type="checkbox" onClick="setEnable()"></td>
  	<td width="10"></td>
	<td width="454"><% write(APUserNumLog); %></td>
    </tr>
    <tr>
  </table></td></tr>
  
  
     
    <tr><td colspan="3" >&nbsp;</td></tr>
	 
  <tr>  <td colspan="3" height="10" align="center"><input type="button" class="input-button" value="<% write(com_save); %>"  onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_help); %>" onClick="window.open('<% write(com_helppage); %>#apLogManage')"></td> </tr> 
  <tr><td colspan="3" height="10">&nbsp;</td></tr>
  <script language="javascript">init();</script></form>
</table></td></tr></table>
</td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>

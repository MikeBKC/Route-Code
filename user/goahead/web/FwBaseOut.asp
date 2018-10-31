<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("FwBase");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">

//Butterlate.setTextDomain("admin"); 

//function initTranslation()
//{
// 
//}

<% aspOutSecDefense(); %>

function save(){
	var f=FwBase_Config;

	 
	 
	f.PingDisable.value=(f.PingDisablew.checked==true?"on":"off"); 
	FwBase_Config.action ="/goform/formFwBaseOut";
	FwBase_Config.submit();
}
function init(){
	var f=FwBase_Config;
 
 
	f.PingDisablew.checked=(PingDisables.toLowerCase()=="on"?true:false);
}



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
  <td><script language="javascript">showSecondTitle(_("<%write(fwb_safeConf);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">//showerror(errorstr);</script>
  <tr valign="top">
    <td>		
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"80%",
						2,
						new Array("<%write(fwb_inner);%>",'FwBase.asp',1,0),
						new Array("<%write(fwb_outer);%>",'FwBaseOut.asp',1,1)
		);
		showThreeMenu();
	</script>
	
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
    <td align="center"  class="three_td" valign="top">
	<table border="0" width="600" align='center'>
	 <form action=""  name="FwBase_Config" method="post" >
 <tr><td colspan="3" height="30">&nbsp;</td></tr>

 <tr><td colspan="4" align="center"><table width="600" border="0" cellspacing="0" cellpadding="0" align="center">
	   <tr style="display:none">
	      <td width="120" height="25" align="left"><strong><%write(fwb_virus);%></strong></td>
			  
	       <td width="10">&nbsp;</td>
	     <td width="468">&nbsp;</td>
		</tr>
		
 
    <tr>
    <td width="120" align="right" ><input name="PingDisablew" type="checkbox"><input name="PingDisable" type="hidden"></td>
  	<td width="10"></td>
	<td width="454"><%write(fwb_ping);%></td>
  </tr>
  </table></td></tr>
  
  
     
    <tr><td colspan="3" >&nbsp;</td></tr>
	 
  <tr>  <td colspan="3" height="10" align="center"><input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_help);%>" onClick="window.open('<% write(com_helppage); %>#FwBaseHl')"></td> </tr> 
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

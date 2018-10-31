<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("TagVlan");%>
<%langTran("commonWords");%>
<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">

<% 
	aspOutputWanTagVlanData();
%>
// 交互end

function clickEnable() {
	var frm = wanTagFrm;
	if (frm.enable.checked) {
		frm.vlanId.disabled = false;
		frm.rule.disabled = false;
	} else {
		frm.vlanId.disabled = true;
		frm.rule.disabled = true;	
	}	
}

function init() 
{
	var frm = wanTagFrm;

	frm.enable.checked = WanTagVlanEnable;
	clickEnable();
	
	frm.vlanId.value = vlanId;
	if (tagged == 1) {
		frm.rule.value = "tag";
	} else {
		frm.rule.value = "untag";
	}
}


function checkData()
{

	if (!checkNumber(wanTagFrm.vlanId, 6, 4094, _("<% write(pvid); %>/<%write(vlanId);%>"), 0, 0)) {
		return false;
	}

	return true;
}

function save()
{
	var f = wanTagFrm;
	if (checkData())
	{

		f.action="/goform/formWanTagVlanConfig";

		f.submit();	
	}
}



function Reset()
{
	wanTagFrm.reset();
	init();
}

</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="images/body_main.gif" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
   <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
  <td class="table_bg"><script language="javascript">showSecondTitle("<% write(wanTagVlan);%>");//显示页面名称</script>
  
  
  
   <table width="80%" border="0" height="450" cellspacing="0" cellpadding="0" align="center">
   <script language="javascript">showerror(errorstr,100);//显示保存返回的错误信息</script>
  <tr valign="top">
    <td>
	<!--//三级菜单-->
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"100%",
						1,
						new Array("<% write(wanTagVlan); %>",'WanTagVlan.asp',1,1)
		);
		showThreeMenu();
	</script>
	 <!--//end-->






<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center"><tr>
    <td align="center"  class="three_td">
	
	
	<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr><td>
  
  
  
  
  
  
  
  <table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<br>




<form name="wanTagFrm" method="post" action="">
<table border="0" width="600" align='center'>
  <tr><td colspan="3" height="10">&nbsp;</td></tr>
	   <tr>
	   <td align="right"  width="40%" height="25" ><%write(enWanTagVlan);%></td>
	   <td  width="4%">&nbsp;</td>
	   <td width="56%"><input type="checkbox" name="enable" id="enable" onClick="clickEnable()"></td>
	   </tr>
	   <tr>
	   <td align="right"  width="40%" height="25"><% write(pvid); %>/<% write(vlanId); %></td>
	   <td  width="4%">&nbsp;</td>
	   <td><input type="text" class="input" name="vlanId" id="vlanId" size="5" maxlength="4" value="" onChange='checkNumber(this,6,4094,_("<% write(pvid); %>/<% write(vlanId); %>"),0,0)'>（<%write(range);%>: 6-4094）</td>
	   </tr>
<tr>
	   <td align="right"  width="40%" height="25"><% write(vlanRule); %></td>
	   <td  width="4%">&nbsp;</td>
	   <td><select name="rule" id="rule" class="input" ><option value="untag">untag</option><option value="tag">tag</option></select></td>
	   </tr>
	    <!--///wpapsktbl end/// -->
  </table>
<table align="center"><tr><td height="5"></td></tr><tr><td>
<input type="button" onClick="save()"class="input-button" value="<%write(save);%>" name="" id="uttsave">&nbsp;
<input type="button" class="input-button" value="<%write(refill);%>" name="" id="uttreset" onClick="Reset();">
<input type="button" class="input-button" value="<%write(help);%>" name="" id="utthelp" onClick="window.open('enrhelpinfo.htm#')">
</td></tr></table>
</form>






<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
</td><td class="gapTd"></td></tr>
  </table>
  
  
  
  
  
  
  
 </td>
  </tr>
</table> 





  </td></tr>
</table>




</td></tr></table>



</td>
<!--////end-->
  <td width="15">&nbsp;</td>
  </tr>
</table>
</body>
</html>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("advideo");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">
var advideo = 0 ;
var act_en = 1 ;
<%aspAdvideo();%> 
function save(){
    var f=formadvideo;
    if(f.advideo.checked)
        f.action="/goform/ConfigAdvideo";
    else 
        f.action="/goform/ConfigAdvideoDel";
    f.submit();
}  

function init()
{
    var f=formadvideo;
    f.advideo.checked=((advideo == 1) ? true:false);
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
  <td><script language="javascript">showSecondTitle("<%write(guanggao);%>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
  <script language="javascript">showerror(errorstr,80);</script>
  <script language="javascript">showright(rightstr,80);</script>
  <tr valign="top">
    <td><form action="/goform/ConfigAdvideo"  name="formadvideo" method="post" enctype="multipart/form-data">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7">
	<table border="0" width="600" align='center'>
 <tr><td colspan="3" height="30">&nbsp;</td></tr>
  <tr>
		<td width="46%" align="right" height="25" id="jiance"><%write(ant_guanggao);%></td>
		<td width="1%">&nbsp;</td>
		<td width="53%" height="20"><input name="advideo" type="checkbox" ></td>
	      </tr>
	   <tr><td colspan="4">
  <table width="21%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="62%"  height="30" align="center" valign="bottom"><input type="button" class="input-button" value="<%write(com_save);%>" onClick="save()" id="uttsave"> &nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#advideo')"></td>
  </tr>
</table>

  </td>
  </tr>
  <tr><td colspan="3" height="10">&nbsp;</td></tr>
 
  <tr>
    <td colspan="5" width="62%"  height="30" align="center" valign="bottom"><input type="checkbox" disabled checked >优酷网&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type="checkbox" checked disabled >土豆网</td>
  </tr>
  <td width="377">&nbsp;&nbsp;<input name="GroupName" type="hidden" maxlength="12" size="18" value="utt_action"></td>
  <td colspan="3" height="20"><input type="hidden" name="Action" value="add"><input type="hidden" name="pdbPolicy" value="143">&nbsp;</td>
  <td height="25" align="right"><input type="hidden" name="sourceIP" value="groupSel"></td>
  <td><input type="hidden" name="day" value="1111111"><td>   
  <td><input type="hidden" name="timestart" value="All"></td>               
  <td><input type="hidden" name="timestop"  value="0"></td>
</table></td></tr></table>
</form></td>
  </tr>
</table></td>
  <td width="15"></td><script>init();</script>
  </tr>
</table>
</body>
</html>

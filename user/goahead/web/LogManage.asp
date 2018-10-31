<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("LogManage"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">

<%getSysLang();%>
<%aspOutputLogManageConfig();%>

function save()
{
	var f = LogManage_Config;

	f.action ="/goform/LogManageConfig";
	f.submit();
}
function init()
{
	var f = LogManage_Config;
 
	f.weblog.checked=(webLogEnable.toLowerCase() == "1"?true:false);
	f.QQlog.checked=(QQLogEnable.toLowerCase() == "1"?true:false);
	f.MSNlog.checked=(MSNLogEnable.toLowerCase() == "1"?true:false);
	f.maillog.checked=(mailLogEnable.toLowerCase() == "1"?true:false);
	f.eimlog.checked=(eimLogEnable.toLowerCase() == "1"?true:false);
	f.luntanlog.checked=(luntanLogEnable.toLowerCase() == "1"?true:false);
	f.weibolog.checked=(weiboLogEnable.toLowerCase() == "1"?true:false);
	f.Natlog.checked=(NatLogEnable.toLowerCase() == "1"?true:false);
	if(xportEnable == "yes"){
	    document.getElementById("luntanLog").style.display="";
	    document.getElementById("weiboLog").style.display="";
	    document.getElementById("NatLog").style.display="";
	    document.getElementById("zhuyi").style.display="";
	}

	setEnable();
}
/*ȫѡ��ť������������������ť��Ӱ��*/
function setAllEnable()
{
    var f = LogManage_Config;
    if(f.AllEnable.checked)
    {
	f.weblog.checked = true;
	f.QQlog.checked = true;
	f.MSNlog.checked = true;
	f.maillog.checked = true;
	f.eimlog.checked = true;
        if(xportEnable == "yes"){
		f.luntanlog.checked = true;
	        f.weibolog.checked = true;
		f.Natlog.checked = true;
    if (!confirm("��������NAT��־���ܣ��˹��ܽ��ϴ���־��Ϣ��\n��ռ�ò��ִ�����Դ��������Ĵ�����Դ���㣬\n��������ʹ�ô˹��ܡ����Ƿ�ȷ�Ͽ����˹��ܣ�")) {
        f.Natlog.checked = false;
        return false;
    }
	}
    }
    else
    {
	f.weblog.checked = false;
	f.QQlog.checked = false;
	f.MSNlog.checked = false;
	f.maillog.checked = false;
	f.eimlog.checked = false;
	f.luntanlog.checked = false;
	f.weibolog.checked = false;
	f.Natlog.checked = false;
    }
}
/*����ѡ���ȫѡ�ؼ���Ӱ��*/
function setEnable(val)
{
    var f = LogManage_Config;
    if((val == 1) && (f.Natlog.checked == true))
    {
    if (!confirm( "���ô˹��ܽ��ϴ���־��Ϣ����ռ�ò��ִ�����Դ��\n������Ĵ�����Դ���㣬��������ʹ�ô˹��ܡ�\n���Ƿ�ȷ�Ͽ����˹��ܣ�")) {
        f.Natlog.checked = false;
        return false;
    }
    }
    if(
	    (f.weblog.checked == false)
	    || (f.QQlog.checked == false)
	    || (f.MSNlog.checked == false)
	    || (f.maillog.checked == false)
	    || (f.eimlog.checked == false)
	    ||(((f.luntanlog.checked == false)
	    || (f.weibolog.checked == false)
	    || (f.Natlog.checked == false))
	    || (xportEnable != "yes"))
      )
    {
	f.AllEnable.checked = false;
    }
    if(
	    (f.weblog.checked == true)
	    && (f.QQlog.checked == true)
	    && (f.MSNlog.checked == true)
	    && (f.maillog.checked == true)
	    && (f.eimlog.checked == true)
	    &&(((f.luntanlog.checked == true)
	    && (f.weibolog.checked == true)
	    && (f.Natlog.checked == true))
	    || (xportEnable != "yes"))
      )
    {
	f.AllEnable.checked = true;
    }
}

</script>
</head>
<body onbeforeunload="logOut()" background="<% write(com_bodyMain); %>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****�м�����****-->
  <tr>
  <td width="15"></td>
  <!--////����-->
  <td><script language="javascript">showSecondTitle(_("<% write(LogManage); %>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td>		
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"80%",
						2,
						new Array("<% write(UBAS); %>",'NetActionAudit.asp',1,0),
						new Array("<% write(LogManage); %>",'LogManage.asp',1,1)
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
	<td width="120" align="right" ><input name="weblog" type="checkbox" onClick="setEnable()"></td>
  	<td width="10"></td>
	<td width="454"><% write(UseWebLog); %></td>
    </tr>
    <tr>
	<td width="120" align="right" ><input name="QQlog" type="checkbox" onClick="setEnable()"></td>
  	<td width="10"></td>
	<td width="454"><% write(UseQQLog); %></td>
    </tr>
    <tr>
	<td width="120" align="right" ><input name="MSNlog" type="checkbox" onClick="setEnable()"></td>
  	<td width="10"></td>
	<td width="454"><% write(UseMSNLog); %></td>
    </tr>
    <tr>
	<td width="120" align="right" ><input name="maillog" type="checkbox" onClick="setEnable()"></td>
  	<td width="10"></td>
	<td width="454"><% write(UseMailLog); %></td>
    </tr>
    <tr>
	<td width="120" align="right" ><input name="eimlog" type="checkbox" onClick="setEnable()"></td>
  	<td width="10"></td>
	<td width="454"><% write(UseEimLog); %></td>
    </tr>
    <tr id="luntanLog" style="display:none">
	<td width="120" align="right" ><input name="luntanlog" type="checkbox" onClick="setEnable()"></td>
  	<td width="10"></td>
	<td width="454">��̳��־</td>
    </tr>
    <tr id="weiboLog" style="display:none" >
	<td width="120" align="right" ><input name="weibolog" type="checkbox" onClick="setEnable()"></td>
  	<td width="10"></td>
	<td width="454">΢����־</td>
    </tr>
    <tr id="NatLog"  style="display:none">
	<td width="120" align="right" ><input name="Natlog" type="checkbox" onClick="setEnable(1)"></td>
  	<td width="10"></td>
	<td width="454">Nat��־</td>
    </tr>
  </table></td></tr>
    <tr><td colspan="3" >&nbsp;</td></tr>
	 
  <tr>  <td colspan="3" height="10" align="center"><input type="button" class="input-button" value="<% write(com_save); %>"  onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_help); %>" onClick="window.open('<% write(com_helppage); %>#LogManage')"></td> </tr> 
  <tr><td colspan="3" height="10">&nbsp;</td></tr>
  <tr><td colspan="4"><table width="100%">
  <tr  height="30">
  <td class="helpfont"  align="center" id="zhuyi" style="display:none"><strong>ע�⣺</strong>��̳��־��΢����־��Nat��־�����Xportʹ�á� </td></tr>
  </table></td></tr>
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

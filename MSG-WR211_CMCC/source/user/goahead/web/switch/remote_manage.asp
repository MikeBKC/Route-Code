<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>��������-��������</title>
<link rel=stylesheet type="text/css" href="../css/style.css" title="cool" id="lnk">
<script src="../js/common.js" type="text/javascript"></script>
<script src="js/common_switch.js" type="text/javascript"></script>
<script language="javascript">
//����
<%
var parentPage=Form("parentPage") ;//�����ĸ�ҳ��
var ipStr=Form("ipStr");//Զ��ip�ִ�
var macStr=Form("macStr");//Զ��mac�ִ�
var passStr=Form("passStr");//Զ�������ִ�
var optType=Form("optType");//��������
if (parentPage=="discover")
{
	aspSignalRemoteAuth();//Զ��������֤����������������֤����
}
else if (parentPage=="topoy")
{
        aspTopoInfo();//����ҳ������������跢��������֤����ȡ����汾����Ϣ
}
else if (optType=="reboot") 
{
	aspRebootRemote();//������c������
} 
else if (optType=="reset")
{
	aspResetRemote();//�ָ��������ã�c������
}
else if (optType=="enablePunish")
{
	//aspEnablePunishRemote();//���������ͷ���c������
} 
else if (optType=="delPunish")
{
	//aspDelPunishRemote();//���ȫ���ͷ���c������
} 
aspOutSysInfoRemote();//��ӡ�豸��Ϣ��c������
%>
//var errorArr = new Array("","","","");//����ÿ���������������ӡ
var ipStr="<%write(ipStr);%>";//Զ��ip�ִ�
var macStr="<%write(macStr);%>";//Զ��mac�ִ�
var passStr="<%write(passStr);%>";//Զ�������ִ�
var parentPage="<%write(parentPage);%>";//���Ե�ҳ��
var optType="<%write(optType);%>";
//end����
var errorstr="";
try
{
	errorstr=errorArr[0];
}
catch(e)
{//ҳ��δ��ӡerrorArr[0]��Ϣ
	errorstr="";
}
if(macStr=="")//�û�ֱ�Ӵ򿪴�ҳ��
{
	closeWin("�Ƿ�������");
}
if(parentPage=="discover" || parentPage=="topoy")
{
	if(trimstr(errorstr)!="")
	{
		closeWin(errorstr);
	}
}
else if(optType!='')
{
	if(errorstr.indexOf("�������")>=0)
	{
		closeWin(errorstr);
	}
}
if(optType!='')//�Ǳ�ҳ���������������(���� �� �ָ���������)
{
	if(errorstr=='')
	alert("�����ɹ���"); 
}
var secondTitle='��������';
var MAC='';
for(var i=0;i<macStr.length;i+=2) MAC=MAC+macStr.slice(i,i+2)+'-';
MAC=MAC.slice(0,-1);
function init()
{  
    controlWin(760,530,1);
	REMOTECONF.passStr.value = passStr;
	REMOTECONF.ipStr.value= ipStr;
	REMOTECONF.macStr.value= macStr;
	window.document.title+=" / "+ipStr;
}
function save(opt)
{
	var page="remote_manage.asp";
	if(opt == "reboot")//�����豸
	{
		if(!confirm("ȷ�������豸?���豸����Լ��1���Ӻ󣬲ſ��ٴι�������")) return;
	}
	else if(opt == "reset")//�ָ���������
	{
		if(!confirm("ȷ���ָ���������?��ע��ǿ�ҽ���ָ�ǰ���Ƚ������ñ��档�ָ��������������豸����")) return;
	}
	else if(opt == "enablePunish")//���������ͷ�
	{
		if(!REMOTECONF.punishEnable.checked)
		{
			if(!confirm("�ر������ͷ�����ǰ���б���ֹ���������Զ������ȷ������?")) return;
		}
		else 
		{
			if(!confirm("���������ͷ�������ֹ�Ƿ�����������ȷ������?")) return;
		}
	}
	else if(opt == "delPunish")//���ȫ���ͷ�
	{
		if(!confirm("ȷ�����ȫ�������ͷ���\nע�⣺ֻ������γͷ�����������������ٴ��зǷ���Ϊ������Ȼ����ֹ��")) 	
		  return;
	}
    else if(opt =="getConf" || opt =="getEventslog")
	{
		page = "wait_remote_conf.asp" ;//ֱ�Ӵ�ҳ��	
	}
	else
	{
		page = opt ;//ֱ�Ӵ�ҳ��
		opt = "";//������֪����ʲô����
	}
	REMOTECONF.optType.value=opt;//��̨���ݸ�ֵȷ����ʲô����
	REMOTECONF.action=page;
	REMOTECONF.submit();
}

</script>
</head>
<body onLoad="init()" onbeforeunload="logOut()" background="../images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
   <!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****�м�����****-->
  <tr>
   <td width="15"></td>
  <!--////����-->
 <td><script language="javascript">showSecondTitle(secondTitle);//��ʾҳ������</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);//��ʾ���淵�صĴ�����Ϣ</script>
  <tr valign="top">
    <td>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
	<table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td align="center">
    <!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
    <br>
    <form name="REMOTECONF" id="REMOTECONF" method="post">
    <input type="hidden" value="Apply" name="_submit">
    <input type="hidden" value="" name="optType">
    <input type="hidden" value="" name="ipStr">
    <input type="hidden" value="" name="macStr">
    <input type="hidden" value="" name="passStr">
     <table width="600" cellpadding="0" cellspacing="0">
    <tr><td width="273"><b>Զ���豸��Ϣ</b></td>
    </tr>
    <tr height="8"><td></td></tr>
    </table>
    <TABLE  cellSpacing=1  width="600" cellPadding=4  class="style22">
      <tr class="style18">
        <td width="68" height="27" class=vncell>�豸��</td>
        <td width="171"><script language="javascript">document.write(deviceName);</script></td>
        <td width="68" class=vncell>�ͺ�</td>
        <td width="254"><script language="javascript">document.write(ProductID);</script></td>
      </tr>
      <tr>
        <td class=vncell height="27">���к�</td>
        <td><script language="javascript">document.write(sysSerial);</script></td>
        <td class=vncell>IP��ַ</td>
        <td><script language="javascript">document.write(ipStr);</script></td>
      </tr>
      <tr class="style18">
        <td class=vncell height="27">MAC��ַ</td>
        <td><script language="javascript">document.write(MAC);</script></td>
        <td class=vncell height="27">����汾</td>
        <td><script language="javascript">document.write(sysVersion);</script></td>
      </tr>
    </TABLE>
    <table width=600 border="0" cellspacing="0" cellpadding="0" align="center">
<tr height="25"><td></td></tr>
<tr height="1"><td background="../images/dot.gif"><img src="../images/space.gif" width="8" height="1"></td></tr>
<tr height="18"><td></td></tr>
</table>
    <table width="600" cellpadding="0" cellspacing="0">
    <tr>
      <td width="273"><b>Զ���豸����</b></td>
      <td width="325" align="right"></td>
    </tr>
    <tr height="8"><td colspan="2"></td></tr>
    </table>
    <table width="600" border="0"  cellpadding="4" cellspacing="1" class="style22">
    <tr class="style18">
    <td colspan="5" align="left"><b>�˿ڹ���</b></td>
    </tr>
    <tr>
    <td width="190"><a href="javascript:save('ports.asp')">�˿ڹ���</a></td>
    <td width="191"><a href="javascript:save('port_vlan.asp')">�˿�VLAN</a></td>
    <td width="191"><a href="javascript:save('aggregation.asp')">�˿ڻ��</a></td>
    </tr>
    </table>
    <br>
    <table width="600" border="0"  cellpadding="4" cellspacing="1" class="style22">
    <tr>
      <td colspan="6" align="left" class="style18"><b>���簲ȫ����</b></td>
    </tr>
    <tr>
      <td width="190"><a href="javascript:save('mirror.asp')">�˿ھ���</a></td>
    </tr>
    <tr style="display:none">
      <td colspan="3">���������ͷ�
      <input type="checkbox" value="1" name="punishEnable" class="input-checkbox">
      <input type="button" class="input-button" value="����" onClick="save('enablePunish')" id="uttsave">
      <input type="submit" name="button2" id="button2"  class="input-button" onClick="save('delPunish')"  value="���ȫ���ͷ�">		     </td>
</tr>
    </table>
    <br>
    <table width="600" border="0"  cellpadding="4" cellspacing="1" class="style22">
    <tr>
      <td colspan="6" align="left" class="style18"><b>ϵͳ����</b></td>
      </tr>
      <tr>
       <td width="86"><a href="javascript:save('sysInfo.asp')">ϵͳ��Ϣ</a></td>
       <td width="82"><a href="javascript:save('sysConf.asp')">ϵͳ����</a></td>
      <td width="69"><a href="javascript:save('reboot')">�����豸</a></td>
      <td width="92"><a href="javascript:save('reset')">�ָ���������</a></td>
      <td width="78"><a href="javascript:save('getConf')">��ȡ����</a></td>
      <td width="138"><a href="javascript:save('linkageConfSend.asp')">�·�����</a></td>
</tr>
</table>   
    </form>               
    <!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
	  </td><td class="gapTd"></td> </tr></table>
 </td>
  </tr>
</table> 
  </td></tr>
</table></td></tr></table></td>
<!--////end-->
<td width="15"></td>
  </tr>
</table>
</body>
</html>

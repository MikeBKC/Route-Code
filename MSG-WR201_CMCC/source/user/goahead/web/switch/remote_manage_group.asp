<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>�������-��������</title>
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
if (optType=="reboot") 
{
	aspRebootRemote();//������c������
} 
else if (optType=="reset")
{
	aspResetRemote();//�ָ��������ã�c������
}
else if (optType=="bind")
{
        aspBindRemote();//��ȫ�󶨣�c������
} 
else if (optType=="setPass")
{
	aspSetPassRemote();//�޸����루c������
} 
else if (optType=="enablePunish")
{
	//aspEnablePunishRemote();//���������ͷ���c������
} 
else if (optType=="delPunish")
{
	//aspDelPunishRemote();//���ȫ���ͷ���c������
} 
else if (parentPage=="wait") aspOutErrorArr();//��ӡerrorArr��c����,ÿ�δ�ӡ��֮����ձ�ɣ�
//����ÿ�������󣬶����ӡ��Ӧ��errorArr��Ϣ
%>
try
{
	if(errorArr.length>0){};//�������ʲô�޹�ϵ���ؼ����ж�errorArr�Ƿ���ڡ���������������쳣
}
catch(e)
{
	var errorArr=new Array("");
}
var errorstr='';
//end����
var ipStr="<%write(ipStr);%>";//Զ��ip�ִ�
var macStr="<%write(macStr);%>";//Զ��mac�ִ�
var passStr="<%write(passStr);%>";//Զ�������ִ�
var parentPage="<%write(parentPage);%>";//���Ե�ҳ��
var optType="<%write(optType);%>";

if(macStr=="")//�û�ֱ�Ӵ򿪴�ҳ��
{
	closeWin("�Ƿ�������");
}

function init()
{
	controlWin(650,339,1);
	REMOTECONF.passStr.value = passStr;
	REMOTECONF.ipStr.value= ipStr;
	REMOTECONF.macStr.value= macStr;
	if(parentPage=="wait")showBatchResult(errorArr,ipStr,macStr,"������֤ʧ����Ϣ",1);
	else if(optType!="") 
	{
		if(!showBatchResult(errorArr,ipStr,macStr,"����ʧ����Ϣ",1))
		{
			alert("�����ɹ�!");
		}
	}
}
function save(opt)
{
	if(opt == "bind")//��ȫ��
	{
		if(!confirm("ȷ����ѡ�豸��ȫ�����а�ȫ��?")) return;
	}
	else if(opt == "reboot")//�����豸
	{
		if(!confirm("ȷ��������ѡ�豸?���豸����Լ��1���Ӻ󣬲ſ��ٴι�������")) return;
	}
	else if(opt == "reset")//�ָ���������
	{
		if(!confirm("ȷ���ָ���ѡ�豸��������?��ע��ǿ�ҽ���ָ�ǰ���Ƚ������ñ��档�ָ��������������豸��")) return;
	}
/*	else if(opt == "getEventslog")//��ȡ��־
	{
		controlWin(750,470,1);
		REMOTECONF.action="wait_remote_conf.asp";
	}
*/	
	else if(opt == "getConf")//��ȡ����
	{
		controlWin(750,505,1);
		REMOTECONF.action="wait_remote_conf.asp";
	}
	else if(opt == "sendConf")//�·�����
	{
		controlWin(750,505,1);
		REMOTECONF.action="linkageConfSend.asp";
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
	else if(opt == "setPass")
	{
		if(!checkPassword(REMOTECONF.pw1, REMOTECONF.pw2, 11, 1))
		{
			return;
		}
		else
		{
			if(!confirm("ȷ���޸���ѡ�豸���룿")) return;
		}
	}
	REMOTECONF.optType.value=opt;//��̨���ݸ�ֵȷ����ʲô����
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
 <td><script language="javascript">showSecondTitle('�������-��������');//��ʾҳ������</script>
  <table width="100%" border="0" height="200" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);//��ʾ���淵�صĴ�����Ϣ</script>
  <tr valign="top">
    <td>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
	<table border="0" width="80%" align='center'><tr><td  class="gapTd"></td><td align="center">
    <!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
    <br><br>
    <form  method="post" name="REMOTECONF" target="_self">
    <input type="hidden" value="Apply" name="_submit">
    <input type="hidden" value="" name="ipStr">
    <input type="hidden" value="" name="macStr">
    <input type="hidden" value="" name="passStr">
    <input type="hidden" value="" name="optType">
    <input type="hidden" name="isGroupOpt" value="1">
    <table width="500" border="0"  cellpadding="4" cellspacing="1" class="style22">
    <tr class="style18">
    <td colspan="5" align="left"><b>��������</b></td>
    </tr>
    <tr>
    <td align="center"><a href="javascript:save('bind')">��ȫ��</a></td>
    <td align="center"><a href="javascript:save('reboot')">�����豸</a></td>
    <td align="center"><a href="javascript:save('reset')">�ָ���������</a></td>
    <td align="center"><a href="javascript:save('getConf')">��ȡ����</a></td>
    <td align="center"><a href="javascript:save('sendConf')">�·�����</a></td>
    <!--
    <td align="center"><a href="javascript:save('getEventslog')">��ȡ��־</a></td>
    -->
    </tr>
        <tr>
      <td height="25" colspan="6">���룺
       <input type="password" class="input" name="pw1" size="16" maxlength="11" value="" onChange="checkPassword1(this,11,1);" id="pw1">
        ����ȷ�ϣ�
        <input type="password" class="input" name="pw2" size="16" maxlength="11" value="" onChange="checkPassword(REMOTECONF.pw1,this,11,1)" id="pw2">
        <label>
          <input type="button" name="button" id="button"  class="input-button" value="�޸�����" onClick="save('setPass')">
        </label></td>
</tr>
        <tr style="display:none">
          <td height="25" colspan="6">���������ͷ�
            <input type="checkbox" value="1" name="punishEnable" class="input-checkbox">
            <input type="button" class="input-button" value="����" onClick="save('enablePunish')" id="uttsave">
            <input type="submit" name="button2" id="button2"  class="input-button" onClick="save('delPunish')"  value="���ȫ���ͷ�"></td>
</tr>
    </table>
    </form>
    <br><br>
   <!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
   </td><td class="gapTd"></td> </tr></table>
 </td>
  </tr>
</table> 
  </td></tr>
</table></td></tr></table></td>
<td width="15"></td>
  </tr>
</table>
</body>
</html>

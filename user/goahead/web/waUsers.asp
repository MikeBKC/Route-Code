<html>
<head>
<% langTran("waUsers"); %>
<% langTran("common"); %>
<title><% write(WebAuthChangePW); %></title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<meta http-equiv="pragma" content="no-cache">
<script language="javascript">
<%getSysLang();%>
<%aspGetWebAuthSelfHelpData();%>

/* �����¼���û�ip��ַ����PPPoE��ַ���ڣ��������û����ʴ�ҳ�� */
if (!isWebAuthIp) {
	alert("<% write(AlertPPPoEAddr); %>");
	window.location.replace("about:blank");
}
</script>
</head>
<body>
<table width="100%" border="0" align="center" cellpadding="0" cellspacing="0"  bgcolor="#0fB0D0"><tr>
<td height="551">
<form name="WebAuthSelfHelp" method="post" action="/goform/formWebAuthSelfHelpConfig">
<table width="50%" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#065C7D" style="border:solid 1px #CCCCCC">
<tr>
<td colspan="3" height="40" valign="middle" align="center" >
	<table width="400" height="40" cellpadding="0" border="0" cellspacing="0" id="fontTb" bgcolor="">
	<tr>
<td align="center" valign="bottom"><font size="+1" id="errFont" color="#FFFFFF"><strong><script language="javascript">document.write(optResult);</script></strong></font></td>
	</tr>
	</table>
</td>
</tr>
<tr>
<td height="20" valign="middle" colspan="3" align="center"><strong><font color="#FFFFFF">-------------------------------------</font></strong></td>
</tr>


<tr>
<td width="33%" height="30" align="right" valign="middle"><strong><font color="#FFFFFF"><% write(com_user); %></font></strong></td>
<td width="3%"></td>
<td width="64%"><input name="userName" id="userName" type="text" value="" style="width:200px; height:23px" maxgth="31"></td>
</tr>
<tr>
<td height="30" valign="middle" align="right"><strong><font color="#FFFFFF"><% write(OldPW); %></font></strong></td>
<td></td>
<td><input name="oldPasswd" id="oldPasswd" type="password" value="" style="width:200px; height:23px" maxgth="31"></td>
</tr> <tr>
<td height="30" valign="middle" align="right"><strong><font color="#FFFFFF"><% write(NewPW); %></font></strong></td>
<td></td>
<td><input name="newPasswd" id="newPasswd" type="password" value="" style="width:200px; height:23px" maxgth="31"></td>
</tr>
<tr>
<td height="30" valign="middle" align="right"><strong><font color="#FFFFFF"><% write(SurePW); %></font></strong></td>
<td></td>
<td><input name="verifyPasswd" id="verifyPasswd" type="password" value="" style="width:200px; height:23px" maxgth="31"></td>
</tr>

<tr>
  <td height="30" valign="middle">&nbsp;</td>
  <td></td>
  <td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" name="configSubmit"  bgcolor="#A9D1DC" value="<% write(Commit); %>" onClick="save()"><input type="hidden" name="userCommand" id="userCommand" value="userAuth"></td>
</tr>

<tr><td height="30" colspan="3">&nbsp;</td></tr>
</table>
</form>
<table width="100%" border="0" align="center" cellpadding="0" cellspacing="0"  bgcolor="#0fB0D0">
<tr>
<td height="40" valign="middle" colspan="3" align="center"><strong><font color="#FFFFFF" size="-1" id="tishi"></font></strong></td>
</tr>
</table>
</td>
</tr>
</table>
<script language="javascript">
///////////////////////////////////////
//���ܣ�����Ƿ�Ϸ��û���
//����˵��:
//cntrl--�ؼ���
//max--�������󳤶�
//tips--��ʾ��Ϣ
//allownull--1����Ϊ�գ�0������Ϊ��
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkName(cntrl,max,tips,allownull)
{
	name=cntrl.value;
	if(name=="")
	{
		if(!eval(allownull))
		{
		  alert(tips+"<% write(NameNULL); %>");
		  return false;
		}
		else
			return true;
	}
	else if (name.gth > max)
	{
		alert(tips+"<% write(NameLen); %>"+max+"<% write(NameLenW); %>");
		cntrl.focus();
		return false;
	}
	else if (name.indexOf("%")!=-1 || name.indexOf("\"")!=-1 || name.indexOf(":")!=-1  || name.indexOf(" ")!=-1 || name.indexOf("\'")!=-1 || name.indexOf("\\")!=-1)
	{
		alert(tips+"<% write(NameRule); %>");
		return false;
	}
	else
	{
		return true;
	}
}

///////////////////////////////////////
//���ܣ�����Ƿ�Ϸ�����
//����˵��:
//cntrl1��cntrl2--�ؼ���
//max--�������󳤶�
//allownull--1����Ϊ�գ�0������Ϊ��
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkPassword(cntrl1,cntrl2,max,allownull)
{
	pas1 = cntrl1.value;
	pas2 = cntrl2.value;
	if (pas1.gth > max)
	{
		alert("<% write(NewPWLen); %>"+maxlen+"<% write(NewPWLenW); %>");
		 cntrl1.focus();  
		return false;
	}
	else if(pas1 == "")
	{
		if(!eval(allownull))
		{
		  alert("<% write(NewPWNULL); %>");
		  cntrl1.focus();
		  return false;
		}
		else if (pas2!="")
		{
		  alert("<% write(InputPWTwiceDiff); %>");
		  cntrl1.focus();
		  return false;
		}
		else
			return true;
	}
	else if(pas1 != pas2)
	{
		alert("<% write(NewPWSurePWDiff); %>");
		cntrl1.focus();
		return false;
	}
	else if (pas1.indexOf("%")!=-1 || pas1.indexOf("\"")!=-1 || pas1.indexOf("\'")!=-1 || pas1.indexOf("\\")!=-1 ||��pas1.indexOf(" ")!=-1)
	{
		alert("<% write(NewPWRule); %>");
		return false;
	}
	else
		return true;
}

/* ���������� */
function checkData() {
	var tbl = WebAuthSelfHelp;
	if (userOptCnt <= 0) {
		alert("<% write(DayCantnotChange); %>");
		return false;
	}
	/* ����û��� */
	if (!checkName(tbl.userName, 31, "<% write(com_user); %>", 0)) {
		return false;
	}
	
	/* �������� */
	if (!checkName(tbl.oldPasswd, 31, "<% write(OldPW); %>", 0)) {
		return false;
	}
	
	/* ����������ȷ������ */
	if (!checkPassword(tbl.newPasswd, tbl.verifyPasswd, 31, 0)) {
	    return false;
	}
	
	return true;
}

/* ���溯�� */
function save()
{
	var tbl = WebAuthSelfHelp;
	if (checkData()) {
		tbl.submit();
	}
	return;
}

function init()
{
	if (userOptCnt == 0) {
		tishi.color = "#FF0000";
		tishi.innerText = "<% write(Tishi); %>";
	} else {
		tishi.innerText = "<% write(Tishi1); %>" + userOptCnt + "<% write(Tishi2); %>";
	}

}
init();
</script>
</body>
</html>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("ap_remote_manage");%>
<%langTran("commonWords");%>
<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
//����

<%
var parentPage=Form("parentPage") ;//�����ĸ�ҳ��
var ipStr=Form("ipStr");//Զ��ip�ִ�
var macStr=Form("macStr");//Զ��mac�ִ�
var vidStr=Form("vidStr");//Զ��vid�ִ�
var passStr=Form("passStr");//Զ�������ִ�
var optType=Form("optType");//��������
var dhcpEn=Form("dhcpEn");//��������
var devType=Form("devType");//�豸�ͺ�
var szName=Form("szName");//�豸���ڷ�����
var managedEn=Form("managedEn");//�Ƿ�ɹ���
if ((parentPage=="apManage")|| (parentPage=="DrawingPreview") || (optType=="verifyPassword"))
{
	aspSignalRemoteAuth();//Զ��������֤����������������֤����
	if ((parentPage=="apManage") && (optType=="reboot"))
	{
		aspRebootRemote();//������c������
	}
}
else if (optType=="reboot") 
{
	aspRebootRemote();//������c������
} 
else if (optType=="reset")
{
	aspResetRemote();//�ָ��������ã�c������
}
aspOutSysInfoRemote();//��ӡ�豸��Ϣ��c������
%>
//var errorArr = new Array("","","","");//����ÿ���������������ӡ
var ipStr="<%write(ipStr);%>";//Զ��ip�ִ�
var macStr="<%write(macStr);%>";//Զ��mac�ִ�
var passStr="<%write(passStr);%>";//Զ�������ִ�
var parentPage="<%write(parentPage);%>";//���Ե�ҳ��
var optType="<%write(optType);%>";
var szName="<%write(szName);%>";
var dhcpEn="<%write(dhcpEn);%>";
var managedEn="<%write(managedEn);%>";
//end����

var errorstr="";
var close_flag = 0;
/*
var errorArr = new Array("");
var sysSerial = "12345678";
var sysVersion = "WA1800N v1.0.0-120507";
var deviceName = "testAP";
var productType = "WA1800A";
var ipStr="200.200.202.124";//Զ��ip�ִ�
var macStr="0022AAB941B5";//Զ��mac�ִ�
var passStr="admin";//Զ�������ִ�
var parentPage="apManage.asp";//���Ե�ҳ��
var optType="";
*/
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
		//alert(macStr);
	closeWin(_("<%write(lawless);%>"));
}
if((parentPage=="apManage")||(parentPage=="DrawingPreview") || (optType == "verifyPassword"))
{
	if(trimstr(errorstr)!="")
	{
		if ((parentPage=="apManage") || (parentPage=="DrawingPreview"))
		{
		}
		else
		{
			closeWin(errorstr);
		}
	}
	else if ((parentPage=="apManage") && (optType=="reboot"))
	{
		close_flag=1;
		window.close();
	}
}
else if(optType!='')
{
	//alert(optType);
	if(errorstr.indexOf(_("passwordError"))>=0)
	{
		closeWin(errorstr);
	}
}
if((optType!='')&&(optType != "verifyPassword"))//�Ǳ�ҳ���������������(���� �� �ָ���������)
{
	if(errorstr=='')
	alert(_("<%write(lawfull);%>")); 
}
var secondTitle=_("<%write(APRemoteMG);%>");
var MAC='';
for(var i=0;i<macStr.length;i+=2) MAC=MAC+macStr.slice(i,i+2)+'-';
MAC=MAC.slice(0,-1);
function init()
{  
    controlWin(760,530,1);
	AP_REMOTECONF.passStr.value = passStr;
	AP_REMOTECONF.ipStr.value= ipStr;
	AP_REMOTECONF.macStr.value= macStr;
	AP_REMOTECONF.szName.value= szName;
	AP_REMOTECONF.serialNo.value = sysSerial;
	AP_REMOTECONF.managedEn.value= managedEn;
	if((managedEn == 0)&&(managedEn != ""))
	{
	document.getElementById("joinSVC_td").style.display="none";	
	document.getElementById("getSoft_td").style.display="none";	
	document.getElementById("apMacFilter").style.display="none";	
	document.getElementById("table2").style.display="none";	
	document.getElementById("outMode_td").style.display="none";	
	document.getElementById("outSoft_td").style.display="none";	
	document.getElementById("reboot_td").style.display="none";	
	document.getElementById("reConfig_td").style.display="none";	
	}else{
	document.getElementById("joinSVC_td").style.display="";	
	document.getElementById("getSoft_td").style.display="";	
	document.getElementById("apMacFilter").style.display="";	
	document.getElementById("table2").style.display="";	
	document.getElementById("outMode_td").style.display="";	
	document.getElementById("outSoft_td").style.display="";	
	document.getElementById("reboot_td").style.display="";	
	document.getElementById("reConfig_td").style.display="";	
	}
	if (((parentPage=="apManage") || (parentPage=="DrawingPreview"))&&(trimstr(errorstr) == "�������"))
	{
		document.getElementById("DivS").style.display="";
		document.getElementById("tableInfo").style.display="none";
	}
	else
	{
		document.getElementById("DivS").style.display="none";
		document.getElementById("tableInfo").style.display="";
	}
	window.document.title+=" / "+ipStr;
	
	if (close_flag==1)
	{
		cancel();
	}
}

function manageAPOk(){
	var page="ap_remote_manage.asp";
	
	if (optType=="reboot")
	{
		AP_REMOTECONF.parentPage.value="apManage";
		AP_REMOTECONF.optType.value="reboot";
	}
	else
	{
		AP_REMOTECONF.optType.value="verifyPassword";//��̨���ݸ�ֵȷ����ʲô����
	}
	AP_REMOTECONF.passStr.value = document.getElementById("getPassword").value;
	AP_REMOTECONF.ipStr.value=ipStr;
	AP_REMOTECONF.macStr.value=macStr;
	AP_REMOTECONF.devType.value=devType;
	AP_REMOTECONF.szName.value= szName;
	AP_REMOTECONF.managedEn.value= managedEn;
	AP_REMOTECONF.action=page;

	AP_REMOTECONF.submit();	
}

function save(opt)
{
	var page="ap_remote_manage.asp";
	if(opt == "reboot")//�����豸
	{
		if(!confirm(_("<%write(rebootAPTips);%>"))) return;
	}
	else if(opt == "reset")//�ָ���������
	{
		if(!confirm(_("<%write(reConfigTips);%>"))) return;
	}
    else if(opt =="getApConf" || opt =="getApSoft")
	{
		page = "ap_wait_remote_conf.asp" ;
	}
	else
	{
		page = opt ;//ֱ�Ӵ�ҳ��
		opt = "";//������֪����ʲô����
	}
	AP_REMOTECONF.optType.value=opt;//��̨���ݸ�ֵȷ����ʲô����
	AP_REMOTECONF.devType.value=devType;
	AP_REMOTECONF.szName.value= szName;
	AP_REMOTECONF.managedEn.value= managedEn;
	AP_REMOTECONF.action=page;
//	alert(AP_REMOTECONF.optType.value);
//	return;
	AP_REMOTECONF.submit();
}

/**ȡ����ť**/
function cancel(){
	//	document.getElementById("DivS").style.display="none";
    if (navigator.userAgent.indexOf("MSIE") > 0) {
        if (navigator.userAgent.indexOf("MSIE 6.0") > 0) {
            window.opener = null;
            window.close();
        } else {
            window.open('', '_top');
            window.top.close();
        }
    }
    else if (navigator.userAgent.indexOf("Firefox") > 0) {
        window.location.href = 'about:blank ';
    } else {
        window.opener = null;
        window.open('', '_self', '');
        window.close();
    }
}
 


</script>
</head>
<body onLoad="init()" onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">

<!--
<div id="DivS" style="display:none; position: absolute; z-index: 9999; width: 250px; height: 100px; top: 120px; left: 250px; background-color: rgb(255, 255, 255); padding: 5px; background-position: initial initial; background-repeat: initial initial;">
 <div id="tbPwd"  style="position:absolute; z-index:99999; " border="0" cellpadding="0" cellspacing="0" style="margin:0; padding:0;">
	<div class="style21" style="background-color:#008893; width:250px; line-height:25px; font-size:12px; color:#fff;">
	<div style="width:130px; text-align:left; float:left;color:#fff;" >&nbsp;��������</div>&nbsp;
	<div style="width:100px; text-align:right; float:right;color:#fff;"><a style=" color:#fff; text-decoration:none;" href="javascript:close();">x</a>&nbsp;&nbsp;</div>
	</div>
    <div style="clear:both"></div>
    <div style="line-height:45px; font-size:12px"><label height="31" colspan="2"  align="center">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;������:
      <input type="password" size="13" value=""  class="input" name="getPassword" id="getPassword"></span>&nbsp;&nbsp;<label id="errorS" style="color:red;"></label>
      <input type="hidden" id="img_id" name="img_id" value="" />
      </div>
    <div  style="line-height:30px; font-size:12px; text-align:center;"><span colspan="2" align="center"><input name="input" type="button" class="input-button" onClick="manageAPOk()" value="ȷ��" />
      <input name="" type="button" class="input-button"  value="ȡ��" onClick="cancel()" /></span></div>
</div>
</div>
-->
<div id="DivS" style="display:none; position: absolute; z-index: 9999; width: 250px; height: 100px; top: 120px; left: 250px; background-color: rgb(255, 255, 255); border:1px solid #f2f2f2;font-size:12px; ">
 <div id="tbPwd">
	<div  style="background-color:#008893;padding-top:5px;padding-bottom:5px;">
		<div style="text-align:left; color:#fff;" >&nbsp;��������</div>
		<div style="width:50px; text-align:right;color:#fff;position:absolute;right:5px;top:5px;border:1px soild red;z-index:1000;">
			<a style=" color:#fff; text-decoration:none;" href="javascript:cancel();">x</a>
		</div>
	</div>
  
    <div style="font-size:12px;padding-top:10px;padding-bottom:10px;"><label height="31" colspan="2"  align="center">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;������:
      <input type="password" size="13" value=""  class="input" name="getPassword" id="getPassword"></span>&nbsp;&nbsp;<label id="errorS" style="color:red;"></label>
      <input type="hidden" id="img_id" name="img_id" value="" />
      </div>
    <div  style="font-size:12px; text-align:center;"><span colspan="2" align="center"><input name="input" type="button" class="input-button" onClick="manageAPOk()" value="ȷ��" />
      <input name="" type="button" class="input-button"  value="ȡ��" onClick="cancel()" /></span></div>
</div>
</div>

<table id="tableInfo" style="display:none" width="100%" border="0" cellspacing="0" cellpadding="0">
   <!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****�м�����****-->
  <tr>
   <td width="15"></td>
  <!--////����-->
 <td><script language="javascript">showSecondTitle(secondTitle);//��ʾҳ������</script>
 
  <table width="100%" border="0" height="330" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
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
    <form name="AP_REMOTECONF" id="AP_REMOTECONF" method="post">
    <input type="hidden" value="Apply" name="_submit">
    <input type="hidden" value="" name="parentPage">
    <input type="hidden" value="" name="optType">
    <input type="hidden" value="" name="devType">
    <input type="hidden" value="" name="ipStr">
    <input type="hidden" value="" name="macStr">
    <input type="hidden" value="" name="szName">
    <input type="hidden" value="" name="passStr">
    <input type="hidden" value="" name="serialNo">
    <input type="hidden" value="" name="managedEn">
     <table width="600" cellpadding="0" cellspacing="0">
    <tr><td width="273" id="APRemoteMSG_td"><b><%write(APRemoteInfo);%></b></td>
    </tr>
    <tr height="8"><td></td></tr>
    </table>
    <TABLE  cellSpacing=1  width="600" cellPadding=4  class="style22">
      <tr class="style18">
        <td width="78" height="27" class=vncell id="devName_td"><%write(devName);%></td>
        <td width="171"><script language="javascript">document.write(deviceName);</script></td>
        <td width="68" class=vncell id="type_td"><%write(model);%></td>
        <td width="254"><script language="javascript">document.write(ProductID);</script></td>
      </tr>
      <tr>
        <td class=vncell height="27" id="serialNum_td"><%write(serialNum);%></td>
        <td><script language="javascript">document.write(sysSerial);</script></td>
        <td class=vncell id="ipAddr_td"><%write(ipAddr);%></td>
        <td><script language="javascript">document.write(ipStr);</script></td>
      </tr>
      <tr class="style18">
        <td class=vncell height="27" id="macAddr_td"><%write(macAddr);%></td>
        <td><script language="javascript">document.write(MAC);</script></td>
        <td class=vncell height="27" id="softVer_td"><%write(softVer);%></td>
        <td><script language="javascript">document.write(sysVersion);</script></td>
      </tr>
    </TABLE>
    <table width=600 border="0" cellspacing="0" cellpadding="0" align="center">
<tr height="25"><td></td></tr>
<tr height="1"><td background="images/dot.gif"><img src="images/space.gif" width="8" height="1"></td></tr>
<tr height="18"><td></td></tr>
</table>
    <br>
    <table width="600" border="0"  cellpadding="4" cellspacing="1" class="style22">
    <tr>
      <td colspan="4" align="left" id="APRemoteMG_td"><b><%write(APRemoteMG);%></b></td>
      </tr>
      <tr class="style18">
        <td width="150"><a href="javascript:save('apBasicConf.asp')" id="baseSET_td"><%write(baseSet);%></a></td>
        <td width="150"><a href="javascript:save('serviceZone.asp')" id="joinSVC_td" style="display:none"><%write(joinSZ);%></a></td>
        <td width="150"><a href="javascript:save('getApSoft')" id="getSoft_td" style="display:none"><%write(getSoft);%></a></td>
<!--        <td><a href="javascript:save('getApConf')" id="getConfig_td"><%write(getConf);%></a></td>-->
        <td width=150><a href="javascript:save('pushApMacFilter.asp')" id="apMacFilter" style="display:none"><%write(apMacFilter);%></a></td>
        </tr>
      <tr id="table2" style="display:none">
<!--      <td width="140"><a href="javascript:save('pushConf.asp')" id="outConf_td"><%write(outConf);%></a></td> -->
      <td><a href="javascript:save('pushTmp.asp')" id="outMode_td" style="display:none"><%write(outMode);%></a></td>
      <td><a href="javascript:save('pushSoft.asp')" id="outSoft_td" style="display:none"><%write(outSoft);%></a></td>
      <td><a href="javascript:save('reboot')" id="reboot_td" style="display:none"><%write(reboot);%></a></td>
      <td><a href="javascript:save('reset')" id="reConfig_td" style="display:none"><%write(reConfig);%></a></td>
      </tr>
</table>   
    </form>               
    <!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
	  </td>
	  <td class="gapTd"></td> </tr>
	  <tr>
	<td class="gapTd"></td>
	<td class="gapTd"></td>
	</tr>
	  </table>
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

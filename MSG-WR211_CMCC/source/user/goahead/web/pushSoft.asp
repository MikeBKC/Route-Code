<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("pushSoft");%>
<%langTran("commonWords");%>
<title><%write(UnitOutSoft);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script src="switch/js/common_switch.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
 //����begin
<% 
	var waitOver=Form("waitOver");
	var isGroupOpt=Form("isGroupOpt");
	var devType=Form("devType");
	aspOutRemoteAPSoft();//��ӡ�����Ϣ�������ˣ�ֻ��ʾ���ʺϸ�AP����Ŀ
	/*
	write('var softwareName = new Array("123.bin", "321.bin");\n');
	write('var softwareVersion = new Array("V15", "V16");\n');
	write('var typesForFile = new Array("510W", "WA1800N");\n');
	write('var firmwareVersion = new Array("V1.0", "V2.0");\n');
	write('var fileSize = Array("1.5MB", "3.0MB");\n');
	write('var totalrecs = 2;\n');
	write('var macStr = "0022aaaedd34";\n');
	write('var ipStr = "200.200.202.125";\n');
	write('var passStr = "admin";\n');
	write('var errorArr = new Array("");\n');
	write('var errorstr="";\n');
	*/
%>
var isGroupOpt="<%write(isGroupOpt);%>";
var waitOver="<%write(waitOver);%>";
var devType="<%write(devType);%>";
//end����

var urcpError="";
var radarray=new Array();
for(var i=0;i<totalrecs;i++)
{
	radarray[i]='<input type="radio" name="radConfSrc" value="'+softwareName[i]+' '+typesForFile[i]+'">';
}

var titles1 = new Array();
titles1[0] = _("<%write(chooseSoft1);%>");
titles1[1] = _("<%write(softName);%>");
titles1[2] = _("<%write(softVersion);%>");
titles1[3] = _("<%write(suitType);%>");
titles1[4] = _("<%write(hardVersion);%>");
titles1[5] = _("<%write(softSize);%>");

currpage[0] = 1;	//��ǰҳ
recscount[0] = totalrecs;	//��ǰ��Ŀ��
maxrecs[0] = 24;	//�����Ŀ��
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] = _("<%write(outSoft);%>");
sortable[0] = new Array(0, 1, 1, 1, 1, 1);
sortType[0] = new Array(TYPE_STR, TYPE_STR, TYPE_STR, TYPE_STR, TYPE_STR, TYPE_NUM);
arrayCount[0] = 6;//�ܹ�������(����ָ���Ǹ�������仯����)
recsPerPage[0] = 10; //ÿҳ����������
lineButton[0] = 0;
var arrayData1 = new Array(radarray, softwareName, softwareVersion, typesForFile, firmwareVersion, fileSize);
var oldDspList=dspList;

/**
��д�����ʾdspList����
Ϊ�������µ�����
**/
dspList=function()
{
	/*ԭ������*/
	oldDspList.apply(this,arguments);//�ȼ���oldDspList(arugumens[0],arguments[1]...arguments[argumens.length-1])
	//Ĭ��ѡ�е�һ��
	if(totalrecs>0)
	{
		frame12.document.getElementById('tableList').rows[1].cells[0].childNodes[0].checked = true;
	}
}

function dsplist(index, list) {
    if (index == 0) dspList(index, list, -1, titles1, titles1.length, arrayData1, 0, 0);
}
function init() {
	frame11.location.href = "_listhead1.htm"; //ע������ڴ˴���ʼ��������ҳ��ˢ�µ�ʱ����ܳ���bug
	frame12.location.href = "_list1.htm"; //���������ˢ�� ҳ����������Ƿ�����
	pushSoft.passStr.value = passStr;
	pushSoft.ipStr.value= ipStr;
	pushSoft.macStr.value= macStr;
	pushSoft.szName.value= szName;
	pushSoft.managedEn.value= managedEn;
	pushSoft.isGroupOpt.value= isGroupOpt;
	if(!isGroupOpt)//��������
	{
		window.document.title = _("<%write(UnitOutSoft);%>");
		window.document.title+=" / "+ipStr;
		if(waitOver != "" && urcpError=="") alert(_("<%write(lawfull);%>"));//�û��޷�ֱ�Ӵӽ����Ͽ��������������ʾ
	}
	else if(waitOver != "")
	{
		if(!showBatchResult(errorArr,ipStr,macStr,_("<%write(failTips);%>"),1))
		{
			alert(_("<%write(lawfull);%>"));
		}
	}
}
function init2() {
  	frame11.location.href = "_listhead1.htm"; //ע������ڴ˴���ʼ��������ҳ��ˢ�µ�ʱ����ܳ���bug
    frame12.location.href = "_list1.htm"; //���������ˢ�� ҳ����������Ƿ�����
	pushSoft.passStr.value = passStr;
	pushSoft.ipStr.value= ipStr;
	pushSoft.macStr.value= macStr;
	pushSoft.szName.value= szName;
	pushSoft.managedEn.value= managedEn;
	pushSoft.isGroupOpt.value= isGroupOpt;
}
function reSet()
{
	pushSoft.reset();
	init2();
}
function save()
{
	var obj,i;
	var tmp;
		pushSoft.remoteSrcSoft.value="";
		obj = frame12.document.getElementsByName('radConfSrc');
		for(i=0;i<obj.length;i++)
		{
			if(obj[i].checked)
			{
				tmp = obj[i].value.split(" ");
				pushSoft.remoteSrcSoft.value = tmp[0];//�ô����ؿؼ���¼����Դ�����ļ���
				pushSoft.web_type.value = devType;
			}
		}
		if(!confirm(_("<%write(continueTips);%>")))
		{
			return;
		}
	if (!softwareName[0])
	{
		alert(_("<%write(chooseFile);%>"));
		return;
	}
//	alert(pushSoft.web_type.value);
//	return;
	pushSoft.action="ap_wait_remote_conf.asp";
	pushSoft.submit();
}
function go_back()
{
	if(isGroupOpt)
		pushSoft.action="ap_remote_manage_group.asp";
	else
		pushSoft.action="ap_remote_manage.asp";
	pushSoft.devType.value= devType;
	pushSoft.optType.value="";//��գ���ֹ���ص�ҳ�������
	pushSoft.submit();
}
</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<script language="javascript">
	urcpErrorPost("ap_remote_manage.asp");
</script>
<table width="100%" border="0" cellspacing="0" cellpadding="0">
   <!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****�м�����****-->
  <tr>
   <td width="15"></td>
  <!--////����-->
  <td><script language="javascript">showSecondTitle(_("<%write(outSoft);%>"));//��ʾҳ������</script>
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
	<table border="0" width="80%" align='center'>	<tr><td class="gapTd"></td><td align="center">
	<!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<br>
<form name="pushSoft" method="post">
<input type="hidden" name="delstr" value="">
<input type="hidden" name="statustr">
<input type="hidden" value="Apply" name="_submit">
<input type="hidden" value="" name="ipStr">
<input type="hidden" value="" name="macStr">
<input type="hidden" value="" name="szName">
<input type="hidden" value="" name="managedEn">
<input type="hidden" value="" name="passStr">
<input type="hidden" value="" name="isGroupOpt">
<input type="hidden" value="" name="oldName">
<input type="hidden" value="" name="remoteSrcSoft">
<input type="hidden" value="sendAPSoft" name="optType">
<input type="hidden" value="" name="web_type">
<input type="hidden" value="" name="devType">
<TABLE  width="600"  border="0"  cellpadding="0" cellspacing="0"   align="center">
<tr><td colspan="2" height="10"></td></tr>
</TABLE>
<table width="100%"  border="0" cellspacing="0" cellping="0">
<tr>
<td ><table width="600" border="0" align="center" cellpadding="0" cellspacing="0">

<tr><td height="" colspan="2" align="center" valign="bottom" >
	<iframe  scrolling="no" name="frame11" valign="bottom" align="center" width="600"  height="44" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden> </iframe>	
</td></tr>
<tr>
<td colspan="2" align="center" valign="top">
<iframe scrolling="no" name="frame12" id="framelist" valign="top" align="center" width="600" height="248" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe>
</td></tr>
<tr><td>
</td>
</tr>
</table>
<table width="600" align="center"  cellpadding="0" cellspacing="0">
<tr>
  <td height="21" align="center" valign="top" nowrap id="">
    <input type="button" name="button" id="outbutton" value="<%write(sureout);%>" onClick="save()" class="input-button">
    <input name=""  type="button" value="<%write(refill);%>" class="input-button" onClick="reSet()" id="resetButton">
    <input type="button" class="input-button" value="<%write(help);%>" name="input" onClick="window.open('<% write(com_helppage); %>#pushSoft')" id="helpButton">
    <input type="button" class="input-button" name="goBack" value="<%write(back);%>" onClick="go_back();" id="backButton">
  </td>
  </tr>
</table>
</tr>
</table>
</form>
	<!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
 </td><td class="gapTd"></td></tr> </table> </td>
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

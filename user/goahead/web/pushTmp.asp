<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("pushTmp");%>
<%langTran("commonWords");%>
<title><%write(UnitOutTemp);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script src="switch/js/common_switch.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
 //����begin
<% 
	aspOutRemoteAPTemp();//��������ģ����Ϣ�Ĺ��ˣ�ֻ��ʾ�����ڸ�AP��ģ��
	var waitOver=Form("waitOver");
	var isGroupOpt=Form("isGroupOpt");
	var devType=Form("devType");
    /*
	write('var outerFlashTotalSpace = "";\n');
	write('var tmpname = new Array("temp_1", "temp_2");\n');
	write('var types = new Array("510W", "WA1800N");\n');
	write('var firmVersion = new Array("V1.0", "V2.0");\n');
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
if(!isGroupOpt && waitOver != "")
{
	urcpError = errorArr[0];//��������
}
if(macStr == "")
{
	closeWin(_("<%write(lawless);%>"));
}
var radarray=new Array();
for(var i=0;i<totalrecs;i++)
{
	radarray[i]='<input type="radio" name="radConfSrc" value="'+tmpname[i]+' '+wlRates[i]+'">';
}


var titles1 = new Array();
titles1[0] = _("<%write(chooseMode);%>");
titles1[1] = _("<%write(modeName);%>");
titles1[2] = _("<%write(wlFre);%>");
titles1[3] = _("<%write(wlMode);%>");
titles1[4] = _("<%write(wlRate);%>");

currpage[0] = 1;	//��ǰҳ
recscount[0] = totalrecs;	//��ǰ��Ŀ��
maxrecs[0] = 24;	//�����Ŀ��
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] = _("<%write(outMode);%>");
sortable[0] = new Array(0, 1, 1, 1,1);
sortType[0] = new Array(TYPE_STR, TYPE_STR, TYPE_STR, TYPE_STR, TYPE_STR);
arrayCount[0] = 5;//�ܹ�������(����ָ���Ǹ�������仯����)
recsPerPage[0] = 10; //ÿҳ����������
lineButton[0] = 0;
var arrayData1 = new Array(radarray, tmpname, wlFres, wlModes, wlRates);
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
	pushTmp.passStr.value = passStr;
	pushTmp.ipStr.value= ipStr;
	pushTmp.macStr.value= macStr;
	pushTmp.szName.value= szName;
	pushTmp.managedEn.value= managedEn;
	pushTmp.isGroupOpt.value= isGroupOpt;
	if(!isGroupOpt)//��������
	{
		window.document.title = _("<%write(UnitOutTemp);%>");
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
	pushTmp.passStr.value = passStr;
	pushTmp.ipStr.value= ipStr;
	pushTmp.macStr.value= macStr;
	pushTmp.szName.value= szName;
	pushTmp.managedEn.value= managedEn;
	pushTmp.isGroupOpt.value= isGroupOpt;
}

function reSet()
{
	pushTmp.reset();
	init2();
}
function getDevRate()
{
	if((devType == "WA500N")||(devType == "WA501N")||(devType == "WA1500N")||(devType == "WA1000N"))
	{
		return 1; /*150M*/
	}else if ((devType == "WA1700N")||(devType == "WA1800N")||(devType == "WA1800NV3")||(devType == "WA1900N")||(devType == "WA2000N")||(devType == "WA2200N"))
	{
		return 2;/*300M*/
	}
}
function nowRate(wlRate)
{
	if(wlRate == "150M")
	{
		return 1;/*150M*/
	}else if(wlRate == "300M")
	{
		return 2;/*300M*/
	}else
	{
		return 0;/*11M,54M,�Զ�*/
	}

}

function isRateValied(wlRate)
{
	var rate = getDevRate();
	if (nowRate(wlRate) <= rate)
	{
		return 1;
	}else
	{
		return 0;
	}

}

function save()
{
	var obj,i;
	var tmp;
		pushTmp.remoteSrcTemp.value="";
		obj = frame12.document.getElementsByName('radConfSrc');
		for(i=0;i<obj.length;i++)
		{
			if(obj[i].checked)
			{
				tmp = obj[i].value.split(" ");
				pushTmp.remoteSrcTemp.value = tmp[0];//�ô����ؿؼ���¼����Դ�����ļ���
				/*
				if (isRateValied(tmp[1]) != 1)
				{
					alert("ģ���������ͺŲ�ƥ��");
					return;
				}
				*/
			}
		}
/*		
		if(!confirm(_("<%write(continueTips);%>")))
		{
			return;
		}
*/		
	if (!tmpname[0])
	{
		alert(_("<%write(chooseFile);%>"));
		return;
	}
//	alert(pushTmp.remoteSrcTemp.value);
//	return;
	pushTmp.devType.value= devType;
	pushTmp.action="ap_wait_remote_conf.asp";
	pushTmp.submit();
}
function go_back()
{
	if(isGroupOpt)
		pushTmp.action="ap_remote_manage_group.asp";
	else
		pushTmp.action="ap_remote_manage.asp";
	pushTmp.devType.value= devType;
	pushTmp.optType.value="";//��գ���ֹ���ص�ҳ�������
	pushTmp.submit();
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
  <td><script language="javascript">showSecondTitle(_("<%write(outMode);%>"));//��ʾҳ������</script>
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
<form name="pushTmp" method="post">
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
<input type="hidden" value="" name="remoteSrcTemp">
<input type="hidden" value="sendAPTemp" name="optType">
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
    <input type="button" class="input-button" value="<%write(help);%>" name="input" onClick="window.open('<% write(com_helppage); %>#pushTmp')" id="helpButton">
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

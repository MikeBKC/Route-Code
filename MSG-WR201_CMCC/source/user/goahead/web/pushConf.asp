<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("pushConf");%>
<%langTran("list");%>
<%langTran("commonWords");%>
<title><%write(UnitOutConf);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script src="switch/js/common_switch.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
 //����begin
<%
	aspOutRemoteAPConf();//��ӡ�����ļ���Ϣ�������ˣ�ֻ��ʾ���ʺϸ�AP����Ŀ
	var waitOver=Form("waitOver");
	var isGroupOpt=Form("isGroupOpt");
	var serialNo=Form("serialNo");
	var devType=Form("devType");
	/*
	write('var outerFlashTotalSpace = "";\n');
	write('var namearray = new Array("configWA1800N_00-22-AA-AE-DD-D5_20110101_015018.txt", "configWA1800N_00-22-AA-AE-DD-D5_20110101_003549.txt");\n');
	write('var statuarray = new Array("0", "0");\n');
	write('var filesizearray = new Array("890B", "890B");\n');
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
var serNo="<%write(serialNo);%>";
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
for(var i=0;i<namearray.length;i++)
{
	radarray[i]='<input type="radio" name="radConfSrc" value="'+namearray[i]+' '+statuarray[i]+'">';
}
var titles1 = new Array(); //ע��˴�������Ϊtitles1(Ϊ�����б��еı�����Ӧ)
titles1[0] = _("<%write(sureConfig);%>");
titles1[1] = _("<%write(confName);%>");
titles1[2] = _("<%write(size);%>");
//titles1[3] = "�汾";

currpage[0] = 1;
recscount[0] = totalrecs;
maxrecs[0] = totalrecs;
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] = _("<%write(configList);%>");
sortable[0] = new Array(0,1, 1);
sortType[0] = new Array(TYPE_STR , TYPE_CHINESE, TYPE_STR);
arrayCount[0] = 3;//�ܹ�������(����ָ���Ǹ�������仯����)
recsPerPage[0] = 10; //ÿҳ����������
lineButton[0] = 0;
var arrayData1 = new Array(radarray,namearray,filesizearray);
var oldDspList=dspList;
var recbegin=0,recend=0;

function dsplisthead(index, listhead, arrayName) {
    var arrayIndex = 1;
    if (arrayName == null) {
        arrayName = "arrayData1";
    }
    if (index == 0) {
        arrayName = "arrayData1";
    }
    if (index == 1) {
        arrayIndex = 2;
        arrayName = "arrayData2";
    }

    var pages;
    var strPages;
    var strRecs;
    if (recscount[index] == 0) strPages = "0/0";
    else {
        pages = Math.floor((recscount[index] - 1) / recsPerPage[index]) + 1; //Ϊ�˱���recscount[index]��ֵ��recsPerPage[index]��������������recscount[index]-1��
        strPages = currpage[index].toString() + "/" + pages.toString();
    }
    strRecs = recscount[index].toString();
    out_head(listhead);
    listhead.document.write('<table width="100%" height="20"	 border="0" cellpadding="0" cellspacing="0">');

    listhead.document.write('  <tr>');
    listhead.document.write('    <td><table width="100%" height="20" border="0" cellspacing="0" cellpadding="0" bgcolor="">');
    listhead.document.write('      <tr>');
    //listhead.document.write('	 <td width="11" ></td>');
    listhead.document.write('	 <td width="226" background="images/tablehead.gif" style="background-repeat:no-repeat"><img src="images/space.gif" width="10" height="8"><span class="style4">' + listheadcaption[index] + '</span></td>');
    //listhead.document.write('	 <td width="26"></td>');
    listhead.document.write('	 <td   width="324"align="right"  class="style5" ><img src="images/space.gif" width="10" height="8"></td><td width="50" align="right"  >' + strRecs + '</td>');
    listhead.document.write('      </tr>');
    listhead.document.write('    </table></td>');
    listhead.document.write('  </tr>');

    listhead.document.write('  <tr class="style18">');
    listhead.document.write('    <td height="24" colspan="4"  ><table width="600"  border="0" cellspacing="0" cellpadding="0">');
    listhead.document.write('      <tr>');
    listhead.document.write('	 <td width="10"><img src="images/space.gif" width="10" height="8"></td>');
    listhead.document.write('	 <td width="25" valign="bottom" class="style6">' + strPages + '</td>');

    if (currpage[index] == 1) listhead.document.write('	 <td width="40" valign="bottom" class="ablacks"><%write(firstPage);%></td>');
    else listhead.document.write('	 <td width="40" valign="bottom"><a href="javascript:parent.firstpage(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')" class="ablacks"><%write(firstPage);%></a></td>');
    if (currpage[index] <= 1) listhead.document.write('	 <td width="40" valign="bottom" class="ablacks"><%write(prePage);%></td>');
    else listhead.document.write('	 <td width="40" valign="bottom"><a href="javascript:parent.prepage(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')" class="ablacks"><%write(prePage);%></a></td>');

    if (currpage[index] * recsPerPage[index] < recscount[index]) listhead.document.write('	    <td width="40" valign="bottom"><a href="javascript:parent.nextpage(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')" class="ablacks"><%write(nextPage);%></a></td>');
    else listhead.document.write('	 <td width="40" valign="bottom" class="ablacks"><%write(nextPage);%></td>');
    if ((recscount[index] < 1) || (currpage[index] == Math.floor((recscount[index] - 1) / recsPerPage[index]) + 1)) //?
    listhead.document.write('	 <td width="40" valign="bottom" class="ablacks"><%write(lastPage);%></td>');
    else listhead.document.write('	 <td width="40" valign="bottom"><a href="javascript:parent.lastpage(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')"  class="ablacks"><%write(lastPage);%></a></td>');
    listhead.document.write('	 <td width="30" valign="bottom"><a href="javascript:parent.goanotherpage(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')" class="ablacks"><%write(go);%></a></td>');
    listhead.document.write('	 <td width="15" valign="bottom"><span class="style6"><%write(the);%></span></td>');
    listhead.document.write('	 <td width="10" valign="bottom" class="style6"><input name="pagenumber" type="text" class="input" onKeyPress="javascript:parent.gotopage(event,' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')"  size="1" maxlength="3"></td>');
    listhead.document.write('	 <td width="10" valign="bottom" class="style6">&nbsp;&nbsp;<%write(page);%></td>');
    listhead.document.write('	 <td width="25" valign="bottom" class="style6">&nbsp;</td>');


    listhead.document.write('	 <td width="35" valign="bottom"><span class="style6"><%write(search);%></span></td>');
    listhead.document.write('	 <td width="55 " valign="bottom" class="style6"><input name="txtSearch" type="text" class="input" onKeyPress="javascript:parent.searchDataEx(event,' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ',parent.' + arrayName + ',this.value,' + arrayIndex + ')"	size="1" maxlength="60"  style="width:100px"></td>');
    listhead.document.write('      </tr>');
    listhead.document.write('    </table></td>');
    listhead.document.write('  </tr>');
    listhead.document.write('</table>');

    out_tail(listhead);
    if (allcheckobj[index] != null) //�����е�checkboxȡ��ѡ��
    allcheckobj[index].checked = false;
    //chgLsHeadColor(listhead);
}

/**
��д�����ʾdspList����
Ϊ�������µ�����
**/
dspList=function()
{
	/*ԭ������*/
	oldDspList.apply(this,arguments);//�ȼ���oldDspList(arugumens[0],arguments[1]...arguments[argumens.length-1])
	//Ĭ��ѡ�е�һ��
	if(namearray.length>0)
	{
		frame12.document.getElementById('tableList').rows[1].cells[0].childNodes[0].checked = true;
	}
}
function dsplist(index, list) {
    if (index == 0) dspList(index, list, -1, titles1, titles1.length, arrayData1,0, 0);
}
function init() {
  	frame11.location.href = "_listhead1.htm"; //ע������ڴ˴���ʼ��������ҳ��ˢ�µ�ʱ����ܳ���bug
    frame12.location.href = "_list1.htm"; //���������ˢ�� ҳ����������Ƿ�����
	CONF.passStr.value = passStr;
	CONF.ipStr.value= ipStr;
	CONF.macStr.value= macStr;
	CONF.isGroupOpt.value= isGroupOpt;
	CONF.serNo.value= serNo;
	if(!isGroupOpt)//��������
	{
		window.document.title = _("<%write(UnitOutConf);%>");
		window.document.title+=" / "+ipStr;
		if(waitOver != "" && urcpError=="") alert(_("<%write(lawfull);%>"));//�û��޷�ֱ�Ӵӽ����Ͽ������������ʾ
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
	CONF.passStr.value = passStr;
	CONF.ipStr.value= ipStr;
	CONF.macStr.value= macStr;
	CONF.isGroupOpt.value= isGroupOpt;
	CONF.serNo.value= serNo;
	CONF.devType.value= devType;
}
/*Դ�����ļ�ģʽѡ�񣨱��������������ļ���
function radMode()
{
	var obj=document.getElementById("confListTb");
	if(CONF.srcMode[0].checked) obj.style.display="none";
	else
	{
		try
		{
			obj.style.display="table";
		}
		catch(e)
		{
			obj.style.display="block";
		}
	}
}
*/
/*�·���ʽѡ���ֶ��򸲸ǣ�*/
function selMode()
{
	var selObj=CONF.sendMode;
	if(selObj.value != 2)
	{
		document.getElementById('tbDiyConf').style.display="none";
	}
	else
	{
		try
		{
			document.getElementById('tbDiyConf').style.display="table";//ie8�����Լ�ff֧��
		}
		catch(e)
		{
			document.getElementById('tbDiyConf').style.display="block";//ie7�Լ����°汾֧��
		}
	}
}
function reSet()
{
	CONF.reset();
	init2();
	//radMode();
	//selMode();
	//init2();
}
function save(opt)
{
	var obj,i;
	var tmp;
	/*
	if(CONF.srcMode[1].checked)//���������ļ�
	{
	*/
		CONF.remoteSrcConf.value="";
		CONF.apTypes.value="";
		obj = frame12.document.getElementsByName('radConfSrc');
		for(i=0;i<obj.length;i++)
		{
			if(obj[i].checked)
			{
				tmp = obj[i].value.split(" ");
				CONF.remoteSrcConf.value = tmp[0];//�ô����ؿؼ���¼����Դ�����ļ���
				CONF.statustr.value = tmp[1];//�ô����ؿؼ���¼�����ļ��Ƿ񱣴浽flash
				CONF.apTypes.value = types[i];//�ô����ؿؼ���¼����Դ�����ļ���
			}
		}
	/*
	}
	if(CONF.sendMode[1].selected)
	{
		i = CONF.sysConf.checked || CONF.vlanConf.checked || CONF.aggrConf.checked ||
			CONF.portConf.checked || CONF.rateLimitConf.checked || CONF.arpConf.checked;
		if(!i)
		{
			alert(_("configOption"));
			return;
		}
	}
	else
	{
		if(!confirm(_("<%write(continueTips);%>")))
		{
			return;
		}
	}
	*/
	if (!namearray[0])
	{
		alert(_("<%write(chooseFile);%>"));
		return;
	}

    if( CONF.apTypes.value != devType ){
        alert(_("<%write(errorApTypes);%>")); 
        return;
    }
//	alert(CONF.apTypes.value);
//	alert(CONF.remoteSrcConf.value);
//	alert(CONF.statustr.value);
//	return;
	CONF.action="ap_wait_remote_conf.asp";
	CONF.submit();
}

function go_back()
{
	if(isGroupOpt)
		CONF.action="ap_remote_manage_group.asp";
	else
		CONF.action="ap_remote_manage.asp";
	CONF.devType.value= devType;
	CONF.optType.value="";//��գ���ֹ���ص�ҳ�������
	CONF.submit();
}
</script>
</head>
<body onLoad="init();" background="images/body_main.png" style="background-repeat:repeat-x">
<script language="javascript">urcpErrorPost("ap_remote_manage.asp");</script>
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****�м�����****-->
  <tr>
   <td width="15"></td>
  <!--////����-->
  <td><script language="javascript">showSecondTitle(_("outConfig"));//��ʾҳ������</script>
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
	<table border="0" width="80%" align='center'>
	  <tr>
	    <td align="center" height="5"></td>
	    </tr>
	  <tr><td class="gapTd"></td><td align="center">
	<!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
    <br>
    <form name="CONF" method="POST">
    <input type="hidden" value="Apply" name="_submit">
<input type="hidden" value="" name="ipStr">
<input type="hidden" value="" name="macStr">
<input type="hidden" value="" name="passStr">
<input type="hidden" value="" name="isGroupOpt">
<input type="hidden" value="" name="remoteSrcConf">
<input type="hidden" value="" name="apTypes">
<input type="hidden" value="sendAPConf" name="optType">
<input type="hidden" name="statustr">
<input type="hidden" name="serNo">
<input type="hidden" value="" name="devType">
<!--
<table width="600">
<tr><td width="93" title="��ǰ�豸����ʹ�õ������ļ�">
  <input type="radio" name="srcMode" value="1"  onClick="radMode()" checked>���������ļ�
</td>
  <td width="115" title="ͨ��������������ȡ�������ڱ��豸�ģ������豸�������ļ�"><input type="radio" name="srcMode"  onClick="radMode()" value="2">
���������ļ�</td>
  <td width="376">&nbsp;</td>
</tr>
</table>
-->
<table width="600" border="0" align="center" cellpadding="0" cellspacing="0" id="confListTb" style="display:block">

<tr><td colspan="5" height="" align="center" valign="bottom" >
	<iframe  scrolling="no" name="frame11" valign="bottom" align="center" width="600"  height="44" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden> </iframe>	
</td></tr>
<tr>
<td colspan="5" align="center" valign="top">
<iframe scrolling="no" name="frame12" id="framelist" valign="top" align="center" width="600" height="248" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe>
</td></tr>
</table>
<table width="600" align="center" border="0">
  <tr height="10"><td align="center"></td></tr>
<tr height="22">
<td width="" align="center">
<input name="" type="button" value="<%write(sureout);%>" class="input-button" onClick="save()" id="sureout">
<input name=""  type="button" value="<%write(refill);%>" class="input-button" onClick="reSet()" id="resetButton">
<input type="button" class="input-button" value="<%write(help);%>" name="input" onClick="window.open('<% write(com_helppage); %>#pushConf')" id="helpButton">
<input type="button" class="input-button" name="goBack" value="<%write(back);%>" onClick="go_back();" id="backButton">
</td></tr>
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

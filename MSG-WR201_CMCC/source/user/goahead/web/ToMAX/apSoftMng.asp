<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("apSoftMng");%>
<%langTran("commonWords");%>
<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script src="switch/js/common_switch.js" type="text/javascript"></script>
<script language="javascript">
//����
<%
	var urcpSet=Form("urcpSet");
	var devType=Form("devType");
	aspOutPutRemoteApSoft();//��ӡ�����ļ���Ϣ
	/*
	write('var outerFlashTotalSpace = "";\n');
	write('var macStr = "";\n');
	write('var ipStr = "200.200.202.125";\n');
	write('var passStr = "admin";\n');
	write('var softwareName = new Array("321.bin", "123.bin", "231.bin");\n');
	write('var softwareVersion = new Array("V16", "V15", "V15");\n');
	write('var typesForFile = new Array("WA1800N", "WA1800N", "WA1800N");\n');
	write('var firmwareVersion = new Array("V2.0", "V2.0", "V1.0");\n');
	write('var fileSize = Array("3.0MB", "2.6MB", "2.0MB");\n');
	write('var defaults = new Array("yes", "0", "0");\n');
	write('var totalrecs = 3;\n');
	write('var errorArr = new Array("");\n');
	write('var errorstr="";\n');
	*/
	var waitOver=Form("waitOver");
	var isGroupOpt=Form("isGroupOpt");
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
 /* For file list */
var titles1 = new Array();
titles1[0] = _("<%write(swName);%>");
titles1[1] = _("<%write(swVersion);%>");
titles1[2] = _("<%write(model);%>");
titles1[3] = _("<%write(hwVersion);%>");
titles1[4] = _("<%write(swSize);%>");
titles1[5] = _("<%write(setDefaut);%>");
var strSecondMenuFocus=_("<%write(AdminAPSoft);%>"); 

currpage[0] = 1;	//��ǰҳ
recscount[0] = totalrecs;	//��ǰ��Ŀ��
maxrecs[0] = 24;	//�����Ŀ��
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] = _("<%write(swList);%>");
sortable[0] = new Array(1, 1, 1, 1, 1, 0);
sortType[0] = new Array(TYPE_STR, TYPE_STR, TYPE_STR, TYPE_STR, TYPE_NUM, TYPE_STR);
arrayCount[0] = 6;//�ܹ�������(����ָ���Ǹ�������仯����)
recsPerPage[0] = 10; //ÿҳ����������
lineButton[0] = 0;
var arrayData1 = new Array(softwareName, softwareVersion, typesForFile, firmwareVersion, fileSize, defaults);

function dsplist(index, list) {
    if (index == 0) dspList(index, list, -1, titles1, titles1.length, arrayData1, 1, 0);
}

function dspList(index, list, tableWidth, arrayTitle, titleCount, arrayData, chkbox, editable) {
    var i;
	var _strSecondMenuFocus=typeof(strSecondMenuFocus)!='undefined'?strSecondMenuFocus:"";
    var recbegin; //��ǰҳ�Ŀ�ʼ����
    var recend; //��ǰҳ�Ľ�������
    var iIndex = -1;
    var jIndex = -1;
    var MAX_LEN = 31;
    var tmp = "";

    if (_array[tmpIndex] != null) iIndex = _array[tmpIndex][0];
    if (_array[tmpIndex] != null) jIndex = _array[tmpIndex][1];
    out_head(list);
    if (tableWidth == -1) tableWidth = "100%";
    list.document.write('<table width="' + tableWidth + '" height="20"	border="0" cellpadding="0" cellspacing="0" bordercolor="#CCCCCC">');
    list.document.write('  <tr>');
    list.document.write('	 <td class="style21"><table width="' + tableWidth + '"  border="0"  cellpadding="0" id="tableList" name="tableList" cellspacing="1" >');
    list.document.write('	<tr bgcolor="ffffff">');
    if (chkbox) list.document.write('<td width="20" height="20" class="style5 tab_title">&nbsp;</td>');
    for (i = 0; i < titleCount; i++) {
        if (sortable[index][i] == 1) {
            list.document.write('<td height="20" align="center" class="style5 tab_title"><a href="javascript:parent.sortData(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ',' + i + ',parent.arrayData' + (index + 1) + ')" class="ablack tab_title">' + arrayTitle[i] + '</a></td>');
        } else {
            list.document.write('<td height="20" align="center" class="style5 tab_title"><a class="ablack tab_title">' + arrayTitle[i] + '</a></td>');
        }
    }
    if (editable) list.document.write('<td align="center" class="style5" id="edit_td"><%write(edit);%></td>');

	if (_strSecondMenuFocus == "PDB") {//���Կ�ҳ��
		list.document.write('<td align="center" class="style5"> _("updateStrategy")</td>');
	}

    list.document.write('	</tr>');
    recbegin = (currpage[index] - 1) * recsPerPage[index];

    recend = recbegin + recsPerPage[index];

    for (i = recbegin; i < recend; i++) { //i = recbegin ������ = recbegin + 1 ��ΪFILLDATA�����еĲ�����arrarData������ ��0��ʼ��
        if (i % 2 == 0) {
            list.document.write('	<tr  class="style18">'); //���иı���ɫ
        } else {
            list.document.write('<tr style="background-color:ffffff">');
        }

        if (i < recscount[index]) {

            if (chkbox) {
                list.document.write('<td height="20"><input type="checkbox" class="input-checkbox" name="chklist" value="on"></td>');
            }
            if (editable) //add by cxhu 090825
            list.document.write('<td height="20" align="center" class="style6">' + arrayData[0][i] + '</td>');
            else list.document.write('<td height="20" align="center" class="style6">' + (arrayData[0][i] || "&nbsp;") + '</td>');
            for (j = 1; j < titleCount; j++) {
                //if ((j == 2 && _strSecondMenuFocus == "��������״̬") || (j == 3 && _strSecondMenuFocus == "IP/MAC��") || (j == 5 && _strSecondMenuFocus == "��Ϊ����") || (j == 3 && _strSecondMenuFocus == "��������")) {
				if (j == 5 && _strSecondMenuFocus == _("<%write(AdminAPSoft);%>")) {
                    strTmp = "";
                    if (arrayData[j][i].toLowerCase() == "yes")�� { //��������
                        strTmp = "checked";
                    }
                    list.document.write('<td align="center" height="20"><input type="checkbox" name="chkAllow" ' + strTmp + ' id="df' + i + '" onClick= "javascript:parent.setAsDefault(' + index + ',' + i.toString() + ')" value=' + arrayData[j][i] + '></td>');
                } else list.document.write('<td height="20" align="center" class="style6">' + arrayData[j][i] + '</td>');
            }
            if (_strSecondMenuFocus == "PDB") list.document.write('		    <td align="center" class="style5"><a href="javascript:parent.updatePolicy(' + index + ',' + i.toString() + ')"><font color="#0000FF">_("renew")</font></a></td>');
            if (editable) {
                if ((_strSecondMenuFocus == _("lineMsgList")) || (_strSecondMenuFocus == _("doubleLine"))) {
                    list.document.write(' <td align="center" class="style5"><a href="javascript:parent.EditConfig(1,' + i + ')" class="ablack"><img src="images/edit.gif" title=_(modify) border="0"></a></td>');
                } else list.document.write(' <td align="center" class="style5"><a href="javascript:parent.EditConfig(1,' + i + ')" class="ablack"><img src="images/edit.gif" title=_("modify") border="0"></a>&nbsp;&nbsp;&nbsp;&nbsp;<a href="javascript:parent.DeleteConfig(' + i + ')" class="ablacks"><img src="images/delete.gif" title=_("del") border="0"></a></td>');
            }
        } else {
            if (chkbox) list.document.write('		<td height="20"></td>');

            for (k = 0; k < titleCount; k++) list.document.write('<td height="20" align="center" class="style6"></td>');
            if (editable) list.document.write('	       <td align="center" class="style6"></td>');
            if (_strSecondMenuFocus == "PDB") list.document.write('	       <td align="center" class="style6"></td>');
        }
        list.document.write('</tr>');
    }

    list.document.write('	   </table></td>');
    list.document.write('	 </tr>');
    list.document.write('</table>');
    out_tail(list);
}


var moveDivObj = null;

function initHelp()
{
	if(macStr!='')
	{
		var obj = document.getElementsByName('helpButton');
		for(var i =0 ;i < obj.length; i++)
		{
	        obj[i].onclick = function() {window.open('<% write(com_helppage); %>?#apSoftMng')};
		}
	}
}

var typeArr =  new Array();
var firmwareArr = new Array();
function initDefault()
{
	var j, n=0;
	for (j = 0; j < defaults.length; j++)
	{
		if (defaults[j] == "yes")
		{
			typeArr[n] = typesForFile[j];
			firmwareArr[n] = firmwareVersion[j];
			n++;
//			alert(typesForFile[j]);
//			alert(firmwareVersion[j]);
//			alert(typeArr[n]);
//			alert(firmwareArr[n]);
		}
	}
}


function setAsDefault(index,i)
{
	var p, q;
	typeArr.popN(typeArr.length);
	firmwareArr.popN(firmwareArr.length);
	initDefault();
//	alert(typesForFile[i]);
//	alert(firmwareVersion[i]);
	for (p = 0; p < typeArr.length; p++)
	{
		for (q = 0; q < firmwareArr.length; q++)
		{
			if ((typesForFile[i] == typeArr[p]) && (firmwareVersion[i] == firmwareArr[q]))
			{
				if (frame12.document.getElementById('df' + i).checked == true)
				{
//					alert(typeArr[p]);
//					alert(firmwareArr[q]);
					frame12.document.getElementById('df' + i).checked = false;
					alert(_("<%write(warning15);%>"));
					return;
				}
			}
		}
	}
	if (defaults[i] == "")
	{	//�ɷ�Ĭ����ΪĬ��
		fileList.defName.value=softwareName[i];
		fileList.softVer.value=softwareVersion[i];
		fileList.forType.value=typesForFile[i];
		fileList.firmVer.value=firmwareVersion[i];
		i = 1;
	}
	else
	{	//��Ĭ����Ϊ��Ĭ��
		fileList.defName.value=softwareName[i];
		fileList.softVer.value=softwareVersion[i];
		fileList.forType.value=typesForFile[i];
		fileList.firmVer.value=firmwareVersion[i];
		i = 0;
	}
//	alert(fileList.softVer.value);
//	alert(fileList.forType.value);
//	alert(fileList.firmVer.value);
//	return;
	if(i == 1)
	{	//�ɷ�Ĭ����ΪĬ��
		fileList.action ="/goform/formSetSoftAsDefault";
	}
	else
	{	//��Ĭ����Ϊ��Ĭ��
		fileList.action ="/goform/formClearSoftAsDefault";
	}
	fileList.submit();
}

function init() {
/* For file list */
	var obj = document.getElementById('reNameTb');
  	frame11.location.href = "_listhead1.htm"; //ע������ڴ˴���ʼ��������ҳ��ˢ�µ�ʱ����ܳ���bug
    frame12.location.href = "_list1.htm"; //���������ˢ�� ҳ����������Ƿ�����
	moveDivObj = new moveDiv(obj,document.getElementById('moveDivPar'));
	try
	{
		obj.style.display="table";
	}
	catch(e)
	{
		obj.style.display="block";
	}
	moveDivObj.init(_("<%write(rename);%>"));
	if (macStr != "")
	{	
		controlWin(850,530,1);
		obj=document.getElementsByName("goBack");
		for(var i=0;i<obj.length;i++)
		{
			obj[i].style.display="inline";
		}
		initHelp();
		document.getElementById('addEntry').style.display = "none";
		document.getElementById('goBack').style.display = "inline";
		if(!isGroupOpt)//��������
		{
			window.document.title+=" / "+ipStr;
			if(waitOver != "" && urcpError=="") alert(_("<%write(lawfull);%>"));
			
		}
		else if(waitOver != "")
		{
			if(!showBatchResult(errorArr,ipStr,macStr,_("<%write(failTips);%>"),1))
			{
				alert(_("<%write(lawfull);%>"));
			}
		}
		document.getElementById('addEntry').style.display = "none";
	}
	else
	{
		document.getElementById('addEntry').style.display = "inline";
		document.getElementById('goBack').style.display = "none";
		//document.getElementById('addEntry').style.display = "inline";
		//document.getElementById('delete').style.display = "inline";
	}
	fileList.passStr.value = passStr;
	fileList.ipStr.value= ipStr;
	fileList.macStr.value= macStr;
	fileList.szName.value= szName;
	fileList.managedEn.value= managedEn;
	fileList.isGroupOpt.value= isGroupOpt;
	fileList.devType.value= devType;
	initDefault();
}

function save(opt)
{
	var index,i,j;
	var actStr='';
	var altStr='';
	if(opt == "reName")
	{//��̨ͨ��newName��ȡ�����ƣ�oldNameΪ������
		if (!checkFileName(fileList.newName, 50, _("<%write(confName);%>"), 1)) {
            return;
        }
		i = namearray.searchDataCaseInsensitive(fileList.oldName.value);//windowϵͳ��Сд�����У�Ӧ����
		j = namearray.searchDataCaseInsensitive(fileList.newName.value);
		if( j>=0 && j!=i)
		{
			inputErro(fileList.newName,_("<%write(usedTips);%>"));
			return;
		}
		fileList.statustr.value = statuarray[i];
		actStr = "/goform/formApSoftReName";
	}
	else if(opt == "list")
	{
		index = fileList.sel_list.selectedIndex;
		if(index == 1)//ɾ��ȫ��
		{
			if(!confirm(_("<%write(confirmDelAllFileTips);%>")))return;
			actStr="/goform/formApSoftDelAll";
		}
		else
		{
/*			
			if (!genData(fileList, 0, frame12, statuarray, "\"�豸�����ļ�\"", ",")) {
				return;
			}
			fileList.statustr.value = fileList.delstr.value.slice(0,-1);//�Ƿ��Ѿ����浽�豸
			fileList.statustr.value = fileList.statustr.value.replace(/,/g,"");
*/			
			if (!genData(fileList, 0, frame12, softwareName, "\"<%write(swName);%>\"", ",")) {
				return;
			}
			fileList.delstr.value = fileList.delstr.value.slice(0,-1);
			if(index == 0)//ɾ��
			{
				altStr=_("<%write(confirmDelTips);%>");
				actStr="/goform/formApSoftDel";
			}
			else if(index == 2) //���浽�豸
			{
				altStr=_("<%write(confirmSaveTips);%>");
				actStr="/goform/formApSoftSave";
			}
			if(!confirm(altStr))return;
		}
	}
//	alert(fileList.delstr.value);
//	return;
	fileList.action=actStr;
	fileList.submit();
	return;
}

function go_back()
{
	fileList.macStr.value = macStr;
	fileList.szName.value = szName;
	fileList.managedEn.value = managedEn;
	if(isGroupOpt)
		fileList.action="ap_remote_manage_group.asp";
	else
		fileList.action="ap_remote_manage.asp";
	fileList.submit();
}

function EditConfig(val, i) {
	
    if (val == 0) 
	{
		if(eval(maxrecs[0]-totalrecs)<1)
		{
			alert(_("<%write(warning7);%>"));
			return;
		}
		fileList.action="wlanUpdateUpload.asp";
		fileList.submit();
	}
    else 
	{
		inputRight(fileList.newName);
		fileList.oldName.value = softwareName[i];
		fileList.newName.value = softwareName[i];
		moveDivObj.show();
		fileList.newName.focus();
	}
}
function DeleteConfig(index) {
    if (confirm(_("<%write(confirmDelTips);%>"))) {
      	fileList.delstr.value = softwareName[index];
		fileList.statustr.value = statuarray[index];
		//alert(fileList.delstr.value);
		//return;
        fileList.action = "/goform/formApSoftDel";
        fileList.submit();
    }
}

</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<script language="javascript">urcpErrorPost("ap_remote_manage.asp");</script>
<table width="100%" border="0" cellspacing="0" cellpadding="0" ><!--1-->
<!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****�м�����****-->
  <tr>
   <td width="15"></td>
  <!--////����-->
  <td class="table_bg"><script language="javascript">showSecondTitle(_("<%write(AdminAPSoft);%>"));//��ʾҳ������</script>
   <table width="80%" border="0"  height="450" cellspacing="0" cellpadding="0" align="center" ><!--2-->
   <script language="javascript">showerror(errorstr,100);//��ʾ���淵�صĴ�����Ϣ</script>
  <tr >
    <td>
	<!--//�����˵�-->
	<p>&nbsp;</p>
	<script language="javascript">
		var remote = (macStr == "") ? 1:0;
		var arrayThreeMenu=new Array(
						"100%",
						2,
						new Array(_("<%write(swList);%>"),'wlanUpdate.asp',1,1),
						new Array(_("<%write(swUpload);%>"),'wlanUpdateUpload.asp',remote,0)
		);
		showThreeMenu();
	</script>
	 <!--//end-->
<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center"><tr><!--3-->
    <td align="center"  class="three_td"><table width="100%" border="0" cellspacing="0" cellpadding="0"><!--4-->
  <tr>
    <td>
    <table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td><!--5-->
	<!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<br>
<form name="fileList" method="post">
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
<input type="hidden" name="defName">
<input type="hidden" name="softVer">
<input type="hidden" name="forType">
<input type="hidden" name="firmVer">
<input type="hidden" value="" name="devType">
<div id="moveDivPar"></div>
<table id="reNameTb" width="300" height="60" style="display:none">
<tr><td id="inputName_td"><%write(enterName);%></td><td><input type="text" size="30" maxlength="31" value=""  onChange='checkFileName(this,50,_("<%write(softName);%>"),1);'  class="input" name="newName" id="newName"></td></tr><tr><td colspan="2" align="center"><input name="input" type="button" class="input-button" onClick="save('reName')" value="<%write(ok);%>" id="confirm"><input name="" type="button" value="<%write(cancel);%>" class="input-button" onClick="moveDivObj.hide();" id="cancel"></td></tr>
</table>
<table  border="0"  width="100%" align="center" cellpadding="0" cellspacing="0">
<tr>
<td colspan="5" height="" align="center" valign="bottom">
<iframe  scrolling="no" name="frame11" valign="bottom" align="center" width="680" height="44" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe>	  </td>
</tr>
<tr>
<td colspan="5" align="center" valign="top">
<iframe name="frame12" scrolling="no" id="framelist" valign="top" align="MIDDLE" width="680" height="248" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe></td>
</tr>
<tr><td width="204" height="21" valign="top"><input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist')) id="delChk">  &nbsp;<%write(allSelect);%> / <%write(allUnselect);%>&nbsp;</td>
<td width="544" align="right" valign="top">
  <select size="1" class="input" name="sel_list" id="sel_list" onChange="">
    <option value="0" selected="selected" id="del"><%write(del);%></option>
    <option value="1" id="delall"><%write(delAll);%></option>
    <script language="javascript">
	if(outerFlashTotalSpace)
	{
		document.write( '<option value="2" id="saveToU"><%write(saveToU);%></option>');
		document.write( '<option value="3" id="saveAllToU"><%write(allSaveToU);%></option>');
	}
	</script>
  </select>
  <input type="button" class="input-button" value="<%write(ok);%>" name="btn_scan2" onClick="save('list')" id="confirm">
  <input type="button" class="input-button" onClick="EditConfig(0)" value="<%write(addNew);%>" id="addEntry">
  <input type="button" id="helpButton" name="helpButton" value="<%write(help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#apSoftMng')">
  <input type="button" class="input-button" style="display:none" name="goBack" value="<%write(back);%>" onClick="go_back();" id="goBack"></td>
</tr></table>
</form>
<!--
<tr>
<td width="379" height="21"><input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist')) id="delChk">
ȫѡ / ȫ��ѡ</td>
<td width="221"  align="right">	   
<input type="button" class="input-button" onClick="EditConfig(0)" value="�������Ŀ" id="addEntry">
<input type="button" class="input-button" onClick="delFunc()" value="ɾ��" id="delete">
<input type="button" class="input-button" style="display:none" name="goBack" value="����" onClick="go_back();" id="goBack"></td>
</tr></table>
</form>
-->
<!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
</td><td class="gapTd"></td></tr>
</table><!--5-->
</td></tr>
  </table><!--4-->
  </td></tr>
</table><!--3-->
</td></tr>
</table><!--2-->
</td>
<!--////end-->
  <td width="15">&nbsp;</td>
  </tr>
</table><!--1-->
</body>
</html>

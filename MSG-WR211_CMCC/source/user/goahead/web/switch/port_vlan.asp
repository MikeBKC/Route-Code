<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>��������-�˿�VLAN</title>
<link rel=stylesheet type="text/css" href="../css/style.css" title="cool" id="lnk">
<script src="../js/common.js" type="text/javascript"></script>
<script src="js/common_switch.js" type="text/javascript"></script>
<script language="javascript">

//����
var errorstr="";
<%
var macStr = Form("macStr");//Զ���豸mac��ַ
var urcpSet = Form("urcpSet");//�Ƿ��������þ������ת����ҳ���
if (macStr!="" || urcpSet!="")
{
	aspOutPutPortVlanUrcp();//������Ϣ
}
else
{
	aspOutPutPortVlan();//������Ϣ
	write('var ipStr="";');
	write('var macStr="";');
	write('var passStr="";');
	write('var urcpError="";');
}
%>
var str='';
var notAlertUrcpError=0;
if(vlanFlag == 0)
{
	str="�豸Vlan����Ϊ\"δ����Vlan\"";
}
else if(vlanFlag==2)
{
	str="�豸Vlan����Ϊ\"Tag Vlan\"";
}
if(str!='')
{
	str+=",�����޷��ڴ�����Port Vlan��";
	if(macStr!="")
	{
		str+="\nȷ���л���Port valnģʽ��";
		if(urcpError=='')//����������������������urcpErrorPost�ȴ���������
		{
			if(!confirm(str))
			{
				urcpError=str;
				notAlertUrcpError=1;
			}
		}
	}
	else
	{
		closeWin(str);//ֱ�Ӵ򿪴�ҳ�棬�ҵ�ǰ���÷�port vlan
	}
 
}

var portTitle=new Array();//�˿���ʾ��Ϣ
var aggArr = new Array();
var vlanArr = new Array();
var isCheckAggr=1;
//��ʼ��ȫ�ֱ���
function init_global_var()
{
	var i=0,j=0;
	//��ʼ��portTitle �� aggArr
	for (i = 0; i < numPorts; i++)
	{
		portTitle [i] = "δ���û��";
	}
	for(i=0;i<aggLists.length;i++)
	{
		aggArr[i] = aggLists[i].split(" ");
		for(j=0;j<aggArr[i].length;j++)
		{
			portTitle[(aggArr[i][j] - 1)] = "���ڻ����"+aggGrpNos[i];
		}
	}
	//��ʼ��portTitle �� vlanArr
	for(i=0;i<vlanLists.length;i++)
	{
		vlanArr[i] = vlanLists[i].split(" ");
	}
	j = checkAggrVlan(aggArr,vlanArr);
	if( j>= 0)
	{
		if(errorstr) errorstr="1. "+errorstr+"<br> 2.";
		errorstr+="���ô��󣺻����"+aggGrpNos[j]+"�ڵĶ˿ڲ���ȫ������һVLAN";
		isCheckAggr=0;
	}
}
init_global_var();
/**vlan����ѡ��*/
function _save() {
	VLANS.action="/goform/formVlan";//��̨���ܵ�vlanFlag��ֵΪ 0������ҳ�� 1���� port_vlan.asp 2����tag_vlan.asp
	VLANS.submit();
}
function go_back()
{
	PVLAN.action="remote_manage.asp";
	PVLAN.submit();
}
function initHelp()
{
	if(macStr!='')
	{
		var obj = document.getElementsByName('helpButton');
		for(var i =0 ;i < obj.length; i++)
		{
	        obj[i].onclick = function() {window.open('../rhelpinfo.htm?#portvlanUrcp')};
		}
	}
}
function init() {
	//����Ϊ���ģʽ
	var obj=null;
	PVLAN.passStr.value = passStr;
	PVLAN.ipStr.value= ipStr;
	PVLAN.macStr.value= macStr;
	PVLAN.numPorts.value= numPorts;
	if(macStr!='')
	{
		obj=document.getElementsByName("goBack");
		for(var i=0;i<obj.length;i++)
		{
			obj[i].style.display="inline";
		}
		window.document.title+=" / "+ipStr;
		initHelp();
	}
    PVLAN.actions[0].checked = true;
    fillData(-1);
	frame11.location.href="_listhead2.htm";//ע������ڴ˴���ʼ��������ҳ��ˢ�µ�ʱ����ܳ���bug
	frame12.location.href="_list1.htm";//���������ˢ�� ҳ����������Ƿ�����
}

var titles1 = new Array("VLAN���","VLAN������","VLAN��Ա" );
var total_recs = vlanGrpNos.length;
var max_totalrecs = numPorts;
currpage[0] = 1;
recscount[0] = total_recs;
maxrecs[0] = max_totalrecs;
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] = "�˿�VLAN�б�";
sortable[0] = new Array(1, 1, 1);
sortType[0]=new Array(TYPE_NUM,TYPE_CHINESE,TYPE_STR);
arrayCount[0] = 3;
recsPerPage[0] = 10; //ÿҳ����������
lineButton[0] = 0;
var arrayData1 = new Array(vlanGrpNos,vlanGrpNames, vlanLists);
function dsplist(index, list) {
    if (index == 0) dspList(index, list, -1, titles1, titles1.length, arrayData1, 1, 1);
}
function EditConfig(val, i) {
	document.getElementById('vlanNo').value='';
	document.getElementById('vlanName').value='';
	inputRight(document.getElementById('vlanNo'));;//��ʼ��������ʾ��Ϣ
	inputRight(document.getElementById('vlanName'));;
    if (val == 0) 
	{
		PVLAN.actions[0].checked = true;
		fillData( - 1); //���
	}
    else 
	{
		PVLAN.actions[1].checked = true;
		fillData(i); //�޸�
	}
    switchDiv(4);
}
function fillData(index) {
	for (i = 0; i < numPorts; i++) {
        document.getElementById('chkPort'+i).checked = false;
		document.getElementById('chkPort'+i).disabled = false;
  
    }
    if (parseInt(index) < 0) {
		//���
        return;
    }
    PVLAN.vlanNo.value = vlanGrpNos[index];
	PVLAN.vlanName.value = vlanGrpNames[index];
	var array = trimstr(vlanLists[index]).split(" ");//ע��˴������ñ�����õ������ݣ�������vlanArr��������ᵼ�´���
    for (var j = 0; j < array.length; j++) {
        document.getElementById('chkPort'+ (array[j] - 1) ).checked = true;
    }
	
}
/**
�޸�ʱ����vlanNo��ֵ���������
**/
function fillData2()
{
	var index = vlanGrpNos.searchData(document.getElementById('vlanNo').value);	
	var gno = document.getElementById('vlanNo').value; //VLAN���
    index = vlanGrpNos.searchData(gno);
	if(index < 0) 
	{
		 for (i = 0; i < numPorts; i++) 
		{
        	document.getElementById('chkPort'+i).checked = false;
			document.getElementById('chkPort'+i).disabled = true;
    	}
		inputErro(document.getElementById('vlanNo'),'�Ƿ��޸�:��VLAN��Ų�����');
		return;
	}
	else
	{
		inputRight(document.getElementById('vlanNo'));
		inputRight(document.getElementById('vlanName'));
		fillData(index);
	}
}
function radioClick()
{
	if(!checkNumber(document.getElementById('vlanNo'), 1, numPorts, '',0,1)) return;
	if(PVLAN.actions[1].checked) fillData2();//�޸�
	else//���
	{
		inputRight(document.getElementById('vlanNo'));//ȥ���޸Ĳ����Ĵ�����ʾ��Ϣ
		for (i = 0; i < numPorts; i++) {
		document.getElementById('chkPort'+i).disabled = false;
   		}
	}
}
function sel_allChkbox(obj) {
    for (var i = 0; i < numPorts; i++) {
          eval("PVLAN.chkPort" + i).checked = obj.checked; 
    }
}
function reSet()
{
	if(PVLAN.actions[1].checked) radioClick();//�޸�
	else
	{
		fillData(-1);
	}
}
/***
�༭ѡ��е�VlanNo�仯ʱ�������¼�
**/
function onVlanNoChange()
{
		if(!checkNumber(document.getElementById('vlanNo'), 1, numPorts, 'VLAN���'))
			return ;
		if(PVLAN.actions[1].checked) fillData2() ;//�޸�
}
/******
�жϱ��β��� ���� ��ǰ�Ķ˿ڻ��������˵ �ǲ��ǺϷ�����
isDel���β���������:trueɾ��, false���/�޸�
*******/
function isAggrOK(isDel) {
	if (!isCheckAggr)
	{
		return true;
	}	
	var i=0,j=0,k=0;
	var tmpVlanArr = vlanArr.slice();//��ʱvlan���飨ע��˴�������Array�������ȿ�����
	//�ȼ��豾���Ѿ��ύ�ɹ����ҳɹ����vlan��ϢΪtmpVlanArr
	if(!isDel)//��ӻ��޸�
	{
		if (PVLAN.actions[0].checked)//���
		{//��Ӳ��������۴���(��Ϊ�Ѵ��ڵĻ�����Ѿ���ȫ����һ��vlan��)
			return true;
		}
		var chkArr = new Array();
		j=0;
		for (i = 0; i < numPorts; i++) {
			if (document.getElementById('chkPort'+i).checked) {
				chkArr[j++] = (i+1);
			}
		}
		/*if (PVLAN.actions[0].checked)//���
		{
			k=tmpVlanArr.length;
			tmpVlanArr[k] = new Array();
		}
		else//�޸�
		{*/
		k=vlanGrpNos.searchData(eval(PVLAN.vlanNo.value));//�޸�
		if(k < 0) return false;
		/*}*/
		
		tmpVlanArr[k] = chkArr;
	} 
	else //ɾ��
	{
        var delNoArr = PVLAN.delstr.value.split(",");
        for (i =0; i<delNoArr.length ;i++)
		{
			k = vlanGrpNos.searchData(delNoArr[i]);
			if(k<0) continue;
			else tmpVlanArr.splice(k,1);//ɾ����vlan
		}
    }
	//����ɹ����vlan��ϢtmpVlanArr�������	
	j = checkAggrVlan(aggArr,tmpVlanArr);
 	if(j  < 0) return true;
    if (isDel) {
        alert("����ɾ��VLAN�������"+aggGrpNos[j]+"�ڵĶ˿ڱ�����ȫ����ĳһVLAN!");
    } else {
        alert("�����"+aggGrpNos[j]+"�ڵĶ˿ڱ�����ȫ����ĳһVLAN!");
    }
    return false;
}
function save() {
    var grpNo = 0;
    var num = 0;
    var i = 0;
   // var tmps = new Array();
    var curPort = 0;
	var gno = document.getElementById('vlanNo').value; //vlan���
	if( !checkNumber(document.getElementById('vlanNo'),1,numPorts,'VLAN���') ) return;	
	i = vlanGrpNos.searchData(gno);
    if (PVLAN.actions[0].checked && i >= 0) {
        inputErro(document.getElementById('vlanNo'),'�Ƿ����:��VLAN����Ѿ�����');
        return;
    }
    if (PVLAN.actions[1].checked && i < 0) {
        inputErro(document.getElementById('vlanNo'),'�Ƿ��޸�:��VLAN��Ų�����');
        return;
    }
	inputRight(document.getElementById('vlanNo'));
	if(!checkName(document.getElementById('vlanName'),11,"VLAN������",1))
	{
		return ;
	}
    for (i = 0; i < numPorts; i++) {
        if (document.getElementById('chkPort'+i).checked) {
            curPort = i + 1;
           // tmps[j++] = curPort;
            num += 1 << i;
        }
    }
	if (num == 0) {
			alert("������ѡ��һ����Ա!");
			return;
	}
    PVLAN.chkstr.value = num; //����Ľ��
    if (!isAggrOK(false)) {
        return;
    }
    PVLAN.action = (macStr==""?"/goform/formPortVlan":"/goform/formPortVlanUrcp");
    PVLAN.submit();
}

function delFunc() {
    var array = null;
    if (genData(PVLAN, 0, frame12, vlanGrpNos, 'VLAN��', ",")) {
		PVLAN.delstr.value = PVLAN.delstr.value.slice(0, PVLAN.delstr.value.length - 1);
        array = PVLAN.delstr.value.split(",");
        for (var i = 0; i < array.length; i++) {
            if (array[i] == "1") {
                alert("���Ϊ1��VLAN����ɾ��!");
                return;
            }
        }
        if (!isAggrOK(true)) {
            return;
        }
		if (!confirm("ȷ��ɾ����")) {return;};
        PVLAN.action = (macStr==""?"/goform/formdelPortVlan":"/goform/formdelPortVlanUrcp");
        PVLAN.submit();
    }
}
function DeleteConfig(index) {
    if (vlanGrpNos[index] == "1") {
        alert("���Ϊ1��VLAN����ɾ��!");
        return;
    } else {
        PVLAN.delstr.value = vlanGrpNos[index];
        if (!isAggrOK(true)) {
            return;
        }
		if (!confirm("ȷ��ɾ����")) {return;};
        PVLAN.action = (macStr==""?"/goform/formdelPortVlan":"/goform/formdelPortVlanUrcp");
        PVLAN.submit();
    }
}
</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="../images/body_main.png" style="background-repeat:repeat-x">
<script language="javascript">urcpErrorPost("remote_manage.asp",notAlertUrcpError);</script>
<table width="100%" border="0" cellspacing="0" cellpadding="0" >
<!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****�м�����****-->
  <tr>
   <td width="15"></td>
  <!--////����-->
  <td class="table_bg"><script language="javascript">showSecondTitle('�˿�VLAN');//��ʾҳ������</script>
   <table width="80%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  >
   <script language="javascript">showerror(errorstr,100);//��ʾ���淵�صĴ�����Ϣ</script>
  <tr valign="top">
    <td>
	<!--//�����˵�-->
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"100%",
						3,
						new Array('VLAN����ѡ��','vlan_sel',1,0),
						new Array('�˿�VLAN�б�','port_vlan_list',1,1),
						new Array('�˿�VLAN����','port_vlan_edit',1,0)
		);
		//if(macStr!='')//����������˵���ת������
		{
			arrayThreeMenu[2][2]=0;//ȥ��vlan����ѡ��
		}
		showThreeMenuNotCommit();
	</script>
   <!--//end-->
  <table width="100%" border="0" cellspacing="0" cellpadding="0" align="center"><tr>
    <td align="center"  class="three_td"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr><td><table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td>
	<!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
    <br>
<div class="inside_div" >
<div id="vlan_sel" style="display:none">
<form method="POST"  name="VLANS">
<input type="hidden" value="Apply" name="_submit">
<br><br>
<TABLE cellSpacing=0 class="" width="600" cellPadding=4 border=0 align="center">

<tr>
<td class=vncell width="100">&nbsp;&nbsp;&nbsp;ѡ��VLAN����</td>
<td class=vtable>
<select name="vlanFlag"  id="vlanFlag" style="width:120">
<option value="0">������VLAN</option>
<option value="1" selected>�˿�VLAN</option>
<option value="2">TAG VLAN</option>
</select>
</td>
<td width="240" align="right"><input type="button" class="input-button" value="����" name="btnSaveSettings" onClick="_save()">
&nbsp;<input type="button" class="input-button" value="����" name="btnCancel" onClick="VLANS.reset();">
&nbsp;<input type="button" id="helpButton" name="helpButton"  class="input-button" value="����" onClick="window.open('../rhelpinfo.htm#portvlan')">
<input type="button" class="input-button" style="display:none" name="goBack" value="����" onClick="go_back();"></td>

</tr>
</TABLE>

</form>
<table id="uttid">
<td  height="20">
</td>
</table>
</div>

<form method="POST" action="/pvlan" name="PVLAN">
<input type="hidden" value="Apply" name="_submit">
<input type="hidden" name="chkstr"><!-- ��ѡ�е�checkbox -->
<input type="hidden" name="delstr">
<input type="hidden" value="" name="ipStr">
<input type="hidden" value="" name="macStr">
<input type="hidden" value="" name="passStr">
<input type="hidden" value="" name="numPorts">
<div id="port_vlan_list" style="display:block">
<table width="600"  border="0" align="center" cellpadding="0" cellspacing="0">


<tr>
<td colspan="2" height="" align="center" valign="bottom">
<iframe  scrolling="no" name="frame11" valign="bottom" align="center" width="600" height="44" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe>	  </td>
</tr>
<tr>
<td colspan="2" align="center" valign="top">
<iframe name="frame12" id="framelist" scrolling="no" valign="top" align="MIDDLE" width="600" height="248" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe></td>
</tr>
<tr>
<td width="296" height="21"><input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'))>
ȫѡ / ȫ��ѡ</td>
<td width="304" height="21"  align="right">	   
<input type="button" class="input-button" onClick="EditConfig(0)" value="�������Ŀ">
<input type="button" class="input-button" onClick="delFunc()" value="ɾ��">
<input type="button" class="input-button" style="display:none" name="goBack" value="����" onClick="go_back();"></td>
</tr>
</table>
</div>
<div id="port_vlan_edit" style="display:none">
<TABLE   width="600" align="center" border=0>
<tr height="30">
<td  align="left" width="500">
VLAN���<input type="text" class="input" name="vlanNo" id="vlanNo" onKeyPress="javascript:figureOnly()" size="4" maxlength="2" onChange="onVlanNoChange();">&nbsp;
VLAN������<input type="text" class="input" name="vlanName" id="vlanName" value="" size="18" maxlength="11" onChange="checkName(this,11,'VLAN������',1)"> </td>
<td width="1" align="left"><span id="vlanteam"></span>&nbsp;</td>
<td width="99" align="right">���<input type="radio" checked="checked" name="actions" value="add" onClick="radioClick()">&nbsp;�޸�<input type="radio" name="actions" value="modify" onClick="radioClick();"></td>
</tr>
<tr><td width="" height="25" valign="middle"  colspan="3">
<script>
//���ɶ˿�vlan�༭�б�

document.write('<table width="600" border="0" align="center" cellpadding=4 cellspacing=1 class=style22>');
document.write('<tr>');
document.write('<td align="center" width="20" class="style18" rowspan="4">');
document.write('<b>��Ա</b></td>');
for (pno = 1; pno <= numPorts/2; pno++) {
document.write('<td width="" rowspan="" class="style18" align="center" >'+ pno +'<\/td>');	
}
document.write('</tr>');
document.write('<tr>');
//document.write('<td width="62" rowspan="" height=""><b>&nbsp;<\/b><\/td>');
for (pno = 1; pno <= numPorts/2; pno++) {
document.write('<td width="" rowspan="" title="'+portTitle[pno-1]+'" align="center"><input type="checkbox" id="chkPort'+ (pno-1) +'" name="chkPort'+ (pno-1) +'"  style="" maxlength="4"><\/td>');	
}
document.write('</tr>');
document.write('<tr>');
//document.write('<td align="right" width="54">');
//document.write('<b>&nbsp;</b></td>');
for (pno = numPorts/2+1; pno <= numPorts; pno++) {
document.write('<td width="" rowspan="" class="style18" align="center">'+ pno +'<\/td>');	
}
document.write('</tr>');
document.write('<tr>');
//document.write('<td width="62" rowspan="" height=""><b>&nbsp;<\/b><\/td>');
for (pno = numPorts/2+1; pno <= numPorts; pno++) {
document.write('<td width="" rowspan="" align="center" title="'+portTitle[pno-1]+'"><input type="checkbox" id="chkPort'+ (pno-1) +'" name="chkPort'+ (pno-1) +'" style="" maxlength="4"><\/td>');	
}
document.write('</tr>');
document.write('</table>');

</script>
</td></tr>
</TABLE>
<table width="600" cellpadding="0" cellspacing="0" align="center">
<tr><td height="8"></td></tr>
<tr align="left" valign="top"><td><input type="checkbox" name="chk_all" onClick="sel_allChkbox(this)">ȫѡ / ȫ��ѡ</td></tr>
<tr><td align="center">
<input type="button" class="input-button" value="����" onClick="javascript:save()"> 
<input type="button" class="input-button" value="����" onClick="reSet();">
<input type="button"  id="helpButton" name="helpButton"  class="input-button" value="����" onClick="window.open('../rhelpinfo.htm#portvlan')">
<input type="button" class="input-button" style="display:none" name="goBack" value="����" onClick="go_back();">
</td></tr>
</table>
</div>
</form>
</div>
<!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
</td><td class="gapTd"></td></tr>
  </table>
 </td>
  </tr>
</table> 
  </td></tr>
</table></td></tr></table></td>
<!--////end-->
  <td width="15">&nbsp;</td>
  </tr>
</table>
</body>
</html>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("port_vlan_rdd"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script language="javascript">

//����
var errorstr="";
/*var numPorts = 4;
var vlanLists = new Array("1 2 3 4");
var vlanGrpNos = new Array("1");
var vlanGrpNames = new Array("");
var errorstr="";*/
<%aspOutPutPortVlan();%>
var vlanArr = new Array();
var isCheckAggr=1;
//��ʼ��ȫ�ֱ���
function init_global_var()
{
	var i=0,j=0;
	//��ʼ�� vlanArr
	for(i=0;i<vlanLists.length;i++)
	{
		vlanArr[i] = vlanLists[i].split(" ");
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
	PVLAN.action="port_vlan_rdd.asp";
	PVLAN.submit();
}
function init() {
	//����Ϊ���ģʽ
	var obj=null;
    PVLAN.actions[0].checked = true;
    fillData(-1);
	frame11.location.href="_listhead1.htm";//ע������ڴ˴���ʼ��������ҳ��ˢ�µ�ʱ����ܳ���bug
	frame12.location.href="_list1.htm";//���������ˢ�� ҳ����������Ƿ�����
}

var titles1 = new Array("<% write(VLANNo); %>","<% write(VLANName); %>","<% write(VLANMember); %>" );
var total_recs = vlanGrpNos.length;
var max_totalrecs = numPorts;
currpage[0] = 1;
recscount[0] = total_recs;
maxrecs[0] = max_totalrecs;
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] = "<% write(VLANList); %>";
sortable[0] = new Array(1, 1, 1);
sortType[0]=new Array(TYPE_NUM,TYPE_CHINESE,TYPE_STR);
arrayCount[0] = 3;
recsPerPage[0] = 10; //ÿҳ����������
lineButton[0] =0;
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
		if(eval(max_totalrecs - totalrecs)<1)	{
			alert("<%write(com_editAlert);%>");
			return;
		}
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
		inputErro(document.getElementById('vlanNo'),'<% write(modify_error); %>');
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
		if(!checkNumber(document.getElementById('vlanNo'), 1, numPorts, '<% write(VLANNo); %>'))
			return ;
		if(PVLAN.actions[1].checked) fillData2() ;//�޸�
}

function save() {
    var grpNo = 0;
    var num = 0;
    var i = 0;
   // var tmps = new Array();
    var curPort = 0;
	var gno = document.getElementById('vlanNo').value; //vlan���
	if( !checkNumber(document.getElementById('vlanNo'),1,numPorts,'<% write(VLANNo); %>') ) return;	
	i = vlanGrpNos.searchData(gno);
    if (PVLAN.actions[0].checked && i >= 0) {
        inputErro(document.getElementById('vlanNo'),'<% write(add_error); %>');
        return;
    }
    if (PVLAN.actions[1].checked && i < 0) {
        inputErro(document.getElementById('vlanNo'),'<% write(modify_error); %>');
        return;
    }
	inputRight(document.getElementById('vlanNo'));
	if(!checkName(document.getElementById('vlanName'),11,"<% write(VLANName); %>",1))
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
			alert("<% write(LeastMember); %>");
			return;
	}
    PVLAN.chkstr.value = num; //����Ľ��
    PVLAN.action = "/goform/formPortVlan";
    PVLAN.submit();
}

function delFunc() {
    var array = null;
    if (genData(PVLAN, 0, frame12, vlanGrpNos, '<% write(VLANGroup); %>', ",")) 
    {
	PVLAN.delstr.value = PVLAN.delstr.value.slice(0, PVLAN.delstr.value.length - 1);
	array = PVLAN.delstr.value.split(",");
	for (var i = 0; i < array.length; i++) 
	{
	    if (array[i] == "1") 
	    {
		alert("<% write(NoOneNotDel); %>");
		return;
	    }
	}
	if (!confirm("<% write(com_deleteTip); %>")) {return;};
        PVLAN.action = "/goform/formdelPortVlan";
        PVLAN.submit();
    }
}
function DeleteConfig(index) 
{
    if(vlanGrpNos[index] == "1") 
    {
        alert("<% write(NoOneNotDel); %>");
        return;
    } 
    else 
    {
	if (confirm("<% write(com_deleteTip); %>")) 
	{
	    PVLAN.delstr.value = vlanGrpNos[index]; 
	    PVLAN.action = "/goform/formdelPortVlan";
	    PVLAN.submit();
	}
    }
}
</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0" >
<!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****�м�����****-->
  <tr>
   <td width="15"></td>
  <!--////����-->
  <td class="table_bg"><script language="javascript">showSecondTitle('<% write(PortVlan); %>');//��ʾҳ������</script>
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
						new Array('<% write(VLANList); %>','port_vlan_list',1,1),
						new Array('<% write(PortVlanSet); %>','port_vlan_edit',1,0)
		);
		arrayThreeMenu[2][2]=0;//ȥ��vlan����ѡ��
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
&nbsp;<input type="button" id="helpButton" name="helpButton"  class="input-button" value="����" onClick="window.open('<% write(com_helppage); %>#port_vlan_rdd')">
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
<% write(com_select); %></td>
<td width="304" height="21"  align="right">	   
<input type="button" class="input-button" onClick="EditConfig(0)" value="<% write(com_addNew); %>">
<input type="button" class="input-button" onClick="delFunc()" value="<% write(com_delete); %>">
</tr>

</table>
</div>
<div id="port_vlan_edit" style="display:none">
<TABLE   width="600" align="center" border=0>
<tr height="30">
<td  align="left" width="500">
<% write(VLANNo); %><input type="text" class="input" name="vlanNo" id="vlanNo" onKeyPress="javascript:figureOnly()" size="4" maxlength="2" onChange="onVlanNoChange();">&nbsp;
<% write(VLANName); %><input type="text" class="input" name="vlanName" id="vlanName" value="" size="18" maxlength="11" onChange="checkName(this,11,'<% write(VLANName); %>',1)"> </td>
<td width="1" align="left"><span id="vlanteam"></span>&nbsp;</td>
<td width="99" align="right"><% write(com_add); %><input type="radio" checked="checked" name="actions" value="add" onClick="radioClick()">&nbsp;<% write(com_edit); %><input type="radio" name="actions" value="modify" onClick="radioClick();"></td>
</tr>
<tr><td width="" height="25" valign="middle"  colspan="3">
<script>
//���ɶ˿�vlan�༭�б�

document.write('<table width="600" border="0" align="center" cellpadding=4 cellspacing=1 class=style22>');
document.write('<tr>');
document.write('<td align="center" width="20" class="style18" rowspan="4">');
document.write('<b><% write(PortMember); %></b></td>');
if(numPorts % 2 == 1)
{
    var numPorts1 = numPorts + 1;
}
for (pno = 1; pno <= numPorts; pno++) {
document.write('<td width="" rowspan="" class="style18" align="center" >'+ pno +'<\/td>');	
}
document.write('</tr>');
document.write('<tr>');
//document.write('<td width="62" rowspan="" height=""><b>&nbsp;<\/b><\/td>');
for (pno = 1; pno <= numPorts; pno++) {
document.write('<td width="" rowspan="" align="center"><input type="checkbox" id="chkPort'+ (pno-1) +'" name="chkPort'+ (pno-1) +'"  style="" maxlength="4"><\/td>');	
}
document.write('</tr>');
document.write('</table>');

</script>
</td></tr>
</TABLE>
<table width="600" cellpadding="0" cellspacing="0" align="center">
<tr><td height="8"></td></tr>
<tr align="left" valign="top"><td><input type="checkbox" name="chk_all" onClick="sel_allChkbox(this)"><% write(com_select); %></td></tr>
<tr><td align="center">
<input type="button" class="input-button" value="<% write(com_save); %>" onClick="javascript:save()"> 
<input type="button" class="input-button" value="<% write(com_reset); %>" onClick="reSet();">
<input type="button"  id="helpButton" name="helpButton"  class="input-button" value="<% write(com_help); %>" onClick="window.open('<% write(com_helppage); %>#port_vlan_rdd')">
<input type="button" class="input-button" name="goBack" value="<% write(com_back); %>" onClick="go_back();">
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

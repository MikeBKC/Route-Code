<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>��������-�˿ڹ���</title>
<link rel=stylesheet type="text/css" href="../css/style.css" title="cool" id="lnk">
<script src="../js/common.js" type="text/javascript"></script>
<script src="js/common_switch.js" type="text/javascript"></script>
<script language="javascript">
<%
var macStr = Form("macStr");//Զ���豸mac��ַ
var urcpSet = Form("urcpSet");//�Ƿ��������ö˿ں���ת����ҳ���
if (macStr!="" || urcpSet!="")
{//������Ϣ
	aspOutPortConfigureUrcp();
}
else
{//������Ϣ
	aspOutPortConfigure();//����������numPorts��ӡ
	//getAggrConfigure();
	write('var ipStr="";');
	write('var macStr="";');
	write('var passStr="";');
	write('var urcpError="";');
}
%>
var pageType;/*switchV10.(��������ʱ����ҳ��ΪswitchV10���*/
if(macStr=='')
{//����������Ϊ3520G��ҳ������
	pageType=getPageType();
}
</script>
</head>
<body onbeforeunload="logOut()" background="../images/body_main.png" style="background-repeat:repeat-x">
<script language="javascript">urcpErrorPost("remote_manage.asp");</script>
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****�м�����****-->
  <tr>
   <td width="15"></td>
  <!--////����-->
  <td><script language="javascript">showSecondTitle("�˿ڹ���");//��ʾҳ������</script>
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
	<form method="POST" name="PORTS">
	<input type="hidden" value="Apply" name="_submit">
	<input type="hidden" value="0" name="hiddenvalue">
	<input type="hidden" value="0" name="hiddenvalue2">
	<input type="hidden" value="0" name="hiddenvalue3">
    <input type="hidden" value="" name="ipStr">
    <input type="hidden" value="" name="macStr">
    <input type="hidden" value="" name="passStr">
    <input type="hidden" value="" name="numPorts">
	<br>
	<table border="0" width="600" class="styleblue" cellpadding="0" cellspacing="0" align="center">
	<tr>
	<td height="25">ȫ������	
	  <script language="javascript">
	     document.write('<select name="sel_global" class="input" onChange="sel_global_fuc(this)">');
	     document.write('<option value="0">����ģʽ</option>');
	     if(pageType!=2) document.write('<option value="1">�������֡</option>');
	     if(pageType!=2 && pageType!=3) document.write('<option value="2">�˿ڱ���</option>');
	     document.write('<option value="3">����</option>');
	     if(pageType!=2 && pageType!=3) document.write('<option value="4">����</option>');
	     document.write('</select>&nbsp;');
	  </script>
      <script language="javascript">
		  document.write('<select class="input" size="1" name="sel1" onBlur="sel_globalport_func(this)" style="display">');
			document.write('<option value="3">�Զ�Э��</option>');
			document.write('<option value="4">10M(��˫��)</option>');
			document.write('<option value="0">10M(ȫ˫��)</option>');
			document.write('<option value="5">100M(��˫��)</option>');
			document.write('<option value="1">100M(ȫ˫��)</option>');
			if(pageType!=2)
			{
				document.write('<option value="2">1000M(ȫ˫��)</option>');
			}
			document.write('<option value="6">��ֹ</option>');
			document.write('</select>');
	  </script>
      <input class="input"  type="text" name="txt1" onKeyUp="javascript:txt_keypress(event,this)" onKeyPress="figureOnly()" maxlength="4" style="display:none;width:80px; height:20px" onChange="checkNumber(this, 1518, 9600, '�������֡')">
      <input type="checkbox" name="chk1" style="display:none" onClick="chk1_click(this)"></td>
<td width="256" align="right">
<input type="button" value="����" class="input-button" onClick="save()" name="">
<!--<input type="button" value="ˢ��" class="input-button" name="" onClick="location.reload()">-->
<input type="button" class="input-button" value="����" name="" onClick="reSet();">
<input type="button" class="input-button" value="����" id="helpButton" name="helpButton" onClick="window.open('../rhelpinfo.htm#ports')">
<input type="button" class="input-button" style="display:none" name="goBack" value="����" onClick="go_back();">
</td>
</tr>
<tr><td colspan="2" height="8"></td></tr>
</table>
<script type="text/javascript">
var pno;
var i;
//����
/*for aggr�˿ڻ��*/
///var has_power_saving = 1; //����ģʽ
//var mode = 3;
//var portprios = new Array('0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0');
var aggArr = new Array();
for (i = 0; i < aggLists.length; i++) {
    aggArr[i] = aggLists[i].split(" ");
}
var linkStyle = new Array(); //����״̬�߿����ʽ
for (var i = 0; i < numPorts; i++) {
    if (trimstr(curSpeed[i]) == 'Down' || trimstr(curSpeed[i]) == 'Disabled') {
        linkStyle[i] = 'border:0px';
    } else if(curSpeed[i].indexOf('G')>=0 || curSpeed[i].indexOf('g')>=0) {
        linkStyle[i] = 'border:3px solid;border-color:#FF6633';//1000m�˿�
    }
	else//100m�Լ�����
	{
		linkStyle[i] = 'border:3px solid;border-color:#79BE4E';
	}
}

function sel_global_fuc(ctl) {
    switch (eval(ctl.value)) {
    case 0:
        //����ģʽ
        PORTS.sel1.style.display = "inline";
        PORTS.txt1.style.display = "none";
        PORTS.chk1.style.display = "none";
        break;
    case 1:
        //�������֡
        PORTS.sel1.style.display = "none";
        PORTS.txt1.style.display = "inline";
        PORTS.chk1.style.display = "none";
        break;
    case 2:
        //�˿ڱ���
    case 3:
        //flowcontrol
    case 4:
        //flooding
        /* case 5:
            //arp*/
        PORTS.sel1.style.display = "none";
        PORTS.txt1.style.display = "none";
        PORTS.chk1.style.display = "inline";
        break;
    }
}
function txt_keypress(evt,ctl) {
    if ((evt.keyCode || evt.which) == 0) {
        return;
    }
    var obj;
    for (var pno = 1; pno <= numPorts; pno++) {
        obj = document.getElementById("R" + (0x10 + pno).toString(16));
		inputRight(obj);
        obj.value = ctl.value;
    }
}
function chk1_click(ctl) {
    var objArray = document.getElementsByTagName("input");
    var index = 0;
    var gb_config = eval(PORTS.sel_global.value);
    if (gb_config == 2 && ctl.checked) {
        if (!confirm("�����˿ڱ�����ֻ������̬�󶨵�MAC�������磬���Ƿ�ȷ����Ҫ�����˿ڱ�����")) {
            ctl.checked = false;
            return;
        }
    }
    for (var i = 0; i < objArray.length; i++) {
        if (objArray[i].type != "checkbox" || objArray[i].name == "chk1") {
            continue;
        }
        if (gb_config == 2) { //�˿ڱ���
            if (objArray[i].title.indexOf("����") != -1) {
                if (ctl.checked) objArray[i].checked = false;
            }
            if (objArray[i].title.indexOf("�˿ڱ���") == -1) {
                continue;
            }
        } else if (gb_config == 3) {
            if (objArray[i].title != "����") {
                continue;
            }
        } else if (gb_config == 4) {
            if (objArray[i].title.indexOf("�˿ڱ���") != -1) {
                if (ctl.checked) objArray[i].checked = false;
            }
            if (objArray[i].title.indexOf("����") == -1) {
                continue;
            }
        }
        /*else if (gb_config == 5) {
                if (objArray[i].title != "ARP") {
                    continue;
                }
            }*/
        objArray[i].checked = ctl.checked;
    }
}
function sel_globalport_func(ctl) {
    var objArray = document.getElementsByTagName("select");
    var index = 0;
    for (var i = 0; i < objArray.length; i++) {
        if (objArray[i].name.charAt(0) != "R") {
            continue;
        }
        setSelectByVal(objArray[i], ctl.value);
    }
}
function go_back()
{
	PORTS.action="remote_manage.asp";
	PORTS.submit();
}
function init() {
	var obj=null;
	PORTS.passStr.value = passStr;
	PORTS.ipStr.value= ipStr;
	PORTS.macStr.value= macStr;
	PORTS.numPorts.value= numPorts;
	if(macStr!='')
	{
		window.document.title = "��������-�˿ڹ���";
		obj=document.getElementsByName("goBack");
		for(var i=0;i<obj.length;i++)
		{
			obj[i].style.display="inline";
		}
		window.document.title+=" / "+ipStr;
	}
    var objArray = document.getElementsByTagName("input");
    var index = 0;
	if (pageType!=2)
	{
		for (var i = 0; i < objArray.length; i++) {
			if (objArray[i].type != "text" || objArray[i].name.charAt(0) != 'R') {
				continue;
			}
			index = eval("0x" + objArray[i].name.substring(1)) - 0x11;
			objArray[i].value = maxFrame[index];
		}
	}
    PORTS.sel1.style.display = 'inline';
    PORTS.txt1.style.display = 'none';
    PORTS.chk1.style.display = 'none';
}
function initHelp()
{
	if(macStr!='')
	{
		var obj = document.getElementsByName('helpButton');
		for(var i =0 ;i < obj.length; i++)
		{
	        obj[i].onclick = function() {window.open('../rhelpinfo.htm?#portsUrcp')};
		}
	}
}
function isAggrOK() {
    var grp = 0;
    var counts = 0,
    tmp = 0;
    var objArray = document.getElementsByTagName("select");
    var arrays = new Array();
    var flag = 0;
    var mems = new Array(); //ģʽ
    var mems1 = new Array(); //maxframe
    var mems2 = new Array(); //protect
    var mems3 = new Array(); //flowcontrol
    var mems4 = new Array(); //flooding
    for (var i = 0; i < objArray.length; i++) {
        if (objArray[i].name == "sel1" || objArray[i].name == "sel_global") {
            continue;
        }
        mems[tmp++] = objArray[i].value;
    }
    objArray = document.getElementsByTagName("input");
    tmp = 0;
    for (var i = 0; i < objArray.length; i++) {
        if (objArray[i].name == "txt1" || objArray[i].type != "text" || objArray[i].name.substring(0, 8) == "portInfo") {
            continue;
        }
        mems1[tmp++] = objArray[i].value;
    }
    objArray = document.getElementsByTagName("input");
    tmp = 0;
    for (var i = 0; i < objArray.length; i++) {
        if (objArray[i].name == "chk1" || objArray[i].type != "checkbox") {
            continue;
        }
        if (objArray[i].title.indexOf("�˿ڱ���") == -1) continue;
        mems2[tmp++] = objArray[i].checked;
    }
    tmp = 0;
    for (var i = 0; i < objArray.length; i++) {
        if (objArray[i].name == "chk1" || objArray[i].type != "checkbox") {
            continue;
        }
        if (objArray[i].title != "����") continue;
        mems3[tmp++] = objArray[i].checked;
    }
    tmp = 0;
    for (var i = 0; i < objArray.length; i++) {
        if (objArray[i].name == "chk1" || objArray[i].type != "checkbox") {
            continue;
        }
        if (objArray[i].title.indexOf("����") == -1) continue;
        mems4[tmp++] = objArray[i].checked;
    }
    tmp = 0;
    for (var ii = 0; ii < aggArr.length; ii++) 
	for (var jj = 0; jj < aggArr[ii].length; jj++) 
	for (var kk = jj + 1; kk < aggArr[ii].length; kk++) {
        i = aggArr[ii][jj] - 1;
        j = aggArr[ii][kk] - 1;
        if (mems[i] != mems[j]) {
            alert("�˿�" + eval(i + 1) + "�Ͷ˿�" + eval(j + 1) + "��һ��������ڣ��˿�����ģʽ����һ��");
            return false;
        }
        if (mems1[i] != mems1[j]) {
            alert("�˿�" + eval(i + 1) + "�Ͷ˿�" + eval(j + 1) + "��һ��������ڣ��˿��������֡����һ��");
            return false;
        }
        if (mems2[i] != mems2[j]) {
            alert("�˿�" + eval(i + 1) + "�Ͷ˿�" + eval(j + 1) + "��һ��������ڣ��˿ڱ������ñ���һ��");
            return false;
        }
        if (mems3[i] != mems3[j]) {
            alert("�˿�" + eval(i + 1) + "�Ͷ˿�" + eval(j + 1) + "��һ��������ڣ��������ñ���һ��");
            return false;
        }
        if (mems4[i] != mems4[j]) {
            alert("�˿�" + eval(i + 1) + "�Ͷ˿�" + eval(j + 1) + "��һ��������ڣ��������ñ���һ��");
            return false;
        }
    }
    return true;
}
function save() {
    var objArray = document.getElementsByTagName("input");
    var n = 0;
    var tmp = null,
    tmp2 = 0;
    var protection = 0,
    bloodmask = 0;
    if (pageType !=2 && pageType !=3 && !checkprotection_flooding()) {
        alert("�˿ڱ����ͷ���ֻ��ѡ��һ�");
        return;
    }

    for (var i = 0; i < objArray.length; i++) {
        if (objArray[i].type == "checkbox" && objArray[i].name != "chk1") {
            if (objArray[i].checked) {
                objArray[i].value = "1";
            } else {
                objArray[i].value = "0";
            }
        }
		if(pageType !=2 && pageType !=3)
		{
			if (objArray[i].type == "checkbox") {
				if (objArray[i].title.indexOf("�˿ڱ���") != -1) {
					tmp = objArray[i].title.split(":");
					if (objArray[i].checked) {
						protection = setbit(protection, tmp[0]-1);
					}
				} else if (objArray[i].title.indexOf("����") != -1) {
					tmp = objArray[i].title.split(":");
					if (objArray[i].checked) {
						bloodmask = setbit(bloodmask, tmp[0]-1);
					}
				}
			}
		}
        if (objArray[i].type != "text" || objArray[i].name == "txt1") {
            continue;
        }
        if (objArray[i].name.substring(0, 8) == "portInfo") {
            if (!checkName(objArray[i], 11, "�˿�����", 1)) {
                return;
            }
            continue;
        }
        n = eval("0x" + objArray[i].name.substring(1)) - 0x10;
        if (!checkNumber(objArray[i], 1518, 9600, '�������֡')) return;
    }
    // objArray = document.getElementsByName("chk_arp");
    // for (var i = 0; i < objArray.length; i++) {
    //      if (objArray[i].checked) {
    //         tmp2 = setbit(tmp2, i)
    //    }
    // }
	    if (!isAggrOK()) {
        return;
    }
    PORTS.hiddenvalue.value = protection.toString();//�˿ڱ���
    PORTS.hiddenvalue2.value = bloodmask.toString();//����
    //PORTS.hiddenvalue3.value = tmp2.toString();
	PORTS.action=(macStr==""?"/goform/formPortConfigure":"/goform/formPortConfigureUrcp");
    PORTS.submit();
}
/*function ena_jumbo(isJumbo) {
    var dfmaxframe = 1518;
    var objArray = document.getElementsByTagName("input");
    var index = 0;
    if (isJumbo) {
        dfmaxframe = 9600;
    }
    for (var i = 0; i < objArray.length; i++) {
        if (objArray[i].type != "text" || objArray[i].name.charAt(0) != 'R') {
            continue;
        }
        objArray[i].value = dfmaxframe;
    }
}*/
function modeSelected(pno, value) {
    if (speedMode[(pno - 1)] == value) {
        return "selected";
    } else {
        return "";
    }
}
/*function mdSelected(m) {
    if (mode == m) return "SELECTED";
    return "";
}*/
function rad_click(obj) {
    var tmp = document.getElementsByName(obj.name);
    if (obj.checked) {
        obj == tmp[0] ? tmp[1].checked = false: tmp[0].checked = false;
    }
}
function chk_onclick(ctl, i, flag) {
    var str = "�����˿ڱ�����ֻ������̬�󶨵�MAC�������磬���Ƿ�ȷ����Ҫ�����˿ڱ�����";
    var ret = false;
    var objArray = document.getElementsByTagName("input");
    for (var i = 0; i < objArray.length; i++) {
        if (objArray[i].type == "checkbox") {
            if (objArray[i].checked) {
                ret++;
            }
        }
    }
    if (ret == 1 && ctl.checked) {
        if (!confirm(str)) {
            ctl.checked = false;
            return;
        }
    }
}
function checkprotection_flooding() {
    var name = "";
    var objs;
    for (var i = 0; i < numPorts; i++) {
        name = 'R' + (0x70 + (i + 1)).toString(16);
        objs = document.getElementsByName(name);
        if (objs[0].checked && objs[1].checked) {
            return false;
        }
    }
    return true;
}
function reSet()
{
	PORTS.reset();
	init();
	allInputRight(PORTS);
}
/*if (has_power_saving > 0) {
    document.write('<table  border="0"  cellpadding="4" cellspacing="1" class="style22"  width="491"  align="">');
    document.write('<tr class="style18"><td width="376">����ģʽ:<\/b><\/td>');
    document.write('<td width="104" align="center"><select size="1" class="input" name="R90">');
    document.write('<option value="0" ' + mdSelected(0) + '>���ж˿�<\/option>');
    document.write('<option value="1" ' + mdSelected(1) + '>���Ӷ˿�<\/option>');
    document.write('<option value="2" ' + mdSelected(2) + '>��·�˿�<\/option>');
    document.write('<option value="3" ' + mdSelected(3) + '>��ֹ<\/option>');
    document.write('<\/select><\/td><\/tr>');
    document.write('<\/table>');
    document.write('<p align="left">&nbsp;<\/p>');
}*/
document.write('<TABLE  width="600"  border="0" align="center"  cellpadding="4" cellspacing="1" class="style22" >' + '<tr style="background-color:ffffff">' + '<td  class="bh" height="22" width="20" align="center"><b>�˿�<\/b><\/td>' + '<td  class="bh" align="center" width="135"><b>�˿�����<\/b><\/td>' + '<td class="bh" align="center" width="120"><b>����״̬<\/b><\/td>' + '<td class="bh" align="center" width="137"><b>����ģʽ<\/b><\/td>');
if(pageType!=2) document.write('<td class="bh" align="center" width="96"><b>�������֡<\/b><\/td>');
document.write('<td class="bh" align="center" width=""><b>����<\/b><\/td>');
if(pageType !=2 && pageType !=3) document.write('<td class="bh" align="center" width="43"><b>�˿ڱ���<\/b><\/td>' + '<td class="bh" align="center" width=""><b>����<\/b><\/td>');
document.write('<\/tr>');
for (pno = 1; pno <= numPorts; pno++) {
    i = pno - 1;
    if (pno % 2 == 0) {
        document.write('<tr style="background-color:ffffff">');
    } else {
        document.write('<tr class="style18">');
    }
    document.write('<td  class=vncell height="22"  align="center">' + pno + '<\/td>' + '<td  align="center" class=vtable><input type="text" class="input" maxlength="11" size="15" name="portInfo' + pno + '"  value="' + portInfo[pno - 1] + '" onChange="checkName(this,11,\'�˿�����\',1)"><div id="errI' + pno + '"></div><\/td>' + '<td   bgcolor=""align="center"><div style="' + linkStyle[pno - 1] + '">' + curSpeed[pno - 1] + '</div><\/td>');
	if (pageType==2)
	{
		document.write('<td  class=vtable align="center"><select class="input" size="1" name="R' + (0x50 + pno -1).toString(16) + '">' + '<option value="3" ' + modeSelected(pno, 3) + '>�Զ�Э��<\/option>' + '<option value="4" ' + modeSelected(pno, 4) + '>10M(��˫��)<\/option>' + '<option value="0" ' + modeSelected(pno, 0) + '>10M(ȫ˫��)<\/option>' + '<option value="5" ' + modeSelected(pno, 5) + '>100M(��˫��)<\/option>' + '<option value="1" ' + modeSelected(pno, 1) + '>100M(ȫ˫��)<\/option>' + '<option value="6" ' + modeSelected(pno, 6) + '>��ֹ<\/option>' + '<\/select><\/td>');
	}
	else if (pageType==3)
	{
		document.write('<td  class=vtable align="center"><select class="input" size="1" name="R' + (0x50 + pno -1).toString(16) + '">' + '<option value="3" ' + modeSelected(pno, 3) + '>�Զ�Э��<\/option>' + '<option value="4" ' + modeSelected(pno, 4) + '>10M(��˫��)<\/option>' + '<option value="0" ' + modeSelected(pno, 0) + '>10M(ȫ˫��)<\/option>' + '<option value="5" ' + modeSelected(pno, 5) + '>100M(��˫��)<\/option>' + '<option value="1" ' + modeSelected(pno, 1) + '>100M(ȫ˫��)<\/option>' + '<option value="2" ' + modeSelected(pno, 2) + '>1000M(ȫ˫��)<\/option>' + '<option value="6" ' + modeSelected(pno, 6) + '>��ֹ<\/option>' + '<\/select><\/td><td  align="center" class=vtable><input class="input" type="text" name="R' + (0x10 + pno).toString(16) + '" id="R' + (0x10 + pno).toString(16) + '" style="width:50px" onKeyPress="javascript:figureOnly()" maxlength="4" onChange="checkNumber(this, 1518, 9600, \'�������֡\')"><div id="err' + pno + '"></div><\/td>');
	}
	else
	{
		document.write('<td  class=vtable align="center"><select class="input" size="1" name="R' + (0x50 + pno -1).toString(16) + '">' + '<option value="3" ' + modeSelected(pno, 3) + '>�Զ�Э��<\/option>' + '<option value="4" ' + modeSelected(pno, 4) + '>10M(��˫��)<\/option>' + '<option value="0" ' + modeSelected(pno, 0) + '>10M(ȫ˫��)<\/option>' + '<option value="5" ' + modeSelected(pno, 5) + '>100M(��˫��)<\/option>' + '<option value="1" ' + modeSelected(pno, 1) + '>100M(ȫ˫��)<\/option>' + '<option value="2" ' + modeSelected(pno, 2) + '>1000M(ȫ˫��)<\/option>' + '<option value="6" ' + modeSelected(pno, 6) + '>��ֹ<\/option>' + '<\/select><\/td><td  align="center" class=vtable><input class="input" type="text" name="R' + (0x10 + pno).toString(16) + '" id="R' + (0x10 + pno).toString(16) + '" style="width:50px" onKeyPress="javascript:figureOnly()" maxlength="4" onChange="checkNumber(this, 1518, 9600, \'�������֡\')"><div id="err' + pno + '"></div><\/td>');
	}
    document.write('<td align="center"  class=vtable><input type="checkbox" title="����" name="R' + (0xd0 + pno -1).toString(16) + '" value="ON" ' + ( fc[pno - 1] ? "checked" :'') + '><\/td>');
	if (pageType!=2 && pageType!=3)
	{
		if (!portprotect[pno - 1])
		{
			document.write('<td  align="center" class=vtable><input type="checkbox" onClick="rad_click(this)" title="' + [pno] + ':�˿ڱ���" name="R' + (0x70 + pno).toString(16) + '" onclick="chk_onclick(this, ' + pno + ', 1)"><\/td>');
		}
		else
		{
			document.write('<td  align="center" class=vtable><input type="checkbox" onClick="rad_click(this)" title="' + [pno] + ':�˿ڱ���" checked name="R' + (0x70 + pno).toString(16) + '" onclick="chk_onclick(this, ' + pno + ', 0)"><\/td>');
		}
		
		if (!portprotect[pno - 1])
		{
			document.write('<td  align="center" class=vtable><input type="checkbox" onClick="rad_click(this)" ' + (floodings[pno-1]?'checked':'' ) + ' title="' + [pno] + ':����" name="R' + (0x70 + pno).toString(16) + '" onclick="chk_onclick(this, ' + pno + ', 0)"><\/td>');
		}
		else
		{
			document.write('<td  align="center" class=vtable><input type="checkbox" onClick="rad_click(this)" title="' + [pno] + ':����" name="R' + (0x70 + pno).toString(16) + '" onclick="chk_onclick(this, ' + pno + ', 0)"><\/td>');
		}
	}
    document.write('<\/tr>');
}
document.write('<\/table>');
addLoadEvent(init);
addLoadEvent(initHelp);
</script>
<table><tr height="10"><td></td></tr><tr><td width="600" align="center">
<input type="button" value="����" class="input-button" onClick="save()" name="">
<!--<input type="button" value="ˢ��" class="input-button" name="" onClick="location.reload()">-->
<input type="button" class="input-button" value="����" name="" onClick="reSet();">
<input type="button" class="input-button" value="����" name="helpButton"  id="helpButton" onClick="window.open('../rhelpinfo.htm#ports')">
<input type="button" class="input-button" style="display:none" name="goBack" value="����" onClick="go_back();">
</td></tr></table>
</form>
	<!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
  </td><td class="gapTd"></td></tr> </table>
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

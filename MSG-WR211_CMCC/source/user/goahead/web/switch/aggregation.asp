<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>��������-�˿ڻ��</title>
<link rel=stylesheet type="text/css" href="../css/style.css" title="cool" id="lnk">
<script src="../js/common.js" type="text/javascript"></script>
<script src="js/common_switch.js" type="text/javascript"></script>
<script language="javascript">
<%
var macStr = Form("macStr");//Զ���豸mac��ַ
var urcpSet = Form("urcpSet");//�Ƿ��������û�ۺ���ת����ҳ���
if (macStr!="" || urcpSet!="")
{//������Ϣ
	aspOutPutAggrGroupListUrcp();
}
else
{//������Ϣ
        if (0)
	{
	    aspOutPutGetQosRatelimitList();
	}
	else
	{//������ٺ���ʹ�����ϴ�ӡ
	    write('var policer = new Array();');
	    write('var shaper = new Array();');
	}
	aspOutPortConfigure();
	aspOutPutAggrGroupList();
	write('var ipStr="";');
	write('var macStr="";');
	write('var passStr="";');
	write('var urcpError="";');
}
%>
//����
/*var aggGrpNos = new Array("1","2","3");//������
var aggLists  = new Array("2 3","4 5 6","7 8 9 10 11");//������Ա
var aggGrpNames =new Array("name1", "name2", "", "name4", "name5", "name6", "name7", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "");//�������
var mirrorPort = 24;//��ض˿�*/
//var rstp = new Array(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);//�Ƿ���rstp
//var lacp = new Array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1); //�Ƿ���lacp
//var mirrorSource = new Array("checked", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "");
//��ض˿�Դ�˿�
/*var speedMode = new Array("1", "1", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3");//�˿�Э��ģʽ
var maxFrame = new Array("6000", "6000", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518");//�������֡
var portprotect = new Array('1', '1', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none');//�˿ڱ���
var fc = new Array(1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);//����*/
//var floodings = new Array(1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);//����
/*var vlanFlag=0;//0������vlan,1�˿�vlan,2tag vlan
var vlantabs = new Array(new Array("1","1 2 3 4 5"),new Array("2","3 4 "));
var vlanAware = new Array("", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "");//vlan�豸
var frameType = new Array("1", "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0");//tag vlan֡����(����tag,���������Լ�disabled)
var pvid = new Array("2", "2", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1");//pvid
var vlanLists = new Array("1 2","1 2 3");//port vlan
var ingr = new Array("", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "");//������
*/
//end ����
//if(mirrorSource.countData("checked") <1) //�ü�ض˿ڲ������ã���Դ�˿ڣ�
//	mirrorPort = 0;

var portTitle=new Array();//�˿���ʾ��Ϣ
var portDisabled=new Array();//�˿��Ƿ����(�˿�����ҳ�еĶ˿�)
var isCheckAggr = 1;/*�ı����õ�ʱ���Ƿ���鱾�β��������vlan�Ϸ�*/
var aggArr=new Array();
var vTagLists=new Array();
var vlanIds=new Array();
var tmparray = null;
function init_global_var() {
    var i = 0,
    j = 0;
    for (i = 0; i < numPorts; i++) {
        portDisabled[i] = "";
        portTitle[i] = "";
        if (lacp[i]) {
            portDisabled[i] = "disabled"; //lacp�˿�
            portTitle[i] = "LACP�ѿ���,�������û��"; //lacp�˿�
        } else if (rstp[i]) {
            portDisabled[i] = "disabled";
            portTitle[i] = "RSTP�ѿ������������û��";
        }

    }

    for (i = 0; i < aggLists.length; i++) {
        aggArr[i] = aggLists[i].split(" ");
        for (j = 0; j < aggArr[i].length; j++) {
            portTitle[(aggArr[i][j] - 1)] = "���ڻ����" + aggGrpNos[i];
        }
    }
    if (parseInt(mirrorPort) > 0) {
        if (portDisabled[parseInt(mirrorPort) - 1] == "disabled") {
            portTitle[parseInt(mirrorPort) - 1] += "&#13;&#10;";
        } //��ض˿�
        else {
            portDisabled[parseInt(mirrorPort) - 1] = "disabled";
        }
        portTitle[parseInt(mirrorPort) - 1] += "��ض˿ڣ��������û��"; //��ض˿�
    }
    if (vlanFlag == 2) {
        tmparray = new Array();
        for (var i = 0; i < vlantabs.length; i++) {
            if (vlantabs[i] != null) {
                vTagLists[i] = vlantabs[i][1].split(" ");
				vlanIds[i] = vlantabs[i][0];
            }
        }
    }
    /*tmparray = new Array();
for (var i = 0; i < vlanGrpNos.length; i++) {
    if (vlanGrpNos[i] != null) {
        tmparray[i] = vlanGrpNos[i];
    }
}
vlanGrpNos = tmparray.slice();*/
    if (vlanFlag == 1) {
        tmparray = new Array();
        for (var i = 0; i < vlanLists.length; i++) {
            if (vlanLists[i] != null) {
                tmparray = vlanLists[i].split(" ");
                vlanLists[i] = new Array();
                vlanLists[i] = tmparray.slice(0);
            }
        }
    }
}
//tmparray = new Array();
//for (var i = 0; i < vlanLists.length; i++) {
 //   if (vlanLists[i] != null) {
//        tmparray[i] = vlanLists[i];
 //   }
//}
//vlanLists = tmparray.slice();
/*��ʼ��������Ϣ������vlan�����л�ʱ�����µ����ô���*/
function init_erro() {
    var flag = 0,
    no = 0;
	var error_index=0;
	var vlanArr = null;
    if (errorstr) no = 1;
	if(vlanFlag == 0) return;
	if(vlanFlag == 1) vlanArr = vlanLists;
	else if(vlanFlag == 2) vlanArr = vTagLists;
	
	j = checkAggrVlan(aggArr,vlanArr);
    if (j>= 0) {
        if (no == 1) {
            errorstr = (no++) + ". " + errorstr + "<br> " + (no++) + ".";
        } else {
            no = 1;
        }
        errorstr += "���ô��󣺻����"+aggGrpNos[j]+"�ڵĶ˿ڲ���ȫ������һVLAN";
        isCheckAggr = 0;
    }
	if(vlanFlag != 2) return;//��tag vlan
    for (var i = 0; ! flag && i < aggArr.length; i++) {
        for (var j = 0; ! flag && j < aggArr[i].length; j++) {
            for (var k = j + 1; ! flag && k < aggArr[i].length; k++) {
                if (vlanAware[aggArr[i][j] - 1] != vlanAware[aggArr[i][k] - 1]) {
                    flag = 1;
					error_index = i;
                    break;
                }
                if (frameType[aggArr[i][j] - 1] != frameType[aggArr[i][k] - 1]) {
                    flag = 1;
					error_index = i;
                    break;
                }
                if (pvid[aggArr[i][j] - 1] != pvid[aggArr[i][k] - 1]) {
                    flag = 1;
					error_index = i;
                    break;
                }
            }
        }
    }
    if (flag) {
        if (no > 0) {
            if (no == 1) {
                errorstr = (no++) + ". " + errorstr + "<br> " + (no++) + ".";
            }

            else {
                errorstr += "<br> "+(no++)+".";
            }
        }
         errorstr += "���ô��󣺻����"+aggGrpNos[error_index]+"�ڵĶ˿ھ��в�ͬ��TAG VLAN�˿�����";
    }
}
init_global_var();
init_erro();
var arrayData1 = new Array(aggGrpNos,aggGrpNames, aggLists);
function arrayData1AscSort()
{
	var temp;
	for (var I = 0; I < arrayData1[0].length - 1; I++) {
		for (var j = 1; j < arrayData1[0].length - I; j++) {
			if(parseInt(arrayData1[0][j])<parseInt(arrayData1[0][j-1]))
			{
				for (var k = 0; k < arrayData1.length; k++)
				{
					temp = arrayData1[k][j];
					arrayData1[k][j] = arrayData1[k][j - 1];
					arrayData1[k][j - 1] = temp;
				}
			}
	
		}
	}
}
arrayData1AscSort();
var titles1 = new Array("������","���������", "������Ա");
var total_recs = aggGrpNos.length;
var max_totalrecs = numPorts;
currpage[0] = 1;
recscount[0] = total_recs;
maxrecs[0] = max_totalrecs;
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] = "�˿ڻ���б�";
sortable[0] = new Array(1,1,1);
sortType[0]=new Array(TYPE_NUM,TYPE_CHINESE,TYPE_STR);
arrayCount[0] = 3;
recsPerPage[0] = 10; //ÿҳ����������
lineButton[0] = 0;
function dsplist(index, list) {
    if (index == 0) dspList(index, list, -1, titles1, titles1.length, arrayData1, 1, 1);
}
function go_back()
{
	AGGR.action="remote_manage.asp";
	AGGR.submit();
}
/****
���� �˿ڻ������ ѡ��� �˿ڻ����ŵ�ֵ(����Ϊ��)
�ı䲻��ѡ�˿�(�ѿ���lacp��)״̬Ϊdisabled
****/
function disabledChk() {
	var exceptIndex = -1;
	 if (AGGR.actions[1].checked) //�޸�
    {
        exceptIndex=eval(aggGrpNos.searchData(AGGR.aggNo.value));
    }
    var array = null;
    for (var i = 0; i < numPorts; i++) {
        document.getElementById('chkPort'+i).disabled = false;
    }
    for (var i = 0; i < numPorts; i++) {
        if (portDisabled[i] == "disabled") //����lacp
        {
            document.getElementById('chkPort'+i).disabled = true;
            continue;
        }
    }
    for (var i = 0; i < aggLists.length; i++) {
        if (i == exceptIndex) continue;
        array = trimstr(aggLists[i]).split(" ");
        for (var j = 0; j < array.length; j++) {
            document.getElementById('chkPort'+ (array[j] - 1) ).disabled = true;
        }

    }

}
function fillData(index) {
	for (i = 0; i < numPorts; i++) {
        document.getElementById('chkPort'+i).checked = false;
    }
    if (parseInt(index) < 0) {
		//���
        AGGR.actions[0].checked = true;
        disabledChk();
        return;
    }
    var array = new Array();
    AGGR.actions[1].checked = true;
    AGGR.aggNo.value = aggGrpNos[index];
	AGGR.aggName.value = aggGrpNames[index];
    disabledChk();
    array = trimstr(aggLists[index]).split(" ");
    for (var j = 0; j < array.length; j++) {
        document.getElementById('chkPort'+ (array[j] - 1) ).checked = true;
    }
}

/**
�޸�ʱ����aggNo��ֵ���������
**/
function fillData2()
{
	var index = aggGrpNos.searchData(document.getElementById('aggNo').value);	
	var gno = document.getElementById('aggNo').value; //������
    index = aggGrpNos.searchData(gno);
	if(index < 0) 
	{
		 for (i = 0; i < numPorts; i++) 
		{
        	document.getElementById('chkPort'+i).checked = false;
			document.getElementById('chkPort'+i).disabled = true;
    	}
		inputErro(document.getElementById('aggNo'),'�Ƿ��޸�:�û����Ų�����');
		return;
	}
	else
	{
		inputRight(document.getElementById('aggNo'));
		inputRight(document.getElementById('aggName'));
		fillData(index);
	}
}
function radioClick()
{
	if(!checkNumber(document.getElementById('aggNo'), 1, numPorts, '',0,1)) return;
	if(AGGR.actions[1].checked) fillData2();//�޸�
	else//���
	{
		inputRight(document.getElementById('aggNo'));//ȥ���޸Ĳ����Ĵ�����ʾ��Ϣ
		fillData(-1);
	}
}

function onAggNoChange()
{
		if(!checkNumber(document.getElementById('aggNo'), 1, numPorts, '������'))
			return ;
		if(AGGR.actions[1].checked) fillData2() ;//�޸�
}
function EditConfig(val, i) {
	document.getElementById('aggNo').value='';
	document.getElementById('aggName').value='';
	inputRight(document.getElementById('aggNo'));;//��ʼ��������ʾ��Ϣ
	inputRight(document.getElementById('aggName'));;
    if (val == 0) 
	{
		fillData( - 1); //���
	}
    else fillData(i); //�޸�
    switchDiv(3);

}
function DeleteConfig(index) {
    if (confirm("ȷ��ɾ����")) {
        AGGR.delstr.value = aggGrpNos[index];
        AGGR.action = (macStr==""?"/goform/formAggrGroupDel":"/goform/formAggrGroupDelUrcp");
        AGGR.submit();
    }
}
function delFunc() {
    var array = null;
    if (genData(AGGR, 0, frame12, aggGrpNos, '�˿ڻ��', ",")) {
		 if (confirm("ȷ��ɾ����")) {
        AGGR.action = (macStr==""?"/goform/formAggrGroupDel":"/goform/formAggrGroupDelUrcp");
        AGGR.delstr.value = AGGR.delstr.value.slice(0, AGGR.delstr.value.length - 1);
        AGGR.submit();
		 }
    }
}
function save() {
    var i = 0,
    j = 0,
    k = 0,
    x = 0,
    y = 0;
    var full_flag = 0;
    var gno = document.getElementById('aggNo').value; //������
    if (!checkNumber(document.getElementById('aggNo'), 1, numPorts, '������')) return;
    i = aggGrpNos.searchData(gno);
    if (AGGR.actions[0].checked && i >= 0) {
        inputErro(document.getElementById('aggNo'), '�Ƿ����:�û������Ѿ�����');
        return;
    }
    if (AGGR.actions[1].checked && i < 0) {
        inputErro(document.getElementById('aggNo'), '�Ƿ��޸�:�û����Ų�����');
        return;
    }
    inputRight(document.getElementById('aggNo'));
    if (!checkName(document.getElementById('aggName'), 11, "���������", 1)) {
        return;
    }
    //if(gno =='') {alert("�����Ų���Ϊ��");return;}
    //if( parseInt(gno) <1 || parseInt(gno) >24 ) { alert("������ӦΪ1��24֮�������");return;}

    var num = 0;
    var tmps = new Array();

    var curPort = 0;
    for (i = 0; i < numPorts; i++) {
        if (document.getElementById('chkPort' + i).checked) {
            curPort = i + 1;
            /* if (curPort == mirrorPort) {
                alert("������ڲ��ܳ��ּ�ض˿�:" + mirrorPort + "!");
                return;
            } */
            tmps[j++] = curPort;
            num += 1 << i;
        }
    }
	if (tmps.length > 16) {
		alert("ͬһ������������֧��16���˿�");
		return;
	}
    if (num == 0) {
        alert("������ѡ��һ����Ա!");
        return;
    }
    AGGR.chkstr.value = num; //����Ľ��
    if (vlanFlag == 1 && tmps.length > 1) { //port vlan
        if (checkAggrVlan(new Array(tmps), vlanLists)>=0) {
            alert("������ڶ˿����ò�һ�£���\r\n\r\n(--�����˿ڱ�����ȫ����ĳһVLAN--)");
            return;
        }
        /*	full_flag=0;//�Ƿ���ȫ����һ��vlan
			for (y = 0; y < vlanLists.length; y++) {
				i=isArrayFullInclude(vlanLists[y],tmps);
				if (i == 1) {//��ȫ����
				   full_flag=1;
				   break;
				}
			}
			if(!full_flag)//�û���鲻��ȫ�����κ�һ��vlan
			{
				 alert("������ڶ˿����ò�һ�£���\r\n\r\n(--�����˿ڱ�����ͬһVLAN��--)");
				 return;
			}*/
    }
    if (vlanFlag == 2 && tmps.length > 1) //tag vlan
    {
        if (checkAggrVlan(new Array(tmps), vTagLists)>=0) {
            alert("������ڶ˿����ò�һ�£���\r\n\r\n(--�����˿ڱ�����ȫ����ĳһVLAN--)");
            return;
        }
    }
    for (j = 0; j < tmps.length; j++) {
        for (var k = j + 1; k < tmps.length; k++) {
			if (policer[tmps[j] - 1] != policer[tmps[k] - 1]) {
				alert("������ڶ˿����ò�һ�£���\r\n\r\n(--�����˿ڱ���������ͬ����������--)");
			return;
			}
			if (shaper[tmps[j] - 1] != shaper[tmps[k] - 1]) {
				alert("������ڶ˿����ò�һ�£���\r\n\r\n(--�����˿ڱ���������ͬ����������--)");
			return;
			}
            if (fc[tmps[j] - 1] != fc[tmps[k] - 1]) {
                alert("������ڶ˿����ò�һ�£���\r\n\r\n(--�����˿ڱ���������ͬ������--)");
                return;
            }
            if (floodings[tmps[j] - 1] != floodings[tmps[k] - 1]) {
                alert("������ڶ˿����ò�һ�£���\r\n\r\n(--�����˿ڱ���������ͬ�ķ���--)");
                return;
            }
            if (speedMode[tmps[j] - 1] != speedMode[tmps[k] - 1]) {
                alert("������ڶ˿����ò�һ�£���\r\n\r\n(--�����˿ڱ���������ͬ�Ķ˿�Э��ģʽ--)");
                return;
            }
            if (maxFrame[tmps[j] - 1] != maxFrame[tmps[k] - 1]) { 
                alert("������ڶ˿����ò�һ�£���\r\n\r\n(--�����˿ڱ���������ͬ������С--)");
                return;
            }
            if (portprotect[tmps[j] - 1] != portprotect[tmps[k] - 1]) {
                alert("������ڶ˿����ò�һ�£���\r\n\r\n(--�����˿ڱ���������ͬ�Ķ˿ڱ���--)");
                return;
            }
            if (vlanFlag == 2) //tag vlan
            {
                if (vlanAware[tmps[j] - 1] != vlanAware[tmps[k] - 1]) {
                    alert("�������TAG VLAN���ò�һ�£���\r\n\r\n(--�����˿ڱ���������ͬ��VLAN�豸--)");
                    return;
                }
                if (ingr[tmps[j] - 1] != ingr[tmps[k] - 1]) {
                    alert("�������TAG VLAN���ò�һ�£���\r\n\r\n(--�����˿ڱ���������ͬ��VLAN����--)");
                    return;
                }
                if (frameType[tmps[j] - 1] != frameType[tmps[k] - 1]) {
                    alert("�������TAG VLAN���ò�һ�£���\r\n\r\n(--�����˿ڱ���������ͬ��֡����--)");
                    return;
                }
                if (pvid[tmps[j] - 1] != pvid[tmps[k] - 1]) {
                    alert("�������TAG VLAN���ò�һ�£���\r\n\r\n(--�����˿ڱ���������ͬ��PVID--)");
                    return;
                }
            }
        }
    }
	if (vlanFlag == 2 && tmps.length > 1) //tag vlan
	{
		for (y = 0; y < vTagLists.length; y++) {
			i = isArrayFullInclude(vTagLists[y], tmps);
			if (i == 0) { //���ְ���
				if (!confirm("ĳЩTAG VLAN����TAG VLAN "+vlanIds[y]+"�����ְ����û�����Ա��ȷ��������")) {
					return;
				}
				break;
			}
		}
	}
    AGGR.action = (macStr==""?"/goform/formAggrGroup":"/goform/formAggrGroupUrcp");
    AGGR.submit();
}
function initHelp()
{
	if(macStr!='')
	{
		var obj = document.getElementsByName('helpButton');
		for(var i =0 ;i < obj.length; i++)
		{
	        obj[i].onclick = function() {window.open('../rhelpinfo.htm?#aggrUrcp')};
		}
	}
}
/**/
function init() {
	//����Ϊ���ģʽ
	var obj=null;
	AGGR.passStr.value = passStr;
	AGGR.ipStr.value= ipStr;
	AGGR.macStr.value= macStr;
	AGGR.numPorts.value= numPorts;
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
    AGGR.actions[0].checked = true;
    fillData(-1);
	frame11.location.href="_listhead2.htm";//ע������ڴ˴���ʼ��������ҳ��ˢ�µ�ʱ����ܳ���bug
	frame12.location.href="_list1.htm";//���������ˢ�� ҳ����������Ƿ�����
}
</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="../images/body_main.png" style="background-repeat:repeat-x">
<script language="javascript">urcpErrorPost("remote_manage.asp");</script>
<table width="100%" border="0" cellspacing="0" cellpadding="0" >
<!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****�м�����****-->
  <tr>
   <td width="15"></td>
  <!--////����-->
  <td class="table_bg"><script language="javascript">showSecondTitle('�˿ڻ��');//��ʾҳ������</script>
   <table width="80%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  >
   <script language="javascript">showerror(errorstr,100);//��ʾ���淵�صĴ�����Ϣ</script>
  <tr valign="top">
    <td>
	<!--//�����˵�-->
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"100%",
						2,
						new Array('�˿ڻ���б�','agg_list',1,1),
						new Array('�˿ڻ������','agg_edit',1,0)
		);
		showThreeMenuNotCommit();
	</script>
  <!--//end-->
  <table width="100%" border="0" cellspacing="0" cellpadding="0" align="center"><tr>
    <td align="center"  class="three_td"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr><td><table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td>
	<!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
	<br>
<div class="inside_div">
<form method="POST" action="" name="AGGR">


<input type="hidden" value="Apply" name="_submit">

<input type="hidden" name="chkstr"><!-- ��ѡ�е�checkbox -->
<input type="hidden" name="delstr">
<input type="hidden" value="" name="ipStr">
<input type="hidden" value="" name="macStr">
<input type="hidden" value="" name="passStr">
<input type="hidden" value="" name="numPorts">

<div id="agg_list" style="display:block">
<table width="600"  border="0" align="center" cellpadding="0" cellspacing="0">


<tr>
<td colspan="2" height="" align="center" valign="bottom">
<iframe  scrolling="no" name="frame11" valign="bottom" align="center" width="600" height="44" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe>	  </td>
</tr>
<tr>
<td colspan="2" align="center" valign="top">
<iframe  name="frame12" scrolling="no" id="framelist" valign="top" align="MIDDLE" width="600" height="248" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe></td>
</tr>
<tr>
<td width="379" height="21"><input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'))>
ȫѡ / ȫ��ѡ</td>
<td width="221"  align="right">	   
<input type="button" class="input-button" onClick="EditConfig(0)" value="�������Ŀ">
<input type="button" class="input-button" onClick="delFunc()" value="ɾ��">
<input type="button" class="input-button" style="display:none" name="goBack" value="����" onClick="go_back();"></td>
</tr>
</table>
</div>
<div id="agg_edit" style="display:none">
<TABLE   width="600" align="center" border=0>
<tr height="30">
<td  align="left" width="340">
������<input type="text" class="input" name="aggNo" id="aggNo" onKeyPress="javascript:figureOnly()" size="4" maxlength="2" onChange="onAggNoChange();">&nbsp;
���������<input type="text" class="input" name="aggName" id="aggName" value="" size="18" maxlength="11" onChange="checkName(this,11,'���������',1)"></td>
<td width="160" align="left"><span id="aggteam"></span>&nbsp;</td>
<td width="100" align="right">���<input type="radio" checked="checked" name="actions" value="add" onClick="radioClick()">&nbsp;�޸�<input type="radio" name="actions" value="modify" onClick="radioClick();"></td>
</tr>
<tr><td width="" height="25" valign="middle"  colspan="3">
<script>
//���ɶ˿�agg�༭�б�

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
document.write('<td width="" rowspan="" title="'+portTitle[pno-1]+'" align="center"><input type="checkbox" id="chkPort'+ (pno-1) +'" name="chkPort'+ (pno-1) +'"  '+portDisabled[pno-1]+' style="" maxlength="4"><\/td>');	
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
document.write('<td width="" rowspan="" align="center" title="'+portTitle[pno-1]+'"><input type="checkbox" id="chkPort'+ (pno-1) +'" name="chkPort'+ (pno-1) +'"  '+portDisabled[pno-1]+' style="" maxlength="4"><\/td>');	
}
document.write('</tr>');
document.write('</table>');

</script>
</td></tr>
</TABLE>
<table width="600" cellpadding="4" cellspacing="1" align="center">
<tr height="10"><td></td></tr>
<tr><td align="center">
<input type="button" class="input-button" value="����" onClick="javascript:save()"> 
<input type="button" class="input-button" value="����" onClick="radioClick();">
<input type="button" id="helpButton" name="helpButton" class="input-button" value="����" onClick="window.open('../rhelpinfo.htm#aggr')">
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

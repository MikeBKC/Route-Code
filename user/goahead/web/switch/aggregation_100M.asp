<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>��������-�˿ڻ��</title>
<link rel=stylesheet type="text/css" href="../css/style.css" title="cool" id="lnk">
<script src="../js/common.js" type="text/javascript"></script>
<script src="js/common_switch.js" type="text/javascript"></script>
<script language="javascript">
//����
<%
var macStr = Form("macStr");//Զ���豸mac��ַ
var urcpSet = Form("urcpSet");//�Ƿ��������û�ۺ���ת����ҳ���
if (macStr!="" || urcpSet!="")
{//������Ϣ
	aspOutPutAggrGroupListUrcp();
}
else
{//������Ϣ
	//aspOutPutGetQosRatelimitList();
	aspOutPortConfigure();
	aspOutPutAggrGroupList();
	write('var ipStr="";');
	write('var macStr="";');
	write('var passStr="";');
	write('var urcpError="";');
}
%>
var urcpError="";
var vlanFlag = 1;

//var vlanLists = new Array();
//����
//var aggGrpNos = new Array("1","2","3");//������
//var aggLists  = new Array("2 3","4 5 6","7 8 9 10 11");//������Ա
//var aggGrpNames =new Array("name1", "name2", "", "name4", "name5", "name6", "name7", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "");//�������
//var mirrorPort = 24;//��ض˿�
//var rstp = new Array(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);//�Ƿ���rstp
//var lacp = new Array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1); //�Ƿ���lacp
//var mirrorSource = new Array("checked", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "");
//��ض˿�Դ�˿�
//var speedMode = new Array("1", "1", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3");//�˿�Э��ģʽ
//var maxFrame = new Array("6000", "6000", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518");//�������֡
//var portprotect = new Array('1', '1', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none');//�˿ڱ���
//var fc = new Array(1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);//����
//var floodings = new Array(1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);//����
//var vlanFlag=0;//0������vlan,1�˿�vlan,2tag vlan
//var vlantabs = new Array(new Array("1","1 2 3 4 5"),new Array("2","3 4 "));
//var vlanAware = new Array("", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "");//vlan�豸
//var frameType = new Array("1", "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0");//tag vlan֡����(����tag,���������Լ�disabled)
//var pvid = new Array("2", "2", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1");//pvid
//var vlanLists = new Array("1 2","1 2 3");//port vlan
//var ingr = new Array("", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "");//������

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
	/*
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

    }*/

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
/*	
	j = checkAggrVlan(aggArr,vlanArr);
    if (j>= 0) {
        if (no == 1) {
            errorstr = (no++) + ". " + errorstr + "<br> " + (no++) + ".";
        } else {
            no = 1;
        }
        errorstr += "���ô��󣺻����"+aggGrpNos[j]+"�ڵĶ˿ڲ���ȫ������һVLAN";
        isCheckAggr = 0;
    }*/
	if(vlanFlag != 2) return;//��tag vlan
    for (var i = 0; ! flag && i < aggArr.length; i++) {
        for (var j = 0; ! flag && j < aggArr[i].length; j++) {
            for (var k = j + 1; ! flag && k < aggArr[i].length; k++) {
               /* if (vlanAware[aggArr[i][j] - 1] != vlanAware[aggArr[i][k] - 1]) {
                    flag = 1;
					error_index = i;
                    break;
                }
                if (frameType[aggArr[i][j] - 1] != frameType[aggArr[i][k] - 1]) {
                    flag = 1;
					error_index = i;
                    break;
                }*/
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
var max_totalrecs = 2;
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
		document.getElementById('chkPort'+i).checked = false;
        document.getElementById('chkPort'+i).disabled = false;
    }
	/*
    for (var i = 0; i < numPorts; i++) {
        if (portDisabled[i] == "disabled") //����lacp
        {
            document.getElementById('chkPort'+i).disabled = true;
            continue;
        }
    }*/
	var start_id = parseInt(document.getElementById('selec').value*4) + 1;
    for (var i = 0; i < aggLists.length; i++) {
        if (i == exceptIndex) continue;
        array = trimstr(aggLists[i]).split(" ");
        for (var j = 0; j < array.length; j++) {
			if(array[j]>= start_id && array[j]<(start_id +4))
            document.getElementById('chkPort'+ (array[j]  - start_id) ).disabled = true;
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
	else
	{
        AGGR.actions[1].checked = true;
		for (var i = 0; i < numPorts; i++) {
        	document.getElementById('chkPort'+i).disabled = false;
    	}
	}
    //AGGR.actions[1].checked = true;
    AGGR.aggNo.value = aggGrpNos[index];
	AGGR.aggName.value = aggGrpNames[index];
    //disabledChk();
    var array = new Array();
    array = trimstr(aggLists[index]).split(" ");
    //for (var j = 0; j < 4; j++) {
        //document.getElementById('chkPort' + j).checked = true;
    //for (var j = 0; j < array.length; j++) {
	var j = 0;
	if (array[j] >= 1 && array[j] <= 4)
	{
		document.getElementsByName('selec')[0].value = 0;
	}
	else if (array[j] >= 5 && array[j] <= 8)
	{
		document.getElementsByName('selec')[0].value = 1;
	}
	else if (array[j] >= 9 && array[j] <= 12)
	{
		document.getElementsByName('selec')[0].value = 2;
	}
	else if (array[j] >= 13 && array[j] <= 16)
	{
		document.getElementsByName('selec')[0].value = 3;
	}
	else if (array[j] >= 17 && array[j] <= 20)
	{
		document.getElementsByName('selec')[0].value = 4;
	}
	else
	{
		document.getElementsByName('selec')[0].value = 5;
	}
	showSuitablePortNo();
	for (; j < array.length; j++) {
		var k = (array[j]-1)%4;
        document.getElementById('chkPort' + (k)).checked = true;
    }
	var index = parseInt(AGGR.selec.value) * 4;
	document.getElementById("chkPort0").name = "chkPort"+(index+0);
	document.getElementById("chkPort1").name = "chkPort"+(index+1);
	document.getElementById("chkPort2").name = "chkPort"+(index+2);
	document.getElementById("chkPort3").name = "chkPort"+(index+3);
}

/**
�޸�ʱ����aggNo��ֵ���������
**/
function fillData2()
{
	//var index = aggGrpNos.searchData(document.getElementById('aggNo').value);	
	var gno = document.getElementById('aggNo').value; //������
    var index = aggGrpNos.searchData(gno);
	if(index < 0) 
	{
		 for (i = 0; i < numPorts; i++) 
		{
        	document.getElementById('chkPort'+i).checked = false;
			//document.getElementById('chkPort'+i).disabled = true;
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
	if(!checkNumber(document.getElementById('aggNo'), 1, 2, '',0,1)) return;
	if(AGGR.actions[1].checked) fillData2();//�޸�
	else//���
	{
		inputRight(document.getElementById('aggNo'));//ȥ���޸Ĳ����Ĵ�����ʾ��Ϣ
		fillData(-1);
	}
	for (var j = 0; j < numPorts; j++) {
		document.getElementById('chkPort'+ j).disabled = false;
	}
}

function onAggNoChange()
{
		if(!checkNumber(document.getElementById('aggNo'), 1, 2, '������'))
			return ;
		if(AGGR.actions[1].checked) fillData2() ;//�޸�
}
function EditConfig2(val, i) {
	if (aggGrpNos.length==2)
	{
		alert("�������Ŀ�Ѿ��ﵽ���ֵ��2");
		AGGR.actions[1].checked = true;
		return;
	}
	document.getElementById('aggNo').value='';
	document.getElementById('aggName').value='';
	inputRight(document.getElementById('aggNo'));;//��ʼ��������ʾ��Ϣ
	inputRight(document.getElementById('aggName'));;
    if (val == 0) 
	{
		fillData( - 1); //���
	}
    else
	{
		fillData(i);
	}
    switchDiv(3);
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
    else
	{
		fillData(i);
	}
    switchDiv(3);
	showSuitablePortNo();
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
	//choice();
    var i = 0,
    j = 0,
    k = 0,
    x = 0,
    y = 0;
	y = checkStr();
	if (y == -1) 
	{
		alert("��������ƺ��зǷ��ַ���<����>����");
		return;
	}
	
    var full_flag = 0;
    var gno = document.getElementById('aggNo').value; //������
    if (!checkNumber(document.getElementById('aggNo'), 1, 2, '������')) return;	//��������żȲ���1Ҳ����2���򷵻�
    i = aggGrpNos.searchData(gno);	//��������Ļ������ڵ�ǰ������������Ƿ���ڣ��������򷵻������е����������������򷵻�-1
    if (AGGR.actions[0].checked && i >= 0) {	//ѡ����ӡ����û����Ŵ��ڵ�����±���������
        inputErro(document.getElementById('aggNo'), '�Ƿ����:�û������Ѿ�����');
        return;
    }
    if (AGGR.actions[1].checked && i < 0) {	//ѡ���޸ġ��������Ų����ڵ�����±���������
        inputErro(document.getElementById('aggNo'), '�Ƿ��޸�:�û����Ų�����');
        return;
    }
    inputRight(document.getElementById('aggNo'));	//�����󡰻����š��Ĳ�����ʽ���
/*    if (!checkName(document.getElementById('aggName'), 11, "���������", 1)) {
        return;
    }
*/	
    //if(gno =='') {alert("�����Ų���Ϊ��");return;}
    //if( parseInt(gno) <1 || parseInt(gno) >2 ) { alert("������ӦΪ1��2");return;}
	//if(document.getElementById('aggName').value =='') { alert("��������Ʋ���Ϊ��");return;}

    var num = 0;
    var tmps = new Array();
	var curPort = 0;

	var selVal = document.getElementsByName('selec')[0].value
	for (i = 0; i < numPorts; i++)
	{
		if (mirrorPort == 0) break;
		if (document.getElementById('chkPort' + i).checked)
		{
			curPort = selVal*4 + i + 1;
		}
		if (curPort == mirrorPort)
		{
			alert("������ڲ��ܳ��ּ�ض˿�: �� " + mirrorPort + " �Ŷ˿�!");
			return;
		}
	}
	
    curPort = 0;
    for (i = 0; i < numPorts; i++) {
        if (document.getElementById('chkPort' + i).checked) {
            curPort = i + 1;
/*            if (curPort == mirrorPort) {
                alert("������ڲ��ܳ��ּ�ض˿�:" + mirrorPort + "!");
                return;
            } 
*/			
			//j = 0;
            tmps[j++] = curPort + selVal*4;
            num += 1 << i;
        }
    }
/*	if (tmps.length > 16) {
		alert("ͬһ������������֧��16���˿�");
		return;
	}
*/	
    if (num == 0) {
        alert("������ѡ��һ����Ա!");
        return;
    }
	
	var m = 0;
	for (i = 0; i < 4; i++)
	{
		if (document.getElementById('chkPort'+i).checked == true) m++;
	}	
	if (m < 2)
	{
		alert("������ѡ��2���˿ڣ�");
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
	/*
    if (vlanFlag == 2 && tmps.length > 1) //tag vlan
    {
        if (checkAggrVlan(new Array(tmps), vTagLists)>=0) {
            alert("������ڶ˿����ò�һ�£���\r\n\r\n(--�����˿ڱ�����ȫ����ĳһVLAN--)");
            return;
        }
    }*/
    for (j = 0; j < tmps.length; j++) {
        for (var k = j + 1; k < tmps.length; k++) {
			/*
			if (policer[tmps[j] - 1] != policer[tmps[k] - 1]) {
				alert("������ڶ˿����ò�һ�£���\r\n\r\n(--�����˿ڱ���������ͬ����������--)");
			return;
			}
			if (shaper[tmps[j] - 1] != shaper[tmps[k] - 1]) {
				alert("������ڶ˿����ò�һ�£���\r\n\r\n(--�����˿ڱ���������ͬ����������--)");
			return;
			}*/
            if (fc[tmps[j] - 1] != fc[tmps[k] - 1]) {
                alert("������ڶ˿����ò�һ�£���\r\n\r\n(--�����˿ڱ���������ͬ������--)");
                return;
            }
			/*
            if (floodings[tmps[j] - 1] != floodings[tmps[k] - 1]) {
                alert("������ڶ˿����ò�һ�£���\r\n\r\n(--�����˿ڱ���������ͬ�ķ���--)");
                return;
            }*/
            if (speedMode[tmps[j] - 1] != speedMode[tmps[k] - 1]) {
                alert("������ڶ˿����ò�һ�£���\r\n\r\n(--�����˿ڱ���������ͬ�Ķ˿�Э��ģʽ--)");
                return;
            }
           /* if (maxFrame[tmps[j] - 1] != maxFrame[tmps[k] - 1]) { 
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
            }*/
        }
    }
/*	if (vlanFlag == 2 && tmps.length > 1) //tag vlan
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
*/	
    AGGR.action = (macStr==""?"/goform/formAggrGroup":"/goform/formAggrGroupUrcp");
    AGGR.submit();
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
	}
    AGGR.actions[0].checked = true;
//    fillData(-1);
	frame11.location.href="_listhead2.htm";//ע������ڴ˴���ʼ��������ҳ��ˢ�µ�ʱ����ܳ���bug
	frame12.location.href="_list1.htm";//���������ˢ�� ҳ����������Ƿ�����
	choice();
}
function initHelp()
{
	if(macStr!='')
	{
		var obj = document.getElementsByName('helpButton');
		for(var i =0 ;i < obj.length; i++)
		{
	        obj[i].onclick = function() {window.open('../rhelpinfo.htm#aggrUrcp')};
			//alert(obj[i].onclick)
		}
	}
}
function reset()
{
	alert("enter reset");
	for (var i = 0; i < numPorts; i++) {
        document.getElementById('chkPort'+i).disabled = false;
    }
	alert("disabled = false exceed");
	var aggNo = document.getElementById('aggNo');
	var aggName = document.getElementById('aggName');
	aggNo.setAttribute(value,"");
	aggName.setAttribute(value,"");
	var i;
	for (i = 0; i < 4; i++)
	{
		document.getElementById("chkPorti" + i).checked = false;
	}
}
function checkStr()
{
	var aggNameStr=document.getElementById('aggName').value;
	if ((aggNameStr.indexOf("<") + 1) || aggNameStr.indexOf(">") + 1) 
	{
		return -1;
	}
	else
	{
		return 0;
	}
}
addLoadEvent(init);
addLoadEvent(initHelp);
</script>
</head>
<body onbeforeunload="logOut()" background="../images/body_main.png" style="background-repeat:repeat-x">
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
<input type="button" class="input-button" onClick="EditConfig2(0)" value="�������Ŀ">
<input type="button" class="input-button" onClick="delFunc()" value="ɾ��">
<input type="button" class="input-button" style="display:none" name="goBack" value="����" onClick="go_back();"></td>
</tr>
</table>
</div>
<div id="agg_edit" style="display:none">
<TABLE   width="600" align="center" border=0>
<tr height="30">
<td  align="left" width="340">
������<input type="text" class="input" name="aggNo" id="aggNo" onKeyPress="javascript:figureOnly()" size="4" maxlength="1" onChange="onAggNoChange();">&nbsp;
���������<input type="text" class="input" name="aggName" id="aggName" value="" size="18" maxlength="11" onChange="checkStr()"></td>
<td width="160" align="left"><span id="aggteam"></span>&nbsp;</td>
<td width="100" align="right">���<input type="radio" checked="checked" name="actions" value="add" onClick="EditConfig2(0)">&nbsp;�޸�<input type="radio" name="actions" value="modify" onClick="radioClick();"></td>
</tr>
<tr><td width="" height="25" valign="middle"  colspan="3">
<!--���ɶ˿�agg�༭�б�-->
<table width="600" border="0" cellpadding="4" cellspacing="1" class="style22" align="center">
<tr class=style18>
<td align="center" width="20" rowspan="4"><div align="center"  color="#000000"><b>��<br />Ա</b></div></td>
<td width="679" colspan="4"><div align="center">
	<select name="selec" id="selec" onChange="choice()">
		<!--<option selected="selected" value="0">���1���˿�1��4��</option>-->     
        <option value="0">���1���˿�1��4��</option>   
		<option value="1">���2���˿�5��8��</option>
        <option value="2">���3���˿�9��12��</option>
        <option value="3">���4���˿�13��16��</option>
        <option value="4">���5���˿�17��20��</option>
        <option value="5">���6���˿�21��24��</option>
	</select>
</div></td>
</tr>
<tr>
    <td align="center" id="port1">&nbsp;</td>
    <td align="center" id="port2">&nbsp;</td>
    <td align="center" id="port3">&nbsp;</td>
    <td align="center" id="port4">&nbsp;</td>
</tr>
<tr>
<script language="javascript">
function choice()
{
	var index = parseInt(AGGR.selec.value) * 4;
	//document.getElementById("chkPort").setAttribute("name","chkPort"+index);
	document.getElementById("chkPort0").name = "chkPort"+(index+0);
	document.getElementById("chkPort1").name = "chkPort"+(index+1);
	document.getElementById("chkPort2").name = "chkPort"+(index+2);
	document.getElementById("chkPort3").name = "chkPort"+(index+3);
	disabledChk();
	showSuitablePortNo();
}

function showSuitablePortNo()
{
	var index = parseInt(AGGR.selec.value) * 4;
	var port1 = document.getElementById("port1");
	var port2 = document.getElementById("port2");
	var port3 = document.getElementById("port3");
	var port4 = document.getElementById("port4");
	port1.firstChild.nodeValue = index+1;
	port2.firstChild.nodeValue = index+2;
	port3.firstChild.nodeValue = index+3;
	port4.firstChild.nodeValue = index+4;
}
</script>
<td align="center">
    <input type="checkbox" name="chkPort0" id="chkPort0" value="on">
    </td>
    <td align="center">
    <input type="checkbox" name="chkPort1"  id="chkPort1" value="on">
    </td>
    <td align="center">
    <input type="checkbox" name="chkPort2"  id="chkPort2" value="on">
    </td>
    <td align="center">
    <input type="checkbox" name="chkPort3"  id="chkPort3" value="on">
</td>
</tr>

</table>
</td></tr>
</TABLE>
<table width="600" cellpadding="4" cellspacing="1" align="center">
<tr height="10"><td></td></tr>
<tr><td align="center">
<input type="button" class="input-button" value="����" onClick="javascript:save()"> 
<input type="button" class="input-button" value="����" onClick="reset()">
<input type="button" id="helpButton" name="helpButton" class="input-button" value="����" onClick="window.open('../rhelpinfo.htm#aggr')">
<input type="button" class="input-button" style="display:none" name="goBack" value="����" onClick="go_back()">
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

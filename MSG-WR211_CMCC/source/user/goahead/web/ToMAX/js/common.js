<% langTran("common"); %>
<% langTran("PdbUpdata"); %>
<% langTran("JS"); %>
var globalCheckTypeFlag = 1;/*Ϊ�˱����������Լ�520w�������жϷ�����Ӵ˱���(���ô˱����������á����ɽ�ʡʱ��)*/
var cookie = {
    set: function(key, value, days) {
        document.cookie = '_' + key + '=' + value + '; expires=' + (new Date(new Date().getTime() + ((days ? days: 14) * 86400000))).toUTCString() + '; path=/';
    },

    get: function(key) {
        var r = ('; ' + document.cookie + ';').match('; _' + key + '=(.*?);');
        return r ? r[1] : null;
    },

    unset: function(key) {
        document.cookie = '_' + key + '=; expires=' + (new Date(1)).toUTCString() + '; path=/';
    }
};
/**
�����ִ��е�����
**/
function parseNumber(num)
{
	num = num || 0;
	try{ 
		num = parseFloat(eval(num));//���ܽ�������������16����
	}
	catch(e){
		num = parseFloat(num);
	}
	return num;
}
/**
��дsplit����
ÿ�����д˺���ǰ����ɾ����β�Ŀո�
**/
var oldSplitFun = String.prototype.split;
String.prototype.split = function() {
    var str = trimstr(this);
    return oldSplitFun.apply(str, arguments);
}
//end split��������д

/**************************************
		��������E
		���ã���ÿؼ���id
		������e�����ؼ�id��
***************************************/
function E(e) {
    return (typeof(e) == 'string') ? document.getElementById(e) : e;
}
/**
Ϊselect�������������(ie8���°汾)������֧��
**/
function lowerIE8ForSel() {
    var obj = new Object();
    obj.sels = null;
    /*��������󼯺�*/
    obj.init = function() {
        if (navigator.appVersion.indexOf("MSIE 5.5") >= 0 || navigator.appVersion.indexOf("MSIE 6.0") >= 0 || navigator.appVersion.indexOf("MSIE 7.0") >= 0)
        /*����ie8�汾*/
        {
            if (!obj.sels)
            /*δ��ֵ��ѡȡ����ҳ���select����*/
            {
                if (document.getElementsByTagName) {
                    obj.sels = document.getElementsByTagName("select");
                } else {
                    return false;
                }
            }
            return true;
        }
        return true;
    }
    /*���select�����option disabled����֧��*/
    obj.addOptDis = function() {
        if (obj.sels.length > 0) {
            obj.pre_selIndex = 0;
            for (var i = 0,
            curSel; i < obj.sels.length; i++) {
                curSel = obj.sels[i];
                curSel.onfocus = function() {
                    /*�����ʱ���ȼ�¼��ǰѡ����*/
                    obj.pre_selIndex = this.selectedIndex;
                }
                curSel.onchange = function() {
                    obj.restore(this);
                }
            }
        }
        obj.restore = function(e) {
            if (e.options[e.selectedIndex].disabled) {
                e.selectedIndex = obj.pre_selIndex;
            } else {
                /*ѡ����Ŀ�仯��ʱ�򣬸��ĵ�ǰѡ������*/
                obj.pre_selIndex = e.selectedIndex;
            }
        }
    }
    return obj;
}
/****
�������ȿ���(�����ÿ���)
**/
function deepClone(source) {
    var ret;
    if (source === null) {
        return null;
    }
    if (source === undefined) {
        return undefined;
    }
    if (typeof(source) != 'object') {
        return source;
    }
    if (source.constructor == Array) {
        ret = [];
        for (var i = 0; i < source.length; i++) {
            ret[i] = arguments.callee(source[i]);
        }
    } else {
        ret = {};
        for (var d in source) {
            ret[d] = arguments.callee(source[d]);
        } //�˴�ֻ����in�﷨����Ϊ�����Ƕ���
    }
    return ret;
}
/**************
�ڵ�ǰ�����в�������data(��Сд����) �ҵ����������� �Ҳ����򷵻�-1
****************/
Array.prototype.searchData = function(data) {
    for (var i = 0; i < this.length; i++) {
        if (this[i] == data) {
            return i;
        }
    }
    return - 1;
}
/**************
�ڵ�ǰ�����в�������data(��Сд������) �ҵ����������� �Ҳ����򷵻�-1
****************/
Array.prototype.searchDataCaseInsensitive = function(data) {
    var reg = data.toLowerCase();
    for (var i = 0; i < this.length; i++) {
        if (this[i].toLowerCase() == reg) {
            return i;
        }
    }
    return - 1;
}
/**************
�ڵ�ǰ�����в�������data ������ ������
****************/
Array.prototype.countData = function(data) {
    var num = 0;
    for (var i = 0; i < this.length; i++) {
        if (this[i] == data) {
            num++;
        }
    }
    return num;
}
/**************
�����������Ԫ�ؽ���һ��eval����
****************/
Array.prototype.evalAll = function() {
    for (var i = 0; i < this.length; i++) {
        this[i] = eval(this[i]);
    }
    return - 1;
}
/**************
�ӵ�ǰ����ĩβ���Ƴ�n��Ԫ��
 n<=0���Ƴ�
****************/
Array.prototype.popN=function(n)
{
    for(var i=0;i<n;i++)
    {
	this.pop();
    }
}
/****
����option����
txt:option��ʾ���ı�,val:option��ֵ
*****/
function newOpt(txt, val) {
    var option = document.createElement("OPTION");
    option.text = txt;
    option.value = val;
    return option;
}
/*************
����obj��value��������
**************/
function copy(obj) {
    var str = obj.value;
    if (obj.value != '') {
        obj.select();
        try { //ie
            window.clipboardData.setData("Text", str);
            alert('<% write(JSAlert0); %>');
        } catch(e) { //��ie ���쳣
            alert('<% write(JSAlert1); %>');
        }

    }
}
/***
�ж�����arr1 �Ƿ� ��ȫ��������arr2 �е�Ԫ��(��arr2 �Ƿ� �� arr1��һ���Ӽ�)
-1ȫ������
0���ְ���
1ȫ������
**/
function isArrayFullInclude(arr1, arr2) {
    var i = 0,
    j = 0,
    n = 0;
    if (arr2 == null || arr1 == null) {
        return - 1;
    }
    if (arr2.length == 0 || arr1.length == 0) {
        return - 1;
    }
    for (i = 0; i < arr2.length; i++) {
        for (j = 0; j < arr1.length; j++) {
            if (arr2[i] == arr1[j]) {
                n++;
                break;
            }
        }
    }
    if (n == 0) { //ȫ������
        return - 1;
    } else if (n != arr2.length) { //���ְ���
        return 0;
    } else { //ȫ������
        return 1;
    }
}
/****************************************
���ݽڵ����Ի�ȡ�ڵ�id
attName���ڵ���������name,id,alt��
attValue:�ڵ����Զ�Ӧ��ֵ
eleRoot :������dom���ĸ� ��������Ϊdocument������dom�ĵ�
tagName: ���������Ľڵ�����  ���������������нڵ�
*****************************************/
document.getElementsByAttribute = function(attName, attValue, eleRoot, tagName) {
    this.d = eleRoot || document;
    this.tName = tagName || '*';
    var children = this.d.getElementsByTagName(this.tName) || document.all;
    var elements = new Array();
    for (var i = 0; i < children.length; i++) {
        //Ϊ�˼��ݻ�������������eval("children[i]."+attName)
        //��ΪĳЩ�������getAttribute��ͨ��.����ʱ�õ�attName�ǲ�һ������class��className������
        if ((children[i].getAttribute(attName) || eval("children[i]." + attName)) == attValue) {
            elements.push(children[i]);
        }
    }
    return elements;
}
/***********
����������
objΪ��������,tipsΪ��ʾ��Ϣ
alertFlag:0 �����������alert��ʾ��button��submit�����¼�������ʾ��������������
          1 ��������ʾ��
		  2 ������ʾ��
************/
function inputErro(obj, tips, alertFlag, showPic) {
	alertFlag = alertFlag || 0;
	if(!globalCheckTypeFlag)
	{/*�ǽ�����ҳ��*/
		if (alertFlag != 1) alert(tips);
		return;
	}
	if( typeof(obj) != 'object' )
	{
		if (alertFlag == 2) alert(tips);
		return;
	}
    var isAlert = 0;
    var ty = '';
    if (!alertFlag) //�������������ʾ��Ϣ
    {
        try {
            ty = event.srcElement.type.toLowerCase(); //�����ú������¼��İ�ť����
            if (ty == 'button' || ty == 'submit') isAlert = 1; //ֻ�б����ʱ��Ų���alert��ʾ��

        } catch(e) {
            isAlert = 1; //��ie���������������ʾ��
        }
    } else if (alertFlag == 1) //������alert��ʾ��Ϣ
    {
        isAlert = 0;
    } else if (alertFlag == 2) //����alert��ʾ��Ϣ
    {
        isAlert = 1;
    }
    obj.style.backgroundRepeat = "no-repeat";
    obj.style.backgroundPosition = "right";
    if (showPic != 1)
    {
	obj.style.backgroundImage = "url(<% write(com_imgerr); %>)";
    }
    obj.setAttribute("title", tips);
    //obj.style.borderColor="red";
    if (isAlert) alert(tips);
    if (obj.type == "text") obj.focus();
}
/*****
�����������ȷ������
objΪ��������
***/
function inputRight(obj) {
	if( typeof(obj) != 'object' ) return;
    obj.style.backgroundImage = "";
    //obj.style.borderColor="";
    obj.setAttribute("title", '');
}

/**
��form�е������ı���������ʾ��Ϣ��Ϊ��ȷ
**/
function allInputRight(formObj) {
    var inputTxt = document.getElementsByAttribute("type", "text", formObj, "input");
    for (var i = 0; i < inputTxt.length; i++) {
        inputRight(inputTxt[i]);
    }

}
// JavaScript Document
//������display���Ը�ֵ������ΪIE��FF
var dis = "";
if (document.all) dis = "block";
else dis = "table-row";

//3G��������
var g3_para_arr = new Array(new Array("CMC", "*99***1#", "CMNET", "CMNET", "CMNET"),
/**/
new Array("CUC", "*99#", "", "", "UNINET"),
/**/
new Array("CDC", "#777", "ctnet@mycdma.cn", "vnet.mobi", "")); //��Ӫ�̣����ţ��߼��û��������룬�������
///////////////////////////////////////
//���ܣ�ע���û�
//����˵��:
//����ֵ����
//////////////////////////////////////

function logOut() {
    /*if(event.clientX > document.body.clientWidth && event.clientY < 0 || event.altKey)
	{
		location.href("/goform/Logout");
		alert("<% write(JSAlert2); %>");
	}*/
}

function GoBack_Page(val) {
    parent.main.location = val;
}
/////////////////����ֵ///////////////////////
var RateArr = new Array("64K", "128K", "256K", "512K", "1M", "1.5M", "2M", "3M", "4M", "5M", "6M", "7M", "8M", "9M", "10M", "11M", "12M", "13M", "14M", "15M", "16M", "17M", "18M", "19M", "20M", "25M", "30M", "35M", "40M", "45M", "50M", "90M", "100M");
var RateArrValue = new Array("64", "128", "256", "512", "1000", "1500", "2000", "3000", "4000", "5000", "6000", "7000", "8000", "9000", "10000", "11000", "12000", "13000", "14000", "15000", "16000", "17000", "18000", "19000", "20000", "25000", "30000", "35000", "40000", "45000", "50000", "90000", "100000");

var RateArr1 = new Array("512K", "1M", "1.5M", "2M", "3M", "4M", "5M", "6M", "7M", "8M", "9M", "10M", "11M", "12M", "13M", "14M", "15M", "16M", "17M", "18M", "19M", "20M", "25M", "30M", "35M", "40M", "45M", "50M", "90M", "100M");
var RateArrValue1 = new Array("512", "1000", "1500", "2000", "3000", "4000", "5000", "6000", "7000", "8000", "9000", "10000", "11000", "12000", "13000", "14000", "15000", "16000", "17000", "18000", "19000", "20000", "25000", "30000", "35000", "40000", "45000", "50000", "90000", "100000");

///////////////////////////////////////
//���ܣ�����Сʱ����ӵ������б�ֵ
//����ֵ����
//////////////////////////////////////
function CreateHOp(len) {
    for (i = 0; i < len; i++) if (i < 10) document.write('<option value=0' + i + '>0' + i + '</option>');
    else document.write('<option value=' + i + '>' + i + '</option>');
}
//////////////////////////////////////

function CreateEftTime() {
    document.write('<tr><td align="right" height="25" ><% write(com_date); %></td><td >&nbsp;</td><td ><input type="checkbox" name="everyday" onClick="seldate(this.form,this.checked)" checked="checked" value="everyday"><% write(com_every); %></td></tr>');
    document.write('<tr><td align="right" height="25" >&nbsp;</td><td width="2%">&nbsp;</td><td><input type="checkbox" name="date" value="1"><% write(com_monday); %>&nbsp;<input type="checkbox" name="date" value="2"><% write(com_tuesday); %>&nbsp;<input type="checkbox" name="date" value="3"><% write(com_wednesday); %>&nbsp;<input type="checkbox" name="date" value="4"><% write(com_thursday); %>&nbsp;<input type="checkbox" name="date" value="5"><% write(com_friday); %>&nbsp;<input type="checkbox" name="date" value="6"><% write(com_saturday); %>&nbsp;<input type="checkbox" name="date" value="7"><% write(com_sunday); %></td></tr>');
    document.write('<tr><td align="right" height="25"><% write(com_time); %></td><td width="2%">&nbsp;</td><td ><input type="radio" name="effecttime" value="All" onClick="seltime(this.form,this.value)" checked>&nbsp;<% write(com_allDay); %></td></tr>');
    document.write('<tr><td align="right" height="25"  >&nbsp;</td><td width="2%">&nbsp;</td><td><input type="radio" name="effecttime" value="timerange" onClick="seltime(this.form,this.value)">&nbsp;<% write(com_from); %>&nbsp;<select name="fromhour" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="fromminute" class="list"><script language="javascript">CreateHOp(60);</script></select>&nbsp;&nbsp;<% write(com_to); %>&nbsp;<select name="endhour" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="endminute" class="list"><script language="javascript">CreateHOp(60);</script></select></td></tr><input type="hidden" name="day"><input type="hidden" name="timestart"><input type="hidden" name="timestop">');
}

var effectDay = "1111111"; //��Чʱ�����ѡ��ÿ��ʱ��ȡ��ֵ��
var effectTime = "All"; //��Чʱ�����ѡ��ȫ��ʱ��ȡ��ֵ
var DateArray = new Array("<% write(com_monday); %>", "<% write(com_tuesday); %>", "<% write(com_wednesday); %>", "<% write(com_thursday); %>", "<% write(com_friday); %>", "<% write(com_saturday); %>", "<% write(com_sunday); %>");
function getTimeValue(f) { //�����Чʱ����е����ں�ʱ��ֵ���������ؿؼ�
    var len = f.date.length;
    if (f.everyday.checked == false) {
        for (i = 0; i < len; i++) {
            f.day.value += (f.date[i].checked == true ? "1": "0");
        }
    } else {
        f.day.value = effectDay;
    }
    if (f.effecttime[0].checked == false) {
        if (f.fromhour.value == "00" && f.fromminute.value == "00" && f.endhour.value == "00" && f.endminute.value == "00") {
            f.timestart.value = effectTime;
            f.timestop.value = "0";
        } else {
            f.timestart.value = f.fromhour.value + ":" + f.fromminute.value;
            f.timestop.value = f.endhour.value + ":" + f.endminute.value;
        }

    } else {
        f.timestart.value = effectTime;
        f.timestop.value = "0";
    }

}
function setTimeValue(f, days, timestarts, timestops) { //ҳ���ʼ��ʱ����Чʱ��θ�ֵ����
    if (days == effectDay) {
        f.everyday.checked = true;
    } else {
        f.everyday.checked = false;
        for (i = 0; i < days.length; i++) {
            if (days.charAt(i) == "1") f.date[i].checked = true;
        }
    }
    if (timestarts == effectTime) {
        f.effecttime[0].checked = true;
    } else {
        f.effecttime[1].checked = true;
        var index1 = timestarts.indexOf(":");
        var index2 = timestops.indexOf(":");
        f.fromhour.value = timestarts.substring(0, index1);
        f.fromminute.value = timestarts.substring(index1 + 1);
        f.endhour.value = timestops.substring(0, index2);
        f.endminute.value = timestops.substring(index2 + 1);
    }
    seldate(f, f.everyday.checked);
    seltime(f, timestarts);
}
function seldate(f, flag) { //��Чʱ�����ѡַ����ʱ��Һ���
    var len = f.date.length;
    for (i = 0; i < len; i++) {
        f.date[i].disabled = (flag == true ? true: false);
    }
}
function seltime(f, val) { //��Чʱ�����ѡ��ʱ��ʱ��Һ���
    f.fromhour.disabled = f.fromminute.disabled = f.endhour.disabled = f.endminute.disabled = (val == effectTime ? true: false);
}

///////////////////////////////////////
//���ܣ��豸wep����ʱ������Կ����
//����˵��:
//����ֵ����
//////////////////////////////////////

function getdisable(keyLength1, keyLength2, keyLength3, keyLength4) {
    key1dis = (keyLength1.selectedIndex == 0);
    key2dis = (keyLength2.selectedIndex == 0);
    key3dis = (keyLength3.selectedIndex == 0);
    key4dis = (keyLength4.selectedIndex == 0);
}
function auto_disable(key1, key2, key3, key4, keynum, keyLength1, keyLength2, keyLength3, keyLength4) {
    var keychoosed;
    key1.disabled = keynum[0].disabled = key1dis;
    key2.disabled = keynum[1].disabled = key2dis;
    key3.disabled = keynum[2].disabled = key3dis;
    key4.disabled = keynum[3].disabled = key4dis;
    for (i = 0; i < keynum.length; i++) {
        if (keynum[i].checked) {
            keychoosed = keynum[i].value;
            break;
        }
    }
    if ((0 == key1dis) || (0 == key2dis) || (0 == key3dis) || (0 == key4dis)) {
        if ((keychoosed == 1 && keyLength1.selectedIndex == 0) || (keychoosed == 2 && keyLength2.selectedIndex == 0) || (keychoosed == 3 && keyLength3.selectedIndex == 0) || (keychoosed == 4 && keyLength4.selectedIndex == 0)) {
            if (0 == key1dis) keynum[0].checked = 1;
            else if (0 == key2dis) keynum[1].checked = 1;
            else if (0 == key3dis) keynum[2].checked = 1;
            else keynum[3].checked = 1;
        }
    }
}

function auto_length(key1, key2, key3, key4, Length1, Length2, Length3, Length4, Index) {

    keylength1 = (1 == key1dis) ? keylength1: (((3 == Length1.selectedIndex) ? 16 : ((2 == Length1.selectedIndex) ? 13 : 5)) * ((0 == Index) ? 2 : 1));
    keylength2 = (1 == key2dis) ? keylength2: (((3 == Length2.selectedIndex) ? 16 : ((2 == Length2.selectedIndex) ? 13 : 5)) * ((0 == Index) ? 2 : 1));
    keylength3 = (1 == key3dis) ? keylength3: (((3 == Length3.selectedIndex) ? 16 : ((2 == Length3.selectedIndex) ? 13 : 5)) * ((0 == Index) ? 2 : 1));
    keylength4 = (1 == key4dis) ? keylength4: (((3 == Length4.selectedIndex) ? 16 : ((2 == Length4.selectedIndex) ? 13 : 5)) * ((0 == Index) ? 2 : 1));

    if ((key1.value.length > keylength1) && (key1dis != 1)) key1.value = key1.value.substring(0, keylength1);
    if ((key2.value.length > keylength2) && (key2dis != 1)) key2.value = key2.value.substring(0, keylength2);
    if ((key3.value.length > keylength3) && (key3dis != 1)) key3.value = key3.value.substring(0, keylength3);
    if ((key4.value.length > keylength4) && (key4dis != 1)) key4.value = key4.value.substring(0, keylength4);
    key1.maxLength = keylength1;
    key2.maxLength = keylength2;
    key3.maxLength = keylength3;
    key4.maxLength = keylength4;
}
function checkWEP(key1, key2, key3, key4, Index) {
    var str = "<% write(JSAlert3); %>";
    if (key1dis && key2dis && key3dis && key4dis) {
        alert("<% write(JSAlert4); %>");
        return false;
    }
    //alert("keylength1="+keylength1+"key1.value.length="+key1.value.length);
    if (!key1dis) {
        if (key1.value.length != keylength1) {
            alert(str + "<% write(JSAlert5); %>" + keylength1 + "<% write(JSAlert6); %>");
            return false;
        } else {
            if (Index == 1) {
                if (!checkEnglishName(key1, keylength1, "<% write(JSAlert7); %>", 0)) return false;
            } else {
                if (!checkHex(key1, "<% write(JSAlert7); %>", 0)) return false;
            }
        }
    }
    if (!key2dis) {
        if (key2.value.length != keylength2) {
            alert(str + "<% write(JSAlert8); %>" + keylength2 + "<% write(JSAlert6); %>");
            return false;
        } else {
            if (Index == 1) {
                if (!checkEnglishName(key2, keylength2, "<% write(JSAlert9); %>", 0)) return false;
            } else {
                if (!checkHex(key2, "<% write(JSAlert9); %>", 0)) return false;
            }
        }
    }
    if (!key3dis) {
        if (key3.value.length != keylength3) {
            alert(str + "<% write(JSAlert10); %>" + keylength3 + "<% write(JSAlert6); %>");
            return false;
        } else {
            if (Index == 1) {
                if (!checkEnglishName(key3, keylength3, "<% write(JSAlert11); %>", 0)) return false;
            } else {
                if (!checkHex(key3, "<% write(JSAlert11); %>", 0)) return false;
            }
        }
    }
    if (!key4dis) {
        if (key4.value.length != keylength4) {
            alert(str + "<% write(JSAlert12); %>" + keylength4 + "<% write(JSAlert6); %>");
            return false;
        } else {
            if (Index == 1) {
                if (!checkEnglishName(key4, keylength4, "<% write(JSAlert13); %>", 0)) return false;
            } else {
                if (!checkHex(key4, "<% write(JSAlert13); %>", 0)) return false;
            }
        }
    }
    return true;
}
///////////////////////////////////////
//���ܣ���ʾ�����ر��
//����˵��:
//tableName--�������
//showFlag--1��ʾ��0����
//����ֵ����
//////////////////////////////////////
function showTable(tableName, showFlag) {
    if (eval(showFlag)) tableName.style.display = dis;
    else tableName.style.display = "none";
}

function makeArray(n) {
    this.length = n;
    for (var i = 0; i < length; i++) {
        this[i] = 0;
    }
    return this;
}
//���ܣ����治�ɹ��󷵻�ҳ����ʾ������Ϣ
function showerror(errorstr, tbwidth) {
    if (errorstr != "") {
        if (tbwidth != "") document.write('<tr><td height="20"><table width=' + tbwidth + '% border="0" cellspacing="0" cellpadding="0" align="center"><tr><td height="20">&nbsp;</td></tr><tr><td align="center" height="50" valign="middle" class="style7" width="630"><img src="<% write(com_imgerr); %>" height="20" width="20">&nbsp;&nbsp;<font color="red">' + errorstr + '</font></td></tr></table></td></tr>');
        else document.write('<tr><td height="20"><table width="630" border="0" cellspacing="0" cellpadding="0" align="center"><tr><td height="20">&nbsp;</td></tr><tr><td align="center" height="50" valign="middle" class="style7" width="630"><img src="<% write(com_imgerr); %>" height="20" width="20">&nbsp;&nbsp;<font color="red">' + errorstr + '</font></td></tr></table></td></tr>');
    }

}
function showright(errorstr, tbwidth) {
    if (errorstr != "") {
        if (tbwidth != "") document.write('<tr><td height="20"><table width=' + tbwidth + '% border="0" cellspacing="0" cellpadding="0" align="center"><tr><td height="20">&nbsp;</td></tr><tr><td align="center" height="50" valign="middle" class="style7" width="630"><font color="red">' + errorstr + '</font></td></tr></table></td></tr>');
        else document.write('<tr><td height="20"><table width="630" border="0" cellspacing="0" cellpadding="0" align="center"><tr><td height="20">&nbsp;</td></tr><tr><td align="center" height="50" valign="middle" class="style7" width="630"><font color="red">' + errorstr + '</font></td></tr></table></td></tr>');
    }

}

//���ܣ���ʾ�������Ϣ
function showfiledtip(tips,tdname){
        if(globalCheckTypeFlag) allInputRight();
	globalCheckTypeFlag = 0;/*���ø÷������򲻲��ý������Ĵ�����ʾ����Է��ص�*/
	tdname.innerHTML="";//alert(tips);
	if(tips!="") tdname.innerHTML+="<font style='font-size:9px;color=#333333'><img src='<% write(com_imgerr); %>'>"+tips+"</font>";
	else  tdname.innerHTML="";
}
function openThreeMenu(k) {
    if (parent.main) parent.main.location = arrayThreeMenu[k][1];
    else window.location = arrayThreeMenu[k][1];
}
function showThreeMenu() { //��������˵�����
	 var tabWidth = 120;
    if (window.screen.width <= 1024) arrayThreeMenu[0] = 600;
    document.write('<table width="' + arrayThreeMenu[0] + '" border="0" cellspacing="0" cellpadding="0" align="center"><tr ><td  height="26"width="30"  class="three_bottom">&nbsp;</td>');
    for (i = 2; i < arrayThreeMenu[1] + 2; i++) {
        if (arrayThreeMenu[i][2]) {
		if(arrayThreeMenu[i][0].length>=17)tabWidth=155;
		if(arrayThreeMenu[i][0].length>=30)tabWidth=180; 
            if (arrayThreeMenu[i][3]) document.write('<td width="3" align="right" valign="bottom"><img src="../<% write(com_imgthreel); %>" height="27"></td><td width="'+tabWidth+'" class="three_top" align="center"><b>' + arrayThreeMenu[i][0] + '</b></td><td width="3" valign="bottom"><img src="../<% write(com_imgthreer); %>" height="27"></td><td width="6" class="three_bottom">&nbsp; </td>');
            else document.write('<td width="3" align="right" class="three_bottom"><img src="../<% write(com_imgthreel); %>"></td><td width="'+tabWidth+'" align="center"  class="three_both"><a onclick="openThreeMenu(' + i + ')" target="main"  style="cursor :pointer; color:#6bb23a">' + arrayThreeMenu[i][0] + '</a></td><td width="3"  class="three_bottom"><img src="../<% write(com_imgthreer); %>"></td><td width="6" class="three_bottom">&nbsp; </td>');
        }
    }
    document.write('');
    document.write('<td class="three_bottom">&nbsp;</td></tr></table>');
}

/**
��������˵�����(���ύ)�е�
�л�div����
k:Ϊ��div����Ӧ���±꣨��2��ʼ��
**/
var cur_index;
function switchDiv(k) {
    var curTdIndex = 0;
    var i = 0,
    j = 0;
    for (i = 2; i < arrayThreeMenu[1] + 2; i++) {
        document.getElementById(arrayThreeMenu[i][1]).style.display = "none";
    }
    document.getElementById(arrayThreeMenu[k][1]).style.display = "block";
    j = cur_index
    for (i = 2; i < cur_index; i++) {
        if (!arrayThreeMenu[i][2]) {
            j--;
        } //����ʾ��ѡ�
    }
    var td = document.getElementById('ThreeMenuTb').rows[0].cells; //��һ�е����е�Ԫ��

    //�ı䵱ǰ����˵��ĸ�ʽΪδ����
    curTdIndex = eval(1 + (j - 2) * 4); //�Ӹ�table�ĵ�һ�еĵڶ�����Ԫ��ʼ��ÿ���˵�ռ��4����Ԫ�� 
    td[curTdIndex].className = 'three_bottom';
    td[curTdIndex].innerHTML = '<img src="../<% write(com_imgthreel); %>">';
    td[curTdIndex + 1].className = 'three_both';
    td[curTdIndex + 1].innerHTML = '<a onclick="switchDiv(' + cur_index + ')"  style="cursor :hand; color:408080">' + arrayThreeMenu[cur_index][0] + '</a>';
    td[curTdIndex + 2].className = 'three_bottom';
    td[curTdIndex + 2].innerHTML = '<img src="../<% write(com_imgthreer); %>">';
    //�������Ĳ˵�
    cur_index = k;
    j = cur_index
    for (i = 2; i < cur_index; i++) {
        if (!arrayThreeMenu[i][2]) {
            j--;
        } //����ʾ��ѡ�
    }
    curTdIndex = eval(1 + (j - 2) * 4);
    td[curTdIndex].className = '';
    td[curTdIndex].innerHTML = '<img src="../<% write(com_imgthreel); %>" height="27">';
    td[curTdIndex + 1].className = 'three_top';
    td[curTdIndex + 1].innerHTML = '<b>' + arrayThreeMenu[cur_index][0] + '</b>';
    td[curTdIndex + 2].className = '';
    td[curTdIndex + 2].innerHTML = '<img src="../<% write(com_imgthreer); %>" height="27">';
    document.body.scrollTop = 0; //������λ����0
}
/**
��������˵����� 
������Ϊ���ύ���� ��ͨ���л� ͬһ��ҳ���еĲ�ͬdiv�ﵽ��ʾ��Ч��
��showThreeMenu()��������һ��ȫ�ֱ���arrayThreeMenu
**/
function showThreeMenuNotCommit(menuWidth) {
    var mWidth = menuWidth || 120;
    if (window.screen.width <= 1024) arrayThreeMenu[0] = 600;
    document.write('<table id="ThreeMenuTb" width="' + arrayThreeMenu[0] + '" border="0" cellspacing="0" cellpadding="0" align="center"><tr ><td  height="26" width="30"  class="three_bottom">&nbsp;</td>');
    for (i = 2; i < arrayThreeMenu[1] + 2; i++) {
        if (arrayThreeMenu[i][2]) {
            if (arrayThreeMenu[i][3]) //��ǰ
            {
                document.write('<td width="3" align="right" valign="bottom"><img src="../<% write(com_imgthreel); %>" height="27"></td><td width="' + mWidth + '" class="three_top" align="center"><b>' + arrayThreeMenu[i][0] + '</b></td><td width="3" valign="bottom"><img src="../<% write(com_imgthreer); %>" height="27"></td><td width="6" class="three_bottom">&nbsp; </td>');
                cur_index = i; //��ǰ��ʾ��ҳ��
            } else document.write('<td width="3" align="right" class="three_bottom"><img src="../<% write(com_imgthreel); %>"></td><td width="' + mWidth + '" align="center"  class="three_both"><a onclick="switchDiv(' + i + ')"  style="cursor :hand; color:408080">' + arrayThreeMenu[i][0] + '</a></td><td width="3"  class="three_bottom"><img src="../<% write(com_imgthreer); %>"></td><td width="6" class="three_bottom">&nbsp; </td>');
        }
    }
    document.write('');
    document.write('<td class="three_bottom">&nbsp;</td></tr></table>');

}
function showTable1() {
    document.write('<table width="100%" border="0" cellspacing="0" cellpadding="0">');
}
function showTable2() {
    document.write('<table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">');
}
function showTable3() {
    document.write('<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">');
}
function showSecondTitle1(str) { //���main �еĶ����˵����͹��ͼƬ
    document.write('<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center"><tr><td width="15" height="28"><img src="../<% write(com_imgthreel); %>" height="28" width="15"></td><td bgcolor="#008893" class="title2" >' + str + '</td><td align="right" bgcolor="#008893"><img src="../<% write(com_imgguanggao); %>" width="85" height="20"></td><td width="15" height="28"><img src="../<% write(com_imglabelr); %>" height="28" width="15"></td></tr></table>');
}
function showSecondTitle(str) { //����main �еĶ����˵����Լ���ҳ���title
    //Ϊonload������ӱ������ƺ������Ա�֤�����ڸ߶ȹ����ʱ�򲻻���ֱ����ϲ�����
    //��Ϊ�˹����ں�����ӣ���û��ÿ��ҳ����Ӵ˹��ܡ���ѡ����һ�����õĺ���������ӡ��������������Ч�ʣ�ȴ�����˴��뽡׳����ɶ���
    var oldOnloadFun = window.onload;
    window.onload = function() {
        if (oldOnloadFun) oldOnloadFun.apply(this, arguments);
        var tbg = null;
        if (! (tbg = document.getElementsByAttribute("className", "table_bg")[0])) {
            return;
        }
        var wh = document.body.clientHeight - 22; //22Ϊ��������ĸ߶�
        if (tbg.getAttribute("height") < wh) tbg.setAttribute("height", wh);
    }
    //end
    var obj = null;
    try {
        obj = parent.secondTop.document.getElementById("secondTopTitle");
    } catch(e) {
        //�ϲ�secondTopTitle������
        return;
    }
    obj.innerHTML = str;
    var dev_name = trimstr(parent.assist.document.getElementById('dev_name').value) || "admin";
    var login_name = trimstr(parent.assist.document.getElementById('login_name').value) || "admin";
    parent.document.title = dev_name + '-' + login_name + '  -  ' + str;
    //	parent.secondTop.document.write('<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center" ><tr><td width="15" height="28"><img src="../images/label_l.gif" height="28" width="15"></td><td bgcolor="#008893" class="title2" >'+str+'</td><td align="right" bgcolor="#008893"><img src="../images/guanggao.png" width="85" height="20"></td><td width="15" height="28"><img src="../images/label_r.gif" height="28" width="15"></td></tr></table>');

}
/******************change macaddr to the linux mode mac************/
function WebMacTOLinuxMac(val) {
    var str = "";
    var len = val.length;
    if (val != "" && len <= 12) {
        for (i = 0; i < len; i += 2) {
            if (i != 10) str += val.substr(i, 2) + ":";
            else str += val.substr(i, 2);
        }
    } else if (len > 12) return val;
    return str;
}

/****************** new add for field check************************/
//���ܣ�����Ƿ�Ϸ� �������벢������ʾ��Ϣ (����ѡ�����Ϊ�գ�
function re_checkMask(cntrl, tips) {
    var retips = "";
    ip_str = trimstr(cntrl.value);
    if (ip_str != "") {
        if (!validateMask(ip_str)) {
            retips = tips + "<% write(JSAlert14); %>";
        }
    } else {
        retips = tips + "<% write(JSAlert15); %>";
    }
    return retips;
}
/*���ipv6��ַ�Ƿ��зǷ��ַ�����Թȸ������)*/
function checkIpv6Sp(cntrl, tips) {
    var retips = "";
    var ip = typeof(cntrl) == 'object' ? cntrl.value: cntrl;
    var onenum;
    ip = trimstr(ip);
    cntrl.value = ip;
    var length = ip.length;
    for (var i = 0; i < length; i++) {
        onenum = ip.substring(i, i + 1);
        if ((onenum < "0" || onenum > "9") && (onenum < "a" || onenum > "f") && (onenum < "A" || onenum > "F") && (onenum != ":")) {
            retips = tips + "<%write(JSAlert66);%>";
            break;
        }
    }
    if (retips != '') {
        alert(retips);
        return false;
    } else {
        return true;
    }
}
/****************** new add for field check************************/
/*���IPv6��ַ�Ϸ���*/
function re_checkIpv6(cntrl,tips,allow) {     
    var allowfe = allow;
    var retips = "";
    var matchStr = "((([0-9a-f]{1,4}:){7}([0-9a-f]{1,4}|:))|(([0-9a-f]{1,4}:){6}(:[0-9a-f]{1,4}|((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9a-f]{1,4}:){5}(((:[0-9a-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9a-f]{1,4}:){4}(((:[0-9a-f]{1,4}){1,3})|((:[0-9a-f]{1,4})?:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9a-f]{1,4}:){3}(((:[0-9a-f]{1,4}){1,4})|((:[0-9a-f]{1,4}){0,2}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9a-f]{1,4}:){2}(((:[0-9a-f]{1,4}){1,5})|((:[0-9a-f]{1,4}){0,3}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9a-f]{1,4}:){1}(((:[0-9a-f]{1,4}){1,6})|((:[0-9a-f]{1,4}){0,4}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(:(((:[0-9a-f]{1,4}){1,7})|((:[0-9a-f]{1,4}){0,5}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:)))(%.+)?\s*$";         
    var ipv6 = cntrl.value;
    var ip = convert2CompleteIpV6(cntrl.value);
    var flag = 0;  
    var ret = ipv6.match(matchStr);
    var ip_item = new Array();          
    var j=0; 
    if ((cntrl.value.substring(0,2)=="ff")||(cntrl.value.substring(0,2)=="Ff")||(cntrl.value.substring(0,2)=="fF")||(cntrl.value.substring(0,2)=="FF")){
        retips = tips + "<%write(JSAlert65);%>";
    }        
    if(allowfe == 0){
        if((cntrl.value.substring(0,4)=="fe80")||(cntrl.value.substring(0,4)=="Fe80")||(cntrl.value.substring(0,4)=="fE80")||(cntrl.value.substring(0,4)=="FE80")){
            retips = tips + "<%write(JSAlert67);%>";
        }        
    }        
    if(ip == "0:0:0:0:0:0:0:1"){
        retips = tips + "<%write(JSAlert68);%>";
    }        

    if (retips == ""){
        ip_item = cntrl.value.split(":");
        if (!ret) {             
            retips = tips + "<%write(JSAlert14);%>";
        }        
    } 
  if (retips == ""){
        ip_item = cntrl.value.split(":");
        for (var i=0; i<ip_item.length; i++) {
            if( ip_item[i]=="" ){
                j=j+1;
            }
            if(ip_item[i].length > 4) {
                flag = 1;
                break;
            }
        }
        if((j > 1)||(ip_item.length > 8)||(flag == 1)){
            retips = tips + "<%write(JSAlert14);%>";
        }
    }
    return retips;
}
       
//���ܣ�����Ƿ�Ϸ�ip��ַ���������벢������ʾ��Ϣ (����ѡ�����Ϊ�գ�
function re_checkIP(cntrl, tips) {
    var retips = "";
    ip_str = trimstr(cntrl.value);
    if (ip_str != "") {
        if (!verifyDottedIP(ip_str)) {
            retips = tips + "<% write(JSAlert14); %>";
        }
    } else {
        retips = tips + "<% write(JSAlert15); %>";
    }
    return retips;
}
//���ܣ������DNS,��re_checkIP�Ļ����ϲ���Ϊ0.0.0.0
function re_checkMainDns(cntrl, tips) {
    var retips = "";
    ip_str = trimstr(cntrl.value);
    if (ip_str != "") {
        if (!verifyDottedIP(ip_str) || (ip_str == '0.0.0.0')) {
            retips = tips + "<% write(JSAlert14); %>";
        }
    } else {
        retips = tips + "<% write(JSAlert15); %>";
    }
    return retips;
}

/****************** new add for field check************************/
//���ܣ�����Ƿ�Ϸ�ip��ַ���������벢������ʾ��Ϣ (���Ǳ���ѡ��ܹ�Ϊ�գ�
function re_checkIP_empty(cntrl, tips) {
    var retips = "";
    ip_str = trimstr(cntrl.value);
    if (ip_str != "") {
        if (!verifyDottedIP(ip_str)) {
            retips = tips + "<% write(JSAlert14); %>";
        }
    }
    return retips;
}

//���ܣ�����Ƿ�Ϸ�MAC��ַ��������ʾ��Ϣ
function re_checkMac(cntrl, tips) {
    var retips = "";
    var num = cntrl.value.toUpperCase();
    num = trimstr(num);
    if (num != "") {
        var length = num.length;
        for (var i = 0; i < length; i++) {
            onenum = num.substring(i, i + 1);
            twonum = num.substring(i + 1, i + 2);
            if ((onenum < "0" || onenum > "9") && (onenum < "a" || onenum > "f") && (onenum < "A" || onenum > "F")) {
                if (onenum == "-" || onenum == ":") {
                    if (twonum == "-" || twonum == ":") {
                        length = 0;
                        break;
                    } else {
                        num = num.substring(0, i) + num.substring(i + 1, length);
                        length--;
                    }
                } else {
                    length = 0;
                    break;
                }
            }
        }
        if (length != 12) retips = tips + "<% write(JSAlert16); %>";
        if (num == "000000000000" || num == "FFFFFFFFFFFF") retips = tips + "<% write(JSAlert16); %>";
    }
    return retips;
}

//���ܣ�����Ƿ�Ϸ��û�����������ʾ��Ϣ
function re_checkName(cntrl, tips) {
    var retips = "";
    var name = cntrl.value;
    if (name != "" && name != null) {
        if (name == 0) {
            retips = tips + "<% write(JSAlert17); %>";
            return retips;
        }
    }

    if (name.indexOf(";") != -1 || name.indexOf(",") != -1 || name.indexOf("%") != -1 || name.indexOf("\"") != -1 || name.indexOf("&") != -1 || name.indexOf(" ") != -1 || name.indexOf("\'") != -1 || name.indexOf("\\") != -1 || name.indexOf("<") != -1 || name.indexOf(">") != -1) {
        retips = tips + "<% write(JSAlert18); %>";
    }
    return retips;
}
//���ܣ�����Ƿ�Ϸ�SSID��������ʾ��Ϣ
function re_checkSSIDName(cntrl, tips) {
    var retips = "";
    var name = cntrl.value;
    if (name != "" && name != null) {
        if (name == 0) {
            retips = tips + "<% write(JSAlert17); %>";
            return retips;
        }
    }

    if (name.indexOf(";") != -1 || name.indexOf(",") != -1 || name.indexOf("%") != -1 || name.indexOf("\"") != -1 || name.indexOf("&") != -1 || name.indexOf("\'") != -1 || name.indexOf("\\") != -1 || name.indexOf("<") != -1 || name.indexOf(">") != -1) {
        retips = tips + "<% write(JSAlert581); %>";
    }
    return retips;
}

function re_checkRemark(cntrl, tips) {
    var retips = "";
    var name = cntrl.value;
    

    if (name.indexOf(";") != -1 || name.indexOf("��") != -1 || name.indexOf(",") != -1 || name.indexOf("%") != -1 || name.indexOf("\"") != -1 || name.indexOf("&") != -1 || name.indexOf(" ") != -1 || name.indexOf("\'") != -1 || name.indexOf("\\") != -1) {
        retips = tips + "<% write(JSAlert19); %>";
    }
    return retips;
}

//���ܣ�����ֻ������Ӣ����ĸ�����ֺ��»���
function re_checkSpecialName(cntrl, tips) {
    var name = cntrl.value;
    var retips = "";
    allowChar = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_";
    for (i = 0; i < name.length; i++) {
        if (allowChar.indexOf(name.substring(i, i + 1)) == -1) retips = tips + "<% write(JSAlert20); %>";
    }
    return retips;
}
//���ܣ�����Ƿ�Ϸ����ֲ�������ʾ��Ϣ
function re_checkNumber(cntrl, minNum, maxNum, tips) {
    var retips = "";
    var num = trimstr(cntrl.value);
    if (cntrl.value == "") retips = tips + "<% write(JSAlert21); %>";
    else if (!isInt(num)) retips = tips + "<% write(JSAlert22); %>";
    else if (eval(num) < minNum || eval(num) > maxNum) retips = tips + "<% write(JSAlert23); %>" + minNum + "<% write(JSAlert24); %>" + maxNum + "<% write(JSAlert25); %>";
    return retips;
}
//���ܣ�����Ƿ�Ϸ����ֲ�������ʾ��Ϣ,������
function re_checkNumber_allowZero(cntrl, minNum, maxNum, tips) {
    var retips = "";
    var num = trimstr(cntrl.value);
    if (cntrl.value == "") retips = tips + "<% write(JSAlert21); %>";
    else if (!isInt(num)) retips = tips + "<% write(JSAlert22); %>";
    else if ((eval(num) < minNum || eval(num) > maxNum) && (eval(num) != 0)) retips = tips + "<% write(JSAlert23); %>" + minNum + "<% write(JSAlert24); %>" + maxNum + "<% write(JSAlert25); %>";
    return retips;
}

//���ܣ�����Ƿ�Ϸ�Ӣ���û�����������ʾ��Ϣ
function re_checkEnglishName(cntrl, tips) {
    var retips = "";
    var name = cntrl.value;
    allowChar = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.<>/?[]{}-_+=|`~!@$^&*()";
    for (i = 0; i < name.length; i++) {
        if (allowChar.indexOf(name.substring(i, i + 1)) == -1) retips = tips +"<% write(JSAlert26); %>";
    }
    return retips;;
}
function re_checkPassword1(cntrl1, maxlen, allownull) {
    var retips = "";
    pas1 = cntrl1.value;
    maxlen = toNumber(maxlen, 0, maxlen.length);
    if (pas1.length > maxlen)
	{
		retips = "<% write(JSAlert27); %>" + maxlen + "<% write(JSAlert29); %>";
    }
	else if (pas1 == "")
	{
		if (!eval(allownull))
		{
			retips = "<% write(JSAlert28); %>";
		}
		else
		{
			retips = "";
		}
    }
    if (pas1.indexOf(",") != -1 || pas1.indexOf(";") != -1 || pas1.indexOf("��") != -1 || pas1.indexOf("%") != -1 || pas1.indexOf("\"") != -1 || pas1.indexOf("\'") != -1 || pas1.indexOf("\\") != -1 || ��pas1.indexOf(" ") != -1 || pas1.indexOf("&") != -1 || pas1.indexOf("#") != -1)
	{
        retips = "<%write(com_passwd);%> <% write(JSAlert582); %>";
    }
    return retips;
}
function re_checkPassword(cntrl1, cntrl2, maxlen) {
    var retips = "";
    pas1 = cntrl1.value;
    pas2 = cntrl2.value;
    len = toNumber(maxlen, 0, maxlen.length);
	if (pas1 != pas2) {
        retips = "<% write(JSAlert30); %>";
        cntrl1.focus();
    }
    if (pas1.indexOf(",") != -1 || pas1.indexOf(";") != -1 || pas1.indexOf("��") != -1 || pas1.indexOf("%") != -1 || pas1.indexOf("\"") != -1 || pas1.indexOf("\'") != -1 || pas1.indexOf("\\") != -1 || ��pas1.indexOf(" ") != -1 || pas1.indexOf("&") != -1) {
        retips = "<%write(com_passwd);%> <% write(JSAlert19); %>";
    }
    return retips;
}
/******************end new add for field check************************/

///////////////////////////////////////
//���ܣ��������ַ�Ƿ���ͬһ����
//����˵��:
//ip1��ip2--ip��ַ
//mask--��ַ����
//����ֵ��true��ͬһ���Σ�false����ͬһ����
//////////////////////////////////////
function checkSameSubnet(ip1, ip2, mask) {
    var pointer = 0;
    var i;
    num = new makeArray(12);
    num[0] = 0;
    for (i = 0; i < ip1.length; i++) {
        if (ip1.substring(i, i + 1) == ".") {
            pointer++;
            num[0 + pointer] = 0;
            continue;
        }
        num[0 + pointer] *= 10;
        num[0 + pointer] += eval(ip1.substring(i, i + 1));
    }
    pointer = 0;
    num[4 + 0] = 0;
    for (i = 0; i < mask.length; i++) {
        if (mask.substring(i, i + 1) == ".") {
            pointer++;
            num[4 + pointer] = 0;
            continue;
        }
        num[4 + pointer] *= 10;
        num[4 + pointer] += eval(mask.substring(i, i + 1));
    }
    pointer = 0;
    num[8 + 0] = 0;
    for (i = 0; i < ip2.length; i++) {
        if (ip2.substring(i, i + 1) == ".") {
            pointer++;
            num[8 + pointer] = 0;
            continue;
        }
        num[8 + pointer] *= 10;
        num[8 + pointer] += eval(ip2.substring(i, i + 1));
    }
    if (((num[0 + 0] & num[4 + 0]) == (num[4 + 0] & num[8 + 0])) && ((num[0 + 1] & num[4 + 1]) == (num[4 + 1] & num[8 + 1])) && ((num[0 + 2] & num[4 + 2]) == (num[4 + 2] & num[8 + 2])) && ((num[0 + 3] & num[4 + 3]) == (num[4 + 3] & num[8 + 3]))) return true;
    return false;
}
///////////////////////////////////////
//���ܣ�����Ƿ�Ϸ�ip��ַ
//����˵��:
//cntrl--�ؼ���
//tips--��ʾ��Ϣ
//allownull--1����Ϊ�գ�0������Ϊ��
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkIP(cntrl, tips, allownull) {
    var retips = "";
    var ip_str = trimstr(cntrl.value);
    cntrl.value = ip_str;
    if (ip_str == '') {
        if (!eval(allownull)) retips = tips + "<% write(JSAlert14); %>";
        else retips = '';
    } else if (!verifyDottedIP(ip_str)) retips = tips + "<% write(JSAlert14); %>";
    if (retips != '') {
        inputErro(cntrl, retips);
        return false;
    } else {
        inputRight(cntrl);
        return true;
    }
}
/*���IPv6��ַ�Ϸ���*/
function checkIpv6(cntrl,tips,allow) {
    var allowfe = allow;
    var retips = "";
    var matchStr = "((([0-9a-f]{1,4}:){7}([0-9a-f]{1,4}|:))|(([0-9a-f]{1,4}:){6}(:[0-9a-f]{1,4}|((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9a-f]{1,4}:){5}(((:[0-9a-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9a-f]{1,4}:){4}(((:[0-9a-f]{1,4}){1,3})|((:[0-9a-f]{1,4})?:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9a-f]{1,4}:){3}(((:[0-9a-f]{1,4}){1,4})|((:[0-9a-f]{1,4}){0,2}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9a-f]{1,4}:){2}(((:[0-9a-f]{1,4}){1,5})|((:[0-9a-f]{1,4}){0,3}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9a-f]{1,4}:){1}(((:[0-9a-f]{1,4}){1,6})|((:[0-9a-f]{1,4}){0,4}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(:(((:[0-9a-f]{1,4}){1,7})|((:[0-9a-f]{1,4}){0,5}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:)))(%.+)?\s*$";
    var ip = convert2CompleteIpV6(cntrl.value);
    var ipv6 = cntrl.value;
    var flag = 0;
    var ret = ipv6.match(matchStr);
    var ip_item = new Array();
    var j=0;
    if ((cntrl.value.substring(0,2)=="ff")||(cntrl.value.substring(0,2)=="Ff")||(cntrl.value.substring(0,2)=="fF")||(cntrl.value.substring(0,2)=="FF")){
        retips = tips + "<%write(JSAlert65);%>";
    }
    if(allowfe == 0){
        if((cntrl.value.substring(0,4)=="fe80")||(cntrl.value.substring(0,4)=="Fe80")||(cntrl.value.substring(0,4)=="fE80")||(cntrl.value.substring(0,4)=="FE80")){
            retips = tips + "<%write(JSAlert67);%>";
        }
    }
    if(ip == "0:0:0:0:0:0:0:1"){
        retips = tips + "<%write(JSAlert68);%>";
    }

    if (retips == ""){
        ip_item = cntrl.value.split(":");
        if (!ret) {
            retips = tips + "<%write(JSAlert14);%>";
        }
    }
  if (retips == ""){
        for (var i=0; i<ip_item.length; i++) {
              if( ip_item[i]=="" ){
                  j=j+1;
              }
              if(ip_item[i].length > 4) {
                  flag = 1;
                  break;
              }

        }
        if((j > 1)||(ip_item.length > 8)||(flag == 1)){
            retips = tips + "<%write(JSAlert14);%>";
        }
    }
    if (retips != '') {
        inputErro(cntrl, retips);
        return false;
    } else {
        inputRight(cntrl);
        return true;
    }

}
/*�Ƚ�����IP��С����ǰ��С�ں��ߣ��򷵻�1 */ 
function compareIPv6(ipBegin, ipEnd)   
{  
    var temp1;   
    var temp2;     
    temp1 = ipBegin.value.split(":"); 
    temp2 = ipEnd.value.split(":");     
    for (var i = 0; i < temp1.length; i++)   
    {    
         var tmp1=parseInt(temp1[i],16);     
         var tmp2=parseInt(temp2[i],16);   
         if (tmp1>tmp2)   
         {    
            return -1;   
         }    
        else if (tmp1<tmp2)   
        {    
            return 1;   
        }    
    }    
    return 0;     
}  
/*�������IPv6��ַ�Ƿ���ͬһ����*/
function checkIpv6Subnet2(cntrl1,cntrl2,length)
{   
    var ipv6mask = ipv6mask_int_to_string(length)
    var ip1 = convert2CompleteIpV6(cntrl1);
    var ip2 = convert2CompleteIpV6(cntrl2);
    var gw1 = ipv6_cmp(ip1, ipv6mask);
    var gw2 = ipv6_cmp(ip2, ipv6mask);
    if( gw1 == gw2){
	return 1;
    }else{
	return 0;
    }
}

/*�������IPv6��ַ�Ƿ���ͬһ����*/
function checkIpv6Subnet(cntrl1,cntrl2,length)
{
    var ipv6mask = ipv6mask_int_to_string(length.value)
    var ip1 = convert2CompleteIpV6(cntrl1.value);
    var ip2 = convert2CompleteIpV6(cntrl2.value);
    var gw1 = ipv6_cmp(ip1, ipv6mask);
    var gw2 = ipv6_cmp(ip2, ipv6mask);
    if( gw1 == gw2){
        return 1;
    }else{
        return 0;
    }
}

/*��ѹ����ʽ��ת��Ϊ�����IPv6��ַ*/
function convert2CompleteIpV6(ip)
{
    var ipV6=ip;
    var index=ip.indexOf("::");
    if(index>0){
        var size=8-(ip.split(":").length-1);
        var tmp="";
        for(var i=0;i<size;i++){
            tmp+=":0";
        }
        tmp+=":";
        ipV6=ip.replace("::",tmp);
    }else if(index==0){
        ipV6=ip.replace("::","0:0:0:0:0:0:0:");
    }
    return ipV6;
}
/*��ipv6���루���ipv4˵�ģ����磺16  ת��Ϊ x:x:x:x:x:x:x:x ����ʽ*/
function ipv6mask_int_to_string(ipv6mask)
{
    var first = 0;
    var second = 0;
    var third = 0;
    var forth = 0;
    var fifth = 0;
    var sixth = 0;
    var seventh = 0;
    var eighth = 0;
    var i = 0;
    var mi = 0;
    if(ipv6mask<0 || ipv6mask>128)
        return "The mask valued " + ipv6mask + " is invalid!"
            if(ipv6mask==0)
                return "0:0:0:0:0:0:0:0";
            else if(ipv6mask>=1&&ipv6mask<=16)
            {
                mi = 15;
                for(i=0;i<ipv6mask;i++)
                {
                    first += Math.pow(2,mi);
                    mi--;
                }
                second = 0;
                third = 0;
                forth = 0;
                fifth = 0;
                sixth = 0;
                seventh = 0;
                eighth = 0;
            }
 else if(ipv6mask>=17&&ipv6mask<=32)
            {
                first = "ffff";
                mi = 15;
                for(i=0;i<ipv6mask-16;i++)
                {
                    second += Math.pow(2,mi);
                    mi--;
                }
                third = 0;
                forth = 0;
                fifth = 0;
                sixth = 0;
                seventh = 0;
                eighth = 0;
            }
            else if(ipv6mask>=33&&ipv6mask<=48)
            {
                first = "ffff";
                second = "ffff";
                mi = 15;
                for(i=0;i<ipv6mask-32;i++)
                {
                    third += Math.pow(2,mi);
                    mi--;
                }
                forth = 0;
                fifth = 0;
                sixth = 0;
                seventh = 0;
                eighth = 0;
            }
 else if(ipv6mask>=49&&ipv6mask<=64)
            {
                first = "ffff";
                second = "ffff";
                third = "ffff";
                mi = 15;
                for(i=0;i<ipv6mask-48;i++)
                {
                    forth += Math.pow(2,mi);
                    mi--;
                }
                fifth = 0;
                sixth = 0;
                seventh = 0;
                eighth = 0;
            }
            else if(ipv6mask>=65&&ipv6mask<=80)
            {
                first = "ffff";
                second = "ffff";
                third = "ffff";
                forth = "ffff"
                    mi = 15;
                for(i=0;i<ipv6mask-64;i++)
                {
                    fifth += Math.pow(2,mi);
                    mi--;
                }
                sixth = 0;
                seventh = 0;
                eighth = 0;
 }else if(ipv6mask>=81&&ipv6mask<=96)
            {
                first = "ffff";
                second = "ffff";
                third = "ffff";
                forth = "ffff";
                fifth = "ffff";
                mi = 15;
                for(i=0;i<ipv6mask-80;i++)
                {
                    sixth += Math.pow(2,mi);
                    mi--;
                }
                seventh = 0;
                eighth = 0;
            }
            else if(ipv6mask>=97&&ipv6mask<=112)
            {
                first = "ffff";
                second = "ffff";
                third = "ffff";
                forth = "ffff";
                fifth = "ffff";
                sixth = "ffff";
                mi = 15;
                for(i=0;i<ipv6mask-96;i++)
                {
                    seventh += Math.pow(2,mi);
                    mi--;
                }
                eighth = 0;
 }else if(ipv6mask>=113&&ipv6mask<=128)
            {
                first = "ffff";
                second = "ffff";
                third = "ffff";
                forth = "ffff";
                fifth = "ffff";
                sixth = "ffff";
                seventh = "ffff";
                mi = 15;
                for(i=0;i<ipv6mask-112;i++)
                {
                    eighth += Math.pow(2,mi);
                    mi--;
                }
            }
            return "" + first.toString(16) + ":" + second.toString(16) + ":" + third.toString(16) + ":" + forth.toString(16) + ":" + fifth.toString(16) + ":" + sixth.toString(16) + ":" + seventh.toString(16) + ":" + eighth.toString(16) + "";
}
//��������ʽת��Ϊ�ַ�������ʽ
function initaddr(ipaddrs)
{
    var iparray ="";
    for(var i=0;i<ipaddrs.length;i++)
    {
	iparray+=ipaddrs[i];
    }
    if(iparray.charAt(iparray.length-1)==':')
    {
	iparray = iparray.substr(0,iparray.length-1);
    }
    return iparray;
}
//ipv6�����루���ipv4�����룩��������
function ipv6_cmp(ipv6, mask)
{
    var ip_split;
    var mask_split;
    var flag = 0;
    var i = 0;
    var dest_ip = new Array();
    var ip = new Array();
    ip_split = ipv6.split(":");
    mask_split = mask.split(":");
    for ( i = 0; i < 8; i++ )
    {
	dest_ip[i] = parseInt(ip_split[i], 16) & parseInt(mask_split[i], 16);
    }
    for ( i = 0; i < 8; i++ )
    {
	if (dest_ip[i] == "0")
	{
	    for ( var j = i + 1; j < 8; j++)
	    {
		if ( dest_ip[j] != "0")
		{
		    flag = 1;
		}
	    }
	    if (flag == 0)
	    {
		for ( var k = 0; k < i; k++ )
		{
		    ip[k] = dest_ip[k].toString(16) + ":";
		}

		ip[k] = "::";
		if (ip[0] == "::")
		{
		    return ip;
		}
		return initaddr(ip);
	    }
	    else
	    {
		flag =0;
	    }
	}
    }
    for ( i = 0; i < 8; i++ )
    {
	if ( i == 7 )
	{
	    ip[i] = dest_ip[i].toString(16);
	}
	ip[i] = dest_ip[i].toString(16) + ":";
    }
    return initaddr(ip);
}
//ȥ���ַ�����ǰ��ո�
String.prototype.trim=function() {
   return this.replace(/(^\s*)|(\s*$)/g,'');
}
/*
 * �����DNS
 */
function checkMainDns(cntrl, tips, allownull) {
    var retips = "";
    var ip_str = trimstr(cntrl.value);
    cntrl.value = ip_str;
    if (ip_str == '') {
        if (!eval(allownull)) retips = tips + "<% write(JSAlert31); %>";
        else retips = '';
    } else if (!verifyDottedIP(ip_str) || (ip_str == '0.0.0.0')) {
		retips = tips + "<% write(JSAlert14); %>";
	}
    if (retips != '') {
        inputErro(cntrl, retips);
        return false;
    } else {
        inputRight(cntrl);
        return true;
    }
}

///////////////////////////////////////
//���ܣ����Lan���Ƿ�Ϸ�ip��ַ��3110vf�����ӵĺ�����
//����˵��:
//cntrl--�ؼ���
//tips--��ʾ��Ϣ
//allownull--1����Ϊ�գ�0������Ϊ��
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkLanIP(cntrl, tips, allownull) {
    ip_str = trimstr(cntrl.value);
    if (ip_str == "0.0.0.0") {
        alert(tips + "<% write(JSAlert32); %>");
        return false;
    }
    if (ip_str == "") {
        if (!eval(allownull)) {
            alert(tips + "<% write(JSAlert33); %>");
            cntrl.focus();
            return false;
        } else return true;
    } else if (!verifyDottedIP2(ip_str)) {
        alert(tips + "<% write(JSAlert34); %>");
        cntrl.focus();
        return false;
    } else return true;
}
///////////////////////////////////////
//���ܣ�����Ƿ�Ϸ����������루3110vf�����ӣ�
//����˵��:
//cntrl--�ؼ���
//tips--��ʾ��Ϣ
//allownull--1����Ϊ�գ�0������Ϊ��
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkMask(cntrl, tips, allownull) {
    var ip_str = trimstr(cntrl.value);
    var retips = '';
    if (ip_str == "") {
        if (!eval(allownull)) {
            retips = tips + "<% write(JSAlert33); %>";
        }
    } else if (!validateMask(ip_str)) {
        retips = tips + "<% write(JSAlert34); %>";
    }
    if (retips != '') {
        inputErro(cntrl, retips);
        return false;
    } else {
        inputRight(cntrl);
        return true;
    }
}
/**
* �������ܣ����ش��������Ӧ��8λ������ֵ
* ���������ip:���ʮ���Ƶ�ֵ(0~255),int���͵�ֵ��
* ����������validateMask
* ���ú�������
* ����ֵ:   ip��Ӧ�Ķ�����ֵ(�磺����255������11111111;����1,����00000001)
**/
function _checkIput_fomartIP(ip) {
    return (ip + 256).toString(2).substring(1); //��ʽ�����(����)
}

/**
* �������ܣ���֤��������ĺϷ���
* �������ߣ�236F(fuwei236#gmail.com)
* ���������MaskStr:���ʮ���Ƶ���������(�磺255.255.255.192) 
* ���ú�����_checkIput_fomartIP(ip) 
* ����ֵ��  true:  MaskStrΪ�Ϸ���������
            false: MaskStrΪ�Ƿ���������
**/
function validateMask(MaskStr) {
    /* ��Ч��У�� */
    var IPPattern = /^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$/
    if (!IPPattern.test(MaskStr)) return false;

    /* �����ֵ */
    var IPArray = MaskStr.split(".");
    var ip1 = parseInt(IPArray[0]);
    var ip2 = parseInt(IPArray[1]);
    var ip3 = parseInt(IPArray[2]);
    var ip4 = parseInt(IPArray[3]);
    if (ip1 < 0 || ip1 > 255
    /* ÿ����ֵ��Χ0-255 */
    || ip2 < 0 || ip2 > 255 || ip3 < 0 || ip3 > 255 || ip4 < 0 || ip4 > 255) {
        return false;
    }
    /* ��������ֵ�Ƿ�Ϸ� */
    //ƴ�Ӷ������ַ���
    var ip_binary = _checkIput_fomartIP(ip1) + _checkIput_fomartIP(ip2) + _checkIput_fomartIP(ip3) + _checkIput_fomartIP(ip4);

    if ( - 1 != ip_binary.indexOf("01")) return false;
    return true;
}

///////////////////////////////////////
//���ܣ�����Ƿ�Ϸ�MAC��ַ
//����˵��:
//cntrl--�ؼ���
//tips--��ʾ��Ϣ
//allownull--1����Ϊ�գ�0������Ϊ��
//noChange--1�ı� ,0���ı�(�Ƿ�ı�cntrl��ֵ)
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkMac(cntrl, tips, allownull, noChange) {
	var retips = '';
    var num = cntrl.value.toUpperCase();
    num = trimstr(num);
    if (num == "") {
        if (!eval(allownull)) {
            retips = tips + "<% write(JSAlert33); %>";
        } 
    } else {
        var length = num.length;
        for (var i = 0; i < length; i++) {
            onenum = num.substring(i, i + 1);
            twonum = num.substring(i + 1, i + 2);
            if ((onenum < "0" || onenum > "9") && (onenum < "a" || onenum > "f") && (onenum < "A" || onenum > "F")) {
                if (onenum == "-" || onenum == ":") {
                    if (twonum == "-" || twonum == ":") {
                        length = 0;
                        break;
                    } else {
                        num = num.substring(0, i) + num.substring(i + 1, length);
                        length--;
                    }
                } else {
                    length = 0;
                    break;
                }
            }
        }
        if (length != 12) {
            retips = tips + "<% write(JSAlert35); %>";
        } else {
            if (num == "000000000000" || num == "FFFFFFFFFFFF") {
                retips = tips + "<% write(JSAlert35); %>";
            }
        }
    }
	if (retips != '') {
        inputErro(cntrl, retips);
        return false;
    } else {
        inputRight(cntrl);
        if (noChange != 1) cntrl.value = num;
        return true;
    }
}

function checktempMac(cntrl, tips, allownull) {
    var num = trimstr(cntrl);
    if (num == "") {
        if (!eval(allownull)) {
            return false;
        } else {
            //cntrl.value="000000000000";
            return true;
        }

    } else {
        var length = num.length;
        for (var i = 0; i < length; i++) {
            onenum = num.substring(i, i + 1);
            twonum = num.substring(i + 1, i + 2);
            if ((onenum < "0" || onenum > "9") && (onenum < "a" || onenum > "f") && (onenum < "A" || onenum > "F")) {
                if (onenum == "-" || onenum == ":") {
                    if (twonum == "-" || twonum == ":") {
                        length = 0;
                        break;
                    } else {
                        num = num.substring(0, i) + num.substring(i + 1, length);
                        length--;
                    }
                } else {
                    length = 0;
                    break;
                }
            }
        }
        if (length != 12) {
            return false;
        } else {
            if (num == "000000000000" || num == "FFFFFFFFFFFF") {

                return false;
            }
            cntrl.value = num;
            return true;
        }
    }
}

///////////////////////////////////////
//���ܣ�����Ƿ�Ϸ�����
//����˵��:
//cntrl--�ؼ���
//maxlen--�������󳤶�
//tips--��ʾ��Ϣ
//allownull--1����Ϊ�գ�0������Ϊ��
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkDomainName(cntrl, maxlen, tips, allownull) {
    var name_old = trimstr(cntrl.value);
	var name = name_old;
    if (name == "") {
        if (!eval(allownull)) {
            alert(tips + "<% write(JSAlert33); %>");
            cntrl.focus();
            return false;
        } else return true;
    }
    index = name.indexOf(".");
    while (index != -1) {
        name = name.substring(0, index) + name.substring(index + 1, name.length);
        maxlen--;
        index = name.indexOf(".");

    }
    cntrl.value = name;
    if (!checkName(cntrl, maxlen, tips, allownull)) {
        cntrl.value = name_old;
        return false;
    } else {
        cntrl.value = name_old;
        return true;
    }

}
///////////////////////////////////////
//���ܣ�����Ƿ�Ϸ��û���
//����˵��:
//cntrl--�ؼ���
//maxlen--�������󳤶�
//tips--��ʾ��Ϣ
//allownull--1����Ϊ�գ�0������Ϊ��
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkName(cntrl, maxlen, tips, allownull) {
    var retips = "";
	var name = typeof(cntrl) == 'object' ? cntrl.value: cntrl;
    name = trimstr(name);
    cntrl.value = name;
    if (name == "") {
        if (!eval(allownull)) retips = tips + "<% write(JSAlert31); %>";
        else retips = '';
    } else if (name == 0) {
        retips = tips + "<% write(JSAlert36); %>";
    } else if (name.lstrLen() > maxlen) {
        retips = tips + "<% write(JSAlert37); %>" + maxlen + "<% write(JSAlert29); %>";
    } else if ((tips != "SSID") && (tips != "������������1") && (tips != "������������2") && (name.indexOf(";") != -1 || name.indexOf(",") != -1 || name.indexOf("%") != -1 || name.indexOf("\"") != -1 || name.indexOf("&") != -1 || name.indexOf(" ") != -1 || name.indexOf("\'") != -1 || name.indexOf("\\") != -1 || name.indexOf("<") != -1 || name.indexOf(">") != -1)) {
        retips = tips + "<% write(JSAlert58); %>";
    }else if(((tips == "SSID")||(tips == "������������1")||(tips == "������������2")) && (name.indexOf(";") != -1 || name.indexOf(",") != -1 || name.indexOf("%") != -1 ||name.indexOf("\"") != -1 || name.indexOf("&") != -1 || name.indexOf("\'") != -1 || name.indexOf("\\") != -1 || name.indexOf("<") != -1 || name.indexOf(">") != -1))
    {
	retips = tips + "<% write(JSAlert581); %>";	
    }
    if (retips != '') {
        typeof(cntrl) == 'object'?inputErro(cntrl, retips):inputErro(cntrl, retips,2);
        return false;
    } else {
        inputRight(cntrl);
        return true;
    }
}

////////////////////////////////////////////
//���ܣ�����ʼ����������Ƿ�Ϸ�
//����˵��:
//cntrl--�ؼ���
//maxlen--�������󳤶�
//tips--��ʾ��Ϣ
//allownull--1����Ϊ�գ�0������Ϊ��(����Ч)
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkSmtpServer(cntrl,maxlen,tips,allownull)
{
    var retips = "";
	var name = typeof(cntrl) == 'object' ? cntrl.value: cntrl;
    name = trimstr(name);
    cntrl.value = name;

    if (name == "") {
        if (!eval(allownull)) retips = tips + "<% write(JSAlert31); %>";
        else retips = '';

    } else if (name.lstrLen() > maxlen) {
        retips = tips + "<% write(JSAlert37); %>" + maxlen + "<% write(JSAlert29); %>";
    } else if(!/^([\w-]+\.)+((com)|(net)|(org)|(gov\.cn)|(info)|(cc)|(com\.cn)|(net\.cn)|(org\.cn)|(name)|(biz)|(tv)|(cn)|(mobi)|(name)|(sh)|(ac)|(io)|(tw)|(com\.tw)|(hk)|(com\.hk)|(ws)|(travel)|(us)|(tm)|(la)|(me\.uk)|(org\.uk)|(ltd\.uk)|(plc\.uk)|(in)|(eu)|(it)|(jp))$/.test(name)) {
	retips = tips + "<% write(JSAlert60); %>" + "<% write(JSAlert35); %>";
    }
    if (retips != "") {
        typeof(cntrl) == 'object'?inputErro(cntrl, retips):inputErro(cntrl, retips,2);
        return false;
    }else{
        inputRight(cntrl);
        return true;
	}
}
////////////////////////////////////////////
//���ܣ�����������Ƿ�Ϸ�
//����˵��:
//cntrl--�ؼ���
//maxlen--�������󳤶�
//tips--��ʾ��Ϣ
//allownull--1����Ϊ�գ�0������Ϊ��(����Ч)
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkEmail(cntrl,maxlen,tips,allownull)
{
    //��֤�����������ʽ
    var reg = /^\w+((-\w+)|(\.\w+))*\@[A-Za-z0-9]+((\.|-)[A-Za-z0-9]+)*\.[A-Za-z]+$/;
    var retips = "";
	var name = typeof(cntrl) == 'object' ? cntrl.value: cntrl;
    name = trimstr(name);
    cntrl.value = name;

    if (name == "") {
        if (!eval(allownull)) retips = tips + "<% write(JSAlert31); %>";
        else retips = '';
    } else if (name.lstrLen() > maxlen) {
        retips = tips + "<% write(JSAlert37); %>" + maxlen + "<% write(JSAlert29); %>";
    } else if(!reg.test(name)){
		retips = tips + "<% write(JSAlert60); %>" + "<% write(JSAlert35); %>";
    }
    if (retips != "") {
        typeof(cntrl) == 'object'?inputErro(cntrl, retips):inputErro(cntrl, retips,2);
        return false;
    }else{
        inputRight(cntrl);
        return true;
	}
}

////////////////////////////////////////////
//���ܣ���鱸ע�Ƿ�Ϸ�
//����˵��:
//cntrl--�ؼ���
//maxlen--�������󳤶�
//tips--��ʾ��Ϣ
//allownull--1����Ϊ�գ�0������Ϊ��(����Ч)
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkRemark(cntrl,maxlen,tips,allownull)
{
    var retips = "";
    var name = typeof(cntrl) == 'object' ? cntrl.value: cntrl;
    name = trimstr(name);
    cntrl.value = name;
    if (name.lstrLen() > maxlen) {
	retips = tips + "<% write(JSAlert37); %>" + maxlen + "<% write(JSAlert29); %>";
    } else if (name.indexOf("%")!=-1 || name.indexOf("\"")!=-1 || name.indexOf(":")!=-1  || name.indexOf(" ")!=-1 || name.indexOf("\'")!=-1 || name.indexOf("\\")!=-1 || name.indexOf("&")!=-1 || name.indexOf("\n")!=-1 || name.indexOf("<") != -1 || name.indexOf(">") != -1) {
	retips = tips + "<% write(JSAlert57); %>";
    }

    if (retips != '') {
	typeof(cntrl) == 'object'?inputErro(cntrl, retips):inputErro(cntrl, retips,2);
	return false;
    } else {
	inputRight(cntrl);
	return true;
    }
}

///////////////////////////////////////
//���ܣ�����Ƿ�Ϸ��ļ���(ƥ��windows�Լ�linux���ļ���)
//����˵��:
//cntrl--�ؼ���
//maxlen--�������󳤶�
//tips--��ʾ��Ϣ
//allownull--1����Ϊ�գ�0������Ϊ��
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkFileName(cntrl, maxlen, tips, allownull) {
    var retips = "";
    var regStr = /^[0-9a-zA-Z_\.\-\u4e00-\u9fa5]+$/;
    var name = trimstr(cntrl.value);
    cntrl.value = name;
    if (name == "") {
        if (!eval(allownull)) retips = tips + "<% write(JSAlert31); %>";
        else retips = '';
    } else if (name == 0) {
        retips = tips + "<% write(JSAlert36); %>";
    } else if (name.charAt(0) == ".") {
        retips = tips + "<% write(JSAlert38); %>";
    } else if (name.lstrLen() > maxlen) {
        retips = tips + "<% write(JSAlert37); %>" + maxlen + "<% write(JSAlert29); %>";
    } else if (!regStr.test(name)) {
        retips = tips + '<% write(JSAlert19); %>';
    }
    if (retips != '') {
        inputErro(cntrl, retips);
        return false;
    } else {
        inputRight(cntrl);
        return true;
    }
}
///////////////////////////////////////
//���ܣ�����Ƿ�Ϸ���¼�û���
//����˵��:
//cntrl--�ؼ���
//maxlen--�������󳤶�
//tips--��ʾ��Ϣ
//allownull--1����Ϊ�գ�0������Ϊ��
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkSpecialName(cntrl, maxlen, tips, allownull) {
    var name = cntrl.value;
    if (name == "") {
        if (!eval(allownull)) {
            alert(tips + "<% write(JSAlert33); %>");
            cntrl.focus();
            return false;
        } else return true;
    } else if (name == 0) {
        alert(tips + "<% write(JSAlert36); %>");
        return false;
    } else if (name.length > maxlen) {
        alert(name.length);
        alert(tips + "<% write(JSAlert37); %>" + maxlen + "<% write(JSAlert29); %>");
        cntrl.focus();
        return false;
    } else {
        allowChar = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_";
        for (i = 0; i < name.length; i++) {
            if (allowChar.indexOf(name.substring(i, i + 1)) == -1) {
                alert(tips + "<% write(JSAlert39); %>");
                return false;
            }
        }
    }
    return true;
}
/*���ܣ�����ֻ���������ֺͶκ���*/
function re_checkSpecialNumber(cntrl, tips) {
    var name = cntrl.value;
    var retips = "";
    allowNumber = "1234567890-";
    for (i = 0; i < name.length; i++) {
        if (allowNumber.indexOf(name.substring(i, i + 1)) == -1) 
	{
	    alert(tips + "<% write(JSAlert39); %>");
            return false;
	}
    }
    return true;
}
///////////////////////////////////////
//���ܣ�����Ƿ�Ϸ��û���(3100vf�����ӣ����������뺺��)
//����˵��:
//cntrl--�ؼ���
//maxlen--�������󳤶�
//tips--��ʾ��Ϣ
//allownull--1����Ϊ�գ�0������Ϊ��
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkEnglishName(cntrl, maxlen, tips, allownull) {
    var name = cntrl.value;
    if (name == "") {
        if (!eval(allownull)) {
            alert(tips + "<% write(JSAlert17); %>");
            cntrl.focus();
            return false;
        } else return true;
    } else if (name == 0) {
        alert(tips + "<% write(JSAlert17); %>");
        return false;
    } else if (name.length > maxlen) {
        alert(tips + "<% write(JSAlert37); %>" + maxlen + "<% write(JSAlert29); %>");
        cntrl.focus();
        return false;
    } else {
        allowChar = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.<>/?[]{}-_+=|`~!@$^&*()";
        for (i = 0; i < name.length; i++) {
            if (allowChar.indexOf(name.substring(i, i + 1)) == -1) {
                alert(tips + "<% write(JSAlert39); %>");
                return false;
            }
        }
    }
    return true;
}
/*********************************************************
* ���ܣ�����Ƿ�Ϸ��û���(3100vf�����ӣ����������뺺��)
* ����˵��:
* cntrl--�ؼ���
* maxlen--�������󳤶�
* tips--��ʾ��Ϣ
* allownull--1����Ϊ�գ�0������Ϊ��
* ����ֵ��true�Ϸ���false�Ƿ�
*********************************************************/
function checkEnglishName_allowZero(cntrl, maxlen, tips, allownull) {
    var name = cntrl.value;
    if (name == "") {
        if (!eval(allownull)) {
            alert(tips + "<% write(JSAlert31); %>");
            cntrl.focus();
            return false;
        } else return true;
    } 
    else if (name.length > maxlen) {
        alert(tips + "<% write(JSAlert37); %>" + maxlen + "<% write(JSAlert29); %>");
        cntrl.focus();
        return false;
    } else {
        allowChar = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890./?[]{}-_+=|`~!@$^&*()";
        for (i = 0; i < name.length; i++) {
            if (allowChar.indexOf(name.substring(i, i + 1)) == -1) {
                alert(tips + "<% write(JSAlert39); %>");
                return false;
            }
        }
    }
    return true;
}
///////////////////////////////////////
//���ܣ�����Ƿ�Ϸ�����
//����˵��:
//cntrl--�ؼ���
//minNum--�������Сֵ
//maxNum--��������ֵ
//tips--��ʾ��Ϣ
//allownull--1����Ϊ�գ�0������Ϊ��
//onlycheck--1ֻ���жϣ�0�ڳ����ʱ�������ʾ��Ϣ
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkNumber(cntrl, minNum, maxNum, tips, allownull, onlycheck) {
    var retips = "";
    var num = trimstr(cntrl.value);
    if (num == '') {
        if (!eval(allownull)) retips = tips + "<% write(JSAlert31); %>";
        else retips = '';
    } else if (!isInt(num)) retips = tips + "<% write(JSAlert39); %>";
    else if (eval(num) < minNum || eval(num) > maxNum) retips = tips + "<% write(JSAlert23); %>" + minNum + "<% write(JSAlert24); %>" + maxNum + "<% write(JSAlert25); %>";
    if (retips != '') {
        if (!eval(onlycheck)) inputErro(cntrl, retips);
        return false;
    } else {
        if (!eval(onlycheck)) inputRight(cntrl);
        return true;
    }
}
///////////////////////////////////////
//���ܣ�����Ƿ�Ϸ�����
//����˵��:
//cntrl1��cntrl2--�ؼ���
//maxlen--�������󳤶�
//allownull--1����Ϊ�գ�0������Ϊ��
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkPassword(cntrl1, cntrl2, maxlen, allownull, alertFlag) {
    if (!checkPassword1(cntrl1, maxlen, allownull, alertFlag)) {
        return false;
    } else if (cntrl1.value != cntrl2.value) {
        inputErro(cntrl2, "<% write(JSAlert30); %>", alertFlag);
        return false;
    }
    inputRight(cntrl2);
    return true;
}

function checkPassword1(cntrl1, maxlen, allownull, alertFlag) {
    var retips = "";
    pas1 = cntrl1.value;
    maxlen = toNumber(maxlen, 0, maxlen.length);
    if (pas1.length > maxlen) {
        retips = "<% write(JSAlert27); %>" + maxlen + "<% write(JSAlert29); %>";
    } else if (pas1 == "") {
        if (!eval(allownull)) {
            retips = "<% write(JSAlert28); %>";
        } else retips = "";
    } else if (pas1.indexOf(",") != -1 || pas1.indexOf(";") != -1 || pas1.indexOf("��") != -1 || pas1.indexOf("%") != -1 || pas1.indexOf("\"") != -1 || pas1.indexOf("\'") != -1 || pas1.indexOf("\\") != -1 || ��pas1.indexOf(" ") != -1 || pas1.indexOf("&") != -1 || pas1.indexOf("#") != -1) {
        retips = "<%write(com_passwd);%> <% write(JSAlert582); %>";
    }
    if (retips != '') {
        inputErro(cntrl1, retips, alertFlag);
        return false;
    } else {
        inputRight(cntrl1);
        return true;
    }
}
/* Add tips */
function checkPassword2(cntrl1, maxlen, tips, allownull, alertFlag) {
    var retips = "";
    pas1 = cntrl1.value;
    maxlen = toNumber(maxlen, 0, maxlen.length);
    if (pas1.length > maxlen) {
        retips = tips + "<%write(JSAlert37);%>" + maxlen + "<%write(JSAlert29);%>";
    } else if (pas1 == "") {
        if (!eval(allownull)) {
            retips = tips + "<%write(JSAlert31);%>";
        } else retips = "";
    } else if (pas1.indexOf(",") != -1 || pas1.indexOf(";") != -1 || pas1.indexOf("��") != -1 || pas1.indexOf("%") != -1 || pas1.indexOf("\"") != -1 || pas1.indexOf("\'") != -1 || pas1.indexOf("\\") != -1 || ��pas1.indexOf(" ") != -1 || pas1.indexOf("&") != -1) {
        retips = tips + "<%write(JSAlert19);%>";
    }
    if (retips != '') {
        inputErro(cntrl1, retips, alertFlag);
        return false;
    } else {
        inputRight(cntrl1);
        return true;
    }
}
///////////////////////////////////////
//���ܣ��Ƚ���ip��ַ�Ĵ�С
//����˵��:
//ip_str1��ip_str2--ip��ַ
//����ֵ��0��ȣ�-1С�ڣ�1����
//////////////////////////////////////
function compareIP(ip_str1, ip_str2) {
    var index = 0;
    var n = 0;
    var ip1 = new Array(4);
    while (index < ip_str1.lastIndexOf(".")) {
        k = index;
        index = ip_str1.indexOf(".", index);
        ip1[n] = toNumber(ip_str1, k, index);
        n++;
        index++;
    }
    ip1[n] = toNumber(ip_str1, index, ip_str1.length);

    index = 0;
    n = 0;
    ip2 = new Array(4);
    while (index < ip_str2.lastIndexOf(".")) {
        k = index;
        index = ip_str2.indexOf(".", index);
        ip2[n] = toNumber(ip_str2, k, index);
        n++;
        index++;
    }
    ip2[n] = toNumber(ip_str2, index, ip_str2.length);

    if (ip1[0] < ip2[0]) return - 1;
    else if (ip1[0] > ip2[0]) return 1;
    else {
        if (ip1[1] < ip2[1]) return - 1;
        else if (ip1[1] > ip2[1]) return 1;
        else {
            if (ip1[2] < ip2[2]) return - 1;
            else if (ip1[2] > ip2[2]) return 1;
            else {
                if (ip1[3] < ip2[3]) return - 1;
                else if (ip1[3] > ip2[3]) return 1;
                else return 0;
            }
        }
    }

}
function ipNum(ip_str1, ip_str2) {
    var index = 0;
    var n = 0;
    var ip1 = new Array(4);
    while (index < ip_str1.lastIndexOf(".")) {
        k = index;
        index = ip_str1.indexOf(".", index);
        ip1[n] = toNumber(ip_str1, k, index);
        n++;
        index++;
    }
    ip1[n] = toNumber(ip_str1, index, ip_str1.length);

    index = 0;
    n = 0;
    ip2 = new Array(4);
    while (index < ip_str2.lastIndexOf(".")) {
        k = index;
        index = ip_str2.indexOf(".", index);
        ip2[n] = toNumber(ip_str2, k, index);
        n++;
        index++;
    }
    ip2[n] = toNumber(ip_str2, index, ip_str2.length);
    return (ip2[0] - ip1[0]) * 256 * 256 * 256 + ((ip2[0] - ip1[0] == 0) ? 0 : 1) + ((ip2[1] - ip1[1] == 0) ? 0 : 1) + ((ip2[2] - ip1[2] == 0) ? 0 : 1) + ((ip2[3] - ip1[3] == 0) ? 0 : 1) + (ip2[1] - ip1[1]) * 256 * 256 + (ip2[2] - ip1[2]) * 256 + (ip2[3] - ip1[3]);
}
///////////////////////////////////////
//���ܣ��Ƚ���ʱ��Ĵ�С
//����˵��:
//cntrl1��cntrl2--�ؼ���
//tips--��ʾ��Ϣ
//����ֵ��trueС�ڻ���ڣ�false����
//////////////////////////////////////
function compareTime(cntrl1, cntrl2, tips) {
    if (! (cntrl1.value < cntrl2.value)) {
        alert(tips + "�Ŀ�ʼʱ�����С�ڽ���ʱ�䡣");
        cntrl1.focus();
        return false;
    }
    return true;
}
///////////////////////////////////////
//���ܣ�����Ƿ�Ϸ�ʱ���ʽ00:00:00
//����˵��:
//cntrl--�ؼ���
//allownull--1����Ϊ�գ�0������Ϊ��
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkTime(cntrl, allownull) {
    str = trimstr(cntrl.value);
    if (str == "") {
        if (!eval(allownull)) {
            alert("<% write(JSAlert40); %>");
            cntrl.focus();
            return false;
        } else return true;
    } else {
        val = 0;
        n = 0;
        index = 0;
        while (index < str.lastIndexOf(":")) {
            k = index;
            index = str.indexOf(":", index);
            if (!isNum(str.substring(k, index))) break;
            val = eval(toNumber(str, k, index));
            if (n == 0 && (val < 0 || val > 24)) break;
            if (val < 0 || val > 59) break;
            n++;
            index++;
        }

        if (n != 2) {
            alert("<% write(JSAlert41); %>");
            cntrl.focus();
            return false;
        } else {
            if (index == str.length) {
                alert("<% write(JSAlert41); %>");
                cntrl.focus();
                return false;
            } else {
                val = eval(toNumber(str, index, str.length));
                if (val < 0 || val > 59) {
                    alert("<% write(JSAlert41); %>");
                    cntrl.focus();
                    return false;
                }
            }
        }
    }
    cntrl.value = str;
    return true;
}
///////////////////////////////////////
//���ܣ�����Ƿ�Ϸ�Сʱ
//����˵��:
//cntrl--�ؼ���
//tipsObj--�����ʾ��Ϣ�Ŀռ�id
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkHour(cntrl, tips, tipsObj) {
	if(!tips) tips="<% write(JSAlert42); %>";
    var retips = '';
    var obj = tipsObj || cntrl;
    if (cntrl.value.length == 1) cntrl.value = "0" + cntrl.value;
    if (cntrl.value == "") {
        retips = tips + "<% write(JSAlert31); %>";
    } else if (!isNum(cntrl.value)) {
        retips = tips + "<% write(JSAlert14); %>";

    } else {
        var hour = toNumber(cntrl.value, 0, 2);
        if (hour < 0 || hour > 23) {
            retips = tips + "<% write(JSAlert14); %>";
        }
    }
    if (retips != '') {
        inputErro(obj, retips, 2, 1);
        return false;
    } else {
        inputRight(obj);
        return true;
    }
}
///////////////////////////////////////
//���ܣ�����Ƿ�Ϸ�����
//����˵��:
//cntrl--�ؼ���
//tipsObj--�����ʾ��Ϣ�Ŀռ�id
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkMinute(cntrl, tips, tipsObj) {
	if(!tips) tips="<% write(JSAlert43); %>";
    var retips = '';
    var obj = tipsObj || cntrl;
    if (cntrl.value.length == 1) cntrl.value = "0" + cntrl.value;
    if (cntrl.value == "") {
        retips = tips + "<% write(JSAlert31); %>";
    } else if (!isNum(cntrl.value)) {
        retips = tips + "<% write(JSAlert34); %>";

    } else {
        var minute = toNumber(cntrl.value, 0, 2);
        if (minute < 0 || minute > 59) {
            retips = tips + "<% write(JSAlert34); %>";
        }
    }
    if (retips != '') {
        inputErro(obj, retips, 2, 1);
        return false;
    } else {
        inputRight(obj);
        return true;
    }
}
///////////////////////////////////////
//���ܣ�����Ƿ�Ϸ�����
//����˵��:
//cntrl--�ؼ���
//tipsObj--�����ʾ��Ϣ�Ŀռ�id
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkSecond(cntrl, tips, tipsObj) {
	if(!tips) tips="<% write(JSAlert59); %>";
    var retips = '';
    var obj = tipsObj || cntrl;
    if (cntrl.value.length == 1) cntrl.value = "0" + cntrl.value;
    if (cntrl.value == "") {
        retips = tips + "<% write(JSAlert31); %>";
    } else if (!isNum(cntrl.value)) {
        retips = tips + "<% write(JSAlert34); %>";

    } else {
        var minute = toNumber(cntrl.value, 0, 2);
        if (minute < 0 || minute > 59) {
            retips = tips + "<% write(JSAlert34); %>";
        }
    }
    if (retips != '') {
        inputErro(obj, retips, 2, 1);
        return false;
    } else {
        inputRight(obj);
        return true;
    }
}

///////////////////////////////////////
//���ܣ���ǰ׺��ascii,ip,hex������û������Ƿ�Ϸ�
//����˵��:
//prefix--ǰ׺�ؼ���
//cntrl--�ؼ���
//tips--��ʾ��Ϣ
//allownull--1����Ϊ�գ�0������Ϊ��
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkPrefix(prefix, cntrl, tips, allownull) {
    if (prefix.value == "hex") {
        return checkHex(cntrl, tips, allownull);
    } else if (prefix.value == "ascii") {
        return true; //��Ϊ��checkName()���ʹ�ã��˴������ظ����
    } else if (prefix.value == "ip") {
        return checkIP(cntrl, tips, allownull);
    } else {
        alert(tips + "<% write(JSAlert34); %>");
        return false;
    }
}
///////////////////////////////////////
//���ܣ�����Ƿ�Ϸ�HEX����
//����˵��:
//cntrl--�ؼ���
//tips--��ʾ��Ϣ
//allownull--1����Ϊ�գ�0������Ϊ��
//����ֵ��true�Ϸ���false�Ƿ�
//////////////////////////////////////
function checkHex(cntrl, tips, allownull) {
    var num = trimstr(cntrl.value);
    if (num == "") {
        if (!eval(allownull)) {
            alert(tips + "<% write(JSAlert33); %>");
            cntrl.focus();
            return false;
        } else return true;

    } else {
        var length = num.length;
        for (var i = 0; i < length; i++) {
            onenum = num.substring(i, i + 1);
            if ((onenum < "0" || onenum > "9") && (onenum < "a" || onenum > "f") && (onenum < "A" || onenum > "F")) {
                alert(tips + "<% write(JSAlert44); %>");
                return false;
            }
        }
    }
    return true;
}

function isInt(theInt) {

    theInt = trimstr(theInt);
    if ((theInt.length > 1 && theInt.substring(0, 1) == "0") || !isNum(theInt)) return false;
    else return true;
}

function verifyDottedIP(ip_str) {
    if (ip_str == '0.0.0.0') return true;
    var val = 0;
    var i = index = 0;
    while (index < ip_str.lastIndexOf(".")) {
        k = index;
        index = ip_str.indexOf(".", index);
        val = toNumber(ip_str, k, index);
        if (k == 0) if ((val == 0) || (val == 127) || (val >= 224)) {
            return false;
        }
        if (!isInt(ip_str.substring(k, index))) break;
        if (val < 0 || val > 255) break;
        i++;
        index++;
    }
    if (i != 3) return false;
    else {
        if (index == ip_str.length) return false;
        else {
            val = toNumber(ip_str, index, ip_str.length);
            if ((val < 0 || val > 255) || !isInt(ip_str.substring(index, ip_str.length))) return false;
        }
    }

    return true;
}
function verifyDottedIP2(ip_str) {
    var val = 0;
    var i = index = 0;

    while (index < ip_str.lastIndexOf(".")) {
        k = index;
        index = ip_str.indexOf(".", index);
        val = toNumber(ip_str, k, index);
        if (!isInt(ip_str.substring(k, index))) break;
        if (val < 0 || val > 255) break;
		if (i==0 && (val < 1 || val > 223 || val == 127)) break;//windows �涨IP��ַ��һ��ֻ��ȡ1~223֮�����
        i++;
        index++;
    }

    if (i != 3) return false;
    else {
        if (index == ip_str.length) return false;
        else {
            val = toNumber(ip_str, index, ip_str.length);
            if ((val <= 0 || val > 255) || !isInt(ip_str.substring(index, ip_str.length))) return false;
        }
    }

    return true;
}
/*
function verifyDottedIP3(ip_str) {
    var val = 0;
    var i = index = 0;
    var mark = 0;

    while (index < ip_str.lastIndexOf(".")) {
        k = index;
        index = ip_str.indexOf(".", index);
        val = toNumber(ip_str, k, index);
        if (!isInt(ip_str.substring(k, index))) break;
        if (val < 0 || val > 255) break;

        if (i == 0 && val == 0) mark = 1;

        i++;
        index++;
    }

    if (i != 3) return false;
    else {
        if (index == ip_str.length) return false;
        else {
            val = toNumber(ip_str, index, ip_str.length);
            if ((val < 0 || val > 255) || !isInt(ip_str.substring(index, ip_str.length))) return false;
        }
    }
    if (ip_str == "127.0.0.1") return false;
    if (mark == 1 && ip_str != "0.0.0.0") return false;

    return true;
}*/
function trimstr(str) {
    str = str.toString();
    if (str == "0") return "0";
    if (str == "") return "";

    while (str.length > 0 && str.indexOf(' ') == 0) {
        str = str.substring(1, str.length);
    }
    while (str.length > 0 && str.lastIndexOf(' ') == str.length - 1) {
        str = str.substring(0, str.length - 1);
    }

    return str;
}

function toNumber(str, start, end) {
	str=str.toString();
	if(!end) end = str.length;
	if(!start) start = 0;
    var tempVal = 0;
    var mySign = 1;
    for (i = start; i < end; i++) {
        c = str.charAt(i);
        if (c < '0' || c > '9') {
            if (i != start && (c != '-' || c != '+')) //?
            return - 1;
            if (c == '-') mySign = -1;
        } else tempVal = tempVal * 10 + (c - '0');
    }
    tempVal *= mySign;
    return tempVal;
}
///////////////////////////////////////
//���ܣ���ʱ��ת��Ϊ�루��UTC��ȵĲ�ֵ��
//����˵��:
//timezone--ʱ���ַ���
//����ֵ����
//////////////////////////////////////
function TimezoneToSecond(timezone) {
    if (timezone == "UTC+0100") return 3600;
    else if (timezone == "UTC+0200") return 7200;
    else if (timezone == "UTC+0300") return 10800;
    else if (timezone == "UTC+0800") return 28800;
    else if (timezone == "UTC+0900") return 32400;
    else if (timezone == "UTC-0500") return - 18000;
    else if (timezone == "UTC-0800") return - 28800;
    else if (timezone == "UTC-1000") return - 36000;
    else return 0;
}
///////////////////////////////////////
//���ܣ���UTCʱ��ת��Ϊ����ʱ��
//����˵��:
//time--��
//timezone--ʱ����ֵ�������ʾ
//����ֵ����
//////////////////////////////////////
function UTCtoLocal(time, timezone) {
    date = new Date();
    return time + timezone + date.getTimezoneOffset() * 60;
}
///////////////////////////////////////
//���ܣ��ѱ���ʱ��ת��ΪUTCʱ��
//����˵��:
//time--��
//timezone--ʱ����ֵ�������ʾ
//����ֵ����
//////////////////////////////////////
function LocalToUTC(time, timezone) {
    date = new Date();
    return time - timezone - date.getTimezoneOffset() * 60;
}
///////////////////////////////////////
//���ܣ���00:00:00��ʽ��ʱ��ת��Ϊ����ʱ��
//����˵��:
//cntrl--�ؼ�����00:00:00��ʽ��ʱ��
//cntrl1--�ؼ���������ʱ��
//////////////////////////////////////
function StrToTime(cntrl, cntrl1) {
    str = trimstr(cntrl.value);
    hour = minute = second = 0;
    index = str.indexOf(":", 0);
    hour = eval(toNumber(str, 0, index));
    str = str.substring(index + 1, str.length);
    index = str.indexOf(":", 0);
    minute = eval(toNumber(str, 0, index));
    second = eval(toNumber(str, index + 1, str.length));
    time = 60 * 60 * hour + 60 * minute + second;
    cntrl1.value = time.toString();
}
///////////////////////////////////////
//���ܣ�����ת��Ϊ00:00:00:00(��:ʱ:��:��)��ʽ
//����˵��:
//time--��
//////////////////////////////////////
function getUsedTime(time) {
    if (time < 1) return "00:00:00:00";
    day = parseInt(time / 86400);
    time = time % 86400;
    if (10 > day) Day = "0" + day;
    else Day = "" + day;
    return Day + ":" + TimeToStr(time);
}
///////////////////////////////////////
//���ܣ�����ת��Ϊ00:00:00��ʽ
//����˵��:
//time--��
//////////////////////////////////////
function TimeToStr(time) {
    hour = parseInt(time / 3600);
    time %= 3600;
    minute = parseInt(time / 60);
    second = time % 60;
    if (10 > hour) Hour = "0" + hour;
    else Hour = "" + hour;
    if (10 > minute) Minute = "0" + minute;
    else Minute = "" + minute;
    if (10 > second) Second = "0" + second;
    else Second = "" + second;
    str = Hour + ":" + Minute + ":" + Second;
    return str;
}
///////////////////////////////////////
//���ܣ�������ʱ��ת��Ϊ00:00:00��ʽ
//����˵��:
//time--����ʱ��
//////////////////////////////////////
function toTime(time) {
    sysDate = new Date(eval(time) * 1000);
    if (10 > sysDate.getHours()) Hour = "0" + sysDate.getHours();
    else Hour = "" + sysDate.getHours();
    if (10 > sysDate.getMinutes()) Minute = "0" + sysDate.getMinutes();
    else Minute = "" + sysDate.getMinutes();
    if (10 > sysDate.getSeconds()) Second = "0" + sysDate.getSeconds();
    else Second = "" + sysDate.getSeconds();
    return (Hour.toString() + ":" + Minute.toString() + ":" + Second.toString());
}
///////////////////////////////////////
//���ܣ��Ѻ�������ת��Ϊ�硰1900��1��1�ա��ĸ�ʽ
//����˵��:
//date--��������
//////////////////////////////////////
function toDate(date) {
    sysDate = new Date(eval(date) * 1000);
    Year = sysDate.getFullYear();
    Month = sysDate.getMonth();
    Day = sysDate.getDate();
    Month1 = Month + 1;
    return (Year.toString() + "<% write(com_year); %>" + Month1.toString() + "<% write(com_month); %>" + Day.toString() + "<% write(com_day1); %>");
}
///////////////////////////////////////
//���ܣ�ֻ��������Ϸ����֣�����С����͸���
//////////////////////////////////////
function numberOnly() {
    if (! (((window.event.keyCode >= 48) && (window.event.keyCode <= 57)) || (window.event.keyCode == 13) || (window.event.keyCode == 46) || (window.event.keyCode == 45))) {
        window.event.keyCode = 0;
    }
}
///////////////////////////////////////
//���ܣ�ֻ��������Ϸ����֣�������С����͸���
//////////////////////////////////////
function figureOnly() {
    if (! ((window.event.keyCode >= 48) && (window.event.keyCode <= 57))) {
        window.event.keyCode = 0;
    }
}

///////////////////////////////////////
//���ܣ�ֻ��������ip��ַ
//////////////////////////////////////
function ipOnly() {
    if (! (((window.event.keyCode >= 48) && (window.event.keyCode <= 57)) || (window.event.keyCode == 46))) {
        window.event.keyCode = 0;
    }
}
///////////////////////////////////////
//���ܣ�ֻ��������MAC��ַ
//////////////////////////////////////
function macOnly() {
    if (! (((window.event.keyCode >= 48) && (window.event.keyCode <= 57)) || ((window.event.keyCode >= 97) && (window.event.keyCode <= 102)) || (window.event.keyCode == 45) || ((window.event.keyCode >= 65) && (window.event.keyCode <= 70)))) {
        window.event.keyCode = 0;
    }
}
///////////////////////////////////////
//���ܣ�ֻ��������16����
//////////////////////////////////////
function HEXOnly() {
    if (! (((window.event.keyCode >= 48) && (window.event.keyCode <= 57)) || ((window.event.keyCode >= 97) && (window.event.keyCode <= 102)) || ((window.event.keyCode >= 65) && (window.event.keyCode <= 70)))) {
        window.event.keyCode = 0;
    }
}

function ipv6Only() {
    if (! (((window.event.keyCode >= 48) && (window.event.keyCode <= 57)) || ((window.event.keyCode >= 97) && (window.event.keyCode <= 102)) || ((window.event.keyCode >= 65) &&
 (window.event.keyCode <= 70)) ||(window.event.keyCode == 58))) {
        window.event.keyCode = 0; 
    }    
}
///////////////////////////////////////
//���ܣ�ֻ��������ʱ��00:00:00
//////////////////////////////////////
function timeOnly() {
    if (! (((window.event.keyCode >= 48) && (window.event.keyCode <= 57)) || (window.event.keyCode == 58))) {
        window.event.keyCode = 0;
    }
}

function HourOnly(obj) {
    var curValue = obj.value + "" + (window.event.keyCode - 48);
    curValue = parseInt(curValue);

    if (! (((window.event.keyCode >= 48) && (window.event.keyCode <= 57)) && (curValue < 24))) {
        window.event.keyCode = 0;
    }
}

function Minute_Second_Only(obj) {
    var curValue = obj.value + "" + (window.event.keyCode - 48);
    curValue = parseInt(curValue);

    if (! (((window.event.keyCode >= 48) && (window.event.keyCode <= 57)) && (curValue < 60))) {
        window.event.keyCode = 0;
    }
}

function clearDefaultText(el, message) {
    el.value = "";
}

///////////////////////////////////////
//���ܣ�����������
//////////////////////////////////////
function inputless(e) {
    e = e || window.event;
    try{/*ie*/
    		e.keyCode=0; 
    		e.returnValue=false;
   	}catch(ex){/*ff*/
    	e.preventDefault();
   }
}
///////////////////////////////////////
//���������� : ,  %  & ; \' \\ \"  �Ϳո�֮����ַ�
function nameOnly() {
    if ((window.event.keyCode == 58) || (window.event.keyCode == 44) || (window.event.keyCode == 37) || (window.event.keyCode == 38) || (window.event.keyCode == 59) || (window.event.keyCode == 92) || (window.event.keyCode == 39) || (window.event.keyCode == 34) || (window.event.keyCode == 32)) {
        window.event.keyCode = 0;
    }
}
///////////////////////////////////////
//����������% \' \\ \"  �Լ��ո�֮����ַ�
function passwordonly() {
    if ((window.event.keyCode == 37) || (window.event.keyCode == 92) || (window.event.keyCode == 39) || (window.event.keyCode == 34) || (window.event.keyCode == 32)) {
        window.event.keyCode = 0;
    }
}

///////////////////////////////////////
//�������� abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.<>/?[]{}-_+=|`~!@$^&*()
function englishnamedonly() {
    if (! (((window.event.keyCode >= 48) && (window.event.keyCode <= 57)) || ((window.event.keyCode >= 97) && (window.event.keyCode <= 122)) || ((window.event.keyCode >= 65) && (window.event.keyCode <= 90)) || ((window.event.keyCode >= 40) && (window.event.keyCode <= 47)) || ((window.event.keyCode >= 60) && (window.event.keyCode <= 64)) || ((window.event.keyCode >= 93) && (window.event.keyCode <= 96)) || ((window.event.keyCode >= 123) && (window.event.keyCode <= 126)) || (window.event.keyCode == 33) || (window.event.keyCode == 36) || (window.event.keyCode == 38) || (window.event.keyCode == 91)

    )) {
        window.event.keyCode = 0;
    }
}
////////////////////////////////////////////////////////////////////
//�������б���
///////////////////////////////////////////////////////////////////
var MAX_LIST = 5;

var currpage = new Array(MAX_LIST);
var recscount = new Array(MAX_LIST);
var lineButton=new Array(MAX_LIST);
var maxrecs = new Array(MAX_LIST);
var listname = new Array(MAX_LIST);
var listheadname = new Array(MAX_LIST);
var listheadcaption = new Array(MAX_LIST);
var allcheckobj = new Array(MAX_LIST);
var recsPerPage = new Array(MAX_LIST);
//�����0���ȸ�ֵ
recsPerPage[0] = 5;
recsPerPage[1] = 5;
recsPerPage[2] = 5;
recsPerPage[3] = 5;
recsPerPage[4] = 5;

TYPE_STR = 1;
TYPE_NUM = 2;
TYPE_IP = 3;
TYPE_PERCENT = 4;
TYPE_CHINESE = 5; //ADD BY JYSHEN
TYPE_DATE = 6; //add by jyshen
TYPE_DATASIZE = 7; //�洢�ռ��Сb,kb,mb,gb��
seq = true; //true˳��false����
sortable = new Array(new Array(), new Array(), new Array()); //�Ƿ������
sortType = new Array(new Array(), new Array(), new Array(), new Array()); //�������ͣ�1�ַ���2���֡�3IP��ַ��4�ٷֺţ�5��Ӣ��
maxDspLen = new Array(new Array(), new Array(), new Array());
arrayCount = new Array(); //�����������Ŀ

//��ӡҳͷ
function out_head(obj) {
    obj.document.close();
    obj.document.open();

    try {
        if (color.toLowerCase() == "blue") {
            obj.document.write("<html><head><style><!--@import url('../css/style_x.css');--></style></head><body>");
        } else if (color.toLowerCase() == "green") {
            obj.document.write("<html><head><style><!--@import url('../css/style.css');--></style></head><body>");
        }
    } catch(ex) {
        obj.document.write("<html><head><style><!--@import url('../css/style.css');--></style></head><body>");
    }
}
//��ӡҳβ
function out_tail(obj) {
    obj.document.write("</body></html>");
}
//����һҳ
function firstpage(index, listhead, list, listTail) {
    currpage[index] = 1;
    dsplisthead(index, listhead);
    dsplistTail(index, listTail);
    dsplist(index, list);

}
//�����һҳ
function lastpage(index, listhead, list, listTail) {
    if (recscount[index] == 0) currpage[index] = 1;
    else currpage[index] = Math.floor((recscount[index] - 1) / recsPerPage[index]) + 1;
    dsplisthead(index, listhead);
    dsplistTail(index, listTail);
    dsplist(index, list);
}

//dsplistTail
function goanotherpage(index, listhead, list) {
    var pagenum;
    var lastpage;
    if (recscount[index] > 0) lastpage = Math.floor((recscount[index] - 1) / recsPerPage[index]) + 1;
    else lastpage = 0;
	pagenum = parseInt(listhead.document.getElementsByName("pagenumber")[0].value);
    if (isNaN(pagenum)) {
        alert("<% write(JSAlert45); %>");
        return;
    }
    if (currpage[index] == pagenum) return;
    if (pagenum > 0 && pagenum <= lastpage) {
        currpage[index] = pagenum;
        dsplisthead(index, listhead);
        dsplist(index, list);
    }
}

function gotopage(e, index, listhead, list) {
	var code = e.keyCode || e.which;
    if (code == 13) goanotherpage(index, listhead, list);
    else if (! ((code >= 48) && (code <= 57))) {
		inputless(e);
    } else ;
}

//����һҳ
function nextpage(index, listhead, list, listTail) {
    if (currpage[index] * recsPerPage[index] < recscount[index]) {
        currpage[index] = currpage[index] + 1;
        dsplisthead(index, listhead);
        dsplistTail(index, listTail);
        dsplist(index, list);
    }
}
//����һҳ
function prepage(index, listhead, list, listTail) {
    if (currpage[index] > 1) {
        currpage[index] = currpage[index] - 1;
        dsplisthead(index, listhead);
        dsplistTail(index, listTail);
        dsplist(index, list);
    }
}
//ȫѡ
function selectAllCheck(index, frmchk, listchk) {
    var recbegin;
    var recend;
    recbegin = (currpage[index] - 1) * recsPerPage[index];
    recend = recbegin + recsPerPage[index];
    if (recend >= recscount[index]) recend = recscount[index];
    var t = 0;
    for (var i = recbegin; i < recend; i++) {
        if (listchk.length && !listchk[t].disabled) listchk[t].checked = frmchk.checked;
        else if (!listchk.disabled) listchk.checked = frmchk.checked;
        t = t + 1;
    }
    eval("allcheckobj[" + index.toString() + "]=frmchk;");
}
/*************************
��ֹ�������ø�ѡ��
**************************/
function disableAllCheck(index, list, isDisabled) {
    var chks = list.document.getElementsByName("chklist");
    var len = chks.length || 0;
    if (len == 0) return;
    for (var i = 0; i < len; i++) {
        chks[i].disabled = isDisabled;
    }
}
/***************************
����IFRAME�ĸ߶� (����Ӧģʽ)
***************************/
function SetIFrameHeight(id) {
    var cwin = document.getElementById(id);

    if (document.getElementById) {
        if (cwin && !window.opera) {
            if (cwin.contentDocument && cwin.contentDocument.body.offsetHeight) {
                cwin.height = cwin.contentDocument.body.offsetHeight + 20;
            } else if (cwin.Document && cwin.Document.body.scrollHeight) {
                cwin.height = cwin.Document.body.scrollHeight + 20;
            }
        }
    }
}

/***************************
����str������tok��һ�γ��ֵ�
������ignoreCaseΪtrue�����
��Сд
***************************/
function searchStr(tok, str, ignoreCase) {
    var toklen = 0;
    var strlen = 0;
    var ret = -1;

    if (tok == null || str == null) {
        return ret;
    }

    tok = trimstr(tok.toString());
    str = trimstr(str.toString());
    toklen = tok.length;
    strlen = str.length;

    if (strlen == 0 || toklen == 0) {
        return ret;
    }

    if (ignoreCase) {
        str = str.toLowerCase();
        tok = tok.toLowerCase();
    }

    for (var i = 0; i < strlen; i++) {
        if (tok == str.substring(i, i + toklen)) {
            ret = i;
            return ret;
        }
    }

    return ret;
}

/******************************
��string���ȡsubstrλ��ǰ��
�ַ���
******************************/
function _strStr(string, substr) {
    var index = 0;
    var _strstr = "";

    if (string == null || substr == null) {
        return null;
    }

    string = string.toString();
    substr = substr.toString();

    index = searchStr(substr, string, true);

    if (index <= 0) {
        return null;
    }

    _strstr = string.substring(0, index);
    return _strstr;
}

/***************************
��subStrΪ�ָ�����ȡ��string
�зָ�������ַ���
***************************/
function strStr(string, subStr) {
    var index = 0;
    var strstr = "";

    if (string == null || subStr == null) {
        return null
    }

    string = string.toString();
    subStr = subStr.toString();

    index = searchStr(subStr, string, true);

    if (index == -1) {
        return null;
    }

    index += subStr.length;

    if (index >= string.length) {
        return null;
    }

    strstr = string.substring(index, string.length);
    return strstr;
}

/******************************
��string�������γ���substr
λ�ú���ַ���
******************************/
function strChr(string, substr) {
    var index = 0;
    var len = string.length;

    string = string.toString();
    substr = substr.toString();
    index = string.lastIndexOf(substr);

    if (index == -1) {
        return null;
    }

    return string.substring(index + substr.length, len);
}
/******************************
��string�������γ���substr
λ��ǰ���ַ���
******************************/
function _strChr(string, substr) {
    var index = 0;
    var len = string.length;

    string = string.toString();
    substr = substr.toString();
    index = string.lastIndexOf(substr);

    if (index == -1) {
        return null;
    }

    return string.substring(0, index);
}

/*****************************
��str�й���tok��ÿ��Ԫ�ر���
�����飬�����ظ�����
******************************/
function strtok(tok, str) {
    var strArray = new Array();
    var n = 0;
    var strLen = 0;
    var tokLen = 0;
    var index = 0;
    var seek = 0;
    var tmp = "";
    var flag = false;

    if (str == null || tok == null) {
        return null;
    }

    tokLen = tok.length;
    strLen = str.length;

    if (strLen == 0) {
        return null;
    }

    if (tokLen == 0) {
        strArray[n] = str;
        return strArray;
    }

    if (tokLen > strLen) {
        return null;
    }

    index = searchStr(tok, str, true);

    while (index != -1) {
        tmp = str.substring(0, index);

        if (tmp.length > 0) {
            strArray[n++] = tmp;
        }

        seek = index + tok.length;
        str = str.substring(seek, str.length);
        flag = true;
        index = searchStr(tok, str, true);
    }

    if (str.length != 0) {
        strArray[n] = str;
    }

    return strArray;
}

/****************************************
��ȡstr�ַ�������startTokΪ��ͷ����endTok
Ϊ��β���ַ���
*****************************************/
function subStr(str, startTok, endTok) {
    var start = 0;
    var end = 0;

    start = searchStr(startTok, str, true);

    if (start == -1) {
        return null;
    }

    end = searchStr(endTok, str, true);
    str = str.substring(start + startTok.length, end + endTok.length - 1);

    return str;
}

/****************************************
���URL�в�����ֵ�����������ͷ���
*****************************************/
function getQueryString(name) {
    var parValues = null;
    var url = strStr(window.location, "?");
    if (url == null) {
        return parValues;
    }

    url = trimstr(url);
    url = eliminateStr("&", url, true);
    parValues = strtok(name + "=", url);

    if (url == parValues) {
        return null;
    }

    return parValues;
}

/****************************************
����str�ַ�����tok���֣�lgoreCaseΪ��ʱ
���Դ�Сд
���ع��˺���ַ���
*****************************************/
function eliminateStr(tok, str, lgoreCase) {
    var tmp = "";
    var start = 0;
    var len = 0;
    var index = searchStr(tok, str, lgoreCase);

    tok = trimstr(tok.toString());
    str = trimstr(str.toString());
    len = str.length;

    while (index != -1) {
        tmp += str.substring(0, index);
        start = index + tok.length;
        str = str.substring(start, len);
        index = searchStr(tok, str, lgoreCase);
    }

    tmp += str;
    return tmp;
}
/*****************************
ͨ��������ʵ�ֶ��ַ�����ɸѡ
���� �ַ�������
******************************/

function strtokIP(tok, str, fillStr) {
    var len = 0;
    var strLen = 0;
    var flag = false;
    var string = "";

    str = trimstr(str);
    tmpArray = str.split(tok);
    len = tmpArray.length;

    for (var i = 0; i < len; i++) {
        tmp = tmpArray[i];
        strLen = tmp.length;

        while (strLen < 3) {
            tmp = fillStr + tmp;
            strLen = tmp.length;
        }

        string += tmp;
    }

    return string;
}

/**********************
��arrayData������ָ���ַ���
***********************/
var titlecount = new Array();
var count1 = 0;
var isNotFirstSearch1 = false;
var strSearch1 = "";
var cuPage1 = 0;
var array1 = new Array();
var _array1 = new Array();
var nIndex1 = 0;
var tmpIndex1 = 0;

var searchInfo1 = new Array(count1, isNotFirstSearch1, strSearch1, cuPage1, array1, _array1, nIndex1, tmpIndex1);

var count2 = 0;
var isNotFirstSearch2 = false;
var strSearch2 = "";
var cuPage2 = 0;
var array2 = new Array();
var _array2 = new Array();
var nIndex2 = 0;
var tmpIndex2 = 0;

var searchInfo2 = new Array(count2, isNotFirstSearch2, strSearch2, cuPage2, array2, _array2, nIndex2, tmpIndex2);

var count3 = 0;
var isNotFirstSearch3 = false;
var strSearch3 = "";
var cuPage3 = 0;
var array3 = new Array();
var _array3 = new Array();
var nIndex3 = 0;
var tmpIndex3 = 0;

var searchInfo3 = new Array(count3, isNotFirstSearch3, strSearch3, cuPage3, array3, _array3, nIndex3, tmpIndex3);
var _array = new Array();
var tmpIndex = 0;

function zeroData(searchInfo) {

    if (searchInfo == null) {
        return
    }

    searchInfo[0] = 0;
    searchInfo[6] = 0;
    searchInfo[1] = false;
}

function matchIPToArray(arrayData, searchIP, innerIPIndex, outerIPIndex) {
    var ipArray = new Array();
    var index = 0;
    var isExist = false;
    searchIP = trimstr(searchIP);

    if (innerIPIndex == -1 && outerIPIndex == -1) {
        ipArray[0] = searchIP;
        return ipArray;
    }

    for (var i = 0; i < arrayData[innerIPIndex].length; i++) {
        if (trimstr(arrayData[innerIPIndex][i]) == searchIP) {
            for (var j = 0; j < ipArray.length; j++) {
                if (ipArray[j] == trimstr(arrayData[outerIPIndex][i])) {
                    isExist = true;
                }
            }

            if (isExist == false) {
                ipArray[index++] = trimstr(arrayData[outerIPIndex][i]);
                continue;
            }
        }
        isExist = false;
    }

    if (ipArray.length == 0) ipArray = null;
    return ipArray;

}

//new search function
//�������������б��ػ滭ʱ��ֻ��ʾ�������ؼ���ƥ��Ķ�
var isSearched = new Array(false, false, false, false, false);
var tmpArrayData = new Array(new Array(), new Array(), new Array(), new Array(), new Array());
var _tmpArrayData = new Array();
var pageCount = new Array(0, 0, 0, 0, 0);
var dataIndexArray = new Array();
var arrayName = null;

function searchDataEx(e, index, listhead, list, arrayData, strSearch, arrayIndex, listTail) { //modify by cxhu 090329
    //var e = evt || listhead.event; 
    var code = e.keyCode || e.which;
    if (code != 13) { //�����µĲ��ǻس����򷵻�
        return;
    }
    var titleCount = 0;

    if (arrayIndex == 1) {
        arrayName = "arrayData1"; //��ס��ǰ����������������
        //titleCount = parent.titles1.length;	
        titleCount = titles1.length;
    } else if (arrayIndex == 2) {
        arrayName = "arrayData2";
        titleCount = titles2.length;
    }
    if (strSearch == "" && !isSearched[index]) {
        return;
    }

    if (strSearch == "" && isSearched[index]) { //����û��������κ����ݲ����û���ǰ��������������ԭ����
        if (arrayIndex == 1) {
            for (var i = 0; i < tmpArrayData[index].length; i++) {
                for (var j = 0; j < tmpArrayData[index][0].length; j++) {
                    arrayData1[i][j] = tmpArrayData[index][i][j];
                }
            }

        } else if (arrayIndex == 2) {
            for (var i = 0; i < tmpArrayData[index].length; i++) {
                for (var j = 0; j < tmpArrayData[index][0].length; j++) {
                    arrayData2[i][j] = tmpArrayData[index][i][j];
                }
            }
        }

        recscount[index] = pageCount[index];
        isSearched[index] = false;
        dsplisthead(index, listhead, arrayName);
        dsplistTail(index, listTail); //�ػ���
        dsplist(index, list);
        return;
    }

    var iCount = 0,
    tmpCount = 0;
    var flag = false;
    var leap = false;

    var tempArray = new Array();
    var tmpArray = new Array();
    var indexArray = new Array();
    indexArray[0] = -1;

    for (var i = 0; i < arrayData.length; i++) {
        tmpArray[i] = new Array();
        tempArray[i] = new Array();
    }

    for (var i = 0; i < titleCount; i++) {
        for (var j = 0; j < arrayData[i].length; j++) { //����������������������
            for (var n = 0; n < indexArray.length; n++) {
                if (j == indexArray[n]) {
                    leap = true;
                }
            }

            if (leap) {
                leap = false;
                continue;
            }

            //���������ݣ��������ݰ��и�ֵ����ʱ����
            if (searchStr(strSearch, trimstr(arrayData[i][j].toString()), true) != -1) {
                for (var k = 0; k < arrayData.length; k++) {
                    tmpArray[k][iCount] = arrayData[k][j];
                }

                if (isSearched[index] == false) {
                    dataIndexArray[iCount] = j;
                }

                flag = true;
                indexArray[iCount] = j;
                iCount++;
            }
        }
    }

    dataIndexArray = retrenchArray(dataIndexArray, true);

    if (isSearched[index] == true) {
        _dataIndexArray = new Array();
        for (var i = 0; i < titleCount; i++) {
            for (var j = 0; j < arrayData[i].length; j++) {
                if (searchStr(strSearch, trimstr(arrayData[i][j].toString()), true) != -1) {
                    _dataIndexArray[tmpCount++] = dataIndexArray[j];
                }
            }
        }

        dataIndexArray = new Array();
        _dataIndexArray = retrenchArray(_dataIndexArray, true);
        dataIndexArray = _dataIndexArray.slice(0);
    }

    if (!flag && strSearch != "") {
        alert('<% write(JSAlert46); %>');
        return;
    }

    //��¼������ǰ�������������Ա㻹ԭ
    if (!isSearched[index]) {
        pageCount[index] = recscount[index];
    }

    currpage[index] = 1;
    recscount[index] = iCount;
    iCount = 0;

    //��¼�����������������������ػ��б�
    //���ڶ���б�
    if (arrayIndex == 1) {
        if (!isSearched[index]) {
            for (var i = 0; i < arrayData1.length; i++) {
                tmpArrayData[index][i] = new Array();
                tmpArrayData[index][i] = arrayData1[i].slice(0);
            }
        }

        arrayName = "arrayData1";

        for (var i = 0; i < tmpArray.length; i++) {
            for (var j = 0; j < tmpArray[0].length; j++) {
                arrayData1[i][j] = tmpArray[i][j];
            }

            arrayData1[i].splice(j, arrayData1[i].length);
        }
    } else if (arrayIndex == 2) {
        if (!isSearched[index]) {
            for (var i = 0; i < arrayData2.length; i++) {
                tmpArrayData[index][i] = new Array();
                tmpArrayData[index][i] = arrayData2[i].slice(0);
            }
        }

        arrayName = "arrayData2";

        for (var i = 0; i < tmpArray.length; i++) {
            for (var j = 0; j < tmpArray[0].length; j++) {
                arrayData2[i][j] = tmpArray[i][j];
            }

            arrayData2[i].splice(j, arrayData2[i].length);
        }
    }
    isSearched[index] = true;

    dsplisthead(index, listhead, arrayName);
    dsplistTail(index, listTail);
    dsplist(index, list);
}

/****************************
�Ƚ������ַ����Ĵ�С
str_chinese1 > str_chinese2 ����1
str_chinese1 < str_chinese2 ����-1
��ͬ���� 0
*****************************/

function compareChinese(str_chinese1, str_chinese2) {

    return str_chinese1.localeCompare(str_chinese2);
}

/***************************
�ж����ڴ�С
srcStr > dstStr ����1
dstStr > srcStr ����-1
��ͬ����0
****************************/
function compareDate(srcDate, dstDate) {
    var ret = -2;
    var len = 0
    var srcArray = strNum(srcDate);
    var dstArray = strNum(dstDate);

    if (srcArray == null || dstArray == null) {
        return ret;
    }

    len = srcArray.length > dstArray.length ? dstArray.length: srcArray.length;

    for (var i = 0; i < len; i++) {
        if (Number(srcArray[i]) > Number(dstArray[i])) {
            ret = 1;
            return ret;
        } else if (Number(srcArray[i]) < Number(dstArray[i])) {
            ret = -1;
            return ret;
        }
    }

    if (srcArray.length > dstArray.length) {
        ret = 1;
    } else if (srcArray.length == dstArray.length) {
        ret = 0;
    } else {
        ret = -1;
    }

    return ret;
}
/*ת������Ϊbyte,֧��kb��mb��gb��tb,pb��byte��ת��*/
function convertToByte(num) {
    var ret;
    num = num || 0;
    num = num.toString();
    num = num.toUpperCase();
    try {
        ret = parseFloat(eval(num));
        /*������eval��Ϊ�������Ĳ���������16����*/
    } catch(e) {
        ret = parseFloat(num);
    }
    if (num.indexOf("K") >= 0) ret = ret * 1024; //��λΪkb
    else if (num.indexOf("M") >= 0) ret = ret * 1024 * 1024; //��λΪmb
    else if (num.indexOf("G") >= 0) ret = ret * 1024 * 1024 * 1024; //��λΪgb
    else if (num.indexOf("T") >= 0) ret = ret * 1024 * 1024 * 1024 * 1024; //��λΪtb
    else if (num.indexOf("P") >= 0) ret = ret * 1024 * 1024 * 1024 * 1024 * 1024; //��λΪpb
    ret = parseInt(ret); //byte��������С��λ
    return ret;

}
/*��ʽ��byteΪkb,mb,gb,tb,pb*/
function byteFormat(num) {
    var retNum = parseFloat(eval(num || 0));
    var retUnit = "B";
    var flag = 0;
    while (retNum > 1024) {
        retNum = retNum / parseFloat(1024);
        flag++;
    }
    retNum = Math.round(retNum * 100);
    retNum = parseFloat(retNum / 100);
    switch (flag) {
    case 1:
        retUnit = "KB";
        break;
    case 2:
        retUnit = "MB";
        break;
    case 3:
        retUnit = "GB";
        break;
    case 4:
        retUnit = "TB";
        break;
    case 5:
        retUnit = "PB";
        break;
    default:
        retUnit = "B";
        break;
    }
    return retNum + retUnit;

}
/*************************
�Ƚ�����ռ�ÿռ��С(add by bhou)
*************************/
function compareDataSize(num1, num2) {
    num1 = convertToByte(num1);
    num2 = convertToByte(num2);
    if (num1 > num2) {
        return 1;
    } else if (num1 < num2) {
        return - 1;
    } else {
        return 0;
    }
}
/*************************
�Է�����Ϊ���Ʒָ��ַ���
��������
����1999��5��3�գ����ص�
����Ԫ��Ϊ1999, 5, 3
*************************/
function strNum(strDate) {
    var len = 0;
    var flag = false;
    var array = new Array();
    var index = 0;
    var str = "";

    len = strDate.length;

    for (var i = 0; i < len; i++) {
        if (strDate.charCodeAt(i) >= 48 && strDate.charCodeAt(i) <= 57) {
            str += strDate.charAt(i);
            flag = false;
        } else {
            if (!flag && str != "") {
                array[index] = str;
                str = "";
                index++;
                flag = true;
            }
        }
    }

    if (str != "") {
        array[index] = str;
    }

    return array;

}


function compareNum(num1, num2) {
	/*�ѿյ�����С*/
	num1 = num1 || "";
	num2 = num2 || "";
	if(!trimstr(num1)) return -1;
	if(!trimstr(num2)) return 1;
	num1=parseNumber(num1);/*ȡǰ�������*/
	num2=parseNumber(num2);
	if (num1 > num2) {
		return 1;
	}
	else if (num1 < num2) {
		return -1;
	} else {
		return 0;
	}
}

/*************************
�ȽϰٷֺŴ�С��ͬʱ�Ƚ�
���ʴ�С
*************************/
function comparePercent(str1, str2) {
    var ret = 0;
    var tmp1 = 0;
    var tmp2 = 0;

    str1 = str1.toString();
    str2 = str2.toString();
    var flag1 = str1.substring(str1.length - 1, str1.length).toLowerCase();
    var flag2 = str2.substring(str2.length - 1, str2.length).toLowerCase();

    if (flag1 == "k" && flag2 != "k") {
        ret = 1;
        return ret;
    } else if (flag1 != "k" && flag2 == "k") {
        ret = -1;
        return ret;
    }

    if (str1.length > 1) {
        tmp1 = eval(str1.substring(0, str1.length - 1));
    } else {
        tmp1 = eval(str1);
    }

    if (str2.length > 1) {
        tmp2 = eval(str2.substring(0, str2.length - 1));
    } else {
        tmp2 = eval(str2);
    }

    if (tmp1 > tmp2) {
        ret = 1;
        return ret;
    }

    if (tmp1 < tmp2) {
        ret = -1;
        return ret;
    }

    return ret;
}
/*********************************
����������ʽ���ж��ַ������������ģ�
�ĳ���
**********************************/
String.prototype.lstrLen = function() {
    var cArr = this.match(/[^\x00-\xff]/ig); //�жϲ�����ASCII��Ϊ x00 - xff
    return this.length + (cArr == null ? 0 : cArr.length);
}
String.prototype.strlen = function() {
	var cArr = this.match(/[^\x00-\xff]/ig);	//�жϲ�����ASCII��Ϊ x00 - xff
	return this.length + (cArr == null ? 0 : cArr.length);
}
String.prototype.isContainChinese = function() {
    var cArr = this.match(/[^\x00-\xff]/ig); //�жϲ�����ASCII��Ϊ x00 - xff
    return cArr == null ? false: true;
}

var str = "";

/****************************
���б���ɾ������
���õݹ��㷨��
isSelectΪ��ʱɾ����ѡ�е���Ŀ��
��֮ɾ���б�ȫ������
*****************************/
function delIPFromList(ctlList, isSelect) {
    if (ctlList == null) {
        return;
    }

    for (var i = 0; i < ctlList.options.length; i++) {
        if (isSelect == true) {
            if (ctlList.options[i].selected) {
                ctlList.options[i] = null;
                delIPFromList(ctlList, true); //ɾ��LISTĳһԪ�غ�LIST��Ԫ����Ӧ����������������
            }
        } else {
            ctlList.options[i] = null;
            delIPFromList(ctlList, false);
        }
    }
}

/****************************
���ܲ��պ��� delIPFromList
*****************************/
function delSelectList(ctlList, isSelect) {
    if (ctlList == null) {
        return;
    }

    for (var i = 0; i < ctlList.options.length; i++) {
        if (isSelect == true) {
            if (ctlList.options[i].selected) {
                ctlList.remove(i);
                delSelectList(ctlList, true); //ɾ��LISTĳһԪ�غ�LIST��Ԫ����Ӧ����������������
            }
        } else {
            ctlList.remove(i);
            delSelectList(ctlList, false);
        }
    }
}

/*******************************
��txtArray��valueArray���
select�ؼ�ctl��isAppend����
ָ���Ƿ�׷�����ݣ�defTxt����
ָ����Ĭ��ѡ�е�ֵ
********************************/
function addDataToSelect(ctl, txtArray, valueArray, isAppend, defTxt) {
    var txtLen = txtArray.length;
    var valueLen = valueArray.length;
    var ctlLen = ctl.options.length;

    if (!isAppend) {
        delSelectList(ctl, false);
    }

    if (valueArray == null) {
        valueArray = new Array();

        for (var i = 0; i < txtLen; i++) {
            valueArray[i] = "";
        }
    }

    if ((valueLen < txtLen) && valueLen != 0) {
        for (var i = valueLen; i < txtLen; i++) {
            valueArray[i] = "";
        }
    }

    ctl.selectedIndex = -1;

    for (var i = 0; i < txtLen; i++) {
        var option = document.createElement("OPTION");
        option.text = txtArray[i];
        option.value = valueArray[i];
        ctl.add(option);

        if (defTxt == option.text && ctl.selectedIndex == -1) {
            ctl.selectedIndex = i;
        }
    }

    if (isAppend) {
        ctl.selectedIndex += ctlLen;
    }
}
function addDataToSelectAndDelete(ctl, txtArray, valueArray, deleteArray, isAppend, defTxt) {
    var txtLen = txtArray.length;
    var valueLen = valueArray.length;
    var deleteLen = deleteArray.length;
    var ctlLen = ctl.options.length;

    if (!isAppend) {
        delSelectList(ctl, false);
    }

    if (valueArray == null) {
        valueArray = new Array();

        for (var i = 0; i < txtLen; i++) {
            valueArray[i] = "";
        }
    }

    if ((valueLen < txtLen) && valueLen != 0) {
        for (var i = valueLen; i < txtLen; i++) {
            valueArray[i] = "";
        }
    }

    ctl.selectedIndex = -1;

    var flag = 1;
    for (var i = 0; i < txtLen; i++) {
        flag = 1; 
        for(var j = 0;j < deleteLen; j++) {
            if(valueArray[i] == deleteArray[j]) {
                flag = 0;
                break;
            }  
        }
        if(flag == 0)
            continue;
        var option = document.createElement("OPTION");
        option.text = txtArray[i];
        option.value = valueArray[i];
        ctl.add(option);

        if (defTxt == option.text && ctl.selectedIndex == -1) {
            ctl.selectedIndex = i;
        }
    }

    if (isAppend) {
        ctl.selectedIndex += ctlLen;
    }
}
/*****************************
�����ַ����õ�select�ؼ�������
******************************/
function getSelIndexByText(ctl, txt) {
    if (ctl == null) {
        return - 1;
    }

    txt = txt.toString();

    for (var i = 0; i < ctl.options.length; i++) {
        if (ctl.options[i].text.toLowerCase() == txt.toLowerCase()) {
            return i;
        }
    }

    return - 1;
}

/*****************************
�����ַ����õ�select�ؼ�������
******************************/
function getSelIndexByValue(ctl, val) {
    if (ctl == null) {
        return - 1;
    }

    val = val.toString();

    for (var i = 0; i < ctl.options.length; i++) {
        if (ctl.options[i].value.toLowerCase() == val.toLowerCase()) {
            return i;
        }
    }

    return - 1;
}
/*****************************
�õ�select�ؼ���ѡ����ı�
******************************/
function getListText(ctl) {
    var txt = null;

    if (ctl == null) return txt;

    for (var i = 0; i < ctl.options.length; i++) {
        if (ctl.options[i].selected) {
            txt = ctl.options[i].text;
            return txt;
        }
    }

    return txt;
}

/*****************************
�õ�select�ؼ���ѡ���ֵ
******************************/
function getListValue(ctl) {
    var val = null;

    if (ctl == null) return val;

    for (var i = 0; i < ctl.options.length; i++) {
        if (ctl.options[i].selected) {
            val = ctl.options[i].value;
            return val;
        }
    }

    return val;
}
/*****************************
�õ�select�ؼ���ѡ���ֵ
******************************/
function getListTxt(ctl) {
    var val = null;

    if (ctl == null) return val;

    for (var i = 0; i < ctl.options.length; i++) {
        if (ctl.options[i].selected) {
            val = ctl.options[i].text;
            return val;
        }
    }

    return val;
}

/*****************************
�õ�select�ؼ���ǰѡ�������
******************************/
function getListIndex(ctl) {
    var index = -1;

    if (ctl == null) return index;

    for (var i = 0; i < ctl.options.length; i++) {
        if (ctl.options[i].selected) {
            index = i;
            return index;
        }
    }

    return index;
}

/********************
�ж�str�Ƿ�Ϊ����
********************/
function isNum(str) {
	str=str.toString();
    if (str.length == 0 || str == null) return false;

    for (i = 0; i < str.length; i++) {
        if (48 > str.charCodeAt(i) || 57 < str.charCodeAt(i)) {
            return false;
        }
    }

    return true;
}

/********************
���˿��Ƿ���Ϲ淶
*********************/
function checkPort(port) {
    if (!isNum(port)) {
        return false;
    }

    if (port >= 0 && port <= 65535) {
        return true;
    } else {
        return false;
    }
}

function retrenchArray(array, aspect) {
    var dstArray = new Array();
    var index = 0;
    var bFlag = false;

    if (aspect) {
        for (var i = 0; i < array.length; i++) {
            for (var j = 0; j < dstArray.length; j++) {
                if (dstArray[j] == array[i]) {
                    bFlag = true;
                    break;
                }
            }

            if (!bFlag) {
                dstArray[index++] = array[i];
            }

            bFlag = false;
        }
    } else {
        for (var i = 0; i < array.length; i++) {
            for (var j = i + 1; j < array.length; j++) {
                if (array[i] == array[j]) {
                    bFlag = true;
                    break;
                }
            }

            if (!bFlag) {
                dstArray[index++] = array[i];
            }

            bFlag = false;
        }
    }

    return dstArray;
}

function searchArrayRepeatData(array, data) {
    var dstArray = new Array();
    var index = 0;

    for (var i = 0; i < array.length; i++) {
        if (array[i] == data) {
            index++;
        }
    }
    return index > 1 ? true: false;
}

/********************
10����ת��16����
*********************/
function Dec2Hex(Decimal) {
    try {
        Decimal = eval(Decimal);
    } catch(e) {
        return Decimal;
    }

    var hexChars = "0123456789ABCDEF";
    var a = Decimal % 16;
    var b = (Decimal - a) / 16;

    hex = "" + hexChars.charAt(b) + hexChars.charAt(a);
    L = hexChars.charAt(a);
    H = hexChars.charAt(b);
    return hex;
}

function dynamicShowList(index,ctrl){
	currpage[index]=1;
	recsPerPage[index]=parseInt(ctrl.value);
	if(recsPerPage[index]==10 ){
		height=65+21*recsPerPage[index]+"px";	
	}else if(recsPerPage[index]==20){
		height=85+21*recsPerPage[index]+"px";	
	}else if(recsPerPage[index]==30){
		height=105+21*recsPerPage[index]+"px";
	}else if(recsPerPage[index]==50){
		height=145+21*recsPerPage[index]+"px"
	}else{
		height=250+21*recsPerPage[index]+"px";	
	}
	var frame = document.getElementById("framelist");
	frame.style.height=height;
	switch(index){
		case 0:
		frame12.parent.dsplisthead(0,frame11);
		frame12.parent.dsplist(0,frame12);
		break;	
	}
}
/**************************************************end by jyshen*************************************************/
//��ʾ�б�ͷ

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
    strRecs = recscount[index].toString() + "/" + maxrecs[index].toString();

    out_head(listhead);
    listhead.document.write('<table width="100%" height="20"	 border="0" cellpadding="0" cellspacing="0">');

    listhead.document.write('  <tr>');
    listhead.document.write('    <td><table width="100%" height="20" border="0" cellspacing="0" cellpadding="0" bgcolor="">');
    listhead.document.write('      <tr>');
    //listhead.document.write('	 <td width="11" ></td>');
	if(listheadcaption[index]!=""){
    listhead.document.write('	 <td width="226" background="../<% write(com_imgtablehead); %>" style="background-repeat:no-repeat"><img src="../<% write(com_imgspace); %>" width="10" height="8"><span class="style4">' + listheadcaption[index] + '</span></td>');
    }
	//listhead.document.write('	 <td width="26"></td>');
    listhead.document.write('	 <td   width="324"align="right"  class="style5" ><img src="../<% write(com_imgspace); %>" width="10" height="8"></td><td width="50" align="right"  >' + strRecs + '</td>');
    listhead.document.write('      </tr>');
    listhead.document.write('    </table></td>');
    listhead.document.write('  </tr>');

    listhead.document.write('  <tr class="style18">');
    listhead.document.write('    <td height="24" colspan="4"  ><table width="600"  border="0" cellspacing="0" cellpadding="0">');
    listhead.document.write('      <tr>');
    listhead.document.write('	 <td width="10"><img src="../<% write(com_imgspace); %>" width="10" height="8"></td>');
    listhead.document.write('	 <td width="25" valign="bottom" class="style6">' + strPages + '</td>');

if(lineButton[index]==0){
	var lineContent='&nbsp;';
}else if(lineButton[index]==1020){
	if(index==0)
		var lineContent='<%write(ShowLine);%>&nbsp;<select id="frame'+(index+1)+'2List" name="frame'+(index+1)+'2List" onchange="parent.dynamicShowList(0,frame'+(index+1)+'2List)" style="width:50px;height="12px;"><option value="10" selected="selected">10</option><option value="20">20</option><option value="30">30</option></select>';
	else if(index==1)
		
		var lineContent='<%write(ShowLine);%>&nbsp;<select id="frame'+(index+1)+'2List" name="frame'+(index+1)+'2List" onchange="parent.dynamicShowList(1,frame'+(index+1)+'2List)" style="width:50px;height="12px;"><option value="10" selected="selected">10</option><option value="20">20</option><option value="30">30</option></select>';
	
}else if(lineButton[index]==10){
	if(index==0)
		var lineContent='<%write(ShowLine);%>&nbsp;<select id="frame'+(index+1)+'2List" name="frame'+(index+1)+'2List" onchange="parent.dynamicShowList(0,frame'+(index+1)+'2List)" style="width:50px;height="12px;"><option value="10" selected="selected">10</option><option value="30">30</option><option value="50">50</option></select>';
	else if(index==1)
		var lineContent='<%write(ShowLine);%>&nbsp;<select id="frame'+(index+1)+'2List" name="frame'+(index+1)+'2List" onchange="parent.dynamicShowList(1,frame'+(index+1)+'2List)" style="width:50px;height="12px;"><option value="10" selected="selected">10</option><option value="30">30</option><option value="50">50</option></select>';
}else if(lineButton[index]==20){
	if(index==0)
		var lineContent='<%write(ShowLine);%>&nbsp;<select id="frame'+(index+1)+'2List" name="frame'+(index+1)+'2List" onchange="parent.dynamicShowList(0,frame'+(index+1)+'2List)" style="width:50px;height="12px;"><option value="20" selected="selected">20</option><option value="50">50</option><option value="100">100</option></select>';
	else if(index==1)
		var lineContent='<select id="frame'+(index+1)+'2List" name="frame'+(index+1)+'2List" onchange="parent.dynamicShowList(1,frame'+(index+1)+'2List)" style="width:50px;height="12px;"><option value="20" selected="selected">20</option><option value="50">50</option><option value="100">100</option></select>';
}else if(lineButton[index]==1050){
	if(index==0)
		var lineContent='<%write(ShowLine);%>&nbsp;<select id="frame'+(index+1)+'2List" name="frame'+(index+1)+'2List" onchange="parent.dynamicShowList(0,frame'+(index+1)+'2List)" style="width:50px;height="12px;"><option value="10" selected="selected">10</option><option value="50">50</option><option value="100">100</option></select>';
	else if(index==1)
		var lineContent='<select id="frame'+(index+1)+'2List" name="frame'+(index+1)+'2List" onchange="parent.dynamicShowList(1,frame'+(index+1)+'2List)" style="width:50px;height="12px;"><option value="10" selected="selected">10</option><option value="50">50</option><option value="100">100</option></select>';
}else{
	var lineContent='&nbsp;';
}
	if(lineButton[index]!=0){
	listhead.document.write('<td class="style6" valign="bottom" width="140">'+lineContent+'</td>');}
    if (currpage[index] == 1) listhead.document.write('	 <td width="40" valign="bottom" class="ablacks"><% write(com_firstPage); %></td>');
    else listhead.document.write('	 <td width="40" valign="bottom"><a href="javascript:parent.firstpage(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')" class="ablacks"><% write(com_firstPage); %></a></td>');
    if (currpage[index] <= 1) listhead.document.write('	 <td width="40" valign="bottom" class="ablacks"><% write(com_prePage); %></td>');
    else listhead.document.write('	 <td width="40" valign="bottom"><a href="javascript:parent.prepage(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')" class="ablacks"><% write(com_prePage); %></a></td>');

    if (currpage[index] * recsPerPage[index] < recscount[index]) listhead.document.write('	    <td width="40" valign="bottom"><a href="javascript:parent.nextpage(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')" class="ablacks"><% write(com_nextPage); %></a></td>');
    else listhead.document.write('	 <td width="40" valign="bottom" class="ablacks"><% write(com_nextPage); %></td>');
    if ((recscount[index] < 1) || (currpage[index] == Math.floor((recscount[index] - 1) / recsPerPage[index]) + 1)) //?
    listhead.document.write('	 <td width="40" valign="bottom" class="ablacks"><% write(com_lastPage); %></td>');
    else listhead.document.write('	 <td width="40" valign="bottom"><a href="javascript:parent.lastpage(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')"  class="ablacks"><% write(com_lastPage); %></a></td>');
    listhead.document.write('	 <td width="30" valign="bottom"><a href="javascript:parent.goanotherpage(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')" class="ablacks"><% write(com_goToPage); %></a></td>');
    listhead.document.write('	 <td width="15" valign="bottom"><span class="style6"><% write(com_at); %></span></td>');
    listhead.document.write('	 <td width="10" valign="bottom" class="style6"><input name="pagenumber" type="text" class="input" onKeyPress="javascript:parent.gotopage(event,' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')"  size="1" maxlength="3"></td>');
    listhead.document.write('	 <td width="10" valign="bottom" class="style6">&nbsp;&nbsp;<% write(com_page); %></td>');
    listhead.document.write('	 <td width="25" valign="bottom" class="style6">&nbsp;</td>');

    listhead.document.write('	 <td width="35" valign="bottom"><span class="style6"><% write(com_search); %></span></td>');
    listhead.document.write('	 <td width="55 " valign="bottom" class="style6"><input name="txtSearch" type="text" class="input" onKeyPress="javascript:parent.searchDataEx(event,' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ',parent.' + arrayName + ',this.value,' + arrayIndex + ')"	size="1" maxlength="60"  style="width:100px"></td>');
    listhead.document.write('      </tr>');
    listhead.document.write('    </table></td>');
    listhead.document.write('  </tr>');
    listhead.document.write('</table>');

    out_tail(listhead);
    if (allcheckobj[index] != null) //�����е�checkboxȡ��ѡ��
    allcheckobj[index].checked = false;
	
		if(lineButton[index]!=0){
		if(index==0){
			frame11.document.getElementById("frame12List").value=recsPerPage[index];
		}
	}
}

function dsplistTail(index, listhead, arrayName) {

}

function shortShow(data, index)
{
    var retStr = data;
    var datatmp = data;
    var i = 0;
    var pos = 0;
    if ((typeof(data) != 'undefined') && (maxDspLen[0][index] > 0) && (data.length > maxDspLen[0][index]))
    {
	for(i=80;i<datatmp.length;i+=80)
	{
	    pos=datatmp.substring(i, i+80).indexOf(")");
	    datatmp=datatmp.substring(0,i+pos+1) + "&#13" + datatmp.substring(i+pos+1);
	    i = i+pos+1;
	}
	retStr = '<span title="'+ datatmp + '">' + data.slice(0,maxDspLen[0][index]) + "  <B>...</B>" +'</span>';
    }
    return retStr;
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
    list.document.write('<table width="' + tableWidth + '" height="21"	border="0" cellpadding="0" cellspacing="0" bordercolor="#CCCCCC">');
    list.document.write('  <tr>');
    list.document.write('	 <td nowrap="nowrap" class="style21"><table width="' + tableWidth + '"  border="0"  cellpadding="0" id="tableList" name="tableList" cellspacing="2" >');
    list.document.write('	<tr bgcolor="ffffff">');
    if (chkbox) list.document.write('<td nowrap="nowrap" width="21" height="20" class="tab_title">&nbsp;</td>');
    for (i = 0; i < titleCount; i++) {
        if (sortable[index][i] == 1) {
            list.document.write('<td nowrap="nowrap" height="21" align="center" class="style5 tab_title"><a href="javascript:parent.sortData(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ',' + i + ',parent.arrayData' + (index + 1) + ')" class="ablack tab_title">' + arrayTitle[i] + '</a></td>');
        } else {
            list.document.write('<td nowrap="nowrap" height="21" align="center" class="style5 tab_title">' + arrayTitle[i] + '</td>');
        }
    }
    if (editable) list.document.write('<td align="center" class="style5 tab_title"> <%write(com_edit); %></td>');//��һ�� �༭

	if (_strSecondMenuFocus == "<% write(PDB); %>") {//���Կ�ҳ��
		list.document.write('<td align="center" class="style5 tab_title"> <%write(com_updateStra); %></td>');
	}

    list.document.write('	</tr>');//��һ�н���
    recbegin = (currpage[index] - 1) * recsPerPage[index];

    recend = recbegin + recsPerPage[index];

    for (i = recbegin; i < recend; i++) { //i = recbegin ������ = recbegin + 1 ��ΪFILLDATA�����еĲ�����arrarData������ ��0��ʼ��
        if (i % 2 == 0) {
            list.document.write('	<tr style="background-color:f0f0f0">'); //���иı���ɫ
        } else {
            list.document.write('<tr  class="style18 c7">');
        }

        if (i < recscount[index]) {

            if (chkbox) {
                list.document.write('<td nowrap="nowrap" height="21"><input type="checkbox" class="input-checkbox" name="chklist" value="on"></td>');
            }
            if (editable) //add by cxhu 090825
            list.document.write('<td nowrap="nowrap" height="21" align="center" class="style6"><a href="javascript:parent.EditConfig(1,' + i + ')" class="ablack">' + shortShow(arrayData[0][i], 0) + '</a></td>');
            else if (_strSecondMenuFocus == "AP����") list.document.write(' <td align="center" class="style5"><a href="javascript:parent.openManagePage(\''+ macarray[i] + '\')" class="ablack"><img src="../images/setting.png" title="����" border="0"></a>&nbsp;<a href="javascript:parent.positionfun(\'' + macarray[i] + '\')" class="ablacks"><img src="../images/position.png" title="��λ" border="0"></a>&nbsp;<a href="javascript:parent.apReboot(\''+ macarray[i] + '\')" class="ablacks"><img src="../images/reboot.png" title="����" border="0"></a></td>');
	    else list.document.write('<td nowrap="nowrap" height="21" align="center" class="style6">' + (shortShow(arrayData[0][i], 0) || "&nbsp;") + '</td>');
            for (j = 1; j < titleCount; j++) {
                if ((j == 2 && _strSecondMenuFocus == "��������״̬") || (j == 3 && _strSecondMenuFocus == "<% write(com_bandList); %>")||(j == 1 && _strSecondMenuFocus == "<% write(com_pppAcc);%>") || (j == 4 && _strSecondMenuFocus == "<% write(com_netAction); %>") || (j == 3 &&_strSecondMenuFocus == "��������") || (j == 1 && _strSecondMenuFocus == "<%write(com_ipsecInfo);%>") || (j == 1 && _strSecondMenuFocus == "<%write(com_control);%>") || (j == 1 && _strSecondMenuFocus == "<%write(com_policyroute);%>") || (j == 2 && _strSecondMenuFocus == "PPTP") || (j == 1 && _strSecondMenuFocus == "<% write(com_parentscontrol); %>")) {
                    strTmp = "";
                    if (arrayData[j][i].toLowerCase() == "yes")�� { //��������
                        strTmp = "checked";
                    }
                    list.document.write('<td nowrap="nowrap" align="center" height="21"><input type="checkbox" name="chkAllow" ' + strTmp + ' onClick= "javascript:parent.SaveUserAllow(' + index + ',' + i.toString() + ')" value=' + arrayData[j][i] + '></td>');
                } else list.document.write('<td nowrap="nowrap" height="21" align="center" class="style6">' + shortShow(arrayData[j][i], j) + '</td>');
            }
            if (_strSecondMenuFocus == "<% write(PDB); %>") list.document.write('		    <td align="center" class="style5"><a href="javascript:parent.updatePolicy(' + index + ',' + i.toString() + ')"><font color="#0000FF"><% write(Update); %></font></a></td>');
			
            if (editable) {
                if ((_strSecondMenuFocus == "<% write(com_LinkList); %>") || (_strSecondMenuFocus == "<% write(com_routeCombination); %>")) {
                    list.document.write(' <td align="center" class="style5"><a href="javascript:parent.EditConfig(1,' + i + ')" class="ablack"><img src="<% write(com_imgedit); %>" title="<% write(Modify); %>" border="0"></a></td>');
                }else if (_strSecondMenuFocus == "���ߺ�����") list.document.write(' <td align="center" class="style5"><a href="javascript:parent.DeleteConfig(' + i + ')" class="ablacks"><img src="<% write(com_imgdelete); %>" title="<% write(JSDel); %>" border="0"></a></td>'); 
		else if (_strSecondMenuFocus == "<% write(com_webAuth); %>"){
		    list.document.write(' <td align="center" class="style5"><a href="javascript:parent.HangUpConfig(1,' + i + ')" class="ablack"><% write(HangUp); %></a>&nbsp;&nbsp;&nbsp;&nbsp;<a href="javascript:parent.EditConfig(1,' + i + ')" class="ablack"><img src="<% write(com_imgedit); %>" title="<% write(Modify); %>" border="0"></a>&nbsp;&nbsp;&nbsp;&nbsp;<a href="javascript:parent.DeleteConfig(' + i + ')" class="ablacks"><img src="<% write(com_imgdelete); %>" title="<% write(JSDel); %>" border="0"></a></td>');
		} else if(_strSecondMenuFocus == "<% write(com_webAuthStatus); %>"){
		    list.document.write(' <td align="center" class="style5"><a href="javascript:parent.HangUpConfig(1,' + i + ')" class="ablack"><% write(HangUp); %></a>&nbsp;&nbsp;&nbsp;&nbsp;</td>');
		} else list.document.write(' <td align="center" class="style5"><a href="javascript:parent.EditConfig(1,' + i + ')" class="ablack"><img src="<% write(com_imgedit); %>" title="<% write(Modify); %>" border="0"></a>&nbsp;&nbsp;&nbsp;&nbsp;<a href="javascript:parent.DeleteConfig(' + i + ')" class="ablacks"><img src="<% write(com_imgdelete); %>" title="<% write(JSDel); %>" border="0"></a></td>');
            }
        } else {
            if (chkbox) list.document.write('		<td nowrap="nowrap" height="21"></td>');

            for (k = 0; k < titleCount; k++) list.document.write('<td height="21" align="center" class="style6"></td>');
            if (editable) list.document.write('	       <td align="center" class="style6"></td>');
            if (_strSecondMenuFocus == "<% write(PDB); %>") list.document.write('	       <td align="center" class="style6"></td>');
        }
        list.document.write('</tr>');
    }

    list.document.write('	   </table></td>');
    list.document.write('	 </tr>');
    list.document.write('</table>');
    out_tail(list);
}

/**
listΪframe��id
tipsΪûѡ���κ���Ŀʱ����ʾ��Ϣ
flagstrΪ��ѡ��Ľ���ָ�����
**/
function genData(formName, index, list, array, tips, flagstr) //ɾ���б��е�Ԫ��
{
    var recbegin;
    var recend;
    var t = 0;
    var chks = list.document.getElementsByName("chklist");
    if (typeof(flagstr) == "undefined" || trimstr(flagstr) == '') flagstr = ',';
    formName.delstr.value = "";
    recbegin = (currpage[index] - 1) * recsPerPage[index]; //��ʾҳ��Ŀ�ʼ��¼�� ��0Ϊ��ʼ�±�
    recend = recbegin + recsPerPage[index]; //��ʾҳ��Ľ�����¼��

    if (recend >= recscount[index]) recend = recscount[index]; //��������ҳ�����ܼ�����ֵ��recend

    for (var i = recbegin; i < recend; i++) { //��Ϊrecbegin����ʾ�Ŀ�ʼ��¼��Сһ������i< ���� <= recend

        if (chks.length) { //ѭ������checkbox
            if (chks[t].checked == 1) if (chks[t].value != "on") {
                formName.delstr.value = formName.delstr.value + array[eval(chks[t].value)] + flagstr;
                //�������ز���
            } else {
                formName.delstr.value = formName.delstr.value + array[i] + flagstr;
            }
        } else {
            if (chks.checked == 1) if (chks.value != "on") {
                formName.delstr.value = formName.delstr.value + array[eval(chks.value)] + flagstr;
            } else {
                formName.delstr.value = formName.delstr.value + array[i] + flagstr;
            }
        }

        t = t + 1;
    }

    if (formName.delstr.value != "") return true; //�ж��Ƿ�ѡ��checkbox
    else {
        alert("<% write(JSAlert47); %>" + tips + "<% write(JSAlert48); %>");
        return false;
    }
}
//modify by jyshen 060316
function sortData(index, listhead, list, col, arrayData, listTail) {
    if (recscount[index] > 500) {
        alert("<% write(JSAlert49); %>");
        return;
    }

    var tmp = null;
    var tmpArray = new Array(null);
    var flag = false;

    //�Ż�IP����
    if (sortType[index][col] == TYPE_IP) {
        tmpArray[col] = arrayData[col].slice(0);

        for (var i = 0; i < arrayData[0].length; i++) {
            arrayData[col][i] = strtokIP(".", arrayData[col][i], "0");
        }
    }

    for (I = 0; I < arrayData[0].length - 1; I++) for (j = 1; j < arrayData[0].length - I; j++) {
        if ((seq == true && ((sortType[index][col] == TYPE_STR && arrayData[col][j] < arrayData[col][j - 1]) || (sortType[index][col] == TYPE_IP && trimstr(arrayData[col][j]) < trimstr(arrayData[col][j - 1])) || (sortType[index][col] == TYPE_NUM && compareNum(arrayData[col][j], arrayData[col][j - 1]) == 1) || (sortType[index][col] == TYPE_PERCENT && comparePercent(arrayData[col][j], arrayData[col][j - 1]) == 1) || (sortType[index][col] == TYPE_CHINESE && compareChinese(arrayData[col][j], arrayData[col][j - 1]) >0) || (sortType[index][col] == TYPE_DATE && compareDate(arrayData[col][j], arrayData[col][j - 1]) == 1) || (sortType[index][col] == TYPE_DATASIZE && compareDataSize(arrayData[col][j], arrayData[col][j - 1]) == 1))) ||

        (seq == false && ((sortType[index][col] == TYPE_STR && arrayData[col][j] > arrayData[col][j - 1]) || (sortType[index][col] == TYPE_IP && trimstr(arrayData[col][j]) > trimstr(arrayData[col][j - 1])) || (sortType[index][col] == TYPE_NUM && compareNum(arrayData[col][j], arrayData[col][j - 1]) == -1) || (sortType[index][col] == TYPE_PERCENT && comparePercent(arrayData[col][j], arrayData[col][j - 1]) == -1) || (sortType[index][col] == TYPE_CHINESE && compareChinese(arrayData[col][j], arrayData[col][j - 1]) < 0) || (sortType[index][col] == TYPE_DATE && compareDate(arrayData[col][j], arrayData[col][j - 1]) == -1) || (sortType[index][col] == TYPE_DATASIZE && compareDataSize(arrayData[col][j], arrayData[col][j - 1]) == -1)))) {
            if (isSearched[index]) {
                temp = dataIndexArray[j];
                dataIndexArray[j] = dataIndexArray[j - 1];
                dataIndexArray[j - 1] = temp;
            }

            for (k = 0; k < arrayCount[index]; k++)
            //for (k = 0;k < arrayData.length;k++)
            {
                temp = arrayData[k][j];
                arrayData[k][j] = arrayData[k][j - 1];
                arrayData[k][j - 1] = temp;

                if (tmpArray[k] == null) {
                    continue;
                }

                temp = tmpArray[k][j];
                tmpArray[k][j] = tmpArray[k][j - 1];
                tmpArray[k][j - 1] = temp;

            }

        }
    }

    if (sortType[index][col] == TYPE_IP) {
        for (var i = 0; i < arrayData[0].length; i++) {
            arrayData[col][i] = tmpArray[col][i];
        }
    }

    currpage[index] = 1;
    dsplisthead(index, listhead);
    dsplistTail(index, listTail);
    dsplist(index, list);
    seq = !seq;
}

//sel ��select����text�ؼ�����val��value
//call method:	 selVal(testform1.TestName,testform2.info1.value);
function selVal(sel, val) { //��ʾselect��ֵ
    var k = 0;
    var len = sel.length;
    for (j = 0; j < len; j++) {
        if (sel.options[j].value == val) {
            k = j;
            break;
        }
    }
    sel.selectedIndex = k;
}

function selTxt(sel, txt) { //��ʾselect��ֵ
    var k = 0;
    var len = sel.length;
    for (j = 0; j < len; j++) {
        if (sel.options[j].text == txt) {
            k = j;
            break;
        }
    }
    sel.selectedIndex = k;
}

var isMenu = true;
var xx = "startimg";

///////////////////////
function restart() {
    if (confirm("<% write(JSAlert50); %>")) {
        window.location = '/goform/RebootDevice';
        // Do _NOT_ change it to location.href(), It won't compatible for Firefox!      
    }
}
function CreateRights() {
    document.write('<table width="100%" border="0" align="center" cellpadding="0" cellspacing="0"><tr><td height="59" align="right" valign="top" background="<% write(com_imgbuttom_bg); %>"><table width="150" height="34" border="0" cellpadding="0" cellspacing="0"><tr><td height="26" align="center"><% write(JSAlert51); %>&nbsp;&nbsp;<% write(JSAlert52); %></td></tr></table></td></tr></table>');
}
function ipCmp(ip1, ip2) {
    ip1 = trimstr(strtokIP(".", ip1, "0"));
    ip2 = trimstr(strtokIP(".", ip2, "0"));
    if (ip1 == ip2) return 0;
    else return ip1 > ip2 ? 1 : -1;

}
/**
��ά�������������򣨸�������ÿ�д���һ�������� new Array(iparr, macarr, namearr)��
sortFunΪ�����㷨
sortRowΪ������У���0��Ӧiparr��
desc�Ƿ�������
bhou
**/
function rowMajorArrSort(tmpArr, sortFun, sortRow, desc) {
    var temp;
    var asc = desc ? -1 : 1;
    sortRow = sortRow || 0;
    if (!sortFun) {
        sortFun = function(a, b) {
            if (a == b) return 0;
            else return (a > b ? 1 : -1);
        }
    }
    for (var I = 0; I < tmpArr[0].length - 1; I++) {
        for (var j = 1; j < tmpArr[0].length - I; j++) {
            if (asc * (sortFun(tmpArr[sortRow][j - 1], tmpArr[sortRow][j])) > 0) {
                for (var k = 0; k < tmpArr.length; k++) {
                    temp = tmpArr[k][j];
                    tmpArr[k][j] = tmpArr[k][j - 1];
                    tmpArr[k][j - 1] = temp;

                }
            }

        }
    }
}
/**********************
�ж϶�ά�����Ƿ�����ͬ��Ԫ��
�ú����Ĳ���Ϊ�䳤���� ��Ϊ(dataIndex,data1,data2,arrIndex,arr1,arr2)����ʽ
����dataIndexΪ����ҵ����ݵ����� arrIndex��Ϊ��������
data1,data2Ϊ����ҵ����� 
arr1,arr2���Ӧ������������ �� ֻҪ ���ϵ�������һ�������򷵻�true
��ע��:��dataIndex��ͬ�� ���������д��� data1,data2 ��Ȼ��Ϊ�����ڣ�
************************/
function isTwoDimArrHaveSameEle() {
    var arg = arguments;
    var key = arg[0];
    var dataNum = arg.length / 2;
    //������ ����Ԫ�صĸ���
    var arrLen = arg[dataNum].length;
    for (var i = 0; i < arrLen; i++) {
        if (arg[dataNum][i] == key) continue;
        for (var j = 1; j < dataNum; j++) {
            if (arg[(j + dataNum)][i] == arg[j]) return true;
        }
    }
    return false;
}
/********************************************************************
 *������	addLoadEvent
 *���ܣ�	ҳ�����ʱ�󶨶��������onload�¼���������
 *����˵��:	func--��Ҫ�󶨵ĺ�����
 *�����	��
 *����ֵ��	��
 *���ߣ�	HuXiaohui
 *������	2011-09-26
 *******************************************************************/
function addLoadEvent(func) {
    var oldonload = window.onload;
    if (typeof window.onload != 'function') {
        window.onload = func;
    } else {
        window.onload = function() {
            oldonload();
            func();
        }
    }
}
/*���ݽ������������豸оƬ���ͣ�ȷ��ҳ����ʾ���
*/
function getPageType()
{
	var type=1;/*���������޿���*/
	var cprefix = 'bw_r';
	var dev_type = cookie.get(cprefix+'devType') || "";
	switch(dev_type)
	{
		case "IP1725":type=2;break;/*IP1725���׽���*/
		case "VSC7390":type=3;break;/*VSC7390ǧ�׽���*/
		default:type=1;break;
	}
	return type;
}
/**
���Զ�λ����϶��¼������ɺ���
**/
function Drag()
{
	var obj=new Object();
	obj.moveDiv=null;//���ƶ�div��id
	obj.curDiv=null;//�����¼���div��id
	obj.x0=obj.y0=obj.x1=obj.y1=null;
	obj.moveable=false;//ע�⣬�ñ������ܸ���
	obj.start=function(evt,curDiv,moveDiv)
	{
		obj.curDiv=curDiv;
		obj.moveDiv=moveDiv;
        //����������;
        if (!window.captureEvents) { //ie
			if (evt.button != 1) return;//��������
			obj.curDiv.setCapture(); 
		}else { //ff
			if (evt.button != 0) return;//��������
			window.captureEvents(evt.MOUSEMOVE|evt.MOUSEUP); 
		} 
        //��¼���Ͳ�λ��;
        obj.x0 = evt.clientX;
        obj.y0 = evt.clientY;
        obj.x1 = parseInt(obj.moveDiv.style.left);
        obj.y1 = parseInt(obj.moveDiv.style.top);
        obj.moveable = true;
	}
	obj.drag=function(evt)
	{
		if (!obj.moveable) return;
   	 	obj.moveDiv.style.left = (obj.x1 + evt.clientX - obj.x0) + "px";
    	obj.moveDiv.style.top = (obj.y1 + evt.clientY - obj.y0) + "px";
	}
	obj.stop=function(evt) {
		if (obj.moveable) {
			if (!window.captureEvents) { //ie
				obj.curDiv.releaseCapture(); 
			}else { //ff
				window.releaseEvents(evt.MOUSEMOVE|evt.MOUSEUP); 
			} 
			obj.moveable = false;
		}
	}
	return obj;
}
/***
���ƶ���������
***/
function moveDiv(containNode,parentNode)
{
	var obj = new Object();
	obj.drag = Drag();//�϶��¼������
	obj.parentNode = parentNode || document.body;//���ڵ㣬���ÿ��ƶ����������ĸ��ڵ�֮��
	obj.containNode = containNode; //�����ڵ㡣�����ڴ˿��ƶ�����ʾ�Ľڵ�id�����ýڵ��°���post���ݣ���parentNode��Ϊһform��id��
	obj.x=obj.y=obj.w=obj.h=0;//���ƶ����x,y����;�Լ��ߺͿ�w,h
	/**
	��ʼ����tΪ���⡣wΪĬ�Ͽ�hΪĬ�ϸ�
	**/
	obj.init = function(t,w,h,prefix)
	{
		obj.prefix = prefix || Math.random().toString().replace(/\./g,"");//ǰ׺�����ɶ�����ƶ���ʱ����ָ����ͬ��ǰ׺
		obj.divMain = null;//���ƶ�����id����������Ҫ��ʾ������
		obj.title = t || "������ҳ����Ϣ";//���ƶ�������ʾ�ò�����div��id
		if(obj.containNode)
		{
			obj.w = parseFloat(obj.containNode.style.width  || obj.containNode.width) + 20;
			obj.h = parseFloat(obj.containNode.style.height || obj.containNode.height) + 10;
		}
		obj.w = w || obj.w || 230;
		obj.h = h || obj.h || 80;

		//Ĭ�Ͼ�����ʾ
		obj.x = (document.body.offsetWidth - obj.w) / 2; 
    	obj.y = (document.body.offsetHeight - obj.h) / 2;
		
		obj.divObj = document.createElement("div");//�������ƶ���
		obj.divObj.style.cssText="display:none; position:absolute;width:"+obj.w+"px;height:"+obj.h+"px;left:"+obj.x+"px;top:"+obj.y+"px;";
		obj.parentNode.appendChild(obj.divObj);//�����ڸ��ڵ�
		obj.divObj.innerHTML = '<table  width="100%" height="100%" cellpadding="2"  cellspacing="0"  style="position:relative; border: #888 1px solid;background-color:#ffffff;" ><tr class="style21" height="22" id="'+obj.prefix+'divHead"><td width="50%" height="22"><font color="#FFFFFF"><b><span id="'+obj.prefix+'divTitle">'+obj.title+'</span></b></font></td><td width="50%" align="right"><img src="<% write(com_imgclose); %>" alt="<% write(JSAlert53); %>" title="<% write(JSAlert53); %>"  style="margin-right:1px;cursor:hand;height:15" id="'+obj.prefix+'divClose"></td></tr><tr><td height="31" colspan="2"  align="center" id="'+obj.prefix+'divMain"></td></tr><tr height="5"><td colspan="2"></td></tr></table> ';//���ƶ���������
		
		obj.divMain=document.getElementById(obj.prefix+'divMain');
		obj.divHead = document.getElementById(obj.prefix+'divHead');
		obj.divTitle = document.getElementById(obj.prefix+'divTitle');
		
		if(obj.containNode) obj.divMain.appendChild(obj.containNode);
		
		document.getElementById(obj.prefix+'divClose').onclick=function(){obj.hide();};//���ز�
		
		//����ƶ�����
		obj.divHead.onmousemove = function (evt) {obj.drag.drag(evt||event);}
		obj.divHead.onmouseup = function (evt) {obj.drag.stop(evt||event)};
		obj.divHead.onmousedown = function(evt)
		{obj.drag.start(evt||event,document.getElementById(obj.prefix+'divHead'),obj.divObj);}
	}
	/**
	��ʾ���ƶ���
	w,h,x,y,tΪ���ƶ���� ���ߣ�x���꣬y����,����
	containNode����Ϊ�գ�����¿��ƶ������ʾ����
	**/
	obj.show = function(w,h,x,y,t,containNode)
	{
		if(w) obj.divObj.style.width= w + "px";
		if(h) obj.divObj.style.heigth=h + "px";
		if(x) obj.divObj.style.left=x+"px";
		if(y) obj.divObj.style.top=y+"px";
		if(t) {
			obj.divTitle.innerHTML=t;
		}
		if(containNode) 
		{
			if(obj.containNode) obj.divMain.removeChild(obj.containNode);//�Ƴ��ϵĽڵ�����
			obj.containNode=containNode;
			obj.divMain.appendChild(obj.containNode);
		}
		moduleDisplay(obj.divObj,1);
	}
	/**
	���ؿ��ƶ���
	**/
	obj.hide = function()
	{
		moduleDisplay(obj.divObj,0);
	}
	return obj;
}
/**
���Ƶ�ǰҳ��Ŀ�͸ߣ���������ʾ
**/
function controlWin(w,h,notMove)
{
	window.resizeTo(w,h);
	var l = (screen.width - w) / 2; 
    var t = (screen.height - h) / 2; 
	if(!notMove) window.moveTo(l,t);
}
function openWin(u, w, h) { 
            var l = (screen.width - w) / 2; 
            var t = (screen.height - h) / 2; 
            var s = 'width=' + w + ', height=' + h + ', top=' + t + ', left=' + l; 
            s += ', toolbar=no, menubar=no, scrollbars=no, resizable=no,location=no, status=no'; 
            window.open(u, '', s); 
}
/****
window.open()��ʽʵ��post�ύ
form:��ǰform
page:���ύ����ҳ��
width:�������ڵĿ��
height:�������ڵĸ߶�
newFormName:�´��ڵ�����(��ÿ�β�����ָ��ͬһ����ֻ�ܴ�һ�����ڣ�����ָ�����������һ��)
ע��:newFormName��form��Name����������ʽһ�¡����ܺ��������ַ�
*****/
function openForm(form,page,width,height,newFormName)
{
	var oldTarget=form.target;
	var oldPage=form.action;
	
	var targetName=newFormName;
	if(!targetName) targetName = Math.random().toString().replace(/\./g,"");/*��������Ψһ����򿪴��ڰ�����������*/
	form.target=targetName;
	form.action=page;
	var l = (screen.width - width) / 2;
    var t = (screen.height - height) / 2;
    var s = 'width=' + width + ', height=' + height + ', top=' + t + ', left=' + l;
    s += ', toolbar=no, menubar=no, scrollbars=yes, resizable=no,location=no, status=yes,hotkeys=yes';
    var win=window.open("", targetName, s);
	win.focus();
	form.submit();
	
	/*������ɺ����form������Ӧ�û�ԭ*/
	form.target=oldTarget;
	form.action=oldPage;
}
/*
�رյ�ǰ���ڡ�strΪ��ʾ��Ϣ
*/
function closeWin(str,notAlert)
{
	if(!notAlert) alert(str+"\n<% write(JSAlert54); %>\n<% write(JSAlert55); %>");
	window.close();
}
function isIP(ip) {
	var ips = ip.split(".");
	var len = ips.length;

	if (len != 4) {
		return false;	
	}
	
	for (var i=0;i<len;i++) {
		if (!isNum(ips[i])) {
			return false;	
		}
		
		if (eval(ips[i] > 255 || eval(ips[i]) < 0)) {
			return false;	
		}	
	}
	
	return true;
}

function print_blank(n) {
	for (var i=0;i<n;i++) {
		document.write("\&nbsp;");	
	}	
}

function setbit(_value, bitno) {
	return (1 << bitno | _value)>>>0;	
}

function testbit(value, bitno) {
	return (value >> bitno & 1); 	//jyshen	
}

function getRadioValue(objs) {
	for (var i=0;i<objs.length;i++) {
		if (objs[i].checked) {
			return (objs[i].value);	
		}	
	}
	
	return null;	
}

String.prototype.GetValue= function(para) {  
  var reg = new RegExp("(^|&)"+ para +"=([^&]*)(&|$)");  
  var r = this.substr(this.indexOf("\?")+1).match(reg);  
  if (r!=null) return unescape(r[2]); return null;  
}  
/*******
�ж�ip1/mask��ip2/mask�Ƿ���ͬһ������
ע��maskΪ������ʽ��24 �� �� ������������Ϊ�Ϸ�����
********/
function isNetIdSame(ip1,ip2,mask)
{
	var ipArr1=ip1.split('.');
	var ipArr2=ip2.split('.');
	var num1,num2;
	var pre = Math.floor(mask / 8);
	//ip�е�ÿ����֮����8λ�Ķ�������
	var rear = mask % 8;//�޷�ֱ�ӱȽϵĶ�����λ��
	var curMask=255;
	var i=0;
	for(i =0 ;i<pre;i++)
	{
		if(parseInt(ipArr1[i]) != parseInt(ipArr2[i]) )
		{
			
			return false;
		}
	}
	if(rear==0) return true;//ȫ����ֱ�ӱȽ�����ͬ
	num1=ipArr1[i];
	num2=ipArr2[i];
	switch (rear)
	{
		case 8: curMask=0xFF;break;
		case 7: curMask=0xFE;break;
		case 6: curMask=0xFC;break;
		case 5: curMask=0xF8;break;
		case 4: curMask=0xF0;break;
		case 3: curMask=0xE0;break;
		case 2: curMask=0xC0;break;
		case 1: curMask=0x80;break;
		
	}
	if( (curMask & num1) != (curMask & num2) ) return false;
	return true;
}
function inet_addr(ip) {
	var ips = ip.split(".");
	var vallow = 0, valhigh = 0, val = 0;
	
	if (ips.length != 4) {
		return -1;
	}
	
	vallow = eval(ips[3]) << 8 | eval(ips[2]);
	valhigh = eval(ips[1]) << 8 | eval(ips[0]);
	val = vallow << 16 | valhigh;
	return val;
}


function leapyear(year)
{
	if(eval(year) % 4 == 0 && eval(year) % 100 != 0) {
		return true;
	} else {
		return false;
	}
}
/****************************
* ����VALUE����SELECT��ѡ��
*
*
****************************/
function setSelectByVal(ctlList, str) {
	ctlList.value = str;
}

function setSelectByTxt(ctlList, str) {
	for (var i=0;i<ctlList.options.length;i++) {
		if (ctlList.options[i].text == str) {
			ctlList.options[i].selected = true;
		}
	}
}
/***************************
ajax�������ɺ���
***************************/
function ajax(){
	var aj=new Object();
    aj.url = null;//Զ��ҳ��
	aj.erroInfo="Զ���������ɴ�";//����ʱ����ʾ��Ϣ
	aj.isAutoRefresh=1;//�Ƿ��Զ�ˢ��(�Զ��Ӻ�̨��ȡ����)
	aj.refreshTime=2;//ˢ��ʱ�������룩
	aj.ptr=null;//�Զ�ˢ�� ����id
	aj.method='get';//ax���̨�������ݵķ�ʽpost or get
	aj.xmlHttp=null;//xmlHttp��
	aj.sendString=null;//POST��ʽʱPOST������
	aj.start=function() {
		var fun=(aj.method=='get'?aj.get:aj.post);
		fun();
        if(aj.isAutoRefresh) aj.ptr=setInterval(fun,aj.refreshTime*1000);
	}	
	aj.Stop=function()
	{
		clearInterval(aj.ptr);
	}
	/******************************
	get��ʽ
	*******************************/
    aj.get=function(newUrl) {
	if(typeof(newUrl)!="string") newUrl = "";/*ȥ���Ƿ�������������һ������Ϊ�¼�����*/
        aj.xmlHttp.open('GET',newUrl || aj.url, true);
		aj.xmlHttp.onreadystatechange =  aj.xmlHttpStateChange  ;/*ie 7/6�汾ajaxÿ��ִ�ж�������ָ��onreadystatechange*/
        aj.xmlHttp.setRequestHeader("Content-type", "text/xml; charset=utf-8");
        aj.xmlHttp.send();
    }
	/******************************
	post��ʽ
	*******************************/
	aj.post = function(newUrl, sendString) {
	    /*ȥ���Ƿ�������������һ������Ϊ�¼�����*/
	    if(typeof(newUrl)!="string") newUrl = "";
	    if(typeof(sendString)!="string") sendString = "";
	    aj.xmlHttp.open('POST', newUrl || aj.url,true);
		aj.xmlHttp.onreadystatechange =  aj.xmlHttpStateChange ;/*ie 7/6�汾ajaxÿ��ִ�ж�������ָ��onreadystatechange*/
	    aj.xmlHttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
	    aj.xmlHttp.send(sendString||aj.sendString);
	}
	/********************************************
	��������:������Զ���������ɴ�
	********************************************/
   	aj.erroProcessHandle= function() {
       // alert(aj.erroInfo);
		return;
    }
	/*******************************************
	ajax�����ı�������
	********************************************/
    aj.processHandle=function(str) {
    }
	/******************************
	xmlHttp��ʼ��
	*******************************/
	aj.creatXmlHttp=function()
	{
        if (window.XMLHttpRequest) { // For Mozilla, Safari, ...
            aj.xmlHttp = new XMLHttpRequest();
            if (aj.xmlHttp.overrideMimeType) { //����MiME���
                aj.xmlHttp.overrideMimeType("text/xml");
            }
        } else if (window.ActiveXObject) { // IE�����
            try {
                aj.xmlHttp = new ActiveXObject("Msxml2.XMLHTTP");
            } catch(e) {
                try {
                    aj.xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");
                } catch(e) {}
            }
        }
        if (!aj.xmlHttp) { // �쳣����������ʵ��ʧ��
            alert("<% write(JSAlert56); %>");
            return false;
        }
		/*ie 7/6�汾ajaxÿ��ִ�ж�������ָ��onreadystatechange
		�ʲ����ڴ�ֱ��ָ��onreadystatechange����
		*/
		aj.xmlHttpStateChange = function() {
            //if (aj.xmlHttp.overrideMimeType) { //����MiME���
              //  aj.xmlHttp.overrideMimeType("text/plain");
           // }
            if (aj.xmlHttp.readyState == 4) {
                if (aj.xmlHttp.status == 200) {
                    aj.processHandle(aj.xmlHttp.responseText);
                } else {
                    aj.erroProcessHandle();
                }
            }
        }
	}
	aj.creatXmlHttp();
	if(!aj.xmlHttp) return false;	
	return aj;
}

function moduleDisplay(module, isDis)
{
	var obj = typeof(module) == 'object'?module:document.getElementById(module);
	if(obj == null) return;
	var disMode = "block";
	var tagNm = obj.tagName.toUpperCase();
	if (document.all) disMode = "block";
	else if(tagNm == 'TABLE')
	{
		disMode = "table";
	}
	else if(tagNm =='TR')
	{
		disMode = "table-row";
	}
	else if(tagNm =='TD')
	{
		disMode = "table-cell";
	}
	
	obj.style.display= isDis?disMode:"none";
    
}


/* �Ƿ�3G�ӿ� */
function is3GPort(index,wanCount) {
	if (((index == 2) && (platform == "nv510W"))||  ((index == 3) && (platform == "nv515W"))||((index==wanCount-1) && ((platform == "nv518W-Plus")||(platform == "nv518W-Lite")))) {
		return true;
	} else {
		return false;
	}
}

/* �Ƿ�APCli�ӿ� */
function isAPCliPort(index, wanCount) {
	if (typeof(remove_apclient) != 'undefined' && remove_apclient == 1) {
		return;
	}
    if (((index == 3) && (platform == "nv510W")) || ((index == wanCount) && (platform == "nv520W"))||((index == wanCount) && (platform == "U1000N"))|| ((index == wanCount) && (platform == "nv518W")) || ((index == wanCount) && (platform == "nv518W-VPN")) || ((index == wanCount) && (platform == "nv512W"))|| ((index == wanCount) && (platform == "nv842W"))|| ((index == wanCount) && (platform == "nv518W-Plus"))|| ((index == wanCount) && (platform == "nv518W-Lite")) || ((index == wanCount) && (platform == "nvA750W")) || ((index == wanCount) && (platform == "nvA650W"))||((index == wanCount)&&(platform == "nvA755W")) || ((index == wanCount) && (platform == "nv750W"))||(index == wanCount && (platform == "nv521Gv2")) ||(index == wanCount && (platform == "nv750GW")) || ((index == wanCount) && (platform == "nv520Wv3"))|| ((index == wanCount) && (platform == "nv842Wv3")) || ((index == wanCount) && (platform == "nvA650W_INT"))|| (index == wanCount && (platform == "u1000Nv2")))   {
		return true;
	} else {
		return false;
	}
}

/* �Ƿ�֧�����߹��� */
function isWirelessSupport() {
    var rst;
    switch(platform) {
	case "nv510W":
	    rst = true;
	    break;
	case "nv520W":
	    rst = true;
	    break;
    case "nv512W":
        rst = true;
        break;
    case "nv515W":
        rst = true;
        break;
    case "nv518W":
        rst = true;
        break;
    case "nv518W-VPN":
        rst = true;
        break;
    case "nv842W":
        rst = true; 
        break;
	case "U1000N":
	    rst = true;
	    break;
    case "nv518W-Plus":
        rst = true;
        break;
    case "nv518W-Lite":
        rst = true;
        break;
	default:
	    rst = false;
	    break;
    }
    return rst;
}

/*�������Ƿ���low��high֮��*/
function CheckDate(date,low,high)
{
    if(!((eval(date)>eval(low))&&(eval(date)<eval(high))))
    {
	return false;
    }
    return true;
}

/*�ж��·��������Ƿ�Ϊ��*/
function checkZero(val,str)
{
    if(val==0)
    {
	if((str=="0")||(str=="00")) { alert("<%write(com_dateFErr);%>"); return false;}
    }
    else
    {
	if((str=="0")||(str=="00")) { alert("<%write(com_dateTErr);%>"); return false;}
    }
    return true;
}

/*ȥ�����ڿ�ͷ��0*/
function ZeroBegin_Del(str)
{
    for (i = 0; i < str.length; i++)
    {
	str[i] = str[i].toString();
	if (str[i] == "0")
	{
	    return "0";
	}
	if (str[i] == "")
	{
	    return "";
	}
	while (str[i].length > 0 && str[i].indexOf('0') == 0) 
	{
	    str[i] = str[i].substring(1, str[i].length);
	}
    }
    return str;
}

/*�ж������·������Ƿ�Ϸ�*/
function CheckMonthDay(year, month, day)
{
    if ((eval(month) == 4 || eval(month) == 6 || eval(month) == 9 || eval(month) == 11) && eval(day) == 31)
    {
	return false;
    }
	if(eval(day) > 29 && eval(month) == 2)
	{
	return false;
    }
	if ((eval(year)%4 != 0) && eval(day) > 28 && eval(month) == 2)
    {
	return false;
    }
    return true;
}

/*��ʾ����ɹ���ʾ��Ϣ*/
function showSuccessTips(flag, content)
{
    if (flag == "saveok")
    {
	if ((typeof(content) != "string") || (content == ""))
	{
	    content ="<%write(com_succ);%>";
	}
	window.parent.art.dialog({
	    title:false,
	    width:'300px',
	    height:'50px',
	    content: "<font size='4'>" + content + "</font>",
	    id: "wating_box",
	    ok: function(){
		this.close();
		return false;
	    }
	}).lock().time(3);
    }
    return true;
}

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>交换功能-端口汇聚</title>
<link rel=stylesheet type="text/css" href="../css/style.css" title="cool" id="lnk">
<script src="../js/common.js" type="text/javascript"></script>
<script src="js/common_switch.js" type="text/javascript"></script>
<script language="javascript">
<%
var macStr = Form("macStr");//远程设备mac地址
var urcpSet = Form("urcpSet");//是否联动设置汇聚后跳转到此页面的
if (macStr!="" || urcpSet!="")
{//联动信息
	aspOutPutAggrGroupListUrcp();
}
else
{//本地信息
        if (0)
	{
	    aspOutPutGetQosRatelimitList();
	}
	else
	{//添加限速后需使用以上打印
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
//交互
/*var aggGrpNos = new Array("1","2","3");//汇聚组号
var aggLists  = new Array("2 3","4 5 6","7 8 9 10 11");//汇聚组成员
var aggGrpNames =new Array("name1", "name2", "", "name4", "name5", "name6", "name7", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "");//汇聚组名
var mirrorPort = 24;//监控端口*/
//var rstp = new Array(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);//是否开启rstp
//var lacp = new Array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1); //是否开启lacp
//var mirrorSource = new Array("checked", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "");
//监控端口源端口
/*var speedMode = new Array("1", "1", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3");//端口协商模式
var maxFrame = new Array("6000", "6000", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518", "1518");//允许最大帧
var portprotect = new Array('1', '1', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none', 'none');//端口保护
var fc = new Array(1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);//流控*/
//var floodings = new Array(1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);//泛洪
/*var vlanFlag=0;//0不设置vlan,1端口vlan,2tag vlan
var vlantabs = new Array(new Array("1","1 2 3 4 5"),new Array("2","3 4 "));
var vlanAware = new Array("", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "");//vlan设备
var frameType = new Array("1", "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0");//tag vlan帧类型(允许tag,允许所有以及disabled)
var pvid = new Array("2", "2", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1");//pvid
var vlanLists = new Array("1 2","1 2 3");//port vlan
var ingr = new Array("", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "");//遗留项
*/
//end 交互
//if(mirrorSource.countData("checked") <1) //该监控端口不起作用（无源端口）
//	mirrorPort = 0;

var portTitle=new Array();//端口提示信息
var portDisabled=new Array();//端口是否可用(端口设置页中的端口)
var isCheckAggr = 1;/*改变配置的时候是否检验本次操作相对于vlan合法*/
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
            portDisabled[i] = "disabled"; //lacp端口
            portTitle[i] = "LACP已开启,不可设置汇聚"; //lacp端口
        } else if (rstp[i]) {
            portDisabled[i] = "disabled";
            portTitle[i] = "RSTP已开启，不可设置汇聚";
        }

    }

    for (i = 0; i < aggLists.length; i++) {
        aggArr[i] = aggLists[i].split(" ");
        for (j = 0; j < aggArr[i].length; j++) {
            portTitle[(aggArr[i][j] - 1)] = "属于汇聚组" + aggGrpNos[i];
        }
    }
    if (parseInt(mirrorPort) > 0) {
        if (portDisabled[parseInt(mirrorPort) - 1] == "disabled") {
            portTitle[parseInt(mirrorPort) - 1] += "&#13;&#10;";
        } //监控端口
        else {
            portDisabled[parseInt(mirrorPort) - 1] = "disabled";
        }
        portTitle[parseInt(mirrorPort) - 1] += "监控端口，不可设置汇聚"; //监控端口
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
/*初始化错误信息：如在vlan类型切换时，导致的配置错误*/
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
        errorstr += "配置错误：汇聚组"+aggGrpNos[j]+"内的端口不完全属于任一VLAN";
        isCheckAggr = 0;
    }
	if(vlanFlag != 2) return;//非tag vlan
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
         errorstr += "配置错误：汇聚组"+aggGrpNos[error_index]+"内的端口具有不同的TAG VLAN端口设置";
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
var titles1 = new Array("汇聚组号","汇聚组名称", "汇聚组成员");
var total_recs = aggGrpNos.length;
var max_totalrecs = numPorts;
currpage[0] = 1;
recscount[0] = total_recs;
maxrecs[0] = max_totalrecs;
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] = "端口汇聚列表";
sortable[0] = new Array(1,1,1);
sortType[0]=new Array(TYPE_NUM,TYPE_CHINESE,TYPE_STR);
arrayCount[0] = 3;
recsPerPage[0] = 10; //每页多少条数据
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
根据 端口汇聚设置 选项卡中 端口汇聚组号的值(可以为空)
改变不可选端口(已开启lacp等)状态为disabled
****/
function disabledChk() {
	var exceptIndex = -1;
	 if (AGGR.actions[1].checked) //修改
    {
        exceptIndex=eval(aggGrpNos.searchData(AGGR.aggNo.value));
    }
    var array = null;
    for (var i = 0; i < numPorts; i++) {
        document.getElementById('chkPort'+i).disabled = false;
    }
    for (var i = 0; i < numPorts; i++) {
        if (portDisabled[i] == "disabled") //开启lacp
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
		//添加
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
修改时根据aggNo的值来填充数据
**/
function fillData2()
{
	var index = aggGrpNos.searchData(document.getElementById('aggNo').value);	
	var gno = document.getElementById('aggNo').value; //汇聚组号
    index = aggGrpNos.searchData(gno);
	if(index < 0) 
	{
		 for (i = 0; i < numPorts; i++) 
		{
        	document.getElementById('chkPort'+i).checked = false;
			document.getElementById('chkPort'+i).disabled = true;
    	}
		inputErro(document.getElementById('aggNo'),'非法修改:该汇聚组号不存在');
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
	if(AGGR.actions[1].checked) fillData2();//修改
	else//添加
	{
		inputRight(document.getElementById('aggNo'));//去掉修改产生的错误提示信息
		fillData(-1);
	}
}

function onAggNoChange()
{
		if(!checkNumber(document.getElementById('aggNo'), 1, numPorts, '汇聚组号'))
			return ;
		if(AGGR.actions[1].checked) fillData2() ;//修改
}
function EditConfig(val, i) {
	document.getElementById('aggNo').value='';
	document.getElementById('aggName').value='';
	inputRight(document.getElementById('aggNo'));;//初始化错误提示信息
	inputRight(document.getElementById('aggName'));;
    if (val == 0) 
	{
		fillData( - 1); //添加
	}
    else fillData(i); //修改
    switchDiv(3);

}
function DeleteConfig(index) {
    if (confirm("确定删除？")) {
        AGGR.delstr.value = aggGrpNos[index];
        AGGR.action = (macStr==""?"/goform/formAggrGroupDel":"/goform/formAggrGroupDelUrcp");
        AGGR.submit();
    }
}
function delFunc() {
    var array = null;
    if (genData(AGGR, 0, frame12, aggGrpNos, '端口汇聚', ",")) {
		 if (confirm("确定删除？")) {
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
    var gno = document.getElementById('aggNo').value; //汇聚组号
    if (!checkNumber(document.getElementById('aggNo'), 1, numPorts, '汇聚组号')) return;
    i = aggGrpNos.searchData(gno);
    if (AGGR.actions[0].checked && i >= 0) {
        inputErro(document.getElementById('aggNo'), '非法添加:该汇聚组号已经存在');
        return;
    }
    if (AGGR.actions[1].checked && i < 0) {
        inputErro(document.getElementById('aggNo'), '非法修改:该汇聚组号不存在');
        return;
    }
    inputRight(document.getElementById('aggNo'));
    if (!checkName(document.getElementById('aggName'), 11, "汇聚组名称", 1)) {
        return;
    }
    //if(gno =='') {alert("汇聚组号不能为空");return;}
    //if( parseInt(gno) <1 || parseInt(gno) >24 ) { alert("汇聚组号应为1到24之间的数字");return;}

    var num = 0;
    var tmps = new Array();

    var curPort = 0;
    for (i = 0; i < numPorts; i++) {
        if (document.getElementById('chkPort' + i).checked) {
            curPort = i + 1;
            /* if (curPort == mirrorPort) {
                alert("汇聚组内不能出现监控端口:" + mirrorPort + "!");
                return;
            } */
            tmps[j++] = curPort;
            num += 1 << i;
        }
    }
	if (tmps.length > 16) {
		alert("同一个汇聚组内最多支持16个端口");
		return;
	}
    if (num == 0) {
        alert("请至少选择一个成员!");
        return;
    }
    AGGR.chkstr.value = num; //保存的结果
    if (vlanFlag == 1 && tmps.length > 1) { //port vlan
        if (checkAggrVlan(new Array(tmps), vlanLists)>=0) {
            alert("汇聚组内端口设置不一致！！\r\n\r\n(--汇聚组端口必须完全属于某一VLAN--)");
            return;
        }
        /*	full_flag=0;//是否完全属于一个vlan
			for (y = 0; y < vlanLists.length; y++) {
				i=isArrayFullInclude(vlanLists[y],tmps);
				if (i == 1) {//完全包含
				   full_flag=1;
				   break;
				}
			}
			if(!full_flag)//该汇聚组不完全属于任何一个vlan
			{
				 alert("汇聚组内端口设置不一致！！\r\n\r\n(--汇聚组端口必须在同一VLAN中--)");
				 return;
			}*/
    }
    if (vlanFlag == 2 && tmps.length > 1) //tag vlan
    {
        if (checkAggrVlan(new Array(tmps), vTagLists)>=0) {
            alert("汇聚组内端口设置不一致！！\r\n\r\n(--汇聚组端口必须完全属于某一VLAN--)");
            return;
        }
    }
    for (j = 0; j < tmps.length; j++) {
        for (var k = j + 1; k < tmps.length; k++) {
			if (policer[tmps[j] - 1] != policer[tmps[k] - 1]) {
				alert("汇聚组内端口配置不一致！！\r\n\r\n(--汇聚组端口必须配置相同的速率限制--)");
			return;
			}
			if (shaper[tmps[j] - 1] != shaper[tmps[k] - 1]) {
				alert("汇聚组内端口配置不一致！！\r\n\r\n(--汇聚组端口必须配置相同的速率限制--)");
			return;
			}
            if (fc[tmps[j] - 1] != fc[tmps[k] - 1]) {
                alert("汇聚组内端口设置不一致！！\r\n\r\n(--汇聚组端口必须设置相同的流控--)");
                return;
            }
            if (floodings[tmps[j] - 1] != floodings[tmps[k] - 1]) {
                alert("汇聚组内端口设置不一致！！\r\n\r\n(--汇聚组端口必须设置相同的泛洪--)");
                return;
            }
            if (speedMode[tmps[j] - 1] != speedMode[tmps[k] - 1]) {
                alert("汇聚组内端口设置不一致！！\r\n\r\n(--汇聚组端口必须设置相同的端口协商模式--)");
                return;
            }
            if (maxFrame[tmps[j] - 1] != maxFrame[tmps[k] - 1]) { 
                alert("汇聚组内端口设置不一致！！\r\n\r\n(--汇聚组端口必须设置相同的祯大小--)");
                return;
            }
            if (portprotect[tmps[j] - 1] != portprotect[tmps[k] - 1]) {
                alert("汇聚组内端口设置不一致！！\r\n\r\n(--汇聚组端口必须设置相同的端口保护--)");
                return;
            }
            if (vlanFlag == 2) //tag vlan
            {
                if (vlanAware[tmps[j] - 1] != vlanAware[tmps[k] - 1]) {
                    alert("汇聚组内TAG VLAN设置不一致！！\r\n\r\n(--汇聚组端口必须设置相同的VLAN设备--)");
                    return;
                }
                if (ingr[tmps[j] - 1] != ingr[tmps[k] - 1]) {
                    alert("汇聚组内TAG VLAN设置不一致！！\r\n\r\n(--汇聚组端口必须设置相同的VLAN设置--)");
                    return;
                }
                if (frameType[tmps[j] - 1] != frameType[tmps[k] - 1]) {
                    alert("汇聚组内TAG VLAN设置不一致！！\r\n\r\n(--汇聚组端口必须设置相同的帧类型--)");
                    return;
                }
                if (pvid[tmps[j] - 1] != pvid[tmps[k] - 1]) {
                    alert("汇聚组内TAG VLAN设置不一致！！\r\n\r\n(--汇聚组端口必须设置相同的PVID--)");
                    return;
                }
            }
        }
    }
	if (vlanFlag == 2 && tmps.length > 1) //tag vlan
	{
		for (y = 0; y < vTagLists.length; y++) {
			i = isArrayFullInclude(vTagLists[y], tmps);
			if (i == 0) { //部分包含
				if (!confirm("某些TAG VLAN（如TAG VLAN "+vlanIds[y]+"）部分包含该汇聚组成员，确定继续？")) {
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
	//设置为添加模式
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
	frame11.location.href="_listhead2.htm";//注意必须在此处初始化，否则页面刷新的时候可能出现bug
	frame12.location.href="_list1.htm";//比如排序后刷新 页面会读缓存而非服务器
}
</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="../images/body_main.png" style="background-repeat:repeat-x">
<script language="javascript">urcpErrorPost("remote_manage.asp");</script>
<table width="100%" border="0" cellspacing="0" cellpadding="0" >
<!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
  <td class="table_bg"><script language="javascript">showSecondTitle('端口汇聚');//显示页面名称</script>
   <table width="80%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  >
   <script language="javascript">showerror(errorstr,100);//显示保存返回的错误信息</script>
  <tr valign="top">
    <td>
	<!--//三级菜单-->
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"100%",
						2,
						new Array('端口汇聚列表','agg_list',1,1),
						new Array('端口汇聚设置','agg_edit',1,0)
		);
		showThreeMenuNotCommit();
	</script>
  <!--//end-->
  <table width="100%" border="0" cellspacing="0" cellpadding="0" align="center"><tr>
    <td align="center"  class="three_td"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr><td><table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
	<br>
<div class="inside_div">
<form method="POST" action="" name="AGGR">


<input type="hidden" value="Apply" name="_submit">

<input type="hidden" name="chkstr"><!-- 已选中的checkbox -->
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
全选 / 全不选</td>
<td width="221"  align="right">	   
<input type="button" class="input-button" onClick="EditConfig(0)" value="添加新条目">
<input type="button" class="input-button" onClick="delFunc()" value="删除">
<input type="button" class="input-button" style="display:none" name="goBack" value="返回" onClick="go_back();"></td>
</tr>
</table>
</div>
<div id="agg_edit" style="display:none">
<TABLE   width="600" align="center" border=0>
<tr height="30">
<td  align="left" width="340">
汇聚组号<input type="text" class="input" name="aggNo" id="aggNo" onKeyPress="javascript:figureOnly()" size="4" maxlength="2" onChange="onAggNoChange();">&nbsp;
汇聚组名称<input type="text" class="input" name="aggName" id="aggName" value="" size="18" maxlength="11" onChange="checkName(this,11,'汇聚组名称',1)"></td>
<td width="160" align="left"><span id="aggteam"></span>&nbsp;</td>
<td width="100" align="right">添加<input type="radio" checked="checked" name="actions" value="add" onClick="radioClick()">&nbsp;修改<input type="radio" name="actions" value="modify" onClick="radioClick();"></td>
</tr>
<tr><td width="" height="25" valign="middle"  colspan="3">
<script>
//生成端口agg编辑列表

document.write('<table width="600" border="0" align="center" cellpadding=4 cellspacing=1 class=style22>');
document.write('<tr>');
document.write('<td align="center" width="20" class="style18" rowspan="4">');
document.write('<b>成员</b></td>');
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
<input type="button" class="input-button" value="保存" onClick="javascript:save()"> 
<input type="button" class="input-button" value="重填" onClick="radioClick();">
<input type="button" id="helpButton" name="helpButton" class="input-button" value="帮助" onClick="window.open('../rhelpinfo.htm#aggr')">
<input type="button" class="input-button" style="display:none" name="goBack" value="返回" onClick="go_back();">

</td></tr>
</table>
</div>

</form>
</div>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
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

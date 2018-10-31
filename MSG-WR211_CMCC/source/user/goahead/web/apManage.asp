<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=GBK"/>
<%langTran("apManage");%>
<%langTran("commonWords");%>
<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script src="switch/js/common_switch.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
//交互begin

<%
	aspOutPutScanMode(); 	//扫描模式
	var show=Form("show");
%>
//var jsShow = "<% write(Form("show"));%>";
//if ((scanMode == 1) || (jsShow == "yes") || (permissionChange == 1))
//{
	<%
	aspOutPutApList();
	%>
//var uploads = new Array();
//var downloads = new Array();
//uploads[0]="1.24M";
//downloads [0]="124K";
//}
//else
//{
	//var namearray = new Array();
	//var types = new Array();
	//var serialno = new Array();
	//var iparray = new Array();
	//var macarray = new Array();
	//var fitOrFat = new Array();
	//var channel = new Array();
	//var clienCt = new Array();
	//var ssid = new Array();
	//var zones = new Array();
	//var permission = new Array();
	//var managed_enable = new Array();
	//var totalrecs = 0;
	//var errorstr = "";
//}
/*
var namearray = new Array("AP1", "", "AP3", "AP4", "AP5","AP6", "AP7", "AP8", "AP9");//ap名称
var types = new Array("510W", "", "510W", "WA1800N","510W", "WA1800N", "510W", "WA1800N", "510W");//ap型号
var serialno = new Array("0018432a", "", "00bc615a", "008b2aa0", "008b2a90", "008b2a91", "008cd728", "00bc615a", "008b2aa0");//序列号
var iparray = new Array("192.200.2.82", "", "192.168.1.154", "192.168.168.254", "192.168.16.254", "192.168.16.254", "192.68.16.251", "192.168.1.154", "192.168.16.254");//IP地址
var macarray = new Array("0022aa5e8f11", "0022aa6581c2", "0022aa6581c3", "0022aa9729a4", "0022aa972815","0022aa972816", "0022aac102e7", "0022aa6581c8", "0022aa9729a9");//MAC地址
var fitOrFat = new Array("2","0","2","1","2","2","2","2","2");//胖瘦AP，0为未知，1为胖AP，2为瘦AP
var channel = new Array("11", "10", "9", "8", "7" ,"6", "5", "4", "3");//信道
var clienCt = new Array("0", "", "2", "3","4", "5", "5", "6", "7");//客户端数目
var ssid = new Array("ssid_1","TP","ssid_9","ssid_8","ssid_7","ssid_6","ssid_5","ssid_4","ssid_3");//ssid
var zones = new Array("zone_1", "", "zone_3", "zone_4","zone_5", "zone_6", "zone_7", "zone_8", "zone_9");//服务区名称
var permission = new Array("1", "0", "1", "1","1", "0", "1", "1", "1");//0为未授权，1为已授权
var managed_enable = new Array("1","0","1","0","1","1","1","0","0");//是否为可管理，1为可管理，0为不可管理
var totalrecs = 9;//当前总条目数
var errorstr = "";
*/


//交互end
var modeAndChannel = new Array();
var downAnduploads = new Array();
var manageAp = new Array();
var Types = new Array();
for (var i = 0; i < totalrecs ; i++)
{
	modeAndChannel[i] = wlMode[i] + "/" + channel[i];
	downAnduploads[i] = downloads[i] + "/" + uploads[i];
	manageAp[i] = i;
	Types[i] = types[i];
}

for(var i = 0;i<channel.length;i++){
	if(isNaN(channel[i])==true){
		if(channel[i]=="自动"){
			channel[i] = 100;
			}
		else{
			var arr = channel[i].split("(");
			var strNew = parseInt(arr[1]);
			channel[i]=100+strNew;
			}
		}
	
	}
//alert(channel)
var permission2=new Array();
for (var a=0; a < permission.length; a++)
{
	permission2[a]=permission[a];
}
var feature_manage=1;//是否开启联动页面的可管理功能
var passValues=new Array();//页面当前输入框密码的保存值
var manages =new Array();//列表中的管理
var passArr = new Array();//列表中的密码框
var indexarray = new Array();
var tmpindex= new Array();
var titles1 = new Array();
var titles1_index = 0;
titles1[titles1_index++] = "管理"
titles1[titles1_index++] = "AP<%write(name);%>";
titles1[titles1_index++] = "AP<%write(type);%>";
titles1[titles1_index++] = "<%write(serialno);%>";
titles1[titles1_index++] = "IP";
titles1[titles1_index++] = "MAC";
titles1[titles1_index++] = "VID";
titles1[titles1_index++] = "状态";
//titles1[titles1_index++] = "无线模式";
//titles1[titles1_index++] = "信道";
titles1[titles1_index++] = "客户端数";
titles1[titles1_index++] = "模式/信道(2.4GHz)";
//titles1[titles1_index++] = "下载（bit/s）";
//titles1[titles1_index++] = "上传（bit/s）";
titles1[titles1_index++] = "SSID(2.4GHz)";
titles1[titles1_index++] = "下载/上传速率(bit/s)"
//titles1[titles1_index++] = "<%write(sz);%>";
//titles1[titles1_index++] = "<%write(mfName);%>";
//titles1[10] = "<%write(power);%>";
if(feature_manage)
{
//	titles1[titles1_index++] = "<%write(passwd);%>";
//	titles1[titles1_index++] = "<%write(admin);%>";
}

currpage[0] = 1;
recscount[0] = totalrecs;
maxrecs[0] = max_totalrecs;
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] = "AP<%write(list);%>";
//sortable[0] = new Array(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0);
sortable[0] = new Array(1, 1, 1, 1, 1, 1, 1, 1, 1, 1,1,0, 1);
//sortable[0] = new Array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
						//AP名称				序列号	IP		MAC		VID		 状态		无线模式	信道	客户端数量		SSID  过滤地址组 密码  管理
//sortType[0] = new Array(TYPE_CHINESE, TYPE_STR, TYPE_NUM, TYPE_IP, TYPE_STR,TYPE_STR, TYPE_STR, TYPE_NUM, TYPE_STR, TYPE_STR ,TYPE_STR, TYPE_STR ,TYPE_STR);
sortType[0] = new Array(TYPE_STR ,TYPE_CHINESE, TYPE_STR, TYPE_NUM, TYPE_IP, TYPE_STR,TYPE_STR, TYPE_STR, TYPE_NUM, TYPE_STR, TYPE_STR,TYPE_STR, TYPE_STR);
arrayCount[0] = 13;//总共多少列(这里指的是跟着排序变化的列)
recsPerPage[0] = 10; //每页多少条数据
lineButton[0] = 1020;
managed_enable.evalAll();
var oldDspList=dspList;
var recbegin=0,recend=0;
var strSecondMenuFocus="AP管理";

/**
重写表格显示dspList函数
为其增加新的内容
**/
dspList=function()
{
	var str='';
	var obj=null;
	var j=0;
	var sum;
	if (!secondSearch)
	{
		sum = totalrecs;
	}
	else
	{
		sum = iCount2;
	}
//	alert(iCount2);
//	alert(sum);
//	alert(secondSearch);
	recbegin = (currpage[0] - 1) * recsPerPage[0]; //显示页面的开始记录数 以0为开始下标
	//原有内容
	oldDspList.apply(this,arguments);//等价于oldDspList(arugumens[0],arguments[1]...arguments[argumens.length-1])
	//用apply实现参数数组到参数列表的转变
	//注意:这里只能先用oldDspList记下dspList的地址，不能直接用dspList否则导致栈溢出（无限递归运行）
	//dspList原有脚本运行完后，运行以下的内容
	var tr=frame12.document.getElementById('tableList').rows;
	if(feature_manage) j=5;
	else j=2;
	for(var i=1;i<tr.length;i++)
	{
		//alert(tr.length);
		if((recbegin+i)>sum) break;//列表可显示记录已经结束
		str=trimstr(tr[i].cells[j].innerHTML);//ip地址所在的单元格(上面根据iparray绘表，故innerHTML只有ip)
		//alert("str:"+str);
		//alert("trimstr(tr[i].cells[13].innerHTML):"+trimstr(tr[i].cells[13].innerHTML));
		tr[i].cells[j].innerHTML='<a href="http://'+str+'" target="_blank">'+str+'</a>';
		//将改行的ip地址改为超级链接（在此更改是为了不打乱列表的排序功能）
		//alert("apStatus[i]:"+i+";value:"+apStatus[i]);
		//if(apStatus)
		//var channelStr = trimstr(tr[i].cells[8].innerHTML);
		//alert("parseInt(channelStr)"+parseInt(channelStr));
		//alert("channelStr = trimstr(tr[i].cells[8].innerHTML):"+trimstr(tr[i].cells[8].innerHTML));
		//if (parseInt(channelStr)>100) {
		//	tr[i].cells[8].innerHTML = "自动("+(parseInt(channelStr)-100)+")";
		//}
		//if (parseInt(channelStr)==100) {
		//	tr[i].cells[8].innerHTML = "自动";
		//}
	
		var aps=trimstr(tr[i].cells[8].innerHTML);
		//alert("aps=tr[i].cells[6].innerHTM:"+trimstr(tr[i].cells[6].innerHTML));
		var macStr = trimstr(tr[i].cells[6].innerHTML);	
		var status = trimstr(tr[i].cells[8].innerHTML);
		var rel_i = (i - 1) + ((currpage[0] - 1) * recsPerPage[0]);				//解决翻页后点击只有第一页有效的问题
		if ((macStr != undefined) && (status==1)) {
			tr[i].cells[6].innerHTML = "<a href='javascript:parent.openAPInfoPage(" + (rel_i) + ")'><font color='blue'>" + macStr + "</font></a>";
		}
		var clientStr = trimstr(tr[i].cells[9].innerHTML);	
		
		if (clientStr != undefined) {
			tr[i].cells[9].innerHTML = "<a href='javascript:parent.goToCltPage(" + (rel_i) + ")'><font color='blue'>" + clientStr + "</font></a>";
		}
		
	//	var objpwd=tr[i].cells[11].childNodes[0];
		//alert("aps:"+aps);
		if(aps==1){
			tr[i].cells[8].innerHTML="<span style='color:'><% write(online);%></span>";
			/*
			if(typeof(objpwd)!="undefined"){
                          objpwd.disabled=false;
			}
			*/
		}else if(aps =="0"){
			tr[i].cells[8].innerHTML="<span style='color:red'><% write(offline);%></span>"	;
			/*
			if(typeof(objpwd) != "undefined"){	
				objpwd.disabled=true;
			}
			if(typeof(tr[i].cells[12].childNodes[0]) !="undefined"){
				tr[i].cells[12].childNodes[0].href="#";
			}
			*/
			//tr[i].cells[13].childNodes[0].childNodes[0].color="#666";
		}else{
			tr[i].cellS[8].innerHTML="<span></span>";
		}
		
		if(feature_manage)
		{
		//alert(indexarray[(i+recbegin-1)]);
			//alert(managed_enable[indexarray[(i+recbegin-1)]]);
			//if(!managed_enable[indexarray[(i+recbegin-1)]])//不可管理设备最前面的checkbox状态更改
			if(!managed_enable[recbegin+i-1])//不可管理设备最前面的checkbox状态更改
			{
				obj=tr[i].cells[0].childNodes[0];
				//alert(obj);
				//if (obj == undefined)
//				if (obj == "object")
//				{
//					obj.disabled=true;
//					obj.checked=false;
//				}
			}
			else//可管理
			{
				//设置密码以保证每次排序密码不会被清空
				
				//obj=tr[i].cells[11].childNodes[0];
				//obj.value=passValues[indexarray[(i+recbegin-1)]];
				
			}
		}
	}
}
function dsplist(index, list) {
	var obj=null;
	var chkBox = 0;
	if(feature_manage) chkBox=1;
    if (index == 0) dspList(index, list, 750, titles1, titles1.length, arrayData1, chkBox, 0);

}

function onPassChg(obj,n)
{
	if(!checkPassword2(obj,11,"<%write(passwd);%>",1,1)) return;
	passValues[n]=frame12.document.getElementById('pw'+n).value;
}
//全局密码设置
function globalSetPs(obj)
{
	var psObj=null;
	for (var i = 0; i < managed_enable.length; i++) {
        if (managed_enable[i]) {
			passValues[i]=obj.value;
		}
	}
	var tr=frame12.document.getElementById('tableList').rows;
	for(var i=1;i<tr.length;i++)
	{
		if(tr[i].cells[0].innerHTML=='') continue;//该行内容为空
		if(managed_enable[indexarray[(i+recbegin-1)]])//可管理
		{
			psObj=tr[i].cells[13].childNodes[0];
//			psObj.value=obj.value;
			inputRight(psObj);
		}
	}
}
function openAPInfoPage(index)
{
	/*if(macarray.countData(macStr) > 1)
	{
		alert("存在MAC地址冲突"+"（" + macStr + "），"+"故无法针对该AP进行管理"+"！");
		return;
	}*/
 	//var index = macarray.searchData(macStr);
	/*var obj=frame12.document.getElementById('pw'+indexarray[index]);
	if(!checkPassword2(obj,11,_("密码"),1,1)) return;
	APList.passStr.value = obj.value;
	APList.ipStr.value= iparray[index];*/
	//alert("index:"+index);
	//alert("macarray[index]:"+macarray[index]);
	APList.macStr.value= macarray[index];
	
	/*APList.vidStr.value= vidarray[index];
	APList.devType.value= types[index];
	APList.szName.value= zones[index];
	APList.dhcpEn.value= dhcp_en[index];
	APList.managedEn.value= managed_enable[index];
	 //alert(APList.ipStr.value+'/'+APList.macStr.value+'/'+APList.passStr.value);
	APList.isGroupOpt.value="";*/
	openForm(APList,"system_summary.asp",750,470);
}

function openManagePage(macStr)
{
	if(macarray.countData(macStr) > 1)
	{
		alert("<%write(MACClash);%>"+"（" + macStr + "），"+"<%write(APAminTips);%>"+"！");
		return;
	}
  	var index = macarray.searchData(macStr);
	var obj=frame12.document.getElementById('pw'+indexarray[index]);
//	if(!checkPassword2(obj,11,_("<%write(passwd);%>"),1,1)) return;
//	APList.passStr.value = obj.value;
	if (APList.passStr.value == "")
	{
		APList.passStr.value = "admin";
	}
	APList.ipStr.value= iparray[index];
	APList.macStr.value= macarray[index];
	APList.vidStr.value= vidarray[index];
	APList.devType.value= types[index];
	APList.szName.value= zones[index];
	APList.dhcpEn.value= dhcp_en[index];
	APList.managedEn.value= managed_enable[index];
	if (apStatus[index] != "1")
	{
		return;
	}
	 //alert(APList.ipStr.value+'/'+APList.macStr.value+'/'+APList.passStr.value);
	APList.isGroupOpt.value="";
	openForm(APList,"ap_remote_manage.asp",750,470);
}

function apReboot(macStr)
{
	if(macarray.countData(macStr) > 1)
	{
		alert("<%write(MACClash);%>"+"（" + macStr + "），"+"<%write(APAminTips);%>"+"！");
		return;
	}
  	var index = macarray.searchData(macStr);
	var obj=frame12.document.getElementById('pw'+indexarray[index]);
	if (APList.passStr.value == "")
	{
		APList.passStr.value = "admin";
	}
	APList.ipStr.value= iparray[index];
	APList.macStr.value= macarray[index];
	APList.vidStr.value= vidarray[index];
	APList.devType.value= types[index];
	APList.szName.value= zones[index];
	APList.dhcpEn.value= dhcp_en[index];
	APList.managedEn.value= managed_enable[index];
	if (apStatus[index] != "1")
	{
		return;
	}
	APList.isGroupOpt.value="";
	if(!confirm(_("确定重启该AP？（AP重启约需1分钟后，才可再次工作。）"))) return;
	APList.optType.value="reboot";//后台根据该值确定是什么操作
	openForm(APList,"ap_remote_manage.asp",750,470);
}

function goToCltPage(index)
{
	APList.apMac.value = macarray[index];
//	alert(APList.apMac.value);
//	return;
	APList.action = "/goform/formWlanCltList";	//页面跳往wlanCltList.asp
	APList.submit();
}

/**
初始化生成列表所需要的变量
**/
function positionfun(mac){
	var index = macarray.searchData(mac);
	//alert("index:"+index);
	APList.serialNo.value = serialno[index];
	//alert("APList.serialNo.value:"+APList.serialNo.value)
	APList.action = "/goform/formFindApInPic";	//页面跳往ap分布图纸页面
	APList.submit();
}
function initDiscoverTable() {
    for (var i = 0; i < managed_enable.length; i++) 
	{
	/*
        if (managed_enable[i]) 
		{
			passValues[i]='';
            passArr[i] = '<input type="password" size="13" maxlength="11" value="" onChange="parent.onPassChg(this,'+i+');" class="input" name="pw' + i + '" id="pw' + i + '"  maxlength="11" value="">';
			manages[i] = "<a href='javascript:parent.openManagePage(\"" + macarray[i] + "\")'><font color='blue'>管理</font></a>&nbsp;<a href='javascript:parent.positionfun(\"" + macarray[i] + "\")'><font color='blue'>定位</font></a>";//远程管理
		   	clienCt[i] = clienCt[i];
        }
		else
		{
//			manages[i] = "";
//			passArr[i] = "";

			passValues[i]='';
            passArr[i] = '<input type="password" size="13" maxlength="11" value="" onChange="parent.onPassChg(this,'+i+');" class="input" name="pw' + i + '" id="pw' + i + '" maxlength="11" value="">';
			manages[i] = "<a href='javascript:parent.openManagePage(\"" + macarray[i] + "\")'><font color='blue'>" + "管理" + "</font></a>&nbsp;<a href='javascript:parent.positionfun(\"" + macarray[i] + "\")'><font color='blue'>定位</font></a>";
		   	clienCt[i] = clienCt[i];
		}
		*/
        indexarray[i] = i;
    }
}

function delAp()
{
		var index, i, j;
		var actStr='';

		index = document.getElementById("sel_list").value;
		if (index == 1) {
		
			actStr = "/goform/formApManageDelAll";
		} else if (index == 0) {
			
			if (!genData(APList, 0, frame12,apStatus,"<%write(status);%>", ",")) {
				return;
			}
			APList.delstr.value= APList.delstr.value.slice(0,-1);
			var apsStr=APList.delstr.value;
			if(apsStr.indexOf(1)!=-1){
				alert("<% write(cannotDelonline);%>");
				return;	
			}
			//alert("APList.delstr.value;"+APList.delstr.value);
			if (!genData(APList, 0, frame12, macarray, "<%write(device);%>", ",")) {
				return;
			}
			APList.delstr.value= APList.delstr.value.slice(0,-1);

			actStr = "/goform/formApManageDel";
		} else {
		
		}

		APList.action = actStr;
		APList.submit();
		return;
}
function group_ok()
{
    var str = '';
    var array = null;
    var array2 = "";
	var i, j = 0, n = 0;
	var passwordVl = new Array();

    if (!genData(APList, 0, frame12, indexarray, "<%write(device);%>", "-")) {
        return;
    }
    APList.delstr.value= APList.delstr.value.slice(0,-1);
    array = APList.delstr.value.split("-");
//    alert("APList.delstr.value:"+APList.delstr.value);
	//alert("array:"+array);
    for(i=0;i<array.length;i++){
	//	alert("array[i"+i+"]:"+array[i]);
	//	alert("tmpSerialno[array["+i+"]]"+tmpSerialno[array[i]]);
	/*
        for(j=0;j<serialno.length;j++){
	//		alert("serialno[j"+j+"]:"+serialno[j])
            if(serialno[j] == tmpSerialno[array[i]]){
                array2 += (j) + "-";
                break;
            }
        }
		*/
		
		array2 += array[i] + "-";			//改法2
    }
    //alert("array2:"+array2);
	for (var k = 0; k < managed_enable.length; k++) {
        if (managed_enable[k]) {
			passwordVl[k]=SCAN.globalPw.value;
		}
	}

    array2 = array2.slice(0,-1);
    array2 = array2.split("-");
    //alert("array2.split('-'):"+array2);
    for (i = 0; i < array2.length; i++) {
//alert(serialno[array2[i]]);
//alert(i);
//alert(types[i]);
//alert(passValues[i]);
//		alert(array2[i]);
//		alert("managed_enable[array2["+i+"]]:"+managed_enable[array2[i]])
		if (managed_enable[array2[i]] == 1)
		{
			j++;
            if(i>0){
			//	alert("types[array["+i+"]]:"+types[array[i]]);
			//	alert("types[array["+(i-1)+"]]:"+types[array[i-1]]);
                if( types[array[i]] != types[array[i-1]] ){
    	            APList.devType.value= "";
//                    return;
                }
            }else{
	            APList.devType.value= types[array[i]];
            }
//			obj=frame12.document.getElementById('pw'+array[i]);
//			alert(i);
//			alert(array[i]);
//			alert(obj);
//alert(passValues[array[i]]);
			/*
			if(checkPassword2(obj,11,"<%write(passwd);%>",1,1))
				str=str+'P'+passwordVl[array2[i]]+' ';//P为设备获取密码标志位。因strtok无法区分连续的分隔符
			else return;
			*/
			str=str+'P'+passwordVl[array2[i]]+' ';//P为设备获取密码标志位。因strtok无法区分连续的分隔符
		}
		else 
		{
			n = 1;
		}
    }
	if (n == 1)
	{
		alert("<%write(warning22);%>");
		return;
	}
	if (j == 0)
	{
		alert("<%write(warning21);%>");
		return;
	}
//alert(str);
	str=str.slice(0,-1);
    APList.passStr.value = str;
	if (!genData(APList, 0, frame12, iparray, "<%write(device);%>", ",")) {
        return;
    }
	APList.ipStr.value= APList.delstr.value.slice(0,-1);
	if (!genData(APList, 0, frame12, macarray, "<%write(device);%>", ",")) {
        return;
    }
	APList.macStr.value=APList.delstr.value.slice(0,-1);
	array = APList.macStr.value.split(",");
    for (i = 0; i < array.length; i++) {
		if(macarray.countData(array[i])>1)
		{
			alert("<%write(MACClash);%>"+"（"+array[i]+"），"+"<%write(addUnitAdminTips);%>"+"！");
			return;
		}
    }
	APList.macStr.value = APList.macStr.value.replace(/-|:/g,'');
	if (!genData(APList, 0, frame12, serialno, "<%write(device);%>", ",")) {
        return;
    }
	APList.serialNo.value = APList.delstr.value.slice(0,-1);

	if (!genData(APList, 0, frame12, zones, "<%write(device);%>", ",")) {
        return;
    }
	APList.szName.value = APList.delstr.value.slice(0,-1);
	var szName_tmp1 = new Array();
	var szName_tmp2 = new Array();
	var sznum=0;
	var szName_tmp = APList.szName.value.split(",");
	for (i =0;i<array.length;i++){
		if(szName_tmp2.countData(szName_tmp[i])>1)
		{
			continue;
		}else if(szName_tmp[i] !="")
		{
			szName_tmp2[sznum]= szName_tmp[i]; 	
			sznum++;
		}
	}
	APList.szName.value = szName_tmp2;


	//alert(APList.ipStr.value+'/'+APList.macStr.value+'/'+APList.passStr.value);
	//alert(APList.serialNo.value);
	APList.isGroupOpt.value="1";
	openForm(APList,"ap_wait_remote_auth.asp",650,215);
}
initDiscoverTable();
//var arrayData1 = new Array(namearray, types , serialno, iparray, macarray, vidarray, apStatus, clienCt, downloads,uploads,ssid, passArr, manages, managed_enable, indexarray);
var arrayData1 = new Array(manageAp, namearray, types , serialno, iparray, macarray, vidarray, apStatus, clienCt, modeAndChannel, ssid, downAnduploads, managed_enable, indexarray);

function init() {
	//alert("arrayData1:"+arrayData1);
	//alert("arrayData1[1]:"+arrayData1[1]);
	//alert("arrayData1[2][1]:"+arrayData1[2][1]);
	if (scanMode == 1)	//自动
	{
//		document.getElementById('scanButton').disabled = true;
//		document.getElementById('scanButton').title = _("APScanCurrentTips");
		document.getElementById('refresh').disabled = false;
	}
	else	//手动
	{
//		document.getElementById('scanButton').disabled = false;
		document.getElementById('refresh').disabled = true;
		document.getElementById('refresh').title = _("<%write(warning20);%>");
	}
	if(!feature_manage)
	{
		var obj = document.getElementsByName("mange_div");
		for(var i=0;i<obj.length;i++)
		{
		 obj[i].style.display="none";
		}
	}
	var m;
	for(m = 0; m < totalrecs; m++)
	{
		if (managed_enable[m] == 1)
		{
			if(permission[m] == 0)
			{
				permission[m] = '<input type="button" value="<%write(allow);%>" style="color:green" onClick="parent.changePermission(this,' + indexarray[m] + ');" id="pms' + m + '">';
			}
			else
			{
				permission[m] = '<input type="button" value="<%write(forbid);%>" style="color:red" onClick="parent.changePermission(this,' + indexarray[m] + ');" id="pms' + m + '">';
			}
		}
		else
		{
			permission[m] = "";
		}
		if (channel[m] == 0)
		{
			channel[m] = "<%write(auto);%>";
		}
	}
	apTypeInit();	

  	frame11.location.href = "_listhead1.htm"; //注意必须在此处初始化，否则页面刷新的时候可能出现bug
    frame12.location.href = "_list1.htm"; //比如排序后刷新 页面会读缓存而非服务器

}

function apTypeInit()
{
	var option, i;
	var f = SCAN;
	var newtypes = new Array();
	var j=0;

	for(i = 0; i < types.length ; i++)
	{
		if(newtypes.countData(types[i]) < 1)
		{  
			newtypes[j++]=types[i];
		}
	}
	for(i = 0; i < newtypes.length ; i++)
	{
		
		option = document.createElement("OPTION");
		option.value = newtypes[i];
		option.text = newtypes[i];
		f.apType.options.add(option);
	}
}

function changePermission(cntrl, index)
{
	APList.serialNo.value = serialno[index];
	//APList.macStr.value = macarray[index];
	//alert(APList.macStr.value);
	//return;
	APList.action = "/goform/formSinglePermissionChange";
	APList.submit();
}

var secondSearch = false;
var tmpArrayData = new Array(new Array());
var pageCount = new Array(0);
var arrayName = null;
var iCount=0;
var iCount2=0;
function searchDataExType(listhead, list, arrayData, strSearch, arrayIndex, listTail) 
{
    var titleCount = 0;
	arrayName = "arrayData1"; //记住当前所搜索的数组名字
	//arrayName = arrayData;
	titleCount = titles1.length;
    if (strSearch == "all" && secondSearch) 	//如果用户不输入任何数据并且用户先前已搜索过数据则还原数据
	{ 
	//alert("tmpArrayData:"+tmpArrayData);
	//alert("tmpArrayData[0]:"+tmpArrayData[0]);
	//alert("tmpArrayData[1][1]:"+tmpArrayData[0][1]);
		for (var i = 0; i < tmpArrayData[0].length; i++)//i共多少列 
		{
			for (var j = 0; j < tmpArrayData[0][0].length; j++) //j共多少行
			{
				arrayData1[i][j] = tmpArrayData[0][i][j];
			}
		}
        recscount[0] = pageCount[0];
        secondSearch = false;
        dsplisthead(0, listhead, arrayName);
        dsplistTail(0, listTail); //重绘表格
        dsplist(0, list);
        return;
    }

    iCount = 0;
    var have = false;
    var tmpArray = new Array();
	if (secondSearch)	//如果是第二次搜索，则先还原arrayData1
	{
		for (var i = 0; i < tmpArrayData[0].length; i++) 
		{
			for (var j = 0; j < tmpArrayData[0][0].length; j++) 
			{
				arrayData1[i][j] = tmpArrayData[0][i][j];
			}
		}
	}
    for (var i = 0; i < arrayData.length; i++) 
	{
        tmpArray[i] = new Array();
    }
	if (strSearch != "other")	//搜索WA1800N或510W
	{
		for (var j = 0; j < arrayData[1].length; j++) 	//遍历arrayData搜索数据
		{ 
			//搜索到数据，并把数据按行赋值给临时数组
			//alert("arrayData[1]:"+arrayData[1]);
			//alert("strSearch:"+strSearch);
			//alert("types[j]:"+types[j]);
			//if (searchStr(strSearch, trimstr(arrayData[1][j].toString()), true) != -1) 	
			//如果strSearch和类型中的某个元素匹配，则将匹配的条目赋给tmpArray
			if(types[j] == strSearch)
			{
				for (var k = 0; k < arrayData.length; k++) 
				{
					tmpArray[k][iCount] = arrayData[k][j];
				}
				have = true;
				iCount++;	//iCount用于记录匹配的条目数
			}
		}
	}
	else	//搜索其他
	{
		for (var j = 0; j < arrayData[1].length; j++) 	//遍历arrayData搜索数据
		{ 
			//搜索到数据，并把数据按行赋值给临时数组
			//if ((searchStr("WA1800N", trimstr(arrayData[1][j].toString()), true) == -1) || (searchStr("510W", trimstr(arrayData[1][j].toString()), true) == -1)) 	//搜索other字串没有找到，则searchStr返回-1
			if (types[j].toString() == "") 	//目前暂以空值区分非WA1800N和非510W设备
			{
				for (var k = 0; k < arrayData.length; k++) 
				{
					tmpArray[k][iCount] = arrayData[k][j];
				}
				have = true;
				iCount++;	//iCount用于记录匹配的条目数
			}
		}
	}
	iCount2=iCount;
//	alert(iCount2);
    if (!have && strSearch != "") 	//没有找到，并且搜索的不是空，则给出没有搜索到的提示
	{
        alert(_("<%write(warning19);%>"));
    //    return;
    }
    //记录下搜索前的数据总数，以便还原
    if (!secondSearch) 
	{
        pageCount[0] = recscount[0];	//pageCount用于记录未搜索时的条目数
    }
    currpage[0] = 1;
    recscount[0] = iCount;	//符合搜索条件的条目数
    iCount = 0;
	//第一次搜索的话，将arrayData1的数据放在tmpArrayData里，以便该函数开始部分的还原
	if (!secondSearch) 
	{
		for (var i = 0; i < arrayData1.length; i++) 
		{
			tmpArrayData[0][i] = new Array();
			tmpArrayData[0][i] = arrayData1[i].slice(0);
		}
	}
	//将符合的条目赋给arrayData1
	for (var i = 0; i < tmpArray.length; i++) 
	{
		for (var j = 0; j < tmpArray[0].length; j++) 
		{
			arrayData1[i][j] = tmpArray[i][j];
		}
		arrayData1[i].splice(j, arrayData1[i].length);
	}
	
    secondSearch = true;
    dsplisthead(0, listhead, arrayName);
    dsplistTail(0, listTail);
    dsplist(0, list);
}

/* 
function changeType()

{
	var typeValue = document.getElementById('apType').value;
	switch(typeValue)
	{
		case "WA1000N":
			searchDataExType(frame11, frame12, arrayData1, "WA1000N", 1);
			//alert("WA1000N");
			break;
		case "WA2000N":
			searchDataExType(frame11, frame12, arrayData1, "WA2000N", 1);
			//alert("WA2000N");
			break;
		case "WA1900N":
			searchDataExType(frame11, frame12, arrayData1, "WA1900N", 1);
			//alert("WA1900N");
			break;
		case "WA1700N":
			searchDataExType(frame11, frame12, arrayData1, "WA1700N", 1);
			//alert("WA1700N");
			break;
		case "WA1800N":
			searchDataExType(frame11, frame12, arrayData1, "WA1800N", 1);
			//alert("WA1800N");
			break;
		case "WA1800NV3":
			searchDataExType(frame11, frame12, arrayData1, "WA1800NV3", 1);
			//alert("WA1800N");
			break;
		case "WA500N":
			searchDataExType(frame11, frame12, arrayData1, "WA500N", 1);
			//alert("WA500N");
			break;
		case "WA501N":
			searchDataExType(frame11, frame12, arrayData1, "WA501N", 1);
			//alert("WA501N");
			break;
	    case "WA1500N":
			searchDataExType(frame11, frame12, arrayData1, "WA1500N", 1);
			//alert("WA1500N");
			break;
		case "WA2200N":
			searchDataExType(frame11, frame12, arrayData1, "WA2200N", 1);
			//alert("WA2200N");
			break;
		case "other":
			searchDataExType(frame11, frame12, arrayData1, "other", 1);
			//alert("other");
			break;
		case "all":
			//alert("all");
			searchDataExType(frame11, frame12, arrayData1, "all", 1);
			break;
	}
}
*/

function changeType()
{
	var typeValue = document.getElementById('apType').value;
	searchDataExType(frame11, frame12, arrayData1, typeValue, 1);
	
	/*由于2.4将型号隐藏，所以types是固定不变的，但现在的这个是变化的*/
	for (var i = 0; i < arrayData1[12].length; i++)
	{
		indexarray[i] = i;
	}
	
}

function reGenData(formName, index, list, array, tips, flagstr)
{
    var recbegin;
    var recend;
    var t = 0;
    var chks = list.document.getElementsByName("chklist");
    if (typeof(flagstr) == "undefined" || trimstr(flagstr) == '') flagstr = ',';
    formName.delstr.value = "";
    recbegin = (currpage[index] - 1) * recsPerPage[index]; //显示页面的开始记录数 以0为开始下标
    recend = recbegin + recsPerPage[index]; //显示页面的结束记录数
	
	var pms = document.getElementById('MultiPermission').value;
    if (recend >= recscount[index]) recend = recscount[index]; //如果是最后页　把总计量赋值给recend
//	alert(recend);
//	alert(recscount[index]);
//	alert(recbegin);
//	alert(pms);
	if (pms == 1)	//允许
	{
		for (var i = recbegin; i < recend; i++)
		{
			if ((chks[t].checked) && (permission2[i] == 0))	//可管理，被勾选，原为禁止
			{ //循环遍历checkbox
				formName.delstr.value = formName.delstr.value + array[i] + flagstr;
			}
			t = t + 1;
		}
	}
	else
	{
		for (var i = recbegin; i < recend; i++)
		{
			if ((chks[t].checked) && (permission2[i] == 1))	//可管理，被勾选，原为禁止
			{ //循环遍历checkbox
				formName.delstr.value = formName.delstr.value + array[i] + flagstr;
			}
			t = t + 1;
		}
	}

//    if (formName.delstr.value != "") return true; //判断是否选择checkbox
//    else {
//        alert("请先选择" + tips + "记录。");
//        return false;
//    }
}

function MultiPermissionChange()
{
	var i, j, p;
	var array = null;
	recbegin = (currpage[0] - 1) * recsPerPage[0]; //显示页面的开始记录数 以0为开始下标
	reGenData(APList, 0, frame12, serialno, _("<%write(device);%>"), ",");
	APList.serialNo.value=APList.delstr.value.slice(0,-1);
	//array = APList.serialNo.value.split(",");
//	alert(APList.serialNo.value);
//	return;
	APList.action = "/goform/formMultiPermissionChange";
	APList.submit();
}

function showOffApNum()
{
	var i,num=0;
	for(i=0;i < apStatus.length;i++)
	{
		if(apStatus[i] == "0")
		{
			num++;
		}
	}
	document.write(num);
}

</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<%write(APAdmin);%>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td><form name="SCAN" id="SCAN" method="post" action=""><input type="hidden" value="Apply" name="_submit">
	<table width="800" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center" class="style7">
   <table id=grouplist style="display" width="80%"  border="0" align="center" cellpadding="0" cellspacing="0"><tr><td colspan="4" height="20">&nbsp;</td></tr>
		     <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe  scrolling="no" name="frame11" valign="bottom" align="center" width="750"  height="44" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden> </iframe>	</td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe scrolling="auto" name="frame12" valign="top" align="center" id="framelist" width="750" height="270" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		     <tr><td colspan="4" height="15" >&nbsp;</td></tr>
            <tr><td colspan="4" >
            <table width="750" align="center"  cellpadding="0" cellspacing="0" border="0">
            <tr valign="middle">
            <td height="21" nowrap width="2%">
              <input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'))></td>
              <td nowrap id="all" width="11%"><%write(com_select);%></td>
              <td  align="left" nowrap width="6%"><!--<input type="submit" name="scanButton" id="scanButton" value="<%write(scan);%>" class="input-button" title="">--><input type="button" name="refresh" id="refresh" value="<%write(refresh);%>" class="input-button" title="" onClick="location.reload()"></td>
              <td  id="type_td" align="right" nowrap width="5%"><%write(model);%></td>
              <td  nowrap width="15%"><select name="apType" id="apType" class="input" onChange="changeType()">
                <option value="all" id="typeAll" selected><%write(allModel);%></option>
              </select></td>
              <td  id="GPWset_td" style="display:none" align="right" nowrap width="10%">
               <%write(globalPasswdSet);%> </td>
            <td  align="left" nowrap width="1%"><input type="password" style="display:none" size="13" maxlength="11" value="admin" onChange='checkPassword2(this,11,"<%write(passwd);%>",1,1);' class="input" name="globalPw" id="globalPw"  style=" margin-bottom:1px">
             <td width="25%"> <input type="button" class="input-button" onClick="group_ok();" value="<%write(bulkOperations);%>" id="mange_div2" name="mange_div2"></td>
             <td width="10%"> <select size="1" class="input" name="sel_list" id="sel_list">
                <option value="0" selected="selected" id="del"><%write(delAp);%></option>
                <option value="1" id="delall"><%write(delAllAp);%> </option>
              </select>
              </td>
              <td width="10%"><input type="button" class="input-button" value="<%write(ok);%> " name="btn_scan2" onClick="delAp()" id="confirm"></td>
              <td width="4%"><input type="button" value="<%write(help);%>" class="input-button" onClick="window.open('<% write(com_helppage);%>#apManage')" id="utthelp"></td>
            </tr>
            </table>
            </td></tr>
             <tr><td colspan="4" height="20">&nbsp;</td></tr>
		      </table></td>
		    </tr>
            <tr><td colspan="4" height="20">&nbsp;</td></tr>
            
		  </table>
		  </td></tr></table>
  </form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
<form method="POST" target="" name="APList">
<input type="hidden" name="delstr" value="">
<input type="hidden" value="apManage" name="parentPage">
<input type="hidden" value="" name="isGroupOpt">
<input type="hidden" value="" name="ipStr">
<input type="hidden" value="" name="macStr">
<input type="hidden" value="" name="vidStr">
<input type="hidden" value="" name="passStr">
<input type="hidden" value="" name="apMac">
<input type="hidden" value="" name="serialNo">
<input type="hidden" value="" name="devType">
<input type="hidden" value="" name="szName">
<input type="hidden" value="" name="dhcpEn">
<input type="hidden" value="" name="managedEn">
<input type="hidden" value="" name="optType">
</form>
</body>
</html>

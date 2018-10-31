<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=GBK"/>
<%langTran("commonWords");%>
<%langTran("serviceZone");%>
<%langTran("edit_SZ");%>
<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script src="switch/js/common_switch.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
/*//����begin
 var zoneName = new Array("default", "SZ_1");	//����������
 var status = new Array("1", "0");	//״̬��1Ϊ������0Ϊ�ر�
 var ssid = new Array("ssid_1", "ssid_2");	//SSID����
 var broadcast = new Array("1", "0");	//�㲥��1Ϊ������0Ϊ�ر�
 var vlanId = new Array("2", "3");	//vlan id��ȡֵ1��4094
 var ipAddr = new Array("192.168.2.1", "172.16.16.1");	//����IP��ַ
 var netMask = new Array("255.255.255.0", "255.255.255.0");	//��������
 var dhcp = new Array("1", "0");	//DHCP��������1Ϊ������0Ϊ�ر�
 var totalrecs = 2;	//��Ŀ��
 var errorstr = "";	//������Ϣ*/
// ����end */
<% 
	var macStr = Form("macStr");//Զ���豸mac��ַ
	var waitOver=Form("waitOver");
	var devType=Form("devType");
    var isGroupOpt=Form("isGroupOpt");
	if (macStr != "")
	{
		//��Ҫָ�����豸�Ƿ�֧�ֶ�SSID�������֧�֣���ҳ�治����ͬʱѡ������������֧�ֶ�SSID���豸�����ֻ��ѡ��4��������
	    aspOutPutSzInfo();
	    aspOutPutServiceZoneUrcp();
	    //write('var multiSsidSupport = 0;');   //�Ƿ�֧�ֶ�SSID,1Ϊ֧�֣�0Ϊ��֧��
	}
	else
	{
	    aspOutPutServiceZone();
	    write('\nvar macStr = "";\n');
	}
%>
//var macStr="<%write(macStr);%>";//Զ��mac�ִ�

/*
limitType[0] = "����";
downUpRate[0] = "100/100"
*/
var downUpRate = new Array();
var limitType = new Array();
for(var i = 0; i < totalrecs ; i++)
{
	/**/
	downUpRate[i] = limit_down[i] + "/" + limit_up[i];
	if (limit_type[i] != "1")
	{
		limitType[i] = "����";
	}
	else
	{
		limitType[i] = "����";
	}
} 

var waitOver="<%write(waitOver);%>";
var devType="<%write(devType);%>";
var isGroupOpt="<%write(isGroupOpt);%>";
//var autoIssued = new Array();
var titles1 = new Array(); //ע��˴�������Ϊtitles1(Ϊ�����б��еı�����Ӧ)
titles1[0] = _("<%write(svcZoneName);%>");
titles1[1] = _("<%write(ssid);%>");
//titles1[3] = _("<%write(broadcast);%>");
titles1[2] = _("<%write(encryption);%>");
titles1[3] = "<%write(vlanId);%>";
titles1[4] ="���ٲ���";
titles1[5] = "����/�ϴ�����(kbit/s)"
titles1[6] = "�Զ��·�";
/*
titles1[0] = _("<%write(svcZoneName);%>");
titles1[1] = _("<%write(svcZoneStatus);%>");
titles1[2] = _("<%write(ssid);%>");
//titles1[3] = _("<%write(broadcast);%>");
titles1[3] = _("<%write(encryption);%>");
titles1[4] = "<%write(vlanId);%>";
titles1[5] = "�Զ��·�";
*/
//titles1[5] = _("<%write(gatewayIpAddr);%>");
//titles1[6] = _("<%write(gatewayIpMask);%>");
//titles1[7] = _("<%write(dhcpServer);%>");

currpage[0] = 1;	//��ǰҳ
recscount[0] = totalrecs;	//��ǰ��Ŀ��
maxrecs[0] = 10;	//�����Ŀ��
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] = _("<%write(svcZoneList);%>");
sortable[0] = new Array(1, 1, 1, 1, 1, 0, 1);
sortType[0] = new Array(TYPE_STR, TYPE_STR, TYPE_STR, TYPE_NUM, TYPE_STR, TYPE_STR, TYPE_STR);
arrayCount[0] = 6;//�ܹ�������(����ָ���Ǹ�������仯����)
recsPerPage[0] = 10; //ÿҳ����������
lineButton[0]=0;
//var arrayData1 = new Array(zoneName, statusSZ, ssid, broadcast, vlanId, autoIssued);
var arrayData1 = new Array(zoneName, ssid, encrytype, vlanId, limitType, downUpRate,autoIssued);

var urcpError="";
//var isGroupOpt=0;
if(!isGroupOpt && waitOver != "")
{
	urcpError = errorstr;//��������
}


var remote;
if (macStr != "")
{
	var remote = 0;
}
else
{
	var remote = 1;
}
function dsplist(index, list) {
    if (index == 0) dspList(index, list, -1, titles1, titles1.length, arrayData1, 1, remote);
}

function go_back()
{
	zoneList.macStr.value = macStr;
    if (!isGroupOpt){
	zoneList.action="ap_remote_manage.asp";
    }else{
	zoneList.action="ap_remote_manage_group.asp";
    }
	zoneList.optType.value="";//��գ���ֹ���ص�ҳ�������
	zoneList.submit();
}

function init() {
	var j;
	frame11.location.href = "_listhead1.htm"; //ע������ڴ˴���ʼ��������ҳ��ˢ�µ�ʱ����ܳ���bug
	frame12.location.href = "_list1.htm"; //���������ˢ�� ҳ����������Ƿ�����
	if (macStr != "")
	{
	    zoneList.passStr.value = passStr;
	    zoneList.ipStr.value= ipStr;
	    zoneList.macStr.value = macStr;
	    zoneList.szName.value = szName;
	    zoneList.managedEn.value = managedEn;
	zoneList.devType.value= devType;
	zoneList.isGroupOpt.value= isGroupOpt;
	    //alert(zoneList.macStr.value);
	}
	for (j = 0; j < ipAddr.length; j++)
	{
		if (statusSZ[j] == "1")
		{
			//status[j] = "����";
			statusSZ[j] = _("<%write(enable);%>");
		}
		else
		{
			//status[j] = "�ر�";
			statusSZ[j] = _("<%write(disable);%>");
		}
		if (broadcast[j] == "1")
		{
			broadcast[j] = _("<%write(enable);%>");
		}
		else
		{
			broadcast[j] = _("<%write(disable);%>");
		}
        if (encrytype[j] == "0") {
            encrytype[j] = _("<%write(no);%>");
        }else if (encrytype[j] == "1"){
            encrytype[j] = "WEP";
        }else if (encrytype[j] == "2"){
            encrytype[j] = "WPA/WPA2";
        }else if (encrytype[j] == "3"){
            encrytype[j] = "WPA-PSK/WPA2-PSK";
        }
		if (dhcp[j] == "1")
		{
			dhcp[j] = _("<%write(enable);%>");
		}
		else
		{
			dhcp[j] = _("<%write(disable);%>");
		}
		if (autoIssued[j] == "1")
		{
			autoIssued[j] = _("<%write(enable);%>");
		}
		else
		{
			autoIssued[j] = _("<%write(disable);%>");
		}
	}
	if (macStr != "")
	{
		document.getElementById('addEntry').style.display = "none";
		document.getElementById('delEntry').style.display = "none";
		document.getElementById('delAllEntry').style.display = "none";
		document.getElementById('fretxt').style.display = "inline";
		document.getElementById('joinSZ').style.display = "inline";
		document.getElementById('goBack').style.display = "inline";
		document.getElementById('f2').style.display = "inline";
		//checkboxInit();
		if (flag_5G != "1")
		{
		    document.getElementById('f5').style.display = "none";
		}
		else
		{
		    document.getElementById('f5').style.display = "inline";
		}
	}
	else
	{
		document.getElementById('addEntry').style.display = "inline";
		document.getElementById('delEntry').style.display = "inline";
		document.getElementById('delAllEntry').style.display = "inline";
		document.getElementById('fretxt').style.display = "none";
		document.getElementById('joinSZ').style.display = "none";
		document.getElementById('goBack').style.display = "none";
	}
/*
	if((macStr != "") && (!isGroupOpt))//��������
	{
		window.document.title = "<%write(APRemoteMG);%>-<%write(joinSZ);%>";
		window.document.title+=" / "+ipStr;
		if(waitOver != "" && urcpError=="") alert("<%write(lawfull);%>");//�û��޷�ֱ�Ӵӽ����Ͽ������������ʾ
	}
*/
    if((macStr != "") && (waitOver != ""))
	{
		if(!showBatchResult(errorArr,ipStr,macStr,_("<%write(failTips);%>"),1))
		{
			alert(_("<%write(lawfull);%>"));
		}
	}
/*
	if (macStr != "")
	{
	    setTimeout("checkboxInit()",100);
	}
*/
}

function EditConfig(val,i){
	if(val==0)	//���
	{ 
		zoneList.oldName.value="";
		if(eval(maxrecs[0]-totalrecs)<1)
		{
			alert(_("<%write(warning7);%>"));
			return;
		}
	}
	else	//�޸�
	{
		zoneList.oldName.value=zoneName[i];
	}
	zoneList.action="edit_SZ.asp";
	zoneList.submit();
}

function DeleteConfig(index){
	if(confirm(_("<%write(warning3);%>"))){
		zoneList.delstr.value=zoneName[index];
		zoneList.action="/goform/formServiceZoneListDel";
	    zoneList.submit();
	}
}

function delAll(){
	if(confirm(_("<%write(warning4);%>"))){
		zoneList.action="/goform/formServiceZoneListDelAll";
		zoneList.submit();
	}
}

function customDel()
{
	genData(zoneList, 0, frame12, zoneName, _("<%write(serviceZone);%>"));
	zoneList.action="/goform/formServiceZoneListDel";
	zoneList.submit();
}

function reGenData(formName, index, list, array, tips, flagstr) //ɾ���б��е�Ԫ��
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
/*
    if (formName.delstr.value != "") return true; //�ж��Ƿ�ѡ��checkbox
    else {
        alert("����ѡ��" + tips + "��¼��");
        return false;
    }
*/
    return true;
}

function joinSZfun()
{
	if (!reGenData(zoneList, 0, frame12, zoneName, _("<%write(svcZoneName);%>"), ",")) {
        return;
    }
	zoneList.SZNameArray.value=zoneList.delstr.value.slice(0,-1);
	//alert(zoneList.SZNameArray.value);
	array = zoneList.SZNameArray.value.split(",");
	if (array.length > 4)
	{
		//alert("AP���ֻ�ܼ���4����������");	
		alert(_("<%write(warning12);%>"));	
		return;
	}
    if (!isGroupOpt){
        if (array.length > maxSzNum)
        {
            //alert("��ѡ�ķ�������Ŀ�Ѿ�������AP��֧�ֵ�����������Ŀ\n��AP���ɼ���" + maxSzNum + "����������");
            alert(_("<%write(warning13);%>"));	
            return;
        }
    }else{
        document.getElementById('optType').value= "PjoinSZ";
    }
    if ( (document.zoneList.fre5.checked == false) && (document.zoneList.fre2.checked == false))
    {
	alert(_("<%write(RfChoseErr);%>"));
	return ;
    }
    //return;
    //alert(zoneList.macStr.value);
    zoneList.action = "ap_wait_remote_conf.asp";
    zoneList.submit();
    //    }else{
    //return;
    //alert(zoneList.macStr.value);
    //	document.getElementById('optType').value= "PjoinSZ";
    //	zoneList.action = "ap_remote_manage_group.asp";
    //	zoneList.submit();

}
function check_5g()
{
    if (document.zoneList.fre5.checked == true)
    {
	if (flag_5G != "1")
	{
	    document.zoneList.fre5.checked = false;
	    alert("���в�֧��5G��AP");
	}
    }
}
/*
function checkboxInit()
{
	var array = null;
	var chks = frame12.document.getElementsByName('chklist');
	array = joinedZones.split(",");
	for (var i=0; i<array.length; i++)
	{
		for (var j=0; j<zoneName.length;j++)
		{
			if (array[i] == zoneName[j])
			{
				chks[j].checked = true;
				break;
			}
		}
	}
}
*/

</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
   <!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****�м�����****-->
  <tr>
   <td width="15"></td>
  <!--////����-->
  <td class="table_bg"><script language="javascript">showSecondTitle(_("<%write(serviceZone);%>"));//��ʾҳ������</script>
   <table width="80%" border="0" height="450" cellspacing="0" cellpadding="0" align="center">
   <script language="javascript">showerror(errorstr,100);//��ʾ���淵�صĴ�����Ϣ</script>
  <tr valign="top">
    <td>
	<!--//�����˵�-->
	<p>&nbsp;</p>
	<script language="javascript">
	
		var arrayThreeMenu=new Array(
						"100%",
						3,
						new Array(_("<%write(svcZoneList);%>"),'serviceZone.asp',1,1),
						new Array(_("<%write(svcZoneConf);%>"),'edit_SZ.asp',remote,0),
						new Array(_("<%write(svcZoneAutoUpdata);%>"),'SzoneAutoUpdata.asp',remote,0)
		);
		showThreeMenu();
	</script>
	 <!--//end-->
<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center"><tr>
    <td align="center"  class="three_td"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr><td><table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td>
	<!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<br>
<form name="zoneList" method="POST">
<input type="hidden" name="delstr" value="">
<input type="hidden" name="SZNameArray" value="">
<input type="hidden" name="statustr">
<input type="hidden" value="Apply" name="_submit">
<input type="hidden" value="" name="ipStr">
<input type="hidden" value="" name="macStr">
<input type="hidden" value="" name="passStr">
<input type="hidden" value="" name="szName">
<input type="hidden" value="" name="managedEn">
<input type="hidden" value="" name="isGroupOpt">
<input type="hidden" value="" name="oldName">
<input type="hidden" value="joinSZ" name="optType" id="optType">
<input type="hidden" value="" name="devType">
<table  border="0" width="600" align="center" cellpadding="0" cellspacing="0">
<tr>
<td colspan="6" height="" align="center" valign="bottom">
<iframe  scrolling="no" name="frame11" valign="bottom" align="center" width="660" height="44" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe>	  </td>
</tr>
<tr>
<td colspan="6" align="center" valign="top">
<iframe name="frame12" scrolling="auto"  valign="top" align="MIDDLE" width="660" height="248" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe></td>
</tr>
<tr>
<td width="25" height="21"><input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist')) id="delChk"></td>
<td align="left" width="20%" id="all"><%write(allSelect);%>/<%write(allUnselect);%></td>
<td width="80%"  align="right">	 
<div id="fretxt" name="fretxt" style="display:none;">Ŀ����Ƶ��Ԫ&nbsp;<span id="f2" name="f2"><input type="checkbox" checked="checked" id="fre2" name="fre2"/>2.4GHz </span><span id="f5" name="f5"><input type="checkbox" id="fre5" name="fre5"/>5GHz&nbsp;</span></div>  
<input type="button" class="input-button" style="display:none" onClick="EditConfig(0)" id="addEntry" value="<%write(addNew);%>">
<input type="button" class="input-button" style="display:none" onClick="delAll()" value="<%write(delAll);%>" id="delAllEntry">
<input type="button" class="input-button" style="display:none" onClick="customDel()" id="delEntry" value="<%write(del);%>">
<input type="button" class="input-button" style="display:none" name="joinSZ" id="joinSZ" value="<%write(joinSZ);%>" onClick="joinSZfun();">
<input type="button" class="input-button" name="helpButton" value="<%write(help);%>" onClick="window.open('<% write(com_helppage);%>#serviceZone')" id="helpButton">
<input type="button" class="input-button" style="display:none" name="goBack" id="goBack" value="<%write(back);%>" onClick="go_back();"></td>
</tr></table>
 </form>
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

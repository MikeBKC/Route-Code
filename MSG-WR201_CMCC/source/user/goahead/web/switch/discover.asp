<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>��������-��������</title>
<link rel=stylesheet type="text/css" href="../css/style.css" title="cool" id="lnk">
<script src="../js/common.js" type="text/javascript"></script>
<script src="js/common_switch.js" type="text/javascript"></script>
<script language="javascript">
<% 
var show=Form("show");
if (show == 'yes')  aspOutPutgetRrcpDiscoveryList();
else
{
	write('var iparray = new Array();\n');
	write('var macarray = new Array();\n');
	write('var portarray = new Array();\n');
	write('var namearray = new Array();\n');
	write('var serialno = new Array();\n');
	write('var types = new Array();\n');
	write('var managed_enable = new Array();\n');
	write('var errorstr = "";\n');
}
%>
function init() {
  	frame11.location.href = "_listhead2.htm"; //ע������ڴ˴���ʼ��������ҳ��ˢ�µ�ʱ����ܳ���bug
    frame12.location.href = "_list1.htm"; //���������ˢ�� ҳ����������Ƿ�����
	if(!feature_manage)
	{
		var obj = document.getElementsByName("mange_div");
		for(var i=0;i<obj.length;i++)
		{
		 obj[i].style.display="none";
		}
	}
}
 /*
var iparray = new Array('92.200.2.82', '92.68.16.251', '192.168.1.154', '192.168.16.254', '192.168.16.254', '192.168.16.254', '92.68.16.251', '192.168.1.154', '192.168.16.254', '192.168.16.254', '192.168.16.254');
var macarray = new Array('00-22-aa-5e-8f-1a', '00-22-aa-c1-02-e8', '00-22-aa-65-81-ca', '00-22-aa-97-29-a0', '00-22-aa-97-28-10','00-22-aa-97-28-10', '00-22-aa-c1-02-e8', '00-22-aa-65-81-ca', '00-22-aa-97-29-a0', '00-22-aa-97-28-10','00-22-aa-97-28-10');
var portarray = new Array('9', '4', '4', '4', '4' ,'5', '4', '4', '4', '4' ,'5');
var namearray = new Array('SW1', 'SW2', 'SW3', 'SW4', 'SW5','SW6', 'SW7', 'SW8', 'SW9', 'SW10','SW11');
var managed_enable = new Array("1","1","1","1","0","1","1","1","1","0","1");
var serialno = new Array('0x0018432a', '0x008cd728', '0x00bc615a', '0x008b2aa0', '0x008b2a90', '0x008b2a90', '0x008cd728', '0x00bc615a', '0x008b2aa0', '0x008b2a90', '0x008b2a90');
var types = new Array("SG2124", "SG3124R", "SG2124F", "SG3124F","SG2124", "SG3124R", "SG3124R", "SG2124F", "SG3124F","SG2124", "SG3124R");//�ͺ�
*/
var feature_manage=1;//�Ƿ�������ҳ��Ŀɹ�����
var passValues=new Array();//ҳ�浱ǰ���������ı���ֵ
var manages =new Array();//�б��еĹ���
var passArr = new Array();//�б��е������
var indexarray = new Array();
var titles1 = new Array();
titles1[0] = "����";
titles1[1] = "�ͺ�";
titles1[2] = "���к�";
titles1[3] = "IP";
titles1[4] = "MAC";
titles1[5] = "���Ӷ˿�";
if(feature_manage)
{
	titles1[6] = "Զ������";
	titles1[7] = "����";
}

currpage[0] = 1;
recscount[0] = iparray.length;
maxrecs[0] = 24;
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] = "����������Ϣ�б�";
sortable[0] = new Array(1, 1, 1, 1, 1, 1, 0, 1);
sortType[0] = new Array(TYPE_CHINESE, TYPE_STR, TYPE_NUM, TYPE_IP, TYPE_STR, TYPE_NUM, TYPE_STR, TYPE_STR);
arrayCount[0] = 9;//�ܹ�������(����ָ���Ǹ�������仯����)
recsPerPage[0] = 10; //ÿҳ����������
lineButton[0] = 0;
managed_enable.evalAll();
var oldDspList=dspList;
var recbegin=0,recend=0;
/**
��д�����ʾdspList����
Ϊ�������µ�����
**/
dspList=function()
{
	var str='';
	var obj=null;
	var j=0;
	var sum=macarray.length;
	recbegin = (currpage[0] - 1) * recsPerPage[0]; //��ʾҳ��Ŀ�ʼ��¼�� ��0Ϊ��ʼ�±�
	/*ԭ������*/
	oldDspList.apply(this,arguments);//�ȼ���oldDspList(arugumens[0],arguments[1]...arguments[argumens.length-1])
	//��applyʵ�ֲ������鵽�����б��ת��
	//ע��:����ֻ������oldDspList����dspList�ĵ�ַ������ֱ����dspList������ջ��������޵ݹ����У�
	/*dspListԭ�нű���������������µ�����*/
	var tr=frame12.document.getElementById('tableList').rows;
	if(feature_manage) j=4;
	else j=3;
	for(var i=1;i<tr.length;i++)
	{
		if( (recbegin+i)>sum) break;//�б����ʾ��¼�Ѿ�����
		str=trimstr(tr[i].cells[j].innerHTML);//ip��ַ���ڵĵ�Ԫ��(�������iparray�����innerHTMLֻ��ip)
		tr[i].cells[j].innerHTML='<a href="http://'+str+'" target="_blank">'+str+'</a>';
		//�����е�ip��ַ��Ϊ�������ӣ��ڴ˸�����Ϊ�˲������б�������ܣ�
		if(feature_manage)
		{
			if(!managed_enable[indexarray[(i+recbegin-1)]])//���ɹ����豸��ǰ���checkbox״̬����
			{
				obj=tr[i].cells[0].childNodes[0];
				obj.checked=false;
				obj.disabled=true;
			}
			else//�ɹ���
			{
				//���������Ա�֤ÿ���������벻�ᱻ���
				obj=tr[i].cells[7].childNodes[0];
				obj.value=passValues[indexarray[(i+recbegin-1)]];
				checkPassword1(obj,11,1,1);
			}
		}
	}
}
function dsplist(index, list) {
	var obj=null;
	var chkBox = 0;
	if(feature_manage) chkBox=1;
    if (index == 0) dspList(index, list, -1, titles1, titles1.length, arrayData1, chkBox, 0);
}
function onPassChg(obj,n)
{
	
		if(!checkPassword1(obj,11,1,1)) return;
		passValues[n]=frame12.document.getElementById('pw'+n).value;
}
//ȫ����������
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
		if(tr[i].cells[0].innerHTML=='') continue;//��������Ϊ��
		if(managed_enable[indexarray[(i+recbegin-1)]])//�ɹ���
		{
			psObj=tr[i].cells[7].childNodes[0];
			psObj.value=obj.value;
			inputRight(psObj);
		}
	}
}
function openManagePage(macStr)
{
	//var ip=iparray[index];
	//var pass=passValues[index];
	//ada.name="admin";
	//ada.ps=pass;
	//ada.url="http://192.168.16.200";
	//ada.yAuthUrl="remote_manage.htm?ip="+ip+"&pass="+pass;
	//ada.start();
	if(macarray.countData(macStr) > 1)
	{
		alert("����MAC��ַ��ͻ��" + macStr + "�������޷���Ըõ�ַ������������");
		return;
	}
  	var index = macarray.searchData(macStr);
	var obj=frame12.document.getElementById('pw'+indexarray[index]);
	if(!checkPassword1(obj,11,1)) return;
	REMOTECONF.passStr.value = obj.value;
	//REMOTECONF.passStr.value = passValues[index];
	REMOTECONF.ipStr.value= iparray[index];
	REMOTECONF.macStr.value= macarray[index].replace(/-|:/g,'');
	 //alert(REMOTECONF.ipStr.value+'/'+REMOTECONF.macStr.value+'/'+REMOTECONF.passStr.value);
	REMOTECONF.isGroupOpt.value="";
	openForm(REMOTECONF,"remote_manage.asp",750,470);
	//REMOTECONF.action = "remote_manage.asp";
	//REMOTECONF.submit();
	//parent.main.location="remote_manage.htm?ip="+ip+"&pass="+pass;
	
}
/**
��ʼ�������б�����Ҫ�ı���
**/
function initDiscoverTable() {
    for (var i = 0; i < managed_enable.length; i++) {
        if (managed_enable[i]) {
			passValues[i]='';
            passArr[i] = '<input type="password" size="13" maxlength="11" value="" onChange="parent.onPassChg(this,'+i+');" class="input" name="pw' + i + '" id="pw' + i + '"  maxlength="11" value="">';
		   manages[i]= "<a href='javascript:parent.openManagePage(\"" + macarray[i] + "\")'><font color='blue'>" + "����" + "</font></a>";//Զ�̹���
        }
		else{manages[i]="";}
        indexarray[i] = i;
        serialno[i] = eval(serialno[i]).toString();
    }
}
/*function openWin2(u, w, h) {
    var l = (screen.width - w) / 2;
    var t = (screen.height - h) / 2;
    var s = 'width=' + w + ', height=' + h + ', top=' + t + ', left=' + l;
    s += ', toolbar=no, menubar=no, scrollbars=yes, resizable=no,location=yes, status=yes';
    window.open(u, '', s);
}*/
function group_ok()
{
    var str = '';
    var array = null;
	var i=0;
    if (!genData(REMOTECONF, 0, frame12, indexarray, "�豸", "-")) {
        return;
    }
	REMOTECONF.delstr.value= REMOTECONF.delstr.value.slice(0,-1);
    array = REMOTECONF.delstr.value.split("-");
    for (i = 0; i < array.length; i++) {
		obj=frame12.document.getElementById('pw'+array[i]);
		if(checkPassword1(obj,11,1))
			str=str+'P'+passValues[array[i]]+' ';//PΪ�豸��ȡ�����־λ����strtok�޷����������ķָ���
		else return;
    }
	str=str.slice(0,-1);
    REMOTECONF.passStr.value = str;
	if (!genData(REMOTECONF, 0, frame12, iparray, "�豸", ",")) {
        return;
    }
	REMOTECONF.ipStr.value= REMOTECONF.delstr.value.slice(0,-1);
	if (!genData(REMOTECONF, 0, frame12, macarray, "�豸", ",")) {
        return;
    }
	REMOTECONF.macStr.value=REMOTECONF.delstr.value.slice(0,-1);
	array = REMOTECONF.macStr.value.split(",");
    for (i = 0; i < array.length; i++) {
		if(macarray.countData(array[i])>1)
		{
			alert("����MAC��ַ��ͻ��"+array[i]+"�������޷���Ըõ�ַ������������");
			return;
		}
    }
	REMOTECONF.macStr.value = REMOTECONF.macStr.value.replace(/-|:/g,'');
	 //alert(REMOTECONF.ipStr.value+'/'+REMOTECONF.macStr.value+'/'+REMOTECONF.passStr.value);
	REMOTECONF.isGroupOpt.value="1";
	openForm(REMOTECONF,"wait_remote_auth.asp",650,215);
	//REMOTECONF.action = "wait_remote_auth.asp";
	//REMOTECONF.submit();
}
initDiscoverTable();
var arrayData1 = new Array(namearray, types, serialno, iparray,macarray, portarray, passArr, manages,indexarray);


</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="../images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
   <!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****�м�����****-->
  <tr>
   <td width="15"></td>
  <!--////����-->
  <td><script language="javascript">showSecondTitle('��������');//��ʾҳ������</script>
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
	<table border="0" width="80%" align='center'><tr><td align="center">
	<!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<form method="post" target="_blank" name="REMOTECONF">
<input type="hidden" name="delstr" value="">
<input type="hidden" value="discover" name="parentPage">
<input type="hidden" value="" name="isGroupOpt">
<input type="hidden" value="" name="ipStr">
<input type="hidden" value="" name="macStr">
<input type="hidden" value="" name="passStr">
</form>
<form name="DISCOVER" method="POST" action="/goform/formRrcpDiscover">
<input type="hidden" value="Apply" name="_submit">
<br>
<table width="100%"  border="0" cellspacing="0" cellping="0">
<tr>
<td ><table width="800" border="0" align="center" cellpadding="0" cellspacing="0">

<tr><td colspan="5" height="" align="center" valign="bottom" >
	<iframe  scrolling="no" name="frame11" valign="bottom" align="center" width="750"  height="44" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden> </iframe>	

</td></tr>

<tr>
<td colspan="5" align="center" valign="top">
<iframe scrolling="no" name="frame12" id="framelist" valign="top" align="center" width="750" height="248" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe>
</td></tr>
<tr><td><table width="750" align="center"  cellpadding="0" cellspacing="0">
<tr><td height="21" valign="top"><span id="mange_div" name="mange_div"><input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'))>&nbsp;ȫѡ / ȫ��ѡ&nbsp;</span></td>

<td align="right" valign="top"><input name="" type="text" style="display:none"><!--�˿ؼ����ã�ֻΪ���ε��������enter�Զ��ύ����--><span id="mange_div" name="mange_div">ȫ����������&nbsp;<input type="password" size="13" maxlength="11" value="" onChange="checkPassword1(this,11,1,1);" class="input" name="globalPw" id="globalPw"  style=" margin-bottom:1px"  onKeyUp="globalSetPs(this)">&nbsp;</span><input type="button" class="input-button" onClick="DISCOVER.submit();" value="��������">&nbsp;<input type="button" class="input-button" onClick="group_ok();" value="��������" id="mange_div" name="mange_div">&nbsp;<input type="button" value="����" class="input-button" onClick="window.open('../rhelpinfo.htm#discover')"></td>
</tr></table></td></tr>
</table>
</td>
</tr>
</table>
</form>
	<!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
   </td></tr> </table>
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

<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=GBK"/>
<% langTran("common"); %>
<%langTran("commonWords");%>
<%langTran("wlanCltList");%>

<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">

<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
var lang="zhcn";

//-----------������ʼ------------
<%aspOutPutWlanCltList();%>


var macSSID = new Array();
for (i=0;i<totalrecs;i++){
	macSSID[i] = macarrays[i]+":"+linkedSSIDs[i];
	}
var strSecondMenuFocus = "wlanApClients";


var titles1 = new Array();
titles1[0] = "�û���";
titles1[1] ="IP��ַ";
titles1[2] ="MAC��ַ";
titles1[3] = "����AP";
titles1[4] = "����SSID";
titles1[5] = "�ź�";
titles1[6] = "����";
titles1[7] = "Ƶ��";
titles1[8] = "����";
titles1[9] = "��������(B/S)";
titles1[10] = "�ϴ�";
titles1[11] = "�ϴ�����(B/S)";
titles1[12] = "����ʱ��";

currpage[0] = 1;	//��ǰҳ
recscount[0] = totalrecs;	//��ǰ��Ŀ��
maxrecs[0] =max_totalrecs;	//�����Ŀ��
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] ="�����û�";
sortable[0] = new Array(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
sortType[0] = new Array(TYPE_STR, TYPE_STR, TYPE_STR, TYPE_STR, TYPE_STR, TYPE_STR, TYPE_STR, TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR);
arrayCount[0] = 13;//�ܹ�������(����ָ���Ǹ�������仯����)
recsPerPage[0] = 10; //ÿҳ����������
lineButton[0] = 1020;
var arrayData1 = new Array(users,iparrays,macarrays,linkedaps,linkedSSIDs,signals,rates,bindwidths,downloads,downRate,uploads,upRate,time,filterRules);

function dsplist(index, list) {
    if (index == 0) dspList(index, list, 1000, titles1, titles1.length, arrayData1, 1, 0);
}


function init() {
  	frame11.location.href = "_listhead1.htm"; //ע������ڴ˴���ʼ��������ҳ��ˢ�µ�ʱ����ܳ���bug
        frame12.location.href = "_list1.htm"; //���������ˢ�� ҳ����������Ƿ�����
		//alert(macSSID);
}
for(i=0;i<totalrecs;i++){
	
	clientenables[i]=(clientenables[i]=="1"?"Yes":"No");
	
}

function SaveUserAllow(index,i)
{
 
	if (clientenables[i].toLowerCase()=="no")��//��ѡ��ԭ����ֵ��"DENY"������ѡ�У���ô�������ñ�����ʱ״̬Ӧ����ѡ��
	{
		if(filterRules[i] == 1)
		{
		str1="�����û���SSID��";
		str2="�����������ַ�����Ƴ���";
		}else if(filterRules[i] == 2)
		{
		str1="�����û����SSID��";
		str2="�����Ľ��õ�ַ�飿";
		}else{
		alert("���û����ڷ������У��޷�ʹ�ô˹��ܣ�");
		dsplist(index,frame12);
		return;
		}
		WlanApClientsList.clientenable.value="1";
	}
	else
	{
		if(filterRules[i] == 1)
		{
		str1="�����û���ӵ�SSID��";
		str2="�����������ַ���У�";
		}else if(filterRules[i] == 2)
		{
		str1="�����û���SSID��";
		str2="�����Ľ�ֹ��ַ�����Ƴ���";
		}else{
		alert("���û����ڷ������У��޷�ʹ�ô˹��ܣ�");
		dsplist(index,frame12);
		return;
		}
		WlanApClientsList.clientenable.value="0";
	}
//	if (confirm("<%write(com_really);%>"+str+"111111"))
	if (confirm(str1+linkedSSIDs[i]+str2))
	{
		 var tempMAC = macarrays[i];
		 tempMAC = tempMAC.replace(/:/g,'');
		 tempMAC = tempMAC.toLowerCase();
		 WlanApClientsList.clientenableID.value = tempMAC;
		 WlanApClientsList.macRule.value = filterRules[i];
		 WlanApClientsList.filterSsid.value = linkedSSIDs[i];
		 WlanApClientsList.action ="/goform/formApMacFilterChange";
		 WlanApClientsList.submit();
	}

	else
	{
		dsplisthead(index,frame11);
		dsplist(index,frame12);
	}
}


function addToFilter(formName, index, list, array, tips, flagstr) //ɾ���б��е�Ԫ��
{
    var recbegin;
    var recend;
    var t = 0;
    var chks = list.document.getElementsByName("chklist");
    if (typeof(flagstr) == "undefined" || trimstr(flagstr) == '') flagstr = ',';
    formName.filterstr.value = "";
    recbegin = (currpage[index] - 1) * recsPerPage[index]; //��ʾҳ��Ŀ�ʼ��¼�� ��0Ϊ��ʼ�±�
    recend = recbegin + recsPerPage[index]; //��ʾҳ��Ľ�����¼��

    if (recend >= recscount[index]) recend = recscount[index]; //��������ҳ�����ܼ�����ֵ��recend

    for (var i = recbegin; i < recend; i++) { //��Ϊrecbegin����ʾ�Ŀ�ʼ��¼��Сһ������i< ���� <= recend

        if (chks.length) { //ѭ������checkbox
            if (chks[t].checked == 1) if (chks[t].value != "on") {
                formName.filterstr.value = formName.filterstr.value + array[eval(chks[t].value)] + flagstr;
                //�������ز���
            } else {
                formName.filterstr.value = formName.filterstr.value + array[i] + flagstr;
            }
        } else {
            if (chks.checked == 1) if (chks.value != "on") {
                formName.filterstr.value = formName.filterstr.value + array[eval(chks.value)] + flagstr;
            } else {
                formName.filterstr.value = formName.filterstr.value + array[i] + flagstr;
            }
        }

        t = t + 1;
    }

    if (formName.filterstr.value != ""){
		formName.filterstr.value=formName.filterstr.value.substring(0,formName.filterstr.value.length-1)
		formName.action = "/goform/formAddToBlackList"
		formName.submit();
		}//�ж��Ƿ�ѡ��checkbox
    else {
        alert("����ѡ��" + tips + "��¼��");
    }
}



function disconnect(formName, index, list, array, tips, flagstr) //ɾ���б��е�Ԫ��
{
    var recbegin;
    var recend;
    var t = 0;
    var chks = list.document.getElementsByName("chklist");
    if (typeof(flagstr) == "undefined" || trimstr(flagstr) == '') flagstr = ',';
    formName.filterstr.value = "";
    recbegin = (currpage[index] - 1) * recsPerPage[index]; //��ʾҳ��Ŀ�ʼ��¼�� ��0Ϊ��ʼ�±�
    recend = recbegin + recsPerPage[index]; //��ʾҳ��Ľ�����¼��

    if (recend >= recscount[index]) recend = recscount[index]; //��������ҳ�����ܼ�����ֵ��recend

    for (var i = recbegin; i < recend; i++) { //��Ϊrecbegin����ʾ�Ŀ�ʼ��¼��Сһ������i< ���� <= recend

        if (chks.length) { //ѭ������checkbox
            if (chks[t].checked == 1) if (chks[t].value != "on") {
                formName.filterstr.value = formName.filterstr.value + array[eval(chks[t].value)] + flagstr;
                //�������ز���
            } else {
                formName.filterstr.value = formName.filterstr.value + array[i] + flagstr;
            }
        } else {
            if (chks.checked == 1) if (chks.value != "on") {
                formName.filterstr.value = formName.filterstr.value + array[eval(chks.value)] + flagstr;
            } else {
                formName.filterstr.value = formName.filterstr.value + array[i] + flagstr;
            }
        }

        t = t + 1;
    }

    if (formName.filterstr.value != ""){
		formName.filterstr.value=formName.filterstr.value.substring(0,formName.filterstr.value.length-1)
		formName.action = "/goform/formDisconnect";
		formName.submit();
		}//�ж��Ƿ�ѡ��checkbox
    else {
        alert("����ѡ��" + tips + "��¼��");
    }
}


function exportData(){
	WlanApClientsList.exportstr.value = "onlineUsers.tar";
	WlanApClientsList.action = "/goform/formExportCltInfo";
	WlanApClientsList.submit();
	
	}

</script>
</head>
<body onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0"><!--�����1-->
  <!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****�м�����****-->
  <tr>
  <td width="15"></td>
  <!--////����-->
  <td><script language="javascript">showSecondTitle("����ͳ��");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg"><!--�������2��-->
   <script language="javascript">showerror(errorstr,80);</script>
  <tr>
    <td>

	<script language="javascript">
		var arrayThreeMenu=new Array(
						  "90%",
	                                                     5,
	                                                     
														 new Array("AP�û�ͳ��",'firstPage.asp',1,0),
														 new Array("SSID�û�ͳ��",'firstWlanSSIDClts.asp',1,0),
														 new Array("��ʷ�û�ͳ��",'firstWlanHistoryClts.asp',1,0),
														 new Array("�����û�",'firstWlanCltList.asp',1,1),
														 new Array("AP��־",'firstApLog.asp',1,0)
						
		);
		showThreeMenu();
	</script>
  <table width="90%" border="0" cellspacing="0" cellpadding="0" align="center"><!--�м�׵Ĳ��ֱ�3-->
<tr>
    <td align="center"  class="three_td" valign="top">
<table border="0" width="100%"  align='center'><tr><td class="gapTd"></td><td align="center"><!--���沿�ֱ�����ı��5-->
<form name="WlanApClientsList" method="post" action="">
   <table id=grouplist style="display"  border="0" align="center" cellpadding="0" cellspacing="0"><!--�б������ı��666666-->
		<tr><td colspan="4" height="20">&nbsp;</td></tr>   
		<tr>
		      <td colspan="4">
			<iframe src="_listhead1.htm" scrolling="no"  name="frame11"  align="MIDDLE" width="700" height="45" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr>
		      <td colspan="4">
			<iframe src="_list1.htm" scrolling="auto"  name="frame12"	id="framelist" align="MIDDLE" width="700" height="265" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe></td>
	<input type="hidden" name="clientenableID"><input type="hidden" name="clientenable"><input type="hidden" name="macRule"><input type="hidden" name="filterSsid"></tr>
                    

			<tr><td colspan="4" height="30">
				 <table width="100%" align="center"  cellpadding="0" cellspacing="0" border="0">
            <tr valign="middle">
           
              <td  align="right" height="30" valign="bottom" width="7%"><input type="button" name="refresh" id="refresh" value="ˢ��" class="input-button" onClick="location.reload()"></td>
             
           
            </tr>
            </table>

			</td></tr>
          
		  </table><!--��Ϣ�б�����������6-->
  </form>
</td><td class="gapTd"></td></tr> </table><!--��Ϣ�б������ı�����5-->
</td></tr>
</table><!--3�׵Ĳ��ֽ���-->

  </td></tr><script language="javascript">init();</script>
</table><!--�����ڶ���2--></td>
<!--////end-->
  <td width="15"></td>
  </tr>
</table><!--�����1-->
</body>
</html>

<head>
<meta http-equiv="Content-Type" content="text/html; charset=GBK"/>
<% langTran("common"); %>
<%langTran("commonWords");%>
<%langTran("wlanCltList");%>
<% langTran("User"); %>

<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">

<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
var lang="zhcn";
var strSecondMenuFocus = "���ߺ�����"




//-----------������ʼ------------
/*
var totalrecs = "3";
var max_totalrecs = "128";
var macarrays = new Array();
var linkedSSIDs = new Array();
macarrays[0] = "0022aa112233";
macarrays[1] = "0022aa445566";
macarrays[2] = "0022aa778899";
linkedSSIDs[0] = "test1";
linkedSSIDs[1] = "test2";
linkedSSIDs[2] = "test3";
*/
<%
aspOutShowBlackList();
%>
//-----------��������------------

var errorstr="";
var titles1 = new Array();
titles1[0] = "MAC��ַ";
titles1[1] ="SSID";

var macSSID = new Array();
for (i=0;i<totalrecs;i++){
	macSSID[i] = macarrays[i]+":"+linkedSSIDs[i];
}



currpage[0] = 1;	//��ǰҳ
recscount[0] = totalrecs;	//��ǰ��Ŀ��
maxrecs[0] =max_totalrecs;	//�����Ŀ��
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] ="�������б�";
sortable[0] = new Array(1,1);
sortType[0] = new Array(TYPE_STR, TYPE_STR);
arrayCount[0] = 2;//�ܹ�������(����ָ���Ǹ�������仯����)
recsPerPage[0] = 10; //ÿҳ����������
lineButton[0] = 0;
var arrayData1 = new Array(macarrays,linkedSSIDs);

function dsplist(index, list) {
    if (index == 0) dspList(index, list, 600, titles1, titles1.length, arrayData1, 1, 1);
}


function init() {
  	frame11.location.href = "_listhead1.htm"; //ע������ڴ˴���ʼ��������ҳ��ˢ�µ�ʱ����ܳ���bug
    frame12.location.href = "_list1.htm"; //���������ˢ�� ҳ����������Ƿ�����
}

function EditConfig(val,i){
	
}


function DeleteConfig(index){
	if(confirm("<% write(MakeSureDel); %>")){
		WlanApClientsList.delstr.value=macSSID[index];
		//alert(WlanApClientsList.delstr.value);
		WlanApClientsList.action="/goform/formDelBlackList";
	    WlanApClientsList.submit();
	}
}
function delConfig(formName, index, list, array, tips, flagstr) //ɾ���б��е�Ԫ��
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
 
    if (formName.delstr.value != ""){
		formName.delstr.value=formName.delstr.value.substring(0,formName.delstr.value.length-1)
		//alert(formName.delstr.value);
		formName.action = "/goform/formDelBlackList"
		formName.submit();
		}//�ж��Ƿ�ѡ��checkbox
    else {
        alert("����ѡ��" + tips + "��¼��");
    }
}




function delAll(){
	if(confirm("<% write(MakeSureDelAll); %>")){
		WlanApClientsList.action="/goform/formDelAllBlackList";
		WlanApClientsList.submit();
	}
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
						"80%",
						6,
						
						new Array("AP�û�ͳ��",'WlanApClientsStatistics.asp',1,0),
						new Array("SSID�û�ͳ��",'WlanSSIDClientsStatistics.asp',1,0),
						new Array("��ʷ�û�ͳ��",'WlanHistoryClientsStatistics.asp',1,0),
						new Array("��ʷ����ͳ��",'historyFlowStatistics.asp',1,0),
						new Array("�û��б�",'wlanCltList.asp',1,0),
						new Array("������",'wlanCltMacFilter.asp',1,1)
						
		);
		showThreeMenu();
	</script>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center"><!--�м�׵Ĳ��ֱ�3-->
<tr>
    <td align="center"  class="three_td" valign="top">
<table border="0"  align='center'><tr><td class="gapTd"></td><td align="center"><!--���沿�ֱ�����ı��5-->
<form name="WlanApClientsList" method="post" action="">
   <table id=grouplist style="display"  border="0" align="center" cellpadding="0" cellspacing="0"><!--�б������ı��666666-->
		<tr><td colspan="4" height="20">&nbsp;</td></tr>   
		<tr>
		      <td colspan="4">
			<iframe src="_listhead1.htm" scrolling="no"  name="frame11"  align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr>
		      <td colspan="4">
			<iframe src="_list1.htm" scrolling="no"  name="frame12"	id="framelist" align="MIDDLE" width="600" height="248" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe></td>
	<input type="hidden" name="clientenableID"><input type="hidden" name="clientenable"><input type="hidden" name="macRule"><input type="hidden" name="filterSsid"></tr>
                    

			<tr><td colspan="4" height="30">
				  <table width="600" align="center"  cellpadding="0" cellspacing="0" border="0">
            <tr valign="middle">
            <td height="21" nowrap width="2%" align="left">
              <input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'))></td>
              <td nowrap id="all" align="left" width="">&nbsp;&nbsp;ȫѡ / ȫ��ѡ</td>
            
             <td width="15%"  align="right"><input type="button" value="<% write(com_delAll); %>" class="input-button" onClick="delAll()" id="delall"></td>
			  <td width="10%" align="right"><input type="submit" value="<% write(com_delete); %>" class="input-button" onClick="return(delConfig(WlanApClientsList,0,frame12, macSSID,'������'))" id="del"><input type="hidden" name="delstr" id="delstr"></td>
             
             
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

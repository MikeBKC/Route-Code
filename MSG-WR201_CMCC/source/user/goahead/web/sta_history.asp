<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=GBK"/>
<% langTran("common"); %>
<%langTran("commonWords");%>
<%langTran("wlanCltList");%>

<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">

<script type="text/javascript" src="js/common.js"></script>
<script src="switch/js/common_switch.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
var lang="zhcn";

//-----------������ʼ------------
<%
var parentPage=Form("parentPage") ;//�����ĸ�ҳ��
var staHostName=Form("HostName");
var macStr=Form("staMac");//Զ��mac�ִ�
aspOutStaHistory();
%>
var staMac = "<%write(macStr);%>";//Զ��mac�ִ�
var HostName = "<%write(staHostName);%>";
//var wlanFre = new Array();
/*
var users = new Array();
var iparrays = new Array();
var macarrays = new Array();
var linkedaps = new Array();
var linkedDate = new Array();
var time = new Array();
var downloads = new Array();
var uploads = new Array();
var hostName = new Array();
var companyName = new Array();
var errorstr = new Array();
users[0] = "test";
iparrays[0] = "192.168.1.199";
macarrays[0] = "D4970B8A16ED";
linkedaps[0] = "0022AA335244";
linkedDate[0] = "2014-12-12 5:30";
time[0] = "99";
downloads[0] = "100K";
uploads[0] = "100K";
hostName[0] = "test_name";
companyName[0] = "test_xiaomi";
totalrecs = 1;
max_totalrecs = 1;
*/
var strSecondMenuFocus = "staHistory";

currpage[0] = 1;	//��ǰҳ
recscount[0] = totalrecs;	//��ǰ��Ŀ��
maxrecs[0] =max_totalrecs;	//�����Ŀ��
listname[0] = "frame12";
listheadname[0] = "frame11";

var titles1 = new Array();
var titles1_index = 0;

//titles1[titles1_index++] ="MAC��ַ";
titles1[titles1_index++] = "��������";
titles1[titles1_index++] = "����AP";
titles1[titles1_index++] = "����ʱ��";
titles1[titles1_index++] = "����";
titles1[titles1_index++] = "�ϴ�";

listheadcaption[0] ="������ʷ";
sortable[0] = new Array(1, 1, 1, 1, 1);
sortType[0] = new Array(TYPE_STR,TYPE_STR,TYPE_STR, TYPE_STR, TYPE_STR);
arrayCount[0] = 5;//�ܹ�������(����ָ���Ǹ�������仯����)
recsPerPage[0] = 10; //ÿҳ����������
lineButton[0] = 1020;
var arrayData1 = new Array(linkedDate,linkedaps,time,downloads,uploads);

function dsplist(index, list) {
    if (index == 0) 
	{
		dspList(index, list, 680, titles1, titles1.length, arrayData1, 0, 0);
	}
}

function init() {
  	frame11.location.href = "_listhead1.htm"; //ע������ڴ˴���ʼ��������ҳ��ˢ�µ�ʱ����ܳ���bug
        frame12.location.href = "_list1.htm"; //���������ˢ�� ҳ����������Ƿ�����
		if (HostName != "")
		{
			document.title = HostName;
		}
		else
		{
			document.title = staMac;
		}
}
/*
for(i=0;i<totalrecs;i++){
	
	clientenables[i]=(clientenables[i]=="1"?"Yes":"No");
	
}
*/
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
  <td><script language="javascript">showSecondTitle("������ʷ");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg"><!--�������2��-->
   <script language="javascript">showerror(errorstr,80);</script>
  <tr>
    <td>

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
			<iframe src="_list1.htm" scrolling="auto"  name="frame12"	id="framelist" align="MIDDLE" width="700" height="300" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden ></iframe></td>
	<input type="hidden" name="clientenableID"><input type="hidden" name="clientenable"><input type="hidden" name="macRule"><input type="hidden" name="filterSsid"></tr>
                    
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

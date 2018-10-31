<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("WebAuth");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

var strSecondMenuFocus = "<%write(com_webAuthStatus);%>";
var rightstr="";
var WebAuthNames = new Array();
var IPAddress = new Array();
var sesIndexs = new Array();
var errorstr="";
var totalrecs=0;
var maxWebAuthSes =120;
WebAuthNames[0]="zhangsan";
IPAddress[0]="192.168.12.13";
sesIndexs[0]="0";
<%aspOutputWebAuthConnListData();%>
var ipaddr = new Array();
var macaddr = new Array();
var totaltime = new Array();
var usedtime = new Array();
var freeAuth = new Array();
var authT = new Array();
<%outputWifiClient();%>

if ((typeof(uttWifiAuth) == 'undefined')) {
    var uttWifiAuth = 0;
}
if(uttWifiAuth == 0) {
recsPerPage[0] =10;
lineButton[0] =10;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=maxWebAuthSes; 
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<%write(web_statusList);%>";
var titles1=new Array(2);
titles1[0]="<%write(com_user);%>";
titles1[1]="<%write(com_IP);%>"


arrayData1=new Array(WebAuthNames,IPAddress,sesIndexs);
//maxDspLen[0]=new Array(10,0,0);
sortable[0]=new Array(1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_IP,TYPE_NUM);
arrayCount[0]=3;
} else { // wifi auth
for(i=0;i<totalrecs;i++)
{
    if(freeAuth[i] == 1) {
	authT[i]="Yes";
    } else {
	authT[i]="No";
    }
}

recsPerPage[0] =10;
lineButton[0] =10;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=totalrecs; 
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<%write(web_statusList);%>";
var titles1=new Array(5);
titles1[0]="<%write(com_IP);%>"
titles1[1]="<%write(com_MAC);%>"
titles1[2]="<%write(web_timeTotal);%>";
titles1[3]="<%write(web_timeUsed);%>";
titles1[4]="<%write(web_freeAuth);%>";


arrayData1=new Array(ipaddr,macaddr,totaltime,usedtime,authT);
//maxDspLen[0]=new Array(10,0,0);
sortable[0]=new Array(1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_STR,TYPE_NUM,TYPE_NUM);
arrayCount[0]=4;
} //end wifi auth

function dsplist(index,list)
{
    if(uttWifiAuth == 0) {
	if(index==0) dspList(index,list,600,titles1,2,arrayData1,1,1);
    } else {
	if(index==0) dspList(index,list,600,titles1,5,arrayData1,1,0);
    }
}
/******************list***********************/
function HangUpConfig(val,index){
	if(confirm("<%write(web_hangTip);%>")){
		WebAuthList.hangup.value=IPAddress[index];
		WebAuthList.action="/goform/formWebAuthHangUp_IP";
	    WebAuthList.submit();
	}
}

function init()
{
    if(uttWifiAuth == 0) {
    } else { //wifi auth
	document.getElementById("dellist").style.display="none";
	document.getElementById("dellist_1").style.display="";
    }
}
</script>
</head>
<body onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("<%write(web_auth);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg" >
   <script language="javascript">showerror(errorstr);</script>
   <script language="javascript">showright(rightstr);</script>
  <tr valign="top">
    <td><form name="WebAuthList" method="post" action="/goform/formWebAuthListHangUp">
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
					"90%",
					3,
					new Array(_("<%write(web_global);%>"),'WebAuthGlobal.asp',1,0),
					new Array(_("<%write(web_server);%>"),'WebAuthServer.asp',1,0),
					new Array(_("<%write(web_status);%>"),'WebAuthStatus.asp',1,1)
		);
		showThreeMenu();
	</script>
   <table id=grouplist style="display" width="90%"  border="0" align="center" cellpadding="0" cellspacing="0" height="300">
  <tr>
    <td align="center" class="three_td" valign="top"><table id=grouplist style="display" width="600"  border="0" align="center" cellpadding="0" cellspacing="0">
	<tr><td colspan="4" height="40">&nbsp;</td></tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="600" height="45" scrolling="no" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="600" height="260" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table name="dellist" id="dellist" style="dsiplay" width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td width="115"  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
				<script language="javascript">document.write(_("<%write(com_select);%>"));</script>
		</div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"><input type="hidden" name="hangup"><input type="hidden" name="AllowID"><input type="hidden" name="Allow"> </td> 
			 <td width="45" valign="top" align="right"><input type="submit" value="<% write(com_hangup); %>" class="input-button" onClick="return(genData(WebAuthList,0,frame12,IPAddress,'<%write(web_authInfo);%>'))" id="dele"></td>
			 <td width="45" valign="top" align="right"><input type="button" value="<%write(com_fresh);%>" onClick="location.reload()" class="input-button" id="del"></td>
			  </tr>
		      </table></td>
		      </tr><tr>
		      <td  colspan="4" valign="bottom"><table name="dellist_1" id="dellist_1" style="display:none" width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td width="115"  class="style6"> &nbsp;</td>
			  <td  class="style6" > &nbsp;</td> 
			 <td width="45" valign="top" align="right">&nbsp;</td>
			 <td width="45" valign="top" align="right"><input type="button" value="<%write(com_fresh);%>" onClick="location.reload()" class="input-button" id="del"></td>
			  </tr>
		      </table></td>
		    </tr>
		  </table></td></tr></table>
  </form></td>
  </tr>
  <script language="javascript">init();</script>
</table></td> 
  <td width="15"></td>
  </tr>
</table>
</body>
</html>

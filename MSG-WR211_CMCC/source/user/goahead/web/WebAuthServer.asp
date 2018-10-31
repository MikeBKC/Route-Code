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

var strSecondMenuFocus = "<%write(com_webAuth);%>";
var rightstr="";
<%aspoutErrorMsg();%>
<%aspOutputWebAuthListData();%>
var i;
for(i = 0;i < totalrecs; i++)
{
    if (timeingPatternEns[i] != "<%write(com_close);%>")
    {
	sysDate=new Date(UTCtoLocal(eval(timeStarts[i]),0) * 1000);
	dateStarts[i] = sysDate.getFullYear()+"-"+eval(sysDate.getMonth()+1)+"-"+sysDate.getDate();
	if (dateStarts[i] == "1970-1-1") 
	{
	    dateStarts[i] ="";
	}
	sysDate=new Date(UTCtoLocal(eval(timeStops[i]),0) * 1000);
	dateStops[i] = sysDate.getFullYear()+"-"+eval(sysDate.getMonth()+1)+"-"+sysDate.getDate();
	if (dateStops[i] == "1970-1-1")
	{
	    dateStops[i] ="";
	}

	var runhour=parseInt(eval(timeTotalSecs[i])/3600);
	var runmiu=parseInt((eval(timeTotalSecs[i])%3600)/60);
	var runsec=parseInt((eval(timeTotalSecs[i])%3600)%60);
	timeTotals[i] =runhour+"<%write(com_hour);%>"+runmiu+"<%write(com_minute);%>"+runsec+"<%write(com_second);%>";

	runhour=parseInt(eval(timeUsedSecs[i])/3600);
	runmiu=parseInt((eval(timeUsedSecs[i])%3600)/60);
	runsec=parseInt((eval(timeUsedSecs[i])%3600)%60);
	timeUseds[i] =runhour+"<%write(com_hour);%>"+runmiu+"<%write(com_minute);%>"+runsec+"<%write(com_second);%>";
	if (timeTotalSecs[i] == "0")
	{
	    timeUseds[i] = "";
	}
    }
    else
    {
	dateStarts[i] ="";
	dateStops[i] ="";
	timeTotals[i] ="";
	timeUseds[i] = "";
    }
}

recsPerPage[0] =10;
lineButton[0] =10;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs; 
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<%write(web_authList);%>";
var titles1=new Array(9);
titles1[0]="<%write(com_user);%>";
titles1[1]="<%write(web_concurrent);%>";
titles1[2]="<%write(com_userSta);%>";
titles1[3]="<%write(web_timeMode);%>";
titles1[4]="<%write(web_timeStart);%>";
titles1[5]="<%write(web_timeStop);%>";
titles1[6]="<%write(web_timeTotal);%>";
titles1[7]="<%write(web_timeUsed);%>";
titles1[8]="<%write(com_descript);%>";

arrayData1=new Array(WebAuthNames,sessionNum,Status,timeingPatternEns,dateStarts,dateStops,timeTotals,timeUseds,Description,instIndexs);
maxDspLen[0]=new Array(10,0,0,0,0,0,0,0,10,0,0);
sortable[0]=new Array(1,1,1,1,1,1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_IP,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR);
arrayCount[0]=11;


function dsplist(index,list)
{
	if(index==0) dspList(index,list,700,titles1,9,arrayData1,1,1);
}
/******************list***********************/
function EditConfig(val,i){
	if(val==0){ 
		WebAuthList.id.value="";
		if(eval(max_totalrecs - totalrecs)<1)	{
			alert("<%write(com_editAlert);%>");
			return;
		}
	}
	else WebAuthList.id.value=instIndexs[i];
	WebAuthList.action="WebAuth_edit.asp";
	WebAuthList.submit();
}
function HangUpConfig(val,index){
	if(Status[index]=="<%write(web_nouse);%>") {
	    alert("<%write(web_nouseTip);%>");
	    return;
	}
	if(confirm("<%write(web_hangTip);%>")){
		WebAuthList.hangup.value=instIndexs[index];
		WebAuthList.action="/goform/formWebAuthHangUp";
	    WebAuthList.submit();
	}
}
function DeleteConfig(index){
	if(confirm("<%write(com_deleteTip);%>")){
		WebAuthList.delstr.value=instIndexs[index];
		WebAuthList.action="/goform/formWebAuthListDel";
	    WebAuthList.submit();
	}
}
function delAll(){
	if(confirm("<%write(com_deleteAllTip);%>")){
		WebAuthList.action="/goform/formWebAuthListDelAll";
		WebAuthList.submit();
	}
}
function preWindow(){
    var preview_web = "http://" + ip + "/WebAuth_auth.asp"
//	window.open(preview_web, '_blank', 'toolbar=no, menubar=no, scrollbars=no, resizable=no, location=no, status=no, top=0, left=0'); 
	openForm(WebAuthpicture,"WebAuth_auth.asp",750,470);
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
    <td><form name="WebAuthList" method="post" action="/goform/formWebAuthListDel">
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
					"90%",
					3,
					new Array(_("<%write(web_global);%>"),'WebAuthGlobal.asp',1,0),
					new Array(_("<%write(web_server);%>"),'WebAuthServer.asp',1,1),
					new Array(_("<%write(web_status);%>"),'WebAuthStatus.asp',1,0)
		);
		showThreeMenu();
	</script>
   <table id=grouplist style="display" width="90%"  border="0" align="center" cellpadding="0" cellspacing="0" height="300">
  <tr>
    <td align="center" class="three_td" valign="top"><table id=grouplist style="display" width="700"  border="0" align="center" cellpadding="0" cellspacing="0">
	<tr><td colspan="4" height="40">&nbsp;</td></tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="700" height="45" scrolling="no" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="700" height="260" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td width="115"  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
				<script language="javascript">document.write(_("<%write(com_select);%>"));</script>
		</div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"><input type="hidden" name="hangup"><input type="hidden" name="AllowID"><input type="hidden" name="Allow"> </td> 
			   <td width="90" valign="top" align="right"><input type="button" value="<%write(com_addNew);%>" class="input-button" onClick="EditConfig(0)" id="add"></td>
			  <td width="95" valign="top" align="right"><input type="button" value="<%write(com_delAll);%>" class="input-button" onClick="delAll()" id="delall"></td>
			 <td width="45" valign="top" align="right"><input type="submit" value="<% write(com_delete); %>" class="input-button" onClick="return(genData(WebAuthList,0,frame12,instIndexs,'<%write(web_authInfo);%>'))" id="del"></td>
			  </tr>
		      </table></td>
		    </tr>
		  </table></td></tr></table>
  </form></td>
  </tr>
</table></td> 
  <td width="15"></td>
  </tr>
</table>
</body>
</html>

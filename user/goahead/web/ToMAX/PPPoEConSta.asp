<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("PPPoEConSta");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

//Butterlate.setTextDomain("internet");

var PPPoEOltimes = new Array();
var PPPoEUpratesOld = new Array();
var PPPoEDownratesOld = new Array();
var errorstr=""; 
var PPPoENames=new Array();
var PPPoEIpaddrs=new Array();
var PPPoEMACaddrs=new Array();
var PPPoEtimes=new Array();
var PPPoEUprates=new Array();
var PPPoEDownrates=new Array();
var expireds=new Array();
var PPPoESessionId=new Array();
var pppoe_remark=new Array();


  <%
  aspPppoeClientStatus();
  aspPppoeServGlobale();
  %>

 
//function initTranslation()
//{
//	var e;
//	
//
//}
var strSecondMenuFocus="<%write(com_pppStaList);%>";

for(i=0;i<totalrecs;i++){
	var runday=parseInt((eval(PPPoEtimes[i])/3600)/24);
	var runhour=parseInt(eval(PPPoEtimes[i])/3600%24);
	var runmiu=parseInt((eval(PPPoEtimes[i])%3600)/60);
	var runsec=parseInt((eval(PPPoEtimes[i])%3600)%60);
	PPPoEOltimes[i] =runday+"<%write(com_day);%>"+runhour+"<%write(com_hour);%>"+runmiu+"<%write(com_minute);%>"+runsec+"<%write(com_second);%>";
	PPPoEUpratesOld[i] =  parseInt(eval(PPPoEUprates[i])/8000);
	PPPoEDownratesOld[i] = parseInt(eval(PPPoEDownrates[i])/8000);
	
}		
/*
function hungUp(){
	 genData(PPPoEConSta,0,frame12,0,'<%write(ppc_hungupinfo);%>');
 }
*/

function hungUp(index,list)
{
  var recbegin;
  var recend;
  var t=0;
  var chks = list.document.getElementsByName("chklist");
  PPPoEConSta.hangstr.value="";
  recbegin=(currpage[index]-1)*recsPerPage[index];
  recend=recbegin+recsPerPage[index];
  if(recend>=recscount[index]) recend=recscount[index];
  for(var i=recbegin;i<recend;i++) {
		if(chks.length) {
			if(chks[t].checked==1) {
				PPPoEConSta.hangstr.value = PPPoEConSta.hangstr.value + PPPoESessionId[i]+",";
				
			}	
		}
	else {
		if(chks.checked==1) {
			PPPoEConSta.hangstr.value = PPPoEConSta.hangstr.value + PPPoESessionId[i]+",";
			}
		}
	t=t+1;
  }
  if(PPPoEConSta.hangstr.value!="")
  {
				PPPoEConSta.action="/goform/formPppoeHang";
				PPPoEConSta.submit();
  }
  else {
	alert("<% write(HangupErrMes); %>");
	return false;
  }
}

recsPerPage[0] =20;
lineButton[0]=20;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]=_("<%write(ppc_list);%>");
var titles1=new Array(9);
titles1[0]=_("<%write(com_user);%>");
titles1[1]=_("<%write(com_IP);%>");
titles1[2]=_("<%write(com_MAC);%>"); 
titles1[3]=_("<%write(ppc_oltime);%>");
titles1[4]=_("<%write(ppc_uprate);%>");
titles1[5]=_("<%write(ppc_downrate);%>");
titles1[6]="<%write(com_userSta);%>";
titles1[7]="<%write(ppc_sessionid);%>";
titles1[8]="<%write(com_remark);%>";

arrayData1=new Array(PPPoENames,PPPoEIpaddrs,PPPoEMACaddrs,PPPoEOltimes,PPPoEUpratesOld,PPPoEDownratesOld,expireds,PPPoESessionId,pppoe_remark);
maxDspLen[0]=new Array(10,0,0,0,0,0,0,0,10);
sortable[0]=new Array(1,1,1,1,1,1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_IP,TYPE_STR,TYPE_DATE,TYPE_NUM,TYPE_NUM,TYPE_STR,TYPE_NUM,TYPE_STR);
arrayCount[0]=9;
<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,920,titles1,9,arrayData1,1,0);
}
-->

</script>
</head>
<body onbeforeunload="logOut()">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("<%write(com_pppSer);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg" >
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td><form name="PPPoEConSta" method="post" action="">
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"95%",
						3,
						new Array(_("<%write(com_pppGlo);%>"),'PPPoEGlobal.asp',1,0),
						new Array(_("<%write(com_pppAcc);%>"),'PPPoEServer.asp',1,0),
						new Array(_("<%write(com_pppSta);%>"),'PPPoEConSta.asp',1,1)
			
		);
		showThreeMenu();
	</script>
   <table id=grouplist style="display" width="95%"  border="0" align="center" cellpadding="0" cellspacing="0" height="300">
  <tr>
    <td align="center" class="three_td" valign="top"><table id=grouplist style="display" width="700"  border="0" align="center" cellpadding="0" cellspacing="0">
	<tr><td colspan="4" height="40">&nbsp;</td></tr>
		    <tr>
		      <td colspan="4" align="center"><iframe src="_listhead1.htm" name="frame11" align="MIDDLE" width="920" scrolling="no" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr>
		      <td colspan="4" align="center"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="920" height="486" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="920"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			   <td width="115"  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick="selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'))">
				 <script language="javascript">document.write(_("<%write(com_select);%>"));</script>
			   </div></td>
			  <td  class="style6"></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"><input type="hidden" name="hangstr"></td> 
			   <td width="40" valign="top" align="right" height="35"><input type="button" value="<%write(com_hangup);%>" class="input-button" onClick="hungUp(0,frame12);" id="hangup"></td>
			  
			  <td width="40" valign="top" align="right" height="35"><input type="button" value="<%write(com_fresh);%>" class="input-button" onClick="location.reload()" id="update"></td>
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

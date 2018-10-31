<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("DoubleRoadInfo");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

//Butterlate.setTextDomain("internet");

//function initTranslation()
//{
//	var e;
//	e = document.getElementById("update");
//	e.value= _("update");
//}
var strSecondMenuFocus="<%write(com_routeCombination);%>";

<%aspMultiPathConfig();%>
<%aspIspRunningConfig();%>

var ConnTypesw=new Array();
var ConnStatussw=new Array();
var PriAddrTypes=new Array();

for(i=0;i<totalrecs;i++){
	if (PortNamesw[i] == "3G") {
		ConnTypesw[i] = "<%write(com_G3Access);%>";
	} else {
		ConnTypesw[i]=(ConnTypes[i]=="STATIC"?"<%write(com_staticAccess);%>":(ConnTypes[i]=="DHCP"?"<%write(com_DynamicAccess);%>":"<%write(com_PPPoE);%>"));
	}
	ConnStatussw[i]=(ConnStatuss[i]=="1"?"<%write(com_Connected);%>":"<%write(com_interrupt);%>");
	
	if(ConnStatuss[i]== "1")
	ConnStatussw[i]=(ConnCheck[i]=="1"?"<%write(com_Connected);%>":"<%write(com_Disconnected);%>(*)");
	
	if(ConnTypes[i]==""){
		ConnTypesw[i]="<font color=red><%write(com_unConfigured);%></font>";
		Weights[i]=ConnStatussw[i]=IPs[i]=KeepLives[i]=RetryTimess[i]=DestIPs[i]=PriAddrTypes[i]="";
	}
	if (DestIPs[i] == "0.0.0.0") {
	    PriAddrTypes[i] = "<% write(gatewayIP); %>";
	} else if (DestIPs[i] != "") {
	    PriAddrTypes[i] = "<% write(otherIP); %>";
	}
}

var max_totalrecs=totalrecs; 
recsPerPage[0] = max_totalrecs;
lineButton[0] =0;
currpage[0]=1;
recscount[0]=totalrecs;
recsPerPage[0] =4;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]=_("<%write(dri_routeList);%>");
var titles1=new Array(9);
titles1[0]="<%write(com_Interface);%>";
titles1[1]="<%write(com_ConnectionType);%>";
titles1[2]="<%write(com_band);%>";
titles1[3]="<%write(routeStatus);%>";
titles1[4]="<%write(com_IP);%>";
titles1[5]="<%write(detectInterval);%>";
titles1[6]="<%write(detectTimes);%>";
titles1[7]="<%write(destIP);%>";
titles1[8]="<%write(targetDetectIP);%>";
arrayData1=new Array(PortNamesw,ConnTypesw,Weights,ConnStatussw,IPs,KeepLives,RetryTimess,PriAddrTypes,DestIPs,PortNames,ConnTypes,ConnStatuss);
sortable[0]=new Array(1,1,1,1,1,1,1,1,1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_CHINESE,TYPE_DATASIZE,TYPE_CHINESE,TYPE_IP,TYPE_NUM,TYPE_NUM,TYPE_STR,TYPE_IP,TYPE_STR,TYPE_STR,TYPE_STR);
arrayCount[0]=12;

<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,920,titles1,9,arrayData1,0,1);
}
/******************list***********************/
function EditConfig(val,i){
	if(val==0) DoubleRoad_List.id.value="";
	else DoubleRoad_List.id.value=PortNames[i];
	DoubleRoad_List.action="DoubleRoadInfo_edit.asp";
	DoubleRoad_List.submit();
}
function DeleteConfig(index){
	if(confirm(_("<%write(com_deleteTip);%>"))){
		DoubleRoad_List.delstr.value=PortNames[index];
		DoubleRoad_List.action="/goform/formDoubleRoad_ListDel";
	    DoubleRoad_List.submit();
	}
}
function delAll(){
	if(confirm(_("<%write(com_deleteAllTip);%>"))){
		DoubleRoad_List.action="/goform/formDoubleRoad_ListDelAll";
		DoubleRoad_List.submit();
	}
}
-->
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
  <td><script language="javascript">showSecondTitle("<%write(com_routeCombination);%>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td><form name="DoubleRoad_List" method="post" action="/goform/formDoubleRoadDel">
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"95%",
						4,
						new Array("<%write(com_globalConfig);%>",'DoubleRoad.asp',1,0),
						new Array("<%write(com_routeCombinationStatus);%>",'DoubleRoadInfo.asp',1,1),
						new Array("<% write(com_lineCombinationCheck); %>",'DoubleRoadInfo_edit.asp',1,0),
						new Array("<% write(com_IdentifyBind); %>",'IdentifyBind.asp',1,0)
		);
		showThreeMenu();
	</script>
   <table id=grouplist style="display" width="95%"  border="0" align="center" cellpadding="0" cellspacing="0" height="250">
  <tr>
    <td align="center" class="three_td" valign="top"><table id=grouplist style="display" width="600"  border="0" align="center" cellpadding="0" cellspacing="0">
	<tr><td colspan="4" height="40">&nbsp;</td></tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="920" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="920" height="145" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"	height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6">&nbsp;</td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td>
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

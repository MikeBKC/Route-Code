<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("IPSec");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

//Butterlate.setTextDomain("firewall");
//function initTranslation()
//{
//	var e;
//	e = document.getElementById("add");
//	e.value= _("add");
//	e = document.getElementById("delall");
//	e.value= _("delall");
//	e= document.getElementById("del");
//	e.value= _("del");
//}

var strSecondMenuFocus="<%write(com_ipsecInfo);%>";

//交互变量
var ids=new Array();
var allow=new Array();
var sa_conn=new Array();
var peer=new Array();
var remoteAddr=new Array();
var packet_out=new Array();
var packet_in=new Array();
var localBind=new Array(); 
var localAddr=new Array();
var totalrecs=0;
var max_totalrecs=100;
var errorstr="";

<%
 getWanIfCount(); 
 aspOutputIPSecListData(); 
 %>
//end

var saStatus=new Array();
var profiles=new Array();
for(var i=0;i<totalrecs;i++)
{
	if(sa_conn[i]==0)
	{
		saStatus[i]="<%write(ipsec_nobuild);%>";
	}
	else if(sa_conn[i]==1)
	{
		saStatus[i]="<%write(ipsec_ike);%>";
	}
	else if(sa_conn[i]==2)
	{
		saStatus[i]="<%write(ipsec_ipsec);%>";
	}
	else
	{
		saStatus[i]="<%write(ipsec_build);%>";
	}
	if(localBind[i].indexOf("PS_")!=-1) {
	    profiles[i]="PPTPS_"+localBind[i].substring(3);
	} else if(localBind[i].indexOf("PC_")!=-1) {
	    profiles[i]="PPTPC_"+localBind[i].substring(3);
	} else{
	if(localBind[i]==0)
	{
		profiles[i]="<%write(ipsec_lan);%>";
	} else if((localBind[i]>=1)&&(localBind[i]<=wanIfCount))
	{
		if (is3GPort(localBind[i],wanIfCount)) {
			profiles[i]="3G";
		} else if (isAPCliPort(localBind[i],wanIfCount)) {
			profiles[i]="APClient";
		}else{
			profiles[i]="WAN"+localBind[i];
		}
	}
	}
}

recsPerPage[0] =10;
lineButton[0]=10;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<%write(com_ipsecInfo);%>";
var titleindex=0;
var titles1=new Array();
titles1[titleindex++]="<%write(ipsec_id);%>";
titles1[titleindex++]="<%write(ipsec_allow);%>";
titles1[titleindex++]="<%write(ipsec_status);%>";
titles1[titleindex++]="<%write(ipsec_peer);%>";
titles1[titleindex++]="<%write(ipsec_remoteaddr);%>";
//titles1[titleindex++]="<%write(ipsec_out);%>";
//titles1[titleindex++]="<%write(ipsec_in);%>";
titles1[titleindex++]="<%write(ipsec_bind);%>";
titles1[titleindex++]="<%write(ipsec_localaddr);%>";

arrayData1=new Array(ids,allow,saStatus,peer,remoteAddr,profiles,localAddr);
sortable[0]=new Array(1,1,1,1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR);
arrayCount[0]=7;

<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,920,titles1,7,arrayData1,1,1);
}
/******************list***********************/
function EditConfig(val,i){
	if(val==0){ 
	    IPSec_List.id.value="";
	    if(eval(max_totalrecs-totalrecs)<1)	{
		alert("<%write(com_editAlert);%>");
		return;
	    }
	}
	else
	{    
	    IPSec_List.id.value=ids[i];
	}
	IPSec_List.action="IPSec_edit.asp";
	IPSec_List.submit();
}
function DeleteConfig(index){
	if(confirm("<%write(com_deleteTip);%>")){
	    IPSec_List.delstr.value=ids[index];
	    IPSec_List.action="/goform/IPSecList_Del";
	    IPSec_List.submit();
	}
}
function delAll(){
	if(confirm("<%write(com_deleteAllTip);%>")){
	    IPSec_List.action="/goform/IPSecList_DelAll";
	    IPSec_List.submit();
	}
}
function SaveUserAllow(index,i)
{
	if (allow[i].toLowerCase()=="no")　//复选框原来的值是"DENY"，即不选中，那么点击后调用本函数时状态应该是选中
	{
		str="<%write(com_allow);%>";
		IPSec_List.Allow.value="yes";
	}
	else
	{
		str="<%write(com_ban);%>";
		IPSec_List.Allow.value="no";
	}
	IPSec_List.AllowID.value=ids[i];
	//alert(IPSec_List.AllowID.value);
	if (confirm("<%write(com_really);%>"+str+"<%write(ipsec_relbuild);%>"))
	{
		 IPSec_List.action ="/goform/formIPSecAllow";
		 IPSec_List.submit();
	}
	else
	{
		dsplisthead(index,frame11);
    	dsplist(index,frame12);
	}
}

/*建立SA*/
function IPSecReqSaData(index,list)
{
	var recbegin;
  	var recend;
  	var t=0;
  	if (eval("0"))
  	{
		alert("<%write(ipsec_vpnmax);%>");
		return false;
  	}
  	IPSec_List.reqstr.value="";
  	var chks = list.document.getElementsByName("chklist");
  	recbegin=(currpage[index]-1)*recsPerPage[index];
  	recend=recbegin+recsPerPage[index];
  	if(recend>=recscount[index]) recend=recscount[index];
  	for(var i=recbegin;i<recend;i++) {
		if(chks.length>1) {
			if(chks[t].checked==1)	{
				if (chks[t].value != "on") {
					if(saStatus[eval(chks[t].value)]=="<%write(ipsec_build);%>") {
						alert(ids[eval(chks[t].value)]+"<%write(ipsec_sabuild);%>");
						return false;
					}
 
					IPSec_List.reqstr.value=IPSec_List.reqstr.value+ids[eval(chks[t].value)]+":";
				} else {
					if(saStatus[i]=="<%write(ipsec_build);%>") {
						alert(ids[i]+"<%write(ipsec_sabuild);%>");
						return false;
					}
					IPSec_List.reqstr.value=IPSec_List.reqstr.value+ids[i]+":";
				}
			}
		}
		else {
			if(chks[t].checked==1)	{
				if (chks[t].value != "on") {
					if(saStatus[eval(chks[t].value)]=="<%write(ipsec_build);%>") {
						alert(ids[eval(chks[t].value)]+"<%write(ipsec_sabuild);%>");
						return false;
					}
 
					IPSec_List.reqstr.value=IPSec_List.reqstr.value+ids[eval(chks[t].value)]+":";
				} else {
					if(saStatus[i]=="<%write(ipsec_build);%>") {
						alert(ids[i]+"<%write(ipsec_sabuild);%>");
						return false;
					}
					IPSec_List.reqstr.value=IPSec_List.reqstr.value+ids[i]+":";
				}
			}
		}
		t=t+1;
  	}
  	if(IPSec_List.reqstr.value=="")
	{
		alert("<%write(ipsec_reqsa);%>");
		return false;
  	}

	IPSec_List.action="/goform/IPSec_ReqSA"
	IPSec_List.submit();
}
/*挂断*/
function IPSecSaData(index,list)
{
	var recbegin;
  	var recend;
  	var t=0;
  	IPSec_List.sa_delstr.value="";
  	var chks = list.document.getElementsByName("chklist");
  	recbegin=(currpage[index]-1)*recsPerPage[index];
  	recend=recbegin+recsPerPage[index];
  	if(recend>=recscount[index]) recend=recscount[index];
  	for(var i=recbegin;i<recend;i++) {
		if(chks.length>1) {
			if(chks[t].checked==1)	{
				if (chks[t].value != "on") {
					if(saStatus[eval(chks[t].value)]!="<%write(ipsec_build);%>") {
						alert(ids[eval(chks[t].value)]+"<%write(ipsec_sanobuild);%>");
						return false;
					}
 
					IPSec_List.sa_delstr.value=IPSec_List.sa_delstr.value+ids[eval(chks[t].value)]+":";
				} else {
					if(saStatus[i]!="<%write(ipsec_build);%>") {
						alert(ids[i]+"<%write(ipsec_sanobuild);%>");
						return false;
					}
 
					IPSec_List.sa_delstr.value=IPSec_List.sa_delstr.value+ids[i]+":";
				}
			}
		}
		else {
			if(chks[t].checked==1) {
				if (chks[t].value != "on") {
					if(saStatus[eval(chks[t].value)]!="<%write(ipsec_build);%>") {
						alert(ids[eval(chks[t].value)]+"<%write(ipsec_sanobuild);%>");
						return false;
					}
 
					IPSec_List.sa_delstr.value=IPSec_List.sa_delstr.value+ids[eval(chks[t].value)]+":";
				} else {
					if(saStatus[i]!="<%write(ipsec_build);%>") {
						alert(ids[i]+"<%write(ipsec_sanobuild);%>");
						return false;
					}
 
					IPSec_List.sa_delstr.value=IPSec_List.sa_delstr.value+ids[i]+":";
				}
			}
		}
		t=t+1;
  	}
  	if(IPSec_List.sa_delstr.value=="")
	{
		alert("<%write(ipsec_clearsa);%>");
		return false;
  	}
	
	IPSec_List.action="/goform/IPSec_Clear";
	IPSec_List.submit();
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
  <td><script language="javascript">showSecondTitle("<%write(ipsec_conf);%>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td ><form name="IPSec_List" method="post" action="/goform/IPSecList_Del">
	<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center" class="style7">
   <table id=grouplist style="display" width="80%"  border="0" align="center" cellpadding="0" cellspacing="0"><tr><td colspan="4" height="20">&nbsp;</td></tr>
		     <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="920" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="920" height="260" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
		<script language="javascript">document.write(_("<%write(com_select);%>"));</script></div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"><input type="hidden" name="Allow"><input type="hidden" name="AllowID"><input type="hidden" name="reqstr"><input type="hidden" name="sa_delstr"></td> 
			   <td width="45" valign="top" align="right"><input type="button" value="<%write(ipsec_setup);%>" class="input-button" onClick="IPSecReqSaData(0, frame12)" id="jianli"></td>
			   <td width="45" valign="top" align="right"><input type="button" value="<%write(com_hangup);%>" class="input-button" onClick="IPSecSaData(0, frame12)" id="guaduan"></td>
			   <td width="90" valign="top" align="right"><input type="button" value="<%write(com_addNew);%>" class="input-button" onClick="EditConfig(0)" id="add"></td>
			  <td width="95" valign="top" align="right"><input type="button" value="<%write(com_delAll);%>" class="input-button" onClick="delAll()" id="delall"></td>
			 <td width="45" valign="top" align="right"><input type="submit" value="<%write(com_delete);%>" class="input-button" onClick="return(genData(IPSec_List,0,frame12,ids,'<%write(ipsec_confinfo);%>'))" id="del"></td>
			  </tr>
		      </table></td>
		    </tr><tr><td colspan="4" height="20">&nbsp;</td></tr>
		  </table>
		  </td></tr></table>
  </form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>

<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("PPPoEServer");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

//Butterlate.setTextDomain("internet");

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
var strSecondMenuFocus = "<%write(com_pppAcc);%>";
<%aspGetPppoeUser();%>
var i;
var bindmacs1 = new Array();
var bindmacs2 = new Array();
for(i = 0;i < totalrecs; i++)
{
sysDate=new Date(UTCtoLocal(eval(starttms[i]),0) * 1000);
startdates[i] = sysDate.getFullYear()+"-"+eval(sysDate.getMonth()+1)+"-"+sysDate.getDate();
if (startdates[i] == "1970-1-1") 
{
    startdates[i] ="";
}
sysDate=new Date(UTCtoLocal(eval(endtms[i]),0) * 1000);
enddates[i] = sysDate.getFullYear()+"-"+eval(sysDate.getMonth()+1)+"-"+sysDate.getDate();
if (enddates[i] == "1970-1-1") 
{
    enddates[i] ="";
}
if((bindmacs[i] != null) && (bindmacs[i].length>13))
{
    bindmacs1[i]=bindmacs[i].slice(0,13);
    bindmacs1[i]+="  <B>...</B>";
    bindmacs2[i] = bindmacs[i].replace(/ /g,"&#13;");
    bindmacs[i] = '<span title="'+ bindmacs2[i] + '">' + bindmacs1[i] +'</span>';
}
if (expireds[i] == "<%write(com_expired);%>")
{
    expireds[i] = '<span style="color:red">' + expireds[i] + '</span>';
}
}
recsPerPage[0] =10;
lineButton[0]=10;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]=_("<%write(pps_pppList);%>");
var titles1=new Array(11);
titles1[0]=_("<%write(com_user);%>");
titles1[1]="<% write(com_startUse);%>";
titles1[2]="<%write(pps_ip);%>";
titles1[3]="<%write(com_userSta);%>";
titles1[4]="<%write(pps_start);%>";
titles1[5]="<%write(pps_end);%>";
titles1[6]="<%write(com_uploadLimit);%>";
titles1[7]="<%write(com_downloadLimit);%>";
titles1[8]="<%write(pps_accmax);%>";
titles1[9]="<%write(com_MAC);%>";
titles1[10]="<%write(com_remark);%>";
arrayData1=new Array(PPPoENames,Allows,aspips,expireds,startdates,enddates,uRates,dRates,accmaxconvs,bindmacs,pppoe_remark,instIndexs);
maxDspLen[0]=new Array(10,0,0,0,0,0,0,0,0,0,10,0);/*省略显示功能，与arrayData一一对应，0为不省略，10为超过10个字符就只显示前十个加省略号*/
sortable[0]=new Array(1,1,1,1,1,1,1,1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_STR,TYPE_IP,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_NUM,TYPE_STR,TYPE_STR);
arrayCount[0]=12;

<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,700,titles1,11,arrayData1,1,1);
}
/******************list***********************/
function SaveUserAllow(index,i)
{
	if (Allows[i].toLowerCase() == "no")　//复选框原来的值是"DENY"，即不选中，那么点击后调用本函数时状态应该是选中
	{
		str="<%write(com_allow);%>";
		PPPoE_edit.Allow.value= '1' ;
	}
	else
	{
		str="<%write(com_ban);%>";
		PPPoE_edit.Allow.value= '0' ;
	}
	PPPoE_edit.AllowID.value=PPPoENames[i];
	if (confirm("<%write(com_really);%>"+str+"<%write(com_relNet);%>"))
	{
		 PPPoE_edit.action ="/goform/formPPPoEAccAllow";
		 PPPoE_edit.submit();
	}
	else
	{
		dsplisthead(index,frame11);
    	dsplist(index,frame12);
	}
}

function EditConfig(val,i){
	if(val==0){ 
		PPPoE_edit.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert(_("<%write(pps_pppWarn);%>"));
			return;
		}
	}
	else PPPoE_edit.id.value=instIndexs[i];
	PPPoE_edit.action="PPPoE_edit.asp";
	PPPoE_edit.submit();
}

function ExportUser(){
/*
	if(val==0){ 
		PPPoE_edit.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert(_("PPPoEwarning"));
			return;
		}
	}
	else PPPoE_edit.id.value=instIndexs[i];
	*/
	PPPoE_edit.action="/goform/formPppoeExport";
	PPPoE_edit.submit();
}

function ImportConfig(){
/*
	if(val==0){ 
		PPPoE_edit.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert(_("PPPoEwarning"));
			return;
		}
	}
	else PPPoE_edit.id.value=instIndexs[i];
	*/
	PPPoE_edit.action="PPPoEImport.asp";
	PPPoE_edit.submit();
}
 
function DeleteConfig(index){
			if(confirm(_("<%write(com_deleteTip);%>"))){
				PPPoE_edit.delstr.value=instIndexs[index];
				PPPoE_edit.action="/goform/formPppoeUserDel";
				PPPoE_edit.submit();
			}

}
function delAll(){
	if(confirm(_("<%write(com_deleteAllTip);%>"))){
		PPPoE_edit.action="/goform/formPppoeUserDelAll";
		PPPoE_edit.submit();
	}
}
function delPppoe(){
		genData(PPPoE_edit,0,frame12,instIndexs,'<%write(pps_pppInfo);%>')
		PPPoE_edit.action="/goform/formPppoeUserDel";
		PPPoE_edit.submit();
}
function doLimit(){
	if(checkNumber(PPPoE_edit.MaxUpRatetxt,0,102400,"<%write(com_uploadLimit);%>",0,0)){
	    if(checkNumber(PPPoE_edit.MaxDownRatetxt,0,102400,"<%write(com_downloadLimit);%>",0,0)){
		genData(PPPoE_edit,0,frame12,instIndexs,'<%write(pps_pppInfo);%>')
		PPPoE_edit.action="/goform/formConfigPppoeRateLimit";
		PPPoE_edit.submit();	
	    }
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
  <td><script language="javascript">showSecondTitle(_("<%write(com_pppSer);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td><form name="PPPoE_edit" method="post" action="/goform/formPppoeUserDel">
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"100%",
						5,
						new Array(_("<%write(com_pppGlo);%>"),'PPPoEGlobal.asp',1,0),
						new Array(_("<%write(com_pppAcc);%>"),'PPPoEServer.asp',1,1),
						new Array(_("<%write(com_pppSta);%>"),'PPPoEConSta.asp',1,0),
						new Array(_("<%write(com_pppExp);%>"),'PPPoEExport.asp',1,0),
						new Array(_("<%write(com_pppImp);%>"),'PPPoEImport.asp',1,0)
		);
		showThreeMenu();
	</script>
   <table id=grouplist style="display" width="100%"  border="0" align="center" cellpadding="0" cellspacing="0" height="490">
  <tr>
    <td align="center" class="three_td" valign="top"><table id=grouplist style="display" width="700"  border="0" align="center" cellpadding="0" cellspacing="0">
	<tr><td colspan="4" height="40">&nbsp;</td></tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11"  align="MIDDLE" width="700" height="45" scrolling="no" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist"  align="MIDDLE" width="700" height="260" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td width="115" class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
				<script language="javascript">document.write(_("<%write(com_select);%>"));</script>
		</div></td>
			  <td width="54"  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"><input type="hidden" name="AllowID"><input type="hidden" name="Allow"></td>
			   <td colspan="1" valign="top" align="right"><input type="button" value="<%write(com_addNew);%>" class="input-button" onClick="EditConfig(0)" id="add">&nbsp;&nbsp;<input type="button" value="<%write(com_delAll);%>" class="input-button" onClick="delAll()" id="delall">&nbsp;&nbsp;<input type="button" value="<%write(com_delete);%>" class="input-button" onClick="delPppoe()" id="del"></td>
			  </tr>
<tr>
    <td colspan="3"></td>
    <td align="right"><%write(com_uploadLimit);%>&nbsp;&nbsp;
	<input type="text" name="MaxUpRatetxt"onKeyPress="figureOnly()" size="8" maxlength="6">&nbsp;&nbsp;kbit/s&nbsp;&nbsp;&nbsp;&nbsp;<%write(com_downloadLimit);%>&nbsp;&nbsp;
	<input type="text" name="MaxDownRatetxt"onKeyPress="figureOnly()" size="8" maxlength="6">&nbsp;&nbsp;kbit/s&nbsp;&nbsp;&nbsp;&nbsp;
	<input align="right" type="button" value="<%write(pps_ratelimit);%>" class="input-button" onClick="doLimit()" id="ratelimit">
    </td>
    </table>
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

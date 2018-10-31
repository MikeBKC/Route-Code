<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("DnsFilter");%>
<%langTran("ParentsControl");%>
<title><%write(com_parentscontrol);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk"/>
<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="lang/b28n.js"></script>

</head>
<script type="text/javascript">
var strSecondMenuFocus="<%write(com_parentscontrol);%>";
var errorstr="";

var totalrecs=0;var max_totalrecs=128;
<%
getArpFromArptable();
getCurrentIPMacInfo();
getParentsControlGlobalInfo();
getParentsControlConf();
%>
/*
var currpage = new Array();
var recscount = new Array();
var listname = new Array();
var listheadname = new Array();
var listheadcaption = new Array();
*/
recsPerPage[0] =10;
lineButton[0] =10;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<%write(par_list);%>";
var titles1=new Array(5);
titles1[0]="<%write(com_user);%>";
titles1[1]="<%write(com_startUse);%>";
titles1[2]="<%write(com_MAC);%>";
titles1[3]="<%write(par_allowSite);%>";
titles1[4]="<%write(dns_effectTime);%>";
/*
var UserNames = new Array();
var Allows= new Array();
var arp_Macs = new Array();
var timeGrps = new Array();
*/
var allowSites = new Array();
 
arrayData1=new Array(UserNames,Allows,arp_Macs,userUrls,timeGrps);
sortable[0]=new Array(1,1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR);
arrayCount[0]=5;
 
 
 
function dsplist(index,list)
{
	if(index==0) dspList(index,list,600,titles1,5,arrayData1,1,1);
}
/******************list***********************/
function SaveUserAllow(index,i)
{
	if (Allows[i].toLowerCase()=="no")　//复选框原来的值是"DENY"，即不选中，那么点击后调用本函数时状态应该是选中
	{
		str="<%write(com_allow);%>";
		ParentControl_Config.Allow.value="yes";
	}
	else
	{
		str="<%write(com_ban);%>";
		ParentControl_Config.Allow.value="no";
	}
	ParentControl_Config.AllowID.value=UserNames[i];
	//alert(IPSec_List.AllowID.value);
	if (confirm("<%write(com_really);%>"+str+""))
	{
		 ParentControl_Config.action ="/goform/formConfigParentsControlAllow";
		 ParentControl_Config.submit();
	}
	else
	{
		dsplisthead(index,frame11);
		dsplist(index,frame12);
	}
}
function EditConfig(val,i){
	if(val==0){ 
		ParentControl_Config.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert(_("<%write(com_editAlert);%>"));
			return;
		}
	}
	else ParentControl_Config.id.value=UserNames[i];
	ParentControl_Config.action="ParentsControl_edit.asp";
	ParentControl_Config.submit();
}
function DeleteConfig(index){
	if(confirm(_("<%write(com_deleteTip);%>"))){
		ParentControl_Config.delstr.value=UserNames[index];
		ParentControl_Config.action="/goform/formParentsControlDel";
		ParentControl_Config.submit();
	}
}
function delAll(){
	if(confirm(_("<%write(com_deleteAllTip);%>"))){
		ParentControl_Config.action="/goform/formParentsControlDelAll";
		ParentControl_Config.submit();
	}
}
function checkSameMac()
{
	var tbl = ParentControl_Config;
	if(tbl.parentpc1.value.toLowerCase()!="") {
	    if( (tbl.parentpc1.value.toLowerCase() == tbl.parentpc2.value.toLowerCase())||
		    (tbl.parentpc1.value.toLowerCase() == tbl.parentpc3.value.toLowerCase())||
		    (tbl.parentpc1.value.toLowerCase() == tbl.parentpc4.value.toLowerCase())){
		    alert("<%write(par_samemac);%>");
		return false;
	    }
	}
	if(tbl.parentpc2.value.toLowerCase()!="") {
		if((tbl.parentpc2.value.toLowerCase() == tbl.parentpc3.value.toLowerCase())||
		 	(tbl.parentpc2.value.toLowerCase() == tbl.parentpc4.value.toLowerCase())){
		    alert("<%write(par_samemac);%>");
		    return false;
		}
	}
	if(tbl.parentpc3.value.toLowerCase()!="") {
		if( (tbl.parentpc3.value.toLowerCase() == tbl.parentpc4.value.toLowerCase()) ) {
		    alert("<%write(par_samemac);%>");
		    return false;
		}
	}

	return true;
}
function checkData()
{
    var tbl = ParentControl_Config;
    if(!checkMac(tbl.parentpc1,"<%write(com_MAC);%>",1)) return false;
    if(!checkMac(tbl.parentpc2,"<%write(com_MAC);%>",1)) return false;
    if(!checkMac(tbl.parentpc3,"<%write(com_MAC);%>",1)) return false;
    if(!checkMac(tbl.parentpc4,"<%write(com_MAC);%>",1)) return false;
    if(!checkSameMac()) return false;
    return true;
}
function save()
{
    if(checkData()) {
	ParentControl_Config.action="/goform/formParentsControlGlobalConfig";
	ParentControl_Config.submit();

    }
}
function addMacToParents()
{
    var tbl = ParentControl_Config;
    for(var i=0;i<tbl.currNetworkPC.options.length;i++)
    {
    if(tbl.currNetworkPC.options[i].selected){
	if(!checkMac(tbl.currNetworkPC.options[i],"<%write(com_MAC);%>",0)) {
	    //alert("MAC 地址输入错误。");
	    return false;
	}
	if((tbl.currNetworkPC.options[i].value.toLowerCase() == tbl.parentpc1.value.toLowerCase())||
		(tbl.currNetworkPC.options[i].value.toLowerCase() == tbl.parentpc2.value.toLowerCase())||
		(tbl.currNetworkPC.options[i].value.toLowerCase() == tbl.parentpc3.value.toLowerCase())||
		(tbl.currNetworkPC.options[i].value.toLowerCase() == tbl.parentpc4.value.toLowerCase())) {
	    alert("<%write(par_macerror);%>");
	    return ;
	}
	if( tbl.parentpc1.value=="") {
	    tbl.parentpc1.value=tbl.currNetworkPC.options[i].value.toLowerCase();
	}else if( tbl.parentpc2.value=="") {
	    tbl.parentpc2.value=tbl.currNetworkPC.options[i].value.toLowerCase();
	}else if( tbl.parentpc3.value=="") {
	    tbl.parentpc3.value=tbl.currNetworkPC.options[i].value.toLowerCase();
	}else if( tbl.parentpc4.value=="") {
	    tbl.parentpc4.value=tbl.currNetworkPC.options[i].value.toLowerCase();
	}else {
	    alert("<%write(par_maxParentsmac);%>");
	}
    }
    }
    return;
}
function init(){
    /*
			dsplisthead(index,frame11);
    	dsplist(index,frame12);
	*/
	var tbl = ParentControl_Config;
	tbl.AllowControlEnable.checked=(AllowControlEnables == "on");
	for(var i=0;i<i_totalrecs;i++) {
	    var option=document.createElement("OPTION");
	    option.text=i_macs[i].toLowerCase();
	    option.value=i_macs[i].toLowerCase();
	    if(i_macs[i].toLowerCase() == currentMac.toLowerCase()) {
	    option.selected = true;
	    }
	    tbl.currNetworkPC.options.add(option);
	}
	if(parentsMac1 != "000000000000") {
	    tbl.parentpc1.value=parentsMac1;
	} else {
	    tbl.parentpc1.value="";
	}
	if(parentsMac2 != "000000000000") {
	    tbl.parentpc2.value=parentsMac2;
	} else {
	    tbl.parentpc2.value="";
	}
	if(parentsMac3 != "000000000000") {
	    tbl.parentpc3.value=parentsMac3;
	} else {
	    tbl.parentpc3.value="";
	}
	if(parentsMac4 != "000000000000") {
	    tbl.parentpc4.value=parentsMac4;
	} else {
	    tbl.parentpc4.value="";
	}
	for(var j=0;j<totalrecs;j++) {
	    if(timeGrps[j] == "") {
		timeGrps[j] = "<%write(dns_allTime);%>";
	    }
	}
}
function Reset()
{
	var tbl = ParentControl_Config;
	ParentControl_Config.reset();
  	if(tbl.currNetworkPC.options.length>=0) {
	    for(var i=(tbl.currNetworkPC.options.length)-1;i>=0;i--){
		tbl.currNetworkPC.remove(i);
	    }
	}
	init();
}

</script>
<body background="<%write(com_bodyMain);%>">
<table width="100%" height="100%" border="0" cellspacing="0" cellpadding="0">
	<tr>
    	<td colspan="3" height="15"></td>
    </tr>
    <tr>
  	<td width="15"></td>
  	    <td align="center">
    	<script language="javascript">showSecondTitle("<%write(com_parentscontrol);%>");</script>
        <table width="100%" border="0" height="100%" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
        <script language="javascript">showerror(errorstr,80);</script>
        <tr valign="top">
        	<td >
				<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
                <p>&nbsp;</p>
                <tr>
                	<td align="center" class="style7">
                    	 <table border="0" width="80%" align='center' cellpadding="0" cellspacing="0">
                        <form action="/goform/formParentsControlDel" onSubmit="return " name="ParentControl_Config" method="post">
                            <tr><td colspan="4" height="20">&nbsp;</td></tr>
                        	<tr>
                          		<td width="40%" height="25" align="right" valign="middle"><%write(par_start);%></td>
                          		<td width="3%" height="25">&nbsp;</td>
                          		<td align="left"  height="25" valign="middle"><input type="checkbox" name="AllowControlEnable" id="AllowControlEnable" /></td>
                                <td align="left" height="25">&nbsp;</td>
                          	</tr>
                             <tr>
                          		<td width="40%" height="25" align="right" valign="middle"><%write(par_innerMac);%></td>
                          		<td width="3%" height="25">&nbsp;</td>
                          		<td align="left"  height="25" valign="middle"><select name="currNetworkPC" class="input" style="width:130px;">
                                <!--	<option value="105056C00001">105056C00001</option> -->
                                    
                                </select><input type="button" value="<%write(com_add);%>" onClick="addMacToParents()" class="input-button"/></td>
                                <td align="left" height="25">&nbsp;</td>
                          	</tr>
                            <tr>
                          		<td width="40%" height="25" align="right" valign="middle"><%write(par_parentsMac_1);%>1<%write(par_parentsMac_2);%></td>
                          		<td width="3%" height="25">&nbsp;</td>
                          		<td align="left" height="25" valign="middle"><input type="text" class="input" name="parentpc1" value="" style="width:130px;" size="18"  maxlength="17" onChange="showfiledtip(re_checkMac(this,'<%write(com_MAC);%>'),mactd1)"/></td>
                                <td align="left" height="25" id="mactd1">&nbsp;</td>
                          	</tr>
                             <tr>
                          		<td width="40%" height="25" align="right" valign="middle"><%write(par_parentsMac_1);%>2<%write(par_parentsMac_2);%></td>
                          		<td width="3%" height="25">&nbsp;</td>
                          		<td align="left" height="25" valign="middle"><input class="input" type="text" name="parentpc2" value="" style="width:130px;" size="18"  maxlength="17" onChange="showfiledtip(re_checkMac(this,'<%write(com_MAC);%>'),mactd2)"/></td>
                                <td align="left" height="25" id="mactd2">&nbsp;</td>
                          	</tr>
                             <tr>
                          		<td width="40%" height="25" align="right" valign="middle"><%write(par_parentsMac_1);%>3<%write(par_parentsMac_2);%></td>
                          		<td width="3%" height="25">&nbsp;</td>
                          		<td align="left" height="25" valign="middle"><input type="text" class="input" name="parentpc3" value="" style="width:130px;" size="18"	maxlength="17" onChange="showfiledtip(re_checkMac(this,'<%write(com_MAC);%>'),mactd3)"/></td>
                                <td align="left" height="25" id="mactd3">&nbsp;</td>
                          	</tr>
                            <tr>
                          		<td width="40%" height="25" align="right" valign="middle"><%write(par_parentsMac_1);%>4<%write(par_parentsMac_2);%></td>
                          		<td width="3%" height="25">&nbsp;</td>
                          		<td align="left" height="25" valign="middle"><input type="text" class="input" name="parentpc4" value="" style="width:130px;" size="18"	maxlength="17" onChange="showfiledtip(re_checkMac(this,'<%write(com_MAC);%>'),mactd4)"/></td>
                                <td align="left" height="25" id="mactd4">&nbsp;</td>
                          	</tr>
                            <!-- <tr>
                          		<td width="40%" height="25" align="right" valign="middle">当前管理电脑的MAC地址</td>
                          		<td width="3%" height="25">&nbsp;</td>
                          		<td align="left"  height="25" valign="middle"><input type="text" class="input" name="currManagePc"style="width:130px;"/><input type="button" value="设为家长电脑" class="input-button" /></td>
                                <td align="left" height="25">&nbsp;</td>
                          	</tr>-->
                            
                              <tr><td colspan="4" align="center" height="40">
                            	
                           				<input type="button" name="saveControl" class="input-button" value="<%write(com_save);%>" onClick="save()"/>
                                        <input type="button" name="resetControl" class="input-button" value="<%write(com_reset);%>" onClick="Reset()"/>
                                        <!-- <input type="button" name="helpControl" class="input-button" value="帮助" /> --> 
                          			                        	</td></tr>
                          	<tr><td colspan="4" align="center" height="40">
                            	<table border="0" cellspacing="0" cellpadding="0" width="100%">
                          			<tr>
                           				<td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
                          			</tr>
                         		</table>
                        	</td></tr>
                          
                      <!--</form>-->
                      
                      	<tr>
                        	<td colspan="4" align="center">
                                  <table id=grouplist style="display"  border="0" align="center" cellpadding="0" cellspacing="0">
                                    <tr bgcolor="c0ddff">
                                      <td colspan="4">
                                    <iframe src="_listhead1.htm" name="frame11" scrolling="no"  align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
                                    </tr>
                                    <tr bgcolor="c0ddff">
                                      <td colspan="4">
                                    <iframe src="_list1.htm" name="frame12" id="framelist"  align="MIDDLE" width="600" height="240" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
                                    </tr>
                                    <tr>
                                      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
                                      <tr>
                                      <td width="5"></td>
                                      <td  class="style6"><div align="left">
                                        <input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
                               <%write(com_select);%></div></td>
                                      <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"><input type="hidden" name="AllowID" ><input type="hidden" name="AllowIP"><input type="hidden" name="AllowMAC"><input type="hidden" name="Allow"></td> 
                                      <td width="90"   valign="top" align="right"><input type="submit" value="<%write(com_addNew);%>" class="input-button"  id="add" onClick="EditConfig(0)"></td>
                                      <td   width="95"  valign="top" align="right"><input type="button" value="<%write(com_delAll);%>" class="input-button" onClick="delAll()" id="delall"></td>
                                      <td   width="45"   valign="top" align="right"><input type="submit" value="<%write(com_delete);%>" class="input-button"onClick="return(genData(ParentControl_Config,0,frame12,UserNames,'<%write(par_list);%>'))" id="del"></td>
                                      </tr>
                                      </table></td>
                                    </tr>
                                    <tr><td colspan="4" height="20">&nbsp;</td></tr>
                                  </table>
                                </form>
                            </td>
                        </tr>
                      </table>             	
                    </td>
                </tr>
				</table>
             </td>
             
        </tr>
        </table>
    </td>
    <td width="15"><script language="javascript">init();</script></td>
    </tr>
</table>
</body>
</html>

<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("DDNSAdvance");%>
<%langTran("common");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

var strSecondMenuFocus="<%write(DDNS_status);%>";
var Profiles=new Array();
var Connstatus=new Array();
var DDNSs=new Array();
var ddns_ips=new Array();
var ddns_times=new Array();
//var action=new Array();

<%outputDDNSList();%>
recsPerPage[0] =4;
lineButton[0]=0;
currpage[0]=1;
recscount[0]=totalrecs;
//maxrecs[0]=4;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<%write(DDNS_statusList);%>";
var titles1=new Array(3);
titles1[0]="<%write(profile);%>";
titles1[1]="<%write(update_status);%>";
titles1[2]="<%write(host);%>";
titles1[3]="<%write(ip_addr);%>";
titles1[4]="<%write(updata_time);%>";
//titles1[5]="操作";

arrayData1=new Array(Profiles,Connstatus,DDNSs,ddns_ips,ddns_times);
sortable[0]=new Array(1,1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_STR,TYPE_STR,TYPE_IP,TYPE_STR);
arrayCount[0]=5;

<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,600,titles1,5,arrayData1,1,1);
}
/******************list***********************/
function EditConfig(val,i){
	if(val==0){ 
		DDNSList.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert("<%write(com_editAlert);%>");
			return;
		}
	}
	else DDNSList.id.value=Profiles[i];
	DDNSList.action="DDNS_edit.asp";
	DDNSList.submit();
}
function DeleteConfig(index){
	if(confirm("<%write(com_deleteTip);%>")){
		DDNSList.delstr.value=Profiles[index];
		DDNSList.action="/goform/ConfigDDNSListDel";
	    DDNSList.submit();
	}
}
function delAll(){
	if(confirm("<%write(com_deleteAllTip);%>")){
		DDNSList.action="/goform/ConfigDDNSListDelAll";
		DDNSList.submit();
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
  <td><script language="javascript">showSecondTitle(_("<%write(DDNS_statusList);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td ><form name="DDNSList" method="post" action="/goform/ConfigDDNSListDel">
<script language="javascript">                                                  
                var arrayThreeMenu=new Array(                                   
                                                "80%",                          
                                                2,                              
                            new Array("<%write(DDNS_status);%>",'DDNSAdvance.asp',1,1),          
                            new Array("<%write(DDNS_setting);%>",'DDNS_edit.asp',1,0) 
                );                                                              
                showThreeMenu();                                                
        </script>                               
    <table id=grouplist style="display" width="80%"  border="0" align="center" cellpadding="0" cellspacing="0" height="350">
  <tr>
    <td align="center" class="three_td" valign="top"><table id=grouplist style="display" width="600"  border="0" align="center" cellpadding="0" cellspacing="0">
    <tr><td colspan="4" height="40">&nbsp;</td></tr>

		     <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="600" height="150" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
		<script language="javascript">document.write(_("<%write(com_select);%>"));</script></div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> 
			   <td width="90" valign="top" align="right"><input type="button" value="<%write(com_addNew);%>" class="input-button" onClick="EditConfig(0)" id="add"></td>
			  <td width="95" valign="top" align="right"><input type="button" value="<%write(com_delAll);%>" class="input-button" onClick="delAll()" id="delall"></td>
			 <td width="45" valign="top" align="right"><input type="submit" value="<%write(com_delete);%>" class="input-button" onClick="return(genData(DDNSList,0,frame12,Profiles,'DDNS'))" id="del"></td>
		         <td width="45" valign="top" align="right"><input type="button" class="input-button" id="update state" onClick="location.reload()" value="<%write(update_status);%>"></td>
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

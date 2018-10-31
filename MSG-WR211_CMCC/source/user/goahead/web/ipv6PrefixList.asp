<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("radvd");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

var strSecondMenuFocus="<%write(ipv6_PreList);%>";
<%outputIpv6PrefixListConfig(); %>

recsPerPage[0] =10;
lineButton[0]=10;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<%write(radvd_prefix);%>";
var titles1=new Array(3);
titles1[0]="<%write(ipv6_prefix);%>";
titles1[1]="<%write(ipv6_PreLen);%>";
titles1[2]="<%write(pre_life);%>";
titles1[3]="<%write(Adv_auto);%>";

arrayData1=new Array(DesPrefix,DesPrefixLength,PrefixstaleTime,AdvAutoEnabled);
sortable[0]=new Array(1,1,1,1);
sortType[0]=new Array(TYPE_IP,TYPE_NUM,TYPE_NUM,TYPE_STR);
arrayCount[0]=4;

<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,600,titles1,4,arrayData1,1,1);
}
/******************list***********************/
function EditConfig(val,i){
	if(val==0){ 
		Ipv6Prefix_List.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert("<%write(com_editAlert);%>");
			return;
		}
	}
	else Ipv6Prefix_List.id.value=DesPrefix[i];
	Ipv6Prefix_List.action="ipv6PrefixList_edit.asp";
	Ipv6Prefix_List.submit();
}
function DeleteConfig(index){
	if(confirm("<%write(com_deleteTip);%>")){
		Ipv6Prefix_List.delstr.value=DesPrefix[index];
		Ipv6Prefix_List.action="/goform/ConfigIpv6PrefixListDel";
	    Ipv6Prefix_List.submit();
	}
}
function delAll(){
	if(confirm("<%write(com_deleteAllTip);%>")){
		Ipv6Prefix_List.action="/goform/ConfigIpv6PrefixListDelAll";
		Ipv6Prefix_List.submit();
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
  <td><script language="javascript">showSecondTitle(_("<%write(ipv6_PreList);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td ><form name="Ipv6Prefix_List" method="post" action="/goform/ConfigIpv6PrefixListDel">
<script language="javascript">                                                  
                var arrayThreeMenu=new Array(                                   
                                                "80%",                          
                                                2,                              
                                new Array("<%write(radvd);%>",'radvd.asp',1,0),          
                                new Array(_("<%write(radvd_prefix);%>"),'ipv6PrefixList.asp',1,1) 
                );                                                              
                showThreeMenu();                                                
        </script>                               
    <table id=grouplist style="display" width="80%"  border="0" align="center" cellpadding="0" cellspacing="0" height="420">
  <tr>
    <td align="center" class="three_td" valign="top"><table id=grouplist style="display" width="600"  border="0" align="center" cellpadding="0" cellspacing="0">
    <tr><td colspan="4" height="40">&nbsp;</td></tr>

		     <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="600" height="265" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
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
			 <td width="45" valign="top" align="right"><input type="submit" value="<%write(com_delete);%>" class="input-button" onClick="return(genData(Ipv6Prefix_List,0,frame12,DesPrefix,'<%write(radvd_prefix);%>'))" id="del"></td>
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

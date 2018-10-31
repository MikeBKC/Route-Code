<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("vlanConfig"); %>
<% langTran("commonWords"); %>
<% langTran("common"); %>
<title><% write(utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">

<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

//交互begin
<%
	aspVlanConfig();
%>
//交互end

var flag = 0;
recsPerPage[0] =5;
lineButton[0]=0;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]=_("<%write(vlanList);%>");
var titles1=new Array();
titles1[0]=_("<%write(dhcpPoolName);%>");
titles1[1]=_("<%write(poolSt);%>");
titles1[2]=_("<%write(poolVid);%>");
titles1[3]=_("<%write(startip);%>");
titles1[4]=_("<%write(subnet);%>");

arrayData1=new Array(poolNames, poolSts, poolVids, beginIp, netMask);
sortable[0]=new Array(1,1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_STR,TYPE_NUM,TYPE_IP,TYPE_IP);
arrayCount[0]=5;




function dsplist(index,list)
{
	if(index==0) dspList(index,list,600,titles1,titles1.length,arrayData1,1,1);
}
var modify=0;
var modifyIndex=-1;
var w=0;
function EditConfig(val,i){
	if(val==0)	//添加
		{ 
		if(eval(maxrecs[0]-totalrecs)<1)
		{
				alert(_("<%write(warning7);%>"));
					return;
		}
		}

			else DhcpPool_List.oldName.value = poolNames[i]


	 DhcpPool_List.action="vlanEdit.asp";
         DhcpPool_List.submit();
}


function DeleteConfig(index){
	if(confirm(_("<%write(warning3);%>"))){
		DhcpPool_List.delstr.value=poolNames[index];
//		alert(DhcpPool_List.delstr.value);
//		return;
		DhcpPool_List.action="/goform/formVlanConfigDel";
	    DhcpPool_List.submit();
	}
}

function delAll(){
	if(confirm(_("<%write(warning4);%>"))){
		DhcpPool_List.action="/goform/formVlanConfigDelAll";
		DhcpPool_List.submit();
	}
}

function customDel()
{
	genData(DhcpPool_List,0,frame12,poolNames,"<%write(APpermissions);%>");
//	alert(DhcpPool_List.delstr.value);
//	return;
	DhcpPool_List.action="/goform/formVlanConfigDel";
	DhcpPool_List.submit();
}

function init()
{

	for(var i=0; i<poolSts.length;i++)
	{       
		if(poolSts[i] == "1")
		{     
		    poolSts[i] = "<%write(poolEnable);%>";

		}else{
			poolSts[i] = "<%write(poolDisable);%>";
		}
	}

	frame11.location.href = "_listhead1.htm";
	frame12.location.href = "_list1.htm";
}




//-->
</script>
</head>
<body onLoad="init()" onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0"><!--最外层1-->
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<%write(vlanList);%>");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"80%",
						2,
						new Array("<%write(vlanList);%>",'vlanConfig.asp',1,1),
						new Array("<%write(vlanEdit);%>",'vlanEdit.asp',1,0)
		);
		showThreeMenu();
	</script>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
    <td align="center"  class="three_td" valign="top">
<form name="DhcpPool_List" method="post" action="/goform/formVlanConfigDel">
   <table id=grouplist style="display"  border="0" align="center" cellpadding="0" cellspacing="0">
		<tr><td colspan="4" height="40">&nbsp;</td></tr>   
		<tr>
		      <td colspan="4">
			<iframe src="_listhead1.htm" name="frame11"  align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr>
		      <td colspan="4">
			<iframe src="_list1.htm" name="frame12"	id="framelist" align="MIDDLE" width="600"  marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>

                    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">


			  <tr>
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
	                        <input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'))>
		       <script language="javascript">document.write("<% write(allSelect); %>");</script>/<script language="javascript">document.write("<% write(allUnselect); %>");</script>
		          </div>
			  </td>
			  <td ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="oldName"><input type="hidden" name="oldMac"></td> 
<td width="80" valign="top" align="right"><input type="button" class="input-button" id="addEntry" onClick="EditConfig(0)" value="<%write(addNew);%>"></td>
<td width="30" valign="top" align="right" ><input type="submit" class="input-button" id="del" onClick="return(genData(DhcpPool_List,0,frame12,poolNames,'<%write(vlanInterface);%>'))" value="<%write(del);%>"></td>
<td width="70" valign="top" align="right"><input type="button" class="input-button"  id="delAllEntry" onClick="delAll()" value="<%write(delAll);%>"></td>
			
			  </tr>
		          </table>
		       </td>
		    </tr>

			<tr><td colspan="4" height="20">&nbsp;</td></tr>
		  </table>
  </form>
</td></tr>
</table>

  </td></tr><!--<script language="javascript">init();</script>-->
</table>
<!--////end-->
  <td width="15"></td>
  </tr>
</table>
</body>
</html>

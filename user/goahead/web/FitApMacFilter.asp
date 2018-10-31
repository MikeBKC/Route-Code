<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("FitApMacFilter"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

//交互变量

var errorstr="";var totalrecs=0;var max_totalrecs=100;
var macConfName=new Array();
var SZname=new Array();


<%aspOutPutFitApMacFilterConf();%>

//end

recsPerPage[0] =10;
lineButton[0]=0;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<% write(MACAddrFilterList); %>";
var titles1=new Array();
titles1[0]="<% write(mf_confName); %>";
titles1[1]="<% write(mf_SZ); %>";
titles1[2]="<% write(mf_rule); %>";


arrayData1=new Array(macConfName,SZname,Rules);
sortable[0]=new Array(1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_STR, TYPE_STR);
arrayCount[0]=3;

<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,650,titles1,titles1.length,arrayData1,1,1);
}
/******************list***********************/
function EditConfig(val,i){
	if(val==0){ 
		WlanMacFilter_List.oldName.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert("<% write(mf_MaxConfErr); %>");
			return;
		}
	}
	else WlanMacFilter_List.oldName.value=macConfName[i];
	WlanMacFilter_List.action="FitApMacFilter_edit.asp";
	WlanMacFilter_List.submit();
}
function copyData(formName, index, list, array, tips, flagstr) //拷贝MAC地址
{
    var chks = list.document.getElementsByName("chklist");
//	alert(chks);
    if (typeof(flagstr) == "undefined" || trimstr(flagstr) == '') 
	{
		flagstr = ',';
	}

	var checkNum=0;
    formName.oldMac.value = "";
  
    for(var i=0;i<chks.length;i++){
		if(chks[i].checked == 1)
		{
			for(var j=0;j<array[i].length;j++)
			{
				formName.oldMac.value =formName.oldMac.value + array[i][j] + flagstr;
	  //	    alert("arrayi["+i+"]["+j+"]=="+array[i][j]);
	  //	    alert("value=="+formName.oldMac.value);
			}
		checkNum++;
	    }
    }

    if (formName.oldMac.value != "") {
	  //if((formName.oldMac.value.split(',').length) > 2){
	  if(checkNum > 1){
	      alert("<%write(mf_selectOne);%>");
	      return false;    
	  }
	  else{
		WlanMacFilter_List.action="FitApMacFilter_edit.asp";
		WlanMacFilter_List.submit();
	        return true;
	  } //判断是否选择checkbox
	    
    }
    else {
        alert("<%write(mf_select1);%>" + tips + "<%write(mf_select2);%>");
        return false;
    }
}

function DeleteConfig(index){
	if(confirm("<% write(SureDel); %>")){
		WlanMacFilter_List.delstr.value=macConfName[index];
		WlanMacFilter_List.action="/goform/formApMacFilterDel";
	    WlanMacFilter_List.submit();
	}
}
function delAll(){
	if(confirm("<% write(SureDelAll); %>")){
		WlanMacFilter_List.action="/goform/formApMacFilterDelAll";
		WlanMacFilter_List.submit();
	}
}
function init()
{
//	WlanMacFilter_List.ExceptMSNEnable.checked=(exceptMSNEnable=="on"?true:false);
}
-->
</script>
</head>
<body onbeforeunload="logOut()" background="<% write(com_bodyMain); %>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<% write(WirelessMacFilter); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr>
    <td >
    
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"80%",
						2,
						new Array("<%write(mf_list);%>",'FitApMacFilter.asp',1,1),
						new Array("<%write(mf_conf);%>",'FitApMacFilter_edit.asp',1,0)
		);
		showThreeMenu();
	</script>
	<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
    <td align="center" class="three_td">
	<table border="0" align='center' width="80%"><tr><td class="gapTd"></td><td><!--下面部分表单外面的表格5-->
	<form name="WlanMacFilter_List" method="post" action="/goform/formApMacFilterDel">
   <table id=grouplist style="display" width="100%"  border="0" align="center" cellpadding="0" cellspacing="0">
    <tr><td  height="18">&nbsp;</td></tr>
		 <tr>
		      <td><iframe src="_listhead1.htm" name="frame11"  scrolling="no" align="MIDDLE" width="650" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		 <tr>
		      <td><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="650" height="248" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="0">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
		<script language="javascript">document.write("<% write(Selecte); %>");</script></div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="oldName"><input type="hidden" name="oldMac"></td> 
			  <td width="40" valign="top" align="right"><input type="button" value="<%write(mf_copy);%>" class="input-button" onClick="return(copyData(WlanMacFilter_List,0,frame12, macData,'<% write(WirelessMacFilter); %>'))" id="copy"></td>
			   <td width="75" valign="top" align="right"><input type="button" value="<% write(com_addNew); %>" class="input-button" onClick="EditConfig(0)" id="add"></td>
			  <td width="85" valign="top" align="right"><input type="button" value="<% write(com_delAll); %>" class="input-button" onClick="delAll()" id="delall"></td>
			 <td width="35" valign="top" align="right"><input type="submit" value="<% write(com_delete); %>" class="input-button" onClick="return(genData(WlanMacFilter_List,0,frame12,macConfName,'<% write(WirelessMacFilter); %>'))" id="del"></td>
	          <td width="35" valign="top" align="right"><input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('rhelpinfo.htm')" id="help"></td>
			  </tr>
		      </table></td>
		    </tr>
			<tr><td height="10">&nbsp;</td></tr>
		  </table>
  </form>
  </td><td class="gapTd"></td>
  </tr>
  </table>
  </td></tr>
</table>
</td></tr>
</table></td>
  <td width="15"></td><script language="javascript">init();</script>
  </tr>
</table>
</body>
</html>

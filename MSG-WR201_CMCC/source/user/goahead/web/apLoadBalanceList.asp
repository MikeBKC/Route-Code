<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("apLoadBalance");%>
<%langTran("commonWords");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" ContentType="text/xml"> 

var strSecondMenuFocus="AP<%write(load_balance);%>";



/*
var APNames=new Array();var Serialnums = new Array();var Macs = new Array();
var state = new Array();var ThresValue = new Array();var DiffValue = new Array();
var Detail = new Array();var ApCounts = new Array();var instIndexs =new Array();
var ApCounts1 = new Array();

var totalrecs=0;
APNames[0]="merry";Serialnums[0]="1234567";Macs[0]="1C:6F:65:ED:89:EB";state[0]="开启";ThresValue[0]="10";Detail[0]="查看";
APNames[1]="merry";Serialnums[1]="1234567";Macs[1]="1C:6F:65:ED:89:EB";state[1]="开启";ThresValue[1]="10";Detail[1]="查看";
ApCounts[0]=["qw 22232 ewewe","23 323424  adaeq"];instIndexs[0]="ID1"; DiffValue[0]="3";
ApCounts[1]=["qw 22232 ewewe","23 323424  adaeq"];instIndexs[1]="ID2"; DiffValue[1]="3"; 
var max_totalrecs=5;var errorstr="";

var totalrecs =2;
*/
var ApCounts1 = new Array();




<%aspOutputApLbListData();%>


recsPerPage[0] =5;
lineButton[0]=0;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="AP<%write(load_balance_list);%>";
var titles1=new Array(3);
titles1[0]="<%write(load_balance_g_n);%>";
titles1[1]="<%write(com_status);%>";
titles1[2]="AP<%write(number);%>";
//titles1[3]="阈值";
//titles1[4]="差值";

for(var i =0;i<ApCounts.length;i++){	
	 ApCounts1[i] = "<span title='"+ApCounts[i].join("&#13;")+"'>"+ApCounts[i].length+"</span>";
}
for(var j=0;j<state.length;j++){
	if(state[j]==0){
		state[j]="<%write(disable);%>";
	}else{
		state[j]="<%write(enable);%>";
	}
}
arrayData1=new Array(APNames,state,ApCounts1);
sortable[0]=new Array(1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_STR,TYPE_STR);
arrayCount[0]=3;
function dsplist(index,list)
{
	if(index==0) dspList(index,list,600,titles1,3,arrayData1,1,1);
}
/******************list***********************/
function EditConfig(val,i){

	if(val==0){ //添加
		apLoadBalance_List.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert("<%write(com_editAlert);%>");
			return;
		}
	}
    else{ 
        apLoadBalance_List.id.value=APNames[i];
    }
	apLoadBalance_List.action="apLoadBalance_edit.asp";
	apLoadBalance_List.submit();

}
function DeleteConfig(index){
	if(confirm("<%write(com_warning3);%>")){
		apLoadBalance_List.delstr.value=APNames[index];
		apLoadBalance_List.action="/goform/formApLbListDel";
	   apLoadBalance_List.submit();
	}

}
function delAll(){
if(confirm("<%write(com_warning4);%>")){
		apLoadBalance_List.action="/goform/formApLbListDelAll";
		apLoadBalance_List.submit();
	}

}

function init()
{
	apLoadBalance_List.GlobalEnable.checked= (uttLoadBalanceEn== "on");
}

function save()
{
	apLoadBalance_List.action="/goform/formApLbGlobalConfig";
	apLoadBalance_List.submit();
}

</script>
</head>

<body background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("AP<%write(load_balance);%>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td ><form name="apLoadBalance_List" method="post" action="/goform/formApLbListDel">
	<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center" class="style7">
   <table id=grouplist style="display"   border="0" align="center" cellpadding="0" cellspacing="0"><tr><td colspan="4" height="20">&nbsp;</td></tr>


	<tr>
	  <td colspan="4">
	    <table width="100%">
	      <tr>
	        <td width="42%" height="25" align="right" valign="middle" id="attention1"><%write(start_load_balance);%></td>
		<td width="2%" height="25"><input type="checkbox" name="GlobalEnable" id="GlobalEnable"></td>
		<td align="left"  height="25" valign="middle">&nbsp;&nbsp;<input type="button" class="input-button" 

value="<%write(com_save);%>" onClick="save()"></td>
	      </tr>
	    </table>
          </td>
	</tr>		
	<tr>
	  <td colspan="4" height="20">
	    <table width="100%" border="0" cellspacing="0" cellpadding="0">
	      <tr>
		<td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
	      </tr>
	    </table>
	  </td>
	</tr>
	<tr height="25"><td></td></tr>



		     <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="600" height="140" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
<!--              
              <input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"><input type="hidden" name="AllowID"><input type="hidden" name="Allow">
-->
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
		<script language="javascript">document.write("<%write(com_select);%>");</script></div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> 
			   <td width="90" valign="top" align="right"><input type="button" value="<%write(com_addNew);%>" class="input-button" onClick="EditConfig(0)" id="add"></td>
			  <td width="95" valign="top" align="right"><input type="button" value="<%write(com_delAll);%>" class="input-button" onClick="delAll()" id="delall"></td>
			 <td width="45" valign="top" align="right"><input type="submit" value="<%write(com_delete);%>" class="input-button" onClick="return(genData(apLoadBalance_List,0,frame12,APNames,'AP<%write(load_balance_list);%>'))" id="del"></td>
              <td width="4%"><input type="button" value="<%write(help);%>" class="input-button" onClick="window.open('<% write(com_helppage);%>#apLoadBalanceList')" id="utthelp"></td>
			  </tr>
		      </table></td>
		    </tr><tr><td colspan="4" height="20">&nbsp;</td></tr>
		  </table>
		  </td></tr></table>
  </form></td>
  </tr>
</table></td>
  <td width="15"><script language="javascript">init();</script></td>
  </tr>
</table>

</body>
</html>

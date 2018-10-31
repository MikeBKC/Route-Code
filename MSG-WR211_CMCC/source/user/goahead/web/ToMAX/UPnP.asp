<html >
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<!-- TemplateBeginEditable name="doctitle" -->
<%langTran("common");%>
<%langTran("UPnP");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
//Butterlate.setTextDomain("upnp");
//Butterlate.setTextDomain("common");
var errorstr="";
<% aspOutUpnpTableAndConfig();%>

var  upnpEnbleOld;

//function initTranslation()
//{
//	var e;
//	e = document.getElementById("uttsave");
//	e.value= _("save");
//	e= document.getElementById("refresh");
//	e.value= _("refresh");
//	e= document.getElementById("start upnp");
//	e.innerHTML= _("start upnp");
//}
function init(){
	var f=UPnP_Config;
	f.upnpEnble.checked=(upnpEnables == "1" ? true:false);
	f.upnpEnbleOld.value = upnpEnables;
	
}

maxrecs[0]=totalrecs;   //列表最大显示量

var strSecondMenuFocus="UPnP";

function save(){
	 
		var f=UPnP_Config;
	 	f.upnpEnblew.value	= (f.upnpEnble.checked == true ? "1":"0");
		f.submit();
	 	
}


var ids=new Array();
for(i=0;i < totalrecs;i++){
	ids[i]=i+1;
}


currpage[0]=1;
recscount[0]=totalrecs;
recsPerPage[0] =10;
lineButton[0]=10;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]=_("<%write(upn_upnpList);%>");
var titles1=new Array(7);
titles1[0]=_("<%write(upn_xuhao);%>");
titles1[1]=_("<%write(upn_innerIP);%>");
titles1[2]=_("<%write(upn_innerPort);%>");
titles1[3]=_("<%write(com_protocol);%>");
titles1[4]=_("<%write(upn_exterIP);%>");
titles1[5]=_("<%write(upn_exterPort);%>");
titles1[6]=_("<%write(com_descript);%>");
arrayData1=new Array(ids,upnpIntIp,upnpIntPort,upnpProt,upnpRemoteIp,upnpExtPort,description);
sortable[0]=new Array(1,1,1,1,1,1,1);
sortType[0]=new Array(TYPE_NUM,TYPE_STR,TYPE_NUM,TYPE_CHINESE,TYPE_STR,TYPE_STR,TYPE_STR);
arrayCount[0]=7;

function dsplist(index,list)
{
	if(index==0) dspList(index,list,920,titles1,7,arrayData1,0,0);
}

</script>

</head>

<body onbeforeunload="logOut()">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <tr>
  <td width="15"></td>
  <td><script language="javascript">showSecondTitle('UPnP');</script>
  <table width="100%" border="0" height="500" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
  <table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
	<table border="0" width="80%" align='center'>
	<form action="/goform/formConfigUpnp" name="UPnP_Config" method="post">
	<tr><td colspan="3" height="20">&nbsp;</td></tr>
	<tr>
	    <td height="20" width="40%" align="right" id="start upnp"><%write(upn_start);%></td>
		<td align="left" width="5%"><input type="checkbox" name="upnpEnble"></td>
		<td align="left"><input type="hidden" name="upnpEnblew"><input type="hidden" name="upnpEnbleOld"><input type="button" class="input-button" id="uttsave" value="<%write(com_save);%>"  onClick="save()">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#UPnP')"></td>
	</tr>
  <tr><td colspan="3" align="center" height="40"><table width=920 border="0" cellspacing="0" cellpadding="0">
  <tr>
   <td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
  </tr>
</table>
</td></tr><input type="hidden" value="1" name="re">
  </form></table></td>
  </tr>
  <tr><td>
<table border="0"  align='center' width="80%">
<form name="UPnP_List" method="post" action="">
   <table id=grouplist style="display"  border="0" align="center" cellpadding="0" cellspacing="0">
		    <tr bgcolor="c0ddff">
		      <td colspan="4">
			<iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="920" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4">
			<iframe src="_list1.htm" name="frame12"	id="framelist" align="MIDDLE" width="920" height="255" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6">&nbsp;</td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> 
			  <td width="40" valign="top" align="right" ><input type="button" value="<%write(com_fresh);%>" class="input-button" onClick="location.reload()" id="refresh"></td>
			
			  </tr>
		      </table></td>
		    </tr>
		  </table>
  </form>
</table></td></tr>
</table> 
  </td></tr><script language="javascript">init();</script>
</table></td>
<!--////end-->
  <td width="15"></td>
  </tr>
</table>
</body>
</html>


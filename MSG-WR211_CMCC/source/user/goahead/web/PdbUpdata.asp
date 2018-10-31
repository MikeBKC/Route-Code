<html >
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<!-- TemplateBeginEditable name="doctitle" -->
<% langTran("PdbUpdata"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
 
<% getSysLang(); %>
var errorstr="";
var strSecondMenuFocus="<% write(PDB); %>";
<% aspformPdbUpdata();%>

for(i=0;i < totalrecs;i++){
	if(types[i] == "1")types[i] = "<% write(IM); %>";
	if(types[i] == "2")types[i] = "<% write(P2P); %>";		
	if(types[i] == "3")types[i] = "<% write(STOCK); %>";
	if(types[i] == "4")types[i] = "<% write(NetVideo); %>";
	if(types[i] == "5")types[i] = "<% write(NetGame); %>";
	if(types[i] == "6")types[i] = "<% write(ShoppingSite); %>";
	if(types[i] == "7")types[i] = "<% write(SocialNetSite); %>";
	if(types[i] == "8")types[i] = "<% write(WEBGAME); %>";
	if(types[i] == "9")types[i] = "<% write(Mail); %>";
	if(types[i] == "10")types[i] = "<% write(Forum); %>";
	if(types[i] == "11")types[i] = "<% write(Others); %>";
}

function init(){
	var f=formPdbUpdata;
}
/*var policyNames = new Array(); var types = new Array(); var descriptions = new Array(); 
policyNames[0] = "CNC"; types[0] = "<% write(Route); %>"; descriptions[0] = "网通静态路由"; 
policyNames[1] = "TEL"; types[1] = "<% write(Route); %>"; descriptions[1] = "电信静态路由"; 
policyNames[2] = "QQ"; types[2] = "<% write(Firewall); %>"; descriptions[2] = "禁止QQ";*/


function updatePolicy(index, i) {
	formPdbUpdata.policyNames.value = policyNames[i];
	formPdbUpdata.action = "/goform/formPdbUpConfig";
	formPdbUpdata.submit();
}


function goall(index,list)
{
 	formPdbUpdata.policyNames.value = "All";
	formPdbUpdata.action = "/goform/formPdbUpConfig";
	formPdbUpdata.submit();
}

maxrecs[0]=totalrecs;   //列表最大显示量
currpage[0]=1;
recscount[0]=totalrecs;
recsPerPage[0] =10;
lineButton[0]=10;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]=_("<% write(SLibInfoList); %>");
var titles1=new Array(3);
titles1[0]=_("<% write(PName); %>");
titles1[1]=_("<% write(PType); %>");
titles1[2]=_("<% write(Pexplain); %>");
 

arrayData1=new Array(policyNames,types,descriptions);
sortable[0]=new Array(1,1,1);
sortType[0]=new Array(TYPE_CHINESE,TYPE_CHINESE,TYPE_CHINESE);
arrayCount[0]=3;

function dsplist(index,list)
{
	if(index==0) dspList(index,list,600,titles1,3,arrayData1,0,0);
}

 

</script>

</head>

<body onbeforeunload="logOut()" background="<% write(com_bodyMain); %>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <tr>
  <td width="15"></td>
  <td><script language="javascript">showSecondTitle('<% write(SLib); %>');</script>
  <table width="100%" border="0" height="500" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
   <script language="javascript">showright(rightstr,80);</script>
<tr valign="top">
    <td>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr><form name="formPdbUpdata" method="post" action="">
    <td align="center" class="style7">
   <table id=grouplist style="display" width="80%"  border="0" align="center" cellpadding="0" cellspacing="0"><tr><td colspan="4" height="20">&nbsp;</td></tr>
		     <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no"  align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="600" height="260" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    
		      <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6">&nbsp;</td>
 		 <td align="right"><input type="button" class="input-button" value="<% write(FreshAll); %>" onClick="goall()" id="updataAll">
			<input type="hidden" name="policyNames" size=10 ><input type="button" value="<% write(com_fresh); %>" class="input-button" onClick="location.reload()" id="refresh">&nbsp;&nbsp;&nbsp;<input type="button" value="<% write(com_help); %>" class="input-button" onClick="window.open('<% write(com_helppage); %>#PdbUpdata')"></td>
			
			  </tr>
		      </table></td>
		     
		    </tr><tr><td colspan="4" height="20">&nbsp;</td></tr>
		  </table> 
  </td>  </form></tr><script language="javascript">init();</script>
</table></td></tr></table>
<!--////end-->
  <td width="15"></td>
  </tr>
</table>
</body>
</html>


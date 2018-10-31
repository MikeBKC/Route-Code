<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<% langTran("FtpServer"); %>
<% langTran("common"); %>
<title><% write(FtpTitle); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" > 
 

var ftpEnable="0";
var WANEnable="0";
var ftp_port=21;

var names=new Array();
var zones=new Array();
var dirctorys=new Array();
var totalrecs = 0;
var errorstr="";


<%aspOutputFtpServer();%>


var strSecondMenuFocus=""

maxrecs[0]=max_totalrecs;
currpage[0]=1;
recsPerPage[0] = 10;
lineButton[0]=0;
recscount[0]=totalrecs;
//var maxrecs=new Array();
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<% write(ftpShareList); %>";
var titles1=new Array(3);
titles1[0]="<% write(name); %>";
titles1[1]="<% write(partition); %>";
titles1[2]="<% write(folderPosition); %>";

 
arrayData1=new Array(names,zones,dirctorys);
sortable[0]=new Array(1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_STR,TYPE_STR);
arrayCount[0]=3;
 

function dsplist(index,list)
{
	if(index==0) dspList(index,list,600,titles1,3,arrayData1,1,1);
}
/******************list***********************/
function init(){
	var f=FtpServerGloable_Config;
	f.FTPServerEnable.checked=(ftpEnable=="0"?false:true);
	f.WANEnable.checked=(wanEnable=="0"?false:true);
	f.ftpPort.value=ftp_port;
}
function save(){
	var f=FtpServerGloable_Config;
	if (!checkNumber(f.ftpPort, 1, 65535, "<% write(ftpPort); %>",0, 0)) {
	    return;
	}
	FtpServerGloable_Config.action="/goform/formFtpServerGloable";
	FtpServerGloable_Config.submit();
}
function EditConfig(val,i){
	if(val==0) WlanMacFilter_List.id.value="";
	else
	{
	    WlanMacFilter_List.id.value=names[i];
	}    
	WlanMacFilter_List.action="FtpServer_edit.asp";
	WlanMacFilter_List.submit();
}
function DeleteConfig(index){
	if(confirm("<% write(sureDel); %>")){
		WlanMacFilter_List.delstr.value=names[index];
		WlanMacFilter_List.action="/goform/formFtpServerDirDel";
	    WlanMacFilter_List.submit();
		
	}
}
function delAll(){
	if(confirm("<% write(sureDelAll); %>")){
		WlanMacFilter_List.action="/goform/formFtpServerDirDelAll";
		WlanMacFilter_List.submit();
	}
}
</script>
</head>

<body onLoad="init()"onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td>
  	<script language="javascript">showSecondTitle("<% write(ftpServer); %>");</script>
  	<table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   		<script language="javascript">showerror(errorstr,80);</script>
  		<tr valign="top">
    		<td>
  				<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
				<p>&nbsp;</p>
				<tr>
    				<td align="center"  class="style7">
                    	<table width="100%" border="0" cellspacing="0" cellpadding="0">
  						<tr>
    					<td>
                        	<table border="0" width="80%" align='center'>
                            	<form action="" onSubmit="return " name="FtpServerGloable_Config" method="post">
                                	<tr><td colspan="3" height="20">&nbsp;</td></tr>
									<tr>
                                      <td width="45%" height="25" align="right" valign="middle" id="start MAC filter"><% write(enableFtp); %></td>
                                      <td width="2%" height="25">&nbsp;</td>
                                      <td align="left"  height="25" valign="middle"><input type="checkbox" name="FTPServerEnable"></td>
                                    </tr>
                                    <tr>
                                      <td  height="25" align="right" valign="middle" id="start MAC filter"><% write(allowWan); %></td>
                                      <td height="25">&nbsp;</td>
                                      <td align="left"  height="25" valign="middle"><input type="checkbox" name="WANEnable"></td>
                                    </tr>
									 <tr>
                                      <td height="25" align="right" valign="middle" id="start MAC filter"><% write(ftpPort); %></td>
                                      <td height="25">&nbsp;</td>
                                      <td align="left"  height="25" valign="middle"><input type="text" name="ftpPort" class="input" size="3" value="21"><% write(ftpPrompt); %></td>
                                    </tr>
                                    <!--
                                     <tr>
                                      <td height="25" align="right" valign="middle" id="start MAC filter">WAN口地址</td>
                                      <td height="25">&nbsp;</td>
                                      <td align="left"  height="25" valign="middle"><span id="wanadd"></span></td>
                                    </tr>
                                    -->
                                
                                    <tr><td colspan="3" height="20" align="center"><input type="button" class="input-button" value="<% write(com_save); %>" onClick="save()"/>&nbsp;&nbsp;&nbsp;&nbsp;<input type="reset" class="input-button" value="<% write(com_reset); %>" />&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_help); %>" onClick="window.open('<% write(com_helppage); %>#FtpServer')"/></td></tr>
                                    <tr><td colspan="3" align="center" height="40"><table width=600 border="0" cellspacing="0" cellpadding="0">
                                          <tr>
                                           <td background="images/dot.gif"><img src="images/space.gif" width="8" height="1"></td>
                                          </tr>
                                        </table>
                                        </td></tr>
                                        
                                                                            
								</form>
                                
                                
							</table>
                        </td>
                        </tr>
                        
                        <tr><td>
<table border="0"  align='center' width="80%">
<form name="WlanMacFilter_List" method="post" action="/goform/formFtpServerDirDel">
   <table id=grouplist style="display"  border="0" align="center" cellpadding="0" cellspacing="0">
		    <tr>
		      <td colspan="4">
			<iframe src="_listhead1.htm" name="frame11" scrolling="no"  align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr>
		      <td colspan="4">
			<iframe src="_list1.htm" name="frame12"	id="framelist"  scrolling="no" align="MIDDLE" width="600" height="250" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick="selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));">
		<script language="javascript">document.write("<% write(com_select); %>");</script></div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> 
			  <td width="90" valign="top" align="right"><input type="button" value="<% write(addFolder); %>" class="input-button" onClick="EditConfig(0)" id="add"></td>
			  <td width="110" valign="top" align="right"><input type="button" value="<% write(delAll); %>" class="input-button" onClick="delAll()" id="delall"></td>
			  <td width="40" valign="top" align="right"><input type="submit" value="<% write(del); %>" class="input-button" onClick="return(genData(WlanMacFilter_List,0,frame12,names,'<% write(ftpListInfo); %>'))" id="del"></td>
			  </tr>
		      </table></td>
		    </tr>
			<tr><td colspan="4" height="20">&nbsp;</td></tr>
		  </table>
  </form>
</table></td></tr>

                        
                        </table>
                     </td>
                 </tr>
                 </table>

   			</td>
   <td width="15"></td>
   </tr>
 </table>
</body>
</html>

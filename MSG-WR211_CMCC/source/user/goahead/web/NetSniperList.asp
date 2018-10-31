<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<% langTran("NetSniper"); %>
<% langTran("common"); %>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>

</head>
<style type="text/css">
html,body{ height:100%;}
</style>
<script type="text/javascript">
var strSecondMenuFocus="<%write(NetSniperList);%>";
var ipAdds = new Array();
var remainTimes = new Array();
var indexID=new Array();
var totalrecs=0;var max_totalrecs=10;
<%aspNetSniperHostList();%>
var errorstr="";
max_totalrecs=totalrecs;
recsPerPage[0] =10;
lineButton[0]=0;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<%write(NetSniperList);%>";
var titles1=new Array(2);
//titles1[2]="ID";
titles1[0]="<%write(IpAddr);%>";
titles1[1]="<%write(RemainTime);%>";
 
 max_totalrecs = totalrecs;
arrayData1=new Array(ipAdds,remainTimes,indexID);
sortable[0]=new Array(1,1);
sortType[0]=new Array(TYPE_STR,TYPE_STR);
arrayCount[0]=3;
 

function dsplist(index,list)
{
	if(index==0) dspList(index,list,600,titles1,2,arrayData1,1,0);
}


</script>
<body background="images/body_main.png">
	<table width="100%" height="100%" border="0" cellspacing="0" cellpadding="0">
    	<!--*****顶上间隔****-->
          <tr>
            <td colspan="3">&nbsp;</td>
          </tr>
		   <!--****中间内容****-->
          <tr valign="top">
          	<td width="15"></td>
          <!--////内容-->
          	<td height="97%"><script language="javascript">showSecondTitle("<%write(NetSniper);%>");</script>
            	<table width="100%" height="450" border="0" cellspacing="0" cellpadding="0" align="center" class="table_bg" >
   					<script language="javascript">showerror(errorstr,80);</script>
                    <tr valign="top">
                    	<td>
                        	<form name="NetSniper_List" method="post" action="/goform/formSniperHostDel">
                            <p>&nbsp;</p>
                            <script language="javascript">
                               var arrayThreeMenu=new Array(
                                                "80%",
                                                3,
                                                new Array("<%write(NetSniperList);%>",'NetSniperList.asp',1,1),
                                                new Array("<%write(NetSniperConf);%>",'NetSniperConf.asp',1,0),
                                                new Array("<%write(NetSniperNotice);%>",'NetSniperNotice.asp',1,0)
                                );
                                showThreeMenu();
                            </script>
                            <table width="80%"  border="0" align="center" cellpadding="0" cellspacing="0" height="100%" >		
                            	<tr>
    								<td align="center" class="three_td" valign="top">
                                    	<table border="0" align="center" cellpadding="0" cellspacing="0">
                                            <tr><td colspan="4" height="40">&nbsp;</td></tr>
                                            <tr bgcolor="c0ddff">
                                              <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
                                            </tr>
                                            <tr bgcolor="c0ddff">
                                              <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="600" height="265" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
                                            </tr>
                                            <tr>
                                              <td  colspan="4" valign="bottom">
                                              <table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
                                              <tr>
                                              <td width="5"></td>
                                              <td  class="style6"><div align="left">
                                                <input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
                                      <%write(CheckAll);%></div></td>
                                              <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> 
                                               <td width="90" valign="top" align="right"></td>
                                               <td width="100" valign="top" align="right"></td>
                                             <td width="40" valign="top" align="right"><input id="del" type="submit" value="<%write(Clear);%>" class="input-button" onClick="return(genData(NetSniper_List,0,frame12,ipAdds,'<%write(NetSniperList);%>'))"></td>
                                              </tr>
												<tr><td colspan="6">&nbsp;</td></tr>
										</table>
                                     		  </td>
                                		   </tr>
										</table>
                                     </td>
                                 </tr>
                             </table>   
							</form>
                        </td>
					</tr>
				</table>
                
			</td>
            <td width="15"></td>
		  </tr>
	</table>
</body>
</html>

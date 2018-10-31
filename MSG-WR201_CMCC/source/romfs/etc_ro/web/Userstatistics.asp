<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("Menu");%>
<%langTran("UserSui");%>
<title><%write(UserStatistics);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 

</head>
<style type="text/css">
.warpstatic{ width:700px;
	margin: 0 auto;
	text-align:center;
	
	}
.group { border-right:1px solid #5e5e5e; border-bottom:1px solid #5e5e5e;}
.group  td{ white-space:nowrap; border-left:1px solid #5e5e5e; border-top:1px solid #5e5e5e;background:#fff;}
.title td{ font-weight:bold;} 
</style>
<script type="text/javascript">


var title = new Array();
title[0] ="ID";
<%outputChangemodel();%>
if(ChangeModel == 0)
{
var Username = new Array();//用户名
var  IPs = new Array();//IP地址
var Macs = new Array();//MAC地址
//var AuthTypes = new Array();
var inbits = new Array();//当前上行
var outbits = new Array(); //当前下行
var rxBytes= new Array();//下载总流量
var txBytes= new Array();//上传总流量
var startTimes = new Array();//上网时间
var connectType = new Array()//连接类型
<%outputUserStatistics();%>
title[1] ="<%write(usr_username);%>";
title[2] ="<%write(com_IP);%>";
title[3] ="<%write(com_MAC);%>";
title[4] ="<%write(usr_nowBit);%>"+"</br>"+"(kbit/s)";
title[5] ="<%write(usr_totalBit);%>"+"</br>"+"(Mbit)";
title[6] ="<%write(usr_onlineTime);%>";
title[7] ="<%write(com_ConnectionType);%>";
title[8] ="<%write(usr_down);%>";
title[9] ="<%write(usr_up);%>";

var arrayData=new Array(Username,IPs,Macs,outbits,inbits,rxBytes,txBytes,startTimes,connectType);
}else{
var Macs = new Array();//MAC地址
//var AuthTypes = new Array();
var BWs = new Array();
var startTimes = new Array();//上网时间
var connectType = new Array()//连接类型
<%outputbridgeusers();%>
title[1] ="<%write(com_MAC);%>";
title[2] ="频宽";
title[3] ="<%write(usr_onlineTime);%>";
title[4] ="<%write(com_ConnectionType);%>";

var arrayData_bridge=new Array(Macs,BWs,startTimes,connectType);
}

function dspList(){
	var inner_html='<tr height="30px" class="title"><td rowspan="2" width="25px">'+title[0]+'</td><td rowspan="2">'+title[1]+'</td><td rowspan="2">'+title[2]+'</td><td rowspan="2">'+title[3]+'</td>'+
'<td colspan="2">'+title[4]+'</td>'+
        '<td colspan="2">'+title[5]+'</td>'+
         '<td rowspan="2">'+title[6]+'</td>'+
         '<td rowspan="2">'+title[7]+'</td>'+
    '</tr> <tr  height="30px"  class="title"> <td>'+title[8]+'</td><td>'+title[9]+'</td><td>'+title[8]+'</td><td>'+title[9]+'</td> </tr>';
	if(totalrecs==0){
			inner_html +='<tr><td colspan="10" align="center"><%write(usr_noNew);%></td></tr>';	
		}else{
			for (var i=0;i<totalrecs;i++){
					inner_html +='<tr height="30px"><td>'+(i+1)+'</td>';
					for(var j=0; j< arrayData.length;j++){
							//alert("arrayData["+i+"]["+j+"]"+arrayData[j][i]);
							if(j==8){
								inner_html +='<td><img src="images_Sui/'+arrayData[j][i]+'.png" /></td>';
							}else{
								inner_html +='<td>'+arrayData[j][i]+'</td>';
							}	
						}
					inner_html +='</tr>';
				}	
		}
		inner_html+=' <tr class="tab_savetd"><td colspan="10" align="right"><span style="padding-right:30px;"><input type="button" class="btn" name="back" onClick="javascript:window.location.reload();" value="<%write(com_fresh);%>"/></span></td> </tr>';
		$("#usersInfo").html(inner_html);	
	
}
function dspList_bridge(){
	var inner_html='<tr height="30px" class="title"><td width="25px">'+title[0]+'</td><td >'+title[1]+'</td><td >'+title[2]+'</td><td >'+title[3]+'</td><td>'+title[4]+'</td>'
	if(totalrecs==0){
			inner_html +='<tr><td colspan="10" align="center"><%write(usr_noNew);%></td></tr>';	
		}else{
			for (var i=0;i<totalrecs;i++){
					inner_html +='<tr height="30px"><td>'+(i+1)+'</td>';
					for(var j=0; j< arrayData_bridge.length;j++){
							//alert("arrayData["+i+"]["+j+"]"+arrayData[j][i]);
							if(j==3){
								inner_html +='<td><img src="images_Sui/'+arrayData_bridge[j][i]+'.png" /></td>';
							}else{
								inner_html +='<td>'+arrayData_bridge[j][i]+'</td>';
							}	
						}
					inner_html +='</tr>';
				}	
		}
		inner_html+=' <tr class="tab_savetd"><td colspan="10" align="right"><span style="padding-right:30px;"><input type="button" class="btn" name="back" onClick="javascript:window.location.reload();" value="<%write(com_fresh);%>"/></span></td> </tr>';
		$("#usersInfo").html(inner_html);	
}

if(ChangeModel == 0)
{
$(function(){
	dspList();
})
}else{
$(function(){
	dspList_bridge();
})
}
</script>
<body  scroll="no">
<div class="warpstatic" style="width:780px;">
<br>
 <table border="0" cellpadding="0" cellspacing="0" width="780px" id="usersInfo"  class="group" >
 </table>

</div>
<br>
</body>
</html>

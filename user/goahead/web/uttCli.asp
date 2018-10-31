 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("DnsFilter");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

var errorstr="";
<%aspOutCommandList();%>
<%aspOutMaxCommandNum();%>  //OK
var max_totalrecs=MaxCommandNum;
var linestr="--------------------------------------------------------------------------------------";
<!--
function init(){
    var f = CliForEngineerOnly;
    createCommandList();
}
function createCommandList(){
    var tbl=CliForEngineerOnly;
	if(CommandLists.length>0){
      for (i=0;i<CommandLists.length;i++)
	  {
		var option1=document.createElement("OPTION");
		option1.text=CommandLists[i];
		tbl.CmdList.options.add(option1);
		
	  }
	  option1=document.createElement("OPTION");
	  option1.text=linestr;
	  tbl.CmdList.options.add(option1);
    }
}

function del(){
    var tbl=CliForEngineerOnly;
	tbl.delstr.value="";
	 if (tbl.CmdList.selectedIndex == -1) return;
       for (var i=0;i<tbl.CmdList.options.length;i++) {
		if (tbl.CmdList.options[i].selected) {
           	if(tbl.CmdList.options[i].text!=linestr)
		   	{
		      tbl.delstr.value=tbl.delstr.value+CommandLists[i]+",";
		   	} 
		   	else
		   	{
			   alert("<%write(dns_delTip);%>");
			   return;
			}
		 }	 
	  }
	  CliForEngineerOnly.action="/goform/formConfigCliForEngineerOnlyDel";
	  CliForEngineerOnly.submit();
}


function save(){
  var tbl=CliForEngineerOnly;
  if(max_totalrecs-CommandLists.length<1){
      alert("命令条数已达最大值");
	  return ;
  }
  for(i=0;i<CommandLists.length;i++){
     if(tbl.addCommand.value.toLowerCase()==CommandLists[i].toLowerCase()){
	     alert("命令已存在");
		 return ;
	 }
  }
  if (!checkCommand(tbl.addCommand,49,"命令",0)) return ;
  if(forbidstring(tbl.addCommand.value) == true){
      alert("请不要输入halt或reboot等命令");
      return;
    }
  CliForEngineerOnly.action="/goform/formConfigCliForEngineerOnly";
  CliForEngineerOnly.submit();
}

function forbidstring(data)
{

    var re = /halt/;//最简单的正则表达式,将匹配he这个单词
    var re1 = /reboot/;
    return (re.test(data) || re1.test(data));
}

function checkCommand(cntrl,maxlen,tips,allownull)
{
	var command=cntrl.value;
	if(command=="") {
		if(!eval(allownull)) {
            alert(tips+_("不能为空！"));
            cntrl.focus();   
            return false;
        }
        else
            return true;
	}
    else if(command==0){
        alert(tips+_("不能全为0"));
		return false;
	}
    else if (command.length > maxlen) {
        alert(command.length);
		alert(tips+_("长度不能超过")+maxlen+_("位"));
		cntrl.focus();
		return false;
	} else {
		allowChar="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 .~'`/!?*${}[]=;-*&|";
		for (i=0;i<command.length;i++) {
			if (allowChar.indexOf(command.substring(i,i+1))==-1) {
				alert(tips+_("命令中存在非法字符!"));
				return false;
			}
		}
	}
	return true;
}
function DelAll(){
	if(confirm(_("确定要删除所有命令"))){
		CliForEngineerOnly.action="/goform/formConfigCliForEngineerOnlyDelAll";
        CliForEngineerOnly.submit();
	}
}
//-->
</script>
</head>


<body onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr> <td colspan="3">&nbsp;</td> </tr>
  <!--****中间内容****-->
  <tr> <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("页面命令行"));</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
<td>
<p>&nbsp;</p>

<table width="90%" border="0" cellspacing="0" cellpadding="0" align="center">

<tr>
    <td align="center"  class="three_td" valign="top">
    <table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr><td>
<table border="0" width="600" align='center'>
<form  name="CliForEngineerOnly" method="post">

<tr><td colspan="4" align="center" height="30">
	<table width=600 border="0" cellspacing="0" cellpadding="0">
  		<tr>
		   <td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
		</tr>
	</table>
	</td></tr>
   
<tr><td width="30%" height="40" align="right" valign="middle" id="command">命令</td>
	<td width="3%">&nbsp;</td>
	<td align="left" width="40%" valign="middle"><input name="addCommand" class="input"type="text" size="45" maxlength="127"></td>
	<td height="40" align="left" valign="middle"><input type="button" value="添加" onClick="save()" class="input-button" id="add"></td>
	</tr>

<tr><td width="30%" height="30" align="right" valign="middle" > </td>
	<td width="3%">&nbsp;</td>
	<td colspan="4" height="20" class="helpfont" >命令能否正确执行，取决于系统是否支持此命令以及命令语法的正确性，请注意！</td>
</tr>
  

<tr><td colspan="4" height="20"><input type="hidden" name="delstr"></td></tr>

<tr>
	<td width="20%" height="30" align="right" valign="top" id="command list">
      <p>命令列表</p></td>
	<td width="3%" height="40">&nbsp;</td>
	<td align="left" colspan="2"><select class="input" name="CmdList" size=10 multiple  id="cmdlist"  color:blue  style="font-size:10pt;SCROLLBAR-FACE-COLOR: #ffffff;  SCROLLBAR-HIGHLIGHT-COLOR: #ffffff;  #c0ddff 1px solid; SCROLLBAR-SHADOW-COLOR: #ffffff; LINE-HEIGHT: 20px; SCROLLBAR-ARROW-COLOR: #c0ddff; SCROLLBAR-TRACK-COLOR: #ffffff; SCROLLBAR-DARKSHADOW-COLOR: #c0ddff; SCROLLBAR-BASE-COLOR: #c0ddff;border:1px solid #c0ddff; width:460px " border=0  >
    </select></td>
</tr>

<tr><td colspan="4" align="center" height="30" valign="bottom" >
  <table width="180" border="0" cellspacing="0" cellpadding="0">
	<tr>
		<td  width="52%"><input type="button" value="删除" class="input-button" onClick="del()" id="delete"></td>
		<td ><input type="button" value="清空" class="input-button" onClick="DelAll()" id="delete all"></td>
	</tr>
  </table>
</td></tr>
  


</form></table></td>
</tr>
</table> 
</td>
</tr>

<script language="javascript">init();</script>
</table></td></tr>

</table></td>
<!--////end-->
<td width="15"></td>
</tr>
</table>
</body>
</html>

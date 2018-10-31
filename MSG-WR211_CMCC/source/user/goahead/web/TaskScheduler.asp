<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("TaskScheduler"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title> 
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

var strSecondMenuFocus="<% write(SchTask); %>";

var errorstr="";
var names = new Array(); var types = new Array(); var times = new Array();  var cmds = new Array();
/*var names = new Array(); var types = new Array(); var times = new Array();  var cmds = new Array();
names[0] = "<% write(Task1); %>"; times[0] = "<% write(Saturday); %> 12:12:01"; types[0] = "01"; cmds[0] = "reboot";
var totalrecs = 1;
var max_totalrecs=5;*/

<%
aspGetScheduler();
%>

recsPerPage[0] =10;
lineButton[0]=0;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]=_("<% write(PlanTaskInfoList); %>");
var titles1=new Array();
var index = 0;
titles1[0]="<% write(TaskName); %>";
titles1[1]="<% write(StartType); %>";
titles1[2]="<% write(RunTime); %>";
titles1[3]="<% write(TaskCon); %>";
//titles1[4]="<% write(Content); %>";
for(i=0;i<totalrecs;i++){
 
    times[i]=times[i].replace("* ","");
	times[i]=times[i].replace("01 ","<% write(Monday); %> ");
	times[i]=times[i].replace("02 ","<% write(Tuesday); %> ");
	times[i]=times[i].replace("03 ","<% write(Wednesday); %> ");
	times[i]=times[i].replace("04 ","<% write(Thursday); %> ");
	times[i]=times[i].replace("05 ","<% write(Friday); %> ");	
	times[i]=times[i].replace("06 ","<% write(Saturday); %> ");
	times[i]=times[i].replace("00 ","<% write(Sunday); %> ");
			  
}

for(i in cmds )
{
    if(cmds[i] == "bindall" )  cmds[i] ="<% write(Allbind); %>";
    else if(cmds[i] == "reboot")　cmds[i] = "<% write(Reboot); %>";
	if(types[i] == "01")　types[i] = "<% write(EveryWeak); %>";
    else if(types[i] == "02")　types[i] = "<% write(EveryDay); %>";
	else if(types[i] == "03")　types[i] = "<% write(EveryH); %>";
	else if(types[i] == "04")　types[i] = "<% write(EveryMin); %>";
}

arrayData1=new Array(names, types, times, cmds);

sortable[0]=new Array(1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_CHINESE,TYPE_CHINESE,TYPE_CHINESE);
arrayCount[0]=4;

function dsplist(index,list) {
    if(index==0) dspList(index,list,720,titles1,titles1.length,arrayData1,1,1,0);
}


function EditConfig(val,i){
    if(val==0){ 
	Scheduler.id.value="";
	if(eval(max_totalrecs-totalrecs)<1)	{
	    alert(_("<% write(Maxmessage); %>"));
	    return;
	}
    }
    else Scheduler.id.value=names[i];
    Scheduler.action="TaskScheduler_edit.asp";
    Scheduler.submit();
}
function DeleteScheduler(index){
    if(confirm("<% write(com_warning3); %>")){
	return(genData(Scheduler,0,frame12,names,'<% write(SchTask); %>'));

	/*
	   Scheduler.delstr.value=names[index];
	   Scheduler.action="/goform/formSchedulerUserDel";
	   Scheduler.submit();*/
    }

}
function DeleteConfig(index){

	Scheduler.delstr.value=names[index];
	Scheduler.action="/goform/formTaskDel";
	Scheduler.submit();
}
function delAll(){
    if(confirm("<% write(com_warning3); %>")){
	Scheduler.action="/goform/formTaskDelAll";
	Scheduler.submit();
    }
}
--> 
</script>
</head>
<body onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0" >
<!--*****顶上间隔****-->
<tr>
<td colspan="3">&nbsp;</td>
</tr>
<!--****中间内容****-->
<tr>
<td width="15"></td>
<!--////内容-->
<td><script language="javascript">showSecondTitle("<% write(SchTask); %>");</script>
<table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
<script language="javascript">showerror(errorstr,80);</script>
<tr valign="top">
<td><form name="Scheduler" method="post" action="/goform/formTaskDel">
<p>&nbsp;</p>
<script language="javascript">
var arrayThreeMenu=new Array(
	"80%",
	2,
	//new Array(_("GlobalConfig	"),'DoubleRoad.asp',1,0),
	//new Array(_("DoubleRoadInfo"),'DoubleRoadInfo.asp',1,1)
	new Array("<% write(SchTask); %>",'TaskScheduler.asp',1,1),
	new Array("<% write(SchTaskConf); %>",'TaskScheduler_edit.asp',1,0)
	);
showThreeMenu();
</script>
<table id=grouplist style="display" width="80%"  border="0" align="center" cellpadding="0" cellspacing="0" height="300">
<tr>
<td align="center" class="three_td" valign="top"><table id=grouplist style="display" width="600"  border="0" align="center" cellpadding="0" cellspacing="0">
<tr><td colspan="4" height="40">&nbsp;</td></tr>
<tr bgcolor="c0ddff">
<td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no"  align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
</tr>
<tr bgcolor="c0ddff">
<td colspan="4"><iframe src="_list1.htm" name="frame12"	id="framelist" align="MIDDLE" width="600" height="265" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
</tr>
<tr height="30">
<td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
<tr  >
<td width="5"></td>
<td  class="style6"><div align="left">
<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
<script language="javascript">document.write(_("<% write(Select); %>"));</script>
</div></td>
<td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> 
<td width="90" valign="top" align="right"><input type="button" value="<% write(AddNewOne); %>" class="input-button" onClick="EditConfig(0)" id="add"></td>
<td width="95" valign="top" align="right"><input type="button" value="<% write(DelAll); %>" class="input-button" onClick="delAll()" id="delall"></td>
<td width="45" valign="top" align="right"><input type="submit" value="<% write(com_delete); %>" class="input-button" onClick="return(genData(Scheduler,0,frame12,names,'<% write(SchTask); %>'))" id="del"></td>
</tr>
</table></td>
</tr>
</table></td></tr></table>	

</form></td>	  
</tr>

</table>
</td><td width="15">&nbsp;</td>
</tr>
</table>
</body>
</html>

<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("UserGroup");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">
var errorstr="";var linkPage="";
/////////////////////////////交互变量/////////////////////
var groupNames=new Array();
var AddrGroupNames=new Array();
//AddrGroupNames[0]="abc";
//AddrGroupNames[1]="def";
var UserGroupNames=new Array();
//UserGroupNames[0]="abc";
//UserGroupNames[1]="def";
//UserGroupNames[2]="ghi";
var WebAuthNames=new Array();
//WebAuthNames[0]="123";
//WebAuthNames[1]="456";
//WebAuthNames[2]="789";
var PPPoENames=new Array();
//PPPoENames[0]="p1";
//PPPoENames[1]="p2";
//PPPoENames[2]="p3";
var totalrecs;
var addrtotalrecs;

var DesIPs="";
var DesMasks="";
var GateWays="";
var Metrics="";
var Profiles="";
var PPPoEFlag = 0;
var WebAuthFlag = 0;
<%aspGetPppoeUser();%>
<%aspOutputWebAuthListData();%>
<%aspOutputIpGroupListData();%>

<% 
    getWanIfCount();
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name!="") aspOutputIpGroupOneInfo(edit_Name);//需要修改
%> 
var edit_Names = "<% write(Form("id"));%>";

<!--
var flag = false;
function dspChange()
{
    var val=Group_Config.AddrSel.value;
    if (val == "newAddr") 
    {
	idList.style.display = 'none';
	idTxt.style.display = '';
    }
    else if (val == "oldAddr") {
	idList.style.display = '';
	idTxt.style.display = 'none';
    }
     flag = !flag;
}
function checkContain(text)
{
	var tbl=Group_Config;
	for(var i=0;i<tbl.AddrRange.options.length;i++)
	{
		if(text==tbl.AddrRange.options[i].text)
		{
			alert("<%write(use_incSame);%>");
			return false;
		}
	}
	return true;
}
function addAddrOptions()
{
	var tbl=Group_Config;
	if(tbl.AddrRange.options.length>9)
	{
		alert("<%write(use_addrMax);%>");
		return false;
	}

	if(!(checkIP(tbl.startIP,"<%write(com_ipFrom);%>",0))) return false;
	if(!(checkIP(tbl.endIP,"<%write(com_ipTo);%>",0))) return false;
	if(compareIP(tbl.startIP.value,tbl.endIP.value)==1)
	{
		alert("<%write(use_ipCmp);%>");
		return false;
	}
//	var option = document.createElement("OPTION");
	var optext="P("+tbl.startIP.value+"-"+tbl.endIP.value+")";
	var opvalue=tbl.startIP.value+'-'+tbl.endIP.value;
//  option.text += "P(";
//	option.text += tbl.startIP.value;
//	option.text +="-";
//	option.text += tbl.endIP.value;
//	option.text += ")";
//	option.value = tbl.startIP.value+'-'+tbl.endIP.value;
	if(checkContain(optext)==false)return false;
	Group_Config.AddrRange.options.add(new Option(optext,opvalue));
	tbl.startIP.value="";
	tbl.endIP.value="";
	return true;
}
function addAddrGroupNameOptions(ctl1,ctl2,val)
{
	var array=new Array();
	var array_value = new Array();
	var t=0;
	for(var i=0;i<ctl1.options.length;i++)
	{
		if(ctl1.options[i].selected)
		{
		/*
		if(Group_Config.groupName.value==ctl1.options[i].value)
		{
		alert("不能包含自己。");
		return false;
		}
		*/
		if(val=="oldAddr")
		{
			array[t]='G('+ctl1.options[i].text+')';
			array_value[t] = ctl1.options[i].value;
		}
		else if(val=="group")
		{
			array[t]='G('+ctl1.options[i].text+')';
			array_value[t]='G-'+ctl1.options[i].value;
		}
		else if(val=="webuser")
		{
			array[t]='W('+ctl1.options[i].text+')';
			array_value[t]='W-'+ctl1.options[i].value;
		}
		else if(val=="pppoeuser")
		{
			array[t]='P('+ctl1.options[i].text+')';
			array_value[t]='P-'+ctl1.options[i].value;
		}
	    for(var j=0;j<ctl2.options.length;j++)
	    {
		    //alert(ctl2.options[j].value);
		    if(array_value[t]==ctl2.options[j].value)
		    {
			alert("<%write(use_listExist);%>"+ctl1.options[i].value);
			return false;
		    }
	    }
			t++;
		}
	}
	if(Group_Config.groupType.value=="groupAddr")
	{
	if(eval(t+ctl2.options.length)>10)
	{
		alert("<%write(use_addrMax);%>");
		return	false;
	}
	}
	else if(Group_Config.groupType.value=="groupUser")
	{
	if(eval(t+ctl2.options.length)>100)
	{
		alert("<%write(use_userMax);%>");
		return	false;
	}
	}
	addDataToSelect(ctl2,array,array_value,1,"");
	return true;
}
function delAddrGroupNameOption(ctl1,ctl2)
{
	var count = 0;
	if(ctl1.selectedIndex == -1)return ;
	for(var i=0;i<ctl1.options.length;i++)
	{
		if(ctl1.options[i].selected)
		{
			count = ctl1.selectedIndex;
			ctl1.remove(i);
			delAddrGroupNameOption(ctl1,ctl2);
		}
	}
}
function fromXXtoXX(flag)
{
	var value;
	if(flag==0)
	{
	if(Group_Config.AddrSel.value == "newAddr")
	{
		addAddrOptions();
	}
	else{
		value="oldAddr";
		if(addAddrGroupNameOptions(Group_Config.oldaddrList,Group_Config.AddrRange,value)==true)
			delAddrGroupNameOption(Group_Config.oldaddrList,Group_Config.AddrRange);
	}
	}
	if(flag==1)
	{
		if(Group_Config.userSel.value=="group")
		{
			value="group";
			if(addAddrGroupNameOptions(Group_Config.olduserList1,Group_Config.UserRange,value)==true)
				delAddrGroupNameOption(Group_Config.olduserList1,Group_Config.UserRange);
		}
		if(Group_Config.userSel.value=="webuser")
		{
			var value="webuser";
			if(addAddrGroupNameOptions(Group_Config.olduserList2,Group_Config.UserRange,value)==true)
				delAddrGroupNameOption(Group_Config.olduserList2,Group_Config.UserRange);
		}
		if(Group_Config.userSel.value=="pppoeuser")
		{
			var value="pppoeuser";
			if(addAddrGroupNameOptions(Group_Config.olduserList3,Group_Config.UserRange,value)==true)
				delAddrGroupNameOption(Group_Config.olduserList3,Group_Config.UserRange);
		}
	}
}
function havename(name,type)
{
	var j;
	for(j = 0;j<addrtotalrecs;j++){   //根据描述找组名
		if(AddrGroupNames[j]!=name)
			continue;
		else
			return true;
	}
	alert("<%write(use_noGrp);%>");
	return false;
}
function delAddrRange(flag)
{
	var tbl=Group_Config;
	if(flag==0)
	{
		if(tbl.AddrRange.options.selectedIndex!=-1)
		{
			tbl.AddrRange.remove(tbl.AddrRange.options.selectedIndex);
		}
	}
	else if(flag==1)
	{
		if(tbl.UserRange.options.selectedIndex!=-1)
		{
			tbl.UserRange.remove(tbl.UserRange.options.selectedIndex);
		}
	}
}
function newfromXXtoXX(flag)
{
	var tbl=Group_Config;
	if(flag==0)
	{
	for(var i=0;i<tbl.AddrRange.options.length;i++)
	{
		if(tbl.AddrRange.options[i].selected)
		{
			if(tbl.AddrRange.options[i].text.indexOf("P(") != -1)
			{
				var start1=tbl.AddrRange.options[i].text.indexOf("(");
				var end1=tbl.AddrRange.options[i].text.indexOf(")");
				tbl.AddrRange.options[i].text=tbl.AddrRange.options[i].text.slice(start1+1,end1);
				tbl.AddrSel.value="newAddr";
				addr=strtok("-",tbl.AddrRange.options[i].text);
				tbl.AddrRange.remove(i);
				if(!verifyDottedIP(addr[0])){alert("<%write(com_IPRangeTip);%>");return;}
				if(addr[1]==undefined){alert("<%write(com_IPRangeTip);%>");return;}
				if(!verifyDottedIP(addr[1])){alert("<%write(com_IPRangeTip);%>");return;}
				tbl.startIP.value=addr[0];
				tbl.endIP.value=addr[1];
				dspChange();
			}
			else if(tbl.AddrRange.options[i].text.indexOf("G(")!=-1)
			{
				if(tbl.AddrRange.options[i].value==tbl.groupName.value)
				{
					alert("<%write(use_addrSelf);%>");
					tbl.AddrRange.remove(i);
					return;
				}
				/*
				if(!havename(tbl.AddrRange.options[i].value))
				{
					tbl.AddrRange.remove(i);
					return;
				}
				*/
				tbl.AddrSel.value="oldAddr";
				//var option = document.createElement("OPTION");
				var texts = tbl.AddrRange.options[i].value;
				var values = tbl.AddrRange.options[i].value;
				for(var j=0;j<tbl.oldaddrList.options.length;j++)
				{
				    if(tbl.oldaddrList.options[j].value==values)
				    {
					alert("<%write(use_exist);%>"+values);
					tbl.AddrRange.remove(i);
					dspChange();
					return;
				    }
				}
				tbl.oldaddrList.options.add(new Option(texts,values));
				tbl.AddrRange.remove(i);
				dspChange();
			}
			break;
		}
	}
	}
	else if(flag==1)
	{
	for(var j=0;j<tbl.UserRange.options.length;j++)
	{
		if(tbl.UserRange.options[j].selected)
		{
			if(tbl.UserRange.options[j].text.indexOf("G(")!=-1)
			{
				if(tbl.UserRange.options[j].value==tbl.groupName.value)
				{
					alert("<%write(use_userSelf);%>");
					tbl.UserRange.remove(j);
					return;
				}
				tbl.userSel.value="group";
				var start2=tbl.UserRange.options[j].value.indexOf("-");
				tbl.UserRange.options[j].value=tbl.UserRange.options[j].value.slice(start2+1);
			//	var option=document.createElement("OPTION");
				var text1 = tbl.UserRange.options[j].value;
				var value1 = tbl.UserRange.options[j].value;
				for(var k=0;k<tbl.olduserList1.options.length;k++)
				{
				    if(tbl.olduserList1.options[k].value==value1)
				    {
					alert("<%write(use_exist);%>"+value1);
					tbl.UserRange.remove(j);
					onUserSelChange();
					return;
				    }
				}
				tbl.olduserList1.options.add(new Option(text1,value1));
				tbl.UserRange.remove(j);
				onUserSelChange();
			}
			else if(tbl.UserRange.options[j].text.indexOf("W(")!=-1)
			{
				if(tbl.UserRange.options[j].value==tbl.groupName.value)
				{
					alert("<%write(use_userSelf);%>");
					tbl.UserRange.remove(j);
					return;
				}
				tbl.userSel.value="webuser";
				var start3=tbl.UserRange.options[j].value.indexOf("-");
				tbl.UserRange.options[j].value=tbl.UserRange.options[j].value.slice(start3+1);
			//	var option=document.createElement("OPTION");
				var text2=tbl.UserRange.options[j].value;
				var value2=tbl.UserRange.options[j].value;
				for(var k=0;k<tbl.olduserList2.options.length;k++)
				{
				    if(tbl.olduserList2.options[k].value==value2)
				    {
					alert("<%write(use_exist);%>"+value2);
					tbl.UserRange.remove(j);
					onUserSelChange();
					return;
				    }
				}
				tbl.olduserList2.options.add(new Option(text2,value2));
				tbl.UserRange.remove(j);
				onUserSelChange();
			}
			else if(tbl.UserRange.options[j].text.indexOf("P(")!=-1)
			{
				if(tbl.UserRange.options[j].value==tbl.groupName.value)
				{
					alert("<%write(use_userSelf);%>");
					tbl.UserRange.remove(j);
					return;
				}
				tbl.userSel.value="pppoeuser";
				var start4=tbl.UserRange.options[j].value.indexOf("-");
				tbl.UserRange.options[j].value=tbl.UserRange.options[j].value.slice(start4+1);
			//	var option=document.createElement("OPTION");
				var text3=tbl.UserRange.options[j].value;
				var value3=tbl.UserRange.options[j].value;
				for(var k=0;k<tbl.olduserList3.options.length;k++)
				{
				    if(tbl.olduserList3.options[k].value==value3)
				    {
					alert("<%write(use_exist);%>"+value3);
					tbl.UserRange.remove(j);
					onUserSelChange();
					return;
				    }
				}
				tbl.olduserList3.options.add(new Option(text3,value3));
				tbl.UserRange.remove(j);
				onUserSelChange();
			}
			break;
		}
	}
	}
}
function init(){
	var f=Group_Config;
	delSelectList(Group_Config.AddrRange,false);
	delSelectList(Group_Config.UserRange,false);
	if(edit_Names!=""&&edit_Names!=null){//alert();
		f.groupName.value=curgroupNames;
		f.groupNameOld.value=curgroupNames;
		f.Action.value="modify";
		if(curgroupType=="ipgroup")
		{
		    f.groupType.value="groupAddr";
		    f.groupTypeOld.value="groupAddr";
		}
		if(curgroupType=="usergroup")
		{
		    f.groupType.value="groupUser";
		    f.groupTypeOld.value="groupUser";
		}
		onGroupTypeChange();
		f.groupName.disabled = true;
		f.groupType.disabled = true;
		//alert(f.groupType.value);
		for(i=0;i<curContent.length;i++){
		    var curstart=curContent[i].indexOf("(");
		    var curend=curContent[i].indexOf(")");
			
		   // var option = document.createElement("OPTION");
		   var text4,value4;
		    if(curgroupType=="ipgroup")
		    {
			text4=curContent[i];
			value4=curContent[i].slice(curstart+1,curend);
			Group_Config.AddrRange.options.add(new Option(text4,value4));
		    }
		    if(curgroupType=="usergroup")
		    {
			text4=curContent[i];
			value4=curContent[i].slice(0,1)+"-"+curContent[i].slice(curstart+1,curend);
			//alert(option.value);
			Group_Config.UserRange.options.add(new Option(text4,value4));
		    }
		}
	}
	else
	{
	    f.groupType.value="groupAddr";
	    onGroupTypeChange();
	}
	if(PPPoEFlag == 0 && WebAuthFlag == 0)
	{
	    //f.groupType.disabled = true;
	}
	dspChange();
	onUserSelChange();
	initAddrList(0);
	initUserGroupList();
	initWebUserList();
	initPPPoEUserList();
}
function checkSpecialChar(str)
{
    //alert("groupName:"+str);
    if((str.indexOf("#")!=-1)||(str.indexOf("$")!=-1)||(str.indexOf("(")!=-1)||(str.indexOf(")")!=-1)||(str.indexOf("[")!=-1)||(str.indexOf("]")!=-1)||(str.indexOf("|")!=-1)||(str.indexOf(":")!=-1)||(str.indexOf("￥")!=-1))
    {
	alert("<%write(use_noInclude);%>#, $, (, ), [, ], |, :, ￥");
	return false;
    }
    return true;
}
function checkDataBeforeSave()
{
var definedName=new Array("Inter_all","Extern_all");
	var tbl=Group_Config;
	if(!checkSpecialChar(tbl.groupName.value))
	{
	    return false;
	}
	if(!checkName(tbl.groupName,11,"<%write(com_group);%>",0)) return false;
	if(tbl.groupName.value.indexOf("-")!=-1)
	{
	    alert("<%write(use_except);%>");
	    return false;
	}
	if(tbl.Action.value=="add")
	{
	for(var i=0;i<groupNames.length;i++)
	{
		if((tbl.groupName.value).toLowerCase()==groupNames[i].toLowerCase())
		{
			alert("<%write(use_contain);%>"+tbl.groupName.value);
			return false;
		}
	}
	}
	//if(tbl.Action[0].checked)
	for(var i=0;i<definedName.length;i++)
	{
		if((tbl.groupName.value).toLowerCase()==definedName[i].toLowerCase())
		{
			alert("<%write(use_default);%>"+tbl.groupName.value);
			return false;
		}
	}
	if(tbl.groupType.value=="groupAddr")
	{
		if(tbl.AddrRange.options.length==0)
		{	
			alert("<%write(use_saveNull);%>");
			return false;
		}
	}
	else if(tbl.groupType.value=="groupUser")
	{
		if(tbl.UserRange.options.length==0)
		{
			alert("<%write(use_saveNull);%>");
			return false;
		}
	}
	return true;
}
function save()
{
	var tbl=Group_Config;

	if (tbl.groupName.value.toLowerCase() == "noticeany")
	{
		alert("<%write(use_new);%>"+tbl.groupName.value+"<%write(use_newApp);%>");
		return;
	}
	if(tbl.Action.value=="modify")
	{
	    if(tbl.groupName.value!=tbl.groupNameOld.value)
	    {
		alert("<%write(use_modName);%>");
		return;
	    }
	    if(tbl.groupType.value!=tbl.groupTypeOld.value)
	    {
		alert("<%write(use_modType);%>");
		return;
	    }
	}
	if(tbl.groupType.value=="groupAddr")
	{
		if(tbl.AddrSel.value=="newAddr")
		{
			if(tbl.startIP.value!=""||tbl.endIP.value!="")
			if(addAddrOptions()==false)return;
		}
	}

	if(checkDataBeforeSave()==false)return;

	tbl.temp1.value="";
	if(tbl.groupType.value=="groupAddr")
	{
		for(var i=0;i<tbl.AddrRange.options.length;i++)
		{
			tbl.temp1.value+=tbl.AddrRange.options[i].value;
			tbl.temp1.value+=";";
		}
		for(var j=tbl.AddrRange.options.length;j<10;j++)
		{
			tbl.temp1.value+=";";
		}
		//alert(tbl.temp1.value);
	}
	else if(tbl.groupType.value=="groupUser")
	{
		for(var k=0;k<tbl.UserRange.options.length;k++)
		{
			tbl.temp1.value+=tbl.UserRange.options[k].value;
			tbl.temp1.value+=";";
		}
		for(var l=tbl.UserRange.options.length;l<100;l++)
		{
			tbl.temp1.value+=";";
		}
		//alert(tbl.temp1.value);
	}
	tbl.groupName.disabled=false;
	//itbl.profile.disabled=false;
	tbl.linkPage.value = linkPage;
	
	if (tbl.Action.value=="add") {
		if(max_totalrecs - totalrecs < 1)  
		{	    
	   		alert("<%write(use_noInst);%>");	    
	    	return ;	
		}
	}

	Group_Config.action="/goform/formIpGroupConfig";
	Group_Config.submit();
}
function Reset(){
	Group_Config.reset();
	init();
}
function checkNameExist(checkName,val)
{
    var tmp="";
    if(edit_Names!=""&&edit_Names!=null){//alert();
	for(var i=0;i<curContent.length;i++)
	{
	    if(val=="webUser")
	    {
		if(curContent[i].indexOf("W")!=-1)
		{
		    var start=curContent[i].indexOf("(");
		    var end=curContent[i].indexOf(")");
		    tmp=curContent[i].slice(start+1,end);
		    //alert(tmp);
		    if(checkName==tmp)
		    {
			//alert("find");
			return true;
		    }
		}
	    }
	    else if(val=="pppoeUser")
	    {
		if(curContent[i].indexOf("P")!=-1)
		{
		    var start=curContent[i].indexOf("(");
		    var end=curContent[i].indexOf(")");
		    tmp=curContent[i].slice(start+1,end);
		    //alert(tmp);
		    if(checkName==tmp)
		    {
			//alert("find");
			return true;
		    }
		}
	    }
	    else
	    {
	    var start=curContent[i].indexOf("(");
	    var end=curContent[i].indexOf(")");
	    tmp=curContent[i].slice(start+1,end);
	    //alert(tmp);
	    if(checkName==tmp)
	    {
		//alert("find");
		return true;
	    }
	    }
	}

    }
    return false;
}
function initAddrList(index,name)
{
	var tbl=Group_Config;
	delSelectList(Group_Config.oldaddrList,false);
	//delSelectList(Group_Config.AddrRange,false);
	for(i=0;i<groupNames.length;i++){
	    if(groupType[i]=="ipgroup")
	    {
		if((name==undefined)||(name!=groupNames[i])){
		    if(checkNameExist(groupNames[i])==false)
		    {
				if(groupNames[i]!=Group_Config.groupName.value)
				{
			//	var option = document.createElement("OPTION");
			//	option.text=groupNames[i];
			//	option.value=groupNames[i];
				Group_Config.oldaddrList.options.add(new Option(groupNames[i],groupNames[i]));
		    	}
			}
		}
	    }
	}
}
//usergroup 
function initUserGroupList(index,name)
{
	var tbl=Group_Config;
	delSelectList(Group_Config.olduserList1,false);
	//delSelectList(Group_Config.UserRange,false);
	for(i=0;i<groupNames.length;i++){
	    if(groupType[i]=="usergroup")
	    {
	    if((name==undefined)||(name!=groupNames[i])){
		if(checkNameExist(groupNames[i])==false)
		{
			if(groupNames[i]!=Group_Config.groupName.value)
			{
	//		var option = document.createElement("OPTION");
	//		option.text=groupNames[i];
	//		option.value=groupNames[i];
			Group_Config.olduserList1.options.add(new Option(groupNames[i],groupNames[i]));
			}
		}
	    }
	    }
    }
}
function initWebUserList()
{
	var tbl=Group_Config;
	delSelectList(Group_Config.olduserList2,false);
	//delSelectList(Group_Config.UserRange,false);
	for(i=0;i<WebAuthNames.length;i++)
	{
	    if((name==undefined)||(name!=WebAuthNames[i])){
		if(checkNameExist(WebAuthNames[i],"webUser")==false)
		{
	//	var option = document.createElement("OPTION");
	//	option.text=WebAuthNames[i];
	//	option.value=WebAuthNames[i];
		Group_Config.olduserList2.options.add(new Option(WebAuthNames[i],WebAuthNames[i]));
		}
	    }
    }
}
function initPPPoEUserList()
{
	var tbl=Group_Config;
	delSelectList(Group_Config.olduserList3,false);
	//delSelectList(Group_Config.UserRange,false);
	for(i=0;i<PPPoENames.length;i++)
	{
	    if((name==undefined)||(name!=PPPoENames[i])){
		if(checkNameExist(PPPoENames[i],"pppoeUser")==false)
		{
	//	var option = document.createElement("OPTION");
	//	option.text=PPPoENames[i];
	//	option.value=PPPoENames[i];
		Group_Config.olduserList3.options.add(new Option(PPPoENames[i],PPPoENames[i]));
		}
	    }
    }	
}
function onUserSelChange()
{
	var value=Group_Config.userSel.value;
	if(value == "group")
	{
		showTable(userList1,1);
		showTable(userList2,0);
		showTable(userList3,0);
		//initUserGroupList();
	}
	if(value == "webuser")
	{
		showTable(userList1,0);
		showTable(userList2,1);
		showTable(userList3,0);
		//initWebUserList();
	}
	if(value == "pppoeuser")
	{
		showTable(userList1,0);
		showTable(userList2,0);
		showTable(userList3,1);
		//initPPPoEUserList();
	}
}
function onGroupTypeChange()
{
	var val=Group_Config.groupType.value;
	if(val == "groupAddr")
	{
		showTable(AddrGroupTab,1);
		showTable(UserGroupTab,0);	
	}
	if(val == "groupUser")
	{
		showTable(AddrGroupTab,0);
		showTable(UserGroupTab,1);
	}
}
function selectType(frm,value)
{
	if(value == "groupAddress")
	{
		showTable(AddrGroupTab,1);
		showTable(UserGroupTab,0);	
	}
	if(value == "groupUser")
	{
		showTable(AddrGroupTab,0);
		showTable(UserGroupTab,1);
	}
}
//-->
</script>
</head>
<body onLoad=""onbeforeunload="logOut()">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle('<%write(com_userConf);%>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>

  <tr valign="top">
    <td><form action="/goform/"  name="Group_Config" method="post">
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center" style="display:">
<p>&nbsp;</p>
<tr><td>&nbsp;&nbsp;</td></tr>
  <tr>
    <td align="center" class="style7">
	
	<div>
		<!--<fieldset>-->
		<br><br>
		  <table width="100%"	 border="0" cellspacing="0" cellpadding="0">
		  <tr>
		  <td><table width="100%"  border="0" cellspacing="0" cellpadding="0">
		  <tr>
		  <td width="36%" height="30" align="right"><%write(com_group);%></td>
		  <td width="2%">&nbsp;</td>
		  <td width="62%"><input name="groupName" type="text"	onClick=""   class="input" maxlength="11"> </td>
		  </tr><input type="hidden" name="groupNameOld">
		  <tr>
		  <td width="36%" height="30" align="right"><%write(use_grpType);%></td>
		  <td width="2%">&nbsp;</td>
		  <td width="62%">
		  <select name="groupType" id="groupType" onChange="onGroupTypeChange()">
		  	<option value="groupAddr"><%write(com_addrGrp);%></option>
		  <script language="javascript">
		  if(PPPoEFlag == 1 || WebAuthFlag == 1)
		  {
		    document.write('<option value="groupUser"><%write(use_accGrp);%></option>');
		  }
		  </script>
		  </select>
		  </tr><input type="hidden" name="groupTypeOld">
		  </table>
		  </td>
		  </tr>
			<tr>
		  <td>
		  <table width="100%"  border="0" cellspacing="0" cellpadding="0">	  
		  <tr>
			<td height="0" colspan="3">
			<input type="hidden" name="hidDomain" id="hidDomain" size="10">
			<input type="hidden" name="hidID" id="hidId" size="10">
			<input type="hidden" name="fillId" id="fillId" size="10">
			<input type="hidden" name="temp1" id="fillId" size="10">
			<input type="hidden" name="linkPage" size="10"></td>
		  </tr>
		  </table>
		  </td>
			</tr>
	    </table>

		<div align="center">
		<table style="display:" width="100%"  border="0" align="center" cellpadding="0" cellspacing="0" id="AddrGroupTab" name="AddrGroupTab">
		  <tr>
		    <td height="156" colspan="3" align="left" valign="top"> <table style="display" width="90%"	border="0" align="center" cellpadding="0" cellspacing="0">
			<tr>
			  <td colspan="3"> <table style="display:" width="100%"	border="0" align="center" cellpadding="0" cellspacing="0">
				<td width="31%" height="40"> </td>
				<td width="53%" ><select name="AddrSel" id="AddrSel" onChange="dspChange()">
				  <option value="newAddr"><%write(use_newAddr);%></option>
				  <option value="oldAddr"><%write(use_oldAddr);%></option>
				  </select>
				<td width="16%">
			      </tr> </table></td>
			</tr>
			<tr>
			  <td width="25%"></td>
			  <td width="75%" align="center" colspan="3"><div style="height:;width:200px;border: 1px solid #adafae" id="idTxt" class="input">
			    <table style=" background-color:#FFFFFF;display:" width="100%"  border="0" align="center" cellpadding="0" cellspacing="0">
                <tr><td height="22"></td></tr>
                  <tr>
                    <td width="30%" valign="middle">&nbsp;<%write(com_ipFrom);%>:</td>
                    <td width="5%"></td>
                    <td width="65%" height="34"><input name="startIP" type="text" class="input" id="startIP" onKeyPress="ipOnly()" size="16" />                    </td>
                  </tr>
                  <tr>
                    <td valign="middle">&nbsp;<%write(com_ipTo);%>:</td>
                    <td width="5%"></td>
                    <td width="65%" height="22"><input name="endIP" type="text" class="input" id="endIP" onKeyPress="ipOnly()" size="16" />                    </td>
                  </tr>
                  <tr>
                    <td height="22"></td>
                  </tr>
                </table>
			  </div></td>
			</tr>			
			<tr>
			 <td width="25%"></td>
			  <td width="75%" align="center" colspan="3"> <table style="background-color:#FFFFFF;display:none" bgcolor="#FFFFFF" width="100%"  border="0" align="center" cellpadding="0" cellspacing="0" id="idList"><tr>
				<td width="10%"> </td>
				<td width="224" colspan="3" valign="top"> <div align="left" style=" background-color:#FFFFFF width=200px;overflow-x:auto">
				  <select class="input" name="oldaddrList" size=7 multiple  id="oldaddrList" ondblclick="fromXXtoXX(0)"style=" background-color: #ffffff; SCROLLBAR-FACE-COLOR: #ffffff;  SCROLLBAR-HIGHLIGHT-COLOR: #ffffff;	#c0ddff 1px solid; SCROLLBAR-SHADOW-COLOR: #ffffff; LINE-HEIGHT: 20px; SCROLLBAR-ARROW-COLOR: #c0ddff; SCROLLBAR-TRACK-COLOR: #ffffff; SCROLLBAR-DARKSHADOW-COLOR: #c0ddff; SCROLLBAR-BASE-COLOR: #c0ddff;border:1px solid #c0ddff; width:200px" border=0>
                  </select>
				</div></td>
			      </tr> </table></td>
			</tr>
		      </table></td>
		    <td width="53%" valign="top"> <table style="display" width="100%"  border="0" align="center" cellpadding="0" cellspacing="0">
			<tr>
			  <td width="13%" height="40" valign="middle">&nbsp;</td>
			  <td align="center" width="230px"><font style="FONT-FAMILY: "Arial", ",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",","Helvetica", ",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",","sans-serif";FONT-SIZE: 12px"><%write(use_rangeList);%>:</font></td>
			  <td></td>
			</tr>
			<tr>
			  <td rowspan="4"><table id="idImg">
			      <tr>
				<td><input name="m1" type="button" value="==>" onClick="fromXXtoXX(0)" style="width:65px"/></td>
			      </tr>
			      <tr>
				<td height="30"><input name="m2" type="button" value="<==" onClick="newfromXXtoXX(0)" style="width:65px" /> </td>
			      </tr>
 <tr>
				<td height="30"><input name="delete" type="button" value="<%write(use_delete);%>" onClick="delAddrRange(0)" style="width:65px"/></td>
			      </tr>
			     </table></td>
			  <td width="87%">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
			    <!--<select class="input" size=7 name="AddrRange"  multiple	 id="addrRange" ondblclick="delarrow()" style="border:0px;width=200px" border=0 >
							</select> -->
			    <select class="input" name="AddrRange" size=7  id="AddrRange" ondblclick="newfromXXtoXX(0)"style="width:200px" border=0>
                </select></td>
			</tr>
		      </table></td>
		  </tr>
		</table>
	      </div>
	     <!-- </fieldset>-->
		<table style="display:none" width="100%"  border="0" align="center" cellpadding="0" cellspacing="0" id="UserGroupTab" name="UserGroupTab">
		  <tr>
		    <td height="156" colspan="3" align="left" valign="top"> <table style="display" width="90%"	border="0" align="center" cellpadding="0" cellspacing="0">
			<tr>
			  <td colspan="3"> <table style="display" width="100%"	border="0" align="center" cellpadding="0" cellspacing="0">
				<td width="31%" height="40"> </td>
				<td width="53%">
				  <select name="userSel" id="userSel" onChange="onUserSelChange()"><option value="group"><%write(use_group);%></option><option value="webuser"><%write(use_webUser);%></option>
				  <script language="javascript">
				  if(PPPoEFlag == 1)
					document.write('<option value="pppoeuser"><%write(use_pppoe);%></option>');
				  </script>
				  </select></td>
				<td width="16%"></td>
			      </tr> </table></td>
			</tr>
			<tr>
			  <td width="25%"></td>
			  <td width="75%" align="center" colspan="3">  
			  <table style="background-color:#FFFFFF;display:" bgcolor="#FFFFFF" width="100%"  border="0" align="center" cellpadding="0" cellspacing="0" id="userList1">
				<tr><td width="10%"> </td>
				<td width="224" colspan="3" valign="top"> <div align="left" style=" background-color:#FFFFFF width=200px;overflow-x:auto">
				  <select class="input" name="olduserList1" size=7 multiple  id="olduserList1" ondblclick="fromXXtoXX(1)"style=" background-color: #ffffff; SCROLLBAR-FACE-COLOR: #ffffff;  SCROLLBAR-HIGHLIGHT-COLOR: #ffffff;	#c0ddff 1px solid; SCROLLBAR-SHADOW-COLOR: #ffffff; LINE-HEIGHT: 20px; SCROLLBAR-ARROW-COLOR: #c0ddff; SCROLLBAR-TRACK-COLOR: #ffffff; SCROLLBAR-DARKSHADOW-COLOR: #c0ddff; SCROLLBAR-BASE-COLOR: #c0ddff;border:1px solid #c0ddff; width:200px" border=0>
                  </select>
				</div></td>
			      </tr> </table>
			  </td>
			</tr>
			<tr>
			  <td width="25%"></td>
			  <td width="75%" align="center" colspan="3">  
			  <table style="background-color:#FFFFFF;display:none" bgcolor="#FFFFFF" width="100%"  border="0" align="center" cellpadding="0" cellspacing="0" id="userList2">
				<tr><td width="10%"> </td>
				<td width="224" colspan="3" valign="top"> <div align="left" style=" background-color:#FFFFFF width=200px;overflow-x:auto">
				  <select class="input" name="olduserList2" size=7 multiple  id="olduserList2" ondblclick="fromXXtoXX(1)"style=" background-color: #ffffff; SCROLLBAR-FACE-COLOR: #ffffff;  SCROLLBAR-HIGHLIGHT-COLOR: #ffffff;	#c0ddff 1px solid; SCROLLBAR-SHADOW-COLOR: #ffffff; LINE-HEIGHT: 20px; SCROLLBAR-ARROW-COLOR: #c0ddff; SCROLLBAR-TRACK-COLOR: #ffffff; SCROLLBAR-DARKSHADOW-COLOR: #c0ddff; SCROLLBAR-BASE-COLOR: #c0ddff;border:1px solid #c0ddff; width:200px" border=0>
                  </select>
				</div></td>
			      </tr> </table>
			  </td>
			</tr>
			<tr>
			  <td width="25%"></td>
			  <td width="75%" align="center" colspan="3">  
			  <table style="background-color:#FFFFFF;display:none" bgcolor="#FFFFFF" width="100%"  border="0" align="center" cellpadding="0" cellspacing="0" id="userList3">
				<tr><td width="10%"> </td>
				<td width="224" colspan="3" valign="top"> <div align="left" style=" background-color:#FFFFFF width=200px;overflow-x:auto">
				  <select class="input" name="olduserList3" size=7 multiple  id="olduserList3" ondblclick="fromXXtoXX(1)"style=" background-color: #ffffff; SCROLLBAR-FACE-COLOR: #ffffff;  SCROLLBAR-HIGHLIGHT-COLOR: #ffffff;	#c0ddff 1px solid; SCROLLBAR-SHADOW-COLOR: #ffffff; LINE-HEIGHT: 20px; SCROLLBAR-ARROW-COLOR: #c0ddff; SCROLLBAR-TRACK-COLOR: #ffffff; SCROLLBAR-DARKSHADOW-COLOR: #c0ddff; SCROLLBAR-BASE-COLOR: #c0ddff;border:1px solid #c0ddff; width:200px" border=0>
                  </select>
				</div></td>
			      </tr> </table>
			  </td>
			</tr>
		      </table></td>
		    <td width="53%" valign="top"> <table style="display:" width="100%"  border="0" align="center" cellpadding="0" cellspacing="0">
			<tr>
			  <td width="13%" height="40" valign="middle">&nbsp;</td>
			  <td align="center" width="230px"><font style="FONT-FAMILY: "Arial", ",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",","Helvetica", ",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",","sans-serif";FONT-SIZE: 12px"><%write(use_grpList);%>:</font></td>
			  <td></td>
			</tr>
			<tr>
			  <td rowspan="4"><table id="idImg">
			      <tr>
				<td><input name="m1" type="button" value="==>" onClick="fromXXtoXX(1)" style="width:65px"/></td>
			      </tr>
			      <tr>
				<td height="30"><input name="m2" type="button" value="<==" onClick="newfromXXtoXX(1)" style="width:65px" /> </td>
			      </tr>
 <tr>
				<td height="30"><input name="delete" type="button" value="<%write(use_delete);%>" onClick="delAddrRange(1)" style="width:65px"/></td>
			      </tr>

			     </table></td>
			  <td width="87%">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
			    <select class="input" name="UserRange" size=7  id="UserRange" ondblclick="newfromXXtoXX(1)"style="width:200px" border=0>
                </select></td>
			</tr>
		      </table></td>

		  </tr>
		</table>
	      </div>
	     <!-- </fieldset>-->
	</div>

  <table border="0" width="90%" align='center'>
  <tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
  <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>" onClick="Reset()">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#UserGroup')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_back);%>" onClick="GoBack_Page('UserGroup.asp')">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>
  </tr>
</table>
  </td></tr>
  </table></td>
  </tr>
</table>
</form></td>
  </tr>
  <script language="javascript">init();</script>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>

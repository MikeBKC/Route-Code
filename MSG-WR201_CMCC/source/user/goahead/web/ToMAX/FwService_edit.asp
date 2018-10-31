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
</script>
<script language="javascript"> 
var errorstr="";var linkPage="";
var totalrecs; var groupNames = new Array(); var line = new Array(new Array()); var line1 = new Array(); var line2 = new Array(); var line3 = new Array(); var line4 = new Array(); var line5 = new Array(); var line6 = new Array(); var line7 = new Array(); var line8 = new Array(); var line9 = new Array(); var line10 = new Array(); var type = new Array();
var curContent = new Array();
var curgroupNames = "";
<!-- 打印 -->
<%aspOutputPortGroupListData();%>
<!-- 打印 -->
<% 
getWanIfCount();
var edit_Name="";
edit_Name=Form("id");
if ( edit_Name!="") aspOutputPortGroupOneInfo(edit_Name);//需要修改
%> 
<!-- 打印 -->
var edit_Names = "<% write(Form("id"));%>";
<!-- 打印 -->
//获取linkPage,初始化到linkPage按钮
var locationStr = window.location.toString();
var locationIndex = locationStr.indexOf("?");
var linkPage = null;
if (locationIndex != -1) {
    linkPage = locationStr.substring(locationIndex + 1);
}
var color = "Green";
//var shorttype=new Array("T(","U(","O(","L(","K(","D(","M(");
var shorttype=new Array("T(","U(","O(","C(","C(","C(","C(");
//var reg=new Array(/^[Tt]\(\d+-\d+,\d+-\d+\)/,/^[Uu]\(\d+-\d+,\d+-\d+\)/,/^[Oo]\(\d+\)/,/^[Ll]\(/,/^[Kk]\(/,/^[Dd]\(/,/^[Mm]\(/,/^[Cc]\(/);
var reg=new Array(/^[Tt]\(\d+-\d+,\d+-\d+\)/,/^[Uu]\(\d+-\d+,\d+-\d+\)/,/^[Oo]\(\d+\)/,/^[Cc]\(/,/^[Cc]\(/,/^[Cc]\(/,/^[Cc]\(/,/^[Cc]\(/);
var reg_tcpudp = /[\)\(,-]/;
var reg_other = /[\)\(]/;
var definedServ=new Array(
        "ftp","ssh","telnet","smtp","sql*net","finger","web","pop3","epmap","netbios_ssn","https","ms-ds","ms-sql-s","ms-sql-m","h.323","pptp","msnlogin","ms-ts",
        "dns","bootps","bootpc","tftp","ntp","netbios-ns","netbios-dgm","snmp","snmptrap","isakmp","l2tp",
        "icmp","igmp","ipinip","gre","esp","ah","ospf","igrp","rsvp"
        );

var line1 = new Array();
var containType=new Array();
//可以用来判断组的深度,以决定是否可以显示到已有服务中
for(var i=0;i<totalrecs;i++)
{
    line1[i] = new Array();
    containType[i] = 0;
    for(var j=0;j<10;j++)
    {
        if((line[i][j].length!=0)&&(reg[0].test(line[i][j])==false)&&(reg[1].test(line[i][j])==false)&& (reg[2].test(line[i][j])==false)&&(reg[3].test(line[i][j])==false)&&(reg[4].test(line[i][j])==false)&&(reg[5].test(line[i][j])==false)&&(reg[6].test(line[i][j])==false)&&(line[i][j].length < 12))
        {
            containType[i] = 1;
            break;
        }
    }
}

var nIndex = 0;

//初始化已有服务的列表
function initAddrList(index,name)
{
    var tbl=FwAddr;
    delSelectList(FwAddr.addrList,false);
    for(i=0;i<groupNames.length;i++){
        if((type[i]==tbl.groupType.options[index].value)&&(containType[i]==0)){
            if((name==undefined)||(name!=groupNames[i])){
                if(checkNameExist(groupNames[i])==false){
                    var option = document.createElement("OPTION");
                    option.text  = groupNames[i] ;
                    option.value = "G(" + groupNames[i] + ")";
                    tbl.addrList.options.add(option);
                }
            }
        }
    }
    if(index==0)
    {
        addDataToSelectAndDelete(tbl.addrList,definedServ,definedServ,curContent,1,"");
    }
}

function checkNameExist(checkName,val)
{
    var tmp="";
    if(edit_Names!=""&&edit_Names!=null){
        for(var i=0;i<curContent.length;i++)
        {
            var start=curContent[i].indexOf("(");
            var end=curContent[i].indexOf(")");
            tmp=curContent[i].slice(start+1,end);
            if(checkName==tmp)
            {
                return true;
            }
        }
    }
    return false;
}

function inittext()
{
    var ctl=FwAddr;
    ctl.dFromPort.value="";
    ctl.dToPort.value="";
    ctl.sFromPort.value="";
    ctl.sToPort.value="";
    ctl.URLaddress.value="";
    ctl.Keyword.value="";
    ctl.DNSAddress.value="";
    ctl.MACaddress.value="";
}

/*function init()
{
    var tbl=FwAddr;
    FwAddr.reset();
    if (linkPage == "QosSpecific") {
        showTable(tbQos, true);
        FwAddr.linkPage.value = linkPage;
    } else if (linkPage == "IPAdmin") {
        FwAddr.linkPage.value = linkPage;
        showTable(tbIPAdmin, true);
    } else if(linkPage == "PolicyRoute"){
        FwAddr.linkPage.value = linkPage;
        showTable(tbPolicyRoute,true);
    }else {
        ;
    }
    //FwAddr.remark.value="";
    tbl.groupType.disabled=false;
    tbl.groupName.value="";
    tbl.Action[0].checked=true;
    //tbl.remark.disabled=false;
    SelType(tbl.groupType);
}*/
function init(){
	var f = FwAddr;
    initAddrList(0);
    delSelectList(FwAddr.AddrRange,false);
	if(edit_Names!=""&&edit_Names!=null){
        initAddrList(0,curgroupNames);
		f.groupName.value=curgroupNames;
		f.groupNameOld.value=curgroupNames;
		f.Action.value="modify";
        f.groupType.value=curgroupType;

        SelType(f.groupType);
		f.groupName.disabled = true;
		f.groupType.disabled = true;
		for(i=0;i<curContent.length;i++){
		    var curstart=curContent[i].indexOf("G(");
		    var curend=curContent[i].indexOf(")");
            var text4,value4;

            text4=curContent[i];
            value4 = text4;
            if(curstart != -1)
                value4=curContent[i].slice(curstart+2,curend);
            FwAddr.AddrRange.options.add(new Option(value4,text4));
		}
	}
	else
	{
	    f.groupType.value="portGroup";
        SelType(f.groupType);
	}
	dspChange();
}

function Reset(){
    FwAddr.reset();
    init();
}

function changeType()
{
    var tbl=FwAddr;
    var index=-1;
    index=getListIndex(tbl.groupType);
    initAddrList(index,curgroupNames);
    delSelectList(FwAddr.AddrRange,false);
}

//加上text值页面会乱掉
function checkdata()
{
    var tbl=FwAddr;
    var checkright=true;
    var text="";
    //if(tbl.Action[0].checked==1){
        //if (!RemarkExist(tbl.remark.value))
        //{
        //    return false;
        //}
    //}
    if(isNaN(tbl.sFromPort.value)||isNaN(tbl.sToPort.value)||isNaN(tbl.dToPort.value)||isNaN(tbl.dFromPort.value))
    {
        alert("端口内容包含非法字符");return false;
    }
    if(tbl.sFromPort.value=="")
    {
        text+="源起始端口不能为空\n";checkright=false;
    }

    if(parseInt(tbl.sFromPort.value)<0)
    {
        text+="源起始端口不能小于1\n";checkright=false;
    }
    if(parseInt(tbl.sFromPort.value)>65535)
    {
        text+="源起始端口不能大于65535\n";checkright=false;
    }

    if(tbl.sToPort.value=="")
    {
        text+="源结束端口不能为空\n";checkright=false;
    }
    if(parseInt(tbl.sToPort.value)<0)
    {
        text+="源结束端口不能小于1\n";checkright=false;
    }
    if(parseInt(tbl.sToPort.value)>65535)
    {
        text+="源结束端口不能大于65535\n";checkright=false;
    }

    if(parseInt(tbl.sFromPort.value)>parseInt(tbl.sToPort.value))
    {
        text+="";checkright=false;
    }

    if(tbl.dFromPort.value=="")
    {
        text+="目的起始端口不能为空\n";checkright=false;
    }
    if(parseInt(tbl.dFromPort.value)<0)
    {
        text+="目的起始端口不能小于1\n";checkright=false;
    }
    if(parseInt(tbl.dFromPort.value)>65535)
    {
        text+="目的起始端口不能大于65535\n";checkright=false;
    }
    if(tbl.dToPort.value=="")
    {
        text+="目的结束端口不能为空\n";checkright=false;
    }
    if(parseInt(tbl.dToPort.value)<0)
    {
        text+="目的结束端口不能小于1\n";checkright=false;
    }
    if(parseInt(tbl.dToPort.value)>65535)
    {
        text+="目的结束端口不能大于65535\n";checkright=false;
    }

    if(parseInt(tbl.sFromPort.value)>parseInt(tbl.sToPort.value))
    {
        text+="源起始端口不能大于源结束端口\n";checkright=false;
    }

    if(parseInt(tbl.dFromPort.value)>parseInt(tbl.dToPort.value))
    {
        text+="目的起始端口不能大于目的结束端口\n";checkright=false;
    }

    if(text!="")alert(text);
    return checkright;
}
function checkContain(text)
{
    var tbl=FwAddr;
    for(var i=0;i<tbl.AddrRange.options.length;i++)
    {
        if(text==tbl.AddrRange.options[i].text)
        {
            alert("本组内已经包含相同的服务");
            return false;
        }
    }
    return true;
}
function checkChar()
{
    if ((window.event.keyCode == 40)||(window.event.keyCode == 41)||(window.event.keyCode == 59))
    {
        window.event.keyCode = 0 ;
    }

}
//保存和修改新服务或者已有服务格式
function addServOption()
{
    var tbl=FwAddr;
    var count=tbl.groupType.selectedIndex;
    var option = document.createElement("OPTION");
    if(tbl.AddrRange.options.length>9)
    {
        alert("一个服务组列表的内容不能大于10条");
        return false;
    }
    switch(count){
        case 0:
            var count1=tbl.idProtocol.selectedIndex;
            option.text +=shorttype[count1];
            if(count1<2){
                if(checkdata()==false)return false;
                option.text += tbl.sFromPort.value;
                option.text += "-";
                option.text += tbl.sToPort.value;
                option.text += ",";
                option.text += tbl.dFromPort.value;
                option.text += "-";
                option.text += tbl.dToPort.value;
                option.text += ")";
                option.value = option.text;
                tbl.sFromPort.value="";
                tbl.sToPort.value="";
                tbl.dFromPort.value="";
                tbl.dToPort.value="";
            }else{
                if(tbl.ProtocolNum.value=="")
                {
                    alert("协议号不能为空");return false;
                }
                if(parseInt(tbl.ProtocolNum.value)<0)
                {
                    alert("协议号不能小于1");return false;
                }
                if(parseInt(tbl.ProtocolNum.value)>65535)
                {
                    alert("协议号不能大于65535");return false;
                }
                if(isNaN(tbl.ProtocolNum.value))
                {
                    alert("端口内容包含非法字符");return false;
                }
                option.text += tbl.ProtocolNum.value;
                option.text += ")";

                option.value = option.text;
                tbl.ProtocolNum.value="";
            }
            if(checkContain(option.text)==false)return false;
            FwAddr.AddrRange.options.add(option);
            break;
        case 1:
            if(tbl.URLaddress.value=="")
            {
                alert("URL内容不能为空");return false;
            }
            if(!checkName(tbl.URLaddress,31,"URL内容",0)) return false;
            option.text +=shorttype[count+2];
            option.text +=tbl.URLaddress.value;
            option.text += ")";

            option.value = option.text;
            if(checkContain(option.text)==false)return false;
            FwAddr.AddrRange.options.add(option);
            FwAddr.AddrRange.options.add(option);
            tbl.URLaddress.value="";
            break;
        case 2:
            if(tbl.Keyword.value=="")
            {
                alert("关键字内容不能为空");return false;
            }
            if(!checkName(tbl.Keyword,31,"关键字内容",0)) return false;
            option.text +=shorttype[count+2];
            option.text +=tbl.Keyword.value;
            option.text += ")";

            option.value = option.text;

            if(checkContain(option.text)==false)return false;
            FwAddr.AddrRange.options.add(option);
            tbl.Keyword.value="";
            break;
        case 3:
            if(tbl.DNSAddress.value=="")
            {
                alert("DNS内容不能为空");return false;
            }
            if(!checkName(tbl.DNSAddress,31,"DNS内容",0)) return false;
            option.text +=shorttype[count+2];
            option.text +=tbl.DNSAddress.value;
            option.text += ")";

            option.value = option.text;
            if(checkContain(option.text)==false)return false;
            FwAddr.AddrRange.options.add(option);
            tbl.DNSAddress.value="";
            break;
        case 4:
            if(tbl.MACaddress.value=="")
            {
                alert("MAC内容不能为空");return false;
            }
            if(tbl.MACaddress.value.length!=12)
            {
                alert("MAC内容必须为12位");return false;
            }
            if(isNaN(tbl.MACaddress.value))
            {
                alert("端口内容包含非法字符");return false;
            }

            option.text +=shorttype[count+2];
            option.text +=tbl.MACaddress.value;
            option.text += ")";

            option.value = option.text;
            if(checkContain(option.text)==false)return false;
            FwAddr.AddrRange.options.add(option);
            tbl.MACaddress.value="";
            break;
        default:
            break;
    }
    return true;
}

function addAddrGroupNameOptions(ctl1,ctl2)
{
    var array=new Array();
    var array1 = new Array();
    var t=0;
    for(var i=0;i<ctl1.options.length;i++)
    {
        if(ctl1.options[i].selected)
        {
            array[t]=ctl1.options[i].text;
            array1[t] = ctl1.options[i].value;
            t++;
        }
    }
    if(eval(t+ctl2.options.length)>10)
    {
        alert("一个服务组列表的内容不能大于10条");
        return false;
    }
    addDataToSelect(ctl2,array,array1,1,"");
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

function fromXXtoXX()
{
    if(FwAddr.idSel[0].checked)
    {
        addServOption();
    }else{
        if(addAddrGroupNameOptions(FwAddr.addrList,FwAddr.AddrRange)==true)
            delAddrGroupNameOption(FwAddr.addrList,FwAddr.AddrRange);
    }
}

function havename(str,grouptype)
{
    var tmp = "";
    var start = "";
    var end = "";
    if(str.length>31){alert("错误的服务内容");return false}
    for(var i=0;i<totalrecs;i++)
    {
        start = str.indexOf("(");
        end   = str.indexOf(")");
        tmp = str.slice(start+1,end);
        if((groupNames[i] == tmp))
        {
            if(type[i]==grouptype) return true;
            else{ alert("此组名和父组的类型不同");return false;}
        }
    }
    if(grouptype=="portGroup"){
        for(var i=0;i<definedServ.length;i++)
        {
            if(definedServ[i].toLowerCase()==str.toLowerCase())
            {
                return true;
            }
        }
    }
    alert("没有这个组名");
    return false;
}

function delAddrRange()
{
    var tbl=FwAddr;
    if(tbl.AddrRange.options.selectedIndex!=-1)
        tbl.AddrRange.options.remove(tbl.AddrRange.options.selectedIndex);
}

function newfromXXtoXX()
{
    var text=new Array();
    var result;
    var tbl=FwAddr;
    var flag=tbl.groupType.selectIndex;
    for(var i=0;i<tbl.AddrRange.options.length;i++)
    {
        if(tbl.AddrRange.options[i].selected)
        {
            if(tbl.groupType.selectedIndex==0)
            {
                var result1=reg[0].test(tbl.AddrRange.options[i].value);
                var result2=reg[1].test(tbl.AddrRange.options[i].value);
                var result3=reg[2].test(tbl.AddrRange.options[i].value);
                if((result1== true)||(result2== true)||(result3== true))
                {
                    tbl.idSel[0].checked = true;
                    dspChange();
                    if(result1==true)
                    {
                        selVal(tbl.idProtocol,"TCP");
                        SelProtlcol(tbl.idProtocol);
                        text=tbl.AddrRange.options[i].value.split(reg_tcpudp);
                        tbl.sFromPort.value=text[1];
                        tbl.sToPort.value=text[2];
                        tbl.dFromPort.value=text[3];
                        tbl.dToPort.value=text[4];
                    }
                    if(result2==true)
                    {
                        selVal(tbl.idProtocol,"UDP");
                        SelProtlcol(tbl.idProtocol);
                        text=tbl.AddrRange.options[i].value.split(reg_tcpudp);
                        tbl.sFromPort.value=text[1];
                        tbl.sToPort.value=text[2];
                        tbl.dFromPort.value=text[3];
                        tbl.dToPort.value=text[4];
                    }
                    if(result3==true)
                    {
                        selVal(tbl.idProtocol,"Other");
                        SelProtlcol(tbl.idProtocol);
                        text=tbl.AddrRange.options[i].value.split(reg_other);
                        tbl.ProtocolNum.value=text[1];
                    }
                }else{
                    if(tbl.AddrRange.options[i].value==tbl.groupName.value)
                    { 
                        alert("此服务组包含了自己");tbl.AddrRange.options.remove(i);return;
                    }
                    if(havename(tbl.AddrRange.options[i].value,tbl.groupType.options[0].value)==false)
                    { 
                        tbl.AddrRange.options.remove(i);return;
                    }
                    tbl.idSel[1].checked=true;
                    var option = document.createElement("OPTION");
                    option.text = tbl.AddrRange.options[i].text;
                    option.value = tbl.AddrRange.options[i].value;
                    tbl.addrList.options.add(option);
                    dspChange();
                }
                tbl.AddrRange.options.remove(i);
                break;
            }
            if(tbl.groupType.selectedIndex==1)
            {
                if(reg[3].test(tbl.AddrRange.options[i].value)==true)
                {
                    text=tbl.AddrRange.options[i].value.split(reg_other);
                    tbl.URLaddress.value=text[1];
                    tbl.idSel[0].checked = true;
                    dspChange();
                }else{
                    if(tbl.AddrRange.options[i].value==tbl.groupName.value)
                    { alert("此服务组包含了自己");tbl.AddrRange.options.remove(i);return;}
                    if(havename(tbl.AddrRange.options[i].value,tbl.groupType.options[1].value)==false)
                    { tbl.AddrRange.options.remove(i);return;}
                    tbl.idSel[1].checked=true;
                    var option = document.createElement("OPTION");
                    option.text = tbl.AddrRange.options[i].text;
                    option.value = tbl.AddrRange.options[i].value;
                    tbl.addrList.options.add(option);
                    dspChange();
                }
                tbl.AddrRange.options.remove(i);
                break;
            }
            if(tbl.groupType.selectedIndex==2)
            {
                if(reg[4].test(tbl.AddrRange.options[i].value)==true)
                {
                    text=tbl.AddrRange.options[i].value.split(reg_other);
                    tbl.Keyword.value=text[1];
                    tbl.idSel[0].checked = true;
                    dspChange();
                }else{

                    if(tbl.AddrRange.options[i].value==tbl.groupName.value)
                    { alert("此服务组包含了自己");tbl.AddrRange.options.remove(i);return;}
                    if(havename(tbl.AddrRange.options[i].value,tbl.groupType.options[2].value)==false)
                    { tbl.AddrRange.options.remove(i);return;}
                    tbl.idSel[1].checked=true;
                    var option = document.createElement("OPTION");
                    option.text = tbl.AddrRange.options[i].text;
                    option.value = tbl.AddrRange.options[i].value;
                    tbl.addrList.options.add(option);
                    dspChange();
                }
                tbl.AddrRange.options.remove(i);
                break;
            }
            if(tbl.groupType.selectedIndex==3)
            {
                if(reg[5].test(tbl.AddrRange.options[i].value)==true)
                {
                    text=tbl.AddrRange.options[i].value.split(reg_other);
                    tbl.DNSAddress.value=text[1];
                    tbl.idSel[0].checked = true;
                    dspChange();
                }else{
                    if(tbl.AddrRange.options[i].value==tbl.groupName.value)
                    { alert("此服务组包含了自己");tbl.AddrRange.options.remove(i);return;}
                    if(havename(tbl.AddrRange.options[i].value,tbl.groupType.options[3].value)==false)
                    { tbl.AddrRange.options.remove(i);return;}
                    tbl.idSel[1].checked=true;
                    var option = document.createElement("OPTION");
                    option.text = tbl.AddrRange.options[i].text;
                    option.value = tbl.AddrRange.options[i].value;
                    tbl.addrList.options.add(option);
                    dspChange();
                }
                tbl.AddrRange.options.remove(i);
                break;
            }
            if(tbl.groupType.selectedIndex==4)
            {
                if(reg[6].test(tbl.AddrRange.options[i].value)==true)
                {
                    text=tbl.AddrRange.options[i].value.split(reg_other);
                    tbl.MACaddress.value=text[1];
                    tbl.idSel[0].checked = true;
                    dspChange();
                }else{
                    if(tbl.AddrRange.options[i].value==tbl.groupName.value)
                    { alert("此服务组包含了自己");tbl.AddrRange.options.remove(i);return;}
                    if(havename(tbl.AddrRange.options[i].value,tbl.groupType.options[4].value)==false)
                    { tbl.AddrRange.options.remove(i);return;}
                    tbl.idSel[1].checked=true;
                    var option = document.createElement("OPTION");
                    option.text = tbl.AddrRange.options[i].text;
                    option.value = tbl.AddrRange.options[i].value;
                    tbl.addrList.options.add(option);
                    dspChange();
                }
                tbl.AddrRange.options.remove(i);
                break;
            }
        }
    }
}

//保存之前检查数据是否为空
function checkBeforSave()
{
    var tbl=FwAddr;
    var count=tbl.groupType.selectedIndex;
    if(tbl.idSel[0].checked==true)
    {
        switch(count)
        {
            case 0:
                var count1=tbl.idProtocol.selectedIndex;
                if(count1<2) {
                    if( tbl.sFromPort.value!=""|| tbl.sToPort.value!=""||tbl.dToPort.value!=""||tbl.dFromPort.value!="")return true;
                }else {
                    if(tbl.ProtocolNum.value!="")return true;
                }
                break;
            case 1:
                if(tbl.URLaddress.value!="")return true;
                break;
            case 2:
                if(tbl.Keyword.value!="")return true;
                break;
            case 3:
                if(tbl.DNSAddress.value!="")return true;
                break;
            case 4:
                if(tbl.MACaddress.value!="")return true;
                break;
            default:
                break;

        }
        return false;
    }

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
    var tbl=FwAddr;
	if(!checkSpecialChar(tbl.groupName.value))
	{
	    return false;
	}
    if(!checkName(tbl.groupName,11,"服务组名",0)) return false;
    if((tbl.groupName.value.indexOf("\()")!=-1) || (tbl.groupName.value.indexOf(";")!=-1))
    {
        alert("服务组名中不应该包含\(,;");
        return false;
    }
    if(tbl.Action.value == "add")
    {
        for(var i=0;i<groupNames.length;i++)
        {
            if(groupNames[i].toLowerCase()==(tbl.groupName.value).toLowerCase())
            {
                alert("已包含组名"+tbl.groupName.value);
                return false;
            }
        }
    }
    if(tbl.Action.value == "add")
    {
        if (eval("99")< 1)
        {
            alert("没有空闲实例数可增加。");
            return false;
        }
        for(var i=0;i<definedServ.length;i++)
        {
            if(definedServ[i].toLowerCase()==(tbl.groupName.value).toLowerCase())
            {
                alert("预定义组包含组名"+tbl.groupName.value);
                return false;
            }
        }
        if("all_service"==(tbl.groupName.value).toLowerCase())
        {
            alert("预定义组包含组名"+tbl.groupName.value);
            return false;
        }
    }
    if(tbl.AddrRange.options.length==0)
    {
        alert("不能保存内容为空的组");
        return false;
    }
    return true;
}


function save()
{
    var tbl=FwAddr;
    var count=tbl.groupType.selectedIndex;
    //选择新的服务
    /*
    if(tbl.idSel[0].checked==true)
    {
        if(checkBeforSave()==true)
            if(addServOption()==false)return;
    }
    */

    if(checkDataBeforeSave()==false)return;

    tbl.temp1.value="";
    for(var i=0;i<tbl.AddrRange.options.length;i++)
    {
        tbl.temp1.value+=tbl.AddrRange.options[i].value;
        tbl.temp1.value+=";";
    }
    for(var j=tbl.AddrRange.options.length;j<10;j++)
    {
        tbl.temp1.value+=";";
    }
    tbl.groupName.disabled=false;
    tbl.groupType.disabled=false;
    FwAddr.action="/goform/formPortGroupConfig";
    FwAddr.submit();
}

var flag = false;


function dspChange() {

    var tbl=FwAddr;
    if (FwAddr.idSel[0].checked == true) {
        idList.style.display = 'none';
        idTxt.style.display = '';
    }
    else if (FwAddr.idSel[1].checked == true) {
        idList.style.display = '';
        idTxt.style.display = 'none';
    }

    flag = !flag;
}

function SelType(ctl)
{
    var tbl=FwAddr;
    showTable(normaltbl,0);
    showTable(URLtbl,0);
    showTable(Keytbl,0);
    showTable(MACtbl,0);
    showTable(DNStbl,0);
    switch(ctl.value)
    {
        case "portGroup" :
            showTable(normaltbl,1);
        showTable(URLtbl,0);
        showTable(Keytbl,0);
        showTable(MACtbl,0);
        showTable(DNStbl,0);
        changeType();

        break;
        case "urlGroup" :
            showTable(normaltbl,0);
        showTable(URLtbl,1);
        showTable(Keytbl,0);
        showTable(MACtbl,0);
        showTable(DNStbl,0);
        changeType();
        break;
        case "keyGroup" :
            showTable(normaltbl,0);
        showTable(URLtbl,0);
        showTable(Keytbl,1);
        showTable(MACtbl,0);
        showTable(DNStbl,0);
        changeType();
        break;
        case "dnsGroup" :
            showTable(normaltbl,0);
        showTable(URLtbl,0);
        showTable(Keytbl,0);
        showTable(MACtbl,0);
        showTable(DNStbl,1);
        changeType();
        break;
        /*
           case "MAC" :
           showTable(normaltbl,0);
           showTable(URLtbl,0);
           showTable(Keytbl,0);
           showTable(MACtbl,1);
           showTable(DNStbl,0);
           changeType();
         */
        default:break;
    }
}

function SelProtlcol(ctl)
{
    var tbl=FwAddr;
    showTable(TUtbl,0);
    showTable(othertbl,0);
    tbl.sFromPort.value="";
    tbl.sToPort.value="";
    tbl.dFromPort.value="";
    tbl.dToPort.value="";
    tbl.ProtocolNum.value="";

    switch(ctl.value)
    {
        case "TCP" :
            showTable(TUtbl,1);
        showTable(othertbl,0);
        break;
        case "UDP" :
            showTable(TUtbl,1);
        showTable(othertbl,0);
        break;
        case "Other" :
            showTable(TUtbl,0);
        showTable(othertbl,1);
        default:break;
    }
}

function dsplist(index,list)
{
    if(index==0) dspList(index,list,648,titles1,3,arrayData1,1,1,0,0,0);
}

window.onload = function(){
    init();
};
</script>
</head>
<body onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle('服务组配置');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>

  <tr valign="top">
  
    <td><form action="/goform/"  name="FwAddr" method="post">
<table width="95%" border="0" bordercolor="red" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
  <td align="center" class="style7">
	 <table width="100%" border="0" bordercolor="blue" cellspacing="0" cellpadding="0" align="center">
	   <tr>
	     <td>
       	   <table width="100%" border="0" bordercolor="green" cellspacing="0" cellpadding="0" align="center">
           <p>&nbsp;</p>
          
 			  <tr>
		      		<td width="414">&nbsp;</td>
		      		<td width="133" height="30">服务组名</td>
		      		<td width="535"><input name="groupName" type="text" value="" checked="checked"  onclick=""  class="input" maxlength="11" /></td>
                    <input type="hidden" name="groupNameOld">
		      </tr>
		   	  <tr>
		      		<td width="414">&nbsp;</td>
		      		<td height="30">服务类型</td>
		      		<td width="535"><select name="groupType" class="input" id="groupType" onChange="SelType(this)">
			 		<option value="portGroup" selected="selected">普通服务</option>
			  		<option value="urlGroup">URL</option>
			  		<option value="keyGroup">关键字</option>
			  		<option value="dnsGroup">DNS</option>
		     	    </select>
                    </td>
		      </tr>
		  </table>
		  <input type="hidden" name="temp1" size=10 value="1">
	      <input type="hidden" name="hidVal">
		 </td>
	    </tr>
	   <tr>
		<td height="26"><div align="center">
		  <table style="display" width="100%"  border="0" bordercolor="yellow" align="center" cellpadding="0" cellspacing="0">
		    <tr>
		      <td align="right" valign="top" colspan="3"><table style="display" width="58%"  border="0" bordercolor="red" align="right" cellpadding="0" cellspacing="0">
			  <tr>
			    <td colspan="3"><table style="display" width="93%"	border="0" bordercolor="green" align="center" cellpadding="0" cellspacing="0">
				<tr>
				  <td width="10%" height="36"></td>
				  <td valign="middle" colspan="2"><input type="radio" id="idSel" name="idSel" checked="checked" onClick="dspChange()" />
				    &nbsp;新服务 </td>
				  <td width="50%"><input type="radio" id="idSel" name="idSel" onClick="dspChange()" />
				  &nbsp;已有服务 </td>
				</tr>
                <tr style="display:none;">
                <td width="10%" height="36"></td>
                </tr>
			    </table></td>
			  </tr>
			  <tr id="idTxt">
			    <td colspan="3" align="right"><table width="96%"  border="0" cellspacing="0" cellpadding="0" >
				<tr>
				  <td width="3%">&nbsp;</td>
				  <td width="95%" align="right"><div style="height:130px;width:200px;border: 1px solid #adafae" >
				      <table width="210"  border="0" cellspacing="0" cellpadding="0" id="normaltbl" style="display: ">
					<tr>
					  <td width="49%" valign="middle" align="right">选择协议</td>
					  <td width="5%">&nbsp;</td>
					  <td width="46%"  height="25" ><select id="idProtocol" name="idProtocol"  style="width:80" class="input" onChange="SelProtlcol(this)">
					      <option value="TCP">TCP</option>
					      <option value="UDP">UDP</option>
					      <option value="Other">其他协议</option>
					    </select>
					  </td>
					</tr>
					<tr>
					  <td colspan="3" align="center"><table width="256"  border="0" cellspacing="0" cellpadding="0" id="TUtbl" style="display:">
					      <tr>
						<td width="49%" valign="middle" align="right">源起始端口</td>
						<td width="5%">&nbsp;</td>
						<td width="46%" height="25" ><input name="sFromPort" type="text" style="IME-MODE:   disabled;"class="input" id="sFromPort" size="10" onKeyPress="figureOnly()" />
						</td>
					      </tr>
					      <tr>
						<td valign="middle" align="right">源结束端口</td>
						<td width="5%">&nbsp;</td>
						<td height="25"><input name="sToPort" type="text" style="IME-MODE:   disabled;"class="input" id="sToPort" size="10" onKeyPress="figureOnly()" />
						</td>
					      </tr>
					      <tr>
						<td valign="middle" align="right">目的起始端口</td>
						<td width="5%">&nbsp;</td>
						<td height="25" ><input name="dFromPort" type="text" style="IME-MODE:	disabled;"class="input" id="dFromPort" size="10" onKeyPress="figureOnly()" />
						</td>
					      </tr>
					      <tr>
						<td  valign="middle" align="right">目的结束端口</td>
						<td width="5%">&nbsp;</td>
						<td height="25"><input name="dToPort" type="text" style="IME-MODE:   disabled;"class="input" id="dToPort" size="10" onKeyPress="figureOnly()" />
						</td>
					      </tr>
					    </table>
					      <table width="200"  border="0" cellspacing="0" cellpadding="0" id="othertbl" style="display:none ">
						<tr>
						  <td width="42%" valign="middle" align="right">&nbsp;</td>
						  <td width="5%">&nbsp;</td>
						  <td height="20" colspan="3">&nbsp;</td>
						</tr>
						<tr>
						  <td width="42%" valign="middle" align="right">协议号</td>
						  <td width="5%">&nbsp;</td>
						  <td height="25" colspan="3"><input name="ProtocolNum" style="IME-MODE:   disabled;"type="text" class="input" id="ProtocolNum" size="10" onKeyPress="figureOnly()" />
						  </td>
						</tr>
					    </table></td>
					</tr>
				      </table>
				    <table width="200"	border="0" cellspacing="0" cellpadding="0" id="URLtbl" style="display:none ">
					<tr>
					  <td width="35%" valign="middle" align="right">&nbsp;</td>
					  <td width="5%">&nbsp;</td>
					  <td height="40" colspan="3">&nbsp;</td>
					</tr>
					<tr>
					  <td width="35%" valign="middle" align="right">URL地址</td>
					  <td width="5%">&nbsp;</td>
					  <td height="25" colspan="3"><input name="URLaddress" type="text" onKeyPress="checkChar()" class="input" id="URLaddress" maxlength="28"size="10" /></td>
					</tr>
				    </table>
				    <table width="200"	border="0" cellspacing="0" cellpadding="0" id="Keytbl" style="display:none">
					<tr>
					  <td width="35%" valign="middle" align="right">&nbsp;</td>
					  <td width="5%">&nbsp;</td>
					  <td height="40" colspan="3">&nbsp;</td>
					</tr>
					<tr>
					  <td width="35%" valign="middle" align="right">关键字</td>
					  <td width="5%">&nbsp;</td>
					  <td height="25" colspan="3"><input name="Keyword" type="text" class="input" id="Keyword" maxlength="28"size="10"onkeypress="checkChar()" />
					  </td>
					</tr>
				    </table>
				    <table width="200"	border="0" cellspacing="0" cellpadding="0" id="DNStbl" style="display:none">
					<tr>
					  <td width="35%" valign="middle" align="right">&nbsp;</td>
					  <td width="5%">&nbsp;</td>
					  <td height="40" colspan="3">&nbsp;</td>
					</tr>
					<tr>
					  <td width="35%" valign="middle" align="right">DNS</td>
					  <td width="5%">&nbsp;</td>
					  <td height="25" colspan="3"><input name="DNSAddress" type="text" class="input" id="DNSAddress" maxlength="28"size="10"onkeypress="checkChar()" />
					  </td>
					</tr>
				    </table>
				    <table width="200"	border="0" cellspacing="0" cellpadding="0" id="MACtbl" style="display:none ">
					<tr>
					  <td width="35%" valign="middle" align="right">&nbsp;</td>
					  <td width="5%">&nbsp;</td>
					  <td height="40" colspan="3">&nbsp;</td>
					</tr>
					<tr>
					  <td width="35%" valign="middle" align="right">MAC地址</td>
					  <td width="5%">&nbsp;</td>
					  <td height="25" colspan="3"><input name="MACaddress" type="text" style="IME-MODE:   disabled;"class="input" id="MACaddress" maxlength="12"size="12" onKeyPress="macOnly()" />
					  </td>
					</tr>
				    </table>
				  </div></td>
								  <td width="2%">&nbsp;</td>
				</tr>
			    </table></td>
			  </tr>
			  <tr>
			    <td colspan="3"><table style="display:none;" width="100%"  border="0" align="center" cellpadding="0" cellspacing="0" id="idList">
				<tr>
				  <td width="15%">&nbsp;</td>
				  <td width="75%" align="right" valign="top"><div align="center" style="width=200px;overflow-x:auto;">
				      <select  class="input" name="addrList" size="8" multiple="multiple"  id="addrList" ondblclick="" style="width:200px; background-color:#FFFFFF;" border="0" >
				      </select>
				  </div></td>
								  <td width="10%">&nbsp;</td>
				</tr>
			    </table></td>
			  </tr>
		      </table></td>
		      <td width="52%" valign="top"><table style="display" width="100%"	border="0" align="center" cellpadding="0" cellspacing="0">
			  <tr>
			    <td width="206" height="37" valign="middle">&nbsp;</td>
			    <td align="center" width="192">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font style="FONT-FAMILY: Arial, Helvetica, sans-serif;FONT-SIZE: 12px;">服务组列表:</font></td>
			    <td width="164"></td>
			  </tr>
			  <tr>
			    <td rowspan="4"><table id="idImg">
				<tr>
				  <td width="98" align="left">
				    <input type="button" name="Submit32" value="  ==&gt;  " onClick="fromXXtoXX(0)" />
				 </td>
				</tr>
				<tr>
				  <td height="30" align="left">
				    <input type="button" name="Submit42" value="  &lt;==  " onClick="newfromXXtoXX()"/>
			      </td>
				</tr>
				<tr>
				  <td height="30" align="left">
				    <input type="button" name="Submit52" value=" 删 除 "  onclick="delAddrRange()"/>
				</td>
				</tr>
			    </table></td>
			    <td width="192">
				<select class="input" name="AddrRange" id="AddrRange" ondblclick="newfromXXtoXX()" size="8" style="width:180px" border="0" >
				</select>
			    </td>
			  </tr>
		      </table></td>
		    </tr>
		  </table>
		</div>
        </td>
	   </tr>
	      <tr>
		<td>&nbsp;</td>
	      </tr>
	      <tr>
		<td>&nbsp;</td>
	      </tr>
	    </table>
		
			<table align="center" width="40%" border="0" bordercolor="pink">
            <tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
		    <tr>
		      <td   height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>" onClick="Reset()">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#UserGroup')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_back);%>" onClick="GoBack_Page('FwService.asp')"></td>
		    </tr>
	      </table>
          
		  <table width="100%" border="0" cellpadding="0" cellspacing="0" style="display:none">
		   <tr>
		      <td width="413">&nbsp;</td>
		      <td width="107" height="30">服务组名</td>
		      <td width="684"><input name="" type="text" value="" checked="checked"  onclick=""  class="input" maxlength="11" /></td>
	    </tr></table>
        
			<table align="center" width="40%" border="0" bordercolor="pink">
	   <tr  height="40">
       <!--<td width="10%" height="40">&nbsp;</td>-->
	   <td class="helpfont" valign="middle" id="attention3"><strong>注意：</strong>
	    服务组最多可配置包含10个服务或其它服务组。</td></tr>
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

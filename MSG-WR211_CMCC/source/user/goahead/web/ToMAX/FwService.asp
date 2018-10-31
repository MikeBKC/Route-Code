<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("UserGroup");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml"> 
var strSecondMenuFocus="服务组配置";

//交互变量

var errorstr="";
var totalrecs; var groupNames = new Array(); var line = new Array(new Array()); var line1 = new Array(); var line2 = new Array(); var line3 = new Array(); var line4 = new Array(); var line5 = new Array(); var line6 = new Array(); var line7 = new Array(); var line8 = new Array(); var line9 = new Array(); var line10 = new Array(); var type = new Array();totalrecs=0;
<!-- 测试 -->
<!-- groupNames[0] = "1"; -->
<!-- line1[0] = "T(10-20,30-40)";line2[0] = "";line3[0] = "";line4[0] = "";line5[0] = "";line6[0] = "";line7[0] = "";line8[0] = "";line9[0] = "";line10[0] = "";-->
<!-- line[0] = new Array(line1[0],line2[0],line3[0],line4[0],line5[0],line6[0],line7[0],line8[0],line9[0],line10[0]); -->
<!-- type[0] = "普通服务"; -->
<!-- 测试 -->
totalrecs=1;
<!--  打印   -->
<%aspOutputPortGroupListData();%>
<!--  打印   -->

//获取linkPage,初始化到linkPage按钮
var locationStr = window.location.toString();
var locationIndex = locationStr.indexOf("?");
var linkPage = null;
if (locationIndex != -1) {
    linkPage = locationStr.substring(locationIndex + 1);
}

var color = "Green";
var shorttype=new Array("T(","U(","O(","L(","K(","D(","M(");
var reg=new Array(/^[Tt]\(\d+-\d+,\d+-\d+\)/,/^[Uu]\(\d+-\d+,\d+-\d+\)/,/^[Oo]\(\d+\)/,/^[Ll]\(/,/^[Kk]\(/,/^[Dd]\(/,/^[Mm]\(/,/^[Gg]\(/,/^[Cc]\(/);
var reg_tcpudp = /[\)\(,-]/;
var reg_other = /[\)\(]/;
var definedServ=new Array(
        "ftp","ssh","telnet","smtp","sql*net","finger","web","pop3","epmap","netbios_ssn","https","ms-ds","ms-sql-s","ms-sql-m","h.323","pptp","msnlogin","ms-ts",
        "dns","bootps","bootpc","tftp","ntp","netbios-ns","netbios-dgm","snmp","snmptrap","isakmp","l2tp",
        "icmp","igmp","ipinip","gre","esp","ah","ospf","igrp","rsvp"
        );

var line1 = new Array();
var containType=new Array();
for(var i=0;i<totalrecs;i++)
{
    line1[i] = new Array();
    containType[i] = 0;
    for(var j=0;j<10;j++)
    {
        for(var m=0;m<reg.length;m++)
        {

        }
        if((line[i][j].length!=0)&&(reg[0].test(line[i][j])==false)&&(reg[1].test(line[i][j])==false)&& (reg[2].test(line[i][j])==false)&&(reg[3].test(line[i][j])==false)&&(reg[4].test(line[i][j])==false)&&(reg[5].test(line[i][j])==false)&&(reg[6].test(line[i][j])==false)&&(line[i][j].length < 12))
        {
            containType[i] = 1;
            break;
        }
    }
}
var formatType = new Array();
for(var i = 0;i < type.length;i++)
{
    if(type[i] == "portGroup") {
        formatType[i] = "普通服务";
    }else if(type[i] == "dnsGroup") {
        formatType[i] = "DNS";
    }else if(type[i] == "keyGroup") {
        formatType[i] = "关键字";
    }else if(type[i] == "urlGroup") {
        formatType[i] = "URL";
    }
}

var areas=new Array();
var isname=true;

function getIndexOfArr(str, arr) {
    for (var ii = 0; ii < arr.length; ii++) {
        if (str == arr[ii]) {
            return ii;
        }
    }
    return -1;
}
for(var i=0;i<totalrecs;i++)
{
    areas[i]="";
    for(var j=0;j<10;j++)
    {
        isname=true;
        if(line[i][j]!="")
        {
            for(var k=0;k<reg.length;k++)
            {
                if(reg[k].test(line[i][j])==true) {isname=false;break;}
            }
            if((isname==true)&&(line[i][j].length<12)){isname=true;}
            else{isname=false;}
            if(isname)
            {
                if (getIndexOfArr(line[i][j], definedServ) != -1) {
                    line1[i][j] = line[i][j];
                } else {
                    line1[i][j] = getRemarkById(line[i][j]);
                }
                areas[i]=areas[i].concat(line1[i][j]);
                areas[i]+=" ; ";
            }else{
                line1[i][j] = line[i][j];
                //areas[i]+="{";
                areas[i]=areas[i].concat(line1[i][j]);
                //areas[i]+="}";
                areas[i]+=" ; ";
            }
        }
    }
    if(areas[i].length>60)
    {
        areas[i]=areas[i].slice(0,60);
        areas[i]+="	<B>...</B>";
    }
}

var maxlen_name = "31";
var nIndex = 0;

function strIsName(str)
{
    for(var i=0;i<reg.length;i++)
    {
        if(reg.test(str)==true)return false;
    }
    if(str.length<12)
        return true;
    else return false;
}


function RemarkExist(remark1)
{
    for (i=0;i<totalrecs;i++)
    { if(remark1==remark[i])
        {
            alert("服务组名称已经存在！");
            return false;
        }
    }
    return true;
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

//加上text值页面会乱掉
function checkdata()
{
    var temp=/[^\x00-\xff]/;
    var tbl=FwAddr;
    var checkright=true;
    var text="";
    if(tbl.Action[0].checked==1){
        if (!RemarkExist(tbl.remark.value))
        {
            return false;
        }
    }
    if(temp.test(tbl.sFromPort.value)||temp.test(tbl.sToPort.value)||temp.test(tbl.dToPort.value)||temp.test(tbl.dFromPort.value))
    {
        alert("端口内容包含非法字符");return false;
    }
    if(tbl.sFromPort.value=="")
    {
        text+="源起始端口不能为空\n";checkright=false;
    }


    if(parseInt(tbl.sFromPort.value)>65535)
    {
        text+="源起始端口不能大于65535\n";checkright=false;
    }

    if(tbl.sToPort.value=="")
    {
        text+="源结束端口不能为空\n";checkright=false;
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
    if(parseInt(tbl.dFromPort.value)>65535)
    {
        text+="目的起始端口不能大于65535\n";checkright=false;
    }

    if(tbl.dToPort.value=="")
    {
        text+="目的结束端口不能为空\n";checkright=false;
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

function isMyself(str) {
    var ismyslef = false;

    if (str == FwAddr.groupName.value)  {
        ismyslef = true;
    }

    return ismyslef;
}

maxrecs[0] = eval("100");
//max_totalrecs = 100;
recsPerPage[0] =10;
lineButton[0] =10;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="服务组信息列表";
recscount[0]=totalrecs;
recsPerPage[0]=10;
lineButton[0]=10;   //显示行数默认值为10
//tabCount[0]=2;		    // 点新建跳的table
//newButton[0]=true;	   //是否有新建按钮
currpage[0]=1

var titles1 = new Array();

titles1[0] = "服务组名";
titles1[1] = "服务组类型";
titles1[2] = "服务组范围";
titles1[3] = "ID";

sortable[0] = new Array(1,1,1,1,1,1,1);
//sortType[0] = new Array();
sortType[0] =  new Array(TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR);
arrayCount[0] = 7;

var arrayData1=new Array(groupNames, formatType, areas,groupNames,line,containType,line1);

var flag = false;

function dsplist(index,list)
{
    if(index==0) dspList(index,list,920,titles1,3,arrayData1,1,1,0,0,0);
}

function EditConfig(val,i){
    if(val==0){ 
        FwAddr.id.value="";
        if(eval(max_totalrecs-totalrecs)<1)	{
            alert("<%write(com_editAlert);%>");
            return;
        }
    }
    else FwAddr.id.value=groupNames[i];
    FwAddr.action="FwService_edit.asp";
    FwAddr.submit();
}
function DeleteConfig(index){
    if(confirm("<%write(com_deleteTip);%>")){
        FwAddr.delstr.value=groupNames[index];
        FwAddr.action="/goform/formPortGroupListDel";
        FwAddr.submit();
    }
}
function delAll(){
    if(confirm("<%write(com_deleteAllTip);%>")){
        FwAddr.action="/goform/formPortGroupListDelAll";
        FwAddr.submit();
    }
}

</script>
</head>
<body onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<!--*****顶上间隔****-->
<tr>
<td colspan="3">&nbsp;</td>
</tr>
<!--****中间内容****-->
<tr>
<td width="15"></td>
<!--////内容-->
<td><script language="javascript">showSecondTitle("服务组配置");</script>
<table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
    <script language="javascript">showerror(errorstr,80);</script>
    <tr valign="top">
        <td >
            <table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
            <p>&nbsp;</p>
            <tr>
            <td align="center" class="style7">
            <!--<table id=grouplist style="display" width="80%"  border="0" align="center" cellpadding="0" cellspacing="0">-->
            <table id=grouplist  width="920"  border="0" align="center" cellpadding="0" cellspacing="0">
                <form action="/goform/formPortGroupListDel" method="post" name="FwAddr" id="FwAddr">
                <tr><td colspan="4" height="20">&nbsp;</td></tr>

                <tr bgcolor="c0ddff">
                    <td colspan="4">
                        <iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="920" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe>
                    </td>
                </tr>
                <tr bgcolor="c0ddff">
                    <td colspan="4">
                        <iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="920" height="260" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe>
                    </td>
                </tr>
                <tr>
                    <td  colspan="4" valign="bottom">
                    <table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
                    <tr>
                        <td width="5"></td>
                        <td  class="style6">
                            <div align="left">
                                <input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
                                <script language="javascript">document.write(_("全选 / 全不选"));</script>
                            </div>
                        </td>
                        <td  class="style6" >
                            <input type="hidden" name="delstr" size=10 ><input type="hidden" name="id">
                        </td> 
                        <td width="90" valign="top" align="right">
                            <input type="button" value="添加新条目" class="input-button" onClick="EditConfig(0)" id="add">
                        </td>
                        <td width="95" valign="top" align="right">
                            <input type="button" value="删除所有条目" class="input-button" onClick="delAll()" id="delall">
                        </td>
                        <td width="45" valign="top" align="right">
                            <input type="submit" value="删除" class="input-button" onClick="return(genData(FwAddr,0,frame12,groupNames,'服务组配置信息'))" id="del">
                        </td>
                    </tr>
                    </table>
                    </td>
                </tr>
                <tr><td colspan="4" height="20">&nbsp;</td></tr>
                </form>
                </table>
</td></tr></table>
</form></td>
</tr>
</table></td>
<td width="15"></td>
</tr>
</table>
</body>
</html>


/**
判断汇聚配置argAggrArr
相对于vlan配置argVlanArr 是否合法
规则为若任何汇聚都完全属于某个vlan则合法，否则不合法
合法返回-1，否则返回首个不合法的汇聚组下标
**/
<%langTran("commonWords");%>
function checkAggrVlan(argAggrArr,argVlanArr)
{
	var full_flag=0;//是否完全属于一个vlan
	var i,j,k;
	for(i=0;i<argAggrArr.length;i++)
	{//port vlan
		if(argAggrArr[i].length < 2) continue;//vlan组的最小成员个数为1(不设置port vlan的端口每个端口等于一个vlan)
		full_flag=0;
		for (j = 0; j < argVlanArr.length; j++) {
			k=isArrayFullInclude(argVlanArr[j],argAggrArr[i]);
			if (k == 1) {//完全包含
			   full_flag=1;
			   break;
			}
		}
		if(!full_flag)
		//该汇聚组不完全属于任何一个vlan
		{
			 return i;//返回配置错误的汇聚组编号
			 break;
		}
	}
	return -1;
}
/*
联动管理出错时的处理函数
自动生成form并将所需数据发送给destUrl
注意:此函数中的全局变量必须事先定义
*/
function urcpErrorPost(destUrl,notAlert)
{
	if(urcpError)
	{
		if(urcpError.indexOf("密码错误")>=0)
		{//密码错误则关闭当前窗口
			closeWin(urcpError,notAlert);
			return true;
		}
		document.write('<form method="POST" name="URCPERROFROM" action="'+destUrl+'">')
		document.write('<input type="hidden" value="Apply" name="_submit">');
		document.write('<input type="hidden" value="'+ipStr+'" name="ipStr">');
		document.write('<input type="hidden" value="'+macStr+'" name="macStr">');
		document.write('<input type="hidden" value="'+passStr+'" name="passStr">');
		document.write('</form>');
		if(!notAlert) alert(urcpError);
		URCPERROFROM.submit();
		return true;
	}
	return false;//无错误
}
/**
批量操作结果生成函数
title为结果弹出层的标题
onlyShowErro为是否只显示结果中出错的信息
若打开了弹出层则返回true，否则返回false
**/
function showBatchResult(errorArr,ipStr,macStr,title,onlyShowErro)
{
	var num = onlyShowErro?(errorArr.length-errorArr.countData("")): errorArr.length;//总共显示多少条目个信息。
	//信息条目超过此数则一行显示多条信息。通过设置此值可降低滚动条出现的几率（此值越小，滚动条出现几率越低）
	var maxPartitionNum=20;
	if(num<1)
	{
		return false;
	}
	else
	{
		var trNum=0,j=0,k=0;
		var indexArr=new Array();
		var moveDivObj=null;//可移动层生成类
		var ipArr = ipStr.split(',');
		var macArr = macStr.split(',');	
		var tmpstr="";
		for(j=0;j<macArr.length;j++)
		{
			macArr[j]=macArr[j].replace("/-/g","");
			macArr[j]=macArr[j].replace("/:/g","");
			tmpstr="";
			for(k=0;k<macArr[j].length;k++)
			{
				if(k && k%2==0)
				{
					tmpstr+=":";//用-分割可能导致表格过长，故用:分割
				}
				tmpstr+=macArr[j].charAt(k);
			}
			macArr[j]=tmpstr;
		}
		var thArr;
		if(num<=maxPartitionNum ) thArr = new Array("<%write(cw_ip);%>","<%write(cw_mac);%>","<%write(cw_result);%>");
		else  thArr = new Array("<%write(cw_ip);%>","<%write(cw_mac);%>","<%write(cw_result);%>","<%write(cw_ip);%>","<%write(cw_mac);%>","<%write(cw_result);%>");//一行显示多条信息
		var tbArr = new Array(ipArr,macArr,errorArr);
		var divObj = document.createElement("div");
		var tb = document.createElement("table");//生成表格
		tb.style.cssText="margin-top: 6px;margin-bottom: 1px;";
		tb.className="style22";
		tb.cellSpacing = 1;
		tb.width = num >maxPartitionNum?500:250;
		var tr = tb.insertRow(trNum++);
		tr.className="style18";
		tr.align="center";
		var td = null;
		//表格插入表头
		for(j=0;j<thArr.length;j++)
		{
			td = tr.insertCell(j);
			td.innerHTML = thArr[j];
		}
		//初始化table数组中要显示的列索引
		for(j=0,k=0;k<tbArr[0].length;k++)
		{
			if(tbArr[2][k]=="")
			{
				if(onlyShowErro)
					continue;
				else
				{
					tbArr[2][k]="<%write(cw_success);%>";
				}	
			}
			indexArr[j++]=k;
		}
		//插入表格主体
		for(k=0;k<indexArr.length;k++)
		{
			tr=tb.insertRow(trNum++);//插入一行
			tr.align="center";
			if((trNum-1)%2==0)
			{
				tr.className="style18";
			}
			for(j=0;j<thArr.length;j++)
			{
				if(j==tbArr.length)//每行显示两条信息
				{
					k++;//不插入新行，但插入新信息
				}
				td = tr.insertCell(j);
				if(k<indexArr.length)
				{
					td.innerHTML = tbArr[j%tbArr.length][indexArr[k]];//j可能大于tbArr的长度，故用取余的方法
				}
				else
				{
					td.innerHTML = "";
				}
			}
		}
		divObj.appendChild(tb);
		document.body.appendChild(divObj);//生成的表格附加到body
		if(tb.offsetHeight>100)
		{
			/*此时显示滚动条，故需计算滚动条宽度*/
			divObj.style.cssText="overflow:auto;height:100px;width:"+(tb.offsetWidth+20)+"px";
		}
		else
		{
			/*此时不显示滚动条*/
			divObj.style.cssText="height:"+tb.offsetHeight+"px;width:"+tb.offsetWidth+"px";
		}
		if(tb)//生成可移动层
		{
			moveDivObj = moveDiv(divObj);
			moveDivObj.init(title);//初始化
			moveDivObj.show();//显示，可用hide方法隐藏
			return true;
		}	
	}
}
/**
根据扫描网段获取arp 扫描的等待时间
**/
function getArpScanWaitTime(netMask)
{
	var ret=0;
	var sendTime=new Array(0.31,0.46,0.73,1.47,2.74,5.5,9.8);/*发包时间对应掩码24-18*/
	var receiveTime=new Array(3,3,3,3,3,4,5);/*收包时间对应掩码段24-18*/
	netMask=parseInt(netMask);
	if(netMask>=24)/*24-32*/
	{
		ret=sendTime[0]+receiveTime[0];
	}
	else if(netMask>=18)/*18-24*/
	{
		ret=sendTime[24-netMask]+receiveTime[24-netMask];
	}
	else
	{
		ret=sendTime[24-18]+receiveTime[24-18];/*最小掩码为16*/
	}
	return Math.round(ret);
}


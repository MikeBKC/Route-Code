/**
�жϻ������argAggrArr
�����vlan����argVlanArr �Ƿ�Ϸ�
����Ϊ���κλ�۶���ȫ����ĳ��vlan��Ϸ������򲻺Ϸ�
�Ϸ�����-1�����򷵻��׸����Ϸ��Ļ�����±�
**/
<%langTran("commonWords");%>
function checkAggrVlan(argAggrArr,argVlanArr)
{
	var full_flag=0;//�Ƿ���ȫ����һ��vlan
	var i,j,k;
	for(i=0;i<argAggrArr.length;i++)
	{//port vlan
		if(argAggrArr[i].length < 2) continue;//vlan�����С��Ա����Ϊ1(������port vlan�Ķ˿�ÿ���˿ڵ���һ��vlan)
		full_flag=0;
		for (j = 0; j < argVlanArr.length; j++) {
			k=isArrayFullInclude(argVlanArr[j],argAggrArr[i]);
			if (k == 1) {//��ȫ����
			   full_flag=1;
			   break;
			}
		}
		if(!full_flag)
		//�û���鲻��ȫ�����κ�һ��vlan
		{
			 return i;//�������ô���Ļ������
			 break;
		}
	}
	return -1;
}
/*
�����������ʱ�Ĵ�����
�Զ�����form�����������ݷ��͸�destUrl
ע��:�˺����е�ȫ�ֱ����������ȶ���
*/
function urcpErrorPost(destUrl,notAlert)
{
	if(urcpError)
	{
		if(urcpError.indexOf("�������")>=0)
		{//���������رյ�ǰ����
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
	return false;//�޴���
}
/**
��������������ɺ���
titleΪ���������ı���
onlyShowErroΪ�Ƿ�ֻ��ʾ����г������Ϣ
�����˵������򷵻�true�����򷵻�false
**/
function showBatchResult(errorArr,ipStr,macStr,title,onlyShowErro)
{
	var num = onlyShowErro?(errorArr.length-errorArr.countData("")): errorArr.length;//�ܹ���ʾ������Ŀ����Ϣ��
	//��Ϣ��Ŀ����������һ����ʾ������Ϣ��ͨ�����ô�ֵ�ɽ��͹��������ֵļ��ʣ���ֵԽС�����������ּ���Խ�ͣ�
	var maxPartitionNum=20;
	if(num<1)
	{
		return false;
	}
	else
	{
		var trNum=0,j=0,k=0;
		var indexArr=new Array();
		var moveDivObj=null;//���ƶ���������
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
					tmpstr+=":";//��-�ָ���ܵ��±�����������:�ָ�
				}
				tmpstr+=macArr[j].charAt(k);
			}
			macArr[j]=tmpstr;
		}
		var thArr;
		if(num<=maxPartitionNum ) thArr = new Array("<%write(cw_ip);%>","<%write(cw_mac);%>","<%write(cw_result);%>");
		else  thArr = new Array("<%write(cw_ip);%>","<%write(cw_mac);%>","<%write(cw_result);%>","<%write(cw_ip);%>","<%write(cw_mac);%>","<%write(cw_result);%>");//һ����ʾ������Ϣ
		var tbArr = new Array(ipArr,macArr,errorArr);
		var divObj = document.createElement("div");
		var tb = document.createElement("table");//���ɱ��
		tb.style.cssText="margin-top: 6px;margin-bottom: 1px;";
		tb.className="style22";
		tb.cellSpacing = 1;
		tb.width = num >maxPartitionNum?500:250;
		var tr = tb.insertRow(trNum++);
		tr.className="style18";
		tr.align="center";
		var td = null;
		//�������ͷ
		for(j=0;j<thArr.length;j++)
		{
			td = tr.insertCell(j);
			td.innerHTML = thArr[j];
		}
		//��ʼ��table������Ҫ��ʾ��������
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
		//����������
		for(k=0;k<indexArr.length;k++)
		{
			tr=tb.insertRow(trNum++);//����һ��
			tr.align="center";
			if((trNum-1)%2==0)
			{
				tr.className="style18";
			}
			for(j=0;j<thArr.length;j++)
			{
				if(j==tbArr.length)//ÿ����ʾ������Ϣ
				{
					k++;//���������У�����������Ϣ
				}
				td = tr.insertCell(j);
				if(k<indexArr.length)
				{
					td.innerHTML = tbArr[j%tbArr.length][indexArr[k]];//j���ܴ���tbArr�ĳ��ȣ�����ȡ��ķ���
				}
				else
				{
					td.innerHTML = "";
				}
			}
		}
		divObj.appendChild(tb);
		document.body.appendChild(divObj);//���ɵı�񸽼ӵ�body
		if(tb.offsetHeight>100)
		{
			/*��ʱ��ʾ�����������������������*/
			divObj.style.cssText="overflow:auto;height:100px;width:"+(tb.offsetWidth+20)+"px";
		}
		else
		{
			/*��ʱ����ʾ������*/
			divObj.style.cssText="height:"+tb.offsetHeight+"px;width:"+tb.offsetWidth+"px";
		}
		if(tb)//���ɿ��ƶ���
		{
			moveDivObj = moveDiv(divObj);
			moveDivObj.init(title);//��ʼ��
			moveDivObj.show();//��ʾ������hide��������
			return true;
		}	
	}
}
/**
����ɨ�����λ�ȡarp ɨ��ĵȴ�ʱ��
**/
function getArpScanWaitTime(netMask)
{
	var ret=0;
	var sendTime=new Array(0.31,0.46,0.73,1.47,2.74,5.5,9.8);/*����ʱ���Ӧ����24-18*/
	var receiveTime=new Array(3,3,3,3,3,4,5);/*�հ�ʱ���Ӧ�����24-18*/
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
		ret=sendTime[24-18]+receiveTime[24-18];/*��С����Ϊ16*/
	}
	return Math.round(ret);
}


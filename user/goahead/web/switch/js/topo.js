/********************************
����ͼ�ڵ���
*********************************/
function topoNode(parent,mac,ip,deviceType,isVirtual,warningLevel,parentPort,port,vlanId,alivePorts,dev_model,parentAggrMem,aggrMem)
{
	/******
	*��ʼ�����Ա
	*******/
	this.parent=parent;//���ڵ�λ��
	this.mac=mac;
	this.ip=ip;
	this.name="";
	this.deviceType=deviceType;//�豸����(��������·����)
	this.version=dev_model;//�汾��3124R
	this.isVirtual=isVirtual;//�Ƿ������豸
	this.warningLevel=warningLevel;//���漶��
	this.parentPort=parentPort;//���ڵ����ӵ��˽ڵ�Ķ˿ں�
	this.port=port;//�������ӵ����ڵ�Ķ˿ں�
	this.vlandId=vlanId;//���ڵ�vlanId
	//this.isManage=isManage;//�Ƿ�ɹ���
	this.alivePorts=alivePorts;//��˿���
	this.desc="";//������Ϣ
	this.aggrMem=aggrMem;//�����û�ۣ���Ϊ���ڵ��Ӧ�Ļ�۶˿ڳ�Ա(�������û�����ʼ��Ϊ'')
	this.parentAggrMem=parentAggrMem;//�����û�ۣ���Ϊ���ڵ��Ӧ�û�۶˿ڳ�Ա(�������û�����ʼ��Ϊ'')
	
	this.x;//x����
	this.y;//y����
	this.deviceIndex;//��Ӧ���豸����(��Ӧtopo��ʼ�����е�devices)
	
	this.lChild=-1;//�ýڵ������ߵĺ��ӣ�С��0�򲻴���
	this.rChild=-1;
	
	
	/*************
	*��Ա����
	**************/
	if(this.parentAggrMem && this.aggrMem)
	{/*��������˻���� ���ڵ� �˿� �� ���ڵ�˿�����ʾ�ı�Ҫ*/
		this.parentPort="";
		this.port="";
	}
	else
	{/*������ڻ�� �� ���ڵ�������һ��Ϊ�� ��֤������쳣 ������ʾ*/
		this.parentAggrMem = "";
		this.aggrMem = "";
	}
}
/*******************************
topoͼ��ʼ����
********************************/
function topoInit(maxDepth,prefix)
{
	this.maxDepth=maxDepth;//����������
	this.prefix=prefix;//�ڵ�����ı���ǰ׺
	this.topoWidth=600;//����ͼ�ܿ��
    this.topoHeight=400;//����ͼ�ܸ߶�
	this.devices=new Array('../images/switch_normal.png','../images/router_normal.png','../images/device_none.png','../images/switch_warning.png');
	this.deviceWidth=78.6;//�豸ͼƬ�Ŀ��
	this.deviceHeight=30.6;//�豸ͼƬ�߶�
	this.tMargin=30;//�ֱ�ΪtopoͼƬ���������ҵĿհ׼��
	this.bMargin=10;
	this.lMargin=5;
	this.rMargin=5;
	this.xSpace=10;//Ҷ�ӽڵ�֮�������
	this.ySpace=30;//������
	this.yInterval=this.deviceHeight + this.ySpace;//ÿ���豸ͼƬy����ռ�ĸ߶�
	/**
	ͳ�Ƴ�Ա ����varName��ֵΪvarValue�ĸ���
	**/
	this.countData=function(varName,varValue)
	{
		
		var curLevel=null;
		var i,j;
		var ret=0;
		for(i=0;i<this.maxDepth;i++)
		{
			curLevel=eval(this.prefix+i);
			for(j=0;j<curLevel.length;j++)
			{
			
				if(eval("curLevel[j]."+varName) == varValue)
				{
					ret++;
				}
			}
		}
		return ret;
	}
	/************************
	��ʼ��ÿ���ڵ����(��)��������
	*************************/
	this.initLRChild=function()
	{
		var curLevel=null;
		var parentLevel=null;
		var i,j;
		for(i= (this.maxDepth -1 );i>0;i--)
		{
			curLevel=eval(this.prefix+i);
			parentLevel=eval( this.prefix + (i-1) );
			parentLevel[curLevel[0].parent].lChild=0;
			parentLevel[curLevel[0].parent].rChild=0;
			for(j=1;j<curLevel.length;j++)
			{
				if(curLevel[j].parent != curLevel[j-1].parent)
				{
					parentLevel[curLevel[j-1].parent].rChild=j-1;
					parentLevel[curLevel[j].parent].lChild=j;
					parentLevel[curLevel[j].parent].rChild=j;
				}
			}
			//���һ��ڵ�
			parentLevel[curLevel[j-1].parent].rChild=j-1;
			if(parentLevel[curLevel[j-1].parent].lChild < 0)//���ýڵ�ĸ��ڵ�ֻ�и��ӽڵ�
			{
				parentLevel[curLevel[j-1].parent].lChild=j-1;
			}
		}
	}
	/***********************************
	��ʼ���ڵ�svg����:x�����Լ������豸����
	***********************************/
	this.initOtherNodeSvg=function(level,index,x)
	{
		var left=x , right=0;
		var curLevel=eval(this.prefix + level);
		var childLevel=null;
		var curNode=curLevel[index];
		this.initDeviceIndex(curNode);
		//curNode.y = this.tMargin + this.yInterval * level ;
		if(curNode.lChild < 0)//Ҷ�ӽڵ�
		{
			curNode.x=x;
			return x + this.deviceWidth + this.xSpace;
		}
		else//�к��ӽڵ�
		{
			for(var i=curNode.lChild;i<=curNode.rChild;i++)
			{
				right=this.initOtherNodeSvg(level+1,i,x);
				x=right;
			}
			childLevel=eval(this.prefix + (level +1) );
			left = childLevel[curNode.lChild].x;
			right = childLevel[curNode.rChild].x + this.deviceWidth;
			curNode.x=left + ( right -left - this.deviceWidth  )/2;
			//���ӽڵ�����м�
			return x;
		}
		
	}
	/************************
	��ʼ��topo�ڵ�������豸����
	*************************/
	this.initDeviceIndex=function(curNode)
	{
		if(curNode.isVirtual==1)//���ɹ����豸
		{
			curNode.deviceIndex=2;
		}
		else if(curNode.deviceType==0)//������
		{
			if(curNode.warningLevel>0)
				curNode.deviceIndex=3;
			else
				curNode.deviceIndex=0;
		}
		else if(curNode.deviceType==1)//·����
		{
			curNode.deviceIndex=1;
		}
	}
	/**************************************
	ʹ���е����нڵ�x�������dis
	***************************************/
	this.moveX=function(dis)
	{
		var curLevel;
		for(var i=0;i<this.maxDepth;i++)
		{
			curLevel=eval(this.prefix + i);
			for(var j=0;j<curLevel.length;j++)
			{
				curLevel[j].x += dis;
			}
			
		}
		
	}
	/*******
	��ʼ���ڵ�y����
	********/
	this.initY=function()
	{
		var curLevel=eval(this.prefix + 0);
		var tag=0;
		var curY=this.tMargin;
		curLevel[0].y=curY;
		for(var i=1;i<this.maxDepth;i++)
		{
			tag=0;
			curLevel= eval(this.prefix + i);
			
			for(var j=1;j<curLevel.length;j++)
			{
				if(curLevel[j].parent == curLevel[j-1].parent)
				{	
					tag=1;break;
				}
			}
			
		
			if(tag)
			{
				curY = curY + this.deviceHeight + this.ySpace ;
			}
			else//�ò�ڵ��޸��ڵ���ͬ�Ľڵ�(���ò�ڵ㵽�ϲ�ڵ������ֻ�д�ֱ��)
			{
				curY = curY + this.deviceHeight + this.ySpace *3 / 5;
			}
		
			for(var j=0;j<curLevel.length;j++)
			{
				curLevel[j].y=curY;
			}
		}
	}
	/**************************************
	��ʼ����ں���
	**************************************/
	this.start=function()
	{
		if(this.maxDepth < 1 )
		{
			this.topoHeight=0;
			//this.topoWidth=0;
			return;
		}
		//��ʼ�� �߶�
		if(this.ySpace < 10) this.ySpace=10;
		if(this.yInterval < (this.deviceHeight+this.ySpace)) this.yInterval=this.deviceHeight+this.ySpace;
		var yInterval=(this.topoHeight - this.tMargin - this.bMargin - this.deviceHeight)/(this.maxDepth -1);
		//ƽ��y��
		if(yInterval > this.yInterval) 
		{	
			this.yInterval=yInterval;
			this.ySpace=this.yInterval - this.deviceHeight;
			if(this.ySpace > 80)//y�᷽���ν���
			{
				this.ySpace=80;
				this.yInterval=this.ySpace+this.deviceHeight;
			}
		}
		var minHeight = this.yInterval * (this.maxDepth-1) +this.deviceHeight + this.tMargin +this.bMargin;
		if(this.topoHeight <minHeight)
		{
			this.topoHeight = minHeight;
		}
		else if( (this.topoHeight-minHeight) > 50)//����ͼƬʵ����Ҫ��ʾ����߶ȹ�С
		{
			this.topoHeight= (minHeight + 50);
		}
		if(this.ySpace < 30 ) this.ySpace=30;
		this.initY();
		//��ʼ�����
		
		this.initLRChild();
		var mostRight=this.initOtherNodeSvg(0,0,this.lMargin) - this.xSpace + this.rMargin;//��ʼ���ڵ�svg����
		if(this.topoWidth < mostRight) 
		{
			this.topoWidth=mostRight;
		}
		else if(this.topoWidth > mostRight)
		{
			this.moveX( (this.topoWidth - mostRight)/2 );
			//ʹͼƬ������ʾ
		}
	
	}

}

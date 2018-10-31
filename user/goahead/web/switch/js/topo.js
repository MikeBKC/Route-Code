/********************************
拓扑图节点类
*********************************/
function topoNode(parent,mac,ip,deviceType,isVirtual,warningLevel,parentPort,port,vlanId,alivePorts,dev_model,parentAggrMem,aggrMem)
{
	/******
	*初始化类成员
	*******/
	this.parent=parent;//父节点位置
	this.mac=mac;
	this.ip=ip;
	this.name="";
	this.deviceType=deviceType;//设备类型(交换机或路由器)
	this.version=dev_model;//版本如3124R
	this.isVirtual=isVirtual;//是否虚拟设备
	this.warningLevel=warningLevel;//警告级别
	this.parentPort=parentPort;//父节点连接到此节点的端口号
	this.port=port;//本节连接到父节点的端口号
	this.vlandId=vlanId;//父节点vlanId
	//this.isManage=isManage;//是否可管理
	this.alivePorts=alivePorts;//活动端口数
	this.desc="";//描述信息
	this.aggrMem=aggrMem;//若设置汇聚，此为父节点对应的汇聚端口成员(若无设置汇聚请初始化为'')
	this.parentAggrMem=parentAggrMem;//若设置汇聚，此为本节点对应得汇聚端口成员(若无设置汇聚请初始化为'')
	
	this.x;//x坐标
	this.y;//y坐标
	this.deviceIndex;//对应的设备索引(对应topo初始化类中的devices)
	
	this.lChild=-1;//该节点的最左边的孩子，小于0则不存在
	this.rChild=-1;
	
	
	/*************
	*成员处理
	**************/
	if(this.parentAggrMem && this.aggrMem)
	{/*如果设置了汇聚则 父节点 端口 与 本节点端口无显示的必要*/
		this.parentPort="";
		this.port="";
	}
	else
	{/*如果父节汇聚 或 本节点汇聚中有一个为空 则证明汇聚异常 不需显示*/
		this.parentAggrMem = "";
		this.aggrMem = "";
	}
}
/*******************************
topo图初始化类
********************************/
function topoInit(maxDepth,prefix)
{
	this.maxDepth=maxDepth;//树的最大深度
	this.prefix=prefix;//节点数组的变量前缀
	this.topoWidth=600;//拓扑图总宽度
    this.topoHeight=400;//拓扑图总高度
	this.devices=new Array('../images/switch_normal.png','../images/router_normal.png','../images/device_none.png','../images/switch_warning.png');
	this.deviceWidth=78.6;//设备图片的宽度
	this.deviceHeight=30.6;//设备图片高度
	this.tMargin=30;//分别为topo图片的上下左右的空白间隔
	this.bMargin=10;
	this.lMargin=5;
	this.rMargin=5;
	this.xSpace=10;//叶子节点之间横向间隔
	this.ySpace=30;//纵向间隔
	this.yInterval=this.deviceHeight + this.ySpace;//每个设备图片y向所占的高度
	/**
	统计成员 变量varName的值为varValue的个数
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
	初始化每个节点的左(右)子树连接
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
			//最后一类节点
			parentLevel[curLevel[j-1].parent].rChild=j-1;
			if(parentLevel[curLevel[j-1].parent].lChild < 0)//即该节点的父节点只有该子节点
			{
				parentLevel[curLevel[j-1].parent].lChild=j-1;
			}
		}
	}
	/***********************************
	初始化节点svg属性:x坐标以及所属设备索引
	***********************************/
	this.initOtherNodeSvg=function(level,index,x)
	{
		var left=x , right=0;
		var curLevel=eval(this.prefix + level);
		var childLevel=null;
		var curNode=curLevel[index];
		this.initDeviceIndex(curNode);
		//curNode.y = this.tMargin + this.yInterval * level ;
		if(curNode.lChild < 0)//叶子节点
		{
			curNode.x=x;
			return x + this.deviceWidth + this.xSpace;
		}
		else//有孩子节点
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
			//孩子节点的正中间
			return x;
		}
		
	}
	/************************
	初始化topo节点的所属设备属性
	*************************/
	this.initDeviceIndex=function(curNode)
	{
		if(curNode.isVirtual==1)//不可管理设备
		{
			curNode.deviceIndex=2;
		}
		else if(curNode.deviceType==0)//交换机
		{
			if(curNode.warningLevel>0)
				curNode.deviceIndex=3;
			else
				curNode.deviceIndex=0;
		}
		else if(curNode.deviceType==1)//路由器
		{
			curNode.deviceIndex=1;
		}
	}
	/**************************************
	使树中的所有节点x坐标横移dis
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
	初始化节点y坐标
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
			else//该层节点无父节点相同的节点(即该层节点到上层节点的连线只有垂直线)
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
	初始化入口函数
	**************************************/
	this.start=function()
	{
		if(this.maxDepth < 1 )
		{
			this.topoHeight=0;
			//this.topoWidth=0;
			return;
		}
		//初始化 高度
		if(this.ySpace < 10) this.ySpace=10;
		if(this.yInterval < (this.deviceHeight+this.ySpace)) this.yInterval=this.deviceHeight+this.ySpace;
		var yInterval=(this.topoHeight - this.tMargin - this.bMargin - this.deviceHeight)/(this.maxDepth -1);
		//平分y轴
		if(yInterval > this.yInterval) 
		{	
			this.yInterval=yInterval;
			this.ySpace=this.yInterval - this.deviceHeight;
			if(this.ySpace > 80)//y轴方向层次较少
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
		else if( (this.topoHeight-minHeight) > 50)//拓扑图片实际需要显示区域高度过小
		{
			this.topoHeight= (minHeight + 50);
		}
		if(this.ySpace < 30 ) this.ySpace=30;
		this.initY();
		//初始化宽度
		
		this.initLRChild();
		var mostRight=this.initOtherNodeSvg(0,0,this.lMargin) - this.xSpace + this.rMargin;//初始化节点svg属性
		if(this.topoWidth < mostRight) 
		{
			this.topoWidth=mostRight;
		}
		else if(this.topoWidth > mostRight)
		{
			this.moveX( (this.topoWidth - mostRight)/2 );
			//使图片居中显示
		}
	
	}

}

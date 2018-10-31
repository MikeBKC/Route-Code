// phupdate.h: interface for the CBaseThread class.
//
//////////////////////////////////////////////////////////////////////
/*! \file phupdate.h
*  \author skyvense
*  \date   2009-09-14
*  \brief PHDDNS �ͻ���ʵ��	
*/

#ifndef _CUPDATECORE_H
#define _CUPDATECORE_H

#include "phglobal.h"
#include "uttMachine.h"
//! ������DDNS�ͻ���ʵ�ֻ���
/*!
*/
	

	//! �������ã����úò�������Ҫ��������˺��������������´���Ҫִ�б�������ʱ�䣨������

#if (DDNS_ADVANCE == FYES)
	int phddns_step(PHGlobal *phglobal,char *interface);	
#else
	int phddns_step(PHGlobal *phglobal);
#endif

	//! ֹͣ������DDNS���£��������ò�����ɽ�����һ��
	void phddns_stop(PHGlobal *phglobal);

	//! ��ʼ��socket
	BOOL InitializeSockets(PHGlobal *phglobal);
	//! �ر�����socket
	BOOL DestroySockets(PHGlobal *phglobal);
	//! ��DDNS���������ӵ�TCP������

#if (DDNS_ADVANCE == FYES)
	int ExecuteUpdate(PHGlobal *phglobal,char *interface);
#else
	int ExecuteUpdate(PHGlobal *phglobal);
#endif
	//! ����UDP�����ӡ�

#if (DDNS_ADVANCE == FYES)
	BOOL BeginKeepAlive(PHGlobal *phglobal,char *interface);
#else
	BOOL BeginKeepAlive(PHGlobal *phglobal);
#endif
	//! ����һ��UDP������
	BOOL SendKeepAlive(PHGlobal *phglobal, int opCode);
	//! ��������������
	int RecvKeepaliveResponse(PHGlobal *phglobal);

#endif

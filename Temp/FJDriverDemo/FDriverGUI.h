#ifndef F_DRIVER_GUI_H
#define F_DRIVER_GUI_H

//http://read.pudn.com/downloads49/sourcecode/windows/vxd/167121/2004-08-31_win2kDDK4.pdf

#include <winddi.h>     //Graphics Driver �����ͽṹ
#include <dmemmgr.h>    //DirectDraw �ѹ���������
/******************************************************************************************************************
* TODO��
*   1.�Կ�֧��AlphaBleanding���ٵĻ���OS����ִ������֧�ֵ� DrvAlphaBlend(���ڲ����Ե���Ӳ���� miniport��
*     Ҳ���Ե��� EngAlphaBlend)������ִ��Windows�� EngAlphaBlend
*
* Ӧ�ò�GDI -> ����ģʽGDI(ʹ��DIB��ʽ����λͼ) -> ͼ����������(����ʾ�����ӡ������)
*   ������������� GDI �������е�ͼ�λ���
*
* ͼ����������(GDI) -- ��ΪӦ�ò�GDI �� ���Ĳ�GDI(Ҳ��Ϊͼ������)�� GDI������ǰ׺Ϊ Eng
* �豸��������ӿ�(DDI) -- ����ǰ׺Ϊ Drv
* ��Ƶ΢�˿���������
* ��̬ӳ����������(STI) -- ƽ��ɨ���Ǻ����־�̬ӳ�������Щ��̬ӳ��Ӳ��
*
* ��ͼ����
*   1.�� PDEV(�����豸���߼���ʾ)����һ������, DrvEnablePDEV + DrvEnableSurface
******************************************************************************************************************/

/******************************************************************************************************************
* ��Ƶ��������ִ�еĹ���
*   ���豸�����������Ƶģʽ
*   ��������λ���豸���ߴ��豸�л�ȡ����λ
*   ����DFB��DFBs֮��Ĳ���(�� Blt/StretchBlt ��)
*   ���庯��
*   alpha-blending
*   ���졢����
******************************************************************************************************************/

/******************************************************************************************************************
* 
******************************************************************************************************************/

#endif //#define F_DRIVER_GUI_H
#pragma once
/**************************************************************************************************************
* ������ͣ�
*   �ٶ�: tn=xxxx
*   
**************************************************************************************************************/

/**************************************************************************************************************
* BHO(Browser Helper Object) -- �������������������Ե���������Ա���Ž����ӿڵ�ҵ���׼��
*   ������BHO�����Լ������������ӣ������ᵼ��IE����������������д��DLL 
*     Ҫ��BHO��ʵ��Winsock���ֻ����IE��Ϣ��ʱ��ſ��� ?
* 
* ��дBHO����
*   1.ʵ�� IObjectWithSite �ӿڣ����� COM ����
*   2.�� HKLM\Software\Microsoft\Windows\CurrentVersion\Explorer\Browser Helper Objects\ ��ע��BHO����
*
* ������ٳ� -- ������ҳ,����(���������������ܵ�),Ю����ת�û������URL����ַ�ֶ��滻,�޸ĵ�ǰ��ҳ����,ͳ�ƺ�̨
*   ����SetSite����ǿ��IE��ת������Ա�趨��ҳ��ȥ
*   ���� about:blank �۸���ҳ�ġ�������ٳ֡�����
*
* IURLSearchHook -- �ȴ�IE����IURLSearchHook�������������������ת��һ��δ֪��URLЭ���ַ
*   ���������ͼȥ��һ��δ֪Э���URL��ַʱ����������ȳ��Դ������ַ�õ���ǰ��Э�飬
*   ������ɹ����������Ѱ��ϵͳ������ע��Ϊ��URL Search Hook������Դ�������ӣ�USH���Ķ��󲢰����IE��������ĵ�ַ���͹�ȥ
* 
* Winsock LSP(Windows Socket Layered Service Provider) -- �ֲ�����ṩ��, Winsock2.0���ṩ��
*   ֧��SPI(Service Provider Interface, �����ṩ�̽ӿ�)��������ϵͳ���Ѿ����ڵĻ���Э���ṩ��(��TCP/IPЭ��),
*   ����ЩЭ�����ɷֳ�����Э�鼴Ϊ���ֲ�Э�顱����SSL��ͨ�� LSP�����Ը��򵥵ķ�������������(�� Sniffer )
*   
**************************************************************************************************************/
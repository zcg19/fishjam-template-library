
#ifndef FTL_CRACK_H
#define FTL_CRACK_H

#pragma once

#ifndef FTL_BASE_H
#  error ftlCrack.h requires ftlbase.h to be included first
#endif

//代码页转换: http://bianma.911cha.com/

/*********************************************************************************************************************************
* 权限检查工具：AccessChk -- 可知道指定账号对特定目录的权限和完整性级别等
*   AccessChk -i 目录 -- 查看目录下文件的完整性级别
*   AccessChk -d -q %AppData% 
*********************************************************************************************************************************/

/*********************************************************************************************************************************
* 安全控制
*   Security Principal -- Windows信任的安装安全主体
* 
*   DEP -- 数据执行保护
*   NTLM -- NT LAN Manager(win2000早期版本中的一种验证身份方式), 基于一种"提问-答复"机制来进行客户端验证
*   MIC(Mandatory Integrity Control) -- 强制完整性控制。Vista中的所有安全性对象和进程都有一个完整性级别，
*     完整性级别(Integrity Level -- IL)低的进程不能修改(可以读取?)级别高的文件或注册表表项。
*     注意：Win2K/XP 下，安全子系统只把进程的访问令牌和资源的访问控制列表进行匹配比较，以确认该进程是否具有访问该资源的权限。
*     1.非信任级别 -- 被设置给匿名空连接会话
*     2.Low(低) -- 保护模式下的IE浏览器, 以 Untrusted 的系统权限运行程序，只能存取低存取级别的路径位置，如 "Temporary Internet Files\Low"。
*       注意：未经用户同意就下载的行为运行在低级别，
*             用户手动的下载或故意的执行激活一个内容 运行在中等级别，
*             被一个提升权限的用户所同意的内容(例如安装一个ActiveX控件)则运行在高完整性级别
*     3.Medium(中等) -- 默认情况下，用户级别的代码，如Windows Explorer和任务管理器, 使用"User"权限，能读写用户的文件和注册表项
* 　　4.High(高) -- 真正的 administrator用户 或提升权限后的程序
* 　　5.System(系统) -- 内核级别的Windows文件
*     6.保护进程级别 -- 级别最高，只有在系统需要的时候才会被使用
*     
*   应用文件和注册虚拟化(虚拟重定向) -- 对指定位置进行读写的时候会被重定向到每个用户的虚拟化的区域，
*     在以下位置建立了一个和"用户配置文件夹"完全一致的且完整性级别为"Low"的目录层次:
*     %UserProfile%\AppData\Local\Microsoft\Windows\Temporary Internet Files\Virtualized\XXXX -- XXX部分即是应该的 C:\ProgramData\MyProgramData 等
*
*     以下注册表进行控制？：HKLM\SYSTEM\CurrentControlSet\services\luafv
*   SFP -- 系统文件保护, Win2K 以前的文件保护机制
*   UAC(User Account Control) -- 用户帐户控制， 让管理员帐户自动获得一个标准用户的访问令牌，以减少Windows Vista系统的受攻击面
*     UAC Elevation -- 实际上一个软件在用不同的模块运行，因Virtualization映射到其他位置的Data文件，所以在操作磁盘文件以及注册表的时候，实际上是一个软件在用不同的模块运行。
*   UIPI(User Interface Privilege Isolation) -- 用户界面特权隔离，完整性级别低(lower integrity)的进程，不能向完整性级别高的进程发送Window消息。
*     默认情况下，所有在 WM_USER 以上的消息都会被屏蔽，可以通过 ChangeWindowMessageFilter(WM_xxx, MSGFLT_ADD) 允许从低完整性进程处接受消息
*   WFP -- Windows文件保护, Win2K时引入，只保护文件
*   WRP -- Windows资源保护，对关键文件、文件夹和注册表值进行保护
*
*
*********************************************************************************************************************************/

/*********************************************************************************************************************************
* 二进制转换十进制：将二进制数的每一位数乘以它的权，然后相加。
*   (100110.101)2 = 1*2^5 + 1*2^2 + 1*2^1 + 1*2^(-1) + 1*2^(-3) = 32+4+2+0.5+0.125 = 38.625
* 十进制转换二进制：整数部分(除2取余)和小数部分(乘2取整)分别转换，然后再合并。
*   或 写成按二进制数权的大小展开的多项式，按权值从高到低依次取各项的系数。
*   (175.71875)10 = 128+32+8+4+2+1+0.5+0.125+0.0625+0.3125 
*                 = 2^7+2^5+2^3+2^2+2^1+2^0+2^(-1)+2^(-3)+2^(-4)+2^(-5)
*                 = (10101111.10111)2
*   
* 原码：最高位是符号位(0表示正号，1表示负号)，其余的n-1位表示数值的绝对值。
*   0的原码表示有两种：[0]原 = 0 000 0000, [-0]原 = 1 000 0000
* 反码：最高位是符号位(0表示正号，1表示负号)，正数的反码和原码相同，负数的反码则是其绝对值按位求反。
*   0的反码表示有两种：[0]反 = 0 000 0000, [-0]反 = 1 111 1111
* 补码：最高位是符号位(0表示正号，1表示负号)，正数的补码和原码相同，负数的补码则等于其反码+1。
*   0有唯一的补码 [+0]补=[-0]补=0 000 0000。利用补码，可以将减法运算转换为加法运算。
* 移码：在数X上增加一个偏移量，常用于表示浮点数中的阶码。如果机器字长为n，在偏移2^(n-1)的情况下，
*   将补码的符号位取反就可获得相应的移码表示（也可以是原码+127,1023或16383）
*   [-45]原=1 010 1101， [-45]反=1 101 0010， [-45]补=1 101 0011，[-45]补=0 101 0011
* 
* 模2运算加减法：按位运算，不发生借位和进位。
* 
* 定点数：小数点的位置固定不变的数。通常有两种：定点整数 和 定点小数，能表示的数值范围较小。
* 浮点数：小数点的位置不固定的数，能表示更大范围，使用阶码和尾数来表示。二进制数N = 2^E * F (其中E称为 阶码， F称为 尾数)
*   表示格式为：阶符|阶码|数符|尾数，其表示不唯一。浮点数能表示的数值范围由阶码决定，所表示数值的精度由尾数决定。
*   为了充分利用尾数来表示更多的有效数字，通常采用规格化浮点数(将尾数的绝对值限定在[0.5,1]的区间内)。
*   通常阶码用移码表示，尾数用原码表示。IEEE 754 标准中对各种类型小数的 指数长度规定分别是：float(8),double(11),扩充精度(15)
*   按IEEE 754标准将 176.0625 表示为单精度浮点数：(176.0625)10 = (1011 0000.0001)2 = 1.011 0000 0001 * 2^7 
*     =>(将小数部分扩展为单精度浮点数所规定的23位尾数，双精度为52,扩充精度为64) 011 0000 0001 0000 0000 0000
*     =>(计算阶码，前面指数为7，加上单精度偏移 127，双精度为1023，扩充精度为16383)E=7+127=134,则指数的移码表示为 1000 0110
*     最后可得：(176.0625)10 的单精度浮点数表示形式为 0 10000110 011 0000 0001 0000 0000 0000(0x43301000)
*     float f = 176.0625;
*     DWORD d = *((DWORD*)(&f));
*     CPPUNIT_ASSERT(d == 0x43301000);
* 
* 汉字编码：
*   西文基本符号较少，编码容易(ASCII)，在计算机系统中，输入、内部处理、存储和输出都是用同一代码（输出应该也有字形码？）；
*   汉字种类繁多，输入、内部处理、存储和输出对汉字代码的要求不尽相同，采用的编码也不同。
*     输入码：是用西文标准键盘输入汉字时的编码，常用的有三类：数字编码(如国标区位码--94x94)、拼音码(以汉语读音为基础)、字形码(以汉字的形状确定，如五笔)
*     内部码：汉字在设备或信息处理系统内部的表达形式，是存储、处理、传输用的代码。如GB2312的汉字国标码，将GB2312两个字节的高位置1，
*       就得到了机内码(GBK内码或ASCII码)，如“大”的GB2312为3473H，机内码为B4F3H
*       UCS(Universal Character Set,也叫ISO 10646) -- 为了统一地表示世界各国的文字，为包括中日韩等国文字在内的
*         各种正在使用的文字规定了统一的编码方案。用4个字节指定 组、平面、行和字位( 2^7 x 2^8 x 2^8 x 2^8)。
*         Unicode 标准对应于 ISO 10646 实现级别 3(即支持所有的 UCS 组合字符)；
*         UTF-8 即 UCS 变形格式 8，是一种兼容于 ASCII 编码和所有 POSIX 文件系统的可变长编码
*     字形码：用于汉字输出，表示汉字字形的字模数据，通常用点阵、矢量函数等方式表示。当显示或打印输出时，检索字库，输出字模点阵得到字形。
*       根据输出的要求不同，点阵的多少也不同。如：简易型的 16x16点阵，高精度型的 24x24点阵、32x32点阵、48x48点阵等。
*       矢量表示法是将汉字看做由笔画组成的图形，提取每个笔画的坐标值，将每一个汉字的所有坐标值组合起来就是该汉字字形的矢量信息。
*
* 校验码 -- 把数据可能出现的编码分为两类：合法编码和错误编码。合法编码用于传送数据，错误编码是不允许在数据中出现的编码。
*   码距 -- 一个编码系统中任意两个合法编码之间至少有多少个二进制位不同,码距为1时无差错功能（如8421码）
*   常用的校验码：
*   1.奇偶校验码(parity code)：在编码中增加一位校验位来使编码中1的个数为奇数(奇校验)或偶数(偶校验)，码距为2，能发现错误，但不能校正错误。
*   2.汉明码(hamming code)：也是利用奇偶性来检错和纠错。在数据位之间插入k个校验位，通过扩大码距来实现检错和纠错。如 8位数据 中插入 4位校验。
*   3.CRC：广泛应用于数据通信领域和存储系统中。利用生成多项式为k个数据位产生r个校验位来进行编码，其编码长度为k+r。编码后共n个数据，其中有
*      k个信息码，则校验码占 n - k 个，称为 (n ,k)码。校验码由信息码产生，其位数越长，校验能力就越强。在求CRC编码时，采用的是模2运算。
*
* 逻辑运算
*   与(逻辑乘) -- AND
*   或(逻辑加) -- OR
*   非(逻辑求反) -- NOT
*   异或(半加) -- XOR
*   

**********************************************************************************************************************************/
/**********************************************************************************************************************************
* 公钥/私钥对 -- 使用单向函数的数学原理，即公钥加密后只能用私钥解密；私钥加密后只能用公钥解密
* 
* 对称密钥：私钥加密合解密，速度快，但密钥管理困难
* 非对称密钥：公钥加密，私钥解密，机制灵活，但速度慢。
*   加密发送：发送者使用接受者的公钥加密并发送，一旦加密，只能由接受方的私钥解密
*   数字签名：用户用自己的私钥加密，可通过其公钥解密进行验证
*   
* 数字证书：有公钥信息，从而确认了拥有密钥对的用户的身份
* 公钥位于 ~/.ssh/ 目录中，该目录有以下限制：除本账户的其他所有账户(包括root)都只读，否则无法登录。特殊需求时可在 sshd_config 配置
*   文件中用 StrictMode no 来取消该限制(man sshd_config)
*
* AES(Advanced Encryption Standard) -- 高级加密标准，即Rigndael加密法，用于替代原先的DES
**********************************************************************************************************************************/
namespace FTL
{
    class CFCrackUtility
    {
    public:
        enum CrackStringType
        {
            //csLowerCase,
            //csUpperCase,
            //csNumber,
            csKiloString,
            //csPlaceString,
        };
        FTLINLINE CFCrackUtility();
        FTLINLINE virtual ~CFCrackUtility();

        FTLINLINE LPCTSTR GetCrackString(CrackStringType csType,DWORD dwPlaceStart = 0);
    private:
        LPTSTR  m_pszCrackString;
    private:
        static LPCTSTR s_csKiloString;
        static LPCTSTR s_csPlaceString;
    };
}

#endif //FTL_CRACK_H



#ifndef USE_EXPORT
#  include "ftlCrack.hpp"
#endif

//http://www.gbtags.com/gb/share/2661.htm -- 45个超实用的JavaScript技巧及最佳实践
//JavaScript 从入门到精通 -- P266(桌面附件)
//精通JavaScript xxxx .pdf -- P33,第三章,创建可重用代码。(但最好在看完入门到精通后，从头再看一遍)
//  本地存储 -- localStorage ?
//JavaScript 框架(如： jQuery, MooTools, Prototype, Dojo 或 YUI 3)，

//开发出专业JavaScript代码的根本方法之一是：快速、静态地提供与其他代码的接口，同时保证自身的可理解性
//开发时需要考虑到客户禁用或不支持JavaScript(用户代理)的情况，而且当前的搜索引擎爬虫基本都不支持JavaScript(SEO优化考虑)，
//  TODO: 为了防止程序不认识JS脚本，需要将脚本代码放入  //<![CDATA[  xxxxxx    //]]> 之中 ?

//<a href="javascript:self.close()"> 关闭 </a>

//??TODO: 分离式JavaScript 似乎很重要，可以使代码在不同的浏览器间使用时可以升级或降级运行(能适应不同的浏览器)
//DOM -- Document Object Model, domObj.onmouseout = function() { xxxx; };  //匿名函数
//DHTML -- 在DOM和事件交互的基础上产生了DHTML，其实质是JavaScript和DOM元素上的CSS属性之间的交互

//匿名函数、匿名对象 -- 似乎在任何地方都可以使用 ?
//定义:
//  function sendMessage(msg, obj) { obj.handleMsg(msg) }
//调用
//  sendMessage("how are you?", handleMsg : function( msg ) { alert("show Msg:" + msg); }); 


/**********************************************************************************************************************************
* 最佳实践
*   1.总是使用关键字var声明变量 -- 如果函数内的变量不用var声明且名字正好和某一全局变量相同，会无意中更改到全局变量
*   2.属性检测胜过浏览器检测 -- 基于是否有指定的属性编程，而非针对浏览器类型编程
*   3.用点符号访问标准对象属性, 使用方括号访问那些定义在页面中的对象属性 -- TODO: 使用点符号时能很容易的发现拼写错误.
*     如：document.forms["myformname"].elements["myinput"].value
*   4.避免使用 eval -- eval可以在运行时期运行任意代码，有很大的安全隐患
*   5.避免使用with语句 -- 可通过创建一个引用指向指定对象，并使用
*   6.表单中text的值如果是数字时，可以通过一元加号操作符告诉JS这个是数字类型，如 var total=(+$("#amount").val()) + xxxx; 
*   7.不要在javascript代码块中使用HTML注解 -- 现在所有的浏览器都已支持脚本
*   8.避免杂乱的全局命名空间, 可能会导致命名冲突 -- 使用闭包(closure)
*   9.避免使用同步ajax调用 -- 
**********************************************************************************************************************************/

/***********************************************************************************************
* JavaScript
*   优点：使得网页和用户之间实现了一种实时、动态和交互的关系
*   缺点：复杂的文档对象模型(DOM)；不一致的浏览器实现；缺乏便捷的开发、调试工具
* 
* 常见的JavaScript库( 通常提供强大的功能，并且兼容各大浏览器 )
*   Prototype(www.prototypejs.org) -- 对内置对象做了大量的扩展，可认为是功能函数库
*     https://ajax.googleapis.com/ajax/libs/prototype/1.7.1.0/prototype.js
*   Dojo(dojotoolkit.org) -- 提供了很多其他库没有提供的功能，如 离线存储API、生成图标，得到多家企业支持
*   YUI(Yahoo User Interface) -- 
*   jQuery(jquery.com) --★Web开发首选★拥有强大的选择器，出色的DOM操作，可靠的事件处理、完善的兼容性和链式操作等功能
*   ExtJS(www.extjs.com) -- 对YUI的扩展，主要用于创建前段用户界面，可利用包括jQuery在内的多种JavaScript框架作为基础库。但本身比较臃肿，商业使用时需要付费获得授权许可
* 
* Json(JavaScript Object Notation) -- 一种比XML更轻量级的数据交换格式，易于阅读和编写的同时，也易于程序解析和生成，表现为键值对集合的文本描述信息 
*
* JavaScript常见的封装类型
*   DOM的选择和操作：经典的例子如jQuery的链式API。
*   DOM事件处理：一个重点是简化事件的delegation，即利用事件冒泡机制在父元素上用一个侦听函数侦听触发在多个子元素上的事件。
*   Ajax：简化烦琐的XMLHttpRequest API，并且加强其语义性。
*   语言增强：主要提供一些对数组和对象进行操作的便利函数。jQuery包含一些，但更典型的有Underscore和Lodash。
*
* 帮助：MSDN -> Development Tools and Languages -> Visual Studio -> JScript
*
* 语法：
*   0.JavaScript中所有的对象变量都是引用(reference)，引用指向的只能是具体的对象，而不是另一个引用 -- 不允许多层引用
*   1.变量区分大小写，使用前需要通过 var <变量名> [ = <初始值> ] 的方式声明，默认赋值为"null"。JavaScript 对数据类型的要求不严格。
*     null -- 一个特殊的空值，当变量未定义，或者定义之后没有对其进行任何赋值操作时的值；
*     Number.NaN(Not a Number) -- 当运算无法返回正确的数值时，返回该值(如 parseInt("字母"))，任何数(包括NaN)跟它都不相等
*     Infinity -- 无穷大，对应的有 Number.NEGATIVE_INFINITY 和 Number.POSITIVE_INFINITY
*   2.数组(Array) -- 对象的集合，里边的对象可以是不同类型的，通过 [下标] 访问, var myArray = new Array(可选的初始列表);
*       JS只支持一维数组。如想模拟多维，需要类似 new Array(new Array(), xxx) 的方式，访问时通过 myArray[2][3] = xxx;
*       join(分隔符) -- 使用分隔符把数组中的各个元素串起来并返回相应的字符串，通常可用于调试输出?
*       sort([排序函数]) -- 按指定的排序函数进行排序，如按升序排列数字的排序函数： function myGreater(a, b){return a-b;}
*   3.Date -- 可存储 0001到9999的任意一个日期，精确到毫秒，内部实现是一个整数(1970-01-01 00:00:00:000 开始的毫秒数)，
*     如不指定时区，默认采用"UTC"。有 get/set[UTC]XXXX 的方法，来获取或设置对应的时间值。
*     getTimezoneOffset() -- 返回日期对象采用的时区与格林威治时间所差的分钟数，中国(GMT+0800)返回 -480。
*   4.异常处理：
*     try { xxxx } catch(e) { alert('some error'); }
*   5.一些常用的特殊函数
*     eval() -- 把括号内的字符串当作标准语句或表达式来运行
*     escape() -- 返回括号中的字符串经过编码后的新字符串，该编码应用于URL，也就是把空格写成"%20"这种格式
*     parseInt("str" [,radix])/parseFloat("str" [,radix]) -- 返回把括号内的内容转换成整数/浮点数之后的值，参数中最好指定 radix 以防止出现特殊字符串(如0xxx);
*   6.JavaScript的作用域(scope)是由函数划分的，而不是由块(block)划分的 -- 所有全局作用域的变量都是window对象的属性
*     闭包(closure) -- 内层的函数可以引用存在于包围它的函数内的变量，即使外层函数的执行已经终止,该特性非常强大和复杂，
*       通常可以使得代码更加清晰(如: 自执行的匿名函数隐藏全局变量; )
*       注意：闭包函数使用父函数中的变量是其范围内的最终值，可能造成错误(如for循环的计数器)
*       http://jibbering.com/faq/notes/closures/
，  7.上下文(context)对象 -- 即this(代码处在该对象中)，全局对象的this是window?
*      函数.call(someObj, 其他参数) -- 在函数执行体中将上下文对象(this) 设置为 someObj，然后调用函数体;
*      函数.apply(somObj, 其他参数) -- ???
* JavaScript 是 HTML5 以及所有现代浏览器中的默认脚本语言,因此在引用时可以不再在 <script> 标签中使用 type="text/javascript"
*
* 链式作用域(chain scope) -- 子对象会一级一级地向上寻找所有父对象的变量,并拥有访问的权限
***********************************************************************************************/

/***********************************************************************************************
* 对象化编程
*   调用时：className.funName1();
*   var className = {
*     funName1 : function(param1, param2) {
*       this.initialized = true;
*       return [返回值];
*     },
*     funName2 : function(param..) {
*     }
*   };
*
*   属性
*   方法
*     任何对象都有 toString() 方法，返回可读的字符串形式
*   事件--响应发生在对象上的事情
*
*   
* 文档接收更多事件 <== document.captureEvent()
***********************************************************************************************/



//Jscript提供SendKeys方法，可用于将一个或多个键击发送到活动窗口。

var CFUtil = {
    //打开一个新的窗口，或是使得已经创建的窗口获得焦点
    openWindow: function(windowName, url, flags, params) {
        var windowsMediator = Components.classes["@mozilla.org/appshell/window-mediator;1"].getService(Components.interfaces.nsIWindowMediator);
        var aWindow = windowsMediator.getMostRecentWindow(windowName);
        if (aWindow) {
            aWindow.focus();
        }
        else {
            aWindow = window.openDialog(url, windowName, flags, params);
        }
        return aWindow;
    },
    
    
   openProgram : function () {
   	    var WshShell = WScript.CreateObject("Wscript.Shell");	//创建WshShell对象
	    WshShell.Run("notepad",3); //利用WshShell对象来启动记事本,并把它最大化
	    WshShell.AppActivate("记事本"); //启动完应用程序后最好能先调用一下AppActivate方法，确保应用程序的窗口被激活
	    WScript.Sleep(1000);//有些GUI的界面响应速度会比较慢，因此在各操作之间最好能插入时间缓冲

	    WshShell.SendKeys("ABCD...");	//发送按键给窗口来操作应用程序的功能
	    WScript.Sleep(1000);

	    WshShell.SendKeys("%OF");   //发送组合功能键(ALT+O+F键调出记事本的字体设置窗口)
   }
};

//CFUtil.openProgram();

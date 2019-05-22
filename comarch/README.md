```flow
st=>start: 开始 
en=>end: 结束 
op1=>operation: 静态获取接口GetInterface(指定模块guid和接口guid)
op2=>operation: 动态获取接口WSCreateInstance
op3=>operation: 解析配置文件加载插件库信息
op4=>operation: 插件库信息根据guid找到库HMODULE
op5=>operation: 获得DllGetClassObject
op6=>operation: 静态函数CFactory::GetClassObject(clsid, iid, ppv);
op7=>operation: CFactoryTemplate来初始化CFactory并返回工厂到ppv;
op8=>operation: CFactory的CreateInstance来创建接口实例;
op9=>operation: CFactoryTemplate模板中CreateInstance;

st->op1()->op2->op3->op4->op5->op6->op7->op8->op9->en 

```
库抽象成插件
* STDAPI DllCanUnloadNow()
* STDAPI DllGetClassObject(const CLSID& clsid,const IID& iid,void\*\* ppv)
* STDAPI DllRegisterServer()
* STDAPI DllUnregisterServer()
CFactory来具体实现

IClassFactory
virtual HRESULT \_\_stdcall CreateInstance(IUnknown* pUnknownOuter,
	                                         const IID& iid,
	                                         void** ppv) ;
普通库         GetInterface 
  动态加载插件支持库句柄   
  获取WSCreateInstance接口
插件支持库     WSCreateInstance  
  第一次加载所有插件  
  然后根据模块guid得到模块句柄HMODULE （这一步guid找库获取库的控制权，模块guid和库是多对一的关系，）   
插件库  
 根据句柄获取插件库通用接口DllGetClassObject  
 直接交给CFactory::GetClassObject(clsid, iid, ppv) 
 根据模块guid从工厂模板列表中找到工厂模板
 用工厂模板初始化CFactory并获取工厂IUnknown接口  
 从CFactory的IUnknown查询获得IClassFactory  
 IClassFactory的CreateInstance接口去调用工厂模板入口，得到com接口
 通过com接口来查询iid接口得到最终接口
 拿到最终接口调用方法

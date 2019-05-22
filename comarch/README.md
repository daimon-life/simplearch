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

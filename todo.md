
## 功能需求
### HJObject
- 将当前的类序列化为JSON
- 通过序列信息，一次性更新Object里边的所有属性的值
- 根据序列化信息构造一个类

### todo
- 对象访问和修改值不使用offset,而是直接通过变量名访问和修改
- [BUG] 给HJOBJECT使用setValue设置HJPROPERTY值时,如果该值的HJVariant内在类型不是严格和该HJPROPERTY对应,则会强制赋值而没有类型转换
- [Feature] HJVariant setValue(HJVariant) with type conversion
- HJVariant的setValue全部没有类型转换setValue应该有类型转换(bool返回有没有设置成功),不需要类型转换直接设置值可以用convertTo(T value)
- HJVariant的 operator = 应该也是有类型转换的


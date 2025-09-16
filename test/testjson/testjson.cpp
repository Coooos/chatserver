#include "json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <vector>
#include <map>
using namespace std;

// json序列化实例1
void func1()
{
    json js;
    // 添加数组
    js["id"] = {1, 2, 3, 4, 5};
    // 添加key-value
    js["name"] = "zhang san";
    // 添加对象
    js["msg"]["zhang san"] = "hello world";
    js["msg"]["liu shuo"] = "hello china";
    // 上面等同于下面这句一次性添加数组对象
    js["msg"] = {{"zhang san", "hello world"}, {"liu shuo", "hello china"}};
    cout<<js<<endl;
}

// json 实例代码 序列化容器
void func2()
{
    json js;
    // 序列化vector 容器
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(5);

    js["list"] = vec;
    //
    map<int, string> m;
    m.insert(1, "黄山");
    m.insert(2, "华山");
    m.insert(3, "泰山");

    js["path"] = m;

    string sendBuf = js.dump(); // json数据结构 -》序列化json字符串
    // 可以通过网络发送；
    cout << sendBuf << endl;
    cout << js << endl;
}

int main()
{
    //func1();
    func2();
    // 接收
    // string recvBuf = func1();
    // 数据反序列化 json的字符串 反序列化 数据对象（看作容器，方便访问）
    // json jsBuf = json::parse
    // cout<<jsBuf["msg_type"]<<endl;

    return 0;
}


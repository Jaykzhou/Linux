// 简易版网络计算器服务端程序
#include "tcp_cala.hpp"
#include <stack>
#include <thread>

// 栈
stack<int> _stack;

// 运算符优先级检测
int Judge(const char e)
{
  switch(e)
  {
    case '+':
    case '-':
      return 1;

    case '*':
    case '/':
      return 2;

    case '(':
      return 3;

    case ')':
      return -1;

    // 当为数字时，返回 0
    default:
      return 0;
  }
}

// 字符串解析 + 计算函数
int string_fun(string& numstr)
{
  // 将中缀表达式转化为后缀表达式
  //    1. 遇见数字直接进行储存/输出
  //    2. 遇见运算符时: 
  //         1. 当栈为空时，直接压栈
  //         2. 当前运算符大于栈定运算符时，直接压栈
  //         3. 当前运算符小于/等于栈定运算符，先出栈，直到大于栈定元素，再入栈
  //         4. 当遇见左括号时，将左括号压栈，将左括号认为栈底(空栈)，继续上述过程，当遇见右括号时，将其中的运算符全部出栈
  char back_numstr[1024] = {0};
  int index= 0;
  for(auto& e : numstr)
  {
    if(e != '\n')
    {
      // 数字直接输出
      if(Judge(e) == 0)
      {
        back_numstr[index++] = e;
      }
      // 运算符处理( +-*/(  )
      else if(Judge(e) > 0) 
      {
        // 栈为空
        if(_stack.empty())
        {
          _stack.push(e);
        }
        // 栈不为空
        else  
        {
          //      栈不能为空          当前优先级 <= 栈顶优先级
          while(!_stack.empty() && Judge(e) <= Judge(_stack.top()) && _stack.top() != '(')
          {
            back_numstr[index++] = _stack.top();
            _stack.pop();
          }

          _stack.push(e);
        }
      }
      // 运算符处理(  ) )
      else 
      {
        while(!_stack.empty() && _stack.top() != '(')
        {
          back_numstr[index++] = _stack.top();
          _stack.pop();
        }

        // 丢掉左括号
        _stack.pop();
      }
    }
  }

  // 走到结尾
  while(!_stack.empty())
  {
    back_numstr[index++] = _stack.top();
    _stack.pop();
  }

  string back_str;
  back_str.assign(back_numstr, index);
  

  // 从后缀表达式中得出运算结果
  //    1. 遇见数字时压栈
  //    2. 遇见运算符时，先将栈定元素返回给右操作数，再将下一个元素返回给左操作数。
  //    3. 遍历全部的后缀表达式。
  for(auto& e : back_str)
  {
    int num1 = 0;
    int num2 = 0;
    if(Judge(e) == 0)
    {
      _stack.push(e - '0');
    }
    else 
    {
      if(!_stack.empty())
      {
        num2 = _stack.top();
        _stack.pop();
        num1 = _stack.top();
        _stack.pop();

        if(e == '+')
        {
          _stack.push(num1 + num2);
        }
        if(e == '-')
        {
          _stack.push(num1 - num2);
        }
        if(e == '*')
        {
          _stack.push(num1 * num2);
        }
        if(e == '/')
        {
          _stack.push(num1 / num2);
        }
      }
    }
  }

  int result = _stack.top();
  _stack.pop();
  
  return result;
}

void th_start(Tcp_socket* sock) 
{
  string numstr;
  while(1)
  {
    // 接受数据
    sock->Recv(numstr);
    int result = string_fun(numstr);
    sock->Send(result);
  }
}


int main(int argc, char* argv[])
{
  if(argc != 3)
  {
    cout << "./a.out 192.168.248.129 9000" << endl;
    return -1;
  }

  string ip = argv[1]; 
  uint16_t port = atoi(argv[2]);

  Tcp_socket lissock;
  lissock.Socket();
  lissock.Bind(ip, port);
  lissock.Listen();

  // 创建线程处理任务
  while(1)
  {
    Tcp_socket* handlesock = new Tcp_socket;
    lissock.Accept(*handlesock);

    thread t1(th_start, handlesock);
    t1.detach();
  }

  return 0;
}

#include "mainwindow.h"

#include <QQuickWindow>
#include <QDebug>

const QString imagesDir = ":/images";
const QString fontFilePath = ":/font/msyhl.ttc";
const QString configFilePath = ":/config/config.txt";
const QString mathjaxScriptPath = "mathjax/es5/tex-mml-chtml.js";
const QString chatRecordsDir = "../../chatrecords/";
QString codeThemeFilePath = ":/config/dark_theme.xml";
const QString webEngineCacheDir = "../../webengine_cache";
QWebEngineProfile *sharedProfile = nullptr;
const int windowFontPointSize = 10;
int windowFontPixelSize = 20;
const int titleFontPointSize = 14;
int titleFontPixelSize = 28;
const int buttonFontPointSize = 9;
const QColor textEditFullBGColor = QColor(224, 224, 224);
const QColor textEditFullBGTColor = QColor(224, 224, 224, 0);
const QColor textEditFullBTColor = QColor(100, 100, 100);
const QColor textEditFullBColor = QColor(100, 100, 100, 0);
const QString initBaseUrl = "http://127.0.0.1:11434/v1";
const QString initApiKey = "EMPTY";
const QString initModel = "deepseek-r1:1.5b";
const int maxTokensMinimum = 0;
const int maxTokensMaximum = 32768;
const int initMaxTokensCurrentVal = 5000;
const int topPMinimum = 0;
const int topPMaximum = 1;
const double initTopPCurrentVal = 0.8;
const double topPSingleStep = 0.01;
const double temperatureMinimum = 0.01;
const int temperatureMaximum = 1;
const double initTemperatureCurrentVal = 0.8;
const double temperatureSingleStep = 0.01;
const QString testText1 = R"(<think>
让我想一下两种方法。第一种方法更直观，适合新手理解。第二种方法效率更高，特别是当n很大的时候。那么对于这个问题来说，两种方式都行。我应该两种方法都写吗？可能问题只需要一种实现，但为了全面，我可以两种情况都考虑一下。

那我先写出循环的方法。代码结构大概是：

```lua
-- Two dashes start a one-line comment.
--[[
   Adding two ['s and ]'s makes it a
   multi-line comment.
http://learnxinyminutes.com/
Have fun with Lua!
--]]
```

这两种方法都可行，我觉得第二种更好，因为效率更高而且代码更简洁。

现在整理一下：

C++：
同样，两种方法都可以。

Python：
可以用循环、公式，或者生成一个range然后求和。

那现在我来写出这三种语言的代码：

C++：
```cpp
#include <iostream>

using namespace std;

int main() {
    #path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
    int n = 100;
    int sum = n * (n + 1) / 2;
    cout << sum << endl;
    return 0;
}
```

或者循环：

```python
sum = 0
for i in range(1, 101):
    sum += i
```

或者使用range和sum函数：

print(sum(range(1, 101)))

嗯，我觉得这三种方法都是可行的。现在，我需要把它们分别用C++和Python写出来。

总结一下，每个语言可以有两种实现方式，但问题可能只需要一种，所以我可以选择其中一种，比如公式的方法，这样更简洁高效。
</think>

这些代码都使用了高斯公式来计算从 1 到 100 的和，结果都是 5050。

### Python
```python
n = 100
total = n * (n + 1) // 2
#path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
print(total)
```
qweertyuiopasdfghj
这些代码都使用了高斯公式来计算从 1 到 100 的和，结果都是 5050。

### Python
```python
n = 100
total = n * (n + 1) // 2
#path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
print(total)
```

这些代码都使用了高斯公式来计算从 1 到 100 的和，结果都是 5050。
让我想一下两种方法。第一种方法更直观，适合新手理解。第二种方法效率更高，特别是当n很大的时候。那么对于这个问题来说，两种方式都行。我应该两种方法都写吗？可能问题只需要一种实现，但为了全面，我可以两种情况都考虑一下。
那我先写出循环的方法。代码结构大概是：
### Python
```python
n = 100
total = n * (n + 1) // 2
#path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
print(total)
```

以下是使用 C++ 和 Python 分别计算整数 1 到 100 的和的代码示例：

### C++
```cpp
#include <iostream>

using namespace std;

int main() {
    #path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
    int n = 100;
    int sum = n * (n + 1) / 2;
    cout << sum << endl;
    return 0;
}
```

### Python
```python
n = 100
total = n * (n + 1) // 2
#path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
print(total)
```

这些代码都使用了高斯公式来计算从 1 到 100 的和，结果都是 5050。
让我想一下两种方法。第一种方法更直观，适合新手理解。第二种方法效率更高，特别是当n很大的时候。那么对于这个问题来说，两种方式都行。我应该两种方法都写吗？可能问题只需要一种实现，但为了全面，我可以两种情况都考虑一下。
那我先写出循环的方法。代码结构大概是：
### Python
```python
n = 100
tota = n * (n + 1) // 2
#path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
print(total)
```

这些代码都使用了高斯公式来计算从 1 到 100 的和，结果都是 5050。
让我想一下两种方法。第一种方法更直观，适合新手理解。第二种方法效率更高，特别是当n很大的时候。那么对于这个问题来说，两种方式都行。我应该两种方法都写吗？可能问题只需要一种实现，但为了全面，我可以两种情况都考虑一下。
那我先写出循环的方法。代码结构大概是：
### Python
```python
n = 100
total = n * (n + 1) // 2
#path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
print(total)
```

以下是使用 C++ 和 Python 分别计算整数 1 到 100 的和的代码示例：
让我想一下两种方法。第一种方法更直观，适合新手理解。第二种方法效率更高，特别是当n很大的时候。那么对于这个问题来说，两种方式都行。我应该两种方法都写吗？可能问题只需要一种实现，但为了全面，我可以两种情况都考虑一下。
那我先写出循环的方法。代码结构大概是：
### C++
)";

const QString testText_simple = R"(<think>
那我先写出循环的方法。代码结构大概是：

```cpp
int sum = 0;
for (int i = 1; i <= 100; ++i) {
    #path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
    sum += i;
}

int main()
{
    int n, sum = 0;

    for (int i = 1; i <= n; ++i)
    {
        sum += i;
    }

    std::cout << "Sum = " << sum;
    return 0;
}
```

或者用公式：

所以我可以选择其中一种，比如公式的方法，这样更简洁高效。
</think>

这些代码都使用了高斯公式来计算从 1 到 100 的和，结果都是 5050。

### Python
```python
n = 100
total = n * (n + 1) // 2
#path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
print(total)
```

这些代码都使用了高斯公式来计算从 1 到 100 的和，结果都是 5050。

以下是使用 C++ 和 Python 分别计算整数 1 到 100 的和的代码示例：

### C++
)";

const QString testText = R"(<think>
让我想一下两种方法。第一种方法更直观，适合新手理解。第二种方法效率更高，特别是当n很大的时候。那么对于这个问题来说，两种方式都行。我应该两种方法都写吗？可能问题只需要一种实现，但为了全面，我可以两种情况都考虑一下。

那我先写出循环的方法。代码结构大概是：

```cpp
int sum = 0;
for (int i = 1; i <= 100; ++i) {
    #path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
    sum += i;
}
std::cout << sum << std::endl;

#include <iostream>
#include <QDebug>

#sdfgs	"fdgdf"
#cfdsf	546

int main()
{
    int n, sum = 0;

    std::cout << "Enter a positive integer: ";
    std::cin >> n;

    for (int i = 1; i <= n; ++i)
    {
        sum += i;
    }

    std::cout << "Sum = " << sum;
    return 0;
}
```

或者用公式：

```cpp
int n = 100;
int sum = n * (n + 1) / 2;
#path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
std::cout << sum << std::endl;
```

同样，两种方法都适用。可能用公式更好。

然后是Python的部分。Python的语法更简单，循环的话：

```python
sum = 0
for i in range(1, 101):
    sum += i
#path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
print(sum)

import sys
from PyQt5.QtWidgets import QTextEdit, QApplication, QWidget
from PyQt5.QtGui import QTextCharFormat, QColor, QPainter, QFont
from PyQt5.QtCore import Qt, QRect

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = CodeEditor()
    window.show()
    sys.exit(app.exec_())
```

或者，直接用公式：

```python
n = 100
total = n * (n + 1) // 2
#path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
print(total)
```
```glsl
precision mediump float;

uniform vec2 resolution;
uniform float time;

void main() {
    mat3 lense = mat3(1.,0.,0.,
                        0.,888989898989898989,0.,
                        0.,0.,1.);
        vec3 pos3 = vec3(pos,camDir.z*10.);
        camDir = vec3(
                camDir.x,
                camDir.y,
                camDir.z);
        camDir*=normalize(dot(camDir,pos3));
    vec3 rayDir = normalize(camSide*pos.x + camUp*pos.y + camDir*focus);

    vec3 normal = getNormal(posOnRay);
    if(abs(d) < 0.001)
    {
        gl_FragColor = vec4(normal, 1.0);
    }else
    {
        gl_FragColor = vec4(0.0);
    }
}
```

这样应该就能得到结果了。或者直接用公式的话，可能更简洁：

```lua
-- Two dashes start a one-line comment.
--[[
   Adding two ['s and ]'s makes it a
   multi-line comment.
http://learnxinyminutes.com/
Have fun with Lua!
--]]
```

这两种方法都可行，我觉得第二种更好，因为效率更高而且代码更简洁。

现在整理一下：

C++：
同样，两种方法都可以。

Python：
可以用循环、公式，或者生成一个range然后求和。

那现在我来写出这三种语言的代码：

C++：
```cpp
#include <iostream>

using namespace std;

int main() {
    #path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
    int n = 100;
    int sum = n * (n + 1) / 2;
    cout << sum << endl;
    return 0;
}
```

或者循环：

```cpp
#include <iostream>

using namespace std;

int main() {
    return 0;
}
```

Python：
```python
n = 100
total = n * (n + 1) // 2
#path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
print(total)
```

或者循环：

```python
sum = 0
for i in range(1, 101):
    sum += i
```

或者使用range和sum函数：

print(sum(range(1, 101)))

嗯，我觉得这三种方法都是可行的。现在，我需要把它们分别用C++和Python写出来。

总结一下，每个语言可以有两种实现方式，但问题可能只需要一种，所以我可以选择其中一种，比如公式的方法，这样更简洁高效。
</think>

这些代码都使用了高斯公式来计算从 1 到 100 的和，结果都是 5050。

### Python
```python
n = 100
total = n * (n + 1) // 2
#path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
print(total)
```

这些代码都使用了高斯公式来计算从 1 到 100 的和，结果都是 5050。

### Python
```python
n = 100
total = n * (n + 1) // 2
#path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
print(total)
```

这些代码都使用了高斯公式来计算从 1 到 100 的和，结果都是 5050。

### Python
```python
n = 100
total = n * (n + 1) // 2
#path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
print(total)
```

以下是使用 C++ 和 Python 分别计算整数 1 到 100 的和的代码示例：

### C++
```cpp
#include <iostream>

using namespace std;

int main() {
    #path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
    int n = 100;
    int sum = n * (n + 1) / 2;
    cout << sum << endl;
    return 0;
}
```

### Python
```python
n = 100
total = n * (n + 1) // 2
#path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
print(total)
```

这些代码都使用了高斯公式来计算从 1 到 100 的和，结果都是 5050。

### Python
```python
n = 100
tota = n * (n + 1) // 2
#path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
print(total)
```

这些代码都使用了高斯公式来计算从 1 到 100 的和，结果都是 5050。

### Python
```python
n = 100
total = n * (n + 1) // 2
#path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
print(total)
```

以下是使用 C++ 和 Python 分别计算整数 1 到 100 的和的代码示例：

### C++
)";

const QString testText_code = R"(<think>
让我想一下两种方法。第一种方法更直观，适合新手理解。第二种方法效率更高，特别是当n很大的时候。那么对于这个问题来说，两种方式都行。我应该两种方法都写吗？可能问题只需要一种实现，但为了全面，我可以两种情况都考虑一下。

那我先写出循环的方法。代码结构大概是：

```cpp
int sum = 0;
for (int i = 1; i <= 100; ++i) {
    #path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
    sum += i;
}
std::cout << sum << std::endl;

#include <iostream>
#include <QDebug>

#define sdfgs	"fdgdf"
#define cfdsf	546

int main()
{
    int n, sum = 0;

    std::cout << "Enter a positive integer: ";
    std::cin >> n;

    for (int i = 1; i <= n; ++i)
    {
        sum += i;
    }

    std::cout << "Sum = " << sum;
    return 0;
}
int fun()
fun()
std:: fun()
int std::fun ()
float std::std::fun()
fun

// sdassadfsa sda

// sada

/* jkhj
jkk
gjh */

int xf;
int ydsf = 1;
int sdy=0
int xv + =1
int xvbd+ =1
int xf+=1
int xfddd - =1
int fvdx * =1
int xfg / =1
int xh % =1
int xnb , y ;
int xdgf = 1,

a.b.fun()
12.3
```

然后是Python的部分。Python的语法更简单，循环的话：
### jfdls;lf
```python
sum = 0
for i in range(1, 101):
    sum += i
#path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
print(sum)

import sys
from PyQt5.QtWidgets import QTextEdit, QApplication, QWidget
from PyQt5.QtGui import QTextCharFormat, QColor, QPainter, QFont
from PyQt5.QtCore import Qt, QRect
from pygments import highlight
from pygments.lexers import PythonLexer
from pygments.formatters import HtmlFormatter

class LineNumberArea(QWidget):
    def __init__(self, editor):
        super().__init__(editor)
        self.editor = editor
        xd - = 8
        fy += 6
        ztrh *= 7
        xr /= 7
        yrte % = 9
        self.editor.document().blockCountChanged.connect(self.update_width)
        """ self.editor.updateRequest.connect(self.update) """
        print(self.editor.document())

    def paintEvent(self, event):
        """ painter = QPainter(self)
        painter.fillRect(event.rect(), QColor(240, 240, 240))
        block = self.editor.document().begin()
        while block and block.isValid():
            line_number = block.blockNumber() + 1
            painter.drawText(0, block.position() -
            self.editor.contentOffset().y(), str(line_number))
            block = block.next() """
        painter = QPainter(self)
        painter.fillRect(event.rect(), QColor(240, 240, 240))
        """ for count in range(0, self.editor.document().blockCount()): """
        painter.end()

class CodeEditor(QTextEdit):
    def __init__(self):
        super().__init__()
        self.setTabStopWidth(4)
        self.lineNumberArea = LineNumberArea(self)

    def resizeEvent(self, event):
        super().resizeEvent(event)
        # 更新行号区域位置
        cr = self.contentsRect()
        self.lineNumberArea.setGeometry(QRect(cr.left(), cr.top(),
        self.lineNumberArea.width(), cr.height()))

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = CodeEditor()
    window.show()
    sys.exit(app.exec_())
```

总结一下，每个语言可以有两种实现方式，但问题可能只需要一种，所以我可以选择其中一种，比如公式的方法，这样更简洁高效。
</think>

以下是分别计算整数 1 到 100 的和的代码示例：

### C++
```cpp
#include <iostream>

using namespace std;

int main() {
    #path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
    int n = 100;
    int sum = n * (n + 1) / 2;
    cout << sum << endl;
    return 0;
}
```

### Python
```python
n = 100
total = n * (n + 1) // 2
#path.addRoundedRect(self.rect().x() + 1, self.rect().y() + 1, self.rect().width() - 2, self.rect().height() - 2, 16, 16)
print(total)
```

这些代码都使用了高斯公式来计算从 1 到 100 的和，结果都是 5050。
)";

const QString testText_markdown = R"(
一级标题
========
二级标题
--------
------------------
# 一级标题
## 二级标题
### 三级标题
这是一个段落，包含一些 *Markdown* 语法。
*斜体文本*
_斜体文本_
**粗体文本**
__粗体文本__
***粗斜体文本***
___粗斜体文本___
~~删除线文本~~
<u>带下划线文本</u>
苹果10$，梨子20$，香蕉30$，橘子40$。
苹果10\$，梨子20\$，香蕉30\$，橘子40\$。
苹果10\\$，梨子20\\$，香蕉30\\$，橘子40\\$。
$\alpha$ $\beta$ $\gamma$ $\delta$ $\epsilon$ $\zeta$ $\eta$ $\theta$ $\iota$ $\kappa$ $\lambda$ $\mu$ $\nu$ $\xi$ $\omicron$ $\pi$ $\rho$ $\sigma$ $\tau$ $\upsilon$ $\phi$ $\chi$ $\psi$ $\omega$ $\Gamma$ $\Delta$ $\Theta$ $\Lambda$ $\Xi$ $\Pi$ $\Sigma$ $\Upsilon$ $\Phi$ $\Psi$ $\Omega$
创建脚注格式类似这样 [^RUNOOB]。
[^RUNOOB]: 菜鸟教程 -- 学的不仅是技术，更是梦想！！！

******************
### 无序列表
* 第一项
* 第二项
* 第三项

+ 第一项
+ 第二项
+ 第三项

- 第一项
- 第二项
- 第三项

### 有序列表
1. 第一项
2. 第二项
3. 第三项

### 列表嵌套
1. 第一项
    - 第一项嵌套的第一个元素
    - 第一项嵌套的第二个元素
2. 第二项
    1. 第二项嵌套的第一个元素
    2. 第二项嵌套的第二个元素
******************
### 区块
> 菜鸟教程
> 学的不仅是技术更是梦想

> 最外层
> > 第一层嵌套
> > > 第二层嵌套

> 区块中使用列表
> 1. 第一项
> 2. 第二项
> + 第一项
> + 第二项
> + 第三项

列表中使用区块
* 第一项
    > 菜鸟教程
    > 学的不仅是技术更是梦想
* 第二项
******************
### 代码
`printHelloWorld()`函数

    def printHelloWorld():
        print('hello world')

```python
def printHelloWorld():
    print('hello world')
```
******************
### 链接
这是一个链接 [菜鸟教程](https://www.runoob.com)
这个链接用 1 作为网址变量 [Google][1]
这个链接用 runoob 作为网址变量 [Runoob][runoob]
然后在文档的结尾为变量赋值（网址）

[1]: http://www.google.com/
[runoob]: http://www.runoob.com/
******************
### 图片
![thinking 图标](images/thinking_icon.png "thinking")

这个链接用 2 作为网址变量 [thinking][2]
然后在文档的结尾为变量赋值（网址）

[2]: images/thinking_icon.png

<img src="images/thinking_icon.png" width="25%">

******************
### 表格
|  表头  |  表头  |  表头  |
| :----- | -----: | :----: |
| 单元格 | 单元格 | 单元格 |
| 单元格 | 单元格 | 单元格 |
******************
### LaTeX公式：
这是一个行内公式 $E=mc^2$ 的示例。
行间公式：
$$\sum_{i=1}^n a_i$$
$$\frac{a}{b} = c$$
积分公式：
$$\int_{a}^{b} {f(x)} \, \mathrm{d}x = F(b) - F(a)$$
微分公式：
$$\frac{d}{dx} e^x = e^x$$
)";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      message(""),
      isShowFirst(true),
      isProcessing(false),
      isSetTexting(false),
      isRegenerating(false),
      isRegeneratePending(false),
      pushButtonIsPress(false),
      screenChanged(false),
      isSending(false),
      isContinueShow(true),
      isScreenMax(false),
      isScreenHalf(false),
      isChangeRectFirst(false),
      isDpiChanged(false),
      avoidRepeatSelfFun(false),
      borderLen(3)
{
    QWebEngineProfile *sharedProfile = new QWebEngineProfile("shared");
    sharedProfile->setHttpCacheType(QWebEngineProfile::DiskHttpCache);
    sharedProfile->setCachePath(webEngineCacheDir);
    QWebEngineView *dummyView = new QWebEngineView();
    // dummyView->setPage(new WebEnginePage(sharedProfile, dummyView));
    WebEnginePage *page = new WebEnginePage(sharedProfile, nullptr); // 无 parent
    dummyView->setPage(page);
    connect(
            dummyView, &QWebEngineView::loadFinished, dummyView,
            [dummyView]() {
                qDebug() << "load finished";
                dummyView->stop();
                QTimer::singleShot(0, dummyView, [dummyView]() {
                    if (dummyView->page()) {
                        dummyView->page()->triggerAction(QWebEnginePage::Stop);
                    }
                    QTimer::singleShot(100, dummyView, [dummyView]() {
                        QWebEnginePage *page = dummyView->page();
                        dummyView->setPage(nullptr);
                        if (page) {
                            delete page;
                        }
                        QTimer::singleShot(500, dummyView, [dummyView]() {
                            delete dummyView;
                            qDebug() << "cleanup done";
                        });
                    });
                });
            },
            Qt::QueuedConnection);

    // connect(
    //         dummyView, &QWebEngineView::loadFinished, dummyView,
    //         [dummyView, page]() {
    //             qDebug() << "load finished, scheduling safe delete";
    //             dummyView->stop();
    //             QTimer::singleShot(0, [dummyView, page]() {
    //                 dummyView->setPage(nullptr);
    //                 delete page;
    //                 delete dummyView;
    //                 qDebug() << "safe delete completed";
    //             });
    //         },
    //         Qt::QueuedConnection);

    // QTimer::singleShot(1000, dummyView, [dummyView]() {
    //     qDebug() << "delayed delete dummyView";
    //     QWebEnginePage *page = dummyView->page();
    //     dummyView->setPage(nullptr);
    //     delete page;
    //     dummyView->deleteLater();
    // });

    // connect(dummyView, &QWebEngineView::loadFinished, this, [dummyView]() {
    //     qDebug() << "delayed delete dummyView";
    //     QWebEnginePage *page = dummyView->page();
    //     dummyView->setPage(nullptr);
    //     delete page;
    //     dummyView->deleteLater();
    // });
    dummyView->load(QUrl("about:blank"));
    // connect(dummyView, &QWebEngineView::loadFinished, []() { qDebug() << "dummyView delete"; });

    setMinimumSize(1110, 795);
    resize(1200, 800);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    setMouseTracking(true);

    mouseLeftButtonIsPress = false;
    regionDir = RegionEnum::Middle;
    padding = 2;
    titleWidgetInit();
    chatFun = new FunWidget();
    // qDebug() << "FunWidget size:" << chatFun->size();
    chatFun->connectChatRecordsButtonClick(this, &MainWindow::showChatRecords);
    chatFun->connectNewChatButtonClick(this, &MainWindow::newChat);
    chatShow = new ListWidget();
    chatShowWidget = new Widget();
    chatShowWidget->setMinimumHeight(244);
    chatShowWidget->resize(1200, 500);
    chatShowWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    chatShowVLayout = new QVBoxLayout(chatShowWidget);
    chatShowVLayout->addWidget(chatShow);
    chatShowVLayout->setContentsMargins(20, 4, 10, 16);
    chatInput = new TextEditFull();
    chatInput->connectSendButtonClick(this, &MainWindow::sendMessage);
    chatInputWidget = new Widget();
    chatInputWidget->setMinimumHeight(100);
    chatInputWidget->resize(1200, 200);
    chatInputWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    chatInputVLayout = new QVBoxLayout(chatInputWidget);
    chatInputVLayout->addWidget(chatInput);
    chatInputVLayout->setContentsMargins(20, 0, 20, 20);
    splitter = new Splitter(Qt::Vertical);
    splitter->resize(1200, 700);
    splitter->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(chatShowWidget);
    splitter->addWidget(chatInputWidget);
    splitter->setContentsMargins(0, 0, 0, 0);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);
    splitter->setHandleWidth(0);
    contentWidget = new Widget();
    contentWidget->resize(1200, 760);
    contentWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    contentVLayout = new QVBoxLayout(contentWidget);
    contentVLayout->addWidget(chatFun);
    contentVLayout->addWidget(splitter);
    contentVLayout->setContentsMargins(0, 0, 0, 0);
    contentVLayout->setSpacing(0);
    contentVLayout->setStretch(0, 0);
    contentVLayout->setStretch(1, 1);
    mainWidget = new Widget();
    mainWidget->resize(size());
    mainWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    mainWidget->setObjectName("mainWidget");
    mainWidget->setStyleSheet("#mainWidget {"
                              "    background-color: #F0F0F0;"
                              "}");
    setCentralWidget(mainWidget);
    mainVLayout = new QVBoxLayout(mainWidget);
    mainVLayout->addWidget(titleWidget);
    mainVLayout->addWidget(contentWidget);
    mainVLayout->setContentsMargins(0, 0, 0, 0);
    mainVLayout->setSpacing(0);
    mainVLayout->setStretch(0, 0);
    mainVLayout->setStretch(1, 1);
    qDebug() << "FunWidget geometry:" << chatFun->geometry();
    // QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    // shadow->setBlurRadius(20);
    // shadow->setOffset(0, 0);
    // shadow->setColor(QColor(0, 0, 0, 150));
    // setGraphicsEffect(shadow);
    settingWidgetInit();
    chatRecordsWidgetInit();
    emptyTextLabel = new PrintLabel("文本不能为空", this);
    emptyTextLabel->move((width() - emptyTextLabel->width()) / 2,
                         titleWidget->height() + chatFun->height() + chatShowWidget->height() + 10);
    emptyTextLabel->raise();
    emptyTextLabel->hide();
    textCopyLabel = new PrintLabel("文本复制成功", this);
    textCopyLabel->move((width() - textCopyLabel->width()) / 2,
                        titleWidget->height() + chatFun->height() + chatShowWidget->height() + 10);
    textCopyLabel->raise();
    textCopyLabel->hide();

    messageWidgetList.clear();
    lastScreen = curScreen = screen();
    initDpi = lastDpi = curDpi = curScreen->logicalDotsPerInch();
    thinkExpandedList.clear();
    thinkTimeLengthList.clear();
    lastNormalGeometry = geometry();
    uiRectWidth = width();
    uiRectHeight = height();
    screens = QApplication::screens();
    for (QScreen *screen : screens) {
        connect(screen, &QScreen::logicalDotsPerInchChanged, this, &MainWindow::onDpiChanged);
    }

    widgetSizeDict["MainWindow"] = size();
    widgetSizeDict["MainWindow minimumSize"] = minimumSize();
    widgetSizeDict["mainWidget"] = mainWidget->size();
    widgetSizeDict["mainWidget x"] = mainWidget->x();
    widgetSizeDict["mainWidget y"] = mainWidget->y();

    checkGraphicsBackend();

#ifdef Q_OS_WIN
    HWND hwnd = reinterpret_cast<HWND>(winId());
    DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                 SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

    // HWND hwnd = reinterpret_cast<HWND>(winId());
    // DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    // style = (style & ~WS_CAPTION) | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
    // SetWindowLongPtr(hwnd, GWL_STYLE, style);

    // DWORD exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    // exStyle |= WS_EX_LAYERED;
    // SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

    // SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
    //              SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
#endif
}

MainWindow::~MainWindow() { }

void MainWindow::titleWidgetInit()
{
    titleWidget = new TitleWidget();
    connect(titleWidget, &TitleWidget::minimizeClicked, this, &MainWindow::uiMinimize);
    connect(titleWidget, &TitleWidget::maximizeClicked, this, &MainWindow::uiMaximize);
    connect(titleWidget, &TitleWidget::closeClicked, this, &MainWindow::uiClose);
}

void MainWindow::settingWidgetInit()
{
    settingWidget = new SettingWidget(mainWidget);
    settingWidget->setGeometry(-mainWidget->width() / 3, titleWidget->height(),
                               mainWidget->width() / 3,
                               mainWidget->height() - titleWidget->height());
    connect(settingWidget, &SettingWidget::baseUrlTextChanged, this,
            &MainWindow::onBaseUrlTextChanged);
    connect(settingWidget, &SettingWidget::apiKeyTextChanged, this,
            &MainWindow::onApiKeyTextChanged);
    connect(settingWidget, &SettingWidget::modelNameTextChanged, this,
            &MainWindow::onModelNameTextChanged);
    connect(settingWidget, &SettingWidget::maxTokensBoxValueChanged, this,
            &MainWindow::onMaxTokensBoxValueChanged);
    connect(settingWidget, &SettingWidget::topPBoxValueChanged, this,
            &MainWindow::onTopPBoxValueChanged);
    connect(settingWidget, &SettingWidget::temperatureBoxValueChanged, this,
            &MainWindow::onTemperatureBoxValueChanged);
    connect(settingWidget, &SettingWidget::maxTokensSliderValueChanged, this,
            &MainWindow::onMaxTokensSliderValueChanged);
    connect(settingWidget, &SettingWidget::topPSliderValueChanged, this,
            &MainWindow::onTopPSliderValueChanged);
    connect(settingWidget, &SettingWidget::temperatureSliderValueChanged, this,
            &MainWindow::onTemperatureSliderValueChanged);

    settingAnimationMove = new QPropertyAnimation(settingWidget, "geometry");
    settingAnimationMove->setDuration(1000);
    settingAnimationMove->setEasingCurve(QEasingCurve::OutQuad);

    settingWidgetIsOpen = false;
}

void MainWindow::chatRecordsWidgetInit()
{
    chatRecordsWidget = new ChatRecordsWidget(mainWidget);
    chatRecordsWidget->setGeometry(-mainWidget->width() / 3, titleWidget->height(),
                                   mainWidget->width() / 3,
                                   mainWidget->height() - titleWidget->height());
    chatRecordsWidget->connectSettingButtonClick(this, &MainWindow::settingButtonClicked);
    chatRecordsWidget->connectLineEditTextChanged(this, &MainWindow::showSearchRecords);
    chatRecordsWidget->connectSearchButtonClick(this, &MainWindow::showSearchRecords);
    chatRecordsWidget->connectClearAllButtonClick(this, &MainWindow::clearAllChatRecords);
    chatRecordsWidget->connectListItemClick(this, &MainWindow::generateChatRecord);

    chatRecordsAnimationMove = new QPropertyAnimation(chatRecordsWidget, "geometry");
    chatRecordsAnimationMove->setDuration(1000);
    chatRecordsAnimationMove->setEasingCurve(QEasingCurve::OutQuad);
    connect(chatRecordsAnimationMove, &QPropertyAnimation::valueChanged, this,
            &MainWindow::chatRecordsUiAnimationMove);
    connect(chatRecordsAnimationMove, &QPropertyAnimation::finished, this,
            &MainWindow::chatRecordsUiMoveFinished);

    curChatFile = "";
    chatRecordsWidgetIsOpen = false;
}

void MainWindow::checkGraphicsBackend()
{
    qwindow = new QQuickWindow();
    qwindow->show();
    QTimer::singleShot(500, [&]() {
        qDebug() << "===== 图形后端信息 =====";
        qDebug() << "当前渲染器:" << qwindow->rendererInterface()->graphicsApi();
        qDebug() << "场景图后端:" << qwindow->sceneGraphBackend();
        qDebug() << "======================";
        qwindow->close();
        qwindow->deleteLater();
    });
}

void MainWindow::applyDWMShadow()
{
#ifdef Q_OS_WIN
    HWND hwnd = reinterpret_cast<HWND>(winId());
    if (!hwnd || !IsWindow(hwnd)) {
        return;
    }

    DWMNCRENDERINGPOLICY policy = DWMNCRP_ENABLED;
    DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof(policy));

    MARGINS margins = { 1, 1, 1, 1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    RedrawWindow(hwnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
    // SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
    //              SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER
    //                      | SWP_NOACTIVATE | SWP_SHOWWINDOW);
#endif
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

#ifdef Q_OS_WIN
    // static bool firstShow = true;
    // if (firstShow) {
    //     firstShow = false;

    //     HWND hwnd = reinterpret_cast<HWND>(winId());
    //     if (!hwnd)
    //         return;

    //     DWORD style = GetWindowLong(hwnd, GWL_STYLE);

    //     style |= WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CAPTION;
    //     style &= ~(WS_BORDER | WS_DLGFRAME);
    //     SetWindowLongPtr(hwnd, GWL_STYLE, style);

    //     DWORD exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    //     exStyle &= ~(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_LAYERED);
    //     exStyle |= WS_EX_NOREDIRECTIONBITMAP;
    //     SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

    //     SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
    //                  SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER
    //                          | SWP_NOACTIVATE);

    //     applyDWMShadow();
    // }
    applyDWMShadow();
#endif
}

void MainWindow::changeEvent(QEvent *event)
{
#ifdef Q_OS_WIN
    if (event->type() == QEvent::WindowStateChange) {
        QWindowStateChangeEvent *stateEvent = static_cast<QWindowStateChangeEvent *>(event);
        Qt::WindowStates oldState = stateEvent->oldState();
        Qt::WindowStates newState = windowState();

        if ((oldState & Qt::WindowMinimized) && !(newState & Qt::WindowMinimized)) {
            qDebug() << "从最小化恢复";
            QTimer::singleShot(10, this, [this]() { applyDWMShadow(); });
        }
        // if ((newState & Qt::WindowMaximized) && !(oldState & Qt::WindowMaximized)) {
        if (!(oldState & Qt::WindowMaximized) && (newState & Qt::WindowMaximized)) {
            qDebug() << "最大化";
            titleWidget->maxButtonToggleIcon(false);
            QTimer::singleShot(10, this, [this]() { applyDWMShadow(); });
            // } else if ((newState & Qt::WindowNoState) && (oldState & Qt::WindowMaximized)) {
        } else if ((oldState & Qt::WindowMaximized) && !(newState & Qt::WindowMaximized)) {
            qDebug() << "从最大化恢复正常";
            titleWidget->maxButtonToggleIcon(true);
            QTimer::singleShot(10, this, [this]() { applyDWMShadow(); });
        }
    }
#endif

    QMainWindow::changeEvent(event);
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
#ifdef Q_OS_WIN
    if (eventType != "windows_generic_MSG")
        return false;

    MSG *msg = static_cast<MSG *>(message);
    HWND hwnd = msg->hwnd;

    switch (msg->message) {
    case WM_NCCALCSIZE: {
        *result = 0;
        return true;
    }

    case WM_NCHITTEST: {
        POINT pt;
        pt.x = GET_X_LPARAM(msg->lParam);
        pt.y = GET_Y_LPARAM(msg->lParam);

        ScreenToClient(hwnd, &pt);

        RECT rcClient;
        GetClientRect(hwnd, &rcClient);

        int nX = pt.x;
        int nY = pt.y;
        int w = rcClient.right;
        int h = rcClient.bottom;

        WINDOWPLACEMENT wp = { sizeof(wp) };
        bool isMaximized = GetWindowPlacement(hwnd, &wp) && wp.showCmd == SW_MAXIMIZE;

        if (isMaximized) {
            int titleHeight = titleWidget->height();

            if (nY >= 0 && nY < titleHeight) {
                int closeBtnWidth = titleWidget->getCloseButtonSize().width();
                int maxBtnWidth = titleWidget->getMaxButtonSize().width();
                int minBtnWidth = titleWidget->getMinButtonSize().width();
                int btnRight = w;
                int btnLeft = btnRight - closeBtnWidth - maxBtnWidth - minBtnWidth;

                if (btnLeft < 0)
                    btnLeft = 0;

                if (nX >= btnLeft && nX < btnRight) {
                    *result = HTCLIENT;
                    return true;
                }
                *result = HTCAPTION;
                return true;
            }
            *result = HTCLIENT;
            return true;
        }

        const int detectBorder = 8;
        const int titleHeight = titleWidget->height();

        int closeBtnWidth = titleWidget->getCloseButtonSize().width();
        int maxBtnWidth = titleWidget->getMaxButtonSize().width();
        int minBtnWidth = titleWidget->getMinButtonSize().width();
        int btnRight = w;
        int btnLeft = btnRight - closeBtnWidth - maxBtnWidth - minBtnWidth;
        if (btnLeft < detectBorder)
            btnLeft = detectBorder;

        if (nX >= 0 && nX < detectBorder && nY >= 0 && nY < detectBorder) {
            *result = HTTOPLEFT;
            return true;
        }
        if (nX >= w - detectBorder && nX < w && nY >= 0 && nY < detectBorder) {
            *result = HTTOPRIGHT;
            return true;
        }
        if (nX >= 0 && nX < detectBorder && nY >= h - detectBorder && nY < h) {
            *result = HTBOTTOMLEFT;
            return true;
        }
        if (nX >= w - detectBorder && nX < w && nY >= h - detectBorder && nY < h) {
            *result = HTBOTTOMRIGHT;
            return true;
        }

        if (nY >= 0 && nY < detectBorder && nX >= detectBorder && nX < w - detectBorder) {
            *result = HTTOP;
            return true;
        }
        if (nY >= h - detectBorder && nY < h && nX >= detectBorder && nX < w - detectBorder) {
            *result = HTBOTTOM;
            return true;
        }
        if (nX >= 0 && nX < detectBorder && nY >= detectBorder && nY < h - detectBorder) {
            *result = HTLEFT;
            return true;
        }
        if (nX >= w - detectBorder && nX < w && nY >= detectBorder && nY < h - detectBorder) {
            *result = HTRIGHT;
            return true;
        }

        if (nY >= detectBorder && nY < titleHeight) {
            if (nX >= btnLeft && nX < btnRight) {
                *result = HTCLIENT;
                return true;
            }
            if (nX >= detectBorder && nX < btnLeft) {
                // qDebug() << "MainWindow::nativeEvent HTCAPTION";
                *result = HTCAPTION;
                return true;
            }
        }

        *result = HTCLIENT;
        return true;
    }

    case WM_GETMINMAXINFO: {
        MINMAXINFO *mmi = reinterpret_cast<MINMAXINFO *>(msg->lParam);
        MONITORINFO mi = { sizeof(mi) };
        if (GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &mi)) {
            mmi->ptMaxPosition.x = mi.rcWork.left + 1;
            mmi->ptMaxPosition.y = mi.rcWork.top + 1;
            mmi->ptMaxSize.x = mi.rcWork.right - mi.rcWork.left;
            mmi->ptMaxSize.y = mi.rcWork.bottom - mi.rcWork.top;
        }
        mmi->ptMinTrackSize.x = 1100;
        mmi->ptMinTrackSize.y = 795;
        *result = 0;
        return true;
    }

    case WM_SIZE: {
        if (msg->wParam == SIZE_RESTORED || msg->wParam == SIZE_MAXIMIZED) {
            QTimer::singleShot(10, this, &MainWindow::applyDWMShadow);
        }
        break;
    }

    case WM_EXITSIZEMOVE: {
        QTimer::singleShot(10, this, &MainWindow::applyDWMShadow);

        qDebug() << "WM_EXITSIZEMOVE isRegenerate";
        messageWidgetRegenerate();
        break;
    }

    case WM_SYSCOMMAND: {
        break;
    }

    default:
        break;
    }
#endif
    return QMainWindow::nativeEvent(eventType, message, result);
}

// void MainWindow::mousePressEvent(QMouseEvent *event)
// {
//     qDebug() << "MainWindow::mousePressEvent";
//     QMainWindow::mousePressEvent(event);
//     if (event->button() == Qt::LeftButton) {
//         QPoint pos = event->pos();

//         int titleHeight = titleWidget->height();

//         if (pos.y() < titleHeight) {
//             int closeBtnWidth = titleWidget->getCloseButtonSize().width();
//             int maxBtnWidth = titleWidget->getMaxButtonSize().width();
//             int minBtnWidth = titleWidget->getMinButtonSize().width();
//             int btnRight = width();
//             int btnLeft = btnRight - closeBtnWidth - maxBtnWidth - minBtnWidth;

//             if (btnLeft < 0)
//                 btnLeft = 0;

//             if (pos.x() < btnLeft) {
// #ifdef Q_OS_WIN
//                 HWND hwnd = reinterpret_cast<HWND>(winId());
//                 if (hwnd) {
//                     ReleaseCapture();
//                     SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
//                 }
// #endif
//                 return;
//             }
//         }
//     }
// }

// void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
// {
//     // QMainWindow::mouseDoubleClickEvent(event);
//     if (event->button() == Qt::LeftButton) {
//         QPoint pos = event->pos();
//         // QPoint pos = mapFromGlobal(event->globalPosition().toPoint());
//         // qDebug() << "MainWindow::mouseDoubleClickEvent" << pos << event->globalPosition().toPoint()
//         //          << mapFromGlobal(event->globalPosition().toPoint());
//         int titleHeight = titleWidget->height();

//         if (pos.y() < titleHeight) {
//             int closeBtnWidth = titleWidget->getCloseButtonSize().width();
//             int maxBtnWidth = titleWidget->getMaxButtonSize().width();
//             int minBtnWidth = titleWidget->getMinButtonSize().width();
//             int btnRight = width();
//             int btnLeft = btnRight - closeBtnWidth - maxBtnWidth - minBtnWidth;

//             if (btnLeft < 0)
//                 btnLeft = 0;

//             if (pos.x() < btnLeft) {
//                 qDebug() << "mouseDoubleClickEvent uiMaximize";
//                 uiMaximize();
//                 return;
//             }
//         }
//     }
// }

// void MainWindow::paintEvent(QPaintEvent *event)
// {
//     QMainWindow::paintEvent(event);

//     if (!isWindowMaximized()) {
//         QPainter painter(this);
//         painter.setRenderHint(QPainter::Antialiasing);

//         QPen pen(QColor(200, 200, 200, 180));
//         pen.setWidth(1);
//         painter.setPen(pen);
//         painter.setBrush(Qt::NoBrush);

//         QRectF rect = mainWidget->geometry().adjusted(0.5, 0.5, -0.5, -0.5);

//         if (rect.width() > 0 && rect.height() > 0) {
//             painter.drawRoundedRect(rect, 16, 16);
//         }
//     }
// }

void MainWindow::moveEvent(QMoveEvent *event)
{
    curScreen = screen();
    if (lastScreen != curScreen) {
        lastScreen = curScreen;
        screenChanged = true;
    }
    QMainWindow::moveEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    isItemShowFull(childAt(event->pos()));
    // QPoint cursorGlobalPos = event->globalPosition().toPoint();
    // cursorGlobalX = cursorGlobalPos.x();
    // cursorGlobalY = cursorGlobalPos.y();
    // uiGlobalTL = mainWidget->mapToGlobal(QPoint(0, 0));
    // uiGlobalBR = mainWidget->mapToGlobal(QPoint(mainWidget->width() - 1, mainWidget->height() - 1));
    // if (!mouseLeftButtonIsPress)
    //     regionDivision();
    // else {
    //     if ((regionDir != RegionEnum::Title) && (regionDir != RegionEnum::Button)
    //         && (regionDir != RegionEnum::Middle))
    //         UiStretch();
    //     else {
    //         if (regionDir == RegionEnum::Title) {
    //             UiDrag(event->globalPosition().toPoint());
    //             if (!(screens.size() > 1)) {
    //                 QRect screenGeometry = screen()->availableGeometry();
    //                 if (event->globalPosition().toPoint().x() <= screenGeometry.x()) {
    //                     if (!((width() == screenGeometry.width() / 2)
    //                           && (height() == screenGeometry.height()))) {
    //                         uiRectWidth = width();
    //                         uiRectHeight = height();
    //                         isChangeRectFirst = true;
    //                     }
    //                     qDebug() << "mouseMoveEvent screenGeometry:" << screenGeometry;
    //                     setGeometry(screenGeometry.x(), screenGeometry.y(),
    //                                 screenGeometry.width() / 2, screenGeometry.height());
    //                     mainWidget->setGeometry(0, 0, width(), height());
    //                     mainWidget->setStyleSheet("#mainWidget {"
    //                                               "    background-color: #F0F0F0;"
    //                                               "}");
    //                     titleWidget->setRightAngle();
    //                     isScreenHalf = true;
    //                 } else {
    //                     if (isChangeRectFirst) {
    //                         isChangeRectFirst = false;
    //                         resize(uiRectWidth, uiRectHeight);
    //                         mainWidget->setStyleSheet("#mainWidget {"
    //                                                   "    border-radius: 16px;"
    //                                                   "    background-color: #F0F0F0;"
    //                                                   "}");
    //                         titleWidget->setRoundAngle();
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::isItemShowFull(QWidget *widget)
{
    for (int i = 0; i < messageWidgetList.size(); ++i) {
        MessageWidget *messageWidget = messageWidgetList.at(i);
        messageWidget->hideFunWidget();
    }
    if (TextWidget *textWidget = qobject_cast<TextWidget *>(widget)) {
        for (int i = 0; i < messageWidgetList.size(); ++i) {
            if (textWidget == messageWidgetList.at(i)->getTextWidget()) {
                messageWidgetList.at(i)->showFunWidget();
            }
        }
    } else if (TextBoxWidget *textBoxWidget = qobject_cast<TextBoxWidget *>(widget)) {
        for (int i = 0; i < messageWidgetList.size(); ++i) {
            if (textBoxWidget == messageWidgetList.at(i)->getTextBoxWidget()) {
                messageWidgetList.at(i)->showFunWidget();
            }
        }
    } else if (MessageWidget *messageWidget = qobject_cast<MessageWidget *>(widget)) {
        for (int i = 0; i < messageWidgetList.size(); ++i) {
            if (messageWidget == messageWidgetList.at(i)) {
                messageWidgetList.at(i)->showFunWidget();
            }
        }
    } else if (ItemWidget *itemWidget = qobject_cast<ItemWidget *>(widget)) {
        QLayoutItem *layoutItem = itemWidget->layout()->itemAt(0);
        if (layoutItem) {
            if (MessageWidget *childWidget = qobject_cast<MessageWidget *>(layoutItem->widget())) {
                for (int i = 0; i < messageWidgetList.size(); ++i) {
                    if (childWidget == messageWidgetList.at(i)) {
                        messageWidgetList.at(i)->showFunWidget();
                    }
                }
            }
        }
    }
}

void MainWindow::regionDivision()
{
    if ((cursorGlobalX >= uiGlobalTL.x()) && (cursorGlobalX <= uiGlobalTL.x() + padding)
        && (cursorGlobalY >= uiGlobalTL.y()) && (cursorGlobalY <= uiGlobalTL.y() + padding)) {
        regionDir = RegionEnum::LeftTop;
        setCursor(QCursor(Qt::SizeFDiagCursor));
    } else if ((cursorGlobalX >= uiGlobalBR.x() - padding) && (cursorGlobalX <= uiGlobalBR.x())
               && (cursorGlobalY >= uiGlobalTL.y())
               && (cursorGlobalY <= uiGlobalTL.y() + padding)) {
        regionDir = RegionEnum::RightTop;
        setCursor(QCursor(Qt::SizeBDiagCursor));
    } else if ((cursorGlobalX >= uiGlobalTL.x()) && (cursorGlobalX <= uiGlobalTL.x() + padding)
               && (cursorGlobalY >= uiGlobalBR.y() - padding)
               && (cursorGlobalY <= uiGlobalBR.y())) {
        regionDir = RegionEnum::LeftBottom;
        setCursor(QCursor(Qt::SizeBDiagCursor));
    } else if ((cursorGlobalX >= uiGlobalBR.x() - padding) && (cursorGlobalX <= uiGlobalBR.x())
               && (cursorGlobalY >= uiGlobalBR.y() - padding)
               && (cursorGlobalY <= uiGlobalBR.y())) {
        regionDir = RegionEnum::RightBottom;
        setCursor(QCursor(Qt::SizeFDiagCursor));
    } else if ((cursorGlobalX >= uiGlobalTL.x()) && (cursorGlobalX <= uiGlobalTL.x() + padding)) {
        regionDir = RegionEnum::Left;
        setCursor(QCursor(Qt::SizeHorCursor));
    } else if ((cursorGlobalX >= uiGlobalBR.x() - padding) && (cursorGlobalX <= uiGlobalBR.x())) {
        regionDir = RegionEnum::Right;
        setCursor(QCursor(Qt::SizeHorCursor));
    } else if ((cursorGlobalY >= uiGlobalTL.y()) && (cursorGlobalY <= uiGlobalTL.y() + padding)) {
        regionDir = RegionEnum::Top;
        setCursor(QCursor(Qt::SizeVerCursor));
    } else if ((cursorGlobalY >= uiGlobalBR.y() - padding) && (cursorGlobalY <= uiGlobalBR.y())) {
        regionDir = RegionEnum::Bottom;
        setCursor(QCursor(Qt::SizeVerCursor));
    } else if ((cursorGlobalX >= uiGlobalTL.x() + padding + 1)
               && (cursorGlobalX <= uiGlobalBR.x() - padding - 1)
               && (cursorGlobalY >= uiGlobalTL.y() + padding + 1)
               && (cursorGlobalY <= uiGlobalTL.y() + titleWidget->height())) {
        if (cursorGlobalX <= uiGlobalBR.x() - titleWidget->getMinButtonSize().width()
                    - titleWidget->getMaxButtonSize().width()
                    - titleWidget->getCloseButtonSize().width() - 1)
            regionDir = RegionEnum::Title;
        else
            regionDir = RegionEnum::Button;
        setCursor(QCursor(Qt::ArrowCursor));
    } else {
        regionDir = RegionEnum::Middle;
        setCursor(QCursor(Qt::ArrowCursor));
    }
}

void MainWindow::UiStretch()
{
    QRect uiGlobalRect(uiGlobalTL, uiGlobalBR);
    switch (regionDir) {
    case RegionEnum::Left:
        if (uiGlobalBR.x() - cursorGlobalX
            > minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
            uiGlobalRect.setX(cursorGlobalX);
        else
            uiGlobalRect.setX(uiGlobalBR.x()
                              - (minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
                              + 1);
        break;
    case RegionEnum::Right:
        if (cursorGlobalX - uiGlobalTL.x()
            > minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
            uiGlobalRect.setWidth(cursorGlobalX - uiGlobalTL.x());
        else
            uiGlobalRect.setWidth(minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>());
        break;
    case RegionEnum::Top:
        if (uiGlobalBR.y() - cursorGlobalY
            > minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
            uiGlobalRect.setY(cursorGlobalY);
        else
            uiGlobalRect.setY(uiGlobalBR.y()
                              - (minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
                              + 1);
        break;
    case RegionEnum::Bottom:
        if (cursorGlobalY - uiGlobalTL.y()
            > minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
            uiGlobalRect.setHeight(cursorGlobalY - uiGlobalTL.y());
        else
            uiGlobalRect.setHeight(minimumHeight()
                                   - 2 * widgetSizeDict["mainWidget y"].value<int>());
        break;
    case RegionEnum::LeftTop:
        if (uiGlobalBR.x() - cursorGlobalX
            > minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
            uiGlobalRect.setX(cursorGlobalX);
        else
            uiGlobalRect.setX(uiGlobalBR.x()
                              - (minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
                              + 1);
        if (uiGlobalBR.y() - cursorGlobalY
            > minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
            uiGlobalRect.setY(cursorGlobalY);
        else
            uiGlobalRect.setY(uiGlobalBR.y()
                              - (minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
                              + 1);
        break;
    case RegionEnum::RightTop:
        if (cursorGlobalX - uiGlobalTL.x()
            > minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
            uiGlobalRect.setWidth(cursorGlobalX - uiGlobalTL.x());
        else
            uiGlobalRect.setWidth(minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>());
        if (uiGlobalBR.y() - cursorGlobalY
            > minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
            uiGlobalRect.setY(cursorGlobalY);
        else
            uiGlobalRect.setY(uiGlobalBR.y()
                              - (minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
                              + 1);
        break;
    case RegionEnum::LeftBottom:
        if (uiGlobalBR.x() - cursorGlobalX
            > minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
            uiGlobalRect.setX(cursorGlobalX);
        else
            uiGlobalRect.setX(uiGlobalBR.x()
                              - (minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
                              + 1);
        if (cursorGlobalY - uiGlobalTL.y()
            > minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
            uiGlobalRect.setHeight(cursorGlobalY - uiGlobalTL.y());
        else
            uiGlobalRect.setHeight(minimumHeight()
                                   - 2 * widgetSizeDict["mainWidget y"].value<int>());
        break;
    case RegionEnum::RightBottom:
        if (cursorGlobalX - uiGlobalTL.x()
            > minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
            uiGlobalRect.setWidth(cursorGlobalX - uiGlobalTL.x());
        else
            uiGlobalRect.setWidth(minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>());
        if (cursorGlobalY - uiGlobalTL.y()
            > minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
            uiGlobalRect.setHeight(cursorGlobalY - uiGlobalTL.y());
        else
            uiGlobalRect.setHeight(minimumHeight()
                                   - 2 * widgetSizeDict["mainWidget y"].value<int>());
    default:
        break;
    }
    QRect windowGlobalRect(uiGlobalRect.x() - widgetSizeDict["mainWidget x"].value<int>(),
                           uiGlobalRect.y() - widgetSizeDict["mainWidget y"].value<int>(),
                           uiGlobalRect.width() + 2 * widgetSizeDict["mainWidget x"].value<int>(),
                           uiGlobalRect.height() + 2 * widgetSizeDict["mainWidget y"].value<int>());
    setGeometry(windowGlobalRect);
}

void MainWindow::UiDrag(QPoint globalPos)
{
    move(pressPosDistanceUiGlobalTL + globalPos);
}

// void MainWindow::mousePressEvent(QMouseEvent *event)
// {
//             if (event->button() == Qt::LeftButton) {
//             QPoint pos = event->pos();

//             if (pos.y() < titleWidget->height()) {
//                 int btnRight = width() - 10;
//                 int btnLeft = btnRight - titleWidget->getCloseButtonSize().width()
//                         - titleWidget->getMaxButtonSize().width()
//                         - titleWidget->getMinButtonSize().width() - 40;

//                 if (pos.x() < btnLeft) {
//     #ifdef Q_OS_WIN
//                     HWND hwnd = reinterpret_cast<HWND>(winId());
//                     if (hwnd) {
//                         ReleaseCapture();
//                         SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
//                     }
//     #endif
//                     return;
//                 }
//             }
//         }
//         QMainWindow::mousePressEvent(event);
// }

// void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
// {
//     if (event->button() == Qt::LeftButton) {
//         QPoint pos = event->pos();
//         if (pos.y() < titleWidget->height()) {
//             int btnRight = width() - 10;
//             int btnLeft = btnRight - titleWidget->getCloseButtonSize().width()
//                     - titleWidget->getMaxButtonSize().width()
//                     - titleWidget->getMinButtonSize().width() - 40;
//             if (pos.x() < btnLeft) {
//                 uiMaximize();
//                 return;
//             }
//         }
//     }
//     QMainWindow::mouseDoubleClickEvent(event);
// }

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    // qDebug() << "mouseReleaseEvent isScreenMax" << isScreenMax;
    qDebug() << "mouseReleaseEvent MainWindow size:" << size();
    if (event->button() == Qt::LeftButton) {
        mouseLeftButtonIsPress = false;
        // if (isScreenHalf) {
        //     QRect screenGeometry = screen()->availableGeometry();
        //     qDebug() << "mouseReleaseEvent screenGeometry:" << screenGeometry;
        //     QRect screenHalfRect = QRect(screenGeometry.x(), screenGeometry.y(),
        //                                  screenGeometry.width() / 2, screenGeometry.height());
        //     qDebug() << "mouseReleaseEvent screenHalfRect:" << screenHalfRect;
        //     if ((geometry().topLeft() != screenHalfRect.topLeft())
        //         || (geometry().width() != screenHalfRect.width())
        //         || (geometry().height() != screenHalfRect.height())) {
        //         if (!isScreenMax)
        //             isScreenHalf = false;
        //     }
        // }
        if (pushButtonIsPress)
            pushButtonIsPress = false;
        else {
            QRect chatInputRect;
            if (settingWidgetIsOpen)
                chatInputRect =
                        QRect(chatInput->geometry().x() + settingWidget->width(),
                              chatInput->geometry().y() + titleWidget->height() + chatFun->height()
                                      + chatShowWidget->height(),
                              chatInput->geometry().width(), chatInput->geometry().height());
            else if (chatRecordsWidgetIsOpen)
                chatInputRect =
                        QRect(chatInput->geometry().x() + chatRecordsWidget->width(),
                              chatInput->geometry().y() + titleWidget->height() + chatFun->height()
                                      + chatShowWidget->height(),
                              chatInput->geometry().width(), chatInput->geometry().height());
            else
                chatInputRect =
                        QRect(chatInput->geometry().x(),
                              chatInput->geometry().y() + titleWidget->height() + chatFun->height()
                                      + chatShowWidget->height(),
                              chatInput->geometry().width(), chatInput->geometry().height());
            if (chatInputRect.contains(event->pos()))
                chatInput->backgroundColorShowLight();
            else {
                chatInput->backgroundColorShowDark();
                chatInput->clearFocus();
            }
        }
        if (screenChanged) {
            qDebug() << "mouseReleaseEvent screenChanged";
            curDpi = curScreen->logicalDotsPerInch();
            windowFontPixelSize = std::ceil(windowFontPointSize * (curDpi / 72));
            titleFontPixelSize = std::ceil(titleFontPointSize * (curDpi / 72));
            screenChanged = false;
        }
    }
    QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::onDpiChanged()
{
    qDebug() << "onDpiChanged";
    lastDpi = curDpi;
    curDpi = curScreen->logicalDotsPerInch();
    windowFontPixelSize = std::ceil(windowFontPointSize * (curDpi / 72));
    isDpiChanged = true;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if (avoidRepeatSelfFun) {
        avoidRepeatSelfFun = false;
        return;
    }
    if (isDpiChanged) {
        setMinimumSize(qRound(widgetSizeDict["MainWindow minimumSize"].value<QSize>().width()
                              * curDpi / lastDpi),
                       qRound(widgetSizeDict["MainWindow minimumSize"].value<QSize>().height()
                              * curDpi / lastDpi));
        widgetSizeDict["MainWindow minimumSize"] = minimumSize();
    }
    // qDebug() << "resizeEvent isScreenMax" << isScreenMax;
    if (isScreenMax)
        mainWidget->setGeometry(0, 0, width(), height());
    else {
        if (isDpiChanged) {
            mainWidget->setGeometry(
                    qRound(widgetSizeDict["mainWidget x"].value<int>() * curDpi / lastDpi),
                    qRound(widgetSizeDict["mainWidget y"].value<int>() * curDpi / lastDpi),
                    qRound(widgetSizeDict["mainWidget"].value<QSize>().width() * curDpi / lastDpi),
                    qRound(widgetSizeDict["mainWidget"].value<QSize>().height() * curDpi
                           / lastDpi));
            widgetSizeDict["mainWidget"] = mainWidget->size();
            widgetSizeDict["mainWidget x"] = mainWidget->x();
            widgetSizeDict["mainWidget y"] = mainWidget->y();
        } else {
            qDebug() << "mainWidget geometry" << mainWidget->geometry();
            mainWidget->setGeometry(widgetSizeDict["mainWidget x"].value<int>(),
                                    widgetSizeDict["mainWidget y"].value<int>(),
                                    width() - 2 * widgetSizeDict["mainWidget x"].value<int>(),
                                    height() - 2 * widgetSizeDict["mainWidget y"].value<int>());
            qDebug() << "mainWidget x y" << widgetSizeDict["mainWidget x"].value<int>()
                     << widgetSizeDict["mainWidget y"].value<int>();
            qDebug() << "mainWidget geometry" << mainWidget->geometry();
            widgetSizeDict["mainWidget"] = mainWidget->size();
        }
    }
    if (isDpiChanged) {
        emptyTextLabel->updateSize(curDpi, lastDpi);
        textCopyLabel->updateSize(curDpi, lastDpi);
        settingWidget->updateSize(curDpi, lastDpi);
        titleWidget->updateSize(curDpi, lastDpi);
        chatRecordsWidget->updateSize(curDpi, lastDpi);
        chatFun->updateSize(curDpi, lastDpi);
        chatInput->updateSendButtonSize(curDpi, lastDpi);
    }
    settingWidget->resize(mainWidget->width() / 3, mainWidget->height() - titleWidget->height());
    chatRecordsWidget->resize(mainWidget->width() / 3,
                              mainWidget->height() - titleWidget->height());
    chatRecordsWidget->resetWidgetSize();
    if (settingWidgetIsOpen || chatRecordsWidgetIsOpen) {
        chatFun->setFixedSize(mainWidget->width() * 2 / 3, chatFun->height());
        chatFun->resetWidgetSize();
        chatShow->resize(mainWidget->width() * 2 / 3 - 29, chatShow->height());
        chatShowWidget->resize(mainWidget->width() * 2 / 3, chatShowWidget->height());
        chatInput->resize(mainWidget->width() * 2 / 3 - 40, chatInput->height());
        chatInput->resetWidgetSize();
        chatInputWidget->resize(mainWidget->width() * 2 / 3, chatInputWidget->height());
        splitter->resize(mainWidget->width() * 2 / 3, splitter->height());
        contentVLayout->setContentsMargins(mainWidget->width() / 3, 0, 0, 0);
        if (settingWidgetIsOpen)
            settingWidget->move(0, titleWidget->height());
        else
            settingWidget->move(-settingWidget->width(), titleWidget->height());
        if (chatRecordsWidgetIsOpen)
            chatRecordsWidget->move(0, titleWidget->height());
        else
            chatRecordsWidget->move(-chatRecordsWidget->width(), titleWidget->height());
    } else {
        chatFun->setFixedSize(mainWidget->width(), chatFun->height());
        chatFun->resetWidgetSize();
        // qDebug() << "mainWindow size:" << size();
        chatInput->resetWidgetSize();
        settingWidget->move(-settingWidget->width(), titleWidget->height());
        chatRecordsWidget->move(-chatRecordsWidget->width(), titleWidget->height());
    }
    if (isDpiChanged)
        settingWidget->updateLayoutMarginsSpacing(curDpi, lastDpi);
    settingWidget->resetWidgetSize();
    titleWidget->saveWidgetSize();
    emptyTextLabel->move((width() - emptyTextLabel->width()) / 2,
                         titleWidget->height() + chatFun->height() + chatShowWidget->height() + 10);
    textCopyLabel->move((width() - textCopyLabel->width()) / 2,
                        titleWidget->height() + chatFun->height() + chatShowWidget->height() + 10);
    if (isDpiChanged) {
        isDpiChanged = false;
        if ((qRound(widgetSizeDict["MainWindow"].value<QSize>().width() * curDpi / lastDpi)
             != width())
            || (qRound(widgetSizeDict["MainWindow"].value<QSize>().height() * curDpi / lastDpi)
                != height())) {
            avoidRepeatSelfFun = true;
            resize(qRound(widgetSizeDict["MainWindow"].value<QSize>().width() * curDpi / lastDpi),
                   qRound(widgetSizeDict["MainWindow"].value<QSize>().height() * curDpi / lastDpi));
        }
        messageWidgetRegenerate();
    }
    widgetSizeDict["MainWindow"] = size();
    qDebug() << "resizeEvent MainWindow size:" << size();
    qDebug() << "resizeEvent lastNormalGeometry:" << lastNormalGeometry;
}

bool MainWindow::isWindowMaximized() const
{
    return windowState() & Qt::WindowMaximized;
}

void MainWindow::uiMaximize()
{
    qDebug() << "MainWindow::uiMaximize";
    if (isWindowMaximized()) {
        showNormal();
        // titleWidget->maxButtonToggleIcon(true);
    } else {
        showMaximized();
        // titleWidget->maxButtonToggleIcon(false);
    }
}

void MainWindow::uiMinimize()
{
    showMinimized();
}

void MainWindow::uiClose()
{
    saveCurChatRecord(true);
    close();
}

void MainWindow::settingButtonClicked()
{
    settingWidget->raise();
    settingAnimationMove->setStartValue(settingWidget->geometry());
    settingAnimationMove->setEndValue(
            QRect(0, titleWidget->height(), settingWidget->width(), settingWidget->height()));
    settingAnimationMove->start();
    settingWidgetIsOpen = true;
    pushButtonIsPress = true;
}

void MainWindow::chatRecordsUiAnimationMove(const QVariant &value)
{
    QRect rect = value.toRect();
    chatFun->setFixedSize(mainWidget->width() - rect.x() - chatRecordsWidget->width(),
                          chatFun->height());
    chatFun->setSize();
    chatShow->resize(mainWidget->width() - rect.x() - chatRecordsWidget->width() - 29,
                     chatShow->height());
    chatShowWidget->resize(mainWidget->width() - rect.x() - chatRecordsWidget->width(),
                           chatShowWidget->height());
    chatInput->resize(mainWidget->width() - rect.x() - chatRecordsWidget->width() - 40,
                      chatInput->height());
    chatInput->resetWidgetSize();
    chatInputWidget->resize(mainWidget->width() - rect.x() - chatRecordsWidget->width(),
                            chatInputWidget->height());
    splitter->resize(mainWidget->width() - rect.x() - chatRecordsWidget->width(),
                     splitter->height());
    contentVLayout->setContentsMargins(rect.x() + chatRecordsWidget->width(), 0, 0, 0);
}

void MainWindow::chatRecordsUiMoveFinished()
{
    chatFun->saveWidgetSize();
    if (!chatRecordsWidgetIsOpen)
        chatRecordsWidget->delAllListItems();
    messageWidgetRegenerate();
}

void MainWindow::onBaseUrlTextChanged(const QString &text)
{
    try {
        QFile file(configFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("无法打开文件");
        }

        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();

        if (lines.isEmpty()) {
            lines.append("");
        }
        lines[0] = text;

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            throw std::runtime_error("无法写入文件");
        }

        QTextStream out(&file);
        for (int i = 0; i < lines.size(); ++i) {
            out << lines[i];
            if (i < lines.size() - 1) {
                out << "\n";
            }
        }
        file.close();

    } catch (const std::exception &e) {
        qDebug() << "错误：" << e.what();
    } catch (...) {
        qDebug() << "发生未知错误";
    }
}

void MainWindow::onApiKeyTextChanged(const QString &text)
{
    try {
        QFile file(configFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("无法打开文件");
        }

        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();

        while (lines.size() < 2) {
            lines.append("");
        }
        lines[1] = text;

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            throw std::runtime_error("无法写入文件");
        }

        QTextStream out(&file);
        for (int i = 0; i < lines.size(); ++i) {
            out << lines[i];
            if (i < lines.size() - 1) {
                out << "\n";
            }
        }
        file.close();

    } catch (const std::exception &e) {
        qDebug() << "错误：" << e.what();
    } catch (...) {
        qDebug() << "发生未知错误";
    }
}

void MainWindow::onModelNameTextChanged(const QString &text)
{
    try {
        QFile file(configFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("无法打开文件");
        }

        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();

        while (lines.size() < 3) {
            lines.append("");
        }
        lines[2] = text;

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            throw std::runtime_error("无法写入文件");
        }

        QTextStream out(&file);
        for (int i = 0; i < lines.size(); ++i) {
            out << lines[i];
            if (i < lines.size() - 1) {
                out << "\n";
            }
        }
        file.close();

    } catch (const std::exception &e) {
        qDebug() << "错误：" << e.what();
    } catch (...) {
        qDebug() << "发生未知错误";
    }
}

void MainWindow::onMaxTokensBoxValueChanged(int i)
{
    try {
        QFile file(configFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("无法打开文件");
        }

        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();

        while (lines.size() < 4) {
            lines.append("");
        }
        lines[3] = QString::number(i);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            throw std::runtime_error("无法写入文件");
        }

        QTextStream out(&file);
        for (int i = 0; i < lines.size(); ++i) {
            out << lines[i];
            if (i < lines.size() - 1) {
                out << "\n";
            }
        }
        file.close();

    } catch (const std::exception &e) {
        qDebug() << "错误：" << e.what();
    } catch (...) {
        qDebug() << "发生未知错误";
    }

    settingWidget->maxTokensSliderSetValue(i);
}

void MainWindow::onTopPBoxValueChanged(double d)
{
    try {
        QFile file(configFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("无法打开文件");
        }

        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();

        while (lines.size() < 5) {
            lines.append("");
        }
        lines[4] = QString::number(d);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            throw std::runtime_error("无法写入文件");
        }

        QTextStream out(&file);
        for (int i = 0; i < lines.size(); ++i) {
            out << lines[i];
            if (i < lines.size() - 1) {
                out << "\n";
            }
        }
        file.close();

    } catch (const std::exception &e) {
        qDebug() << "错误：" << e.what();
    } catch (...) {
        qDebug() << "发生未知错误";
    }

    settingWidget->topPSliderSetValue(static_cast<int>(d * 100));
}

void MainWindow::onTemperatureBoxValueChanged(double d)
{
    try {
        QFile file(configFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("无法打开文件");
        }

        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();

        while (lines.size() < 6) {
            lines.append("");
        }
        lines[5] = QString::number(d);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            throw std::runtime_error("无法写入文件");
        }

        QTextStream out(&file);
        for (int i = 0; i < lines.size(); ++i) {
            out << lines[i];
            if (i < lines.size() - 1) {
                out << "\n";
            }
        }
        file.close();

    } catch (const std::exception &e) {
        qDebug() << "错误：" << e.what();
    } catch (...) {
        qDebug() << "发生未知错误";
    }

    settingWidget->temperatureSliderSetValue(static_cast<int>((d - 0.01) * 100));
}

void MainWindow::onMaxTokensSliderValueChanged(int i)
{
    try {
        QFile file(configFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("无法打开文件");
        }

        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();

        while (lines.size() < 4) {
            lines.append("");
        }
        lines[3] = QString::number(i);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            throw std::runtime_error("无法写入文件");
        }

        QTextStream out(&file);
        for (int i = 0; i < lines.size(); ++i) {
            out << lines[i];
            if (i < lines.size() - 1) {
                out << "\n";
            }
        }
        file.close();

    } catch (const std::exception &e) {
        qDebug() << "错误：" << e.what();
    } catch (...) {
        qDebug() << "发生未知错误";
    }

    settingWidget->maxTokensBoxSetValue(i);
}

void MainWindow::onTopPSliderValueChanged(int i)
{
    try {
        QFile file(configFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("无法打开文件");
        }

        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();

        while (lines.size() < 5) {
            lines.append("");
        }
        lines[4] = QString::number(i / 100.0);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            throw std::runtime_error("无法写入文件");
        }

        QTextStream out(&file);
        for (int i = 0; i < lines.size(); ++i) {
            out << lines[i];
            if (i < lines.size() - 1) {
                out << "\n";
            }
        }
        file.close();

    } catch (const std::exception &e) {
        qDebug() << "错误：" << e.what();
    } catch (...) {
        qDebug() << "发生未知错误";
    }

    settingWidget->topPBoxSetValue(i / 100.0);
}

void MainWindow::onTemperatureSliderValueChanged(int i)
{
    try {
        QFile file(configFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("无法打开文件");
        }

        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();

        while (lines.size() < 6) {
            lines.append("");
        }
        lines[5] = QString::number(i / 100.0 + 0.01);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            throw std::runtime_error("无法写入文件");
        }

        QTextStream out(&file);
        for (int i = 0; i < lines.size(); ++i) {
            out << lines[i];
            if (i < lines.size() - 1) {
                out << "\n";
            }
        }
        file.close();

    } catch (const std::exception &e) {
        qDebug() << "错误：" << e.what();
    } catch (...) {
        qDebug() << "发生未知错误";
    }

    settingWidget->temperatureBoxSetValue(i / 100.0 + 0.01);
}

void MainWindow::messageWidgetResize(MessageWidget *selfMessageWidget)
{
    qDebug() << "messageWidgetResize start";
    const int count = qMin(chatShow->count(), messageWidgetList.size());
    int i = 0;
    for (; i < count; ++i) {
        if (selfMessageWidget == messageWidgetList.at(i))
            break;
    }
    for (; i < count; ++i) {
        MessageWidget *messageWidget = messageWidgetList.at(i);
        // messageWidget->setSize();
        QListWidgetItem *item = chatShow->item(i);
        QWidget *itemWidget = chatShow->itemWidget(item);
        if (!itemWidget)
            return;
        const int w = chatShow->width();
        const int h = messageWidget->height() + 10;
        itemWidget->setFixedSize(w, h);
        QLayout *itemLayout = itemWidget->layout();
        if (itemLayout) {
            if (messageWidget->getIsUser())
                itemLayout->setContentsMargins(itemWidget->width() - messageWidget->width() - 25, 5,
                                               25, 5);
            else
                itemLayout->setContentsMargins(0, 5, itemWidget->width() - messageWidget->width(),
                                               5);
        }
        item->setSizeHint(QSize(w, h));
    }
    qDebug() << "messageWidgetResize end";
}

void MainWindow::sendMessage()
{
    if (!isSending) {
        QList<QVariantMap> context = {};
        QString text = chatInput->toPlainText().trimmed();
        if (!text.isEmpty()) {
            for (auto *w : messageWidgetList) {
                QVariantMap m;
                m["role"] = w->getIsUser() ? "user" : "assistant";
                m["content"] = w->getText();
                context.append(m);
            }
            thinkTimeLengthList.append(0);
            messageSendWidget = new MessageWidget(
                    text, [this]() { textCopy(); }, [this]() { messageRenewResponse(); },
                    [this](MessageWidget *selfMessageWidget) {
                        messageWidgetResize(selfMessageWidget);
                    },
                    [this](bool state) { getSetTexting(state); }, [this]() { onExecuteNext(); },
                    chatShow, thinkTimeLengthList, messageWidgetList.size(), true, true,
                    chatShow->width() * 3 / 4);
            messageSendWidget->hide();
            messageSendWidget->updateFunWidgetSize(curDpi, initDpi);
            // messageSendWidget->connectResizeFinished(this, &MainWindow::messageWidgetResize);
            // messageSendWidget->connectSetTexting(this, &MainWindow::getSetTexting);
            // messageSendWidget->connectExecuteNext(this, &MainWindow::onExecuteNext);
            // messageSendWidget->toggleWidget();
            messageWidgetList.append(messageSendWidget);

            itemSendWidget = new ItemWidget(this);
            itemSendHLayout = new QHBoxLayout(itemSendWidget);
            itemSendHLayout->addWidget(messageSendWidget);
            itemSendWidget->setFixedSize(chatShow->width(), messageSendWidget->height() + 10);
            itemSendHLayout->setContentsMargins(
                    itemSendWidget->width() - messageSendWidget->width() - 25, 5, 25, 5);

            sendItem = new QListWidgetItem(chatShow);
            sendItem->setSizeHint(QSize(chatShow->width(), messageSendWidget->height() + 10));
            chatShow->setItemWidget(sendItem, itemSendWidget);

            thread = new MessageThread(text, context);
            chatInput->clearText();
            chatInput->setSending(true);
            isSending = true;
            qDebug() << "sendMessage:" << text;
        } else {
            //         emptyTextLabel->printStart();
        }
    } else {
        if (thread)
            thread->stop();
        isSending = false;
        if (messageRecvWidget)
            messageRecvWidget->breakHandle();
    }
}

void MainWindow::onExecuteNext()
{
    qDebug() << "onExecuteNext";
    messageSendWidget->show();
    QTimer::singleShot(50, this, &MainWindow::startThread);
}

void MainWindow::startThread()
{
    connect(thread, &QThread::started, this, &MainWindow::messageStart);
    connect(thread, &MessageThread::newMessage, this, &MainWindow::queueMessage);
    connect(thread, &QThread::finished, this, &MainWindow::messageFinish);
    thread->start();
    qDebug() << "startThread";
}

void MainWindow::messageStart()
{
    message.clear();
    int i = messageWidgetList.size() - 1;
    if (i != 0) {
        if (messageWidgetList[i]->getIsUser())
            messageWidgetList[i - 1]->removeRenewResponseButton();
        else
            messageWidgetList[i]->removeRenewResponseButton();
    }

    thinkTimeLengthList.append(0);
    messageRecvWidget = new MessageWidget(
            message, [this]() { textCopy(); }, [this]() { messageRenewResponse(); },
            [this](MessageWidget *selfMessageWidget) { messageWidgetResize(selfMessageWidget); },
            [this](bool state) { getSetTexting(state); }, nullptr, chatShow, thinkTimeLengthList,
            messageWidgetList.size(), false, true, chatShow->width() * 3 / 4);
    // messageRecvWidget->connectResizeFinished(this, &MainWindow::messageWidgetResize);
    // messageRecvWidget->connectSetTexting(this, &MainWindow::getSetTexting);
    messageWidgetList.append(messageRecvWidget);

    itemRecvWidget = new ItemWidget(this);
    itemRecvHLayout = new QHBoxLayout(itemRecvWidget);
    itemRecvHLayout->addWidget(messageRecvWidget);
    itemRecvWidget->setFixedSize(chatShow->width(), messageRecvWidget->height() + 10);
    itemRecvHLayout->setContentsMargins(0, 5, itemRecvWidget->width() - messageRecvWidget->width(),
                                        5);

    recvItem = new QListWidgetItem(chatShow);
    recvItem->setSizeHint(QSize(chatShow->width(), messageRecvWidget->height() + 10));
    chatShow->setItemWidget(recvItem, itemRecvWidget);

    first = true;
    qDebug() << "messageStart";

    HWND hwnd = reinterpret_cast<HWND>(winId());
    DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                 SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
}

void MainWindow::queueMessage(const QString &text)
{
    messageQueue.enqueue(text);
    if (!isProcessing) {
        isProcessing = true;
        recvMessage(text);
    }
}

void MainWindow::recvMessage(const QString &text)
{
    qDebug() << "recvMessage:" << text;
    // QSignalBlocker blocker(thread);
    // isProcessing = true;
    if (first) {
        first = false;
        if (text.startsWith("\n "))
            message = text.mid(2);
        else
            message = text;
    } else {
        message += text;
    }

    if (isContinueShow && messageRecvWidget) {
        messageRecvWidget->setText(message);
        itemRecvWidget->setFixedSize(chatShow->width(), messageRecvWidget->height() + 10);
        itemRecvHLayout->setContentsMargins(
                0, 5, itemRecvWidget->width() - messageRecvWidget->width(), 5);
        recvItem->setSizeHint(QSize(chatShow->width(), messageRecvWidget->height() + 10));
    }

    HWND hwnd = reinterpret_cast<HWND>(winId());
    DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                 SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

    qDebug() << "recvMessage: setText finish";
    messageQueue.dequeue();
    qDebug() << "recvMessage: messageQueue dequeue";
    if (!messageQueue.isEmpty()) {
        QString next = messageQueue.head();
        QTimer::singleShot(0, this, [this, next]() { recvMessage(next); });
    } else {
        isProcessing = false;
        qDebug() << "recvMessage: isProcessing false";
    }
}

void MainWindow::messageFinish()
{
    chatInput->setSending(false);
    if (messageRecvWidget) {
        messageRecvWidget->removeLoadingWidget();
        messageRecvWidget->updateFunWidgetSize(curDpi, initDpi);
        // messageRecvWidget->toggleWidget();
    }

    if (messageRecvWidget && itemRecvWidget && itemRecvHLayout && recvItem) {
        itemRecvWidget->setFixedSize(chatShow->width(), messageRecvWidget->height() + 10);
        itemRecvHLayout->setContentsMargins(
                0, 5, itemRecvWidget->width() - messageRecvWidget->width(), 5);
        recvItem->setSizeHint(QSize(chatShow->width(), messageRecvWidget->height() + 10));
    }

    if (message.isEmpty() && !messageWidgetList.isEmpty() && chatShow->count() > 0) {
        delete messageWidgetList.takeLast();
        int last = chatShow->count() - 1;
        QWidget *itemWidget = chatShow->itemWidget(chatShow->item(last));
        if (itemWidget)
            itemWidget->deleteLater();
        QListWidgetItem *lastItem = chatShow->takeItem(last);
        delete lastItem;
        messageRenewResponse();
    }
    isSending = false;

    qDebug() << "chatShow item count:" << chatShow->count();
    // const int count = chatShow->count();
    // for (int i = 0; i < count; ++i) {
    //     MessageWidget *messageWidget = messageWidgetList.at(i);
    //     qDebug() << i << "messageWidget size:" << messageWidget->size();
    // }
}

void MainWindow::textCopy() { }

void MainWindow::messageRenewResponse() { }

void MainWindow::writeToChatRecordFile(bool withholdCurChatFile)
{
    chatRecordFileName = "chat_";
    chatRecordFileName += QDateTime::currentDateTime().toString("yyyy_MM_dd_HH_mm_ss");
    chatRecordFileName += ".txt";

    QString filePath = QDir(chatRecordsDir).filePath(chatRecordFileName);
    qDebug() << "writeToChatRecordFile filePath:" << filePath;

    try {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
            throw std::runtime_error("无法打开文件进行写入");
        }
        qDebug() << "writeToChatRecordFile success open file:";

        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);

        for (int i = 0; i < messageWidgetList.size(); i++) {
            MessageWidget *messageWidget = messageWidgetList.at(i);
            if (!messageWidget)
                continue;
            QString chatRecordStr = messageWidget->getText() + '\n'
                    + QString("消息部件思考时长:%1秒\n")
                              .arg(i < thinkTimeLengthList.size() ? thinkTimeLengthList.at(i) : 0)
                    + (messageWidget->getIsUser() ? "True\n" : "False\n");
            out << chatRecordStr;
        }

        file.close();

    } catch (const std::exception &e) {
        qDebug() << "发生未知错误：" << e.what();
    }

    if (!withholdCurChatFile) {
        curChatFile = chatRecordFileName;
    }
}

void MainWindow::saveCurChatRecord(bool withholdCurChatFile)
{
    QString chatRecordStr;
    int chatStrCount = 0;

    if (messageWidgetList.size() != 0) {
        if (curChatFile.isEmpty()) {
            writeToChatRecordFile(withholdCurChatFile);
        } else {
            QString filePath = QDir(chatRecordsDir).filePath(curChatFile);
            if (!QFile::exists(filePath)) {
                writeToChatRecordFile(withholdCurChatFile);
            } else {
                try {
                    for (int i = 0; i < messageWidgetList.size(); i++) {
                        chatStrCount += messageWidgetList[i]->getText().count('\n') + 2;
                    }

                    QStringList lines;
                    QFile file(filePath);
                    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QTextStream in(&file);
                        in.setEncoding(QStringConverter::Utf8);
                        while (!in.atEnd()) {
                            lines.append(in.readLine() + '\n');
                        }
                        file.close();
                    }

                    if (chatStrCount > lines.size()) {
                        QFile writeFile(filePath);
                        if (writeFile.open(QIODevice::WriteOnly | QIODevice::Text
                                           | QIODevice::Truncate)) {
                            writeFile.resize(0);
                            writeFile.close();
                        }

                        QFile appendFile(filePath);
                        if (appendFile.open(QIODevice::WriteOnly | QIODevice::Text
                                            | QIODevice::Append)) {
                            QTextStream out(&appendFile);
                            out.setEncoding(QStringConverter::Utf8);
                            for (int i = 0; i < messageWidgetList.size(); i++) {
                                MessageWidget *messageWidget = messageWidgetList.at(i);
                                if (!messageWidget)
                                    continue;
                                chatRecordStr = messageWidget->getText() + '\n'
                                        + QString("消息部件思考时长:%1秒\n")
                                                  .arg(i < thinkTimeLengthList.size()
                                                               ? thinkTimeLengthList.at(i)
                                                               : 0)
                                        + (messageWidget->getIsUser() ? "True\n" : "False\n");
                                out << chatRecordStr;
                            }
                            appendFile.close();
                        }
                    }
                } catch (const std::exception &e) {
                    qDebug() << "发生未知错误：" << e.what();
                }
            }
        }
    }
}

void MainWindow::chatRecordsGenerateItem(QString searchText)
{
    Q_UNUSED(searchText);
}

void MainWindow::generateCurChatRecord(bool lastIsToggle, bool useThinkExpandList)
{
    QString text;
    bool isUser = true;
    thinkTimeLengthList.clear();
    int thinkTimeIndex = 0;
    int expandIndex = 0;

    QStringList lines;
    QString filePath = QDir(chatRecordsDir).filePath(curChatFile);
    try {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            in.setEncoding(QStringConverter::Utf8);
            while (!in.atEnd()) {
                lines.append(in.readLine() + '\n');
            }
            file.close();
        }
    } catch (const std::exception &e) {
        qDebug() << "发生未知错误：" << e.what();
    }

    for (int i = 0; i < lines.size(); i++) {
        if (lines[i] == "True\n" || lines[i] == "False\n") {
            if (lines[i] == "True\n") {
                isUser = true;
            } else {
                isUser = false;
            }

            int j = messageWidgetList.size() - 1;
            if (j != -1 && j != 0) {
                if (!messageWidgetList[j]->getIsUser()) {
                    messageWidgetList[j]->removeRenewResponseButton();
                }
            }
            if (text.endsWith('\n'))
                text.chop(1);

            qDebug() << "generateCurChatRecord" << text;
            if (useThinkExpandList) {
                if (!isUser) {
                    bool thinkExpand = true;
                    if (expandIndex < thinkExpandedList.size())
                        thinkExpand = thinkExpandedList[expandIndex];
                    messageWidget = new MessageWidget(
                            text, [this]() { textCopy(); }, [this]() { messageRenewResponse(); },
                            [this](MessageWidget *selfMessageWidget) {
                                messageWidgetResize(selfMessageWidget);
                            },
                            [this](bool state) { getSetTexting(state); }, nullptr, chatShow,
                            thinkTimeLengthList, thinkTimeIndex, isUser, thinkExpand,
                            chatShow->width() * 3 / 4);
                } else {
                    // 重建恢复历史消息，不传 executeNextFun，避免 TextShow 渲染完成后自动触发新线程
                    messageWidget = new MessageWidget(
                            text, [this]() { textCopy(); }, [this]() { messageRenewResponse(); },
                            [this](MessageWidget *selfMessageWidget) {
                                messageWidgetResize(selfMessageWidget);
                            },
                            [this](bool state) { getSetTexting(state); }, nullptr, chatShow,
                            thinkTimeLengthList, thinkTimeIndex, isUser, true,
                            chatShow->width() * 3 / 4);
                }
            } else {
                // 重建恢复历史消息，不传 executeNextFun，避免 TextShow 渲染完成后自动触发新线程
                messageWidget = new MessageWidget(
                        text, [this]() { textCopy(); }, [this]() { messageRenewResponse(); },
                        [this](MessageWidget *selfMessageWidget) {
                            messageWidgetResize(selfMessageWidget);
                        },
                        [this](bool state) { getSetTexting(state); }, nullptr, chatShow,
                        thinkTimeLengthList, thinkTimeIndex, isUser, true,
                        chatShow->width() * 3 / 4);
            }
            // messageWidget->connectResizeFinished(this, &MainWindow::messageWidgetResize);
            // messageWidget->connectSetTexting(this, &MainWindow::getSetTexting);

            qDebug() << "generateCurChatRecord" << i;
            if (!isUser) {
                if (i == lines.size() - 1) {
                    if (lastIsToggle) {
                        messageWidget->removeLoadingWidget();
                    }
                } else {
                    messageWidget->removeLoadingWidget();
                }
            }

            messageWidgetList.append(messageWidget);

            itemWidget = new ItemWidget(this);
            itemHLayout = new QHBoxLayout();
            itemHLayout->addWidget(messageWidget);
            itemWidget->setLayout(itemHLayout);
            itemWidget->setFixedSize(chatShow->width(), messageWidget->height() + 10);
            if (isUser) {
                itemHLayout->setContentsMargins(itemWidget->width() - messageWidget->width() - 25,
                                                5, 25, 5);
            } else {
                itemHLayout->setContentsMargins(0, 5, itemWidget->width() - messageWidget->width(),
                                                5);
            }
            item = new QListWidgetItem(chatShow);
            item->setSizeHint(QSize(chatShow->width(), messageWidget->height() + 10));
            chatShow->setItemWidget(item, itemWidget);

            if (i == lines.size() - 1) {
                if (lastIsToggle) {
                    messageWidget->updateFunWidgetSize(curDpi, initDpi);
                    // messageWidget->toggleWidget();
                }
            } else {
                messageWidget->updateFunWidgetSize(curDpi, initDpi);
                // messageWidget->toggleWidget();
            }

            text.clear();

            thinkTimeIndex++;
            if (useThinkExpandList && !isUser) {
                expandIndex++;
            }

        } else if (lines[i].left(8) == QString("消息部件思考时长")) {
            QRegularExpression re("\\d+");
            QRegularExpressionMatch match = re.match(lines[i]);
            if (match.hasMatch()) {
                thinkTimeLengthList.append(match.captured(0).toInt());
            }
        } else {
            text += lines[i];
        }
    }
}

void MainWindow::getSetTexting(bool state)
{
    isSetTexting = state;
    // 渲染结束后若有待执行的重建，延迟到事件循环空闲时执行，避免在 setText 栈内重建
    if (!state && isRegeneratePending && !isRegenerating) {
        QTimer::singleShot(0, this, &MainWindow::messageWidgetRegenerate);
    }
}

void MainWindow::messageWidgetRegenerate()
{
    // 重入保护：正在重建或 AI 消息渲染中（嵌套事件循环内 WM_EXITSIZEMOVE 会被再次分发）
    // 再次触发时仅标记待重建，避免半成品控件重复创建与悬空指针访问
    if (isRegenerating || isSending) {
        isRegeneratePending = true;
        return;
    }
    isRegenerating = true;
    do {
        isRegeneratePending = false;
        if (messageWidgetList.size() != 0) {
            thinkExpandedList.clear();

            if (isSending) {
                isContinueShow = false;
            }

            if (!isSending) {
                currentScrollValue = chatShow->verticalScrollBar()->value();
                maxScrollValue = chatShow->verticalScrollBar()->maximum();
            }

            saveCurChatRecord();
            for (int i = 0; i < messageWidgetList.size(); i++) {
                MessageWidget *messageWidget = messageWidgetList[i];
                if (!messageWidget->getIsUser()) {
                    thinkExpandedList.append(messageWidget->getThinkIsExpanded());
                }
            }

            // 重建期间置空接收指针，防止嵌套事件循环中消息回调访问旧控件
            messageRecvWidget = nullptr;
            messageWidgetList.clear();
            for (int i = 0; i < chatShow->count(); i++) {
                QWidget *itemWidget = chatShow->itemWidget(chatShow->item(i));
                if (itemWidget) {
                    itemWidget->deleteLater();
                }
            }
            chatShow->clear();

            if (!isSending) {
                generateCurChatRecord(true, true);
                QTimer::singleShot(5, this, &MainWindow::setScrollValue);
            } else {
                generateCurChatRecord(false, true);
                messageRecvWidget = messageWidget;
                itemRecvHLayout = itemHLayout;
                itemRecvWidget = itemWidget;
                recvItem = item;
                isContinueShow = true;
            }
        }
    } while (isRegeneratePending);
    isRegenerating = false;
}

void MainWindow::setScrollValue()
{
    int newMaxScrollValue = chatShow->verticalScrollBar()->maximum();
    if (maxScrollValue != 0) {
        int newValue =
                qRound(static_cast<qreal>(currentScrollValue) / maxScrollValue * newMaxScrollValue);
        chatShow->verticalScrollBar()->setValue(newValue);
    }
}

void MainWindow::showChatRecords() { }

void MainWindow::showSearchRecords()
{
    QString text = chatRecordsWidget->getLineEditText();
    chatRecordsWidget->delAllListItems();
    chatRecordsGenerateItem(text);
}

void MainWindow::clearAllChatRecords() { }

void MainWindow::generateChatRecord(QListWidgetItem *item)
{
    if (isSending) {
        thread->stop();
        isSending = false;
        if (messageRecvWidget)
            messageRecvWidget->breakHandle();
    }
    saveCurChatRecord();
    messageWidgetList.clear();
    for (int i = 0; i < chatShow->count(); ++i) {
        QWidget *itemWidget = chatShow->itemWidget(chatShow->item(i));
        itemWidget->deleteLater();
    }
    chatShow->clear();
    curChatFile = chatRecordsWidget->listItemToString(item);
    generateCurChatRecord();
}

void MainWindow::newChat() { }

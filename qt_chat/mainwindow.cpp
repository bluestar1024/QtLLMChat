#include "mainwindow.h"

#include <QQuickWindow>
#include <QDebug>

const QString imagesDir = ":/images";
const QString fontFilePath = ":/font/msyhl.ttc";
const QString mathjaxScriptPath = "mathjax/es5/tex-mml-chtml.js";
QString codeThemeFilePath = ":/config/dark_theme.xml";
const QString webEngineCacheDir = ":/webengine_cache";
QWebEngineProfile* sharedProfile = nullptr;
const int windowFontPointSize = 10;
const int windowFontPixelSize = 20;
const int titleFontPixelSize = 28;
const int buttonFontPointSize = 9;
const QColor textEditFullBGColor = QColor(224, 224, 224);
const QColor textEditFullBGTColor = QColor(224, 224, 224, 0);
const QColor textEditFullBTColor = QColor(100, 100, 100);
const QColor textEditFullBColor = QColor(100, 100, 100, 0);
const QString testText_1 = R"(<think>
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

const QString testText2 = R"(<think>
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

const QString testText1 = R"(
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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), message(""), isProcessing(false)
{
    QWebEngineProfile* sharedProfile = new QWebEngineProfile("shared");
    sharedProfile->setHttpCacheType(QWebEngineProfile::DiskHttpCache);
    sharedProfile->setCachePath(webEngineCacheDir);
    QWebEngineView *dummyView = new QWebEngineView(this);
    dummyView->setPage(new WebEnginePage(sharedProfile, dummyView));
    dummyView->load(QUrl("about:blank"));
    connect(dummyView, &QWebEngineView::loadFinished, dummyView, &QObject::deleteLater);
    connect(dummyView, &QWebEngineView::loadFinished, []() {
        qDebug() << "dummyView delete";
    });

    setMinimumSize(1110, 795);
    resize(1220, 820);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
    setMouseTracking(true);

    mouseLeftButtonIsPress = false;
    regionDir = RegionEnum::Middle;
    padding = 2;
    // titleWidgetInit();
    chatFun = new FunWidget();
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
    mainWidget = new Frame(this);
    mainWidget->setGeometry(10, 10, width() - 20, height() - 20);
    mainWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    mainWidget->setObjectName("mainWidget");
    mainWidget->setStyleSheet("#mainWidget {"
                              "    border-radius: 16px;"
                              "    background-color: #F0F0F0;"
                              "}");
    mainVLayout = new QVBoxLayout(mainWidget);
    // mainVLayout->addWidget(titleWidget);
    mainVLayout->addWidget(contentWidget);
    mainVLayout->setContentsMargins(0, 0, 0, 0);
    mainVLayout->setSpacing(0);
    mainVLayout->setStretch(0, 0);
    mainVLayout->setStretch(1, 1);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor(0, 0, 0, 150));
    setGraphicsEffect(shadow);
    messageWidgetList.clear();

    // settingWidgetInit();
    // chatRecordsWidget = ChatRecordsWidget(mainWidget);
    // chatRecordsWidget.connectSettingButtonClick(settingButtonClicked);
    // chatRecordsWidget.connectLineEditTextChanged(showSearchRecords);
    // chatRecordsWidget.connectSearchButtonClick(showSearchRecords);
    // chatRecordsWidget.connectClearAllButtonClick(clearAllChatRecords);
    // chatRecordsWidget.connectListItemClick(generateChatRecord);
    // chatRecordsWidget.move(-self.chatRecordsWidget.width(), self.titleWidget.height());
    // curChatFile = '';

    // chatRecordsAnimationMove = new QPropertyAnimation(chatRecordsWidget, "geometry");
    // chatRecordsAnimationMove->setDuration(1000);
    // chatRecordsAnimationMove->setEasingCurve(QEasingCurve::OutQuad);
    // connect(chatRecordsAnimationMove, &QPropertyAnimation::valueChanged, this, &MainWindow::chatRecordsUiAnimationMove);
    // connect(chatRecordsAnimationMove, &QPropertyAnimation::finished, this, &MainWindow::chatRecordsUiMoveFinished);

    // chatRecordsWidgetIsOpen = false;
    // emptyTextLabel = new PrintLabel("文本不能为空", this);
    // emptyTextLabel->move((width() - emptyTextLabel->width()) / 2,
    //                      titleWidget->height() + chatFun->height() + chatShowWidget->height() + 10);
    // emptyTextLabel->raise();
    // emptyTextLabel->hide();
    // textCopyLabel = new PrintLabel("文本复制成功", this);
    // textCopyLabel->move((width() - textCopyLabel->width()) / 2,
    //                     titleWidget->height() + chatFun->height() + chatShowWidget->height() + 10);
    // textCopyLabel->raise();
    // textCopyLabel->hide();
    isRegenerate = false;
    isRegenerateFirst = true;
    isSetTexting = false;
    pushButtonIsPress = false;
    lastScreen = curScreen = screen();
    initDpi = lastDpi = curDpi = curScreen->logicalDotsPerInch();
    screenChanged = false;
    thinkExpandedList.clear();
    thinkTimeLengthList.clear();
    isSending = false;
    isContinueShow = true;
    isScreenMax = false;
    isScreenHalf = false;
    lastNormalGeometry = geometry();
    uiRectWidth = width();
    uiRectHeight = height();
    isChangeRectFirst = false;
    screens = QApplication::screens();
    for (QScreen *screen : screens) {
        connect(screen, &QScreen::logicalDotsPerInchChanged, this, &MainWindow::onDpiChanged);
    }
    isDpiChanged = false;
    widgetSizeDict["MainWindow"] = size();
    widgetSizeDict["MainWindow minimumSize"] = minimumSize();
    widgetSizeDict["mainWidget"] = mainWidget->size();
    widgetSizeDict["mainWidget x"] = mainWidget->x();
    widgetSizeDict["mainWidget y"] = mainWidget->y();
    avoidRepeatSelfFun = false;
    // messageSendWidgetIsFinished = false;

    checkGraphicsBackend();

    // dummyView->deleteLater();
}

MainWindow::~MainWindow() { }

void MainWindow::checkGraphicsBackend()
{
    // 创建一个成员变量或使用静态变量
    window = new QQuickWindow();
    window->show();
    QTimer::singleShot(500, [&]() {
        qDebug() << "===== 图形后端信息 =====";
        qDebug() << "当前渲染器:" << window->rendererInterface()->graphicsApi();
        qDebug() << "场景图后端:" << window->sceneGraphBackend();
        qDebug() << "======================";
        window->close();
        window->deleteLater();
    });
}

void MainWindow::onDpiChanged() { }

void MainWindow::messageWidgetResize()
{
    const int count = chatShow->count();
    for (int i = 0; i < count; ++i) {
        MessageWidget *messageWidget = messageWidgetList.at(i);
        messageWidget->setSize();
        QListWidgetItem *item = chatShow->item(i);
        QWidget *itemWidget = chatShow->itemWidget(item);
        if (!itemWidget)
            continue;
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
                    text, [this]() { textCopy(); }, [this]() { messageRenewResponse(); }, chatShow,
                    thinkTimeLengthList, messageWidgetList.size(), true, true,
                    chatShow->width() * 3 / 4);
            messageSendWidget->hide();
            messageSendWidget->updateFunWidgetSize(curDpi, initDpi);
            messageSendWidget->connectResizeFinished(this, &MainWindow::messageWidgetResize);
            messageSendWidget->connectSetTexting(this, &MainWindow::getSetTexting);
            messageSendWidget->connectExecuteNext(this, &MainWindow::onExecuteNext);
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
    // if (!messageSendWidgetIsFinished) {
    //     QTimer::singleShot(50, this, &MainWindow::startThread);
    //     return;
    // }
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
            message, [this]() { textCopy(); }, [this]() { messageRenewResponse(); }, chatShow,
            thinkTimeLengthList, messageWidgetList.size(), false, true, chatShow->width() * 3 / 4);
    messageRecvWidget->connectResizeFinished(this, &MainWindow::messageWidgetResize);
    messageRecvWidget->connectSetTexting(this, &MainWindow::getSetTexting);
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

    if (isContinueShow) {
        messageRecvWidget->setText(message);
        itemRecvWidget->setFixedSize(chatShow->width(), messageRecvWidget->height() + 10);
        itemRecvHLayout->setContentsMargins(
                0, 5, itemRecvWidget->width() - messageRecvWidget->width(), 5);
        recvItem->setSizeHint(QSize(chatShow->width(), messageRecvWidget->height() + 10));
    }
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
    messageRecvWidget->removeLoadingWidget();
    messageRecvWidget->updateFunWidgetSize(curDpi, initDpi);
    // messageRecvWidget->toggleWidget();

    itemRecvWidget->setFixedSize(chatShow->width(), messageRecvWidget->height() + 10);
    itemRecvHLayout->setContentsMargins(0, 5, itemRecvWidget->width() - messageRecvWidget->width(),
                                        5);
    recvItem->setSizeHint(QSize(chatShow->width(), messageRecvWidget->height() + 10));

    if (message.isEmpty()) {
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
}

void MainWindow::textCopy() { }

void MainWindow::messageRenewResponse() { }

void MainWindow::getSetTexting(bool state)
{
    isSetTexting = state;
}

void MainWindow::showChatRecords() { }

void MainWindow::newChat() { }

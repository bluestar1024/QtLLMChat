#include "textshow.h"

#include <QPointer>

TextShow::TextShow(AppContext *appContext, const QString &text, std::function<void()> sizeFinishFun,
                   std::function<void()> executeNextFun, int maxWidth, QWidget *parent)
    : ThinkWidget(appContext, text, sizeFinishFun, maxWidth, parent),
      executeNextFun(executeNextFun),
      firstExecuteNextEmit(true)
{
}

TextShow::~TextShow() { }

void TextShow::onUpdateSize()
{
    const char *js = R"(
function getPageSize(){
  var body=document.body, html=document.documentElement;
  var w=Math.max(html.clientWidth,html.scrollWidth,html.offsetWidth);
  var h=Math.max(html.clientHeight,html.scrollHeight,html.offsetHeight);
  var content=document.querySelector('.content');
  if(content){ w=content.offsetWidth; h=content.offsetHeight; }
  return [w,h];
}
getPageSize();
)";
    static int textShowFuni = 0;
    // 用 QPointer 持有自身，防止控件在重建中被销毁后异步回调访问悬空指针
    QPointer<TextShow> self = this;
    webEngineView->page()->runJavaScript(js, [self](const QVariant &res) {
        if (!self)
            return;
        if (res.isNull()) {
            if (self->waitForValidSize())
                return;
            // 加载完成后 JS 仍无返回（页面异常）：放弃尺寸等待，标记完成，
            // 避免 MessageWidget 的等待循环死锁；控件保持当前初始宽度
            if (self->isSetTextEnd) {
                self->isSetTextEnd = false;
                self->isSizeFinish = true;
            }
            return;
        }
        QList<QVariant> list = res.toList();
        if (list.size() != 2)
            return;
        int w = list[0].toInt();
        int h = list[1].toInt();
        qDebug() << "WebEngineView get size:" << w << h << self.data();
        // 量不到正尺寸时由基类统一判定等待/收敛：空内容块不能无限重试，
        // 否则 MessageWidget::setText 的嵌套等待循环永远等不到 isSizeFinish
        if (w <= 0 || h <= 0) {
            if (self->waitForValidSize())
                return;
        }
        // JS 量测的 .content 宽度（代码长行自然宽、MathJax 展开宽等）可能超过构造时
        // 传入的最大宽度（拖窄窗口重建后尤其明显）：不夹取会把 TextShow 固定到
        // 量测宽，使气泡宽度超过 MessageWidget 的最大宽度（14.txt 中 762 > 679）
        w = qMin(w, self->getMaxWidth());
        if (self->webEngineSize == QSize(w, h)) {
            if (self->isSetTextEnd) {
                self->isSetTextEnd = false;
                self->isSizeFinish = true;
            }
            return;
        }
        self->webEngineSize = QSize(w, h);
        textShowFuni += 1;
        qDebug() << "textShowFuni:" << textShowFuni << self.data();
        self->webEngineView->setFixedSize(w, h);
        // mainHLayout->addWidget(webEngineView);
        // if (webEngineView->isHidden()) {
        //     qDebug() << "isHidden:" << webEngineView->isHidden();
        //     webEngineView->show();
        // }
        self->setFixedSize(w + 10, h);
        emit self->setSizeFinished();
        // isEmitSizeFinish = true;
        if (self->isSetTextEnd) {
            self->isSetTextEnd = false;
            self->isSizeFinish = true;
        }
        if (self->firstExecuteNextEmit) {
            self->firstExecuteNextEmit = false;
            if (self->executeNextFun) {
                connect(self.data(), &TextShow::executeNext, self->executeNextFun);
                QTimer::singleShot(0, self.data(), [self]() { emit self->executeNext(); });
                qDebug() << "textShow executeNext emit" << self.data();
            }
        }
        qDebug() << "textShow onUpdateSize end" << self.data();
    });
}

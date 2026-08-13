#include "textshow.h"

#include <QPointer>

TextShow::TextShow(const QString &text, std::function<void()> sizeFinishFun, std::function<void()> executeNextFun, int maxWidth, QWidget *parent)
    : ThinkWidget(text, sizeFinishFun, maxWidth, parent), executeNextFun(executeNextFun), firstExecuteNextEmit(true)
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
            self->updateSizeTimer->start(10);
            return;
        }
        QList<QVariant> list = res.toList();
        if (list.size() != 2)
            return;
        int w = list[0].toInt();
        int h = list[1].toInt();
        qDebug() << "WebEngineView get size:" << w << h << self.data();
        if (w <= 0 || h <= 0) {
            self->updateSizeTimer->start(10);
            return;
        }
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

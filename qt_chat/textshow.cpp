#include "textshow.h"

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
    webEngineView->page()->runJavaScript(js, [this](const QVariant &res) {
        if (res.isNull()) {
            updateSizeTimer->start(10);
            return;
        }
        QList<QVariant> list = res.toList();
        if (list.size() != 2)
            return;
        int w = list[0].toInt();
        int h = list[1].toInt();
        qDebug() << "WebEngineView get size:" << w << h << this;
        if (w <= 0 || h <= 0) {
            updateSizeTimer->start(10);
            return;
        }
        if (webEngineSize == QSize(w, h)) {
            if (isSetTextEnd) {
                isSetTextEnd = false;
                isSizeFinish = true;
            }
            return;
        }
        webEngineSize = QSize(w, h);
        textShowFuni += 1;
        qDebug() << "textShowFuni:" << textShowFuni << this;
        webEngineView->setFixedSize(w, h);
        // mainHLayout->addWidget(webEngineView);
        // if (webEngineView->isHidden()) {
        //     qDebug() << "isHidden:" << webEngineView->isHidden();
        //     webEngineView->show();
        // }
        setFixedSize(w + 10, h);
        emit setSizeFinished();
        // isEmitSizeFinish = true;
        if (isSetTextEnd) {
            isSetTextEnd = false;
            isSizeFinish = true;
        }
        if (firstExecuteNextEmit) {
            firstExecuteNextEmit = false;
            if (this->executeNextFun) {
                connect(this, &TextShow::executeNext, this->executeNextFun);
                QTimer::singleShot(0, this, [this]() { emit executeNext(); });
                qDebug() << "textShow executeNext emit" << this;
            }
        }
        qDebug() << "textShow onUpdateSize end" << this;
    });
}

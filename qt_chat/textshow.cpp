#include "textshow.h"

TextShow::TextShow(const QString &text, int maxWidth, QWidget *parent)
    : ThinkWidget(text, maxWidth, parent), firstExecuteNextEmit(true)
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
            updateSizeTimer->start(20);
            return;
        }
        if (webEngineSize == QSize(w, h))
            return;
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
        isEmitSizeFinish = true;
        if (firstExecuteNextEmit) {
            firstExecuteNextEmit = false;
            emit executeNext();
            qDebug() << "textShow executeNext emit" << this;
        }
        qDebug() << "textShow onUpdateSize end" << this;
    });
}

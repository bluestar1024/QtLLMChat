#include "label.h"
#include "globalvariables.h"

#include <QFont>
#include <QFontDatabase>

Label::Label(QWidget *parent) : QLabel(parent) {
  resize(50, 32);

  int fontId = QFontDatabase::addApplicationFont(fontFilePath);
  if (fontId != -1) {
    QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
    if (!fontFamilies.isEmpty()) {
      QString fontFamily = fontFamilies.at(0);
      QFont font(fontFamily);
      font.setPixelSize(windowFontPixelSize);
      setFont(font);
    }
  }
}

Label::~Label() {}

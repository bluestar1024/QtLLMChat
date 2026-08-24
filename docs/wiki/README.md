# QtLLMChat 项目文档

本目录为 QtLLMChat 项目（Qt 6.8.0 桌面 LLM 聊天客户端）的初始 Wiki 文档。

## 文章列表

| 文档 | 内容 |
| --- | --- |
| [项目概览](项目概览.md) | 项目定位、核心功能、技术栈、目录结构、当前状态 |
| [项目架构](项目架构.md) | 总体分层、模块职责、关键数据流、构建产物位置 |
| [消息渲染管线](消息渲染管线.md) | 分片接收 → 消息部件 → 流式渲染 → Markdown/HTML → 代码块 全链路 |
| [构建与运行](构建与运行.md) | 开发环境、qmake 构建、VSCode 配置、运行时目录约定、常见问题 |
| [数据与配置](数据与配置.md) | config.txt、聊天记录格式、主题/高亮 XML、qrc 资源、全局常量 |
| [测试原型](测试原型.md) | test/ 目录各原型项目说明 |

## 模块知识卡片

模块级技术笔记位于 [docs/knowledge/](../knowledge/)，与上述 Wiki 文章互补：

| 卡片 | 对应模块 |
| --- | --- |
| [主窗口框架](../knowledge/主窗口框架.md) | mainwindow / globalvariables |
| [消息渲染与显示](../knowledge/消息渲染与显示.md) | messagewidget / thinkwidget / textshow / codeshow / loadingwidget 等 |
| [Markdown 解析与 HTML 渲染](../knowledge/Markdown解析与HTML渲染.md) | markdown_parser / html_renderer / markdown_block_element / markdown_inline_element |
| [代码编辑器与语法高亮](../knowledge/代码编辑器与语法高亮.md) | codeeditor / stylesyntaxhighlighter / 各语言高亮器 / syntaxstyle |
| [设置与配置管理](../knowledge/设置与配置管理.md) | settingwidget / settingedit / spinbox / doublespinbox / slider |
| [消息线程与聊天记录](../knowledge/消息线程与聊天记录.md) | messagethread / chatrecordswidget / chatrecords 格式 |
| [基础控件库](../knowledge/基础控件库.md) | widget / pushbutton / textedit / webengineview / language 等通用控件 |

## 维护说明

- 修改主应用后，涉及架构/接口变化时同步更新对应文档。
- 新增模块时在 [docs/knowledge/](../knowledge/) 新增卡片，并在本文档登记。

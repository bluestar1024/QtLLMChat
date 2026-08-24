# Markdown 解析与 HTML 渲染

## overview

自研轻量 Markdown 渲染子系统：`MarkdownParser` 解析原始文本为块/行内元素，`HtmlRenderer` 输出 HTML，`ThinkWidget` / `TextShow` 组装为含 MathJax 的完整页面并由 `WebEngineView` 加载。无第三方 Markdown 库依赖。

## architecture_design

- 数据模型：`MarkdownBlockElement`（`BlockType` 枚举：Paragraph / Headinglevel1-3 / HorizontalRules / CodeBlocks / BlockQuote / OrderedList / UnorderedList）内含 `std::vector<LineElement>`；`LineElement` 由文本 + `std::vector<MarkdownInlineElement>` 组成。
- 解析流程（MarkdownParser）：
  1. `split(rawText)`：按行切分为 `rawBlock`（`std::vector<std::vector<QString>>`）。
  2. `blockParse`：识别块类型并构建 `MarkdownBlockElement` 序列。
  3. `inlineParse`：行内元素解析（粗斜体、行内代码、链接等），`refLinks`（QHash）存放引用链接。
- 渲染（HtmlRenderer）：`blockHtml` 输出块级标签、`inlineHtml` 输出行内标签；`setStyle` 加载 CSS；`init` / `tail` 拼装页面骨架，`getHtml` 取结果。
- 页面装配（ThinkWidget）：`mathJaxCdn` 模板（MathJax 3 配置：`inlineMath ["$","$"]`、`displayMath ["$$","$$"]`、SVG 输出、禁菜单）→ `fullHtmlText = 模板 + 渲染HTML + 尾部` → `webEngineView->setHtml(fullHtmlText, base)`。
- 表格：`getTable` 解析 `|` 管道表格 → `TableInfo{items, alignList, row, col, complete}`，`getAlignmentClass` 映射对齐类。

## tech_stack

- 纯 QtCore（QString / QHash）+ STL（vector / std::function），无第三方依赖。
- MathJax 3 es5（`mathjax/es5/tex-mml-chtml.js`，随 resources.qrc 打包）。
- WebEngine 加载：`WebEngineView::setHtml(html, base)`，base 指向可执行文件上级目录以保证相对资源可解析。

## coding_conventions

- 块类型枚举命名使用驼峰（`Headinglevel1` 等，非标准 Qt 风格），保持现状。
- 解析器类为普通 C++ 类（非 QObject），仅数据函数。

## unique_setup_and_commands

- MathJax 资源路径全局常量：`mathjaxScriptPath = "mathjax/es5/tex-mml-chtml.js"`（相对 base URL 加载）。
- 修改渲染规则后需重新编译（资源内嵌），并清理 WebEngine 缓存验证。

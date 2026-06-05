# Codex 全局指导

## 编码规范
- 文件统一使用 UTF-8 编码，无 BOM（如果原来是utf-8 with bom则继续使用原来的）；Windows上行尾 CRLF；缩进 4 个空格。


## 代码质量
- 所有文件必须添加文件注释， 所有注释必须清晰描述代码意图；公共 API 注释需使用 Doxygen 风格。
- 添加适当的错误处理


## 搜索工具使用
- 查找文件时优先使用 `fd` 或 `rg --files`；如果都不可用，再使用 PowerShell 原生命令。
- 查找文本内容时优先使用 `rg`；需要文件名和内容组合筛选时，优先组合 `rg --files`、`rg` 和管道。
- 需要模糊筛选文件或内容时，可以使用 `fzf --filter "<关键词>"` 做非交互筛选。
- 避免在自动化任务中直接启动交互式 `fzf`，以免命令等待人工输入导致任务卡住。


## Git 提交规范
- 提交信息使用中文 Conventional Commit 格式：`type(scope): 中文描述`。
- 常用类型包括 `feat`、`fix`、`docs`、`style`、`refactor`、`test`、`build`、`ci`、`chore`。
- 描述使用简洁中文，明确说明本次提交的实际变更意图。
- 示例：`docs(agents): 补充全局搜索工具使用规范`。


### 实现思路

```mermaid
flowchart LR
  A[解析参数] --> B[检索收集]
  B --> C[格式整理]
  C --> D[打印输出]
```
### 模块划分

| 文件 | 职责 |
| --- | --- |
| `lrg.c` | 程序入口，初始化各模块，汇总退出码 |
| `parse.c` / `parse.h` | **参数解析** 识别`OPTION`、`PATTERN`、`FILE` |
| `search.c` / `search.h` | **检索搜集** 逐块 `read` 文件，拆行，匹配，记录匹配行 |
| `arrange.c` / `arrange.h` | **格式整理** 根据选项把匹配行格式化成最终输出 |
| `mstring.c` / `mstring.h` | 动态字符串，负责拼接与扩容 |
| `error.c` / `error.h` | 统一报告错误 |



### 构建与测试
使用**make**进行构建
```make
make
```
将在根目录下生成`lrg`可执行文件
```make
make test
```
将自动构建并运行测试脚本`run_tests.sh`
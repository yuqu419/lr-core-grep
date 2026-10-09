## 实现思路
主要分为两个步骤：**解析参数**和**在文件中查找pattern**
### 解析参数
解析参数功能在parse.c文件中实现
我将解析分别分为处理pattern，处理选项，处理文件名，并分别存储起来，留到查找步骤使用
如果在每个步骤中遇到错误，会修改退出码和相应文件名，回到main函数进行错误处理
### 文件中查找
查找功能在search.c文件中实现
文件中查找以此分割为当个文件查找，单行查找，


错误处理
错误信息一律写到 stderr，必须包含 lrg: 前缀、文件名与失败原因（用 strerror(errno) 取到原因文本），推荐格式：

$ ./lrg error sample.log no-such-file
error: give up
lrg: no-such-file: No such file or directory      # 这一行在 stderr
某个文件打开或读取失败：报告错误，继续处理剩下的文件，最终退出码为 2。

用法错误：向 stderr 输出用法提示并退出 2，不处理任何文件，推荐格式 usage: lrg [-nic] PATTERN [FILE...]。

所有打开的文件描述符都必须关闭，包括出错提前返回的路径；read 返回 -1 且 errno == EINTR 时应重试，而不是当作错误。

打开文件错误
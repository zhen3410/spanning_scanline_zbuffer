# spanning_scanline_zbuffer
# 区间扫描线z-buffer算法

## 使用说明
### 使用键盘控制：
  1. ‘x’:绕x轴旋转.
  2. ‘y’:绕y轴旋转.
  3. ‘+’:放大.
  4. ‘-’:缩小.
  5. 方向键‘↑’或数字‘8’:向上移动.
  6. 方向键‘↓’或数字‘2’:向下移动.
  7. 方向键‘←’或数字‘4’:向左移动.
  8. 方向键‘→’或数字‘6’:向右移动.
  9. ‘ESC’:退出.

## 数据结构说明

  [***存储OBJ模型的结构体***]
  Model中存放该模型的面片数，以及为使其正常显示需要初始放大的倍数，还有面片链表。每个面片中仅存放顶点信息.
  
  [***分类边表和分类多边形表、活化边表和活化多边形表***]
  
  [***OBJ类***]
  主要用来导入OBJ模型，并对其进行旋转平移缩放等操作。主要包括以下函数：
  1. Init_face_list():
  初始化面片链表.
  2. Load_face():
  导入OBJ模型，存放到Model中.
  3. trans():
  对导入的OBJ模型进行旋转平移缩放等操作.

  [***Zbuffer类***]
  主要用来对导入的OBJ模型进行处理，以使其正常显示。主要包括以下函数.
  1. init():
  初始化分类边表和分类多边形表、活化边表和活化多边形表.
  2. creatDS():
  根据导入的OBJ模型创建相应的分类边表和分类多边形表.
  3. exc_zbuffer():
  执行扫描线z缓冲器算法，对当前扫描线缓冲器x_resolution赋以相应的像素值.
  4. update_act():
  更新活化边表和活化多边形表.


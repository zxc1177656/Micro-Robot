import sys
import os
import socket
import threading
from PyQt5.QtWidgets import (QApplication, QWidget, QVBoxLayout, QHBoxLayout,
                             QPushButton, QLabel, QGroupBox, QGridLayout, QSizePolicy,
                             QFrame)
from PyQt5.QtCore import Qt, QTimer, QPoint, pyqtSignal, QObject
from PyQt5.QtGui import QFont, QPainter, QColor, QPen, QBrush, QPalette, QIcon

# 手动设置平台插件路径
plugin_path = r"D:\大创\wifi\.venv\Lib\site-packages\PyQt5\Qt5\plugins"
os.environ['QT_QPA_PLATFORM_PLUGIN_PATH'] = plugin_path

# 服务器配置
SERVER_IP = "192.168.110.138"
SERVER_PORT = 8000

# 新增方向命令映射
direction_commands = {
    'forward': b'k',  # 前进
    'backward': b'l',  # 后退
    'left_forward': b'm',  # 左前
    'right_forward': b'n',  # 右前
    'left_backward': b'o',  # 左后
    'right_backward': b'p'  # 右后
}

# 全局变量
client_connection = None
connection_status = "未连接"
command_map = {
    '0': "LCD初始化",
    '2': "左轮关",
    '3': "右轮关",
    '4': "左轮开",
    '5': "右轮开",
    '6': "黑车",
    '7': "黑马",
    '8': "黑炮",
    '9': "黑象",
    'a': "黑卒",
    'b': "黑士",
    'c': "黑将",
    'd': "红车",
    'e': "红马",
    'f': "红炮",
    'g': "红象",
    'h': "红卒",
    'i': "红士",
    'j': "红将",
    'k': "前进",
    'l': "后退",
    'm': "左前",
    'n': "右前",
    'o': "左后",
    'p': "右后"
}


# 自定义信号类
class SignalEmitter(QObject):
    direction_changed = pyqtSignal(str)


# 控制轮盘组件
class Joystick(QFrame):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setMinimumSize(200, 200)
        self.setStyleSheet("background-color: #e0e0e0; border-radius: 100px;")
        # 使用整数除法确保得到整数坐标
        self.center = self.width() // 2, self.height() // 2
        self.radius = min(self.width(), self.height()) // 2 - 10
        self.thumb_radius = 30
        self.current_pos = QPoint(self.center[0], self.center[1])
        self.emitter = SignalEmitter()
        self.direction = None

    def resizeEvent(self, event):
        # 使用整数除法确保得到整数坐标
        self.center = self.width() // 2, self.height() // 2
        self.radius = min(self.width(), self.height()) // 2 - 10
        super().resizeEvent(event)

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)

        # 绘制背景圆环
        pen = QPen(QColor(100, 100, 100), 2)
        painter.setPen(pen)
        painter.drawEllipse(int(self.center[0] - self.radius),
                            int(self.center[1] - self.radius),
                            int(self.radius * 2), int(self.radius * 2))

        # 绘制方向指示线
        pen = QPen(QColor(150, 150, 150), 1, Qt.DashLine)
        painter.setPen(pen)
        # 水平线
        painter.drawLine(int(self.center[0] - self.radius), int(self.center[1]),
                         int(self.center[0] + self.radius), int(self.center[1]))
        # 垂直线
        painter.drawLine(int(self.center[0]), int(self.center[1] - self.radius),
                         int(self.center[0]), int(self.center[1] + self.radius))
        # 对角线
        painter.drawLine(int(self.center[0] - self.radius * 0.707),
                         int(self.center[1] - self.radius * 0.707),
                         int(self.center[0] + self.radius * 0.707),
                         int(self.center[1] + self.radius * 0.707))
        painter.drawLine(int(self.center[0] - self.radius * 0.707),
                         int(self.center[1] + self.radius * 0.707),
                         int(self.center[0] + self.radius * 0.707),
                         int(self.center[1] - self.radius * 0.707))

        # 绘制当前位置
        if self.direction:
            if self.direction == 'forward':
                color = QColor(76, 175, 80, 150)  # 绿色
            elif self.direction == 'backward':
                color = QColor(244, 67, 54, 150)  # 红色
            elif self.direction in ['left_forward', 'right_forward']:
                color = QColor(33, 150, 243, 150)  # 蓝色
            else:
                color = QColor(255, 152, 0, 150)  # 橙色
        else:
            color = QColor(150, 150, 150, 150)

        painter.setBrush(QBrush(color))
        painter.setPen(QPen(QColor(50, 50, 50), 2))
        painter.drawEllipse(self.current_pos, self.thumb_radius, self.thumb_radius)

        # 绘制方向文本
        if self.direction:
            painter.setPen(QPen(QColor(50, 50, 50), 1))
            painter.setFont(QFont("SimHei", 10))
            direction_text = {
                'forward': "前",
                'backward': "后",
                'left_forward': "左前",
                'right_forward': "右前",
                'left_backward': "左后",
                'right_backward': "右后"
            }
            painter.drawText(10, 20, direction_text.get(self.direction, ""))

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self._update_position(event.pos())

    def mouseMoveEvent(self, event):
        if event.buttons() & Qt.LeftButton:
            self._update_position(event.pos())

    def mouseReleaseEvent(self, event):
        if event.button() == Qt.LeftButton:
            # 回到中心位置
            self.current_pos = QPoint(self.center[0], self.center[1])
            self.direction = None
            self.emitter.direction_changed.emit("stop")
            self.update()

    def _update_position(self, pos):
        # 计算相对于中心的位置
        dx = pos.x() - self.center[0]
        dy = pos.y() - self.center[1]

        # 计算距离
        distance = (dx ** 2 + dy ** 2) ** 0.5

        # 限制在圆内
        if distance > self.radius:
            scale = self.radius / distance
            dx = int(dx * scale)  # 转换为整数
            dy = int(dy * scale)  # 转换为整数

        # 更新当前位置
        self.current_pos = QPoint(self.center[0] + dx, self.center[1] + dy)

        # 确定方向
        angle = self._calculate_angle(dx, dy)
        self.direction = self._get_direction(angle)

        # 发送方向变化信号
        self.emitter.direction_changed.emit(self.direction)

        # 重绘
        self.update()

    def _calculate_angle(self, dx, dy):
        """重新计算角度，调整坐标系映射（y轴正向为下，需反转dy）"""
        import math
        # 反转dy以匹配"上拉为前"的逻辑（原代码中dy向下为正，现改为向上为正）
        angle = math.degrees(math.atan2(dy, dx))  # 注意参数顺序改为(dy, dx)
        if angle < 0:
            angle += 360
        return angle

    def _get_direction(self, angle):
        """重新定义方向映射，使上拉为前进（forward）"""
        # 调整角度区间以匹配新的方向逻辑
        if (angle >= 330 or angle < 30) or (angle >= 150 and angle < 210):
            if (angle >= 330 or angle < 30):
                return 'forward'  # 上拉为前（原逻辑为0°方向）
            else:
                return 'backward'  # 下拉为后（180°方向）
        elif angle >= 30 and angle < 90:
            return 'right_forward'  # 右上为右前（45°方向）
        elif angle >= 90 and angle < 150:
            return 'right_backward'  # 右下为右后（135°方向）
        elif angle >= 210 and angle < 270:
            return 'left_backward'  # 左下为左后（225°方向）
        elif angle >= 270 and angle < 330:
            return 'left_forward'  # 左上为左前（315°方向）
        else:
            return None


# 主界面类（在原有基础上添加轮盘控制）
class MicroRobotControlUI(QWidget):
    def __init__(self):
        super().__init__()
        self.server_socket = None
        self.timer = QTimer(self)
        self.initUI()
        self.start_server()
        self.timer.timeout.connect(self.update_status)
        self.timer.start(1000)  # 每秒刷新一次状态

    def initUI(self):
        # 窗口基本设置
        self.setWindowTitle('微型机器人控制系统')
        self.setMinimumSize(600, 800)
        self.setStyleSheet("background-color: #f5f5f5;")

        # 主布局
        main_layout = QVBoxLayout(self)
        main_layout.setSpacing(15)
        main_layout.setContentsMargins(15, 15, 15, 15)

        # 状态显示区域
        status_group = QGroupBox("系统状态")
        status_group.setStyleSheet("font-weight: bold;")
        status_layout = QHBoxLayout()

        self.status_label = QLabel(f"状态: {connection_status}")
        self.status_label.setFont(QFont("SimHei", 11))
        self.status_label.setStyleSheet("color: #ff5722;")

        status_layout.addWidget(self.status_label)
        status_group.setLayout(status_layout)
        main_layout.addWidget(status_group)

        # 控制区域（新增轮盘控制）
        control_group = QGroupBox("方向控制")
        control_layout = QHBoxLayout()

        # 控制轮盘
        self.joystick = Joystick()
        self.joystick.emitter.direction_changed.connect(self.handle_direction)
        control_layout.addWidget(self.joystick)

        # 方向说明
        direction_info = QVBoxLayout()
        direction_info.addWidget(QLabel("方向控制说明:", font=QFont("SimHei", 10, QFont.Bold)))
        direction_info.addWidget(QLabel("→ 前进", font=QFont("SimHei", 9)))
        direction_info.addWidget(QLabel("← 后退", font=QFont("SimHei", 9)))
        direction_info.addWidget(QLabel("↗ 左前", font=QFont("SimHei", 9)))
        direction_info.addWidget(QLabel("↘ 右前", font=QFont("SimHei", 9)))
        direction_info.addWidget(QLabel("↖ 左后", font=QFont("SimHei", 9)))
        direction_info.addWidget(QLabel("↙ 右后", font=QFont("SimHei", 9)))
        direction_info.addStretch()
        control_layout.addLayout(direction_info)

        control_group.setLayout(control_layout)
        main_layout.addWidget(control_group)

        # 电机控制区域（保留原有功能）
        motor_group = QGroupBox("电机驱动控制")
        motor_layout = QHBoxLayout()

        left_motor = QVBoxLayout()
        left_motor.addWidget(QLabel("左轮", font=QFont("SimHei", 10)))
        self.left_on = QPushButton("启动")
        self.left_on.setStyleSheet("background-color: #4caf50; color: white;")
        self.left_off = QPushButton("停止")
        self.left_on.clicked.connect(lambda: self.send_command(b'4'))
        self.left_off.clicked.connect(lambda: self.send_command(b'2'))
        left_motor.addWidget(self.left_on)
        left_motor.addWidget(self.left_off)

        right_motor = QVBoxLayout()
        right_motor.addWidget(QLabel("右轮", font=QFont("SimHei", 10)))
        self.right_on = QPushButton("启动")
        self.right_on.setStyleSheet("background-color: #4caf50; color: white;")
        self.right_off = QPushButton("停止")
        self.right_on.clicked.connect(lambda: self.send_command(b'5'))
        self.right_off.clicked.connect(lambda: self.send_command(b'3'))
        right_motor.addWidget(self.right_on)
        right_motor.addWidget(self.right_off)

        motor_layout.addLayout(left_motor)
        motor_layout.addLayout(right_motor)
        motor_group.setLayout(motor_layout)
        main_layout.addWidget(motor_group)

        # LCD显示控制（保留原有功能）
        lcd_group = QGroupBox("LCD显示控制")
        lcd_layout = QVBoxLayout()
        lcd_btn = QPushButton("LCD初始化测试")
        lcd_btn.setStyleSheet("background-color: #2196f3; color: white;")
        lcd_btn.clicked.connect(lambda: self.send_command(b'0'))
        lcd_layout.addWidget(lcd_btn)
        lcd_group.setLayout(lcd_layout)
        main_layout.addWidget(lcd_group)

        # 象棋图案控制区域（保留原有功能）
        chess_group = QGroupBox("象棋图案显示")
        chess_layout = QGridLayout()
        chess_layout.setSpacing(8)

        # 黑棋按钮
        chess_layout.addWidget(self.create_chess_button('6', "黑车", "#333333"), 0, 0)
        chess_layout.addWidget(self.create_chess_button('7', "黑马", "#333333"), 0, 1)
        chess_layout.addWidget(self.create_chess_button('8', "黑炮", "#333333"), 0, 2)
        chess_layout.addWidget(self.create_chess_button('9', "黑象", "#333333"), 0, 3)
        chess_layout.addWidget(self.create_chess_button('a', "黑卒", "#333333"), 1, 0)
        chess_layout.addWidget(self.create_chess_button('b', "黑士", "#333333"), 1, 1)
        chess_layout.addWidget(self.create_chess_button('c', "黑将", "#333333"), 1, 2)

        # 红棋按钮
        chess_layout.addWidget(self.create_chess_button('d', "红车", "#ff4444"), 2, 0)
        chess_layout.addWidget(self.create_chess_button('e', "红马", "#ff4444"), 2, 1)
        chess_layout.addWidget(self.create_chess_button('f', "红炮", "#ff4444"), 2, 2)
        chess_layout.addWidget(self.create_chess_button('g', "红象", "#ff4444"), 2, 3)
        chess_layout.addWidget(self.create_chess_button('h', "红卒", "#ff4444"), 3, 0)
        chess_layout.addWidget(self.create_chess_button('i', "红士", "#ff4444"), 3, 1)
        chess_layout.addWidget(self.create_chess_button('j', "红将", "#ff4444"), 3, 2)

        chess_group.setLayout(chess_layout)
        main_layout.addWidget(chess_group)

        # 日志显示区域（保留原有功能）
        log_group = QGroupBox("通信日志")
        log_group.setFixedHeight(200)
        log_layout = QVBoxLayout()
        self.log_display = QLabel("等待连接...")
        self.log_display.setStyleSheet("background-color: white; border: 1px solid #ddd; padding: 5px;")
        self.log_display.setWordWrap(True)
        self.log_display.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        log_layout.addWidget(self.log_display)
        log_group.setLayout(log_layout)
        main_layout.addWidget(log_group)

    def create_chess_button(self, cmd, text, bg_color):
        """创建象棋图案按钮"""
        btn = QPushButton(text)
        btn.setStyleSheet(f"background-color: {bg_color}; color: white; font-weight: bold;")
        btn.clicked.connect(lambda _, c=cmd: self.send_command(c.encode()))
        return btn

    def handle_direction(self, direction):
        """处理方向变化"""
        if direction == "stop":
            # 停止所有电机
            self.send_command(b'2')  # 左轮停
            self.send_command(b'3')  # 右轮停
            self.log("方向控制: 停止")
        else:
            # 发送对应方向命令
            if direction in direction_commands:
                self.send_command(direction_commands[direction])
                self.log(f"方向控制: {direction}")

    def start_server(self):
        """启动TCP服务器"""
        try:
            self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.server_socket.bind((SERVER_IP, SERVER_PORT))
            self.server_socket.listen(1)
            self.log("服务器已启动，监听地址: {}:{}".format(SERVER_IP, SERVER_PORT))

            # 启动连接监听线程
            conn_thread = threading.Thread(target=self.accept_connection)
            conn_thread.daemon = True
            conn_thread.start()
        except socket.error as e:
            self.log(f"服务器启动失败: {e}")
            self.status_label.setText(f"状态: 服务器启动失败")
            self.status_label.setStyleSheet("color: #f44336;")

    def accept_connection(self):
        """接受单片机连接"""
        global client_connection, connection_status
        self.log("等待单片机连接...")
        try:
            while True:
                conn, addr = self.server_socket.accept()
                client_connection = conn
                connection_status = f"已连接 (IP: {addr[0]})"
                self.log(f"单片机已连接: {addr}")
                self.status_label.setText(f"状态: {connection_status}")
                self.status_label.setStyleSheet("color: #4caf50;")

                # 启动数据接收线程
                recv_thread = threading.Thread(target=self.receive_data)
                recv_thread.daemon = True
                recv_thread.start()
        except Exception as e:
            self.log(f"接受连接失败: {e}")
            connection_status = "连接失败"
            self.status_label.setText(f"状态: {connection_status}")
            self.status_label.setStyleSheet("color: #f44336;")

    def receive_data(self):
        """接收单片机返回的数据"""
        global client_connection
        try:
            while True:
                if client_connection:
                    data = client_connection.recv(1024)
                    if not data:
                        break
                    data_str = data.decode('utf-8', errors='ignore').strip()
                    self.log(f"接收数据: {data_str}")
        except Exception as e:
            self.log(f"数据接收失败: {e}")
            if client_connection:
                client_connection.close()
            client_connection = None
            connection_status = "未连接"
            self.status_label.setText(f"状态: {connection_status}")
            self.status_label.setStyleSheet("color: #ff5722;")

    def send_command(self, command):
        """向单片机发送命令"""
        global client_connection
        if client_connection:
            try:
                client_connection.sendall(command)
                cmd_char = command.decode()
                self.log(f"已发送命令: {cmd_char} ({command_map.get(cmd_char, '未知命令')})")
            except socket.error as e:
                self.log(f"发送失败: {e}")
                client_connection = None
                connection_status = "未连接"
                self.status_label.setText(f"状态: {connection_status}")
                self.status_label.setStyleSheet("color: #ff5722;")
        else:
            self.log("没有连接的客户端，无法发送命令")

    def update_status(self):
        """定时更新界面状态"""
        if client_connection:
            self.left_on.setEnabled(True)
            self.left_off.setEnabled(True)
            self.right_on.setEnabled(True)
            self.right_off.setEnabled(True)
        else:
            self.left_on.setEnabled(False)
            self.left_off.setEnabled(False)
            self.right_on.setEnabled(False)
            self.right_off.setEnabled(False)

    def log(self, message):
        """更新日志显示"""
        print(message)
        current_log = self.log_display.text()
        new_log = f"[{threading.current_thread().name}] {message}\n" + current_log
        if len(new_log) > 1000:
            new_log = new_log[:1000]
        self.log_display.setText(new_log)

    def closeEvent(self, event):
        """窗口关闭时释放资源"""
        global client_connection
        self.timer.stop()
        if client_connection:
            client_connection.close()
        if self.server_socket:
            self.server_socket.close()
        self.log("程序已关闭")
        event.accept()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    font = QFont("SimHei")
    app.setFont(font)
    ui = MicroRobotControlUI()
    ui.show()
    sys.exit(app.exec_())




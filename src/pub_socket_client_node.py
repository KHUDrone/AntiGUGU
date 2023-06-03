import rospy
import socketio
import json
import numpy as np
from std_msgs.msg import String
from geometry_msgs.msg import PoseStamped
from sensor_msgs.msg import Image
import cv2
from cv_bridge import CvBridge
import eventlet

host = "http://127.0.0.0:5001"

global comp_img

comp_img = 0

QR_loc = PoseStamped()
server_data = PoseStamped()

bridge = CvBridge()

def handle_img(raw_img):
        global comp_img
        img = bridge.imgmsg_to_cv2(raw_img)
        frame = cv2.imencode('.jpg', img, [cv2.IMWRITE_JPEG_QUALITY, 80])[1]
        comp_img = frame

sio = socketio.Client()
pub_QR = rospy.Publisher('/server/QR_locating',PoseStamped,queue_size=1)
pub_data = rospy.Publisher('/server/data',PoseStamped,queue_size=1)

sub_camera = rospy.Subscriber('/camera/image_raw',Image,handle_img)

rospy.init_node('pub_socket_client_node', anonymous=True)
rate = rospy.Rate(0.3)

@sio.on('connect', namespace='/realtime')
def connect():
    global comp_img
    rospy.loginfo("connected")
    while True:
        if type(comp_img) == type(int):
            continue
        sio.emit("camera", {"frame": comp_img.tolist()}, namespace='/realtime')
        sio.sleep(1)

@sio.on('disconnect', namespace='/realtime')
def disconnect():
    rospy.loginfo('disconnected')
    while 1:
        try :
               sio.connect(host, namespaces='/realtime')
               break
        except Exception as e:
               continue

@sio.on("result", namespace='/realtime')
def result(data):
    QR_loc.pose.position.x=data["QR_loc_x"]
    QR_loc.pose.position.y=data["QR_loc_y"]
    server_data.pose.position.x = data["current_home"]
    server_data.pose.position.y = data["next_home"]
    server_data.pose.position.z = data['bird']
    pub_QR.publish(QR_loc)	
    pub_data.publish(server_data)

if __name__ == "__main__" :
	rospy.loginfo('Client start!')
	sio.connect(host, namespaces=['/realtime'])
	sio.wait()


import rospy
import socketio
import json
import numpy as np
from std_msgs.msg import String

from geographic_msgs.msg import GeoPoseStamped
from geometry_msgs.msg import PoseStamped, Vector3
from sensor_msgs.msg import Image
from mavros_msgs.msg import GlobalPositionTarget

import eventlet

host = "http://127.0.0.0:5001"

global comp_img


def handle_img(raw_img):



sio = socketio.Client()
pub_target = rospy.Publisher('targeting',GeoPoseStamped,queue_size=1)
sub_camera = rospy.Subscriber('/camera/image_raw',Image,handle_img)

rospy.init_node('pub_socket_client_node', anonymous=True)
rate = rospy.Rate(0.3)

@sio.on('connect', namespace='/realtime')
def connect():
    print("connected")


    while True:
        ret, frame = capture.read()

        if ret:
            # cv2.imshow('video', frame)


            frame = cv2.imencode('.jpg', frame, [cv2.IMWRITE_JPEG_QUALITY, 80])[1]
            #print(frame.shape)
            frame = frame.tolist()
            #frame = np.array(frame)

            print("hello")
            #print(type(frame))
            sio.emit("camera", {"frame": frame}, namespace='/realtime')
            # videoWriter.write(cv2.cvtColor(frame, cv2.COLOR_RGB2BGR))

        if cv2.waitKey(1) == 27:
            break


    capture.release()
    cv2.destroyAllWindows()


@sio.on('disconnect', namespace='/realtime')
def disconnect():
    print('disconnected')
    #sio.connect(host, namespaces='/realtime')


@sio.on("result", namespace='/realtime')
def result(data):
    print('received:', data)

if __name__ == "__main__" :
	print('start')
    sio.connect(host, namespaces='/realtime')
    sio.wait()


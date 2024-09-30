import cv2 as cv
from cv2 import aruco
import cv2.typing
import numpy as np
import websocket
import socketio
import json
import keyboard
import sys

sio = socketio.Client()

calib_data_path = "./MultiMatrix.npz"

calib_data = np.load(calib_data_path)

cam_mat = calib_data["camMatrix"]
dist_coef = calib_data["distCoef"]
r_vectors = calib_data["rVector"]
t_vectors = calib_data["tVector"]

def run():
    MARKER_SIZE = 8  # centimeters

    # Use the correct method to get the predefined dictionary
    marker_dict = aruco.getPredefinedDictionary(aruco.DICT_6X6_50)

    param_markers = aruco.DetectorParameters()

    cap = cv.VideoCapture(0)
    # cap = cv.VideoCapture('http://10.10.203.15:8080/video')

    # ws.run_forever()

    while True:
        if keyboard.is_pressed('q'): 
            sys.exit()
        ret, frame = cap.read()
        if not ret:
            break
        gray_frame = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        marker_corners, marker_IDs, reject = aruco.detectMarkers(
            gray_frame, marker_dict, parameters=param_markers
        )
        # sio.emit("vision", {'type': 'car', 'x': 10, 'y': 0, 'r': 0})
        if marker_corners:
            rVec, tVec, _ = aruco.estimatePoseSingleMarkers(
                marker_corners, MARKER_SIZE, cam_mat, dist_coef
            )
            total_markers = range(0, marker_IDs.size)
            
            id_car = -1
            id_marker = -1
            
            tcar = None
            tmarker = None
            
            for ids, corners, i in zip(marker_IDs, marker_corners, total_markers):
                cv.polylines(
                    frame, [corners.astype(np.int32)], True, (0, 255, 255), 4, cv.LINE_AA
                )
                corners = corners.reshape(4, 2)
                corners = corners.astype(int)
                top_right = corners[0].ravel()
                top_left = corners[1].ravel()
                bottom_right = corners[2].ravel()
                bottom_left = corners[3].ravel()
                
                if ids[0] == 10:
                    id_car = 10
                    tcar = tVec[i][0]
                else:
                    id_marker = ids[0]
                    tmarker = tVec[i][0]

                # Calculate the distance
                distance = np.sqrt(
                    tVec[i][0][2] ** 2 + tVec[i][0][0] ** 2 + tVec[i][0][1] ** 2
                )
                # Draw the pose of the marker
                point = cv.drawFrameAxes(frame, cam_mat, dist_coef, rVec[i], tVec[i], 4, 4)
                cv.putText(
                    frame,
                    f"id: {ids[0]} Dist: {round(distance, 2)}",
                    top_right,
                    cv.FONT_HERSHEY_PLAIN,
                    1.3,
                    (0, 0, 255),
                    2,
                    cv.LINE_AA,
                )
                cv.putText(
                    frame,
                    f"x:{round(tVec[i][0][0],1)} y: {round(tVec[i][0][1],1)} ",
                    bottom_right,
                    cv.FONT_HERSHEY_PLAIN,
                    1.0,
                    (0, 0, 255),
                    2,
                    cv.LINE_AA,
                )
                # id = (int)ids[0]
                # print(ids, "  ", corners)
        
            if id_car == 10 and id_marker != -1:
                tc = tcar - tmarker 
                print(f"x:{round(tc[0], 5)} y: {round(tc[0],5)} ")
                # sio.emit("vision", {'id': int(ids[0]), 'x': round(tc[0],5), 'y': round(tc[1],5), 'r': 0})
        
        cv.imshow("frame", frame)
        key = cv.waitKey(1)
        if key == ord("q"):
            break
    cap.release()
    cv.destroyAllWindows()
    
@sio.event
def connect():
    print('connection established')
    run()

@sio.event
def my_message(data):
    print('message received with ', data)
    sio.emit('my response', {'response': 'my response'})

@sio.event
def disconnect():
    print('disconnected from server')

sio.connect('ws://localhost:3000/?id=opencv')
sio.wait() 

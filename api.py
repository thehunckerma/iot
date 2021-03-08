from fastapi import FastAPI, WebSocket
from fastapi.middleware.cors import CORSMiddleware
import uvicorn
import cv2 as cv
import numpy as np
from urllib.request import urlopen
import keyboard

api = FastAPI()

api.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


@api.websocket("/ws/motor")
async def websocket_endpoint(websocket: WebSocket):
    await websocket.accept()
    bts = b''
    stream = urlopen("http://192.168.43.154:81/stream")
    face_cascade = cv.CascadeClassifier('haarcascade_frontalface_alt.xml')
    while True:
        try:
            bts += stream.read(1024)
            jpghead = bts.find(b'\xff\xd8')
            jpgend = bts.find(b'\xff\xd9')
            if jpghead > -1 and jpgend > -1:
                jpg = bts[jpghead:jpgend+2]
                bts = bts[jpgend+2:]
                buff = np.frombuffer(
                    jpg, dtype=np.uint8)
                if len(buff) > 0:
                    print("------ Frame ------")
                    img = cv.imdecode(buff, cv.IMREAD_UNCHANGED)
                    height, width, c = img.shape
                    print('height ' + str(height) + ', width ' + str(width))
                    img = cv.flip(img, 1)
                    norm_img = np.zeros((400, 296))
                    norm_img = cv.normalize(
                        img, norm_img, 0, 255, cv.NORM_MINMAX)
                    gray_img = cv.cvtColor(norm_img, cv.COLOR_BGR2GRAY)
                    detected_faces = face_cascade.detectMultiScale(
                        gray_img, 1.1, minNeighbors=5, minSize=(30, 30), flags=cv.CASCADE_SCALE_IMAGE)

                    # TODO: Filter by biggest width...
                    for (y, x, w, h) in detected_faces:
                        cv.rectangle(img, (y, x), (y+w, x+h), (250, 0, 0), 2)
                        cv.putText(img, 'FACE', (y, x), 1,
                                   2, (255, 255, 255), 3)
                        delta = width / 2 - (y + w / 2)
                        print('delta ' + str(delta) + ', y ' + str(y) + ', x ' + str(x) + ', h ' +
                              str(h) + ', w ' + str(w))
                    cv.imshow("", img)
                    cv.waitKey(1)
                    if len(detected_faces) == 1:
                        if abs(delta) > 20:
                            sig = "1"
                            if delta > 0:
                                sig = "-1"
                            await websocket.send_text(sig)
        except Exception as e:
            print("Error:" + str(e))
            bts = b''
            stream = urlopen("http://192.168.43.154:81/stream")
            continue


if __name__ == '__main__':
    uvicorn.run(api, host='0.0.0.0', port=8080)
